#ifndef H860222E1_D71A_4059_B35F_D7CE0E4A35C2
#define H860222E1_D71A_4059_B35F_D7CE0E4A35C2

#include "Noncopyable.h"
#include "Timestamp.h"
#include "Thread.h"
#include "Callbacks.h"
#include "TimerId.h"
#include "TimerQueue.h"

class OSSTimer :Noncopyable
{
public:
    OSSTimer();
    ~OSSTimer();
    ///
    TimerId runAt(const Timestamp& time, const TimerCallback& cb);
    ///
    /// Runs callback after @c delay seconds.
    ///
    TimerId runAfter(double delay, const TimerCallback& cb);
    ///
    /// Runs callback every @c interval seconds.
    ///
    TimerId runEvery(double interval, const TimerCallback& cb);
private:

private:
    // Thread ioThread; // IOÏß³Ì;
     TimerQueue timerQueue_;
};

#endif /* H860222E1_D71A_4059_B35F_D7CE0E4A35C2 */
