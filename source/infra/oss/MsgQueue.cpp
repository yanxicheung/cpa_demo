#include "MsgQueue.h"
#include <string>
#include <functional>
#include <iostream>
#include <utility>
#include "OSSDef.h"

using namespace std;

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
    shared_ptr<executorList> executors = GetExecutors();
    assert(!executors.unique());
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
    if(!executors_.unique()) //if executors_ is read in dispatch, we copy on write
    {
        executors_.reset(new executorList(*executors_));
    }
    assert(executors_.unique());
    executors_->push_back(pExecutor);
}

void MsgQueue::removeExecutor(Executor* p)
{
    // this is safe, because MsgQueue's life is Entire program life cycle
    MutexLockGuard lock(executorsMutex_);
    if(!executors_.unique()) //if executors_ is read in dispatch, we copy on write
    {
        executors_.reset(new executorList(*executors_));
    }
    assert(executors_.unique());

    auto iter = executors_->begin();
    while(iter!= executors_->end())
    {
        if(iter->expired())
        {
            iter = executors_->erase(iter);
        }
        else
        {
            ++iter;
        }
    }
    delete p;
}

Handle MsgQueue::observerRegist(const ObserverConfig& config)
{
    shared_ptr<Executor> pExecutor(new Executor(config.entry, config.instKey),
                                   std::bind(&MsgQueue::removeExecutor, this, _1));
    addObserver(pExecutor);
    poweron(config.instKey);
    return pExecutor;
}

void MsgQueue::poweron(const char* instKey)
{
    addMsg(instKey, EV_STARTUP, nullptr, 0);
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

        shared_ptr<executorList> executors = GetExecutors();
        assert(!executors.unique());
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

