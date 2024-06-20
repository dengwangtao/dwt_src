#include "Services.h"
#include "tcp/Logger.h"

#include <sstream>

namespace dwt {

Services::Services()
    : m_root(std::make_unique<DNode>()) {

    m_root->data = "root";

    // 绑定handler

    m_funcs[dwt_proto::ServiceType::Create] = std::bind(&Services::createNodeHandler,   this, std::placeholders::_1, std::placeholders::_2);
    m_funcs[dwt_proto::ServiceType::Get]    = std::bind(&Services::getNodeHandler,      this, std::placeholders::_1, std::placeholders::_2);
    m_funcs[dwt_proto::ServiceType::Set]    = std::bind(&Services::setNodeHandler,      this, std::placeholders::_1, std::placeholders::_2);
    m_funcs[dwt_proto::ServiceType::Delete] = std::bind(&Services::deleteNodeHandler,   this, std::placeholders::_1, std::placeholders::_2);
    m_funcs[dwt_proto::ServiceType::Ls]     = std::bind(&Services::lsNodeHandler,       this, std::placeholders::_1, std::placeholders::_2);
    m_funcs[dwt_proto::ServiceType::Stat]   = std::bind(&Services::statNodeHandler,     this, std::placeholders::_1, std::placeholders::_2);
    m_funcs[dwt_proto::ServiceType::Exists] = std::bind(&Services::existsNodeHandler,   this, std::placeholders::_1, std::placeholders::_2);
}


std::string Services::handle(dwt_proto::ServiceType operation, const std::string& str, size_t sessionId) {
    if(m_funcs.count(operation)) {
        return m_funcs[operation](str, sessionId);
    }
    return "error";
}


/**
 * path: "/userService/method1/para1/"
 * =>
 * pathes: ["", "userService", "method1", "para1"]
 */
std::vector<std::string> Services::splitPath(const std::string& path) {
    std::vector<std::string> pathes;
    std::stringstream ss(path);
    std::string item;
    while(std::getline(ss, item, '/')) {
        pathes.push_back(item);
        // LOG_INFO("path: |%s|", item.c_str());
    }
    return pathes;
}

/**
 * nodeName
 * outNode为传出参数
 */
bool Services::walkTo(const std::string& path, std::string& nodeName, DNode** outNode) { // path: /userService/method1/para1
    *outNode = nullptr;
    std::vector<std::string> pathes = splitPath(path);  // 右值

    if(pathes.size() <= 0) {
        LOG_INFO("pathes.size() <= 0");
        *outNode = nullptr;
        return false;
    }
    nodeName = pathes.back();
    pathes.pop_back();

    // walkto
    DNode* curr = m_root.get();
    
    for(int i = 1; i < pathes.size() && curr; ++ i) {
        bool found = false;
        for(auto& ch : curr->children) {
            if(ch.first == pathes[i]) {
                curr = ch.second.get();
                found = true;
                break;
            }
        }
        if(!found) {
            // path不存在
            *outNode = nullptr;
            return false;
        }
    }
    *outNode = curr;
    return true;
}



std::string Services::createNodeHandler(const std::string& str, size_t sessionId) {
    dwt_proto::CreateNodeRequest req;
    if(!req.ParseFromString(str)) {
        LOG_ERROR("%s:%d ParseFromString error", __FILE__, __LINE__);

        dwt_proto::CreateNodeResponse resp;
        resp.set_success(false);
        resp.set_errmsg("Create Node Request Parse Error");
        return resp.SerializeAsString();
    }
    return createNode(sessionId, req.path(), req.nodedata(), static_cast<NodeType>(req.nodetype()));
}
std::string Services::createNode(size_t sessionId, const std::string& path, const std::string& nodeData, NodeType nodeType) {
    DNode* curr = nullptr;
    dwt_proto::CreateNodeResponse resp;
    std::string nodeName;
    if(!walkTo(path, nodeName, &curr)) {
        // 错误
        resp.set_success(false);
        resp.set_errmsg("path not exists");
        return resp.SerializeAsString();
    }

    if(curr->children.count(nodeName) || nodeName == "") { // 已存在/根节点
        // 节点已存在
        resp.set_success(false);
        resp.set_errmsg("node already exists");
    } else {
        // 创建节点
        curr->children[nodeName] = std::make_unique<DNode>(nodeName, nodeData, nodeType, sessionId);
        resp.set_success(true);
        
    }
    return resp.SerializeAsString();
}


std::string Services::getNodeHandler(const std::string& str, size_t sessionId) {
    dwt_proto::GetNodeRequest req;
    if(!req.ParseFromString(str)) {
        LOG_ERROR("%s:%d ParseFromString error", __FILE__, __LINE__);

        dwt_proto::GetNodeResponse resp;
        resp.set_success(false);
        resp.set_errmsg("Create Node Request Parse Error");
        return resp.SerializeAsString();
    }
    return getNode(sessionId, req.path());
}
std::string Services::getNode(size_t sessionId, const std::string& path) {
    DNode* curr = nullptr;
    dwt_proto::GetNodeResponse resp;
    std::string nodeName;
    if(!walkTo(path, nodeName, &curr)) {
        // 错误
        resp.set_success(false);
        resp.set_errmsg("path not exists / path error");
        return resp.SerializeAsString();
    }

    if(nodeName == "") {
        // root 节点
        resp.set_success(true);
        resp.set_nodedata(curr->data);

    } else if(curr->children.count(nodeName)) {
        // 节点已存在
        resp.set_success(true);
        resp.set_nodedata(curr->children[nodeName]->data);

    } else {
        // 节点不存在
        resp.set_success(false);
        resp.set_errmsg("node not exists");
        
    }
    return resp.SerializeAsString();
}


std::string Services::setNodeHandler(const std::string& str, size_t sessionId) {
return "";
}
std::string Services::setNode(size_t sessionId, const std::string& path, const std::string& nodeData) {
return "";
}


std::string Services::deleteNodeHandler(const std::string& str, size_t sessionId) {
return "";
}
std::string Services::deleteNode(size_t sessionId, const std::string& path) {
return "";
}


std::string Services::lsNodeHandler(const std::string& str, size_t sessionId) {
return "";
}
std::string Services::lsNode(size_t sessionId, const std::string& path) {
return "";
}


std::string Services::statNodeHandler(const std::string& str, size_t sessionId) {
return "";
}
std::string Services::statNode(size_t sessionId, const std::string& path) {
return "";
}


std::string Services::existsNodeHandler(const std::string& str, size_t sessionId) {
return "";
}
std::string Services::existsNode(size_t sessionId, const std::string& path) {
return "";
}



} // end namespace dwt