# 概述：

`cpa_demo`是一个面向控制面、管理面的业务框架。

`cpa_demo`使用单进程多线程模型，一个业务模块在代码上对应一个`entry`，从程序运行的角度，对应一个线程。

各个业务模块之间的通信使用消息队列，每个业务模块都有一个`instkey`，作为模块间通信的标识。

`cpa_demo`适合使用消息驱动的应用场景。

进程间的通信目前暂时没有支持，优先考虑使用`TCP`或者`HTTP`。



# 代码结构：

`cpa_demo`的物理设计如下：

```shell
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
    ├── infra
    │   ├── base
    │   │   ├── Thread.cpp
    │   │   └── TimerWheel.cpp
    │   ├── log
    │   └── oss
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



框架代码位于`infra`目录下，其中`base`目录下提供了系统编程常用的工具，包括队列、条件变量、互

斥锁、线程、定时器、倒计时。`oss`目录下的内容基于`base`提供的常用工具，对上层业务提供如下接口：

```cpp
// 系统初始化
void OSS_Init();
// 回调函数注册，用于接收消息
void OSS_UserRegist(ObserverConfig* configs, uint16_t size);
// 消息发送
void OSS_Send(const char* instKey, int eventId, const void* msg, int msgLen);
// 循环定时器
void OSS_SetLoopTimer(uint8_t timerNo, uint32_t duration);
// sigle-shot timer
void OSS_SetRelativeTimer(uint8_t timerNo, uint32_t duration);
```



一个业务模块的消息入口要满足如下形式：

```cpp
typedef function<void (int state, int eventid, void *msg, int msgLen, void* data)> EntryCallback;
```



支持两种风格消息入口的注册：

1. C语言风格接口。
2. 类的成员函数。

```cpp
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
```



消息发送使用：

`void OSS_Send(const char* instKey, int eventId, const void* msg, int msgLen);`

其中`instKey`作为接收消息的标识，在消息入口注册时指定。



# 示例：

示例中，模块`entry1`和`entry2`使用`protobuf`演示了消息的发送和处理，如需运行`demo`请安装`protobuf`。

`movie.proto`为`protobuf`消息定义文件，`movie.pb.h`和`movie.pb.cc`由`protobuf`编译器自动生成。



模块`CtrlFoo`演示了定时器的使用。

