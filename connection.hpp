#pragma once
#include "common.hpp"
#include "listener.hpp"

class Sender;
class Receiver;

class Connection {
public:
    class Handle {
        static int get(const Connection& connection) {
            return connection.fd;
        }
        friend class Sender;
        friend class Receiver;
    };

    Connection();
    ~Connection();

    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    Connection(Connection&& other);
    Connection& operator=(Connection&& other);
    
    int connect(const std::string ip, uint16_t port);
    bool isConnected() const;

    int send(const std::vector<uint8_t>& bytes, int index, size_t size);
    int recv(std::vector<uint8_t>& bytes, size_t size);

    int shutdownRecv();
    int shutdownSend();

    std::string getIp() const;
    uint16_t getPort() const;

private:
    int fd;
    bool connected = false;
    std::string ip = "";
    uint16_t port = 0;

    Connection(int fd, bool connected, const std::string& ip, uint16_t port);
    friend std::unique_ptr<Connection> Listener::accept();
};




