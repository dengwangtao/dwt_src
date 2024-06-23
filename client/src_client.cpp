#include "src_client.h"
#include "tcp/Logger.h"

namespace dwt {

static const int BUF_SIZE = 65536;

SRCCLient::SRCCLient()
    : m_isConnected(false) {

    // ctor
}

SRCCLient& SRCCLient::getInstance() {
    static SRCCLient instance;
    return instance;
}

bool SRCCLient::connect(const std::string& ip, uint16_t port, uint32_t seconds) {

    if(m_isConnected) {
        LOG_WARN("already connect to %s:%d", m_serverIp.c_str(), m_serverPort);
        return false;
    }

    m_serverIp = ip;
    m_serverPort = port;


    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr;
    ::memset(&serverAddr, 0, sizeof serverAddr);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = ::htons(m_serverPort);
    serverAddr.sin_addr.s_addr = ::inet_addr(m_serverIp.c_str());

    if(::connect(fd, (sockaddr*)&serverAddr, sizeof serverAddr) == -1) {
        ::perror("connect");
        LOG_FATAL("connect error");
    }

    dwt_proto::WrappedRequest connReq;
    connReq.set_requesttype(dwt_proto::MessageType::Connection);
    std::string tobeSend = connReq.SerializeAsString();
    int n = ::send(fd, &tobeSend[0], tobeSend.size(), 0);
    if(n < 0) {
        ::perror("write");
        LOG_FATAL("%s:%d send error", __FILE__, __LINE__);
    }

    char buf[1024] = {0};
    // ::memset(buf, 0, sizeof buf);

    n = ::read(fd, buf, sizeof buf);
    if(n < 0) {
        ::perror("read");
        LOG_FATAL("%s:%d reveive error", __FILE__, __LINE__);
    }

    
    dwt_proto::WrappedResponse res;
    res.ParseFromArray(buf, n);


    if(res.responsetype() == dwt_proto::MessageType::Connection) {

        dwt_proto::ConnectionResponse resp;

        if(resp.ParseFromString(res.data())) {
            m_sessionId = resp.sessionid();
            LOG_INFO("connect success, sessionId = %lu", m_sessionId);
        } else {
            LOG_FATAL("%s:%d sessionId parse error", __FILE__, __LINE__);
        }
        

    } else if(res.responsetype() == dwt_proto::MessageType::Error) {
        LOG_FATAL("%s:%d Error Response: %s", __FILE__, __LINE__, res.data().c_str());
    }

    close(fd);


    // 创建线程, 发送心跳
    m_heartbeatThread = std::make_unique<std::thread>([=]() { // & -> =
        // udp fd
        int udp_fd = ::socket(AF_INET, SOCK_DGRAM, 0);

        dwt_proto::HeartbeatMessage heartbeat;
        heartbeat.set_sessionid(m_sessionId);
        std::string sendstr = heartbeat.SerializeAsString();

        while(m_sessionId != 0) {
            ::sendto(udp_fd, &sendstr[0],sendstr.size(), 0, (sockaddr*)&serverAddr, sizeof serverAddr); // 发送sessionid
            std::this_thread::sleep_for(std::chrono::seconds(seconds));
        }
    });

    m_heartbeatThread->detach();    // 线程分离

    m_isConnected = true;

    return true;
}

void SRCCLient::disconnect() {
    if(!m_isConnected) {
        return;
    }
    m_serverIp = "";
    m_serverPort = 0;
    m_isConnected = false;

    m_sessionId = 0;
}


std::string SRCCLient::wrappedServiceString(const std::string& requestStr, dwt_proto::ServiceType type) {
    
    dwt_proto::ServiceRequest sreq;
    sreq.set_servicetype(type);
    sreq.set_sessionid(m_sessionId);
    sreq.set_serviceparameters(requestStr);

    dwt_proto::WrappedRequest wreq;
    wreq.set_requesttype(dwt_proto::MessageType::Service);
    wreq.set_requestparameters(sreq.SerializeAsString());
    return wreq.SerializeAsString();
}


// 创建fd, 并发送, 返回文件描述符
int SRCCLient::sendData(const std::string& str) {
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr;
    ::memset(&serverAddr, 0, sizeof serverAddr);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = ::htons(m_serverPort);
    serverAddr.sin_addr.s_addr = ::inet_addr(m_serverIp.c_str());

    if(::connect(fd, (sockaddr*)&serverAddr, sizeof serverAddr) == -1) {
        ::perror("connect");
        exit(1);
    }

    int n = ::send(fd, &str[0], str.size(), 0);
    if(n < 0) {
        ::perror("::send");
        LOG_FATAL("%s:%d send error", __FILE__, __LINE__);
    }

    return fd;
}

// 接收数据
std::string SRCCLient::receiveData(int fd) {

    // 接收数据
    char buf[BUF_SIZE] = {0};       // receive
    int n = ::read(fd, buf, sizeof buf);
    if(n < 0) {
        ::perror("read");
        LOG_FATAL("%s:%d receive error", __FILE__, __LINE__);
    }
    dwt_proto::WrappedResponse wres;
    wres.ParseFromArray(buf, n);


    if(wres.responsetype() == dwt_proto::MessageType::Error) {
        LOG_FATAL("%s:%d Error Response: %s", __FILE__, __LINE__, wres.data().c_str());
    }

    if(wres.responsetype() == dwt_proto::MessageType::Service) {
        dwt_proto::ServiceResponse sres;
        if(!sres.ParseFromString(wres.data())) {
            LOG_FATAL("%s:%d ServiceResponse parse error", __FILE__, __LINE__);
        }
        return sres.data();
    }
    return "";
}






/**
 * 创建节点
 * @param data: 节点数据
 * @param type: 1永久节点, 2会话节点
 */
bool SRCCLient::createNode(const std::string& path, const std::string& data, int type) {

    dwt_proto::CreateNodeRequest req;
    req.set_path(path);
    req.set_nodedata(data);
    req.set_nodetype(static_cast<dwt_proto::NodeType>(type));

    std::string tobeSend = wrappedServiceString(req.SerializeAsString(), dwt_proto::ServiceType::Create);

    int fd = sendData(tobeSend);
    

    bool result = false;

    std::string receiveStr = receiveData(fd);

    dwt_proto::CreateNodeResponse resp;
    if(!resp.ParseFromString(receiveStr)) {
        LOG_FATAL("%s:%d CreateNodeResponse parse error", __FILE__, __LINE__);
    }

    if(resp.success()) {
        result = true;
    } else {
        LOG_WARN("创建节点失败: %s", resp.errmsg().c_str());
    }


    close(fd);
    return result;
}

/**
 * 获取节点数据
 */
std::string SRCCLient::getNode(const std::string& path) {
    dwt_proto::GetNodeRequest req;
    req.set_path(path);

    std::string tobeSend = wrappedServiceString(req.SerializeAsString(), dwt_proto::ServiceType::Get);

    int fd = sendData(tobeSend);
    

    std::string result ;

    std::string receiveStr = receiveData(fd);

    dwt_proto::GetNodeResponse resp;
    if(!resp.ParseFromString(receiveStr)) {
        LOG_FATAL("%s:%d GetNodeResponse parse error", __FILE__, __LINE__);
    }

    if(resp.success()) {
        result = resp.nodedata();
    } else {
        LOG_WARN("获取节点失败: %s", resp.errmsg().c_str());
    }


    close(fd);
    return result;
}

/**
 * 设置节点数据
 * @param data: 节点数据
 */
bool SRCCLient::setNode(const std::string& path, const std::string& data) {
    dwt_proto::SetNodeRequest req;
    req.set_path(path);
    req.set_nodedata(data);

    std::string tobeSend = wrappedServiceString(req.SerializeAsString(), dwt_proto::ServiceType::Set);

    int fd = sendData(tobeSend);
    

    bool result = false;

    std::string receiveStr = receiveData(fd);

    dwt_proto::SetNodeResponse resp;
    if(!resp.ParseFromString(receiveStr)) {
        LOG_FATAL("%s:%d SetNodeResponse parse error", __FILE__, __LINE__);
    }

    if(resp.success()) {
        result = true;
    } else {
        LOG_WARN("设置节点失败: %s", resp.errmsg().c_str());
    }


    close(fd);
    return result;
}

/**
 * 删除节点
 */
bool SRCCLient::deleteNode(const std::string& path) {
    dwt_proto::DeleteNodeRequest req;
    req.set_path(path);

    std::string tobeSend = wrappedServiceString(req.SerializeAsString(), dwt_proto::ServiceType::Delete);

    int fd = sendData(tobeSend);
    

    bool result = false;

    std::string receiveStr = receiveData(fd);

    dwt_proto::DeleteNodeResponse resp;
    if(!resp.ParseFromString(receiveStr)) {
        LOG_FATAL("%s:%d DeleteNodeResponse parse error", __FILE__, __LINE__);
    }

    if(resp.success()) {
        result = true;
    } else {
        LOG_WARN("删除节点失败: %s", resp.errmsg().c_str());
    }


    close(fd);
    return result;
}

/**
 * list节点
 */
std::vector<std::string> SRCCLient::lsNode(const std::string& path)  {
    dwt_proto::LsNodeRequest req;
    req.set_path(path);

    std::string tobeSend = wrappedServiceString(req.SerializeAsString(), dwt_proto::ServiceType::Ls);

    int fd = sendData(tobeSend);
    

    std::vector<std::string> result;

    std::string receiveStr = receiveData(fd);

    dwt_proto::LsNodeResponse resp;
    if(!resp.ParseFromString(receiveStr)) {
        LOG_FATAL("%s:%d LsNodeResponse parse error", __FILE__, __LINE__);
    }

    if(resp.success()) {
        for(int i = 0; i < resp.children_size(); ++ i) {
            result.push_back(resp.children(i));
        }
    } else {
        LOG_WARN("列举节点失败: %s", resp.errmsg().c_str());
    }


    close(fd);
    return result;
}

/**
 * 获取节点状态
 */
dwt_proto::NodeState SRCCLient::statNode(const std::string& path) {
    dwt_proto::StatNodeRequest req;
    req.set_path(path);

    std::string tobeSend = wrappedServiceString(req.SerializeAsString(), dwt_proto::ServiceType::Stat);

    int fd = sendData(tobeSend);
    

    dwt_proto::NodeState result;

    std::string receiveStr = receiveData(fd);

    dwt_proto::StatNodeResponse resp;
    if(!resp.ParseFromString(receiveStr)) {
        LOG_FATAL("%s:%d StatNodeResponse parse error", __FILE__, __LINE__);
    }

    if(resp.success()) {
        result = resp.state();
    } else {
        LOG_WARN("获取节点状态失败: %s", resp.errmsg().c_str());
    }


    close(fd);
    return result;
}

/**
 * 判断节点是否存在
 */
bool SRCCLient::existsNode(const std::string& path) {
    dwt_proto::ExistsNodeRequest req;
    req.set_path(path);

    std::string tobeSend = wrappedServiceString(req.SerializeAsString(), dwt_proto::ServiceType::Exists);

    int fd = sendData(tobeSend);
    

    bool result;

    std::string receiveStr = receiveData(fd);

    dwt_proto::ExistsNodeResponse resp;
    if(!resp.ParseFromString(receiveStr)) {
        LOG_FATAL("%s:%d ExistsNodeResponse parse error", __FILE__, __LINE__);
    }

    if(resp.success()) {
        result = resp.exist();
    } else {
        LOG_WARN("获取节点存在失败: %s", resp.errmsg().c_str());
    }


    close(fd);
    return result;
}





} // end namespace dwt