#include "HeartbeatCounter.h"

#include "tcp/Logger.h"

namespace dwt {

void HeartbeatCounter::tick() {
    m_timer.tick();
    m_counter.tick();
}


void HeartbeatCounter::timeoutFunc(size_t id) {
    // 超时: 未收到心跳包, 开始新的计时
    m_timer.push(
        id, m_timeout,
        std::bind(&HeartbeatCounter::timeoutFunc, this, std::placeholders::_1)
    );

    // 将心跳计数+1
    m_counter.addOne(id);

    LOG_INFO("HeartbeatCounter::timeoutFunc id=%lu", id);
}

void HeartbeatCounter::heartbeatReach(size_t id) {
    // 删除计时器
    m_timer.del(id);
    
    // 指定心跳计数器上限的回调
    if(m_heartbeatCountReachCallback) {
        m_heartbeatCountReachCallback(id);
    }
}


void HeartbeatCounter::push(size_t id) {
    m_timer.push(id, m_timeout, std::bind(&HeartbeatCounter::timeoutFunc, this, std::placeholders::_1));

    // bug: 此处为 heartbeatReach而不是 timeoutFunc
    m_counter.push(id, std::bind(&HeartbeatCounter::heartbeatReach, this, std::placeholders::_1));
}


void HeartbeatCounter::reset(size_t id) {
    m_counter.adjust(id, 0);
    m_timer.adjust(id, m_timeout); // 重新计时
}


} // end namespace dwt