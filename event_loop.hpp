#pragma once
#include "common.hpp"
#include "epoll.hpp"
#include "flag.hpp"

class EventLoop { 
public:
    EventLoop();
    virtual ~EventLoop();

    EventLoop(const EventLoop&) = delete;
    EventLoop& operator=(const EventLoop&) = delete;

    void join();
    void release();

protected:
    virtual void handleEvents(uint32_t events, void*) = 0;

    int add(int fd, uint32_t events, void* data);
    int mod(int fd, uint32_t events, void* data);
    int del(int fd);

private:
    Epoll epoll;
    Flag flag;

    int numWorkers = 0;
    bool releasing = false;

    pthread_mutex_t mutex;
    pthread_cond_t condition;
    
    void eventLoop();
    
    static const int MAX_EVENTS = 64;
};

