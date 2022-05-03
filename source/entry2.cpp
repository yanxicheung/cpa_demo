#include "OSSDef.h"
#include "OSS.h"
#include <iostream>
#include "movie.pb.h"
using namespace std;
using namespace movie;

namespace
{
    void fillBaseInfo(BasicInfo* basicInfo)
    {
        basicInfo->set_movie_type(MovieType::science);
        basicInfo->add_directors("Daniel");
        basicInfo->add_directors("Jack");
    }

    void fillScore(Score* score, uint32_t value,const string& comment)
    {
        score->set_value(value);
        score->set_comment(comment);
    }

    void sendMovieInfoResponse(const string&name)
    {
        Message msg;
        MovieInfoResponse* rsp = msg.mutable_movie_info_response();
        rsp->set_name(name);

        fillBaseInfo(rsp->mutable_basic_info());
        fillScore(rsp->add_score_items(),100,"Good");
        fillScore(rsp->add_score_items(),99,"Perfect");

        cout << "=======sendMovieInfoResponse=========" << endl;
        cout << msg.DebugString() << endl;

        string str;
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

