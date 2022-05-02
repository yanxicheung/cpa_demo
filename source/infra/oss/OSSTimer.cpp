#include "OSSTimer.h"
#include "OSSDef.h"

namespace
{
    uint16_t getTimeoutEvent(uint8_t timerNo)
    {
        return EV_TIMER_BEGIN + timerNo;
    }
}

struct TimerParam
{
    TimerParam(pthread_t threadId, uint8_t timerNo):threadId(threadId), timerNo(timerNo){}
    pthread_t threadId;
    uint8_t timerNo;
};

OSSTimer::OSSTimer()
{

}

void OSSTimer::setSendTimeOutCallback(const SendTimeOutCallback& cb)
{
    sendTimeOutcb_ = cb;
}

void OSSTimer::setLoopTimer(uint8_t timerNo, uint32_t duration)
{
    pthread_t threadId = pthread_self();
    TimerParam* param = new TimerParam(threadId, timerNo);
    assert(param != nullptr);

    timerWheel_.setLoopTimer(duration, std::bind([&](void* param){
        TimerParam* pTimeParam = (TimerParam*)param;
        assert(pTimeParam != nullptr);
        sendTimeOutcb_(pTimeParam->threadId, getTimeoutEvent(pTimeParam->timerNo));

    }, _1), param);
}

void OSSTimer::setRelativeTimer(uint8_t timerNo, uint32_t duration)
{
    pthread_t threadId = pthread_self();
    TimerParam* param = new TimerParam(threadId, timerNo);
    assert(param != nullptr);

    timerWheel_.setRelativeTimer(duration, std::bind([&](void* param){

        TimerParam* pTimeParam = (TimerParam*)param;
        assert(pTimeParam != nullptr);
        sendTimeOutcb_(pTimeParam->threadId, getTimeoutEvent(pTimeParam->timerNo));
        delete pTimeParam;

    }, _1), param);
}

OSSTimer::~OSSTimer()
{

}
