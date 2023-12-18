#pragma once
#include "common.hpp"

/**
 * @brief Flag encapsulates a file descriptor, 
 * which can become writeable and readable and
 * can be used for signaling in file descriptor
 * multiplexing (select, poll, epoll). 
 * 
 * Under the hood, this is just an eventfd.
 * Even though eventfd_write and eventfd_read functions
 * are used and are thread-safe, this class doesn't use
 * synchronization mechanisms for other 
 * functionality. So the class is NOT thread-safe.
 * 
 */
class Flag {
private:
    int fd;
    bool raised = false;

public:
    Flag() {
        fd = eventfd(0, 0);
        if(fd == -1) throw std::runtime_error(strerror(errno));
    }

    ~Flag() { close(fd); }

    Flag(const Flag&) = delete;
    Flag& operator=(const Flag&) = delete;

    /**
     * @brief Returns the file descriptor
     * associated with the Flag instance.
     */
    int getFd() const { return fd; }


    /**
     * @brief Writes to the fd
     * making it readable.
     * @return 0 on success, -1 on error (errno is set).
     */
    int raise() {
        if(raised) return 0; 
        if(eventfd_write(fd, (eventfd_t) 1)) return -1;
        raised = true;
        return 0;
    }

    /**
     * @brief Reads from the fd
     * making it non-readable.
     * @return 0 on success, -1 on error (errno is set).
     */
    int lower() {
        if(!raised) return 0;
        eventfd_t value;
        if(eventfd_read(fd, &value)) return -1;
        raised = false;
        return 0;
    }

    /** 
     * @return true iff the Flag is raised.
     */
    bool isRaised() { return raised; }
};
