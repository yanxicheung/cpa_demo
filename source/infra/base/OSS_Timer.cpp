#include "OSS_Timer.h"

OSSTimer::OSSTimer()
{
}

OSSTimer::~OSSTimer()
{
}

TimerId OSSTimer::runAt(const Timestamp& time, const TimerCallback& cb)
{
    return timerQueue_.addTimer(cb, time, 0.0);
}

TimerId OSSTimer::runAfter(double delay, const TimerCallback& cb)
{
    Timestamp time(addTime(Timestamp::now(), delay));
     return runAt(time, cb);
}

TimerId OSSTimer::runEvery(double interval, const TimerCallback& cb)
{
    Timestamp time(addTime(Timestamp::now(), interval));
    return timerQueue_.addTimer(cb, time, interval);
}
