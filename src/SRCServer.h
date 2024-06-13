#pragma once

#include "tcp/TcpServer.h"

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "HeartbeatCounter.h"
#include "SessionManager.h"


namespace dwt {

// enum class NodeType {
//     PERSISTENT,         // 永久节点
//     EPHEMERAL,          // 临时节点 (会话节点, 会话关闭后, 节点失效)
// };


// 节点类型
struct Node {
    int id;
    std::string name;
};

class SRCServer {

public:
    SRCServer(
        EventLoop* loop, const InetAddress& listenAddr,
        const std::string& name,
        int timeout = 10000, int max_heartbeat = 5);

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



    std::unordered_map<std::string, Node>   m_data;   // name    -> Node
    std::unordered_map<int, std::string>    m_dict;   // Node.id -> name


    SessionManager m_sessionManager;
    // UDP
    HeartbeatCounter m_heartbeatCounter;
};




} // end namespace dwt

