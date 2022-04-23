#ifndef H27857A6C_DBA2_47F9_8204_EF2E7B65A0C9
#define H27857A6C_DBA2_47F9_8204_EF2E7B65A0C9
#include<stdint.h>
#include<functional>
using std::function;

typedef function<void (int state, int eventid, void *msg, int msgLen, void* data)> Entry;

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

void OSS_KillTimer(uint8_t timerId, uint32_t param);

void OSS_SetLoopTimer(uint8_t timerId, uint32_t timerLen, uint32_t param);

void OSS_SetRelativeTimer(uint8_t timerId, uint32_t timerLen, uint32_t param);

bool OSS_ValidTimer(uint8_t timerId);

#endif /* H27857A6C_DBA2_47F9_8204_EF2E7B65A0C9 */
