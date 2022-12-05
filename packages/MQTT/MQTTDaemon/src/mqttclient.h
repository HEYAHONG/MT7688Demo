#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H
#include <stdint.h>
#include <stdlib.h>
#include "mosquitto.h"

#ifdef __cplusplus
#include <string>
#include <functional>
#include <memory>

typedef struct
{
    std::string host;//主机名
    uint16_t port; //端口
    int keepalive; //保活时间
    bool cleansession;//是否开启干净会话
    std::string clientid;
    struct _auth
    {
        std::string username; //用户名
        std::string password;//密码
    } auth;
    struct _will
    {
        std::string will_topic;//遗嘱主题
        std::string will_payload;//遗嘱负载数据
        uint8_t will_qos;//遗嘱qos
        bool will_retain;//遗嘱是否为保留消息
    } will;
    struct _subscribe
    {
        std::string subtopic;//订阅主题
        int qos;//订阅的服务质量
    } subscribe;//默认订阅
    struct _ssl
    {
        std::string cadir;//根证书搜索目录
        std::string cacertpath;//根证书路径
        std::string certpath;//证书路径
        std::string keypath;//密钥路径
        bool enablessl;//是否启用ssl
        bool checkservername;//SSL中是否检查服务器名称
    } ssl;
} MQTT_Cfg_t;

typedef struct
{
    std::string topic;
    std::string payload;
    uint8_t qos;
    bool retain;
} MQTT_Message_t;

typedef std::shared_ptr<MQTT_Message_t> MQTT_Message_Ptr_t;

class MQTTClient;

typedef struct
{
    std::function<void(MQTTClient &)> connect;
    std::function<void(MQTTClient &)> disconnect;
    std::function<void(MQTTClient &, MQTT_Message_Ptr_t)> onmessage;
} MQTT_Callback_t;

class MQTTClient
{
public:
    MQTTClient();
    virtual ~MQTTClient();
    MQTTClient(const MQTTClient &other);
    MQTTClient &operator=(const MQTTClient &other);

    bool Start();
    bool Stop();
    bool ReStart();
    bool IsStart();

    MQTT_Cfg_t &SetCfg(MQTT_Cfg_t _cfg);
    MQTT_Cfg_t  GetCfg();
    MQTT_Callback_t &SetCallback(MQTT_Callback_t _cb);
    MQTT_Callback_t  GetCallback();

    bool MQTTPublish(MQTT_Message_Ptr_t msg);
    bool MQTTSubscribe(std::string topic, uint8_t qos);

protected:
    MQTT_Cfg_t cfg;
    MQTT_Callback_t cb;
private:
    void mosq_new();
    void mosq_free();
    struct mosquitto *mosq;
    static void MQTT_on_log(struct mosquitto *, void *, int, const char *);
    static void MQTT_on_connect(struct mosquitto *mosq, void *obj, int rc);
    static void MQTT_on_disconnect(struct mosquitto *mosq, void *obj, int rc);
    static void MQTT_on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg);
};



#endif // __cplusplus

#endif // MQTTCLIENT_H
