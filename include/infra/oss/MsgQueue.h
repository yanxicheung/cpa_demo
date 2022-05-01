
#ifndef H416EBFF0_B6F3_4122_8549_E347482AE4A9
#define H416EBFF0_B6F3_4122_8549_E347482AE4A9
#include "Noncopyable.h"
#include "BlockingQueue.h"
#include "CountDownLatch.h"
#include "Thread.h"
#include "Msg.h"
#include <vector>
#include "OSSDef.h"

class Executor;

class MsgQueue: Noncopyable
{
public:
    MsgQueue();
    ~MsgQueue();
    void addMsg(const char *instKey, int eventId, const void* msg, int msgLen);
    void addTimeoutMsg(const pthread_t& threadId, uint16_t timeOutEvent);
    void observerRegist(ObserverConfig* configs, uint16_t size);
    void poweron();
private:
    void dispatch();
private:
    MutexLock mutex_;
    Condition notEmpty_;
    std::deque<Msg> msgs_;
    Thread thread_;
    std::vector<Executor*> executors_;
};

#endif /* H416EBFF0_B6F3_4122_8549_E347482AE4A9 */
