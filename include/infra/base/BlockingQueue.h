#ifndef HBF87374E_7387_4B53_82F6_FFBD35EE758A
#define HBF87374E_7387_4B53_82F6_FFBD35EE758A

#include "Condition.h"
#include "Mutex.h"
#include <deque>
#include <assert.h>
#include <iostream>
using namespace std;

template<typename T>
class BlockingQueue: Noncopyable
{
public:
    BlockingQueue()
    : mutex_(), notEmpty_(mutex_), queue_()
    {
    }

    void put(const T& x)
    {
        MutexLockGuard lock(mutex_);
        queue_.push_back(x);
        notEmpty_.notify();
    }

    void put(T&& x)
    {
        MutexLockGuard lock(mutex_);
        queue_.push_back(forward<T>(x)); // must perfect forward!
        notEmpty_.notify();
    }

    T take()
    {
        MutexLockGuard lock(mutex_);
        // always use a while-loop, due to spurious wakeup
        while (queue_.empty())
        {
            notEmpty_.wait();
        }
        assert(!queue_.empty());
        T front(queue_.front());
        queue_.pop_front();
        return front;
    }

    size_t size() const
    {
        MutexLockGuard lock(mutex_);
        return queue_.size();
    }

private:
    mutable MutexLock mutex_;
    Condition notEmpty_;
    std::deque<T> queue_;
};

#endif /* HBF87374E_7387_4B53_82F6_FFBD35EE758A */
