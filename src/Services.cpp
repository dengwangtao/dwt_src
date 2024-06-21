#include "Services.h"
#include "tcp/Logger.h"

#include <sstream>

namespace dwt {

Services::Services()
    : m_dummy(std::make_unique<DNode>()) {

    m_dummy->data = "dummy";
    m_dummy->children[""] = std::make_unique<DNode>();   // 根节点
    m_dummy->children[""]->data = "root";

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
        if(item == "" && !pathes.empty()) { // 路径格式错误
            return {};
        }
        pathes.push_back(item);
        // LOG_INFO("path: |%s|", item.c_str());
    }
    return pathes;
}


DNode* Services::walkTo(const std::vector<std::string>& pathes) { // path: /userService/method1/para1

    // walkto
    DNode* curr = m_dummy.get();
    
    for(int i = 0; i < pathes.size(); ++ i) {
        if(curr->children.count(pathes[i])) {
            curr = curr->children[pathes[i]].get();
        } else {
            // path不存在
            return nullptr;
        }
    }
    return curr;
}

// ===============================================================================

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
    dwt_proto::CreateNodeResponse resp;
    
    auto pathes = splitPath(path);

    if(pathes.size() <= 0) {
        // 错误
        resp.set_success(false);
        resp.set_errmsg("path error");
        return resp.SerializeAsString();
    }

    std::string nodeName = pathes.back();
    pathes.pop_back();

    DNode* curr = walkTo(pathes);   // 指向父节点

    if(curr == nullptr) {
        // 错误
        resp.set_success(false);
        resp.set_errmsg("path not exists");
        return resp.SerializeAsString();
    }

    if(curr->children.count(nodeName)) { // 已存在
        // 节点已存在
        resp.set_success(false);
        resp.set_errmsg("node already exists");
    } else {
        // 创建节点
        curr->children[nodeName] =
            std::make_unique<DNode>(nodeName, nodeData, nodeType, (nodeType == NodeType::EPHEMERAL ? sessionId: 0));
        resp.set_success(true);
        
    }
    return resp.SerializeAsString();
}


// ===============================================================================

std::string Services::getNodeHandler(const std::string& str, size_t sessionId) {
    dwt_proto::GetNodeRequest req;
    if(!req.ParseFromString(str)) {
        LOG_ERROR("%s:%d ParseFromString error", __FILE__, __LINE__);

        dwt_proto::GetNodeResponse resp;
        resp.set_success(false);
        resp.set_errmsg("Get Node Request Parse Error");
        return resp.SerializeAsString();
    }
    return getNode(sessionId, req.path());
}
std::string Services::getNode(size_t sessionId, const std::string& path) {
    dwt_proto::GetNodeResponse resp;
    
    auto pathes = splitPath(path);

    if(pathes.size() <= 0) {
        // 错误
        resp.set_success(false);
        resp.set_errmsg("path error");
        return resp.SerializeAsString();
    }

    DNode* curr = walkTo(pathes);

    if(curr != nullptr) {
        // 节点已存在
        resp.set_success(true);
        resp.set_nodedata(curr->data);

    } else {
        // 节点不存在
        resp.set_success(false);
        resp.set_errmsg("node not exists");
        
    }
    return resp.SerializeAsString();
}

// ===============================================================================


std::string Services::setNodeHandler(const std::string& str, size_t sessionId) {
    dwt_proto::SetNodeRequest req;
    if(!req.ParseFromString(str)) {
        LOG_ERROR("%s:%d ParseFromString error", __FILE__, __LINE__);

        dwt_proto::SetNodeResponse resp;
        resp.set_success(false);
        resp.set_errmsg("Set Node Request Parse Error");
        return resp.SerializeAsString();
    }
    return setNode(sessionId, req.path(), req.nodedata());
}
std::string Services::setNode(size_t sessionId, const std::string& path, const std::string& nodeData) {
    dwt_proto::SetNodeResponse resp;
    
    auto pathes = splitPath(path);

    if(pathes.size() <= 0) {
        // 错误
        resp.set_success(false);
        resp.set_errmsg("path error");
        return resp.SerializeAsString();
    }

    DNode* curr = walkTo(pathes);

    if(curr != nullptr) {
        // 节点已存在
        resp.set_success(true);

        curr->data = nodeData;

    } else {
        // 节点不存在
        resp.set_success(false);
        resp.set_errmsg("node not exists");
        
    }
    return resp.SerializeAsString();
}


// ===============================================================================

