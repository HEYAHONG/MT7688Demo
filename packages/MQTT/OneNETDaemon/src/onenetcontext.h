#ifndef ONENETCONTEXT_H
#define ONENETCONTEXT_H
#ifdef __cplusplus
/*
Context主要提供执行上下文。
所有非MQTT触发的事件均由Context类所在线程执行。
*/
#include <thread>
#include <mutex>
#include <queue>
#include <map>
#include <functional>
#include <stdint.h>

class OneNETMQTT;
class OneNETDevice;

class OneNETContext
{
public:
    OneNETContext();
    virtual ~OneNETContext();

    //上下文锁
    std::mutex &GetContextLock();

    //运行上下文（一般不由用户调用 ，且此函数一般不会退出）
    void Run();

    //添加动作（注意：不能在MQTT回调或者上下文回调中使用此函数）
    void AddAction(std::function<void()> action);

    /*
    * 事件ID(32位)由事件类（高16位）与实例ID（低16位）组成
    */
    typedef uint32_t EventID;
    typedef uint16_t EventClassID;
    typedef enum
    {
        CONTEXT_START_CLASS=0,
        CONTEXT_LOOP_CLASS,
        CONTEXT_MQTT_CONNECTED_CLASS,
        CONTEXT_MQTT_DISCONNECTED_CLASS,
        CUSTOM_CLASS_START,
        UNKOWN_CLASS=0xFFFF
    } EventClass;
    //注册事件回调
    EventID RegisterEvent(EventClass evtclass,std::function<void()> callback);
    EventID RegisterEvent(EventClassID evtclass,std::function<void()> callback);
    //反注册事件
    void UnRegisterEvent(EventID evtid);

    //设置OneNETMQTT
    void SetOneNETMQTT(OneNETMQTT *_mqtt);

    //设置OneNETDevice
    void SetOneNETDevice(OneNETDevice *_device);


private:
    //添加动作(无锁,仅供内部使用)
    void AddActionWithoutLock(std::function<void()> action);

    //触发事件
    void EmitEventWithoutLock(EventClass evtclass);
    void EmitEventWithoutLock(EventClassID evtclass);

    //上下文锁
    std::mutex context_lock;

    //动作队列
    std::queue<std::function<void()>> action_queue;

    //事件表
    std::mutex event_map_lock;
    std::map<EventID,std::function<void()>> event_map;

    //OneNETMQTT
    OneNETMQTT *mqtt;
    bool mqtt_is_connected;

    //OneNETDevice
    OneNETDevice *device;

};

OneNETContext & OneNETContextDefault();

void OneNETContextInit();

#endif // __cplusplus
#endif // ONENETCONTEXT_H
