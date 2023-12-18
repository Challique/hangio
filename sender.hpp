#pragma once
#include "common.hpp"
#include "event_loop.hpp"
#include "ts_queue.hpp"
#include "dispatcher.hpp"
#include "connection.hpp"
#include "error.hpp"

typedef std::function<void(Error, std::vector<uint8_t>, size_t)> Callback;

struct Packet{
    std::vector<uint8_t> bytes;
    Callback callback;
};

struct Outbox {
    std::shared_ptr<Connection> connection;
    std::atomic_bool unenroll = false;

    TSQueue<Packet> packets;
    Packet packet;
    Dispatcher dispatcher;
};

class Sender: public EventLoop { 
public:
    bool enroll(std::shared_ptr<Connection> connection);
    bool send(std::shared_ptr<Connection>, std::vector<uint8_t> bytes, Callback callback);
    bool unenroll(std::shared_ptr<Connection> connection);

private:
    std::mutex mutex;
    std::unordered_map<int, Outbox*> map;

    static const int FLAGS_INACTIVE = EPOLLET|EPOLLONESHOT;
    static const int FLAGS_ACTIVE = EPOLLOUT|EPOLLET|EPOLLONESHOT;
private:
    void handleEvents(uint32_t events, void* data);
    void handleError(Error state, Outbox* outbox);
    void onSuccess(Outbox* outbox);
    void onFailure(Error error, Outbox* outbox);
    Error dispatch(Outbox* outbox);
    Outbox* findOutbox(int fd);
};
