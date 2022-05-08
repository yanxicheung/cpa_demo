# 概述：

`cpa_demo`是一个面向控制面、管理面的业务框架。

`cpa_demo`使用单进程多线程模型，一个业务模块在代码上对应一个`entry`，从程序运行的角度，对应一个线程。

各个业务模块之间的通信使用消息队列，每个业务模块都有一个`instkey`，作为模块间通信的标识。

`cpa_demo`适合使用消息驱动的应用场景。

进程间的通信目前暂时没有支持，优先考虑使用`TCP`或者`HTTP`。



# 代码结构：

`cpa_demo`的物理设计如下：

```shell
.
├── build.sh
├── CMakeLists.txt
├── include
│   ├── CtrlFoo.h
│   ├── infra
│   │   ├── base
│   │   │   ├── BlockingQueue.h
│   │   │   ├── Callbacks.h
│   │   │   ├── Condition.h
│   │   │   ├── CountDownLatch.h
│   │   │   ├── Mutex.h
│   │   │   ├── Noncopyable.h
│   │   │   ├── Thread.h
│   │   │   └── TimerWheel.h
│   │   ├── log
│   │   └── oss
│   │       ├── Cleaner.h
│   │       ├── Executor.h
│   │       ├── Msg.h
│   │       ├── MsgQueue.h
│   │       ├── OSSDef.h
│   │       ├── OSS.h
│   │       └── OSSTimer.h
│   └── pb
│       ├── movie.pb.h
│       └── movie.proto
├── README.md
└── source
    ├── CtrlFoo.cpp
    ├── entry1.cpp
    ├── entry2.cpp
    ├── infra
    │   ├── base
    │   │   ├── Thread.cpp
    │   │   └── TimerWheel.cpp
    │   ├── log
    │   └── oss
    │       ├── Cleaner.cpp
    │       ├── Executor.cpp
    │       ├── Msg.cpp
    │       ├── MsgQueue.cpp
    │       ├── OSS.cpp
    │       └── OSSTimer.cpp
    ├── main.cpp
    └── pb
        └── movie.pb.cc
```

 `build.sh`为一键编译、运行脚本。

框架代码位于`infra`目录下，其中`base`目录下提供了系统编程常用的工具，包括队列、条件变量、互斥锁、线程、定时器、`coutdownLatch`。



# OSS接口：

上层业务模块使用`OSS`只需要包含`OSS.h` 

`OSS`提供的接口，具备线程安全性。对上层业务提供如下接口：



## OSS_Init

```cpp
void OSS_Init(); // 系统初始化
```

`OSS`系统初始化，在最开始的时候调用一次。



## OSS_UserRegist

```cpp
struct ObserverConfig
{
    EntryCallback entry;
    const char* instKey;   // 内部通信使用的key
};

Handle OSS_UserRegist(const ObserverConfig& config);
```

将业务模块注册到`OSS`，注册后`OSS`会为业务模块分配消息队列、线程资源。

返回句柄`Handle`，`Handle`代表`OSS`为业务模块分配的资源。

**Handle值一定要用一个变量保存**，否则分配的资源将被自动释放，无法使用。

手动释放资源调用`OSS_UserUnregist`。



一个业务模块的消息入口要满足如下形式：

```cpp
typedef function<void (int state, int eventid, void *msg, int msgLen, void* data)> EntryCallback;
```

支持两种风格消息入口的注册：

1. C语言风格接口。
2. 类的成员函数。



使用示例：

```cpp
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
```



## OSS_UserUnregist

```cpp
void OSS_UserUnregist(Handle& handle);
```

参数值为`OSS_UserRegist`的返回值，调用后由`OSS`释放`handle`对应的资源（线程、消息队列）。



## OSS_Send

```cpp
void OSS_Send(const char* instKey, int eventId, const void* msg, int msgLen);
```

发送消息接口，其中`instKey`作为对端模块标标识，在消息入口注册时指定。



## OSS_SetLoopTimer

```cpp
void OSS_SetLoopTimer(uint8_t timerNo, uint32_t duration);
```

设置循环定时器，`timerNo`为定时器标识，`duration`为定时器长度。

`timerNo`对应的定时器超时后，会发送一条超时消息，`timerNo`和超时消息的映射关系见`OSSDef.h`



## OSS_SetRelativeTimer

```cpp
void OSS_SetRelativeTimer(uint8_t timerNo, uint32_t duration);
```

设置相对定时器，仅被触发一次，`timerNo`为定时器标识，`duration`为定时器长度。



# 其他：

示例中，模块`entry1`和`entry2`使用`protobuf`演示了消息的发送和处理，如需运行`demo`请安装`protobuf`。

`movie.proto`为`protobuf`消息定义文件，`movie.pb.h`和`movie.pb.cc`由`protobuf`编译器自动生成。

模块`CtrlFoo`演示了定时器的使用。

