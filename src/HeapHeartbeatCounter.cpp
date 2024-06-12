#include "HeapHeartbeatCounter.h"
#include "Logger.h"

namespace dwt {



HeapHeartBeatCounter::HeapHeartBeatCounter(int limit): LIMIT(limit) {
    m_heap.reserve(64); // 保留64个位置
}


HeapHeartBeatCounter::~HeapHeartBeatCounter() {

}

void HeapHeartBeatCounter::push(size_t id, const HeartbeatCountReachCallback& cb) {

    // 判断节点是否存在
    if(m_map.count(id) > 0) {

        // 已存在该 id, 直接更新
        size_t idx = m_map[id];
        m_heap[idx].heartbeatCount = 0; // 设置为0
        m_heap[idx].cb = cb;

        shift_down(shift_up(idx));  // 先往上走, 再往下 down

    } else {

        // 新增节点
        size_t new_idx = m_heap.size(); // bug: size_t new_idx = m_map[id];
        m_heap.push_back({id, 0, cb});
        m_map[id] = new_idx;
        shift_down(shift_up(new_idx));
    }

}

void HeapHeartBeatCounter::adjust(size_t id, int count) {
    // id不存在
    if(m_map.count(id) == 0) {
        return;
    }
    size_t idx = m_map[id];
    m_heap[idx].heartbeatCount = count;
    shift_down(shift_up(idx));  // 先往上走, 再往下 down
}

void HeapHeartBeatCounter::addOne(size_t id) {
    // id不存在
    if(m_map.count(id) == 0) {
        return;
    }
    size_t idx = m_map[id];
    m_heap[idx].heartbeatCount ++;
    shift_down(shift_up(idx));  // 先往上走, 再往下 down
}

void HeapHeartBeatCounter::tick() {
    while(!m_heap.empty()) {
        HeartBeatCounterNode& top = m_heap.front();
        if(top.heartbeatCount < LIMIT) {
            // 堆顶未过期
            break;
        }
        // LOG_INFO("id = %d expired", top.id);
        top.cb(top.id);
        pop();
    }
}



void HeapHeartBeatCounter::pop() {
    if(m_heap.size() > 0) {
        delete_node(0);
    }
}

void HeapHeartBeatCounter::clear() {
    m_heap.clear();
    m_map.clear();
}




void HeapHeartBeatCounter::swap_node(size_t a, size_t b) {

    std::swap(m_heap[a], m_heap[b]);
    m_map[m_heap[a].id] = a;
    m_map[m_heap[b].id] = b;

}

size_t HeapHeartBeatCounter::shift_up(size_t idx) {

    while(idx > 0) {
        size_t fa = (idx - 1) / 2;
        if(m_heap[idx] < m_heap[fa]) {
            swap_node(idx, fa);
            idx = fa;
        } else {
            return idx;
        }
    }
    return idx;
}


size_t HeapHeartBeatCounter::shift_down(size_t idx) {
    size_t n = m_heap.size();
    while(idx < n) {
        size_t lc = idx * 2 + 1;
        size_t rc = idx * 2 + 2;
        size_t cur = idx;

        if(lc < n && m_heap[lc] < m_heap[cur]) cur = lc;
        if(rc < n && m_heap[rc] < m_heap[cur]) cur = rc;

        if(cur == idx) {
            return idx;
        }

        swap_node(cur, idx);
        idx = cur;
    }
    return idx;
}


void HeapHeartBeatCounter::delete_node(size_t idx) {

    size_t end_idx = m_heap.size() - 1;
    if(idx < end_idx) {
        swap_node(idx, end_idx);
        m_map.erase(m_heap[end_idx].id);
        m_heap.pop_back();

        shift_down(shift_up(idx));  // bug: 忘记该行
        return;
    }
    
    m_map.erase(m_heap[idx].id);
    m_heap.pop_back();
    // shift_down(shift_up(idx));      // delete the line
}



} // end namespace dwt