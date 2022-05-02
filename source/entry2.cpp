#include "OSSDef.h"
#include "OSS.h"
#include <iostream>
#include "movie.pb.h"
using namespace std;
using namespace movie;

namespace
{
    void sendMovieInfoResponse(const string&name)
    {
        Message msg;
        MovieInfoResponse* rsp = msg.mutable_movie_info_response();
        rsp->set_name(name);

        auto basicInfo = rsp->mutable_basic_info();
        basicInfo->set_movie_type(MovieType::other);

        auto directors = basicInfo->mutable_directors();
        directors->Add("foo1");
        directors->Add("foo2");

        string str;
        cout << "=======sendMovieInfoResponse=========" << endl;
        cout << msg.DebugString() << endl;
        msg.SerializeToString(&str);
        OSS_Send("entry1", msg.Messages_case(), str.c_str(), msg.ByteSizeLong());
    }

    void parseMoveInfoRequest(const string& str)
    {
        Message msg;
        msg.ParseFromString(str);
        MovieInfoRequest req = msg.movie_info_request();
        cout << req.name() << endl;
        sendMovieInfoResponse(req.name());
    }
}

void DemoEntry2(int state, int eventid, void *msg, int msgLen, void* data)
{
    if (eventid == EV_STARTUP)
    {
        cout << "DemoEntry2 Power on" << endl;
    }
    else if (eventid == Message::kMovieInfoRequest)
    {
        cout << "DemoEntry2 recv MoveInfoRequest" << endl;
        string str((const char*) msg);
        parseMoveInfoRequest(str);
    }
    else
    {
        cout << "DemoEntry2 recv unknow event " << eventid << endl;
    }
}

