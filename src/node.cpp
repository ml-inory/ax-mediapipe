//
// Created by inory on 2022/10/30.
//
#include "node.hpp"
#include "utils.h"

namespace ax {
    /**
     * @brief: 创建输入端口
     *
     * @param： p_portName      端口名称
     * @param： p_packetType    数据类型
     * @return：true
     * @return：false
     */
    bool Node::MakeInputPort(const std::string& p_portName, E_PACKET_TYPE p_packetType, E_PORT_POLICY p_portPolicy) {
        if (m_portNameMap.find(p_portName) != m_portNameMap.end()) {
            return false;
        }
        std::shared_ptr<Port> port = std::make_shared<InputPort>(p_packetType, p_portPolicy);
        port->m_nIndex = m_nPortNum++;
        port->m_strName = p_portName;
        port->m_ptrNode = this;
        m_portNameMap.insert({p_portName, port});
        m_portIndexMap.insert({port->m_nIndex, port});
        return true;
    }

    /**
     * @brief: 创建输出端口
     *
     * @param： p_portName      端口名称
     * @param： p_packetType    数据类型
     * @param： p_portPolicy    端口策略
     * @return：true
     * @return：false
     */
    bool Node::MakeOutputPort(const std::string& p_portName, E_PACKET_TYPE p_packetType, E_PORT_POLICY p_portPolicy, E_FLOWCON_POLICY p_flowPolicy) {
        if (m_portNameMap.find(p_portName) != m_portNameMap.end()) {
            return false;
        }
        std::shared_ptr<Port> port = std::make_shared<OutputPort>(p_packetType, p_portPolicy, p_flowPolicy);
        port->m_nIndex = m_nPortNum++;
        port->m_strName = p_portName;
        port->m_ptrNode = this;
        m_portNameMap.insert({p_portName, port});
        m_portIndexMap.insert({port->m_nIndex, port});
        return true;
    }

    /**
     * @brief: 向端口发送数据
     *
     * @param： p_Port
     * @param： packet
     * @return：true
     * @return：false
     */
    bool Node::SendToPort(Port* p_Port, const Packet& packet) {
        if (p_Port->PortType() != PORT_TYPE_OUTPUT) {
            return false;
        }

        return p_Port->SendPacket(packet);
    }

    /**
     * @brief: 向端口名称为p_portName的端口发送数据
     *
     * @param： p_portName
     * @param： packet
     * @return：true
     * @return：false
     */
    bool Node::SendToPort(const std::string& p_portName, const Packet& packet) {
        auto port = GetPortByName(p_portName);
        if (port == nullptr) {
            return false;
        }

        return SendToPort(port, packet);
    }

    /**
     * @brief: 向端口索引为p_nPortIndex的端口发送数据
     *
     * @param： p_nPortIndex
     * @param： packet
     * @return：true
     * @return：false
     */
    bool Node::SendToPort(int p_nPortIndex, const Packet& packet) {
        auto port = GetPortByIndex(p_nPortIndex);
        if (port == nullptr) {
            return false;
        }

        return SendToPort(port, packet);
    }

    /**
     * @brief: 从端口接收数据
     *
     * @param： p_Port
     * @param： packet
     * @return：bool
     */
    bool Node::ReceiveFromPort(Port* p_Port, Packet& packet) {
        if (p_Port->PortType() != PORT_TYPE_INPUT) {
            packet = Packet();
            return false;
        }

        return p_Port->ReceivePacket(packet);
    }


    /**
     * @brief: 从端口名为p_portName的端口接收数据
     *
     * @param： p_portName
     * @param： packet
     * @return：Packet
     */
    bool Node::ReceiveFromPort(const std::string& p_portName, Packet& packet) {
        auto port = GetPortByName(p_portName);
        if (port == nullptr) {
            return false;
        }

        return ReceiveFromPort(port, packet);
    }

    /**
     * @brief: 从端口索引为p_nPortIndex的端口接收数据
     *
     * @param： p_nPortIndex
     * @param： packet
     * @return：bool
     */
    bool Node::ReceiveFromPort(int p_nPortIndex, Packet& packet) {
        auto port = GetPortByIndex(p_nPortIndex);
        if (port == nullptr) {
            return false;
        }

        return ReceiveFromPort(port, packet);
    }

    /**
     * @brief 同步接收
     * @param p_portNames
     * @param packets
     * @param p_nTimeoutMS   毫秒级超时
     * @return
     */
    bool Node::SyncReceive(const std::vector<std::string>& p_portNames, std::vector<Packet>& packets, int p_nTimeoutMS) {
        std::vector<Port*> ports;
        for (const auto& name : p_portNames) {
            auto p = GetPortByName(name);
            if (p == nullptr) {
                log_error("Cannot find port with name: %s\n", name.c_str());
                return false;
            }
            ports.push_back(p);
        }

        if (ports.empty()) {
            return false;
        }

        auto start_ts = get_timestamp_in_ms();
        packets.clear();
        for (auto& port : ports) {
            Packet p;
            while (!port->ReceivePacket(p) || p.IsEmpty()) {
                auto ts = get_timestamp_in_ms();
                if (p_nTimeoutMS > 0 && ts - start_ts >= p_nTimeoutMS) {
                    return false;
                }
                std::this_thread::sleep_for(std::chrono::microseconds (100));
            }

            packets.push_back(p);
        }

        return true;
    }
}