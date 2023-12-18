#pragma once
#include "common.hpp"
#include "event_loop.hpp"

class LoopRunner {

public:
    LoopRunner(int numThreads, std::shared_ptr<EventLoop> eventLoop);
    ~LoopRunner();

    int run();
    void stop();

    int spawned() const;

private:
    std::shared_ptr<EventLoop> eventLoop;
    std::vector<pthread_t> threads;
    std::vector<bool> running;
    int numSpawnedThreads;
    int numRequestedThreads;

    static void* runRoutine(void*);
};