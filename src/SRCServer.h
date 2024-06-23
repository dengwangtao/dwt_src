#pragma once

#include "tcp/TcpServer.h"

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "HeartbeatCounter.h"
#include "SessionManager.h"


namespace dwt {

class SRCServer {

public:
    SRCServer(
        EventLoop* loop, const InetAddress& listenAddr,
        const std::string& name,
        int timeout = 12000, int max_heartbeat = 5);

    ~SRCServer();

    // 开启服务
    void start();

private:

    void onConnection(const TcpConnectionPtr& conn);

    void onMessage(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp time);

    void onHeartbeatReachLimit(size_t id);

    void onUDPMessage(size_t sessionId);


    TcpServer m_server;   // Tcp Server, 主从Reactor, one loop per thread
    EventLoop* m_loop;    // TCP 事件循环 (mainLoop)



    SessionManager m_sessionManager;
    // UDP
    HeartbeatCounter m_heartbeatCounter;

};




} // end namespace dwt

