#pragma once
#include "common.hpp"

/**
 * @brief This class is a wrapper of epoll API.
 * Under the hood the file descriptor of epoll
 * instance is kept. 
 * 
 */
class Epoll {
private:
    int epollFd;

public:
    Epoll() {
        epollFd = epoll_create1(0);
        if(epollFd == -1) throw std::runtime_error(strerror(errno));
    }
    
    ~Epoll() { close(epollFd); }

    Epoll(const Epoll&) = delete;
    Epoll& operator=(const Epoll&) = delete;
    
    /**
     * @brief Wrapper of epoll_ctl with EPOLL_CTL_ADD flag.
    */
    int add(int fd, epoll_event event) { 
        return epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event); 
    }

    /**
     * @brief Wrapper of epoll_ctl with EPOLL_CTL_MOD flag.
    */
    int mod(int fd, epoll_event event) { 
        return epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &event); 
    }

    /**
     * @brief Wrapper of epoll_ctl with EPOLL_CTL_DEL flag.
    */
    int del(int fd) { 
        return epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, nullptr); 
    }
    
    /**
     * @brief Wrapper of epoll_wait.
    */
    int wait(epoll_event* events, int maxEvents, int timeout) { 
        return epoll_wait(epollFd, events, maxEvents, timeout); 
    }
};
