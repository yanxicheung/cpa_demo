#ifndef HF1A5E03D_DFA7_48B5_AAA6_89EB479F31E2
#define HF1A5E03D_DFA7_48B5_AAA6_89EB479F31E2

#include <functional>
#include "Noncopyable.h"
#include "Thread.h"
#include "Callbacks.h"
#include "Timestamp.h"
#include <set>
#include <vector>

class Timer;
class TimerId;

class TimerQueue :Noncopyable
{
public:
    TimerQueue();
    ~TimerQueue();

    TimerId addTimer(const TimerCallback& cb,
                     Timestamp when,
                     double interval);
private:
    // FIXME: use unique_ptr<Timer> instead of raw pointers.
    typedef std::pair<Timestamp, Timer*> Entry;
    typedef std::set<Entry> TimerList;

    // called when timerfd alarms
    void handleRead();
    // move out all expired timers
    std::vector<Entry> getExpired(Timestamp now);
    void reset(const std::vector<Entry>& expired, Timestamp now);
    bool insert(Timer* timer);
private:
    const int timerfd_;
    // Timer list sorted by expiration
    TimerList timers_;
  //  Thread ioThread; // IOÏß³Ì;
};

#endif /* HF1A5E03D_DFA7_48B5_AAA6_89EB479F31E2 */
