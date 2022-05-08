#ifndef H9FFC891D_1A11_4AFE_80D5_2A7CEB20936F
#define H9FFC891D_1A11_4AFE_80D5_2A7CEB20936F

#include "CountDownLatch.h"
#include "Thread.h"
#include "Callbacks.h"
#include "Msg.h"
#include <string>
#include <deque>

class Executor: Noncopyable
{
public:
    Executor(EntryCallback entry, const char* key);
    void addMsg(const Msg& x);
    const std::string& getKey() const;
    const pthread_t& getThreadId() const;
    ~Executor();
private:
    void exec();
private:
    MutexLock mutex_;
    Condition notEmpty_;
    std::deque<Msg> msgs_;
private:
    EntryCallback entryCallback_;
    Thread thread_;
private:
    std::string instKey_;
    bool running_;
};

#endif /* H9FFC891D_1A11_4AFE_80D5_2A7CEB20936F */
