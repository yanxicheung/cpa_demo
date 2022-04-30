#ifndef H9FFC891D_1A11_4AFE_80D5_2A7CEB20936F
#define H9FFC891D_1A11_4AFE_80D5_2A7CEB20936F

#include "BlockingQueue.h"
#include "CountDownLatch.h"
#include "Thread.h"
#include "Callbacks.h"
#include "Msg.h"
#include <string>
#include <deque>

class Executor: Noncopyable
{
public:
    Executor(Entry entry, const char* key);
    void addMsg(const Msg& x);
    const std::string& getKey() const;
private:
    void exec();
private:
    MutexLock mutex_;
    Condition notEmpty_;
    std::deque<Msg> msgs;
private:
    Entry entry;
    Thread thread;
private:
    std::string instKey;
};

#endif /* H9FFC891D_1A11_4AFE_80D5_2A7CEB20936F */
