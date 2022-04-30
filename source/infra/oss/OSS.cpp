#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#include <assert.h>
#include "OSS.h"
#include "BlockingQueue.h"
#include "CountDownLatch.h"
#include "Thread.h"
#include "Executor.h"
#include "Timer.h"

using namespace std;

CountDownLatch* g_latch = nullptr;

class OSS: Noncopyable
{
public:
    OSS();
    ~OSS();
    void send(const char *instKey, int eventId, const void* msg, int msgLen);
    void regist(ThreadConfig* configs, uint16_t size);
    void poweron();
private:
    void dispatch();
private:
    MutexLock mutex_;
    Condition notEmpty_;
    std::deque<Msg> msgs;
    Thread dispatchThread;
    vector<Executor*> executors;
};

OSS::OSS()
: mutex_(),
  notEmpty_(mutex_),
  dispatchThread(std::bind(&OSS::dispatch, this))
{
}

OSS::~OSS()
{
    dispatchThread.join();
    for (auto executor : executors)
    {
        delete executor;
    }
}

void OSS::send(const char *instKey, int eventId, const void* msg, int msgLen) // 在其他线程中调用;
{
    Msg cMsg(instKey, eventId, msg, msgLen);
    {
        MutexLockGuard lock(mutex_);
        msgs.push_back(move(cMsg));  // 减少不必要拷贝;
        notEmpty_.notify();
    }
}

void OSS::regist(ThreadConfig* configs, uint16_t size)
{
    assert((configs != nullptr) && (size != 0));
    g_latch = new CountDownLatch(size);
    for (int i = 0; i < size; ++i)
    {
        auto pExecutor = new Executor(configs[i].entry, configs[i].instKey);
        assert(pExecutor != nullptr);
        executors.push_back(pExecutor);
    }
    dispatchThread.start();
    g_latch->wait();  // 所有工作线程都启动完成后,发送power on 消息;
    poweron();
}

void OSS::poweron()
{
    send("all", EV_STARTUP, "hello", 6);
}

void OSS::dispatch()
{
    while (true)
    {
        while (msgs.empty())
        {
            notEmpty_.wait();
        }
        assert(!msgs.empty());
        const Msg& msg = msgs.front();
        for (auto &executor : executors)
        {
            string instKey(msg.instKey_);
            if (instKey == "all" || instKey == executor->getKey())
            {
                executor->addMsg(msg);
            }
        }
        msgs.pop_front();
    }
}

////////////////////////////////////////////////////////////////////////
class OSSTimer: Noncopyable
{
public:
    OSSTimer();

    void SetLoopTimer(uint32_t timerLen, uint32_t param, const TimerCallback& cb);

    void SetRelativeTimer(uint32_t timerLen, uint32_t param, const TimerCallback& cb);
    ~OSSTimer();
private:
    LPTIMERMANAGER pMgr;
};

OSSTimer::OSSTimer()
{
    pMgr = CreateTimerManager();
}

void OSSTimer::SetLoopTimer(uint32_t timerLen, uint32_t param, const TimerCallback& cb)
{
    CreateTimer(pMgr, cb, pMgr, timerLen, timerLen);
}

void OSSTimer::SetRelativeTimer(uint32_t timerLen, uint32_t param, const TimerCallback& cb)
{
    CreateTimer(pMgr, cb, pMgr, timerLen, 0);
}

OSSTimer::~OSSTimer()
{
    DestroyTimerManager(pMgr);
}

////////////////////////////////////////////////////
static OSS oss;
static OSSTimer ossTimer;

void OSS_Init()
{

}

void OSS_UserEntryRegist(ThreadConfig* configs, uint16_t size)
{
    oss.regist(configs, size);
}

void OSS_Send(const char *instKey, int eventId, const void* msg, int msgLen)
{
    oss.send(instKey, eventId, msg, msgLen);
}

void OSS_SetLoopTimer(uint32_t timerLen, uint32_t param, const TimerCallback& cb)
{
    ossTimer.SetLoopTimer(timerLen, param, cb);
}

void OSS_SetRelativeTimer(uint32_t timerLen, uint32_t param, const TimerCallback& cb)
{
    ossTimer.SetRelativeTimer(timerLen, param, cb);
}
