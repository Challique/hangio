#pragma once
#include "common.hpp"

template <typename T>
struct Header {
    static_assert(std::is_enum_v<T>, "Type of Header must be enum.");

    T type{};
    size_t bodySize{};

    static const size_t size = sizeof(Header<T>);
};

template <typename T>
std::vector<uint8_t> serialize(const Header<T>& header) {
    std::vector<uint8_t> bytes;
    bytes.resize(header.size);
    memcpy(bytes.data(), &header, header.size);
    return bytes;
}

template <typename T>
bool deserialize(const std::vector<uint8_t>& bytes, Header<T>& header) {
    if(bytes.size() < header.size) return false;
    memcpy(&header, bytes.data(), header.size);
    return true;
}
