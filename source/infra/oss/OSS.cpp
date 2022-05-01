#include "OSS.h"
#include "MsgQueue.h"
#include "OSSTimer.h"
#include <functional>

using namespace std;

class OSS
{
public:
    OSS();
    void init();
    void userRegist(ObserverConfig* configs, uint16_t size);
    void send(const char* instKey, int eventId, const void* msg, int msgLen);
    void setLoopTimer(uint8_t timerNo, uint32_t duration);
    void setRelativeTimer(uint8_t timerNo, uint32_t duration);
private:
    MsgQueue msgQueue_;
    OSSTimer timer_;
};

OSS::OSS()
{

}

void OSS::init()
{
    timer_.setSendTimeOutCallback(std::bind(&MsgQueue::addTimeoutMsg, &msgQueue_, _1, _2));
}

void OSS::userRegist(ObserverConfig* configs, uint16_t size)
{
    msgQueue_.observerRegist(configs, size);
}

void OSS::send(const char* instKey, int eventId, const void* msg, int msgLen)
{
    msgQueue_.addMsg(instKey, eventId, msg, msgLen);
}

void OSS::setLoopTimer(uint8_t timerNo, uint32_t duration)
{
    timer_.setLoopTimer(timerNo, duration);
}

void OSS::setRelativeTimer(uint8_t timerNo, uint32_t duration)
{
    timer_.setRelativeTimer(timerNo, duration);
}


static OSS g_OSS;

void OSS_Init()
{
    g_OSS.init();
}

void OSS_UserRegist(ObserverConfig* configs, uint16_t size)
{
    g_OSS.userRegist(configs, size);
}

void OSS_Send(const char* instKey, int eventId, const void* msg, int msgLen)
{
    g_OSS.send(instKey, eventId, msg, msgLen);
}

void OSS_SetLoopTimer(uint8_t timerNo, uint32_t duration)
{
    g_OSS.setLoopTimer(timerNo, duration);
}

void OSS_SetRelativeTimer(uint8_t timerNo, uint32_t duration)
{
    g_OSS.setRelativeTimer(timerNo, duration);
}

