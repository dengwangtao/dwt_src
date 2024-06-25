#include "SRCServer.h"

#include <cstring>

#include "uuid4.h"
#include "messages.pb.h"
#include "Services.h"

namespace dwt {




SRCServer::SRCServer(
    EventLoop* loop, const InetAddress& listenAddr,
    const std::string& name, int timeout, int max_heartbeat)
    : m_loop(loop)
    , m_server(loop, listenAddr, name)
    , m_heartbeatCounter(listenAddr, timeout, max_heartbeat) {


    // 构造函数
    m_server.setConnectionCallback(
        std::bind(&SRCServer::onConnection, this, std::placeholders::_1)
    );
    
    m_server.setMessageCallback(
        std::bind(&SRCServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
    );

    // 设置业务线程数
    m_server.setThreadNum(3);


    // ==

    
    // 设置心跳上限回调
    m_heartbeatCounter.setHeartbeatReachCallback(
        std::bind(&SRCServer::onHeartbeatReachLimit, this, std::placeholders::_1)
    );

    // 设置UDP心跳消息到达处理回调
    m_heartbeatCounter.setOnUDPMessageCallback(
        std::bind(&SRCServer::onUDPMessage, this, std::placeholders::_1)
    );


    
}

SRCServer::~SRCServer() {

}


void SRCServer::start() {
    m_server.start();
    m_heartbeatCounter.start(); // 启动心跳检测线程
}

void SRCServer::onConnection(const TcpConnectionPtr& conn) {
    if(!conn->connected()) {
        conn->shutdown();
    }
}

// 该函数执行的线程为 ioLoop(s)
void SRCServer::onMessage(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp time) {
    std::string data = buffer->retrieveAllAsString();

    dwt_proto::WrappedRequest wrappedRequest;
    dwt_proto::WrappedResponse resp;    // 响应
    if(!wrappedRequest.ParseFromString(data)) {
        LOG_ERROR("wrappedRequest.ParseFromString(%s)", data.c_str());
        resp.set_responsetype(dwt_proto::MessageType::Error);
        resp.set_data("request parse error");
        conn->send(resp.SerializeAsString());
        return;
    }

    if(wrappedRequest.requesttype() == dwt_proto::MessageType::Connection) {
        // 连接请求
        size_t sessionId = UUID4::generateSessionId();

        LOG_INFO("generated sessionId = %lu to %s", sessionId, conn->peerAddress().toIpPort().c_str());

        // 创建会话映射
        m_sessionManager.add(sessionId, conn->peerAddress());
        // 放入心跳定时器
        m_heartbeatCounter.push(sessionId);

        // 连接响应
        dwt_proto::ConnectionResponse connRes;
        connRes.set_sessionid(sessionId);

        
        resp.set_responsetype(dwt_proto::MessageType::Connection);
        resp.set_data(connRes.SerializeAsString());

        // 发送响应
        conn->send(resp.SerializeAsString());


    } else if(wrappedRequest.requesttype() == dwt_proto::MessageType::Service) {

        // 业务请求
        // LOG_INFO("业务请求到达");
        // 解析业务参数
        dwt_proto::ServiceRequest serviceRequest;
        if(!serviceRequest.ParseFromString(wrappedRequest.requestparameters())) {
            LOG_ERROR("请求参数错误");
            resp.set_responsetype(dwt_proto::MessageType::Error);
            resp.set_data("service request parse error");
            conn->send(resp.SerializeAsString());
            return;
        }

        // 如果是 create delete set, 同步转发到其他节点 (2PC)
        

        std::string serviceResp = Services::getInstance().handle(
            serviceRequest.servicetype(),
            serviceRequest.serviceparameters(),
            serviceRequest.sessionid());
        
        dwt_proto::ServiceResponse serviceResponse;
        serviceResponse.set_servicetype(serviceRequest.servicetype());
        serviceResponse.set_data(serviceResp);

        resp.set_responsetype(dwt_proto::MessageType::Service);
        resp.set_data(serviceResponse.SerializeAsString());

        conn->send(resp.SerializeAsString());

    } else {
        LOG_ERROR("请求参数错误");
        resp.set_responsetype(dwt_proto::MessageType::Error);
        resp.set_data("illegal request parameters");
        conn->send(resp.SerializeAsString());
    }
}



void SRCServer::onUDPMessage(size_t sessionId) {
    if(m_sessionManager.exist(sessionId)) { // TCP已经建立了会话
        // 心跳消息到达
        m_heartbeatCounter.reset(sessionId);
    } else {
        // 没有会话, 不作处理
        LOG_WARN("receive not registerd Heartbeat, sessionId = %lu", sessionId);
    }
}



/**
 * 运行在udp thread
 */
void SRCServer::onHeartbeatReachLimit(size_t sessionId) {
    LOG_INFO("session %lu died", sessionId);
    m_sessionManager.remove(sessionId);

    // 删除会话节点
    Services::getInstance().removeEphemeral(sessionId);

    
}

} // end namespace dwt