#pragma once

#include <vector>
#include <unordered_map>
#include <chrono>
#include <mutex>
#include <condition_variable>

#include "Callbacks.h"

namespace dwt {

using Clock = std::chrono::high_resolution_clock;
using TimePoint = Clock::time_point;   // 时间点
using MS = std::chrono::milliseconds;   //  毫秒

struct TimerNode {
    int id;             // 节点id
    TimePoint expires;  // 过期时间
    TimeoutCallback cb; // 过期执行的回调

    bool operator<(const TimerNode& other) {
        return this->expires < other.expires;
    }
};

/**
 * TODO: 需要考虑线程安全
 */
class Timer {

public:

    Timer();
    ~Timer();

    // 添加节点, 超时时间(毫秒)和超时回调
    void push(int id, int timeout, const TimeoutCallback& cb);

    // 更新节点的过期时间
    void adjust(int id, int timeout);

    // tick, 删除超时节点
    void tick();

    // 获取最近一个过期的事件距离now的时间
    int getNextTick();
    
    // 移除堆顶
    void pop();

    // 清空堆
    void clear();


private:
    size_t shift_up(size_t idx);
    size_t shift_down(size_t idx);
    void swap_node(size_t a, size_t b);
    void delete_node(size_t idx);


    std::vector<TimerNode> m_heap;
    std::unordered_map<int, size_t> m_map;  // 根据id找到在heap中的下标
};




} // end namespace dwt