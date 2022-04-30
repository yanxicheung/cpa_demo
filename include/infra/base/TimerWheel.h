#ifndef INCLUDE_INFRA_BASE_TIMER_H_
#define INCLUDE_INFRA_BASE_TIMER_H_

#include "Callbacks.h"
#include "Noncopyable.h"
#include "Thread.h"
#include "Mutex.h"
#include <stdint.h>

#define TVN_BITS (6)
#define TVR_BITS (8)
#define TVN_SIZE (1 << TVN_BITS)   // 64
#define TVR_SIZE (1 << TVR_BITS)   // 256
#define TVN_MASK (TVN_SIZE - 1)
#define TVR_MASK (TVR_SIZE - 1)
#define MAX_TVAL ((unsigned long)((1ULL << (TVR_BITS + 4*TVN_BITS)) - 1))

#define TIME_AFTER(a,b) ((long)(b) - (long)(a) < 0)
#define TIME_BEFORE(a,b) TIME_AFTER(b,a)
#define TIME_AFTER_EQ(a,b) ((long)(a) - (long)(b) >= 0)
#define TIME_BEFORE_EQ(a,b) TIME_AFTER_EQ(b,a)

typedef struct LIST_TIMER
{
    struct LIST_TIMER *pPrev;
    struct LIST_TIMER *pNext;
} LISTTIMER, *LPLISTTIMER;

typedef struct TIMER_NODE
{
    struct LIST_TIMER ltTimer; // 定时器双向链表的入口
    uint32_t uExpires;          // 定时器到期时间   绝对时间
    uint32_t uPeriod;           // 定时器触发后，再次触发的间隔时长。如果为 0，表示该定时器为一次性的
    TimerCallback cb;           // 定时器回调函数
    void *pParam;               // 回调函数的参数
} TIMERNODE, *LPTIMERNODE;

class TimerWheel: Noncopyable
{
public:
    TimerWheel();
    ~TimerWheel();
    void setLoopTimer(uint32_t timerLen, const TimerCallback& cb, void *pParam = NULL);
    void setRelativeTimer(uint32_t timerLen, const TimerCallback& cb, void *pParam = NULL);
private:
    // uDueTime 首次触发的超时时间间隔。uPeriod 定时器循环周期，若为0，则该定时器只运行一次。
    void addTimer(uint32_t uDueTime, uint32_t uPeriod, const TimerCallback& cb, void *pParam);
private:
    void runTimer();
    void run();
    void addTimer(LPTIMERNODE pTmr);
    uint32_t cascadeTimer(struct LIST_TIMER *arrListTimer, uint32_t idx);
private:
    MutexLock mutex;
    Thread thread;
    uint32_t uExitFlag;    // 线程退出标识(0:Continue, other: Exit)
    uint32_t uJiffies;     // 基准时间(当前时间,从系统启动开始算)，单位：毫秒
    // 轮盘里面的时间是剩余超时时间;
    struct LIST_TIMER arrListTimer1[TVR_SIZE];
    struct LIST_TIMER arrListTimer2[TVN_SIZE];
    struct LIST_TIMER arrListTimer3[TVN_SIZE];
    struct LIST_TIMER arrListTimer4[TVN_SIZE];
    struct LIST_TIMER arrListTimer5[TVN_SIZE];
};

#endif /* INCLUDE_INFRA_BASE_TIMER_H_ */
