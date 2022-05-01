#include "CtrlFoo.h"
#include <../include/infra/oss/OSSDef.h>
#include "OSS.h"
#include <iostream>
#include "movie.pb.h"
using namespace std;
using namespace movie;

namespace
{
    void parseMoveInfoRequest(const string& str)
    {
        Message msg;
        msg.ParseFromString(str);
        MovieInfoRequest req = msg.movie_info_request();
        cout << req.name() << endl;
    }
}

void CtrlFoo::Entry(int state, int eventid, void* msg, int msgLen, void* data)
{
    if (eventid == EV_STARTUP)
    {
        cout << "CtrlFoo Power on" << endl;
        OSS_SetRelativeTimer(TIMER_NO_3, 2000);
    }
    else if (eventid == Message::kMovieInfoRequest)
    {
        cout << "CtrlFoo recv MoveInfoRequest" << endl;
        string str((const char*) msg);
        parseMoveInfoRequest(str);

        OSS_Send("entry1", 100, nullptr, 0);
    }
    else if(eventid == EV_TIMER_3)
    {
        cout << "CtrlFoo OSS_SetRelativeTimer[2000ms]:" <<eventid <<endl;
        OSS_SetRelativeTimer(TIMER_NO_3, 1500);
    }
    else
    {
        cout << "CtrlFoo recv unknow event " << eventid << endl;
    }
}
