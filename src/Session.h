#pragma once

#include <string>
#include <unordered_map>
#include "tcp/InetAddress.h"

namespace dwt {


class Session {

public:
    explicit Session(size_t sessiodId, InetAddress TCP);
    ~Session() = default;


    size_t sessionId() const {
        return m_sessionId;
    }

    friend class SessionManager;

private:

    size_t m_sessionId;
    InetAddress m_TCP;
};




} // end namespace dwt

