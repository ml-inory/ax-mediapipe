//
// Created by inory on 2022/10/30.
//
#include <algorithm>

#include "port.h"
#include "node.h"

namespace ax {

    // 获取 Node
    Node* Port::GetNode() const {
        return m_ptrNode;
    }

    // 添加 Stream
    bool Port::AddStream(const std::shared_ptr<Stream>& stream) {
        if (m_nMaxStreamNum > 0 && m_streams.size() >= m_nMaxStreamNum) {
            return false;
        }

        auto iter = std::find(m_streams.begin(), m_streams.end(), stream);
        if (iter != m_streams.end()) {
            return false;
        }

        if (m_portType == PORT_TYPE_INPUT) {
            stream->SetInputPort(this);
        } else {
            stream->SetOutputPort(this);
        }

        m_streams.push_back(stream);

        return true;
    }

    /**
     * @brief: 接受包
     *
     * @return：bool
     */
    bool InputPort::ReceivePacket(Packet& packet) {
        if (m_streams.empty()) {
            return false;
        }

        if (m_portPolicy == PORT_POLICY_NONE || m_portPolicy == PORT_POLICY_CLONE) {
            for (auto& stream : m_streams) {
                if (!stream->IsEmpty()) {
                    return stream->Pop(packet);
                }
            }
        }
        else if (m_portPolicy == PORT_POLICY_TIMESTAMP) {
            std::vector<std::shared_ptr<Stream>> not_empty_streams;
            for (const auto& stream : m_streams) {
                if (!stream->IsEmpty()) {
                    not_empty_streams.push_back(stream);
                }
            }
            // printf("not empty streams: %d\n", not_empty_streams.size());
            if (not_empty_streams.empty()) {
                return false;
            } else if (not_empty_streams.size() == 1) {
                return not_empty_streams[0]->Pop(packet);
            } else {
                auto stream = std::min_element(not_empty_streams.begin(), not_empty_streams.end(), [](const std::shared_ptr<Stream>& lhs, const std::shared_ptr<Stream>& rhs) {
                    return lhs->Timestamp() > 0 && lhs->Timestamp() < rhs->Timestamp();
                });
                if (stream != m_streams.end()) {
                    return (*stream)->Pop(packet);
                }
            }
        }
        else if (m_portPolicy == PORT_POLICY_PINGPONG) {
            if (!m_streams.empty()) {
                if (m_recvIndex % m_streams.size() == 0) {
                    m_recvIndex = 0;
                }
                auto stream = m_streams[m_recvIndex++];
                return stream->Pop(packet);
            }
        }

        return false;
    }


    /**
     * @brief: 发送包
     *
     * @param： packet
     * @return：true
     * @return：false
     */
    bool OutputPort::SendPacket(const Packet& packet) {
        if (m_streams.empty()) {
//            log_error("stream is empty!\n");
            return false;
        }

        if (m_packetType != packet.Type()) {
            log_error("packet type mismatch!\n");
            return false;
        }

        return SendPacketWithFlowcon(packet);
    }

    /**
     * @brief 获取 缓存文件路径
     * @return
     */
    std::string OutputPort::GetCachePath() const {
        std::string fileName = Name();
        if (fileName.empty()) {
            fileName = m_ptrNode->Name() + std::string(":") + std::string("port_") + std::to_string(Index());
        }
        return fileName;
    }

    // 根据端口策略发包
    bool OutputPort::SendPacketOnPolicy(const Packet& packet) {
        if (m_portPolicy == PORT_POLICY_NONE || m_portPolicy == PORT_POLICY_TIMESTAMP) {
            for (auto& stream : m_streams) {
                if (stream->Push(packet))
                    return true;
            }
            return false;
        }
        else if (m_portPolicy == PORT_POLICY_PINGPONG) {
            if (m_sendIndex % m_streams.size() == 0) {
                m_sendIndex = 0;
            }
            auto stream = m_streams[m_sendIndex++];
            return stream->Push(packet);
        }
        else if (m_portPolicy == PORT_POLICY_CLONE) {
            for (const auto& stream : m_streams) {
                if (stream->IsFull()) {
                    return false;
                }
            }

            for (auto& stream : m_streams) {
                stream->Push(packet);
            }

            return true;
        } else {
            return false;
        }
    }

