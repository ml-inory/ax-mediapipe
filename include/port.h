//
// Created by inory on 2022/10/30.
//

#ifndef AX_MEDIAPIPLE_PORT_H
#define AX_MEDIAPIPLE_PORT_H

#include <string>
#include <vector>
#include <algorithm>

#include "named_object.h"
#include "packet.h"
#include "stream.h"

namespace ax {
    // 端口类型
    enum E_PORT_TYPE {
        PORT_TYPE_INPUT = 0,    // 输入
        PORT_TYPE_OUTPUT = 1,   // 输出

        PORT_TYPE_NUM           // DO NOT USE THIS
    };

    // 端口策略
    enum E_PORT_POLICY {
        PORT_POLICY_NONE = 0,         // 无策略，有数据就取
        PORT_POLICY_TIMESTAMP = 1,    // 按照时间戳来，先来后到
        PORT_POLICY_PINGPONG = 2,     // Ping-Poing策略，雨露均沾
        PORT_POLICY_CLONE   = 3,      // 每个Stream克隆一个数据

        PORT_POLICY_NUM
    };

    // 流控策略
    enum E_FLOWCON_POLICY {
        FLOWCON_POLICY_NONE = 0,        // 不做流控
        FLOWCON_POLICY_CLONE = 1,       // 队列满时 复制一个Stream，最大Stream数量由SetMaxStreamNum控制
        FLOWCON_POLICY_DROP = 2,        // 队列满时，抽帧发
        FLOWCON_POLICY_CACHE = 3,       // 队列满时，本地缓存
        FLOWCON_POLICY_CLONE_CACHE = 4, // FLOWCON_POLICY_CLONE和FLOWCON_POLICY_CACHE的结合，到达最大Stream数量时做本地缓存

        FLOWCON_POLICY_NUM
    };

    class Node;

    /**
     * @brief: Node的输入或输出端口，用名称或索引区分，从若干个Stream获取或推送数据，只接受某一类型的Packet，可以有不同的数据同步策略
     *
     */
    class Port : public NamedObject {
    public:
        // 不支持默认构造
        Port() = delete;

        /**
         * @brief: Port 构造函数
         *
         * @param： p_portType      端口类型
         * @param： p_packetType    数据类型
         * @param： p_portPolicy    端口策略
         * @param： p_flowPolicy    流控策略
         * @see: E_PORT_TYPE  E_PACKET_TYPE
         */
        Port(E_PORT_TYPE p_portType, E_PACKET_TYPE p_packetType, E_PORT_POLICY p_portPolicy = PORT_POLICY_NONE, E_FLOWCON_POLICY p_flowPolicy = FLOWCON_POLICY_NONE):
                m_portType(p_portType),
                m_packetType(p_packetType),
                m_portPolicy(p_portPolicy),
                m_flowPolicy(p_flowPolicy),
                m_nIndex(-1),
                m_ptrNode(nullptr),
                m_nMaxStreamNum(0) {

        }

        virtual ~Port() = default;

        friend class Node;

        // 获取 端口类型
        inline E_PORT_TYPE PortType() const {
            return m_portType;
        }

        // 获取 数据类型
        inline E_PACKET_TYPE PacketType() const {
            return m_packetType;
        }

        // 获取 端口策略
        inline E_PORT_POLICY PortPolicy() const {
            return m_portPolicy;
        }

        // 获取 流控策略
        inline E_FLOWCON_POLICY FlowPolicy() const {
            return m_flowPolicy;
        }

        // 获取 最大Stream数量
        inline int MaxStreamNum() const {
            return m_nMaxStreamNum;
        }

        // 获取 Stream数量
        inline int StreamNum() const {
            return (int)m_streams.size();
        }

        // 获取 Node
        Node* GetNode() const;

        // 设置 最大Stream数量
        inline void SetMaxStreamNum(int p_nMaxStreamNum) {
            m_nMaxStreamNum = p_nMaxStreamNum;
        }

