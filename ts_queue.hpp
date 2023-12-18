#pragma once
#include "common.hpp"

template <typename T>
class TSQueue {
public:
    TSQueue();
    ~TSQueue();
    
    TSQueue(const TSQueue& other);
    TSQueue& operator=(const TSQueue& other); 

    /**
     * @brief Returns the number of elements
     * that were present in the queue before 
     * new element was enqueued. 
     */
    int enqueue(T&& element);


    /**
     * @brief Assigns frontmost element to the passed object.
     * If queue is empty, passed object isn't affected. Returns the 
     * number of elements that were present in the queue before the
     * operation.
     * @note If T supports move semantics, takes the advantage.
     */
    int tryDequeue(T& element);


    /**
     * @brief If queue is not empty, works like the ordinary tryDequeue.
     * but if the queue is empty, the thread will block until 
     * an element is enqueued.
     */
    int dequeue(T& element);

    /**
     * @brief Removes all the elements from the queue.
     */
    void clear();

private:
    pthread_mutex_t mutex;
    pthread_cond_t condition;

    bool pending = false; // Is true iff the last tryDequeue or dequeue failed.
    std::deque<T> elements;
};

template<typename T>
TSQueue<T>::TSQueue() {
    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_init(&condition, nullptr);
}


template <typename T>
TSQueue<T>::TSQueue(const TSQueue& other) {
    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_init(&condition, nullptr);

    pthread_mutex_lock(&other.mutex);
    elements = other.elements;
    pthread_mutex_unlock(&other.mutex);
}

template<typename T>
TSQueue<T>::~TSQueue() {
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condition);
}

/**
 * @brief Every thread trying A = B will take the lock which 
 * belongs to the queue having the smaller address of the two.
 * Approach avoids deadlock - that's provable for any number of threads.
 */
template <typename T>
TSQueue<T>& TSQueue<T>::operator=(const TSQueue& other) {
    if(this != &other) {

        if(this < &other) {
            pthread_mutex_lock(&mutex);
            pthread_mutex_lock(&other.mutex);
        }else {
            pthread_mutex_lock(&other.mutex);
            pthread_mutex_lock(&mutex);
        }

        elements = other.elements;
        pthread_mutex_unlock(&other.mutex);
        pthread_mutex_unlock(&mutex);
    }

    return *this;
}

template <typename T>
int TSQueue<T>::enqueue(T&& element) {
    pthread_mutex_lock(&mutex);

    int size = elements.size();
    if(size == 0 && pending) size = -1;

    elements.push_back(std::forward<T>(element));

    pthread_cond_signal(&condition);
    pthread_mutex_unlock(&mutex);

    return size;
}


template <typename T>
int TSQueue<T>::tryDequeue(T& element) {
    pthread_mutex_lock(&mutex);
    if(elements.size() == 0) {
        pending = true;
        pthread_mutex_unlock(&mutex);
        return 0;
    }

    int size = elements.size();

    element = std::move(elements.front());
    elements.pop_front();
    pending = false;

    pthread_mutex_unlock(&mutex);
    return size;
}

template <typename T>
int TSQueue<T>::dequeue(T& element) {
    pthread_mutex_lock(&mutex);
    while(elements.size() == 0) {
        pending = true;
        pthread_cond_wait(&condition, &mutex);
    }

    int size = elements.size();
    
    element = std::move(elements.front());
    elements.pop_front();
    pending = false;

    pthread_mutex_unlock(&mutex);
    return size;
}

template <typename T>
void TSQueue<T>::clear() {
    pthread_mutex_lock(&mutex);
    elements.clear();
    pthread_mutex_unlock(&mutex);
}
