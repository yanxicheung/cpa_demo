#include "MsgQueue.h"
#include <string>
#include <functional>
#include <iostream>
#include <utility>
#include "OSSDef.h"

using std::string;

MsgQueue::MsgQueue()
: queueMutex_(),
  notEmpty_(queueMutex_),
  thread_(std::bind(&MsgQueue::dispatch, this)),
  executorsMutex_(),
  executors_(new executorList)
{
    thread_.start();
}

MsgQueue::~MsgQueue()
{

}

void MsgQueue::addMsg(const char *instKey, int eventId, const void* msg, int msgLen) // 在其他线程中调用;
{
    Msg cMsg(instKey, eventId, msg, msgLen);
    {
        MutexLockGuard lock(queueMutex_);
        msgs_.push_back(std::move(cMsg));
        notEmpty_.notify();
    }
}

void MsgQueue::addTimeoutMsg(const pthread_t& threadId, uint16_t timeOutEvent)
{
    shared_ptr<executorList> executors;
    {
        MutexLockGuard lock(executorsMutex_);
        executors = executors_;
        assert(!executors.unique());
    }

    for (auto iter = executors->begin();iter!= executors->end();++iter)
    {
        shared_ptr<Executor>  pExecutor(iter->lock());
        if (pExecutor && threadId == pExecutor->getThreadId())
        {
            auto& key = pExecutor->getKey();
            addMsg(key.c_str() ,timeOutEvent , nullptr, 0);
            break;
        }
    }
}

void MsgQueue::addObserver(const std::shared_ptr<Executor>& pExecutor)
{
    MutexLockGuard lock(executorsMutex_);
    if(!executors_.unique()) // executors_ is read in dispatch
    {
        std::cout<< "=======copy on write======="<<std::endl;
        executors_.reset(new executorList(*executors_));
    }
    assert(executors_.unique());
    executors_->push_back(pExecutor);
    std::cout<< "addObserver :"<< executors_->size()<<std::endl;
}

Handle MsgQueue::observerRegist(const ObserverConfig& config)
{
    shared_ptr<Executor> pExecutor(new Executor(config.entry, config.instKey));
    addObserver(pExecutor);
    poweron(config.instKey);
    return pExecutor;
}

void MsgQueue::poweron(const char* instKey)
{
    addMsg(instKey, EV_STARTUP, "hello", 6);
}

void MsgQueue::dispatch()
{
    while (true)
    {
        MutexLockGuard lock(queueMutex_);
        while (msgs_.empty())
        {
            notEmpty_.wait();
        }
        assert(!msgs_.empty());
        const Msg& msg = msgs_.front();

        shared_ptr<executorList> executors;
        {
            MutexLockGuard lock(executorsMutex_);
            executors = executors_;
            assert(!executors.unique());
        }
        for(auto iter = executors->begin();iter!= executors->end();++iter)
        {
            shared_ptr<Executor>  pExecutor(iter->lock());
            string instKey(msg.instKey_);
            if (pExecutor && instKey == pExecutor->getKey())
            {
                pExecutor->addMsg(msg);
            }
        }
        msgs_.pop_front();
    }
}
