#include "MsgQueue.h"
#include "Executor.h"
#include <string>
#include <functional>
#include <iostream>
#include <utility>
#include "OSSDef.h"

using std::string;

MsgQueue::MsgQueue()
: mutex_(),
  notEmpty_(mutex_),
  thread_(std::bind(&MsgQueue::dispatch, this))
{
}

MsgQueue::~MsgQueue()
{
    thread_.join();
    for (auto executor : executors_)
    {
        delete executor;
    }
}

void MsgQueue::addMsg(const char *instKey, int eventId, const void* msg, int msgLen) // 在其他线程中调用;
{
    Msg cMsg(instKey, eventId, msg, msgLen);
    {
        MutexLockGuard lock(mutex_);
        msgs_.push_back(std::move(cMsg));
        notEmpty_.notify();
    }
}

// fixme race condtion!
void MsgQueue::addTimeoutMsg(const pthread_t& threadId, uint16_t timeOutEvent)
{
    for (auto &executor : executors_)
     {
         if (threadId == executor->getThreadId())
         {
             auto& key = executor->getKey();
             addMsg(key.c_str() ,timeOutEvent , nullptr, 0);
             break;
         }
     }
}

void MsgQueue::observerRegist(ObserverConfig* configs, uint16_t size)
{
    assert((configs != nullptr) && (size != 0));
    for (int i = 0; i < size; ++i)
    {
        auto pExecutor = new Executor(configs[i].entry, configs[i].instKey);
        assert(pExecutor != nullptr);
        executors_.push_back(pExecutor);
    }
    thread_.start();
    poweron();
}

void MsgQueue::poweron()
{
    addMsg("all", EV_STARTUP, "hello", 6);
}

void MsgQueue::dispatch()
{
    while (true)
    {
        MutexLockGuard lock(mutex_);
        while (msgs_.empty())
        {
            notEmpty_.wait();
        }
        assert(!msgs_.empty());
        const Msg& msg = msgs_.front();
        for (auto &executor : executors_)
        {
            string instKey(msg.instKey_);
            if (instKey == "all" || instKey == executor->getKey())
            {
                executor->addMsg(msg);
            }
        }
        msgs_.pop_front();
    }
}
