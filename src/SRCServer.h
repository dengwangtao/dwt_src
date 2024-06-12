#pragma once

#include "tcp/TcpServer.h"

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "HeartbeatCounter.h"


namespace dwt {

enum class NodeType {
    PERSISTENT,         // 永久节点
    EPHEMERAL,          // 临时节点 (会话节点, 会话关闭后, 节点失效)
};


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

    void HeartbeatHandler();    // 单线程接收心跳包

    void onHeartbeatReachLimit(size_t id);


    TcpServer m_server;   // Tcp Server, 主从Reactor, one loop per thread
    EventLoop* m_loop;    // TCP 事件循环 (mainLoop)

    Thread m_UDPThread;   // UDP线程, 用于接收心跳包


    std::unordered_map<std::string, Node>   m_data;   // name    -> Node
    std::unordered_map<int, std::string>    m_dict;   // Node.id -> name


    // UDP
    std::unordered_set<size_t> m_sessions;      // 存储会话id, 根据对端的ip和端口, 进行哈希计算得到session 
    HeartbeatCounter m_heartbeatCounter;
    int m_UDPListenFd;                          // udp socket套接字
};




} // end namespace dwt

