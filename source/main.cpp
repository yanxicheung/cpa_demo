#include "OSS.h"
#include <iostream>
#include <unistd.h>
#include <functional>
#include "CtrlFoo.h"
using namespace std;

extern void DemoEntry1(int state, int eventid, void *msg, int msgLen, void* data);
extern void DemoEntry2(int state, int eventid, void *msg, int msgLen, void* data);

int main(int argc, char **argv)
{
    cout<<"Hello cpa demo"<<endl;
    OSS_Init();

    shared_ptr<CtrlFoo> pCtrlFooObj(new CtrlFoo);
    auto fooCb = std::bind(&CtrlFoo::Entry, pCtrlFooObj, _1, _2, _3, _4, _5);
    const ObserverConfig userConfigs[] =
    {
        { DemoEntry1, "entry1"},
        { DemoEntry2, "entry2"},
        { fooCb, "foo"}
    };


    Handle h1 = OSS_UserRegist(userConfigs[0]);
    Handle h2 = OSS_UserRegist(userConfigs[1]);
    Handle h3 = OSS_UserRegist(userConfigs[2]);

//    OSS_UserRegist(userConfigs[0]);
//    OSS_UserRegist(userConfigs[1]);
//    OSS_UserRegist(userConfigs[2]);
    int cnt = 0;
    while (1)
    {
        cnt++;
        sleep(1);
        if(cnt == 30)  // release
        {
            h1.reset();
            h2.reset();
            h3.reset();
        }
    }
    return 0;
}
