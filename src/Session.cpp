#include "Session.h"

namespace dwt{




Session::Session(size_t sessiodId, InetAddress TCP)
    : m_TCP(TCP)
    , m_sessionId(sessiodId) {
    
    // ctor
    
}

} // end namespace dwt