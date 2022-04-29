/*
 * Timer.cpp
 *
 *  Created on: 2022��4��29��
 *      Author: Administrator
 *      refrence :
 *      http://www.langdebuqing.com/algorithm%20notebook/%E5%A4%9A%E7%BA%A7%E6%97%B6%E9%97%B4%E8%BD%AE%E5%AE%9A%E6%97%B6%E5%99%A8.html
 */
#include <stddef.h>
#include <stdlib.h>
#include <sys/time.h>
#include "Timer.h"

typedef void* (*FNTHREAD)(void *pParam);

static pthread_t ThreadCreate(FNTHREAD fnThreadProc, void *pParam)
{
    pthread_t t;
    if(fnThreadProc == NULL)
        return 0;
    if(pthread_create(&t, NULL, fnThreadProc, pParam) == 0)
        return t;
    else
        return (pthread_t)0;
}

static void ThreadJoin(pthread_t thread)
{
    pthread_join(thread, NULL);
}

static void ThreadDestroy(pthread_t thread)
{

}

// ��ȡ��׼ʱ��
static uint32 GetJiffies_old(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

static uint32 GetJiffies(void)
{
    struct timespec ts;  // ��ȷ�����루10 �� -9 �η��룩
    // ʹ�� clock_gettime ����ʱ����Щϵͳ������ rt �⣬�� -lrt ��������Щ����Ҫ���� rt ��
    clock_gettime(CLOCK_MONOTONIC, &ts);  // ��ȡʱ�䡣���У�CLOCK_MONOTONIC ��ʾ��ϵͳ������һ����ʼ��ʱ,����ϵͳʱ�䱻�û��ı��Ӱ��
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);  // ���غ���ʱ��
}

// ��˫��ѭ��������½����뵽��� pPrev �� pNext ֮��
static void ListTimerInsert(struct LIST_TIMER *pNew, struct LIST_TIMER *pPrev, struct LIST_TIMER *pNext)
{
    pNext->pPrev = pNew;
    pNew->pNext = pNext;
    pNew->pPrev = pPrev;
    pPrev->pNext = pNew;
}

static void ListTimerInsertHead(struct LIST_TIMER *pNew, struct LIST_TIMER *pHead)
{
    ListTimerInsert(pNew, pHead, pHead->pNext);
}

static void ListTimerInsertTail(struct LIST_TIMER *pNew, struct LIST_TIMER *pHead)
{
    ListTimerInsert(pNew, pHead->pPrev, pHead);
}

// ʹ���½�� pNew �滻 pOld ��˫��ѭ�������е�λ�á����˫�������н���һ����� pOld��ʹ�� pNew �滻��ͬ��������һ����� pNew
static void ListTimerReplace(struct LIST_TIMER *pOld, struct LIST_TIMER *pNew)
{
    pNew->pNext = pOld->pNext;
    pNew->pNext->pPrev = pNew;
    pNew->pPrev = pOld->pPrev;
    pNew->pPrev->pNext = pNew;
}

// ʹ���½�� pNew �滻 pOld ��˫��ѭ�������е�λ�á�
static void ListTimerReplaceInit(struct LIST_TIMER *pOld, struct LIST_TIMER *pNew)
{
    ListTimerReplace(pOld, pNew);
    // ʹ�� pNew �滻 pOld ��˫��ѭ�������е�λ�ú�pOld ���������ж��������ˣ�����Ҫ�� pOld ָ���Լ�
    pOld->pNext = pOld;
    pOld->pPrev = pOld;
}

// ��ʼ��ʱ�����е����� tick����ʼ����ÿ�� tick �е�˫������ֻ��һ��ͷ���
static void InitArrayListTimer(struct LIST_TIMER *arrListTimer, uint32 nSize)
{
    uint32 i;
    for(i=0; i<nSize; i++)
    {
        arrListTimer[i].pPrev = &arrListTimer[i];
        arrListTimer[i].pNext = &arrListTimer[i];
    }
}

static void DeleteArrayListTimer(struct LIST_TIMER *arrListTimer, uint32 uSize)
{
    struct LIST_TIMER listTmr, *pListTimer;
    struct TIMER_NODE *pTmr;
    uint32 idx;

    for(idx=0; idx<uSize; idx++)
    {
        ListTimerReplaceInit(&arrListTimer[idx], &listTmr);
        pListTimer = listTmr.pNext;
        while(pListTimer != &listTmr)
        {
            //pTmr = (struct TIMER_NODE *)((uint8 *)pListTimer - offsetof(struct TIMER_NODE, ltTimer)); //reinterpret_cast
        	pTmr = (struct TIMER_NODE *)(pListTimer);
            pListTimer = pListTimer->pNext;
            free(pTmr);
        }
    }
}