std::string Services::deleteNodeHandler(const std::string& str, size_t sessionId) {
    dwt_proto::DeleteNodeRequest req;
    if(!req.ParseFromString(str)) {
        LOG_ERROR("%s:%d ParseFromString error", __FILE__, __LINE__);

        dwt_proto::DeleteNodeResponse resp;
        resp.set_success(false);
        resp.set_errmsg("Delete Node Request Parse Error");
        return resp.SerializeAsString();
    }
    return deleteNode(sessionId, req.path());
}
std::string Services::deleteNode(size_t sessionId, const std::string& path) {
    dwt_proto::DeleteNodeResponse resp;
    
    auto pathes = splitPath(path);

    if(pathes.size() <= 0) {
        // 错误
        resp.set_success(false);
        resp.set_errmsg("path error");
        return resp.SerializeAsString();
    }

    if(pathes.size() == 1) { // 根节点不允许删除
        // 错误
        resp.set_success(false);
        resp.set_errmsg("delete the root node is not allowed");
        return resp.SerializeAsString();
    }

    std::string nodeName = pathes.back();
    pathes.pop_back();

    DNode* curr = walkTo(pathes);   // 指向父节点

    if(curr != nullptr && curr->children.count(nodeName)) {
        // 节点已存在, 删除
        resp.set_success(true);

        curr->children.erase(nodeName); // 不需要递归删除, 节点使用了智能指针

    } else {
        // 节点不存在
        resp.set_success(false);
        resp.set_errmsg("node not exists");
        
    }
    return resp.SerializeAsString();
}

// ===============================================================================


std::string Services::lsNodeHandler(const std::string& str, size_t sessionId) {
    dwt_proto::LsNodeRequest req;
    if(!req.ParseFromString(str)) {
        LOG_ERROR("%s:%d ParseFromString error", __FILE__, __LINE__);

        dwt_proto::LsNodeResponse resp;
        resp.set_success(false);
        resp.set_errmsg("Ls Node Request Parse Error");
        return resp.SerializeAsString();
    }
    return lsNode(sessionId, req.path());
}
std::string Services::lsNode(size_t sessionId, const std::string& path) {
    dwt_proto::LsNodeResponse resp;
    
    auto pathes = splitPath(path);

    if(pathes.size() <= 0) {
        // 错误
        resp.set_success(false);
        resp.set_errmsg("path error");
        return resp.SerializeAsString();
    }

    DNode* curr = walkTo(pathes);
    if(curr != nullptr) {
        // 节点已存在
        resp.set_success(true);

        // 遍历子节点
        for(const auto& ch : curr->children) { // key: value
            resp.add_children(ch.first);
        }

    } else {
        // 节点不存在
        resp.set_success(false);
        resp.set_errmsg("node not exists");
        
    }
    return resp.SerializeAsString();
}


// ===============================================================================

std::string Services::statNodeHandler(const std::string& str, size_t sessionId) {
    dwt_proto::StatNodeRequest req;
    if(!req.ParseFromString(str)) {
        LOG_ERROR("%s:%d ParseFromString error", __FILE__, __LINE__);

        dwt_proto::StatNodeResponse resp;
        resp.set_success(false);
        resp.set_errmsg("Stat Node Request Parse Error");
        return resp.SerializeAsString();
    }
    return statNode(sessionId, req.path());
}
std::string Services::statNode(size_t sessionId, const std::string& path) {
    dwt_proto::StatNodeResponse resp;
    
    auto pathes = splitPath(path);

    if(pathes.size() <= 0) {
        // 错误
        resp.set_success(false);
        resp.set_errmsg("path error");
        return resp.SerializeAsString();
    }

    DNode* curr = walkTo(pathes);

    if(curr != nullptr) {
        // 节点已存在
        resp.mutable_state()->set_datalength(curr->data.size());
        resp.mutable_state()->set_ephemeralowner(curr->ephemeralOwner);
        resp.mutable_state()->set_numchildren(curr->children.size());
        resp.mutable_state()->set_nodetype(static_cast<dwt_proto::NodeType>(curr->type));
        resp.set_success(true);

    } else {
        // 节点不存在
        resp.set_success(false);
        resp.set_errmsg("node not exists");
        
    }
    return resp.SerializeAsString();
}

// ===============================================================================


std::string Services::existsNodeHandler(const std::string& str, size_t sessionId) {
    dwt_proto::ExistsNodeRequest req;
    if(!req.ParseFromString(str)) {
        LOG_ERROR("%s:%d ParseFromString error", __FILE__, __LINE__);

        dwt_proto::ExistsNodeResponse resp;
        resp.set_success(false);
        resp.set_errmsg("Exists Node Request Parse Error");
        return resp.SerializeAsString();
    }
    return existsNode(sessionId, req.path());
}
std::string Services::existsNode(size_t sessionId, const std::string& path) {
    dwt_proto::ExistsNodeResponse resp;
    
    auto pathes = splitPath(path);

    if(pathes.size() <= 0) {
        // 错误
        resp.set_success(false);
        resp.set_errmsg("path error");
        return resp.SerializeAsString();
    }

    DNode* curr = walkTo(pathes);

    resp.set_success(true);

    resp.set_exist(curr != nullptr);

    return resp.SerializeAsString();
}



} // end namespace dwt