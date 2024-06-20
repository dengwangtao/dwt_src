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

    std::string handle(dwt_proto::ServiceType operation, const std::string& str, size_t sessionId);

    std::string createNodeHandler(const std::string& str, size_t sessionId);
    std::string createNode(size_t sessionId, const std::string& path, const std::string& nodeData, NodeType nodeType);

    std::string getNodeHandler(const std::string& str, size_t sessionId);
    std::string getNode(size_t sessionId, const std::string& path);

    std::string setNodeHandler(const std::string& str, size_t sessionId);
    std::string setNode(size_t sessionId, const std::string& path, const std::string& nodeData);

    std::string deleteNodeHandler(const std::string& str, size_t sessionId);
    std::string deleteNode(size_t sessionId, const std::string& path);

    std::string lsNodeHandler(const std::string& str, size_t sessionId);
    std::string lsNode(size_t sessionId, const std::string& path);

    std::string statNodeHandler(const std::string& str, size_t sessionId);
    std::string statNode(size_t sessionId, const std::string& path);

    std::string existsNodeHandler(const std::string& str, size_t sessionId);
    std::string existsNode(size_t sessionId, const std::string& path);

private:

    std::vector<std::string> splitPath(const std::string& path);
    bool walkTo(const std::string& path, std::string& nodeName, DNode** outNode);


    using Handler = std::function<std::string(const std::string&, size_t)>;

    std::unordered_map<dwt_proto::ServiceType, Handler> m_funcs;

    std::unique_ptr<DNode> m_root;   // 根节点 "/"
};

} // end namespace dwt