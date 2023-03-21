#ifndef ONENETMQTT_H
#define ONENETMQTT_H
#ifdef __cplusplus
#include "onenetconfig.h"
#include <string>
#include <functional>
#include <mutex>

class OneNETMQTT
{
private:
    void *mqtt_context;
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
};

OneNETMQTT &OneNETMQTTDefault();

void OneNETMQTTInit();

#endif // __cplusplus
#endif // ONENETMQTT_H
