//
// Created by inory on 2022/10/30.
//

#ifndef AX_MEDIAPIPLE_GRAPH_H
#define AX_MEDIAPIPLE_GRAPH_H


#include <vector>

#include "named_object.h"
#include "json/json.h"
#include "stream.h"
#include "node.h"
#include "node_factory.h"

namespace ax {
    class Graph : public NamedObject {
    public:
        /**
         * @brief 初始化
         * @return
         */
        bool Init(const Json::Value& config);

        /**
         * @brief 打开节点
         * @return
         */
        bool Open();

        /**
         * @brief 关闭节点
         * @return
         */
        bool Close();

        /**
         * @brief 反初始化
         */
        void Uninit();

    private:
        std::shared_ptr<Node> add_node(const std::string& node_type, const std::string& node_name, const Json::Value& config);
        void add_stream(int capacity, const std::string& input_node_name, const std::string& input_port_name, const std::string& output_node_name, const std::string& output_port_name);
        void add_stream(int capacity, std::shared_ptr<Node>& input_node, const std::string& input_port_name, std::shared_ptr<Node>& output_node, const std::string& output_port_name);
        std::shared_ptr<Node> find_node(const std::string& node_name);

    private:
        std::vector<std::shared_ptr<Stream> > m_stream;
        std::vector<std::shared_ptr<Node> > m_nodes;
    };
} // namespace ax

#endif //AX_MEDIAPIPLE_GRAPH_H
