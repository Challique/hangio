#include "dispatcher.hpp"

void Dispatcher::load(int fd, std::vector<uint8_t> bytes) {
    this->fd = fd;
    this->loaded = true;
    this->bytes = std::move(bytes);
    this->sent = 0;
}

void Dispatcher::discard() {
    fd = -1;
    loaded = false;
    bytes.clear();
    sent = 0;
}

bool Dispatcher::isLoaded() const {return loaded; }

bool Dispatcher::isDone() const { return bytes.size() == sent; }

int Dispatcher::getFd() const { return fd; }

std::vector<uint8_t> Dispatcher::getBytes() const { return bytes; }

size_t Dispatcher::bytesTotal() const { return bytes.size(); }

size_t Dispatcher::bytesSent() const { return sent; }

int Dispatcher::send() {
    if(!loaded) return 0;
    if(bytes.size() == sent) return 0;

    int ret = ::send(fd, bytes.data() + sent, bytes.size() - sent, SEND_FLAGS);

    if(ret == -1) return ret;
    sent += ret;
    return ret;
}