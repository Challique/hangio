#include "listener.hpp"
#include "connection.hpp"

Listener::Listener() {
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1)
        throw std::runtime_error(strerror(errno));
}

Listener::~Listener() { close(fd); }

bool Listener::bind(const std::string& ip, uint16_t port) {
    sockaddr_in bindAddress;
    bindAddress.sin_family = AF_INET;
    bindAddress.sin_port = htons(port);
    bindAddress.sin_addr.s_addr = inet_addr(ip.c_str());

    if(bindAddress.sin_addr.s_addr == INADDR_NONE) return false;
    if(::bind(fd, (sockaddr*)&bindAddress, sizeof(bindAddress))) return false;
    bound = true;

    return true;
}

bool Listener::listen(int backlog) {
    if(::listen(fd, backlog)) return false;    
    listening = true;
    return true;
}

bool Listener::isBound() const { return bound; }

bool Listener::isListening() const { return listening; }


std::unique_ptr<Connection> Listener::accept() {
    sockaddr_in peerAddress;
    socklen_t addressLength = sizeof(peerAddress);
    int connFd = ::accept(fd, (sockaddr*)&peerAddress, &addressLength);

    if(connFd == -1) return nullptr;

    uint16_t connPort = ntohs(peerAddress.sin_port);
    std::string connIp(inet_ntoa(peerAddress.sin_addr));

    return std::unique_ptr<Connection>(new Connection(connFd, true, connIp, connPort));
}