#include "OSS.h"
#include <iostream>
#include <string>
#include "movie.pb.h"
#include "OSS_Timer.h"
using namespace std;
using namespace movie;

#define FOO_TIMER(name)                FOO_##name##_TIMER
#define FOO_TIMER_DEFAULT_LEN(name)    FOO_##name##_TIMER_DEFAULT_LEN
#define FOO_TIMER_EV(name)             EV_FOO_##name##_TIMEOUT

#define DEF_TIMER(name, no, len)  \
const uint8_t  FOO_TIMER(name)             = TIMER_NO_##no; \
const uint16_t FOO_TIMER_EV(name)          = EV_TIMER_##no; \
const uint32_t FOO_TIMER_DEFAULT_LEN(name) = len;

DEF_TIMER(DDDD, 1, 100)

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
    }
    else
    {
        cout << "DemoEntry1 recv unknow event " << eventid << endl;
    }
}
