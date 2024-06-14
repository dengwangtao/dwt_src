#pragma once

#include <string>
#include <unordered_map>


/**
 * ZooKeeper -server host:port cmd args
        stat path [watch]
        set path data [version]
        ls path [watch]
        delquota [-n|-b] path
        ls2 path [watch]
        setAcl path acl
        setquota -n|-b val path
        history 
        redo cmdno
        printwatches on|off
        delete path [version]
        sync path
        listquota path
        rmr path
        get path [watch]
        create [-s] [-e] path data acl
        addauth scheme auth
        quit 
        getAcl path
        close 
        connect host:port
 */

namespace dwt {

enum class NodeType {
    PERSISTENT,         // 永久节点
    EPHEMERAL,          // 临时节点 (会话节点, 会话关闭后, 节点失效)
};


// 节点类型
struct DNode {
    std::string name;   // 节点名称
    std::string data;   // 节点数据
    NodeType    type;   // 永久/会话
    size_t      ephemeralOwner;     // 所属的会话Id
    std::unordered_map<std::string, DNode*> children;   // 子节点

    // dataLength
    // numChildren
};




} // end namespace dwt