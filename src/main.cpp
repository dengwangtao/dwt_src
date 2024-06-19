#include <iostream>

#include "tcp/TcpServer.h"
#include "SRCServer.h"


#include "Services.h"

void test() {
    auto s = new dwt::Services();
    // for(auto s : s->splitPath("/userService/method1/para1/")) {
    //     std::cout << "|" << s << "|" << std::endl;
    // }

    s->createNode(1, "/", "userService", "userService_data", dwt::NodeType::EPHEMERAL);
    s->createNode(1, "/userService", "method01", "method01_data", dwt::NodeType::EPHEMERAL);

    dwt_proto::CreateNodeResponse resp;
    resp.ParseFromString(s->createNode(1, "/userService/method01", "getUser", "getUser", dwt::NodeType::EPHEMERAL));
    if(resp.success()) {
        std::cout << "success" << std::endl;
    } else {
        std::cout << resp.errmsg() << std::endl;
    }

    dwt::DNode* curr;
    if(s->walkTo("/userService/method01/a", &curr)) {
        std::cout << curr->data << std::endl;
    } else {
        std::cout << "walk error" << std::endl;
    }
}

int main() {

    test();
    return 0;
    
    dwt::EventLoop loop;

    dwt::SRCServer server(
        &loop,
        dwt::InetAddress("127.0.0.1", 8888),
        "SRCServer"
    );

    server.start();
    loop.loop();

    return 0;
}