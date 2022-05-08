#include "OSS.h"
#include <iostream>
#include <unistd.h>
#include <functional>
#include "CtrlFoo.h"
using namespace std;

extern void DemoEntry1(int state, int eventid, void *msg, int msgLen, void* data);
extern void DemoEntry2(int state, int eventid, void *msg, int msgLen, void* data);

Handle regist(const char*key, const EntryCallback& cb)
{
    ObserverConfig config =
    {
        .entry = cb,
        .instKey = key
    };
    return OSS_UserRegist(config);
}


int main(int argc, char **argv)
{
    cout<<"Hello cpa demo"<<endl;
    OSS_Init();

    Handle h1 = regist("entry1", DemoEntry1);
    Handle h2 = regist("entry2", DemoEntry2);
    Handle h3 = regist("foo", std::bind(&CtrlFoo::Entry, make_shared<CtrlFoo>(), _1, _2, _3, _4, _5));
    int cnt = 0;
    while (1)
    {
        cnt++;
        sleep(1);
        if(cnt == 10)  // release
        {
            OSS_UserUnregist(h1);
            OSS_UserUnregist(h2);
            OSS_UserUnregist(h3);
        }
    }
    return 0;
}
