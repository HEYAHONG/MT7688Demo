#ifndef ONENETMQTT_H
#define ONENETMQTT_H
#ifdef __cplusplus
#include "onenetconfig.h"
#include "onenetcontext.h"
#include <string>
#include <functional>
#include <chrono>

class OneNETMQTT
{
private:
    std::chrono::steady_clock::time_point last_publish_time;
    void *mqtt_context;
    OneNETContext *context;
    OneNETConfig current_config;
    bool _IsConnected;
    std::function<void(std::string,std::string)> _OnMessage;
    void MQTTMessage(std::string topic,std::string payload);
public:
    OneNETMQTT();
    virtual ~OneNETMQTT();
    bool Start(OneNETConfig &config=OneNETConfigDefault());
    void Stop();
    bool IsRunning();
    bool IsConnected();
    bool Publish(std::string topic,std::string payload);
    void SetOnMessage(std::function<void(std::string,std::string)> OnMessage);
    void SetContext(OneNETContext *_context);
};

OneNETMQTT &OneNETMQTTDefault();

void OneNETMQTTInit();

#endif // __cplusplus
#endif // ONENETMQTT_H
