#include "HeapHeartbeatCounter.h"
#include "HeapTimer.h"
#include "Logger.h"

#include <iostream>
#include <string>

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unordered_map>
#include <functional>
#include <unistd.h>
#include <fcntl.h>


void test_HeapHeartbeatCounter() {
    dwt::HeapHeartBeatCounter hbc(5);

    for(int i = 1; i <= 5; ++ i) {
        hbc.push(i, [](int id) {
            std::cout << "节点 " << id << "达到LIMIT, 关闭会话" << std::endl;
        });
    }

    std::string s;
    int id;

    while(std::cin >> id) {
        hbc.tick();
        if(id == -1) return;
        hbc.addOne(id);
        hbc.tick();
    }

}


void test_HeapHeartbeatCounter_HeapTimer() {
    dwt::HeapHeartBeatCounter heartBeat(5); // 心跳上限为 5
    dwt::HeapTimer timer;                   // 定时器

    int listenfd = ::socket(AF_INET, SOCK_DGRAM, 0);    // 创建UDP监听套接字

    // 设置为非阻塞
    // int opts = ::fcntl(listenfd, F_GETFL);
    // ::fcntl(listenfd, F_SETFL, opts | O_NONBLOCK);

    // 设置超时时间
    struct timeval timeout;
    timeout.tv_sec = 1;     //秒
    timeout.tv_usec = 0;    //微秒
    setsockopt(listenfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);       //设置接收方端口号
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    ::bind(listenfd, (struct sockaddr*)&addr, sizeof addr);


    std::unordered_map<int, int> sessions;  // {id, _}


    // =============== 回调函数
    // 不要放在局部, bug
    std::function<void(int)> timeoutFunc = [&](int id) {
        LOG_INFO("*** id = %d timeout ***", id);
        // 如果超时未收到心跳包, 开始新的计时
        timer.push(id, 5000, timeoutFunc);
        // 将心跳计数+1
        heartBeat.addOne(id);
    };

    std::function<void(int)> heartbeatReach = [&](int id) {
        LOG_INFO("$$$ id = %d reach heartbeat limit $$$", id);
        // 删除计时器
        timer.del(id);
        // 删除会话
        sessions.erase(id);
    };

    char buf[256] = {0};
    struct sockaddr_in peer_addr;
    while(1) {
        timer.tick();
        heartBeat.tick();

        ::memset(&peer_addr, 0, sizeof peer_addr);
        socklen_t len = sizeof(peer_addr);
        int n = ::recvfrom(listenfd, buf, 256, 0, (sockaddr*)&peer_addr, &len);
        buf[n] = '\0';
        if(n <= 0) {
            // LOG_INFO("======== ::recvfrom timeout ========");
            continue;
        }

        int sessionId = ::atoi(buf);   // 强转为id
        LOG_INFO(" >>> receive: len=%d, id=%d", n, sessionId);

        // 第一次收到该id的心跳包, 设置超时时间, 设置心跳上限回调
        if(sessions.count(sessionId) <= 0) {
            sessions[sessionId] = 1;
            
            timer.push(sessionId, 5000, timeoutFunc);

            heartBeat.push(sessionId, heartbeatReach);

        } else {
            // 当心跳消息到达时, 将心跳计数置为0
            LOG_INFO("&&& sessionId = %d again &&&", sessionId);
            heartBeat.adjust(sessionId, 0);
            timer.adjust(sessionId, 5000); // 重新计时
        }
    }

}