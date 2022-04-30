#include "OSS_Timer.h"

#define BEGIN_GET_EVENT_ID    switch(timerId) {

#define GET_TIMER_EVENT(no)   case TIMER_NO_##no: return EV_TIMER_##no;

#define END_GET_EVENT_ID      default: return 0xFFFF;}

uint16_t getTimeoutEvent(uint8_t timerId)
{
    BEGIN_GET_EVENT_ID
        GET_TIMER_EVENT(1)
        GET_TIMER_EVENT(2)
        GET_TIMER_EVENT(3)
        GET_TIMER_EVENT(4)
        GET_TIMER_EVENT(5)
        GET_TIMER_EVENT(6)
        GET_TIMER_EVENT(7)
        GET_TIMER_EVENT(8)
    END_GET_EVENT_ID
}
