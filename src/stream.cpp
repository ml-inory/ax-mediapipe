//
// Created by inory on 2022/10/30.
//
#include "stream.h"
#include "port.h"

namespace ax {
    /**
     * @brief 生成Stream
     * @param p_nCapacity       容量，默认-1，表示不限制
     * @return
     */
    std::shared_ptr<Stream> MakeStream(int p_nCapacity) {
        return std::make_shared<Stream>(p_nCapacity);
    }

    /**
     * @brief: 推数据
     *
     * @param： packet
     */
    bool Stream::Push(const Packet& packet) {
        if (IsFull()) {
            return false;
        }

        const std::lock_guard<std::mutex> lg(m_Mutex);
        m_PacketQueue.push(packet);

        if (m_TimestampQueue.empty()) {
            m_Timestamp = packet.Timestamp();
        } else {
            m_TimestampQueue.push(packet.Timestamp());
        }
        return true;
    }

    /**
     * @brief: 拿数据
     *
     * @return：bool
     */
    bool Stream::Pop(Packet& packet) {
        if (IsEmpty()) {
            packet = Packet();
            return false;
        }

        const std::lock_guard<std::mutex> lg(m_Mutex);

        packet = m_PacketQueue.front();
        m_PacketQueue.pop();

        if (!m_TimestampQueue.empty()) {
            m_Timestamp = m_TimestampQueue.front();
            m_TimestampQueue.pop();
        }

        return true;
    }
}