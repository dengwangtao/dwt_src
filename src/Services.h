#pragma once

#include "DNode.h"
#include "messages.pb.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace dwt {


class Services {

public:
    
    Services(); // 绑定handler
    ~Services() = default;

    std::string createNodeHandler(const std::string& str, size_t sessionId);
    std::string createNode(size_t sessionId, const std::string& path, const std::string& nodeName, const std::string& nodeData, NodeType nodeType);

    std::string getNodeHandler(const std::string& str, size_t sessionId);
    std::string getNode(size_t sessionId, const std::string& path, const std::string& nodeName);

    std::string setNodeHandler(const std::string& str, size_t sessionId);
    std::string setNode(size_t sessionId, const std::string& path, const std::string& nodeName, const std::string& nodeData);

    std::string deleteNodeHandler(const std::string& str, size_t sessionId);
    std::string deleteNode(size_t sessionId, const std::string& path, const std::string& nodeName);

    std::string lsNodeHandler(const std::string& str, size_t sessionId);
    std::string lsNode(size_t sessionId, const std::string& path, const std::string& nodeName);

    std::string statNodeHandler(const std::string& str, size_t sessionId);
    std::string statNode(size_t sessionId, const std::string& path, const std::string& nodeName);

    std::string existsNodeHandler(const std::string& str, size_t sessionId);
    std::string existsNode(size_t sessionId, const std::string& path, const std::string& nodeName);

public:

    std::vector<std::string> splitPath(const std::string& path);
    bool walkTo(const std::string& path, DNode** outNode);

private:

    using Handler = std::function<std::string(const std::string&, size_t)>;

    std::unordered_map<dwt_proto::ServiceType, Handler> m_funcs;

    std::unique_ptr<DNode> m_root;   // 根节点 "/"
};

} // end namespace dwt