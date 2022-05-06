#ifndef HA05DA7BE_0E45_4EF2_83D3_E75A399B38F9
#define HA05DA7BE_0E45_4EF2_83D3_E75A399B38F9

#include <stdint.h>
#include "Callbacks.h"
#include <memory>

typedef std::shared_ptr<void> Handle;

struct ObserverConfig
{
    EntryCallback entry;
    const char* instKey;   // 内部通信使用的key
};

////////////////////////////oss event///////////////////////////////////////

#define    EV_OSS_BEGIN      (uint16_t)(1000)
#define    EV_STARTUP        (uint16_t)(EV_OSS_BEGIN + 1)

////////////////////////////timer event////////////////////////////////////

#define   TIMER_NO_1         (uint8_t)(1)
#define   TIMER_NO_2         (uint8_t)(2)
#define   TIMER_NO_3         (uint8_t)(3)
#define   TIMER_NO_4         (uint8_t)(4)
#define   TIMER_NO_5         (uint8_t)(5)
#define   TIMER_NO_6         (uint8_t)(6)
#define   TIMER_NO_7         (uint8_t)(7)
#define   TIMER_NO_8         (uint8_t)(8)

#define    EV_TIMER_BEGIN    (uint16_t)(500 + EV_OSS_BEGIN)  /*1500*/
#define    EV_TIMER_1        (uint16_t)(EV_TIMER_BEGIN + TIMER_NO_1)
#define    EV_TIMER_2        (uint16_t)(EV_TIMER_BEGIN + TIMER_NO_2)
#define    EV_TIMER_3        (uint16_t)(EV_TIMER_BEGIN + TIMER_NO_3)
#define    EV_TIMER_4        (uint16_t)(EV_TIMER_BEGIN + TIMER_NO_4)
#define    EV_TIMER_5        (uint16_t)(EV_TIMER_BEGIN + TIMER_NO_5)
#define    EV_TIMER_6        (uint16_t)(EV_TIMER_BEGIN + TIMER_NO_6)
#define    EV_TIMER_7        (uint16_t)(EV_TIMER_BEGIN + TIMER_NO_7)
#define    EV_TIMER_8        (uint16_t)(EV_TIMER_BEGIN + TIMER_NO_8)

#endif /* HA05DA7BE_0E45_4EF2_83D3_E75A399B38F9 */