// ���ݼ�ʱ���Ľ���ʱ���������ʱ���֡��ڸ�ʱ�����ϵ� tick��Ȼ���¼�ʱ�������뵽�� tick ��˫��ѭ�������β��
static void AddTimer(LPTIMERMANAGER lpTimerManager, LPTIMERNODE pTmr)
{
    struct LIST_TIMER *pHead;
    uint32 i, uDueTime, uExpires;

    uExpires = pTmr->uExpires; // ��ʱ�����ڵ�ʱ��
    uDueTime = uExpires - lpTimerManager->uJiffies;
    if (uDueTime < TVR_SIZE)   // idx < 256 (2��8�η�)
    {
        i = uExpires & TVR_MASK; // expires & 255
        pHead = &lpTimerManager->arrListTimer1[i];
    }
    else if (uDueTime < 1 << (TVR_BITS + TVN_BITS)) // idx < 16384 (2��14�η�)
    {
        i = (uExpires >> TVR_BITS) & TVN_MASK;      // i = (expires>>8) & 63
        pHead = &lpTimerManager->arrListTimer2[i];
    }
    else if (uDueTime < 1 << (TVR_BITS + 2 * TVN_BITS)) // idx < 1048576 (2��20�η�)
    {
        i = (uExpires >> (TVR_BITS + TVN_BITS)) & TVN_MASK; // i = (expires>>14) & 63
        pHead = &lpTimerManager->arrListTimer3[i];
    }
    else if (uDueTime < 1 << (TVR_BITS + 3 * TVN_BITS)) // idx < 67108864 (2��26�η�)
    {
        i = (uExpires >> (TVR_BITS + 2 * TVN_BITS)) & TVN_MASK; // i = (expires>>20) & 63
        pHead = &lpTimerManager->arrListTimer4[i];
    }
    else if ((signed long) uDueTime < 0)
    {
        /*
         * Can happen if you add a timer with expires == jiffies,
         * or you set a timer to go off in the past
         */
        pHead = &lpTimerManager->arrListTimer1[(lpTimerManager->uJiffies & TVR_MASK)];
    }
    else
    {
        /* If the timeout is larger than 0xffffffff on 64-bit
         * architectures then we use the maximum timeout:
         */
        if (uDueTime > 0xffffffffUL)
        {
            uDueTime = 0xffffffffUL;
            uExpires = uDueTime + lpTimerManager->uJiffies;
        }
        i = (uExpires >> (TVR_BITS + 3 * TVN_BITS)) & TVN_MASK; // i = (expires>>26) & 63
        pHead = &lpTimerManager->arrListTimer5[i];
    }
    ListTimerInsertTail(&pTmr->ltTimer, pHead);
}

// ����ʱ���� arrlistTimer ��˫��ѭ�����������еļ�ʱ�����ݵ���ʱ����뵽ָ����ʱ������
static uint32 CascadeTimer(LPTIMERMANAGER lpTimerManager, struct LIST_TIMER *arrListTimer, uint32 idx)
{
    struct LIST_TIMER listTmr, *pListTimer;
    struct TIMER_NODE *pTmr;

    ListTimerReplaceInit(&arrListTimer[idx], &listTmr);
    pListTimer = listTmr.pNext;
    // ����˫��ѭ��������Ӷ�ʱ��
    while(pListTimer != &listTmr)
    {
        // ���ݽṹ�� struct TIMER_NODE �ĳ�Ա ltTimer ��ָ���ַ�͸ó�Ա�ڽṹ���еı�����������ṹ�� struct TIMER_NODE �ĵ�ַ
       // pTmr = (struct TIMER_NODE *)((uint8 *)pListTimer - offsetof(struct TIMER_NODE, ltTimer));
        pTmr = (struct TIMER_NODE *)(pListTimer);
        pListTimer = pListTimer->pNext;
        AddTimer(lpTimerManager, pTmr);
    }
    return idx;
}

