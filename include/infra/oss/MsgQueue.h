
#ifndef H416EBFF0_B6F3_4122_8549_E347482AE4A9
#define H416EBFF0_B6F3_4122_8549_E347482AE4A9
#include "Noncopyable.h"
#include "CountDownLatch.h"
#include "Thread.h"
#include "Msg.h"
#include <vector>
#include "OSSDef.h"
#include "Executor.h"
using std::weak_ptr;
using std::shared_ptr;

class MsgQueue: Noncopyable
{
public:
    typedef std::vector<weak_ptr<Executor>> executorList;
    MsgQueue();
    ~MsgQueue();
    void addMsg(const char *instKey, int eventId, const void* msg, int msgLen);
    void addTimeoutMsg(const pthread_t& threadId, uint16_t timeOutEvent);
    Handle observerRegist(const ObserverConfig& config);
    void poweron(const char* instKey);
private:
    void addObserver(const shared_ptr<Executor>& pExecutor);
    void dispatch();
private:
    MutexLock queueMutex_;
    Condition notEmpty_;
    std::deque<Msg> msgs_;
    Thread thread_;
private:
    MutexLock executorsMutex_;
    shared_ptr<executorList> executors_;
};

#endif /* H416EBFF0_B6F3_4122_8549_E347482AE4A9 */
