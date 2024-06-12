#pragma once

#include <functional>

#include "HeapHeartbeatCounter.h"
#include "HeapTimer.h"
#include "Callbacks.h"

namespace dwt {

class HeartbeatCounter {

public:
    HeartbeatCounter(int timeout, int max_heartbeat)
    : m_counter(max_heartbeat)
    , m_timeout(timeout) {

    }

    ~HeartbeatCounter() = default;

    void setHeartbeatReachCallback(const HeartbeatCountReachCallback& cb) {
        m_heartbeatCountReachCallback = cb;
    }

    void tick();

    void push(size_t id);

    void reset(size_t id);

private:

    void timeoutFunc(size_t id);
    void heartbeatReach(size_t id);

    HeartbeatCountReachCallback m_heartbeatCountReachCallback;

    dwt::HeapTimer m_timer;                     // 定时器
    dwt::HeapHeartBeatCounter m_counter;        // 心跳计数器
    int m_timeout;    //  毫秒
};


} // end namespace dwt