static void RunTimer(LPTIMERMANAGER lpTimerManager)
{
#define INDEX(N) ((lpTimerManager->uJiffies >> (TVR_BITS + (N) * TVN_BITS)) & TVN_MASK)
    uint32 idx, uJiffies;
    struct LIST_TIMER listTmrExpire, *pListTmrExpire;
    struct TIMER_NODE *pTmr;

    if(NULL == lpTimerManager)
        return;
    uJiffies = GetJiffies();
    pthread_mutex_lock(&lpTimerManager->lock);
    while(TIME_AFTER_EQ(uJiffies, lpTimerManager->uJiffies))
    {
        // unint32 �� 32bit��idx Ϊ��ǰʱ��ĵ� 8bit��INDEX(0) Ϊ�� 6bit��INDEX(1) Ϊ�ٴ� 6bit��INDEX(2) Ϊ�ٴ� 6bit��INDEX(3) Ϊ�� 6bit
        // ��� 1 ��ʱ���ֵ� 256 ���������ˣ�������� 2 ��ʱ�����еļ�ʱ���������еļ�ʱ�����ݵ���ʱ����뵽ָ��ʱ���֡����� 1 ��ʱ�����о��м�ʱ���ˡ�
        // ��� 1��2 ��ʱ���ֵ� 256*64 ���붼�����ˣ������ 3 ��ʱ���֣������еļ�ʱ�����뵽ָ��ʱ���֡����� 1 ���� 2 ��ʱ�����о��м�ʱ���ˡ�
        // ��� 1��2��3 ��ʱ���ֵ� 256*64*64 ���붼�����ˣ�...
        // ��� 1��2��3��4 ��ʱ���ֵ� 256*64*64*64 ���붼�����ˣ�...
        idx = lpTimerManager->uJiffies & TVR_MASK;
        if (!idx &&
                (!CascadeTimer(lpTimerManager, lpTimerManager->arrListTimer2, INDEX(0))) &&
                (!CascadeTimer(lpTimerManager, lpTimerManager->arrListTimer3, INDEX(1))) &&
                !CascadeTimer(lpTimerManager, lpTimerManager->arrListTimer4, INDEX(2)))
            CascadeTimer(lpTimerManager, lpTimerManager->arrListTimer5, INDEX(3));
        pListTmrExpire = &listTmrExpire;
        // �½�� pListTmrExpire �滻 arrListTimer1[idx] ��˫��ѭ������ arrListTimer1[idx] ��ֻ�����Լ�һ������ˡ�pListTmrExpire ��Ϊ˫��ѭ����������
        ListTimerReplaceInit(&lpTimerManager->arrListTimer1[idx], pListTmrExpire);
        // ����ʱ���� arrListTimer1 ��˫��ѭ������ִ�и��������ж�ʱ���Ļص�����
        pListTmrExpire = pListTmrExpire->pNext;
        while(pListTmrExpire != &listTmrExpire)
        {
           // pTmr = (struct TIMER_NODE *)((uint8 *)pListTmrExpire - offsetof(struct TIMER_NODE, ltTimer));
            pTmr = (struct TIMER_NODE *)(pListTmrExpire);
            pListTmrExpire = pListTmrExpire->pNext;
            pTmr->cb(pTmr->pParam);
            //
            if(pTmr->uPeriod != 0)
            {
                pTmr->uExpires = lpTimerManager->uJiffies + pTmr->uPeriod;
                AddTimer(lpTimerManager, pTmr);
            }
            else free(pTmr);
        }
        lpTimerManager->uJiffies++;
    }
    pthread_mutex_unlock(&lpTimerManager->lock);
}

// ��ʱ���̡߳��� 1 ����Ϊ��λ���м�ʱ
static void *ThreadRunTimer(void *pParam)
{
    LPTIMERMANAGER pTimerMgr;

    pTimerMgr = (LPTIMERMANAGER)pParam;
    if(pTimerMgr == NULL)
        return NULL;
    while(!pTimerMgr->uExitFlag)
    {
        RunTimer(pTimerMgr);
        SleepMilliseconds(1);  // �߳�˯ 1 ����
    }
    return NULL;
}

// ˯ uMs ����
void SleepMilliseconds(uint32 uMs)
{
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = uMs * 1000;  // tv.tv_usec ��λ��΢��
    select(0, NULL, NULL, NULL, &tv);
}

