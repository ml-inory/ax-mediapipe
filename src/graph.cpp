//
// Created by inory on 2022/10/30.
//
#include "graph.h"
#include "utils.h"

namespace ax {
    /**
     * @brief 初始化
     * @return
     */
    bool Graph::Init(const Json::Value& config) {

//        auto capture = add_node("OpenCVCapture", "capture", config);
//        auto ped_detect = add_node("PedDetect", "ped_detect", config);
//        auto imshow = add_node("OpenCVImshow", "imshow", config);
//        auto heartbeat = add_node("Heartbeat", "heartbeat", config);
//        auto upload_snapshot = add_node("UploadSnapshot", "upload_snapshot", config);
//        auto fetch_config = add_node("FetchConfig", "fetch_config", config);
//        auto business = add_node("DoorAnalysis", "business", config);
//
//        const int stream_len = 60;
//        add_stream(stream_len, capture, "output", ped_detect, "input");
//        add_stream(5, capture, "output", upload_snapshot, "input");
//
//        add_stream(stream_len, fetch_config, "output", business, "config");
//        add_stream(stream_len, capture, "output", business, "image_input");
//        add_stream(stream_len, ped_detect, "output", business, "detect_input");
//
//        add_stream(stream_len, capture, "output", imshow, "image_input");
//        add_stream(stream_len, business, "track_output", imshow, "track_result");
//        add_stream(stream_len, business, "door_pts_output", imshow, "door_pts_input");

        return true;
    }

    /**
     * @brief 打开节点
     * @param config    APP配置
     * @return
     */
    bool Graph::Open() {
        for (auto node : m_nodes) {
            if (!node->Open()) {
                log_error("Open node %s failed!\n", node->Name().c_str());
                return false;
            }
        }

        return true;
    }

    /**
     * @brief 关闭节点
     * @return
     */
    bool Graph::Close() {
        for (auto node : m_nodes) {
            node->Close();
        }
        return true;
    }

    /**
     * @brief 反初始化
     */
    void Graph::Uninit() {

    }

    std::shared_ptr<Node> Graph::add_node(const std::string& node_type, const std::string& node_name, const Json::Value& config) {
        auto ret = NodeFactory::GetInstance().GetNodeByName(node_type);
        if (ret == nullptr) {
            log_error("Cannot find node type: %s\n", node_type.c_str());
            return nullptr;
        }

        ret->SetName(node_name);

        if (!ret->Init(config)) {
            log_error("node [%s][%s] init failed!\n", node_type.c_str(), node_name.c_str());
            return nullptr;
        }

        m_nodes.push_back(ret);
        return ret;
    }

    void Graph::add_stream(int capacity, const std::string& input_node_name, const std::string& input_port_name, const std::string& output_node_name, const std::string& output_port_name) {
        auto input_node = find_node(input_node_name);
        auto output_node = find_node(output_node_name);
        add_stream(capacity, input_node, input_node_name, output_node, output_node_name);
    }

    void Graph::add_stream(int capacity, std::shared_ptr<Node>& input_node, const std::string& input_port_name, std::shared_ptr<Node>& output_node, const std::string& output_port_name) {
        auto stream = MakeStream(capacity);
        m_stream.push_back(stream);
        input_node->GetPortByName(input_port_name)->AddStream(stream);
        output_node->GetPortByName(output_port_name)->AddStream(stream);
    }

    std::shared_ptr<Node> Graph::find_node(const std::string& node_name) {
        std::shared_ptr<Node> ret = nullptr;
        for (auto n : m_nodes) {
            if (n->Name() == node_name) {
                ret = n;
            }
        }
        return ret;
    }
}