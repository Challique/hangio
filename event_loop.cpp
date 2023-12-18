#include "event_loop.hpp"

EventLoop::EventLoop() { 
    epoll_event event = {EPOLLIN, {.ptr = nullptr}};
    if(epoll.add(flag.getFd(), event))
        throw std::runtime_error(strerror(errno));

    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_init(&condition, nullptr);
}

EventLoop::~EventLoop() { 
    pthread_cond_destroy(&condition);    
    pthread_mutex_destroy(&mutex); 
}

int EventLoop::add(int fd, uint32_t events, void* data) {
    if(data == nullptr) return -1;
    return epoll.add(fd, {events, {.ptr = data}});
}

int EventLoop::mod(int fd, uint32_t events, void* data) {
    if(data == nullptr) return -1;
    return epoll.mod(fd, {events, {.ptr = data}});
}

int EventLoop::del(int fd) {
    if(fd == flag.getFd()) return -1;
    return epoll.del(fd);
}

void EventLoop::join() {
    pthread_mutex_lock(&mutex);
    while(releasing) {
        pthread_cond_wait(&condition, &mutex);
    }
    numWorkers++;
    pthread_mutex_unlock(&mutex);    

    eventLoop();
        
    pthread_mutex_lock(&mutex);
    numWorkers--;
    if(numWorkers == 0) {
        flag.lower();
        releasing = false;
        pthread_cond_broadcast(&condition);
    }
    pthread_mutex_unlock(&mutex);
}

void EventLoop::release() {
    pthread_mutex_lock(&mutex);
    flag.raise();
    releasing = true;
    pthread_mutex_unlock(&mutex);
}

void EventLoop::eventLoop() {
    bool done = false;
    while(true) {
        epoll_event events[MAX_EVENTS];
        int numEvents = epoll.wait(events, MAX_EVENTS, -1);
        for(int i = 0; i < numEvents; i++) {
            void* data = events[i].data.ptr;
            if(data == nullptr) {
                done = true;
                break;
            }
            handleEvents(events[i].events, data);
        }
        if(done) break;
    }
}