/*
 * Timer.h
 *
 *  Created on: 2022年4月29日
 *      Author: Administrator
 */

#ifndef INCLUDE_INFRA_BASE_TIMER_H_
#define INCLUDE_INFRA_BASE_TIMER_H_
#include <pthread.h>
#include "Callbacks.h"

//基本数据类型定义
typedef char int8;
typedef unsigned char uint8;
typedef uint8 byte;
typedef short int16;
typedef unsigned short uint16;
typedef long int32;
typedef unsigned long uint32;

#define TVN_BITS (6)
#define TVR_BITS (8)
#define TVN_SIZE (1 << TVN_BITS)
#define TVR_SIZE (1 << TVR_BITS)
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
    struct LIST_TIMER ltTimer;  // 定时器双向链表的入口
    uint32 uExpires;            // 定时器到期时间
    uint32 uPeriod;             // 定时器触发后，再次触发的间隔时长。如果为 0，表示该定时器为一次性的
    TimerCallback cb;           // 定时器回调函数
    void *pParam;               // 回调函数的参数
} TIMERNODE, *LPTIMERNODE;

typedef struct TIMER_MANAGER
{
    pthread_mutex_t lock;       // 同步锁
    pthread_t thread;           // 线程句柄
    uint32 uExitFlag;           // 线程退出标识(0:Continue, other: Exit)
    uint32 uJiffies;            // 基准时间(当前时间)，单位：毫秒
    struct LIST_TIMER arrListTimer1[TVR_SIZE];  // 1 级时间轮。在这里表示存储未来的 0 ~ 255 毫秒的计时器。tick 的粒度为 1 毫秒
    struct LIST_TIMER arrListTimer2[TVN_SIZE];  // 2 级时间轮。存储未来的 256 ~ 256*64-1 毫秒的计时器。tick 的粒度为 256 毫秒
    struct LIST_TIMER arrListTimer3[TVN_SIZE];  // 3 级时间轮。存储未来的 256*64 ~ 256*64*64-1 毫秒的计时器。tick 的粒度为 256*64 毫秒
    struct LIST_TIMER arrListTimer4[TVN_SIZE];  // 4 级时间轮。存储未来的 256*64*64 ~ 256*64*64*64-1 毫秒的计时器。tick 的粒度为 256*64*64 毫秒
    struct LIST_TIMER arrListTimer5[TVN_SIZE];  // 5 级时间轮。存储未来的 256*64*64*64 ~ 256*64*64*64*64-1 毫秒的计时器。tick 的粒度为 256*64*64 毫秒
} TIMERMANAGER, *LPTIMERMANAGER;

void SleepMilliseconds(uint32 uMs);

// 创建定时器管理器
LPTIMERMANAGER CreateTimerManager(void);

// 删除定时器管理器
void DestroyTimerManager(LPTIMERMANAGER lpTimerManager);

// 创建一个定时器。fnTimer 回调函数地址。pParam 回调函数的参数。uDueTime 首次触发的超时时间间隔。uPeriod 定时器循环周期，若为0，则该定时器只运行一次。
LPTIMERNODE CreateTimer(LPTIMERMANAGER lpTimerManager, const TimerCallback& cb, void *pParam, uint32 uDueTime, uint32 uPeriod);

// 删除定时器
int32 DeleteTimer(LPTIMERMANAGER lpTimerManager, LPTIMERNODE lpTimer);



#endif /* INCLUDE_INFRA_BASE_TIMER_H_ */
