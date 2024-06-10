#include "Timer.h"
#include "Logger.h"

namespace dwt {



Timer::Timer() {
    m_heap.reserve(64); // 保留64个位置
}


Timer::~Timer() {

}

void Timer::push(int id, int timeout, const TimeoutCallback& cb) {

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

void Timer::adjust(int id, int timeout) {
    size_t idx = m_map[id];
    m_heap[idx].expires = Clock::now() + MS(timeout);
    shift_down(shift_up(idx));  // 先往上走, 再往下 down
}

void Timer::tick() {
    while(!m_heap.empty()) {
        TimerNode& top = m_heap.front();
        if(std::chrono::duration_cast<MS>(top.expires - Clock::now()).count() > 0) {
            // 堆顶未过期
            break;
        }
        // LOG_INFO("id = %d expired", top.id);
        top.cb();
        pop();
    }
}


int Timer::getNextTick() {
    tick();
    int res = -1;   // 没有节点则返回-1
    if(!m_heap.empty()) {
        res = std::chrono::duration_cast<MS>(m_heap.front().expires - Clock::now()).count();
        if(res < 0) res = 0;
    }
    return res;
}


void Timer::pop() {
    if(m_heap.size() > 0) {
        delete_node(0);
    }
}

void Timer::clear() {
    m_heap.clear();
    m_map.clear();
}




void Timer::swap_node(size_t a, size_t b) {

    std::swap(m_heap[a], m_heap[b]);
    m_map[m_heap[a].id] = a;
    m_map[m_heap[b].id] = b;

}

size_t Timer::shift_up(size_t idx) {

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


size_t Timer::shift_down(size_t idx) {
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


void Timer::delete_node(size_t idx) {

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
    shift_down(shift_up(idx));      // bug: 忘记该行
}



} // end namespace dwt