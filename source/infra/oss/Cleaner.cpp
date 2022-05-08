#include "Cleaner.h"

Cleaner::Cleaner():
thread_(std::bind(&Cleaner::clean,this))
{
    thread_.start();
}

void Cleaner::addUnregistHandle(Handle& handle)
{
    handles_.put(std::move(handle));
}

void Cleaner::clean()
{
    while(true)
    {
        Handle handle = handles_.take();
        handle.reset();
        cout<<"=====clean======"<<endl;
    }
}
