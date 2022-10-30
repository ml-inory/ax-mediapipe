//
// Created by inory on 2022/10/30.
//

#ifndef AX_MEDIAPIPLE_NODE_FACTORY_H
#define AX_MEDIAPIPLE_NODE_FACTORY_H

#include <string>
#include <unordered_map>
#include <memory>

#include "node.hpp"

namespace ax {
    // 创建Node函数指针
    typedef std::shared_ptr<Node> (*pCreateNode)(void);

    // Node工厂
    class NodeFactory {
    public:
        ~NodeFactory() = default;

        std::shared_ptr<Node> GetNodeByName(const std::string& className);

        void RegisterClass(const std::string& name, pCreateNode method);

        static NodeFactory& GetInstance();

    private:
        NodeFactory() {};

        std::unordered_map<std::string, pCreateNode> m_mapNodeFunc;
    };

    // 注册动作类
    class NodeRegisterAction
    {
    public:
        NodeRegisterAction(const std::string& className, pCreateNode pCreateFn) {
            NodeFactory::GetInstance().RegisterClass(className, pCreateFn);
        }
    };

#define REGISTER_NODE(className)                      \
        static std::shared_ptr<Node> Create##className() {                     \
            return std::make_shared<className>();                            \
        }                                                    \
        const NodeRegisterAction createRegister##className(           \
            #className, (pCreateNode)Create##className);
}

#endif //AX_MEDIAPIPLE_NODE_FACTORY_H
