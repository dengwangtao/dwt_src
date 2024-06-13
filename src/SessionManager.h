#pragma once


#include <string>
#include <unordered_map>
#include <mutex>
#include <memory>

#include "Session.h"

namespace dwt {


class SessionManager {

public:

    // 在SessionManager建立映射关系, 并返回sessionId
    size_t add(size_t sessionId, InetAddress TCP) {

        std::lock_guard<std::mutex> lock(m_mutex);
        m_TCP_Id[TCP.toIpPort()] = sessionId;
        m_sessions[sessionId] = std::make_unique<Session>(sessionId, TCP);
        return sessionId;
    }


    bool exist(size_t sessionId) const {
        std::lock_guard<std::mutex> lock(m_mutex);

        return m_sessions.count(sessionId);
    }

    void remove(size_t sessionId) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if(m_sessions.count(sessionId)) {
            m_TCP_Id.erase(m_sessions[sessionId]->m_TCP.toIpPort());
            m_sessions.erase(sessionId);
        }
    }



private:

    std::unordered_map<std::string, std::string> m_TCP_Id; // {TCP -> sessionId}
    std::unordered_map<size_t, std::unique_ptr<Session>> m_sessions;     // {sessionId -> Session}

    mutable std::mutex m_mutex;
};


} // end namespace dwt