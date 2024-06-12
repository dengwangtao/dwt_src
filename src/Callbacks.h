#pragma once

#include <functional>

namespace dwt {

using TimeoutCallback = std::function<void(int id)>;

// 心跳计数达到上限的回调
using HeartbeatCountReachCallback = std::function<void(int id)>;


} // end namespace dwt