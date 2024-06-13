#pragma once

#include <functional>
#include "tcp/InetAddress.h"

namespace dwt {

using TimeoutCallback = std::function<void(size_t id)>;

// 心跳计数达到上限的回调
using HeartbeatCountReachCallback = std::function<void(size_t id)>;


using OnUDPMessageCallback = std::function<void(size_t)>;

} // end namespace dwt