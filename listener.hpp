#pragma once
#include "common.hpp"

class Connection;

class Listener {
public:
    Listener();
    ~Listener();

    Listener(const Listener&) = delete;
    Listener& operator=(const Listener&) = delete;

    bool bind(const std::string& ip, uint16_t port);
    bool listen(int backlog);

    std::unique_ptr<Connection> accept();

    bool isBound() const;
    bool isListening() const;

private:
    int fd;

    bool bound = false;
    bool listening = false;
};
