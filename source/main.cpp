#include "OSS.h"
#include <iostream>
#include <unistd.h>
#include <functional>
#include "CtrlFoo.h"
using namespace std;

extern void DemoEntry1(int state, int eventid, void *msg, int msgLen, void* data);
extern void DemoEntry2(int state, int eventid, void *msg, int msgLen, void* data);

enum ThreadType
{
    E_Entry1,
    E_Entry2,
    E_FOOEntry
};

int main(int argc, char **argv)
{
    cout<<"Hello cpa demo"<<endl;
    OSS_Init();

    CtrlFoo ctrlFooObj;
    auto fooCb = std::bind(&CtrlFoo::Entry, &ctrlFooObj, _1, _2, _3, _4, _5);
    const ObserverConfig userConfigs[] =
    {
        {E_Entry1, DemoEntry1, 100, "entry1"},
        {E_Entry2, DemoEntry2, 100, "entry2"},
        {E_FOOEntry, fooCb, 100, "foo"}
    };

    int size = sizeof(userConfigs)/ sizeof(ObserverConfig);
    OSS_UserRegist((ObserverConfig*)userConfigs, size);
    while (1)
    {
        sleep(1);
    }
    return 0;
}
