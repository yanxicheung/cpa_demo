# cpa_demo
`cpa_demo`是一个面向控制面、管理面的业务框架。代码物理设计如下：

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

 `build.sh`为一键编译脚本。

示例中使用`protobuf`来演示消息的发送和处理，如需运行`demo`请安装`protobuf`。

`movie.proto`为`protobuf`消息定义文件，`movie.pb.h`和`movie.pb.cc`由`protobuf`编译器自动生成。



框架代码位于`infra`目录下，其中`base`目录下提供了系统编程常用的工具，包括队列、条件变量、互

斥锁、线程、定时器。



`oss`目录下的内容基于`base`提供的常用工具，对上层业务提供如下接口：

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



`main.cpp`演示了消息入口的注册，消息入口要满足如下形式：

```cpp
typedef function<void (int state, int eventid, void *msg, int msgLen, void* data)> EntryCallback;
```



支持两种风格消息入口注册：

1. C风格接口。
2. 类的成员函数。

```cpp
    CtrlFoo ctrlFooObj;
    const ObserverConfig userConfigs[] =
    {
        {E_DemoEntry1, DemoEntry1, 100, "entry1"},
        {E_DemoEntry2, std::bind(&CtrlFoo::Entry, &ctrlFooObj, _1, _2, _3, _4, _5), 100, "entry2"}
    };

    OSS_UserRegist((ObserverConfig*)userConfigs, sizeof(userConfigs)/ sizeof(ObserverConfig));
```



消息发送使用：

`void OSS_Send(const char* instKey, int eventId, const void* msg, int msgLen);`

其中`instKey`作为接收消息的标识，在消息入口注册时指定。