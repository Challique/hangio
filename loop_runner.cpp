#include "loop_runner.hpp"


LoopRunner::LoopRunner(int numThreads, std::shared_ptr<EventLoop> eventLoop) {
    if(numThreads <= 0)
        throw std::logic_error("Size of thread pool must be positive.");

    this->eventLoop = std::move(eventLoop);
    this->threads = std::vector<pthread_t>(numThreads);
    this->running = std::vector<bool>(numThreads, false);
    this->numRequestedThreads = numThreads;
    this->numSpawnedThreads = 0;
}

LoopRunner::~LoopRunner() {}

int LoopRunner::run() {
    for(int i = 0; i < numRequestedThreads; i++) {
        if(!running[i]) {
            int status = pthread_create(&threads[i], nullptr, &runRoutine, this);
            if(status) return status;
            
            running[i] = true;
            numSpawnedThreads++;
        }
    }
    return 0;
}

int LoopRunner::spawned() const { return numSpawnedThreads; }

void LoopRunner::stop() {
    eventLoop->release();
    for(int i = 0; i < numRequestedThreads; i++) {
        if(running[i]) {
            pthread_join(threads[i], nullptr);
        }
    }
    numSpawnedThreads = 0;
}

void* LoopRunner::runRoutine(void* loopRunnerPtr) {
    LoopRunner& loopRunner = *static_cast<LoopRunner*>(loopRunnerPtr);
    loopRunner.eventLoop->join();
    return nullptr;
}