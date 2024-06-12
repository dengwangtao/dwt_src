#include "SRCServer.h"

#include <cstring>

namespace dwt {

static int createUDPSocket(int timeout) {
    int fd = ::socket(AF_INET, SOCK_DGRAM, 0);    // 创建UDP监听套接字
    if(fd == -1) {
        LOG_FATAL("%s:%d createUDPSocket(%d) error", __FILE__, __LINE__, timeout);
    }

    // 设置超时时间
    struct timeval timeout_;
    timeout_.tv_sec = timeout;      //秒
    timeout_.tv_usec = 0;           //微秒
    if(setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout_, sizeof(timeout_)) == -1) {
        LOG_FATAL("%s:%d createUDPSocket(%d) error", __FILE__, __LINE__, timeout);
    }

    return fd;
}



SRCServer::SRCServer(
    EventLoop* loop, const InetAddress& listenAddr,
    const std::string& name, int timeout, int max_heartbeat)
    : m_loop(loop)
    , m_server(loop, listenAddr, name)
    , m_UDPThread(std::bind(&SRCServer::HeartbeatHandler, this), name + "_UDP")
    , m_heartbeatCounter(timeout, max_heartbeat) {


    // 构造函数
    m_server.setConnectionCallback(
        std::bind(&SRCServer::onConnection, this, std::placeholders::_1)
    );
    
    m_server.setMessageCallback(
        std::bind(&SRCServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
    );

    m_server.setThreadNum(3);


    // ==

    m_UDPListenFd = createUDPSocket(3);    // 创建并设置UDP套接字超时时间为3秒
    // 绑定ip和端口, 和tcp同样的端口    
    if(::bind(m_UDPListenFd, (struct sockaddr*)(listenAddr.getSockAddr()), sizeof(struct sockaddr_in)) == -1) {
        LOG_FATAL("::bind UDP error");
    }

    // 设置心跳上限回调
    m_heartbeatCounter.setHeartbeatReachCallback(
        std::bind(&SRCServer::onHeartbeatReachLimit, this, std::placeholders::_1)
    );
    
}

SRCServer::~SRCServer() {

}


void SRCServer::start() {
    m_server.start();
    m_UDPThread.start();    // 启动心跳监测线程
}

void SRCServer::onConnection(const TcpConnectionPtr& conn) {
    if(!conn->connected()) {
        conn->shutdown();
    }
}

void SRCServer::onMessage(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp time) {
    std::string buf = buffer->retrieveAllAsString();
    conn->send(buf);
}

// 单独线程进行心跳检测
void SRCServer::HeartbeatHandler() {
    char buf[256] = {0};
    struct sockaddr_in peer_addr;

    while(1) {
        m_heartbeatCounter.tick();  // 清理超时节点, 并执行回调
        ::memset(&peer_addr, 0, sizeof peer_addr);
        socklen_t len = sizeof(peer_addr);
        int n = ::recvfrom(m_UDPListenFd, buf, 256, 0, (sockaddr*)&peer_addr, &len);
        buf[n] = '\0';

        InetAddress addr(peer_addr);
        if(n <= 0) {
            // ::recvfrom 超时
            // LOG_INFO("======== ::recvfrom timeout ========");

            // TODO: 可以执行其他函数 ...
            continue;
        }

        LOG_INFO("reveive data from %s", addr.toIpPort().c_str());

        // 根据[ip:port]计算session id
        size_t sessionId = std::hash<std::string>()(addr.toIpPort());
        if(m_sessions.count(sessionId) <= 0) {
            // 第一次收到该id的心跳包, 设置超时时间, 设置心跳上限回调 
            m_sessions.insert(sessionId);

            m_heartbeatCounter.push(sessionId);
        } else {
            // 心跳消息到达
            m_heartbeatCounter.reset(sessionId);
        }


    }
}


/**
 * TODO:删除会话, 删除会话节点
 *      runinLoop ...
 */
void SRCServer::onHeartbeatReachLimit(size_t id) {
    LOG_INFO("session %lu died", id);
    m_sessions.erase(id);
}


} // end namespace dwt