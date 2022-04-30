#ifndef INCLUDE_INFRA_BASE_CALLBACKS_H_
#define INCLUDE_INFRA_BASE_CALLBACKS_H_

#include<functional>
using std::function;

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;
using std::placeholders::_5;

typedef function<void (int state, int eventid, void *msg, int msgLen, void* data)> Entry;
typedef function<void (void*)> TimerCallback;

#endif /* INCLUDE_INFRA_BASE_CALLBACKS_H_ */
