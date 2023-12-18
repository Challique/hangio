#pragma once
#include "common.hpp"
#include "header.hpp"

/**
 * @brief This is a class template for Messages.
 * Message is a simple wrapper which keeps
 * type of the message and raw bytes.
 * @note T must be a simple enum or enum class.
 * This can be checked at the time of compilation. 
 */
template <typename T>
struct Message {
    static_assert(std::is_enum_v<T>, "Message type must be enum.");

    T type{};
    std::vector<uint8_t> bytes{};

    size_t size() const;

    /**
     * @brief Copies the bytes of the data object to the right of Message bytes.
     */
    template<typename U, typename D>
    friend Message<U>& operator<<(Message<U>& message, const D& data);

    /**
     * @brief Copies the rightmost sizeof(D) bytes to the data object.
     * @note If size of Message is less than sizeof(D), nothing will happen.
     */
    template<typename U, typename D>
    friend Message<U>& operator>>(Message<U>& message, D& data);
};


template <typename T>
size_t Message<T>::size() const { return bytes.size(); }

template<typename U, typename D>
Message<U>& operator<<(Message<U>& message, const D& data) {
    static_assert(std::is_trivially_copyable_v<D>, "data must be trivially copyable.");

    size_t oldSize = message.size();
    size_t newSize = oldSize + sizeof(D);

    message.bytes.resize(newSize);
    memcpy(message.bytes.data() + oldSize, &data, sizeof(D));

    return message;
}


template<typename U, typename D>
Message<U>& operator>>(Message<U>& message, D& data) {
    static_assert(std::is_trivially_copyable_v<D>, "data must be trivially copyable.");

    if(message.size() < sizeof(D)) return message;

    size_t oldSize = message.size();
    size_t newSize = oldSize - sizeof(D);

    memcpy(&data, message.bytes.data() + newSize, sizeof(D));
    message.bytes.resize(newSize);

    return message;
}

template <typename T>
std::vector<uint8_t> serialize(const Message<T>& message) {
    Header<T> header = {message.type, message.size()};
    std::vector<uint8_t> bytes = serialize(header);
    bytes.insert(bytes.end(), message.bytes.begin(), message.bytes.end());
    return bytes;
}

template <typename T>
bool deserialize(const std::vector<uint8_t>& bytes, Message<T>& message) {
    Header<T> header;
    if(!deserialize(bytes, header)) return false;

    size_t fullSize = header.size + header.bodySize;
    if(bytes.size() < fullSize) return false;

    message.type = header.type;
    message.bytes = std::vector<uint8_t>(bytes.begin() + header.size, bytes.begin() + fullSize);
    return true;
}