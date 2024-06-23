#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <iomanip>


#include "src_client.h"
#include "messages.pb.h"

void run(size_t sessionId) {
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


            
            dwt_proto::ServiceRequest sreq;
            sreq.set_servicetype(dwt_proto::ServiceType::Create);
            sreq.set_sessionid(sessionId);
            sreq.set_serviceparameters(req.SerializeAsString());


            dwt_proto::WrappedRequest wreq;
            
            wreq.set_requesttype(dwt_proto::MessageType::Service);
            wreq.set_requestparameters(sreq.SerializeAsString());
            std::string tobeSend = wreq.SerializeAsString();
            int n = ::send(fd, &tobeSend[0], tobeSend.size(), 0);
            if(n < 0) {
                ::perror("::send");
                exit(1);
            }
            

        } else if(op == "get") {
            // get /userservice/method/info
            std::string path;
            ss >> path;

            dwt_proto::GetNodeRequest req;
            req.set_path(path);

            dwt_proto::ServiceRequest sreq;
            sreq.set_servicetype(dwt_proto::ServiceType::Get);
            sreq.set_sessionid(sessionId);
            sreq.set_serviceparameters(req.SerializeAsString());


            dwt_proto::WrappedRequest wreq;
            
            wreq.set_requesttype(dwt_proto::MessageType::Service);
            wreq.set_requestparameters(sreq.SerializeAsString());
            std::string tobeSend = wreq.SerializeAsString();
            int n = ::send(fd, &tobeSend[0], tobeSend.size(), 0);
            if(n < 0) {
                ::perror("::send");
                exit(1);
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


            dwt_proto::ServiceRequest sreq;
            sreq.set_servicetype(dwt_proto::ServiceType::Set);
            sreq.set_sessionid(sessionId);
            sreq.set_serviceparameters(req.SerializeAsString());


            dwt_proto::WrappedRequest wreq;
            
            wreq.set_requesttype(dwt_proto::MessageType::Service);
            wreq.set_requestparameters(sreq.SerializeAsString());
            std::string tobeSend = wreq.SerializeAsString();
            int n = ::send(fd, &tobeSend[0], tobeSend.size(), 0);
            if(n < 0) {
                ::perror("::send");
                exit(1);
            }

            
        } else if(op == "delete") {
            // delete /userservice/method/info
            std::string path;
            ss >> path;

            dwt_proto::DeleteNodeRequest req;
            req.set_path(path);


            dwt_proto::ServiceRequest sreq;
            sreq.set_servicetype(dwt_proto::ServiceType::Delete);
            sreq.set_sessionid(sessionId);
            sreq.set_serviceparameters(req.SerializeAsString());


            dwt_proto::WrappedRequest wreq;
            
            wreq.set_requesttype(dwt_proto::MessageType::Service);
            wreq.set_requestparameters(sreq.SerializeAsString());
            std::string tobeSend = wreq.SerializeAsString();
            int n = ::send(fd, &tobeSend[0], tobeSend.size(), 0);
            if(n < 0) {
                ::perror("::send");
                exit(1);
            }


            
        } else if(op == "ls") {
            // ls /userservice/method/info
            std::string path;
            ss >> path;

            dwt_proto::LsNodeRequest req;
            req.set_path(path);


            dwt_proto::ServiceRequest sreq;
            sreq.set_servicetype(dwt_proto::ServiceType::Ls);
            sreq.set_sessionid(sessionId);
            sreq.set_serviceparameters(req.SerializeAsString());


            dwt_proto::WrappedRequest wreq;
            
            wreq.set_requesttype(dwt_proto::MessageType::Service);
            wreq.set_requestparameters(sreq.SerializeAsString());
            std::string tobeSend = wreq.SerializeAsString();
            int n = ::send(fd, &tobeSend[0], tobeSend.size(), 0);
            if(n < 0) {
                ::perror("::send");
                exit(1);
            }

            
        } else if(op == "stat") {
            // stat /userservice/method/info
            std::string path;
            ss >> path;

            dwt_proto::StatNodeRequest req;
            req.set_path(path);


            dwt_proto::ServiceRequest sreq;
            sreq.set_servicetype(dwt_proto::ServiceType::Stat);
            sreq.set_sessionid(sessionId);
            sreq.set_serviceparameters(req.SerializeAsString());


            dwt_proto::WrappedRequest wreq;
            
            wreq.set_requesttype(dwt_proto::MessageType::Service);
            wreq.set_requestparameters(sreq.SerializeAsString());
            std::string tobeSend = wreq.SerializeAsString();
            int n = ::send(fd, &tobeSend[0], tobeSend.size(), 0);
            if(n < 0) {
                ::perror("::send");
                exit(1);
            }

           
        } else if(op == "exists") {
            // exists /userservice/method/info
            std::string path;
            ss >> path;

            dwt_proto::ExistsNodeRequest req;
            req.set_path(path);


            dwt_proto::ServiceRequest sreq;
            sreq.set_servicetype(dwt_proto::ServiceType::Exists);
            sreq.set_sessionid(sessionId);
            sreq.set_serviceparameters(req.SerializeAsString());


            dwt_proto::WrappedRequest wreq;
            
            wreq.set_requesttype(dwt_proto::MessageType::Service);
            wreq.set_requestparameters(sreq.SerializeAsString());
            std::string tobeSend = wreq.SerializeAsString();
            int n = ::send(fd, &tobeSend[0], tobeSend.size(), 0);
            if(n < 0) {
                ::perror("::send");
                exit(1);
            }

            
        } else {
            std::cout << "invalid operation" << std::endl;
            continue;
        }




        char buf[1024] = {0}; // receive
        // ::memset(buf, 0, sizeof buf);
        int n = ::read(fd, buf, sizeof buf);
        if(n < 0) {
            ::perror("read");
            exit(1);
        }

        dwt_proto::WrappedResponse wres;
        wres.ParseFromArray(buf, n);
        if(wres.responsetype() == dwt_proto::MessageType::Error) {
            std::cout << "Error: " << wres.data() << std::endl;
        }
        else if(wres.responsetype() == dwt_proto::MessageType::Service) {
            dwt_proto::ServiceResponse sres;
            sres.ParseFromString(wres.data());
            
            if(sres.servicetype() == dwt_proto::ServiceType::Create) {
                dwt_proto::CreateNodeResponse resp;
                if(resp.ParseFromString(sres.data())) {
                    if(resp.success()) {
                        std::cout << "创建节点成功" << std::endl;
                    } else {
                        std::cout << "创建节点失败, " << resp.errmsg() << std::endl;
                    }
                }
            } else if(sres.servicetype() == dwt_proto::ServiceType::Get) {
                dwt_proto::GetNodeResponse resp;
                if(resp.ParseFromString(sres.data())) {
                    if(resp.success()) {
                        std::cout << resp.nodedata() << std::endl;
                    } else {
                        std::cout << "获取节点失败, " << resp.errmsg() << std::endl;
                    }
                }
            } else if(sres.servicetype() == dwt_proto::ServiceType::Set) {
                dwt_proto::SetNodeResponse resp;
                if(resp.ParseFromString(sres.data())) {
                    if(resp.success()) {
                        std::cout << "set success" << std::endl;
                    } else {
                        std::cout << "设置节点失败, " << resp.errmsg() << std::endl;
                    }
                }
            } else if(sres.servicetype() == dwt_proto::ServiceType::Delete) {
                dwt_proto::DeleteNodeResponse resp;
                if(resp.ParseFromString(sres.data())) {
                    if(resp.success()) {
                        std::cout << "delete success" << std::endl;
                    } else {
                        std::cout << "删除节点失败, " << resp.errmsg() << std::endl;
                    }
                }
            } else if(sres.servicetype() == dwt_proto::ServiceType::Ls) {
                dwt_proto::LsNodeResponse resp;
                if(resp.ParseFromString(sres.data())) {
                    if(resp.success()) {
                        std::cout << "[";
                        for(int i = 0; i < resp.children_size(); ++ i) {
                            if(i) std::cout << ", ";
                            std::cout << resp.children(i);
                        }
                        std::cout << "]" << std::endl;
                    } else {
                        std::cout << "ls 节点失败, " << resp.errmsg() << std::endl;
                    }
                }
            } else if(sres.servicetype() == dwt_proto::ServiceType::Stat) {
                 dwt_proto::StatNodeResponse resp;
                if(resp.ParseFromString(sres.data())) {
                    if(resp.success()) {
                        std::cout << "nodeType: " << resp.state().nodetype() << std::endl;
                        std::cout << "dataLength: " << std::dec << resp.state().datalength() << std::endl;
                        std::cout << "numChildren: " << resp.state().numchildren() << std::endl;
                        std::cout << "ephemeralOwner: " << std::hex << resp.state().ephemeralowner() << std::endl;
                    } else {
                        std::cout << "stat 节点失败, " << resp.errmsg() << std::endl;
                    }
                }
            } else if(sres.servicetype() == dwt_proto::ServiceType::Exists) {
                dwt_proto::ExistsNodeResponse resp;
                if(resp.ParseFromString(sres.data())) {
                    if(resp.success()) {
                        std::cout << (resp.exist() ? " exists" : " not exists") << std::endl;
                    } else {
                        std::cout << "exists 节点失败, " << resp.errmsg() << std::endl;
                    }
                }
            }
        }
        
    }
}

