#include "OSSDef.h"
#include "OSS.h"
#include <iostream>
#include <string>
#include "movie.pb.h"

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

    void parseMoveInfoResponse(const string& str)
    {
        Message msg;
        msg.ParseFromString(str);
        cout << msg.DebugString() << endl;
        MovieInfoResponse rsp = msg.movie_info_response();
    }
}

void DemoEntry1(int state, int eventid, void *msg, int msgLen, void* data)
{
    if (eventid == EV_STARTUP)
    {
        cout << "DemoEntry1 Power on" << endl;
        sendMovieInfoRequest();
    }
    else if(eventid == Message::kMovieInfoResponse)
    {
        cout << "DemoEntry1 recv MovieInfoResponse" << endl;
        string str((const char*) msg);
        parseMoveInfoResponse(str);
    }
    else
    {
        cout << "DemoEntry1 recv unknow event " << eventid << endl;
    }
}
