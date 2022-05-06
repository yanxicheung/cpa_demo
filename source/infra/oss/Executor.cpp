#include "Executor.h"
#include <functional>
#include <iostream>
Executor::Executor(EntryCallback entry, const char* key)
: mutex_(),
  notEmpty_(mutex_),
  entryCallback_(entry),
  thread_(std::bind(&Executor::exec, this)),
  instKey_(key),
  running_(true)
{
    thread_.start();
}

Executor::~Executor()
{
    std::cout<< "~Executor()"<<std::endl;
    running_ = false;
    Msg msg;
    addMsg(msg);
    thread_.join();
    std::cout<< "~Executor() end"<<std::endl;
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

        if(msg.empty())  // recv quit msg
        {
            assert(running_ == false);
            break;
        }
        entryCallback_(1, msg.eventId_, msg.data_, msg.len_, nullptr);  // todo state
        msgs_.pop_front();
    }
    std::cout<< "exec exit"<<std::endl;
}

