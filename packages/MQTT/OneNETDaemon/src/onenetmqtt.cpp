#include "onenetmqtt.h"
#include "mosquitto.h"
#include "onenettokencpp.h"
#include "args.h"
#include "log.h"
#include <thread>
#include <chrono>

static const char *TAG="OneNET MQTT";

static class libmosquitto_gaurd
{
public:
    libmosquitto_gaurd()
    {
        mosquitto_lib_init();
    }
    ~libmosquitto_gaurd()
    {
        mosquitto_lib_cleanup();
    }
} g_libmosquitto_gaurd;

static bool checkConfig(OneNETConfig &config)
{
    if(config.MQTT.serveraddr.empty())
    {
        return false;
    }

    if(config.MQTT.serverport > 0xFFFF || config.MQTT.serverport == 0)
    {
        return false;
    }

    if(config.MQTT.keeplive <= 0)
    {
        return false;
    }

    if(config.MQTT.ssl && config.MQTT.ca.empty())
    {
        return false;
    }

    if(config.OneNET.devicename.empty() || config.OneNET.productid.empty() || config.OneNET.accesskey.empty())
    {
        return false;
    }

    return true;
}

OneNETMQTT::OneNETMQTT():mqtt_context(NULL),on_message_lock(NULL)
{

}
OneNETMQTT::~OneNETMQTT()
{

}

void OneNETMQTT::MQTTMessage(std::string topic,std::string payload)
{
    //LOGINFO("%s->topic:%s,payload:%s",TAG,topic.c_str(),payload.c_str()); //此行代码一般用于Debug
    if(_OnMessage!=NULL)
    {
        try
        {
            if(on_message_lock==NULL)
            {
                _OnMessage(topic,payload);
            }
            else
            {
                std::lock_guard<std::mutex> lock(*on_message_lock);
                _OnMessage(topic,payload);
            }

        }
        catch(...)
        {
            LOGINFO("%s->process message error!",TAG);
        }

    }
}


bool OneNETMQTT::Start(OneNETConfig &config)
{
    if(!checkConfig(config))
    {
        return false;
    }

    current_config=config;

    if(IsRunning())
    {
        return false;
    }
    _IsConnected=false;

    struct mosquitto *mosq=mosquitto_new(config.OneNET.devicename.c_str(),true,this);
    if(mosq!=NULL)
    {
        mqtt_context=mosq;
        //OneNET要求MQTT版本必须为3.1.1
        mosquitto_int_option(mosq,MOSQ_OPT_PROTOCOL_VERSION,MQTT_PROTOCOL_V311);
        //设置SSL信息
        if(config.MQTT.ssl)
        {
            mosquitto_tls_set(mosq,config.MQTT.ca.c_str(),NULL,NULL,NULL,NULL);
            mosquitto_tls_insecure_set(mosq,!config.MQTT.checkservername);
        }
        {
            //设置OneNET认证信息

            std::string res=OneNETTokenGetDeviceRes(config.OneNET.productid,config.OneNET.devicename);
            std::string sign=OneNETTokenGetSign(0x7FFFFFFF,res,config.OneNET.accesskey,ONENETCPP_DEFAULT_METHOD,ONENETCPP_DEFAULT_VERSION);
            //计算token
            std::string token=OneNETTokenGenerateURLToken(0x7FFFFFFF,res,sign,ONENETCPP_DEFAULT_METHOD,ONENETCPP_DEFAULT_VERSION);

            mosquitto_username_pw_set(mosq,config.OneNET.productid.c_str(),token.c_str());
        }

        {
            //设置日志回调函数
            auto log=[](struct mosquitto *mosq,void *obj,int level,const char *msg)
            {
                LOGINFO("%s->MQTTLog %08X (%08X),level %d,msg=%s",TAG,(int)reinterpret_cast<uint64_t>(obj), (int)reinterpret_cast<uint64_t>((void *)mosq), level, msg);
            };
            mosquitto_log_callback_set(mosq,log);
            //设置连接的回调函数
            auto connect=[](struct mosquitto *mosq,void *obj,int rc)
            {
                OneNETMQTT *mqttobj=static_cast<OneNETMQTT *>(obj);
                if(mqttobj!=NULL)
                {
                    //订阅所有OneNET消息
                    mosquitto_subscribe((struct mosquitto *)mqttobj->mqtt_context,NULL, (std::string("$sys/")+mqttobj->current_config.OneNET.productid+"/"+mqttobj->current_config.OneNET.devicename+"/#").c_str(),0);
                    mqttobj->_IsConnected=true;
                }
            };
            mosquitto_connect_callback_set(mosq,connect);
            auto disconnect=[](struct mosquitto *mosq,void *obj,int rc)
            {
                OneNETMQTT *mqttobj=static_cast<OneNETMQTT *>(obj);
                if(mqttobj!=NULL)
                {
                    mqttobj->_IsConnected=false;
                }

            };
            mosquitto_disconnect_callback_set(mosq,disconnect);
            //设置消息回调函数
            auto message=[](struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
            {
                OneNETMQTT *mqttobj=static_cast<OneNETMQTT *>(obj);
                if(mqttobj!=NULL)
                {
                    std::string topic(msg->topic);
                    std::string payload;
                    if(msg->payloadlen>0)
                    {
                        payload=std::string((const char *)msg->payload,msg->payloadlen);
                    }

                    mqttobj->MQTTMessage(topic,payload);
                }
            };
            mosquitto_message_callback_set(mosq,message);

        }

        //启动循环
        mosquitto_loop_start(mosq);

        //启动连接
        mosquitto_connect_async(mosq,config.MQTT.serveraddr.c_str(),config.MQTT.serverport,config.MQTT.keeplive);
        mosquitto_reconnect_delay_set(mosq,3,30,false);
    }

    return IsRunning();
}

void OneNETMQTT::Stop()
{
    if(IsRunning())
    {
        mosquitto_loop_stop((struct mosquitto *)mqtt_context,false);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        mosquitto_destroy((struct mosquitto *)mqtt_context);
        mqtt_context=NULL;
    }
}
bool OneNETMQTT::IsRunning()
{
    return mqtt_context!=NULL;
}

bool OneNETMQTT::IsConnected()
{
    if(IsRunning())
    {
        return _IsConnected;
    }
    return false;
}

bool OneNETMQTT::Publish(std::string topic,std::string payload)
{
    if(topic.empty())
    {
        return false;
    }
    if(IsConnected())
    {
        return MOSQ_ERR_SUCCESS==mosquitto_publish((struct mosquitto *)mqtt_context,NULL,topic.c_str(),payload.length(),payload.c_str(),0,false);
    }
    return false;
}

void OneNETMQTT::SetOnMessage(std::function<void(std::string,std::string)> OnMessage)
{
    _OnMessage=OnMessage;
}

void OneNETMQTT::SetOnMessageLock(std::mutex *lock)
{
    on_message_lock=lock;
}

static OneNETMQTT g_mqtt;

OneNETMQTT &OneNETMQTTDefault()
{
    return g_mqtt;
}

void OneNETMQTTInit()
{
    if(checkConfig(OneNETConfigDefault()))
    {
        LOGINFO("%s->Check config Ok!Now start mqtt!",TAG);
        g_mqtt.Start();
        if(g_mqtt.IsRunning())
        {
            LOGINFO("%s->MQTT is started!",TAG);
        }
    }
    else
    {
        LOGINFO("%s->Check config failed!",TAG);
    }
}
