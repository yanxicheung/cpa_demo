#ifndef H0401AA16_77CC_4B5A_8B63_D4C8BA9C1909
#define H0401AA16_77CC_4B5A_8B63_D4C8BA9C1909

#include "TimerWheel.h"

class OSSTimer: Noncopyable
{
public:
    OSSTimer();
    void setLoopTimer(uint8_t timerNo, uint32_t duration);
    void setRelativeTimer(uint8_t timerNo, uint32_t duration);
    void setSendTimeOutCallback(const SendTimeOutCallback& cb);
    ~OSSTimer();
private:
    TimerWheel timerWheel_;
    SendTimeOutCallback  sendTimeOutcb_;
};

#endif /* H0401AA16_77CC_4B5A_8B63_D4C8BA9C1909 */
