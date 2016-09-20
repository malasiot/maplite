//
// Copyright (c) 2013 Juan Palacios juan.palacios.puyana@gmail.com
// Subject to the BSD 2-Clause License
// - see < http://opensource.org/licenses/BSD-2-Clause>
//

// Modified to accept stop request by client

#ifndef CONCURRENT_QUEUE_
#define CONCURRENT_QUEUE_

#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

template <typename T>
class Queue
{
public:

    void pop(T& item)
    {
        std::unique_lock<std::mutex> mlock(mutex_);

        cond_.wait(mlock, [&](){ return !queue_.empty() || stop_; });

        if ( stop_ )
             throw std::exception();

        item = queue_.front();
        queue_.pop();
    }

    void push(const T& item)
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        queue_.push(item);
        mlock.unlock();
        cond_.notify_one();
    }

    void stop(){
        stop_ = true ;
        cond_.notify_all();
    }

    Queue(): stop_(false) {}
    Queue(const Queue&) = delete;            // disable copying
    Queue& operator=(const Queue&) = delete; // disable assignment

private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;
    std::atomic<bool> stop_ ;
};

#endif
