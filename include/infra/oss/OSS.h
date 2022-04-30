#ifndef H27857A6C_DBA2_47F9_8204_EF2E7B65A0C9
#define H27857A6C_DBA2_47F9_8204_EF2E7B65A0C9
#include <stdint.h>
#include "Callbacks.h"

enum OSS_EV
{
    EV_STARTUP
};

struct ThreadConfig
{
    uint16_t threadType;
    EntryCallback entry;
    uint32_t dataRegionSize;
    const char* instKey;   // 内部通信使用的key
};

void OSS_Init();

void OSS_UserRegist(ThreadConfig* configs, uint16_t size);

void OSS_Send(const char* instKey, int eventId, const void* msg, int msgLen);

void OSS_SetLoopTimer(uint8_t timerNo, uint32_t duration);

void OSS_SetRelativeTimer(uint8_t timerNo, uint32_t duration);

#endif /* H27857A6C_DBA2_47F9_8204_EF2E7B65A0C9 */
