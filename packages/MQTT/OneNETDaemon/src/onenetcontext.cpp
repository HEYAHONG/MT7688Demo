#include "onenetcontext.h"
#include "onenetmqtt.h"
#include "onenetdevice.h"
#include "log.h"
static const char *TAG="OneNET Context";

OneNETContext::OneNETContext():mqtt(NULL),mqtt_is_connected(false)
{

}
OneNETContext::~OneNETContext()
{

}

std::mutex &OneNETContext::GetContextLock()
{
    return context_lock;
}

void OneNETContext::Run()
{
    //触发循环开始事件
    EmitEventWithoutLock(CONTEXT_START_CLASS);
    while(true)
    {
        //触发循环事件
        EmitEventWithoutLock(CONTEXT_LOOP_CLASS);
        //处理MQTT状态
        {
            OneNETMQTT *current_mqtt=mqtt;
            if(current_mqtt!=NULL)
            {
                bool is_connected=current_mqtt->IsConnected();
                if(is_connected!=mqtt_is_connected)
                {
                    //连接状态改变
                    if(is_connected)
                    {
                        EmitEventWithoutLock(CONTEXT_MQTT_CONNECTED_CLASS);
                    }
                    else
                    {
                        EmitEventWithoutLock(CONTEXT_MQTT_DISCONNECTED_CLASS);
                    }
                    mqtt_is_connected=is_connected;
                }
            }
        }
        {
            //动作缓存
            std::lock_guard<std::mutex> lock(action_queue_cache_lock);
            while(!action_queue_cache.empty())
            {
                action_queue.push(action_queue_cache.front());
                action_queue_cache.pop();
            }
        }
        //处理动作
        if(!action_queue.empty())
        {
            while(!action_queue.empty())
            {
                auto action=action_queue.front();
                {
                    std::lock_guard<std::mutex> lock(context_lock);
                    action_queue.pop();
                    if(action!=NULL)
                    {
                        try
                        {
                            action();
                        }
                        catch(...)
                        {

                        }

                    }
                }
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
        std::this_thread::yield();
    }
}

void OneNETContext::AddAction(std::function<void()> action)
{
    std::lock_guard lock(context_lock);
    AddActionWithoutLock(action);
}

void OneNETContext::AddActionWithCache(std::function<void()> action)
{
    std::lock_guard<std::mutex> lock(action_queue_cache_lock);
    action_queue.push(action);
}

OneNETContext::EventID OneNETContext::RegisterEvent(EventClass evtclass,std::function<void()> callback)
{
    return RegisterEvent(static_cast<EventClassID>(evtclass),callback);
}

OneNETContext::EventID OneNETContext::RegisterEvent(EventClassID evtclass,std::function<void()> callback)
{
    for(uint16_t insid=0; insid < 0xFFFF ; insid++)
    {
        EventID id=evtclass;
        id<<=16;
        id+=insid;
        if(event_map.find(id)==event_map.end())
        {
            std::lock_guard<std::mutex> lock(event_map_lock);
            event_map[id]=callback;
            return id;
        }
        else
        {
            std::function<void()> action=event_map[id];
            if(action==NULL)
            {
                std::lock_guard<std::mutex> lock(event_map_lock);
                event_map[id]=callback;
                return id;
            }
        }
    }
    return ((uint32_t)static_cast<uint16_t>(UNKOWN_CLASS))<<16;
}

void OneNETContext::UnRegisterEvent(EventID evtid)
{
    std::lock_guard<std::mutex> lock(event_map_lock);
    if(event_map.find(evtid)!=event_map.end())
    {
        //使用NULL标记移除
        event_map[evtid]=NULL;
    }
}

void OneNETContext::SetOneNETMQTT(OneNETMQTT *_mqtt)
{
    mqtt=_mqtt;
}

void OneNETContext::SetOneNETDevice(OneNETDevice *_device)
{
    device=_device;
}

void OneNETContext::AddActionWithoutLock(std::function<void()> action)
{
    action_queue.push(action);
}

void OneNETContext::EmitEventWithoutLock(EventClass evtclass)
{
    EmitEventWithoutLock(static_cast<EventClassID>(evtclass));
}
void OneNETContext::EmitEventWithoutLock(EventClassID evtclass)
{
    for(uint16_t insid=0; insid < 0xFFFF ; insid++)
    {
        EventID id=evtclass;
        id<<=16;
        id+=insid;
        if(event_map.find(id)!=event_map.end())
        {
            std::function<void()> action=event_map[id];
            if(action==NULL)
            {
                continue;
            }
            AddActionWithoutLock(action);
        }
        else
        {
            break;
        }
    }
}

static OneNETContext g_context;

OneNETContext & OneNETContextDefault()
{
    return g_context;
}

void OneNETContextInit()
{
    //设置MQTT
    g_context.SetOneNETMQTT(&OneNETMQTTDefault());

    //设置设备
    g_context.SetOneNETDevice(&OneNETDeviceDefault());


    {
        //打印部分上下文信息
        g_context.RegisterEvent(OneNETContext::CONTEXT_START_CLASS,[]()
        {
            LOGINFO("%s->Context start!",TAG);
        });
        g_context.RegisterEvent(OneNETContext::CONTEXT_MQTT_CONNECTED_CLASS,[]()
        {
            LOGINFO("%s->MQTT is connected!",TAG);
        });
        g_context.RegisterEvent(OneNETContext::CONTEXT_MQTT_DISCONNECTED_CLASS,[]()
        {
            LOGINFO("%s->MQTT is connected!",TAG);
        });
    }

    //启动上下文
    std::thread([=]()
    {
        g_context.Run();
    }).detach();

    std::this_thread::yield();
}
