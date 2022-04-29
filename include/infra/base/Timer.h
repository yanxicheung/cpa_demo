/*
 * Timer.h
 *
 *  Created on: 2022��4��29��
 *      Author: Administrator
 */

#ifndef INCLUDE_INFRA_BASE_TIMER_H_
#define INCLUDE_INFRA_BASE_TIMER_H_
#include <pthread.h>
#include "Callbacks.h"

//�����������Ͷ���
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
    struct LIST_TIMER ltTimer;  // ��ʱ��˫����������
    uint32 uExpires;            // ��ʱ������ʱ��
    uint32 uPeriod;             // ��ʱ���������ٴδ����ļ��ʱ�������Ϊ 0����ʾ�ö�ʱ��Ϊһ���Ե�
    TimerCallback cb;           // ��ʱ���ص�����
    void *pParam;               // �ص������Ĳ���
} TIMERNODE, *LPTIMERNODE;

typedef struct TIMER_MANAGER
{
    pthread_mutex_t lock;       // ͬ����
    pthread_t thread;           // �߳̾��
    uint32 uExitFlag;           // �߳��˳���ʶ(0:Continue, other: Exit)
    uint32 uJiffies;            // ��׼ʱ��(��ǰʱ��)����λ������
    struct LIST_TIMER arrListTimer1[TVR_SIZE];  // 1 ��ʱ���֡��������ʾ�洢δ���� 0 ~ 255 ����ļ�ʱ����tick ������Ϊ 1 ����
    struct LIST_TIMER arrListTimer2[TVN_SIZE];  // 2 ��ʱ���֡��洢δ���� 256 ~ 256*64-1 ����ļ�ʱ����tick ������Ϊ 256 ����
    struct LIST_TIMER arrListTimer3[TVN_SIZE];  // 3 ��ʱ���֡��洢δ���� 256*64 ~ 256*64*64-1 ����ļ�ʱ����tick ������Ϊ 256*64 ����
    struct LIST_TIMER arrListTimer4[TVN_SIZE];  // 4 ��ʱ���֡��洢δ���� 256*64*64 ~ 256*64*64*64-1 ����ļ�ʱ����tick ������Ϊ 256*64*64 ����
    struct LIST_TIMER arrListTimer5[TVN_SIZE];  // 5 ��ʱ���֡��洢δ���� 256*64*64*64 ~ 256*64*64*64*64-1 ����ļ�ʱ����tick ������Ϊ 256*64*64 ����
} TIMERMANAGER, *LPTIMERMANAGER;

void SleepMilliseconds(uint32 uMs);

// ������ʱ��������
LPTIMERMANAGER CreateTimerManager(void);

// ɾ����ʱ��������
void DestroyTimerManager(LPTIMERMANAGER lpTimerManager);

// ����һ����ʱ����fnTimer �ص�������ַ��pParam �ص������Ĳ�����uDueTime �״δ����ĳ�ʱʱ������uPeriod ��ʱ��ѭ�����ڣ���Ϊ0����ö�ʱ��ֻ����һ�Ρ�
LPTIMERNODE CreateTimer(LPTIMERMANAGER lpTimerManager, const TimerCallback& cb, void *pParam, uint32 uDueTime, uint32 uPeriod);

// ɾ����ʱ��
int32 DeleteTimer(LPTIMERMANAGER lpTimerManager, LPTIMERNODE lpTimer);



#endif /* INCLUDE_INFRA_BASE_TIMER_H_ */
