#include "Executor.h"
#include <functional>

Executor::Executor(EntryCallback entry, const char* key)
: mutex_(),
  notEmpty_(mutex_),
  entryCallback_(entry),
  thread_(std::bind(&Executor::exec, this)),
  instKey_(key)
{
    thread_.start();
}

void Executor::addMsg(const Msg& x)  // 在其他线程中调用;
{
    MutexLockGuard lock(mutex_);
    msgs_.push_back(x);
    notEmpty_.notify();
}

const std::string& Executor::getKey() const
{
    return instKey_;
}

const pthread_t& Executor::getThreadId() const
{
    return thread_.GetThreadId();
}

void Executor::exec()  // 消息处理线程;
{
    while (true)
    {
        MutexLockGuard lock(mutex_);
        while (msgs_.empty())
        {
            notEmpty_.wait();
        }
        const Msg& msg = msgs_.front();
        entryCallback_(1, msg.eventId_, msg.data_, msg.len_, nullptr);  // todo state
        msgs_.pop_front();
    }
}

