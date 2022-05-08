#include "OSS.h"
#include "MsgQueue.h"
#include "OSSTimer.h"
#include <functional>
#include "Cleaner.h"
using namespace std;

class OSS
{
public:
    OSS();
    void init();
    Handle userRegist(const ObserverConfig& config);
    void userUnRegist(Handle& handle);
    void send(const char* instKey, int eventId, const void* msg, int msgLen);
    void setLoopTimer(uint8_t timerNo, uint32_t duration);
    void setRelativeTimer(uint8_t timerNo, uint32_t duration);
private:
    MsgQueue msgQueue_;
    OSSTimer timer_;
    Cleaner cleaner_;
};

OSS::OSS()
{

}

void OSS::init()
{
    timer_.setSendTimeOutCallback(std::bind(&MsgQueue::addTimeoutMsg, &msgQueue_, _1, _2));
}

Handle OSS::userRegist(const ObserverConfig& config)
{
    return msgQueue_.observerRegist(config);
}

void OSS::userUnRegist(Handle& handle)
{
    cleaner_.addUnregistHandle(handle);
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

/////////////////////////////////////////////////////////////////////
static OSS g_OSS;

void OSS_Init()
{
    g_OSS.init();
}

Handle OSS_UserRegist(const ObserverConfig& config)
{
    return g_OSS.userRegist(config);
}

void OSS_UserUnregist(Handle& handle)
{
    g_OSS.userUnRegist(handle);
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

