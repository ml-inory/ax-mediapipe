//
// Created by inory on 2022/10/30.
//

#ifndef AX_MEDIAPIPLE_NODE_H
#define AX_MEDIAPIPLE_NODE_H

#include <vector>
#include <unordered_map>
#include <memory>
#include <string>
#include <thread>
#include <pthread.h>

#include "named_object.h"
#include "port.hpp"
#include "stream.hpp"
#include "json/json.h"
#include "utils.h"

namespace ax {
    class Node : public NamedObject {
    public:
        // 默认构造
        Node():
                m_nPortNum(0) {

        }

        // 默认析构
        virtual ~Node() = default;

        friend class Graph;

        /**
         * @brief 初始化
         * @return
         */
        virtual bool Init(const Json::Value& config) {
            return false;
        }

        /**
         * @brief 初始化
         * @param config
         * @return
         */
        bool Open() {
            std::thread t([this]{
                pthread_setname_np(pthread_self(), this->Name().substr(0, 15).c_str());
                while (1) {
                    this->Process();
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                }
            });
            t.detach();

            return true;
        }

        /**
         * @brief 主要工作
         */
        virtual void Process() {
        }

        /**
         * @brief 停止工作
         * @return
         */
        virtual bool Close() {
            return true;
        }

        /**
         * @brief: 创建输入端口
         *
         * @param： p_portName      端口名称
         * @param： p_packetType    数据类型
         * @param： p_portPolicy    端口策略
         * @return：true
         * @return：false
         */
        bool MakeInputPort(const std::string& p_portName, E_PACKET_TYPE p_packetType, E_PORT_POLICY p_portPolicy = PORT_POLICY_NONE);

        /**
         * @brief: 创建输出端口
         *
         * @param： p_portName      端口名称
         * @param： p_packetType    数据类型
         * @param： p_portPolicy    端口策略
         * @param： p_flowPolicy    流控策略
         * @return：true
         * @return：false
         */
        bool MakeOutputPort(const std::string& p_portName, E_PACKET_TYPE p_packetType, E_PORT_POLICY p_portPolicy = PORT_POLICY_NONE, E_FLOWCON_POLICY p_flowPolicy = FLOWCON_POLICY_NONE);

        // 返回端口数量
        inline int PortNum() const {
            return m_nPortNum;
        }

        /**
         * @brief: 获取 Port By Name
         *
         * @param： p_portName
         * @return：Port*
         */
        inline Port* GetPortByName(const std::string& p_portName) const {
            auto port = m_portNameMap.find(p_portName);
            if (port == m_portNameMap.end()) {
                return nullptr;
            } else {
                return m_portNameMap.at(p_portName).get();
            }
        }

        /**
         * @brief: 获取 Port By Index
         *
         * @param： p_portIndex
         * @return：Port*
         */
        inline Port* GetPortByIndex(int p_portIndex) const {
            for (const auto & it : m_portIndexMap) {
                if (it.first == p_portIndex) {
                    return it.second.get();
                }
            }
            return nullptr;
        }

        // 获取 Index
        inline int Index() const {
            return m_nIndex;
        }

        /**
         * @brief: 向端口发送数据
         *
         * @param： p_Port
         * @param： packet
         * @return：true
         * @return：false
         */
        bool SendToPort(Port* p_Port, const Packet& packet);

        /**
         * @brief: 向端口名称为p_portName的端口发送数据
         *
         * @param： p_portName
         * @param： packet
         * @return：true
         * @return：false
         */
        bool SendToPort(const std::string& p_portName, const Packet& packet);

        /**
         * @brief: 向端口索引为p_nPortIndex的端口发送数据
         *
         * @param： p_nPortIndex
         * @param： packet
         * @return：true
         * @return：false
         */
        bool SendToPort(int p_nPortIndex, const Packet& packet);

        /**
         * @brief: 从端口接收数据
         *
         * @param： p_Port
         * @param： packet
         * @return：bool
         */
        bool ReceiveFromPort(Port* p_Port, Packet& packet);

        /**
         * @brief: 从端口名为p_portName的端口接收数据
         *
         * @param： p_portName
         * @param： packet
         * @return：bool
         */
        bool ReceiveFromPort(const std::string& p_portName, Packet& packet);

        /**
         * @brief: 从端口索引为p_nPortIndex的端口接收数据
         *
         * @param： p_nPortIndex
         * @param： packet
         * @return：bool
         */
        bool ReceiveFromPort(int p_nPortIndex, Packet& packet);

        /**
         * @brief 同步接收
         * @param p_portNames
         * @param packets
         * @param p_nTimeoutMS   毫秒级超时
         * @return
         */
        bool SyncReceive(const std::vector<std::string>& p_portNames, std::vector<Packet>& packets, int p_nTimeoutMS);

    private:
        std::unordered_map<std::string, std::shared_ptr<Port> > m_portNameMap;
        std::unordered_map<int, std::shared_ptr<Port> > m_portIndexMap;
        int m_nPortNum;
        int m_nIndex{};
    };
} // namespace ax

#endif //AX_MEDIAPIPLE_NODE_H
