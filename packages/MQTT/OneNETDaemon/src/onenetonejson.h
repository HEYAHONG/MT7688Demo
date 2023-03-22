#ifndef ONENETONEJSON_H
#define ONENETONEJSON_H
#ifdef __cplusplus
#include <json/value.h>
#include <json/reader.h>
#include <json/writer.h>
#include <string>
#include <functional>
#include <vector>
#include <map>
#include <time.h>

class OneNETOneJson
{
public:
    OneNETOneJson();
    virtual ~OneNETOneJson();

    typedef enum
    {
        ONENET_ONEJSON_TYPE_MQTT=1,
        ONENET_ONEJSON_TYPE_DEFAULT=ONENET_ONEJSON_TYPE_MQTT
    } OneJsonType;

    //设置产品ID和设备名称,注意：不符合产品ID与设备名称的消息将忽略
    void SetDev(std::string _ProductID,std::string _DeviceName);

    //MQTT接口(一般不由用户调用，初始化MQTT栈时使用)
    bool OnMQTTMessage(std::string topic,std::string payload);
    void SetMQTTPublish(std::function<bool(std::string,std::string)> _MQTTPublish);

    //OneJson设备属性/事件接口
    typedef struct
    {
        std::string key;
        Json::Value value;
        time_t timestamp;
    } PropertyPostParam;
    bool PropertyPost(std::vector<PropertyPostParam> params,std::string id,OneJsonType type=ONENET_ONEJSON_TYPE_DEFAULT);
    void SetOnPropertyPostReply(std::function<void(std::string,int,std::string)> _OnPropertyPostReply);
    void SetOnPropertySet(std::function<bool(std::string,Json::Value)> _OnPropertySet);
    void SetOnPropertyGet(std::function<bool(std::string,Json::Value&)> _OnPropertyGet);
    typedef struct
    {
        std::string identifier;
        std::vector<PropertyPostParam> value;
        time_t timestamp;
    } EventPostParam;
    bool EventPost(std::vector<EventPostParam> params,std::string id,OneJsonType type=ONENET_ONEJSON_TYPE_DEFAULT);
    void SetOnEventPostReply(std::function<void(std::string,int,std::string)> _OnEventPostReply);
    typedef struct
    {
        struct
        {
            std::string productid;
            std::string devicename;
        } identity;
        std::vector<PropertyPostParam> properties;
        std::vector<EventPostParam> events;
    } PackPostParam;
    bool PackPost(std::vector<PackPostParam> params,std::string id,OneJsonType type=ONENET_ONEJSON_TYPE_DEFAULT);
    void SetOnPackPostReply(std::function<void(std::string,int,std::string)> _OnPackPostReply);
    typedef struct
    {
        Json::Value value;
        time_t timestamp;
    } HistoryPropertyValue;
    typedef struct
    {
        std::string key;
        std::vector<HistoryPropertyValue> value;
    } HistoryProperty;
    typedef struct
    {
        std::vector<PropertyPostParam> value;
        time_t timestamp;
    } HistoryEventValue;
    typedef struct
    {
        std::string identifier;
        std::vector<HistoryEventValue> value;
    } HistoryEvent;
    typedef struct
    {
        struct
        {
            std::string productid;
            std::string devicename;
        } identity;
        std::vector<HistoryProperty> properties;
        std::vector<HistoryEvent> events;
    } HistoryPostParam;
    bool HistoryPost(std::vector<HistoryPostParam> params,std::string id,OneJsonType type=ONENET_ONEJSON_TYPE_DEFAULT);
    void SetOnHistoryPostReply(std::function<void(std::string,int,std::string)> _OnHistoryPostReply);

    //OneJson设备服务调用接口
    typedef std::map< std::string,Json::Value> ServiceParam;
    typedef std::map< std::string,Json::Value> ServiceData;
    bool ServiceReturnAsync(ServiceData data,std::string identifier,std::string id,int code,std::string msg,OneJsonType type=ONENET_ONEJSON_TYPE_DEFAULT);
    void SetOnServiceInvoke(std::function<ServiceData(ServiceParam,std::string,std::string)> _OnServiceInvoke);

    //OneJson属性期望值接口
    bool PropertyDesiredGet(std::vector<std::string> params,std::string id,OneJsonType type=ONENET_ONEJSON_TYPE_DEFAULT);
    typedef struct
    {
        Json::Value value;
        int version;
    } PropertyDesiredValue;
    void SetOnPropertyDesiredGetReply(std::function<void(std::map<std::string,PropertyDesiredValue>,std::string,int,std::string)> _OnPropertyDesiredGetReply);
    bool PropertyDesiredDelete(std::vector<std::string> params,std::string id,OneJsonType type=ONENET_ONEJSON_TYPE_DEFAULT);
    bool PropertyDesiredDelete(std::map<std::string,int> params,std::string id,OneJsonType type=ONENET_ONEJSON_TYPE_DEFAULT);
    void SetOnPropertyDesiredDeleteReply(std::function<void(std::string,int,std::string)> _OnPropertyDesiredDeleteReply);


private:
    //MQTT发送函数
    std::function<bool(std::string,std::string)> MQTTPublish;

    //设备信息
    std::string ProductID;
    std::string DeviceName;

    //OneJson设备属性/事件接口
    std::function<void(std::string,int,std::string)> OnPropertyPostReply;
    std::function<bool(std::string,Json::Value)> OnPropertySet;
    std::function<bool(std::string,Json::Value&)> OnPropertyGet;
    std::function<void(std::string,int,std::string)> OnEventPostReply;
    std::function<void(std::string,int,std::string)> OnPackPostReply;
    std::function<void(std::string,int,std::string)> OnHistoryPostReply;

    //OneJson设备服务调用接口
    std::function<ServiceData(ServiceParam,std::string,std::string)> OnServiceInvoke;

    //OneJson属性期望值接口
    std::function<void(std::map<std::string,PropertyDesiredValue>,std::string,int,std::string)> OnPropertyDesiredGetReply;
    std::function<void(std::string,int,std::string)> OnPropertyDesiredDeleteReply;

};

OneNETOneJson &OneNETOneJsonDefault();

void OneNETOneJsonInit();

#endif // __cplusplus
#endif // ONENETONEJSON_H
