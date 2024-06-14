#include <iostream>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

#include "messages.pb.h"

int main() {

    int fd = ::socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr;
    ::memset(&serverAddr, 0, sizeof serverAddr);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = ::htons(8888);
    serverAddr.sin_addr.s_addr = ::inet_addr("127.0.0.1");

    if(::connect(fd, (sockaddr*)&serverAddr, sizeof serverAddr) == -1) {
        ::perror("connect");
        exit(1);
    }


    dwt_proto::WrappedRequest connReq;
    char buf[512] = {0};
    connReq.set_requesttype(dwt_proto::MessageType::Connection);
    std::string tobeSend = connReq.SerializeAsString();
    int n = ::send(fd, &tobeSend[0], tobeSend.size(), 0);
    // int n = ::send(fd, "hahhaha", 8, 0);
    if(n < 0) {
        ::perror("write");
        exit(1);
    }

    std::cout << "start read" << std::endl;

    // receive
    memset(buf, 0, sizeof buf);

    n = ::read(fd, buf, sizeof buf);
    if(n < 0) {
        ::perror("read");
        exit(1);
    }
    
    dwt_proto::WrappedResponse res;
    res.ParseFromArray(buf, n);

    size_t sessionId = 0;

    if(res.responsetype() == dwt_proto::MessageType::Connection) {
        dwt_proto::ConnectionResponse resp;
        resp.ParseFromString(res.data());
        sessionId = resp.sessionid();
        std::cout << "sessionId=" << resp.sessionid() << std::endl;
    } else if(res.responsetype() == dwt_proto::MessageType::Error) {
        std::cout << "Error Response" << std::endl;
    }

    close(fd);

    // udp fd
    int udp_fd = ::socket(AF_INET, SOCK_DGRAM, 0);

    dwt_proto::HeartbeatMessage heartbeat;
    heartbeat.set_sessionid(sessionId);
    std::string sendstr = heartbeat.SerializeAsString();

    if(sessionId != 0) {        
        for(int i = 0; i < 5; ++ i) {
            ::sendto(udp_fd, &sendstr[0],sendstr.size(), 0, (sockaddr*)&serverAddr, sizeof serverAddr); // 发送sessionid

            std::this_thread::sleep_for(std::chrono::seconds(8));
        }
        
    }
    
    

    return 0;
}