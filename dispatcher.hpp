#pragma once
#include "common.hpp"

class Dispatcher {
public:
    void load(int fd, std::vector<uint8_t> bytes);
    void discard();

    int getFd() const;
    std::vector<uint8_t> getBytes() const;

    bool isLoaded() const;
    bool isDone() const;

    size_t bytesTotal() const;
    size_t bytesSent() const;

    int send();

private:
    int fd = -1;
    bool loaded = false;

    std::vector<uint8_t> bytes;
    size_t sent = 0;

    static const int SEND_FLAGS = MSG_DONTWAIT|MSG_NOSIGNAL;
};