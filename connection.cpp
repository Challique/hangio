#include "connection.hpp"

Connection::Connection() {
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1)
        throw std::runtime_error(strerror(errno));
}

Connection::Connection(int fd, bool connected, const std::string& ip, uint16_t port) {
    this->fd = fd;
    this->connected = connected;
    this->ip = ip;
    this->port = port;
}

Connection::Connection(Connection&& other) { *this = std::move(other); }

Connection::~Connection() { close(fd); }

Connection& Connection::operator=(Connection&& other) {
    if(this != &other) {
        fd = other.fd;
        other.fd = -1;

        connected = other.connected;
        other.connected = false;

        ip = std::move(other.ip);

        port = other.port;
        other.port = 0;
    }
    return *this;
}

int Connection::connect(const std::string ip, uint16_t port) {
    sockaddr_in peerAddress;
    peerAddress.sin_family = AF_INET;
    peerAddress.sin_port = htons(port);
    peerAddress.sin_addr.s_addr = inet_addr(ip.c_str());

    if(peerAddress.sin_addr.s_addr == INADDR_NONE) return -1;

    if(::connect(fd, (sockaddr*)&peerAddress, sizeof(peerAddress))) return -1;

    this->ip = std::string(ip);
    this->port = port;

    connected = true;
    return 0;
}

int Connection::send(const std::vector<uint8_t>& bytes, int index, size_t size) {
    return ::send(fd, &bytes[index], size, MSG_NOSIGNAL);
}

int Connection::recv(std::vector<uint8_t>& bytes, size_t size) {
    size_t oldSize = bytes.size();
    bytes.resize(oldSize + size);
    int retval = ::recv(fd, &bytes[oldSize], size, MSG_NOSIGNAL);
    if(retval == -1) {
        bytes.resize(oldSize);
        return -1;
    }
    bytes.resize(oldSize + retval);
    return retval;
}

int Connection::shutdownSend() {
    return shutdown(fd, SHUT_WR);
}

int Connection::shutdownRecv() {
    return shutdown(fd, SHUT_RD);
}

bool Connection::isConnected() const { return connected; }

std::string Connection::getIp() const { return ip; }

uint16_t Connection::getPort() const { return port; }
