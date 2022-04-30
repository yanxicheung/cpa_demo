#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#include <assert.h>
#include "OSS.h"
#include "TimerWheel.h"
#include "BlockingQueue.h"
#include "CountDownLatch.h"
#include "Thread.h"
#include "Executor.h"
#include "OSS_Timer.h"

using namespace std;

CountDownLatch* g_latch = nullptr;

extern uint16_t getTimeoutEvent(uint8_t timerNo);

struct TimerParam
{
    TimerParam(pthread_t threadId, uint8_t timerNo):threadId(threadId), timerNo(timerNo){}
    pthread_t threadId;
    uint8_t timerNo;
};

class OSS: Noncopyable
{
public:
    OSS();
    ~OSS();
    void send(const char *instKey, int eventId, const void* msg, int msgLen);
    void sendTimeoutEvent(const TimerParam& timerParam);
    void regist(ThreadConfig* configs, uint16_t size);
    void poweron();
private:
    void dispatch();
private:
    MutexLock mutex_;
    Condition notEmpty_;
    std::deque<Msg> msgs_;
    Thread thread_;
    vector<Executor*> executors_;
};

OSS::OSS()
: mutex_(),
  notEmpty_(mutex_),
  thread_(std::bind(&OSS::dispatch, this))
{
}

OSS::~OSS()
{
    thread_.join();
    for (auto executor : executors_)
    {
        delete executor;
    }
}

void OSS::send(const char *instKey, int eventId, const void* msg, int msgLen) // 在其他线程中调用;
{
    Msg cMsg(instKey, eventId, msg, msgLen);
    {
        MutexLockGuard lock(mutex_);
        msgs_.push_back(move(cMsg));
        notEmpty_.notify();
    }
}
// fixme race condtion!
void OSS::sendTimeoutEvent(const TimerParam& timerParam)
{
    for (auto &executor : executors_)
     {
         if (timerParam.threadId == executor->getThreadId())
         {
             uint32_t timeOutEvent = getTimeoutEvent(timerParam.timerNo);
             auto& key = executor->getKey();
             send(key.c_str() ,timeOutEvent , nullptr, 0);
             break;
         }
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
        executors_.push_back(pExecutor);
    }
    thread_.start();
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

static OSS gOss;

////////////////////////////////////////////////////////////////////////
class OSSTimer: Noncopyable
{
public:
    OSSTimer();
    void setLoopTimer(uint8_t timerNo, uint32_t duration);
    void setRelativeTimer(uint8_t timerNo, uint32_t duration);
    ~OSSTimer();
private:
    TimerWheel timerWheel;
};

OSSTimer::OSSTimer()
{

}

void OSSTimer::setLoopTimer(uint8_t timerNo, uint32_t duration)
{
    pthread_t threadId = pthread_self();
    TimerParam* param = new TimerParam(threadId, timerNo);
    assert(param != nullptr);

    timerWheel.setLoopTimer(duration, std::bind([](void* param){
        TimerParam* pTimeParam = (TimerParam*)param;
        assert(pTimeParam != nullptr);
        gOss.sendTimeoutEvent(*pTimeParam);

    }, _1), param);
}

void OSSTimer::setRelativeTimer(uint8_t timerNo, uint32_t duration)
{
    pthread_t threadId = pthread_self();
    TimerParam* param = new TimerParam(threadId, timerNo);
    assert(param != nullptr);

    timerWheel.setRelativeTimer(duration, std::bind([](void* param){

        TimerParam* pTimeParam = (TimerParam*)param;
        assert(pTimeParam != nullptr);
        gOss.sendTimeoutEvent(*pTimeParam);
        delete pTimeParam;

    }, _1), param);
}

OSSTimer::~OSSTimer()
{

}

static OSSTimer gOssTimer;
////////////////////////////////////////////////////

void OSS_Init()
{

}

void OSS_UserRegist(ThreadConfig* configs, uint16_t size)
{
    gOss.regist(configs, size);
}

void OSS_Send(const char *instKey, int eventId, const void* msg, int msgLen)
{
    gOss.send(instKey, eventId, msg, msgLen);
}

void OSS_SetLoopTimer(uint8_t timerNo, uint32_t duration)
{
    gOssTimer.setLoopTimer(timerNo, duration);
}

void OSS_SetRelativeTimer(uint8_t timerNo, uint32_t duration)
{
    gOssTimer.setRelativeTimer(timerNo, duration);
}

