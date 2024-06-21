#include <iostream>
#include <sstream>
#include <iomanip>

#include "tcp/TcpServer.h"
#include "SRCServer.h"

#include "Services.h"

void test() {
    auto* s = new dwt::Services();

    const int sessionId = 123; // 0x7b
    
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
            if(resp.ParseFromString(s->handle(dwt_proto::ServiceType::Create, req.SerializeAsString(), sessionId))) {
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
            if(resp.ParseFromString(s->handle(dwt_proto::ServiceType::Get, req.SerializeAsString(), sessionId))) {
                if(resp.success()) {
                    std::cout << path << ": " << resp.nodedata() << std::endl;
                } else {
                    std::cout << "获取节点: " << path << " 失败, " << resp.errmsg() << std::endl;
                }
            }
        } else if(op == "set") {
            // set /userservice/method/info [data]
            std::string path;
            ss >> path;

            std::string data;
            ss >> data;

            dwt_proto::SetNodeRequest req;
            req.set_path(path);
            req.set_nodedata(data);

            dwt_proto::SetNodeResponse resp;
            if(resp.ParseFromString(s->handle(dwt_proto::ServiceType::Set, req.SerializeAsString(), sessionId))) {
                if(resp.success()) {
                    std::cout << "set " << path << " = " << data << std::endl;
                } else {
                    std::cout << "设置节点: " << path << " 失败, " << resp.errmsg() << std::endl;
                }
            }
        } else if(op == "delete") {
            // delete /userservice/method/info
            std::string path;
            ss >> path;

            dwt_proto::DeleteNodeRequest req;
            req.set_path(path);

            dwt_proto::DeleteNodeResponse resp;
            if(resp.ParseFromString(s->handle(dwt_proto::ServiceType::Delete, req.SerializeAsString(), sessionId))) {
                if(resp.success()) {
                    std::cout << "delete " << path << " success" << std::endl;
                } else {
                    std::cout << "删除节点: " << path << " 失败, " << resp.errmsg() << std::endl;
                }
            }
        } else if(op == "ls") {
            // ls /userservice/method/info
            std::string path;
            ss >> path;

            dwt_proto::LsNodeRequest req;
            req.set_path(path);

            dwt_proto::LsNodeResponse resp;
            if(resp.ParseFromString(s->handle(dwt_proto::ServiceType::Ls, req.SerializeAsString(), sessionId))) {
                if(resp.success()) {
                    std::cout << "[";
                    for(int i = 0; i < resp.children_size(); ++ i) {
                        if(i) std::cout << ", ";
                        std::cout << resp.children(i);
                    }
                    std::cout << "]" << std::endl;
                } else {
                    std::cout << "ls 节点: " << path << " 失败, " << resp.errmsg() << std::endl;
                }
            }
        } else if(op == "stat") {
            // stat /userservice/method/info
            std::string path;
            ss >> path;

            dwt_proto::StatNodeRequest req;
            req.set_path(path);

            dwt_proto::StatNodeResponse resp;
            if(resp.ParseFromString(s->handle(dwt_proto::ServiceType::Stat, req.SerializeAsString(), sessionId))) {
                if(resp.success()) {
                    std::cout << "nodeType: " << resp.state().nodetype() << std::endl;
                    std::cout << "dataLength: " << resp.state().datalength() << std::endl;
                    std::cout << "numChildren: " << resp.state().numchildren() << std::endl;
                    std::cout << "ephemeralOwner: " << std::hex << resp.state().ephemeralowner() << std::endl;
                } else {
                    std::cout << "ls 节点: " << path << " 失败, " << resp.errmsg() << std::endl;
                }
            }
        } else if(op == "stat") {
            // stat /userservice/method/info
            std::string path;
            ss >> path;

            dwt_proto::StatNodeRequest req;
            req.set_path(path);

            dwt_proto::StatNodeResponse resp;
            if(resp.ParseFromString(s->handle(dwt_proto::ServiceType::Stat, req.SerializeAsString(), sessionId))) {
                if(resp.success()) {
                    std::cout << "nodeType: " << resp.state().nodetype() << std::endl;
                    std::cout << "dataLength: " << resp.state().datalength() << std::endl;
                    std::cout << "numChildren: " << resp.state().numchildren() << std::endl;
                    std::cout << "ephemeralOwner: " << std::hex << resp.state().ephemeralowner() << std::endl;
                } else {
                    std::cout << "ls 节点: " << path << " 失败, " << resp.errmsg() << std::endl;
                }
            }
        } else if(op == "exists") {
            // exists /userservice/method/info
            std::string path;
            ss >> path;

            dwt_proto::ExistsNodeRequest req;
            req.set_path(path);

            dwt_proto::ExistsNodeResponse resp;
            if(resp.ParseFromString(s->handle(dwt_proto::ServiceType::Exists, req.SerializeAsString(), sessionId))) {
                if(resp.success()) {
                    std::cout << path << (resp.exist() ? " exists" : " not exists") << std::endl;
                } else {
                    std::cout << "exists 节点: " << path << " 失败, " << resp.errmsg() << std::endl;
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