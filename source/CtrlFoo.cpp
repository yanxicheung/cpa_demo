#include "CtrlFoo.h"
#include "OSSDef.h"
#include "OSS.h"
#include <iostream>
#include "movie.pb.h"
using namespace std;
using namespace movie;

void CtrlFoo::Entry(int state, int eventid, void* msg, int msgLen, void* data)
{
    if (eventid == EV_STARTUP)
    {
        cout << "CtrlFoo Power on" << endl;
        OSS_SetRelativeTimer(TIMER_NO_3, 2000);
    }
    else if(eventid == EV_TIMER_3)
    {
        cout << "CtrlFoo recv EV_TIMER_3 [2000ms]:" <<eventid <<endl;
        OSS_SetLoopTimer(TIMER_NO_6, 1500);
    }
    else if(eventid == EV_TIMER_6)
    {
        cout << "CtrlFoo recv EV_TIMER_6 [1500ms]:" <<eventid <<endl;
    }
    else
    {
        cout << "CtrlFoo recv unknow event " << eventid << endl;
    }
}
