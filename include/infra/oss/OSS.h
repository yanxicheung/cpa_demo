#ifndef H27857A6C_DBA2_47F9_8204_EF2E7B65A0C9
#define H27857A6C_DBA2_47F9_8204_EF2E7B65A0C9

#include "OSSDef.h"

void OSS_Init();

Handle OSS_UserRegist(const ObserverConfig& config);

void OSS_UserUnregist(Handle& handle);

void OSS_Send(const char* instKey, int eventId, const void* msg, int msgLen);

void OSS_SetLoopTimer(uint8_t timerNo, uint32_t duration);

void OSS_SetRelativeTimer(uint8_t timerNo, uint32_t duration);

#endif /* H27857A6C_DBA2_47F9_8204_EF2E7B65A0C9 */
