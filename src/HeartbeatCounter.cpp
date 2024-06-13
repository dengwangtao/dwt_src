#include "HeartbeatCounter.h"

#include "tcp/Logger.h"
#include "tcp/InetAddress.h"
#include "SessionManager.h"
#include "messages.pb.h"

#include <netinet/in.h>
#include <cstring>

namespace dwt {

static int createUDPSocket(int timeout) {
    int fd = ::socket(AF_INET, SOCK_DGRAM, 0);    // 创建UDP监听套接字
    if(fd == -1) {
        LOG_FATAL("%s:%d createUDPSocket(%d) error", __FILE__, __LINE__, timeout);
    }

    // 设置超时时间
    struct timeval timeout_;
    timeout_.tv_sec = timeout;      // 秒
    timeout_.tv_usec = 0;           // 微秒
    if(setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout_, sizeof(timeout_)) == -1) {
        LOG_FATAL("%s:%d createUDPSocket(%d) error", __FILE__, __LINE__, timeout);
    }

    return fd;
}



HeartbeatCounter::HeartbeatCounter(InetAddress udpAddr, int timeout, int max_heartbeat)
    : m_counter(max_heartbeat)
    , m_timeout(timeout)
    , m_UDPThread(std::bind(&HeartbeatCounter::ThreadFunc, this), "HeartbeatCounter_UDP") {

    m_UDPListenFd = createUDPSocket(1);    // 创建并设置UDP套接字超时时间为1秒
    // 绑定ip和端口, 和tcp同样的端口    
    if(::bind(m_UDPListenFd, (struct sockaddr*)(udpAddr.getSockAddr()), sizeof(struct sockaddr_in)) == -1) {
        LOG_FATAL("::bind UDP error");
    }


}


void HeartbeatCounter::tick() {
    m_timer.tick();
    m_counter.tick();
}


void HeartbeatCounter::timeoutFunc(size_t id) {
    // 超时: 未收到心跳包, 开始新的计时
    m_timer.push(
        id, m_timeout,
        std::bind(&HeartbeatCounter::timeoutFunc, this, std::placeholders::_1)
    );

    // 将心跳计数+1
    m_counter.addOne(id);

    LOG_INFO("HeartbeatCounter::timeoutFunc id=%lu", id);
}

void HeartbeatCounter::heartbeatReach(size_t id) {
    // 删除计时器
    m_timer.del(id);
    
    // 指定心跳计数器上限的回调
    if(m_heartbeatCountReachCallback) {
        m_heartbeatCountReachCallback(id);
    }
}


void HeartbeatCounter::push(size_t id) {
    m_timer.push(id, m_timeout, std::bind(&HeartbeatCounter::timeoutFunc, this, std::placeholders::_1));

    // bug: 此处为 heartbeatReach而不是 timeoutFunc
    m_counter.push(id, std::bind(&HeartbeatCounter::heartbeatReach, this, std::placeholders::_1));
}


void HeartbeatCounter::reset(size_t id) {
    m_counter.adjust(id, 0);
    m_timer.adjust(id, m_timeout); // 重新计时
}


void HeartbeatCounter::ThreadFunc() {
    char buf[256] = {0};
    struct sockaddr_in peer_addr;

    while(1) {
        this->tick();  // 清理超时节点, 并执行回调
        ::memset(&peer_addr, 0, sizeof peer_addr);
        socklen_t len = sizeof(peer_addr);
        int n = ::recvfrom(m_UDPListenFd, buf, 256, 0, (sockaddr*)&peer_addr, &len);
        buf[n] = '\0';


        InetAddress addr(peer_addr);    // UDP ip和端口号
        if(n <= 0) {
            // ::recvfrom 超时
            // LOG_INFO("======== ::recvfrom timeout ========");

            // TODO: 可以执行其他函数 ...
            continue;
        }

        dwt_proto::HeartbeatMessage msg;
        msg.ParseFromArray(buf, n); // 反序列化

        LOG_INFO("heartbeat data from %s", addr.toIpPort().c_str());

        // 收到udp包, 执行回调
        if(m_onUDPMessageCallback) m_onUDPMessageCallback(msg.sessionid());
    }
}



} // end namespace dwt