// ������ʱ��������
LPTIMERMANAGER CreateTimerManager(void)
{
    LPTIMERMANAGER lpTimerMgr = (LPTIMERMANAGER)malloc(sizeof(TIMERMANAGER));
    if(lpTimerMgr != NULL)
    {
        lpTimerMgr->thread = (pthread_t)0;
        lpTimerMgr->uExitFlag = 0;
        pthread_mutex_init(&lpTimerMgr->lock, NULL);
        lpTimerMgr->uJiffies = GetJiffies();
        InitArrayListTimer(lpTimerMgr->arrListTimer1, sizeof(lpTimerMgr->arrListTimer1)/sizeof(lpTimerMgr->arrListTimer1[0]));
        InitArrayListTimer(lpTimerMgr->arrListTimer2, sizeof(lpTimerMgr->arrListTimer2)/sizeof(lpTimerMgr->arrListTimer2[0]));
        InitArrayListTimer(lpTimerMgr->arrListTimer3, sizeof(lpTimerMgr->arrListTimer3)/sizeof(lpTimerMgr->arrListTimer3[0]));
        InitArrayListTimer(lpTimerMgr->arrListTimer4, sizeof(lpTimerMgr->arrListTimer4)/sizeof(lpTimerMgr->arrListTimer4[0]));
        InitArrayListTimer(lpTimerMgr->arrListTimer5, sizeof(lpTimerMgr->arrListTimer5)/sizeof(lpTimerMgr->arrListTimer5[0]));
        lpTimerMgr->thread = ThreadCreate(ThreadRunTimer, lpTimerMgr);
    }
    return lpTimerMgr;
}

// ɾ����ʱ��������
void DestroyTimerManager(LPTIMERMANAGER lpTimerManager)
{
    if(NULL == lpTimerManager)
        return;
    lpTimerManager->uExitFlag = 1;
    if((pthread_t)0 != lpTimerManager->thread)
    {
        ThreadJoin(lpTimerManager->thread);
        ThreadDestroy(lpTimerManager->thread);
        lpTimerManager->thread = (pthread_t)0;
    }
    DeleteArrayListTimer(lpTimerManager->arrListTimer1, sizeof(lpTimerManager->arrListTimer1)/sizeof(lpTimerManager->arrListTimer1[0]));
    DeleteArrayListTimer(lpTimerManager->arrListTimer2, sizeof(lpTimerManager->arrListTimer2)/sizeof(lpTimerManager->arrListTimer2[0]));
    DeleteArrayListTimer(lpTimerManager->arrListTimer3, sizeof(lpTimerManager->arrListTimer3)/sizeof(lpTimerManager->arrListTimer3[0]));
    DeleteArrayListTimer(lpTimerManager->arrListTimer4, sizeof(lpTimerManager->arrListTimer4)/sizeof(lpTimerManager->arrListTimer4[0]));
    DeleteArrayListTimer(lpTimerManager->arrListTimer5, sizeof(lpTimerManager->arrListTimer5)/sizeof(lpTimerManager->arrListTimer5[0]));
    pthread_mutex_destroy(&lpTimerManager->lock);
    free(lpTimerManager);
}

// ����һ����ʱ����fnTimer �ص�������ַ��pParam �ص������Ĳ�����uDueTime �״δ����ĳ�ʱʱ������uPeriod ��ʱ��ѭ�����ڣ���Ϊ0����ö�ʱ��ֻ����һ�Ρ�
LPTIMERNODE CreateTimer(LPTIMERMANAGER lpTimerManager, const TimerCallback& cb, void *pParam, uint32 uDueTime, uint32 uPeriod)
{
    LPTIMERNODE pTmr = NULL;
    if(NULL == cb || NULL == lpTimerManager)
        return NULL;
    pTmr = (LPTIMERNODE)malloc(sizeof(TIMERNODE));
    if(pTmr != NULL)
    {
        pTmr->uPeriod = uPeriod;
        pTmr->cb = cb;
        pTmr->pParam = pParam;

        pthread_mutex_lock(&lpTimerManager->lock);
        pTmr->uExpires = lpTimerManager->uJiffies + uDueTime;
        AddTimer(lpTimerManager, pTmr);
        pthread_mutex_unlock(&lpTimerManager->lock);
    }
    return pTmr;
}

//ɾ����ʱ��
int32 DeleteTimer(LPTIMERMANAGER lpTimerManager, LPTIMERNODE lpTimer)
{
    struct LIST_TIMER *pListTmr;
    if(NULL != lpTimerManager && NULL != lpTimer)
    {
        pthread_mutex_lock(&lpTimerManager->lock);
        pListTmr = &lpTimer->ltTimer;
        pListTmr->pPrev->pNext = pListTmr->pNext;
        pListTmr->pNext->pPrev = pListTmr->pPrev;
        free(lpTimer);
        pthread_mutex_unlock(&lpTimerManager->lock);
        return 0;
    }
    else
        return -1;
}