    // 根据端口策略和流控策略发包
    bool OutputPort::SendPacketWithFlowcon(const Packet& packet) {
        if (m_IsDropStarted) {
            if (m_nSentNum >= m_nDrop) {
//                log_info("drop num: %d\n", m_nDrop);
                m_nSentNum = 0;

                if (!SendPacketOnPolicy(packet)) {
                    return FlowControlSend(packet);
                } else {
                    m_nDropSentSuccessNum = std::min(5, m_nDropSentSuccessNum + 1);

                    // 成功发送超过最长Stream的容量时，抽帧--
//                    auto longest_stream = *std::max_element(m_streams.begin(), m_streams.end(),
//                                                                                  [](const std::shared_ptr<Stream>& lhs, const std::shared_ptr<Stream>& rhs) { return lhs->Size() >= rhs->Size(); });
//                    if (m_nDropSentSuccessNum >= longest_stream->Capacity() / 2) {
//                        m_nDropSentSuccessNum = 0;
//                        m_nDrop = std::max(0, m_nDrop - 1);
//                        if (m_nDrop == 0) {
//                            m_IsDropStarted = false;
//                        }
//                    }
                }
            }
            m_nSentNum++;
            return false;
        } else {
            if (!SendPacketOnPolicy(packet)) {
                return FlowControlSend(packet);
            }
        }

        return true;
    }

    // 流控
    bool OutputPort::FlowControlSend(const Packet& packet) {
        if (m_flowPolicy == FLOWCON_POLICY_CLONE) {
            return FlowControlSend_Clone(packet);
        } else if (m_flowPolicy == FLOWCON_POLICY_CLONE_CACHE) {
            if (!FlowControlSend_Clone(packet)) {
                return FlowControlSend_Cache(packet);
            } else {
                return true;
            }
        } else if (m_flowPolicy == FLOWCON_POLICY_DROP) {
            auto longest_stream = *std::max_element(m_streams.begin(), m_streams.end(),
                                                    [](const std::shared_ptr<Stream>& lhs, const std::shared_ptr<Stream>& rhs) { return lhs->Size() >= rhs->Size(); });
            if (longest_stream->Size() <= longest_stream->Capacity() / 2) {
                FlowControlSend_Drop(packet);
            }
            return false;
        } else if (m_flowPolicy == FLOWCON_POLICY_CACHE) {
            return FlowControlSend_Cache(packet);
        } else {
            return false;
        }
    }

    // 流控 克隆
    bool OutputPort::FlowControlSend_Clone(const Packet& packet) {
        int nStreamNum = (int)m_streams.size();
        if (nStreamNum >= m_nMaxStreamNum) {
            return false;
        } else {
            auto stream = m_streams[0]->Clone();
            stream->GetInputPort()->AddStream(stream);
            stream->GetOutputPort()->AddStream(stream);
            return stream->Push(packet);
        }
    }

    // 流控 缓存
    bool OutputPort::FlowControlSend_Cache(const Packet& packet) {
        std::string fileName = GetCachePath();
        std::ofstream ofs(fileName);
        if (packet.Cache(ofs)) {
            if (!m_IsCacheWorkerStarted) {
                std::thread cache_thread(&OutputPort::FlowControlSend_CacheResendWorker, this);
                cache_thread.detach();
                m_IsCacheWorkerStarted = true;
            }
        }
        return true;
    }

    // 流控 恢复缓存数据
    void OutputPort::FlowControlSend_CacheResendWorker() {
        std::string fileName = GetCachePath();
        std::ifstream ifs;
        while (1) {
            bool allStreamsEmpty = true;
            for (const auto& s : m_streams) {
                allStreamsEmpty = allStreamsEmpty && s->IsEmpty();
            }

            if (allStreamsEmpty) {
                ifs.open(fileName);
                while (ifs.is_open()) {
                    auto p = Packet().Restore(ifs);
                    if (p.IsEmpty()) {
                        ifs.close();
                    } else {
                        SendPacket(p);
                    }
                }
            }

            std::this_thread::sleep_for(std::chrono::seconds(60));
        }
    }

    // 流控 丢包
    void OutputPort::FlowControlSend_Drop(const Packet& packet) {
        m_IsDropStarted = true;
        m_nDrop++;
    }
}