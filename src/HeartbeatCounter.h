#pragma once

#include <functional>

#include "HeapHeartbeatCounter.h"
#include "HeapTimer.h"
#include "Callbacks.h"
#include "tcp/Thread.h"

namespace dwt {

/**
 * 启动一个线程, 并创建一个UDP套接字, 接收处理心跳包
 */
class HeartbeatCounter {

public:
    HeartbeatCounter(InetAddress udpAddr, int timeout, int max_heartbeat);

    ~HeartbeatCounter() = default;

    void setHeartbeatReachCallback(const HeartbeatCountReachCallback& cb) {
        m_heartbeatCountReachCallback = cb;
    }

    void setOnUDPMessageCallback(const OnUDPMessageCallback& cb) {
        m_onUDPMessageCallback = cb;
    }

    void start() {
        m_UDPThread.start();
    }

    void tick();

    void push(size_t id);

    void reset(size_t id);

    

private:

    void ThreadFunc();

    void timeoutFunc(size_t id);
    void heartbeatReach(size_t id);

    HeartbeatCountReachCallback m_heartbeatCountReachCallback;

    dwt::HeapTimer m_timer;                     // 定时器
    dwt::HeapHeartBeatCounter m_counter;        // 心跳计数器
    int m_timeout;    //  毫秒 (每多少毫秒需要需要接受一个心跳包)

    Thread m_UDPThread;   // UDP线程, 用于接收心跳包
    int m_UDPListenFd;

    OnUDPMessageCallback m_onUDPMessageCallback;
};


} // end namespace dwt