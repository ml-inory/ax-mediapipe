//
// Created by inory on 2022/10/30.
//

#ifndef AX_MEDIAPIPLE_STREAM_H
#define AX_MEDIAPIPLE_STREAM_H


#include <mutex>
#include <deque>
#include <queue>
#include <memory>
#include <thread>

#include "named_object.h"
#include "packet.hpp"

namespace ax {
    class Port;

    class Stream : public NamedObject {
    public:
        Stream(int p_nCapacity = -1):
                m_nCapacity(p_nCapacity) {

        }

        virtual ~Stream() = default;

        /**
         * @brief 克隆一个相同配置的Stream，元素不会被克隆
         * @return std::shared_ptr<Stream>
         */
        std::shared_ptr<Stream> Clone() const {
            auto ret = std::make_shared<Stream>(m_nCapacity);
            ret->m_InputPort = m_InputPort;
            ret->m_OutputPort = m_OutputPort;
            return ret;
        }

        /**
         * @brief: 推数据
         *
         * @param： packet
         */
        bool Push(const Packet& packet);

        /**
         * @brief: 拿数据
         *
         * @return：bool
         */
        bool Pop(Packet& packet);

        // 获取 队列长度
        inline int Size() const {
            const std::lock_guard<std::mutex> lg(m_Mutex);
            int size = (int)m_PacketQueue.size();
            return size;
        }

        // 获取 队列容量，-1表示无限制
        inline int Capacity() const {
            return m_nCapacity;
        }

        // 获取 是否为空
        inline bool IsEmpty() const {
            return Size() == 0;
        }

        // 获取 是否满了
        inline bool IsFull() const {
            return m_nCapacity > 0 && Size() >= m_nCapacity;
        }

        // 获取 当前最后一个元素的时间戳
        inline unsigned long Timestamp() const {
            return m_Timestamp;
        }

        // 获取 输入端口
        inline Port* GetInputPort() const {
            return m_InputPort;
        };

        // 获取 输出端口
        inline Port* GetOutputPort() const {
            return m_OutputPort;
        };

        // 设置 输入端口
        inline void SetInputPort(Port* p_Port) {
            m_InputPort = p_Port;
        }

        // 设置 输出端口
        inline void SetOutputPort(Port* p_Port) {
            m_OutputPort = p_Port;
        }

    private:
        mutable std::mutex m_Mutex;
        std::queue<Packet> m_PacketQueue;
        std::queue<unsigned long> m_TimestampQueue;
        // us级时间戳
        unsigned long m_Timestamp;

        Port* m_InputPort, *m_OutputPort;
        int m_nCapacity;
    };

    /**
     * @brief 生成Stream
     * @param p_nCapacity       容量，默认-1，表示不限制
     * @return
     */
    std::shared_ptr<Stream> MakeStream(int p_nCapacity = -1);
} // namespace ax

#endif //AX_MEDIAPIPLE_STREAM_H