        // 设置 流控策略
        inline void SetFlowPolicy(E_FLOWCON_POLICY policy) {
            if (policy == FLOWCON_POLICY_CLONE || policy == FLOWCON_POLICY_CLONE_CACHE) {
                if (m_nMaxStreamNum <= 0) {
                    m_nMaxStreamNum = std::max(4, (int)m_streams.size());
                }
            }
            m_flowPolicy = policy;
        }

        inline int Index() const {
            return m_nIndex;
        }

        virtual bool SendPacket(const Packet& packet) {
            return false;
        }

        virtual bool ReceivePacket(Packet& packet) {
            return false;
        }

        // 添加 Stream
        bool AddStream(const std::shared_ptr<Stream>& stream);

    protected:
        E_PORT_TYPE m_portType;
        const E_PACKET_TYPE m_packetType;
        E_PORT_POLICY m_portPolicy;

        // 流控策略
        E_FLOWCON_POLICY m_flowPolicy;

        int m_nIndex;
        class Node* m_ptrNode;

        int m_nMaxStreamNum;
        std::vector<std::shared_ptr<Stream> > m_streams;
    };

    /**
     * @brief: 输入端口
     *
     */
    class InputPort : public Port {
    public:
        // 不支持默认构造
        InputPort() = delete;

        /**
         * @brief: 输入端口 构造函数
         *
         * @param： p_packetType
         * @param： p_portPolicy
         */
        explicit InputPort(E_PACKET_TYPE p_packetType, E_PORT_POLICY p_portPolicy = PORT_POLICY_NONE):
                Port(PORT_TYPE_INPUT, p_packetType, p_portPolicy),
                m_recvIndex(0) {

        }

        /**
         * @brief: 接受包
         *
         * @return：bool
         */
        bool ReceivePacket(Packet& packet) final;

    private:
        int m_recvIndex;
    };

    class OutputPort : public Port {
    public:
        // 不支持默认构造
        OutputPort() = delete;

        /**
         * @brief: 输出端口 构造函数
         *
         * @param： p_packetType
         * @param： p_portPolicy
         */
        explicit OutputPort(E_PACKET_TYPE p_packetType, E_PORT_POLICY p_portPolicy = PORT_POLICY_NONE, E_FLOWCON_POLICY p_flowPolicy = FLOWCON_POLICY_NONE):
                Port(PORT_TYPE_OUTPUT, p_packetType, p_portPolicy, p_flowPolicy),
                m_sendIndex(0),
                m_nDefaultMaxStreamNum(4),
                m_IsCacheWorkerStarted(false),
                m_IsDropStarted(false),
                m_nDrop(0),
                m_nSentNum(0),
                m_nDropSentSuccessNum(0) {

            // 当流控策略有克隆操作时，设置默认最大Stream数量
            if (p_flowPolicy == FLOWCON_POLICY_CLONE || p_flowPolicy == FLOWCON_POLICY_CLONE_CACHE) {
                m_nMaxStreamNum = m_nDefaultMaxStreamNum;
            }
        }

        /**
         * @brief: 发送包
         *
         * @param： packet
         * @return：true
         * @return：false
         */
        bool SendPacket(const Packet& packet) final;

        /**
         * @brief 获取 缓存文件路径
         * @return
         */
        std::string GetCachePath() const;

    private:
        // 根据端口策略发包
        bool SendPacketOnPolicy(const Packet& packet);

        // 根据端口策略和流控策略发包
        bool SendPacketWithFlowcon(const Packet& packet);

        // 流控
        bool FlowControlSend(const Packet& packet);

        // 流控 克隆
        bool FlowControlSend_Clone(const Packet& packet);

        // 流控 缓存
        bool FlowControlSend_Cache(const Packet& packet);

        // 流控 恢复缓存数据
        void FlowControlSend_CacheResendWorker();

        // 流控 丢包
        void FlowControlSend_Drop(const Packet& packet);

    private:
        int m_sendIndex;
        const int m_nDefaultMaxStreamNum;
        bool m_IsCacheWorkerStarted;

        bool m_IsDropStarted;
        int m_nDrop;
        int m_nSentNum;
        int m_nDropSentSuccessNum;
    };
} // namespace ax

#endif //AX_MEDIAPIPLE_PORT_H
