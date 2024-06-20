#include <iostream>
#include <sstream>

#include "tcp/TcpServer.h"
#include "SRCServer.h"

#include "Services.h"

void test() {
    auto* s = new dwt::Services();
    
    std::string line;
    std::string op;
    while(std::getline(std::cin, line)) {
        std::stringstream ss(line);
        ss >> op;    // get / create / set / ...
        if(op == "create") {
            // create /userservice/method/info  [data]  [1:永久2:会话]
            std::string path;
            ss >> path;

            std::string data;
            ss >> data;

            int type = 1; // 默认为永久节点
            ss >> type;
            if(type == 0) type = 1;

            dwt_proto::CreateNodeRequest req;
            req.set_path(path);
            req.set_nodedata(data);
            req.set_nodetype(static_cast<dwt_proto::NodeType>(type));
            dwt_proto::CreateNodeResponse resp;
            if(resp.ParseFromString(s->handle(dwt_proto::ServiceType::Create, req.SerializeAsString(), 123))) {
                if(resp.success()) {
                    std::cout << "创建节点: " << path << " 成功" << std::endl;
                } else {
                    std::cout << "创建节点: " << path << " 失败, " << resp.errmsg() << std::endl;
                }
            }

        } else if(op == "get") {
            // get /userservice/method/info
            std::string path;
            ss >> path;

            dwt_proto::GetNodeRequest req;
            req.set_path(path);

            dwt_proto::GetNodeResponse resp;
            if(resp.ParseFromString(s->handle(dwt_proto::ServiceType::Get, req.SerializeAsString(), 123))) {
                if(resp.success()) {
                    std::cout << path << ": " << resp.nodedata() << std::endl;
                } else {
                    std::cout << "获取节点: " << path << " 失败, " << resp.errmsg() << std::endl;
                }
            }
        } else {
            std::cout << "invalid operation" << std::endl;
        }
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