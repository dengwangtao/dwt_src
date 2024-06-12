#include "HeapTimer.h"
#include "Logger.h"

namespace dwt {



HeapTimer::HeapTimer() {
    m_heap.reserve(64); // 保留64个位置
}


HeapTimer::~HeapTimer() {

}

void HeapTimer::push(size_t id, int timeout, const TimeoutCallback& cb) {
    // LOG_INFO("%s:%d HeapTimer::push(%d, %d, cb)", __FILE__, __LINE__, id, timeout);

    // 判断节点是否存在
    if(m_map.count(id) > 0) {

        // 已存在该 id, 直接更新
        size_t idx = m_map[id];
        m_heap[idx].expires = Clock::now() + MS(timeout);
        m_heap[idx].cb = cb;

        shift_down(shift_up(idx));  // 先往上走, 再往下 down

    } else {

        // 新增节点
        size_t new_idx = m_heap.size(); // bug: size_t new_idx = m_map[id];
        m_heap.push_back({id, Clock::now() + MS(timeout), cb});
        m_map[id] = new_idx;
        shift_down(shift_up(new_idx));
    }

}

void HeapTimer::adjust(size_t id, int timeout) {
    if(m_map.count(id) == 0) {
        return;
    }
    size_t idx = m_map[id];
    m_heap[idx].expires = Clock::now() + MS(timeout);
    shift_down(shift_up(idx));  // 先往上走, 再往下 down
}

void HeapTimer::tick() {
    std::vector<std::function<void()>> callbacks;

    while(!m_heap.empty()) {
        TimerNode& top = m_heap.front();
        if(std::chrono::duration_cast<MS>(top.expires - Clock::now()).count() > 0) {
            // 堆顶未过期
            break;
        }
        // LOG_INFO("id = %d expired", top.id);
        // callbacks.push_back(std::bind(top.cb, top.id));
        callbacks.push_back([cb = top.cb, id = top.id]() { cb(id); });

        pop();
        // top.cb();
    }
    // 超时回调, 统一执行
    for(auto& cb : callbacks) {
        cb();
    }
}


int HeapTimer::getNextTick() {
    tick();
    int res = -1;   // 没有节点则返回-1
    if(!m_heap.empty()) {
        res = std::chrono::duration_cast<MS>(m_heap.front().expires - Clock::now()).count();
        if(res < 0) res = 0;
    }
    return res;
}


void HeapTimer::pop() {
    if(m_heap.size() > 0) {
        delete_node(0);
    }
}

void HeapTimer::clear() {
    m_heap.clear();
    m_map.clear();
}

void HeapTimer::del(size_t id) {
    if(m_map.count(id) == 0) {
        return;
    }
    size_t idx = m_map[id];
    delete_node(idx);
}





void HeapTimer::swap_node(size_t a, size_t b) {

    std::swap(m_heap[a], m_heap[b]);
    m_map[m_heap[a].id] = a;
    m_map[m_heap[b].id] = b;

}

size_t HeapTimer::shift_up(size_t idx) {

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


size_t HeapTimer::shift_down(size_t idx) {
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


void HeapTimer::delete_node(size_t idx) {

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
    // shift_down(shift_up(idx));      // remove the line
}



} // end namespace dwt