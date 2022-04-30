#include "Executor.h"
#include <functional>
extern CountDownLatch* g_latch;

Executor::Executor(Entry entry, const char* key)
: mutex_(),
  notEmpty_(mutex_),
  entry(entry),
  thread(std::bind(&Executor::exec, this)),
  instKey(key)
{
    thread.start();
}

void Executor::addMsg(const Msg& x)  // 在其他线程中调用;
{
    MutexLockGuard lock(mutex_);
    msgs.push_back(x);
    notEmpty_.notify();
}

const std::string& Executor::getKey() const
{
    return instKey;
}

void Executor::exec()  // 消息处理线程;
{
    g_latch->countDown();
    while (true)
    {
        while (msgs.empty())
        {
            notEmpty_.wait();
        }
        const Msg& msg = msgs.front();
        entry(1, msg.eventId_, msg.data_, msg.len_, nullptr);  // todo state
        msgs.pop_front();
    }
}

