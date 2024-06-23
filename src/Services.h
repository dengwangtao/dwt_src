#pragma once

#include "DNode.h"
#include "messages.pb.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>

namespace dwt {

/**
 * 单例
 */
class Services {

private:
    Services(); // 绑定handler
    ~Services() = default;
    Services(const Services&) = delete;
    Services& operator=(const Services&) = delete;

public:
    static Services& getInstance();
    
    std::string handle(dwt_proto::ServiceType operation, const std::string& str, size_t sessionId);
    
    void removeEphemeral(size_t sessionId); // 删除会话节点

private:
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

    /**
     * @deprecated 递归扫图删除会话节点
     */
    void removeEphemeral(DNode* node, size_t sessionId);


private:

    std::vector<std::string> splitPath(const std::string& path);
    DNode* walkTo(const std::vector<std::string>& pathes);

    using Handler = std::function<std::string(const std::string&, size_t)>;

    std::unordered_map<dwt_proto::ServiceType, Handler> m_funcs;

    std::unordered_map<size_t, std::vector<std::weak_ptr<DNode>>> m_sessionMap;

    std::shared_ptr<DNode> m_dummy;   // 哑节点  [] => root

    std::mutex m_mutex;
};

} // end namespace dwt