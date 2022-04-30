#include "OSS.h"
#include <iostream>
#include <string>
#include "movie.pb.h"
#include "OSS_Timer.h"

using namespace std;
using namespace movie;

namespace
{
    void sendMovieInfoRequest()
    {
        Message msg;
        MovieInfoRequest * req = msg.mutable_movie_info_request();
        req->set_name("Judy");
        string str;
        cout << "=======sendMovieInfoRequest=========" << endl;
        cout << msg.DebugString() << endl;
        msg.SerializeToString(&str);
        OSS_Send("entry2", msg.Messages_case(), str.c_str(),
        msg.ByteSizeLong());
    }
}

void DemoEntry1(int state, int eventid, void *msg, int msgLen, void* data)
{
    if (eventid == EV_STARTUP)
    {
        cout << "DemoEntry1 Power on" << endl;
        sendMovieInfoRequest();
        OSS_SetLoopTimer(TIMER_NO_3, 1000);
    }
    else if(eventid == EV_TIMER_3)
    {
        cout << "DemoEntry1 EV_TIMER_3:" << eventid << endl;
    }
    else
    {
        cout << "DemoEntry1 recv unknow event " << eventid << endl;
    }
}
