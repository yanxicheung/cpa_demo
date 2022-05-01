#include "OSSTimer.h"
#include "OSSDef.h"

namespace
{

#define BEGIN_GET_EVENT_ID    switch(timerId) {

#define GET_TIMER_EVENT(no)   case TIMER_NO_##no: return EV_TIMER_##no;

#define END_GET_EVENT_ID      default: return 0xFFFF;}

    uint16_t getTimeoutEvent(uint8_t timerId)
    {
        BEGIN_GET_EVENT_ID
            GET_TIMER_EVENT(1)
            GET_TIMER_EVENT(2)
            GET_TIMER_EVENT(3)
            GET_TIMER_EVENT(4)
            GET_TIMER_EVENT(5)
            GET_TIMER_EVENT(6)
            GET_TIMER_EVENT(7)
            GET_TIMER_EVENT(8)
        END_GET_EVENT_ID
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
