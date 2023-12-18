#pragma once
#include "common.hpp"
#include "event_loop.hpp"
#include "connection.hpp"


struct Inbox {
    std::shared_ptr<Connection> connection;
    
    // other fields
};


class Receiver: EventLoop {


public:
    Receiver();
    ~Receiver();

private:

};
