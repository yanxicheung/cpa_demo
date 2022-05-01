#ifndef INCLUDE_INFRA_BASE_CALLBACKS_H_
#define INCLUDE_INFRA_BASE_CALLBACKS_H_

#include <functional>
#include <stdint.h>
#include <pthread.h>
using std::function;

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;
using std::placeholders::_5;

typedef function<void (int state, int eventid, void *msg, int msgLen, void* data)> EntryCallback;
typedef function<void (void* pParam)> TimerCallback;
typedef function<void (const pthread_t& threadId, uint16_t timeOutEvent)> SendTimeOutCallback;

#endif /* INCLUDE_INFRA_BASE_CALLBACKS_H_ */
