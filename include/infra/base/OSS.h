#ifndef H27857A6C_DBA2_47F9_8204_EF2E7B65A0C9
#define H27857A6C_DBA2_47F9_8204_EF2E7B65A0C9
#include<stdint.h>
#include "Callbacks.h"

const uint8_t MAX_INST_KEY_LEN = 32;

enum OSS_EV
{
    EV_STARTUP
};

struct ThreadConfig
{
    uint16_t threadType;
    Entry entry;
    uint32_t dataRegionSize;
    const char* instKey;   // 内部通信使用的key
};

void OSS_Init();

void OSS_UserEntryRegist(ThreadConfig* configs, uint16_t size);

void OSS_Send(const char* instKey, int eventId, const void* msg, int msgLen);

void OSS_SetLoopTimer(uint32_t timerLen, uint32_t param, const TimerCallback& cb);

void OSS_SetRelativeTimer(uint32_t timerLen, uint32_t param, const TimerCallback& cb);

#endif /* H27857A6C_DBA2_47F9_8204_EF2E7B65A0C9 */
