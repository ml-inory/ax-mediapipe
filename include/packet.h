//
// Created by inory on 2022/10/30.
//

#ifndef AX_MEDIAPIPLE_PACKET_H
#define AX_MEDIAPIPLE_PACKET_H


#include <memory>
#include <chrono>
#include <typeindex>
#include <fstream>
#include <sstream>

#include "utils.h"

namespace ax {
    /**
     * @brief: Packet类型
     *
     */
    enum E_PACKET_TYPE {
        PACKET_TYPE_EMPTY = 0,  // Empty type
        PACKET_TYPE_DUMMY = 1,  // For test
        PACKET_TYPE_IMAGE = 2,  // Image type
        PACKET_TYPE_DETECT = 3, // Detect result
        PACKET_TYPE_TRACK = 4,  // Track result
        PACKET_TYPE_DATA  = 5,  // Raw data

        PACKET_TYPE_NUM,        // Do not use this
    };

    /**
     * @brief: Stream和Port中的数据包.
     *
     */
    class Packet {
    public:
        // 创建一个空的Packet，IsEmpty()为true
        Packet():
                m_typeIndex(std::type_index(typeid(void))),
                m_packetType(PACKET_TYPE_EMPTY),
                m_realObj(nullptr),
                m_timeStamp(0) {

        }

        /**
         * @brief: Packet 构造函数
         * @details: Timestamp()为构造的时间，实现了类型擦除，参数2是实际的数据，可以是任意类型
         * @example: Packet p(PACKET_TYPE_DUMMY, 10);  int value = p.Get<int>();
         * @example: Packet p(PACKET_TYPE_DUMMY, cv::Mat());  const auto& img = p.Get<cv::Mat>();
         * @tparam T
         * @param： p_packetType    数据包类型
         * @param： p_realObj       传入的真实数据
         * @see E_PACKET_TYPE
         */
        template<typename T>
        Packet(E_PACKET_TYPE p_packetType, const T& p_realObj):
                m_typeIndex(std::type_index(typeid(T))),
                m_packetType(p_packetType),
                m_realObj(std::make_shared<Wrapper<T> >(p_realObj)),
                m_timeStamp(get_timestamp_in_us()) {

        }

        /**
         * @brief: Packet 拷贝构造函数，时间戳是一样的
         *
         * @param： other
         */
        Packet(const Packet& other) = default;

        Packet& operator = (const Packet& other) {
            if (this == &other) {
                return *this;
            }

            this->m_typeIndex = other.m_typeIndex;
            this->m_packetType = other.m_packetType;
            this->m_realObj = other.m_realObj;
            this->m_timeStamp = other.m_timeStamp;

            return *this;
        }

        virtual ~Packet() = default;

        /**
         * @brief: 返回数据类型
         *
         * @return：E_PACKET_TYPE
         */
        inline E_PACKET_TYPE Type() const {
            return m_packetType;
        }

        /**
         * @brief: 返回时间戳
         *
         * @return：std::time_t
         */
        inline unsigned long Timestamp() const {
            return m_timeStamp;
        }

        /**
         * @brief: 判断真实数据是否类型U
         * @example: packet.Is<int>();
         * @tparam U
         * @return：true
         * @return：false
         */
        template<typename U>
        bool Is() const {
            return (m_typeIndex == std::type_index(typeid(U)));
        }

        /**
         * @brief: 获取const T&类型的数据，如果实际上不是这个类型的话会报错
         *
         * @tparam T
         * @return：const T&
         */
        template<class T>
        inline const T& Get() const {
            if (!Is<T>()) {
                throw std::bad_cast();
            }
            auto* derived = static_cast<Wrapper<T>*>(m_realObj.get());
            return *((T*)derived->GetData());
        }

        /**
         * @brief: 返回 是否是空的Packet
         *
         * @return：true
         * @return：false
         */
        bool IsEmpty() const {
            return (m_realObj == nullptr);
        }

        /**
         * @brief 缓存数据
         * @param p_File
         * @return
         */
        bool Cache(std::ofstream& p_File) const {
            if (!p_File.is_open()) {
                return false;
            }

            p_File << CacheToStr() << std::endl;

            return true;
        }

        /**
         * @brief 恢复数据
         * @param p_File
         * @return
         */
        Packet Restore(std::ifstream& p_File) const {
            if (!p_File.is_open()) {
                return {};
            }

            std::string cache_str;
            if (!std::getline(p_File, cache_str)) {
                return {};
            }

            return RestoreFromStr(cache_str);
        }

        /**
         * @brief 缓存数据转成string
         * @return
         */
        virtual std::string CacheToStr() const {
            return "";
        }

        virtual Packet RestoreFromStr(const std::string& p_CacheStr) const{
            return {};
        }

    private:
        /**
         * @brief: 类型擦除Idiom，通过PacketBase和Wrapper的组合可以包装任意类型
         *
         */
        struct PacketBase {
            virtual ~PacketBase() {}
            virtual void* GetData() = 0;
        };

        template<typename T>
        struct Wrapper : public PacketBase {
            T data;

            Wrapper(const T& t):
                    data(std::move(t)) { }

            void* GetData() override {
                return (void*)&data;
            }
        };

    private:
        std::type_index m_typeIndex;
        E_PACKET_TYPE m_packetType;
        std::shared_ptr<PacketBase> m_realObj;
        unsigned long m_timeStamp;
    };
} // namespace ax

#endif //AX_MEDIAPIPLE_PACKET_H