void run2() {
    
    auto& client = dwt::SRCCLient::getInstance();
    client.connect("127.0.0.1", 8888);
           
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

            bool res = client.createNode(path, data, type);

            if(res) {
                std::cout << "create success" << std::endl;
            } else {
                std::cout << "create error" << std::endl;
            }
            

        } else if(op == "get") {
            // get /userservice/method/info
            std::string path;
            ss >> path;

            dwt_proto::GetNodeRequest req;
            req.set_path(path);

            std::string res = client.getNode(path);
            std::cout << res << std::endl;

            
        } else if(op == "set") {
            // set /userservice/method/info [data]
            std::string path;
            ss >> path;

            std::string data;
            ss >> data;

            bool res = client.setNode(path, data);
            if(res) {
                std::cout << "set success" << std::endl;
            } else {
                std::cout << "set error" << std::endl;
            }

            
        } else if(op == "delete") {
            // delete /userservice/method/info
            std::string path;
            ss >> path;

            bool res = client.deleteNode(path);
            if(res) {
                std::cout << "set success" << std::endl;
            } else {
                std::cout << "set error" << std::endl;
            }

            
        } else if(op == "ls") {
            // ls /userservice/method/info
            std::string path;
            ss >> path;

            auto children = client.lsNode(path);
            std::cout << "[";
            for(int i = 0; i < children.size(); ++ i) {
                if(i) std::cout << ", ";
                std::cout << children[i];
            }
            std::cout << "]" << std::endl;

            
        } else if(op == "stat") {
            // stat /userservice/method/info
            std::string path;
            ss >> path;

            auto res = client.statNode(path);
            std::cout << "nodeType: " << res.nodetype() << std::endl;
            std::cout << "dataLength: " << std::dec << res.datalength() << std::endl;
            std::cout << "numChildren: " << res.numchildren() << std::endl;
            std::cout << "ephemeralOwner: " << std::hex << res.ephemeralowner() << "\n" << std::endl;
           
        } else if(op == "exists") {
            // exists /userservice/method/info
            std::string path;
            ss >> path;

            bool res = client.existsNode(path);
            if(res) {
                std::cout << "exists" << std::endl;
            } else {
                std::cout << "not exists" << std::endl;
            }
            
        } else {
            std::cout << "invalid operation" << std::endl;
            continue;
        }
        
    }
}


int main() {

    run2();

    return 0;
}


#if 0
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

    // receive
    ::memset(buf, 0, sizeof buf);

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

    
    
    std::thread heartbeatSend([=](size_t sessionId) {
        // udp fd
        int udp_fd = ::socket(AF_INET, SOCK_DGRAM, 0);

        dwt_proto::HeartbeatMessage heartbeat;
        heartbeat.set_sessionid(sessionId);
        std::string sendstr = heartbeat.SerializeAsString();

        if(sessionId != 0) {        
            while(1) {
                ::sendto(udp_fd, &sendstr[0],sendstr.size(), 0, (sockaddr*)&serverAddr, sizeof serverAddr); // 发送sessionid
                std::this_thread::sleep_for(std::chrono::seconds(8));
            }
        }
    }, sessionId);

    

    run(sessionId);

    
    heartbeatSend.join();

    return 0;
}

#endif