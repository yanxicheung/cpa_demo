#include "OSS.h"
#include <iostream>
#include <unistd.h>
#include <functional>
#include "CtrlFoo.h"
using namespace std;

extern void DemoEntry1(int state, int eventid, void *msg, int msgLen, void* data);

enum ThreadType
{
    E_DemoEntry1,
    E_DemoEntry2
};

int main(int argc, char **argv)
{
    cout<<"Hello cpa demo"<<endl;
    OSS_Init();

    CtrlFoo ctrlFooObj;
    const ThreadConfig userConfigs[] =
    {
        {E_DemoEntry1, DemoEntry1, 100, "entry1"},
        {E_DemoEntry2, std::bind(&CtrlFoo::Entry, &ctrlFooObj, _1, _2, _3, _4, _5), 100, "entry2"}
    };

    OSS_UserRegist((ThreadConfig*)userConfigs, sizeof(userConfigs)/ sizeof(ThreadConfig));
    while (1)
    {
        sleep(1);
    }
    return 0;
}
