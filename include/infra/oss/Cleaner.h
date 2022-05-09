#ifndef H7554811F_2870_45E5_9A39_F37B3AA9C6EF
#define H7554811F_2870_45E5_9A39_F37B3AA9C6EF
#include "BlockingQueue.h"
#include "OSSDef.h"
#include "Thread.h"

class Cleaner: Noncopyable
{
public:
    Cleaner();
    void addUnregistHandle(Handle& handle);
private:
    void clean();
private:
    BlockingQueue<Handle> handles_;
    Thread thread_;
};

#endif /* H7554811F_2870_45E5_9A39_F37B3AA9C6EF */
