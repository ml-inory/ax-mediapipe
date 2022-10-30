//
// Created by inory on 2022/10/30.
//
#include "node_factory.h"
#include "node_type.h"
#include "utils.h"

namespace ax {
    std::shared_ptr<Node> NodeFactory::GetNodeByName(const std::string& className) {
        auto iter = m_mapNodeFunc.find(className);
        if (iter == m_mapNodeFunc.end()) {
            log_error("Cannot find node type: %s\n", className.c_str());
            return nullptr;
        }

        return iter->second();
    }

    void NodeFactory::RegisterClass(const std::string& name, pCreateNode method) {
        if (m_mapNodeFunc.find(name) == m_mapNodeFunc.end()) {
            log_info("Register node %s\n", name.c_str());
            m_mapNodeFunc.insert(make_pair(name, method));
        }
    }

    NodeFactory& NodeFactory::GetInstance() {
        static NodeFactory oFactory;
        return oFactory;
    }
}