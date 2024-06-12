#pragma once

#include <vector>
#include <unordered_map>
#include <chrono>
#include <mutex>
#include <condition_variable>

#include "Callbacks.h"

namespace dwt {


struct HeartBeatCounterNode {
    int id;             // 节点id
    int heartbeatCount; // 心跳计数
    HeartbeatCountReachCallback cb; // 过期执行的回调

    bool operator<(const HeartBeatCounterNode& other) {                // 大根堆
        return this->heartbeatCount > other.heartbeatCount;
    }
};

/**
 * 心跳计数堆, 超过上限则执行回调
 */
class HeapHeartBeatCounter {

public:

    HeapHeartBeatCounter(int limit);
    ~HeapHeartBeatCounter();

    // 添加节点和上限回调
    void push(int id, const TimeoutCallback& cb);

    // 更新节点的心跳计数
    void adjust(int id, int count);

    // 节点的心跳计数+1
    void addOne(int id);

    // tick, 删除达到上限的节点
    void tick();
    
    // 移除堆顶
    void pop();

    // 清空堆
    void clear();


private:
    size_t shift_up(size_t idx);
    size_t shift_down(size_t idx);
    void swap_node(size_t a, size_t b);
    void delete_node(size_t idx);


    std::vector<HeartBeatCounterNode> m_heap;
    std::unordered_map<int, size_t> m_map;  // 根据id找到在heap中的下标

    const int LIMIT;
};




} // end namespace dwt