#include "mqttclient.h"
#include "args.h"
#include "log.h"

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
} libmosquitto;



static MQTT_Cfg_t LoadCfgFromArg()
{
    MQTT_Cfg_t cfg;
    {
        std::string sn = std::string("MQTTClient") + std::to_string(std::rand());
        //填写默认参数
        cfg.host = "mqtt.hyhsystem.cn";
        cfg.port = 1883;
        cfg.cleansession = true;
        cfg.clientid = sn;
        cfg.keepalive = 120;
        cfg.auth.username = sn;
        cfg.auth.password = sn;
        cfg.ssl.enablessl = false;
        cfg.subscribe.subtopic = (sn + "/#");
        cfg.subscribe.qos = 0;
    }

    {
        //从参数中获取MQTT默认参数
        const char *_val = NULL;
        if ((_val = args_get("MQTTDaemonAddress")) != NULL)
        {
            std::string val(_val);
            if (val.find(":") != std::string::npos)
            {
                cfg.host = val.substr(0, val.find(":"));
                cfg.port = std::stoul(val.substr(val.find(":") + 1));
            }
            else
            {
                cfg.host = val;
                cfg.port = 1883;
            }
        }

        if ((_val = args_get("MQTTDaemonKeepalive")) != NULL)
        {
            std::string val(_val);
            cfg.keepalive = std::stoul(val);
        }

        if ((_val = args_get("MQTTDaemonClientId")) != NULL)
        {
            std::string val(_val);
            if (!val.empty())
            {
                cfg.clientid = val;
            }
        }

        if ((_val = args_get("MQTTDaemonUserName")) != NULL)
        {
            std::string val(_val);
            if (!val.empty())
            {
                cfg.auth.username = val;
            }
        }

        if ((_val = args_get("MQTTDaemonPassWord")) != NULL)
        {
            std::string val(_val);
            if (!val.empty())
            {
                cfg.auth.password = val;
            }
        }

        if ((_val = args_get("MQTTDaemonSSL")) != NULL)
        {
            std::string val(_val);
            if (!val.empty())
            {
                if (std::stoul(val) != 0)
                {
                    cfg.ssl.enablessl = true;

                    if ((_val = args_get("MQTTDaemonCAFile")) != NULL)
                    {
                        std::string val(_val);
                        if (!val.empty())
                        {
                            cfg.ssl.cacertpath = val;
                        }
                    }

                    if ((_val = args_get("MQTTDaemonCAPath")) != NULL)
                    {
                        std::string val(_val);
                        if (!val.empty())
                        {
                            cfg.ssl.cadir = val;
                        }
                    }

                    if ((_val = args_get("MQTTDaemonCertFile")) != NULL)
                    {
                        std::string val(_val);
                        if (!val.empty())
                        {
                            cfg.ssl.certpath = val;
                        }
                    }

                    if ((_val = args_get("MQTTDaemonKeyFile")) != NULL)
                    {
                        std::string val(_val);
                        if (!val.empty())
                        {
                            cfg.ssl.keypath = val;
                        }
                    }

                    if ((_val = args_get("MQTTDaemonCheckServerName")) != NULL)
                    {
                        std::string val(_val);
                        if (!val.empty())
                        {
                            if (std::stoul(val) != 0)
                            {
                                cfg.ssl.checkservername = true;
                            }
                            else
                            {
                                cfg.ssl.checkservername = false;
                            }
                        }
                    }


                }
                else
                {
                    cfg.ssl.enablessl = false;
                }
            }
        }


    }
    return cfg;
}

MQTTClient::MQTTClient(): mosq(NULL), cfg(LoadCfgFromArg())
{

}

MQTTClient::~MQTTClient()
{
    mosq_free();
}

MQTTClient::MQTTClient(const MQTTClient &other)
{

    cfg = other.cfg;
    cb = other.cb;
}

MQTTClient &MQTTClient::operator=(const MQTTClient &rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    cfg = rhs.cfg;
    cb = rhs.cb;
    return *this;
}

bool MQTTClient::Start()
{
    bool ret = (mosq == NULL);
    mosq_new();
    return ret;
}
bool MQTTClient::Stop()
{
    bool ret = (mosq != NULL);
    mosq_free();
    return ret;
}
bool MQTTClient::ReStart()
{
    Stop();
    Start();
    return true;
}
bool MQTTClient::IsStart()
{
    return mosq != NULL;
}

MQTT_Cfg_t &MQTTClient::SetCfg(MQTT_Cfg_t _cfg)
{
    cfg = _cfg;
    return cfg;
}
MQTT_Cfg_t  MQTTClient::GetCfg()
{
    return cfg;
}
MQTT_Callback_t &MQTTClient::SetCallback(MQTT_Callback_t _cb)
{
    cb = _cb;
    return cb;
}
MQTT_Callback_t  MQTTClient::GetCallback()
{
    return cb;
}

bool  MQTTClient::MQTTPublish(MQTT_Message_Ptr_t msg)
{
    if (IsStart() && !msg->topic.empty())
    {
        return MOSQ_ERR_SUCCESS == mosquitto_publish(mosq, NULL, msg->topic.c_str(), msg->payload.length(), msg->payload.c_str(), msg->qos, msg->retain);
    }
    return false;
}
bool  MQTTClient::MQTTSubscribe(std::string topic, uint8_t qos)
{
    if (IsStart() && !topic.empty())
    {
        return MOSQ_ERR_SUCCESS == mosquitto_subscribe(mosq, NULL, topic.c_str(), qos);
    }
    return false;
}

void MQTTClient::mosq_new()
{
    if (mosq == NULL)
    {
        mosq = mosquitto_new(cfg.clientid.c_str(), cfg.cleansession, this);
        if (!cfg.auth.password.empty())
        {
            //设置帐号密码
            mosquitto_username_pw_set(mosq, cfg.auth.username.c_str(), cfg.auth.password.c_str());
        }
        if (cfg.ssl.enablessl)
        {
            //设置SSL信息
            mosquitto_tls_set(mosq, cfg.ssl.cacertpath.c_str(), cfg.ssl.cadir.c_str(), cfg.ssl.certpath.c_str(), cfg.ssl.keypath.c_str(), NULL);
            mosquitto_tls_insecure_set(mosq, !cfg.ssl.checkservername);
        }

        if (!cfg.will.will_topic.empty())
        {
            mosquitto_will_set(mosq, cfg.will.will_topic.c_str(), cfg.will.will_payload.length(), cfg.will.will_payload.c_str(), cfg.will.will_qos, cfg.will.will_retain);
        }

        mosquitto_connect_callback_set(mosq, MQTT_on_connect);

        mosquitto_disconnect_callback_set(mosq, MQTT_on_disconnect);

        mosquitto_message_callback_set(mosq, MQTT_on_message);

        mosquitto_log_callback_set(mosq, MQTT_on_log);


        //启动线程
        mosquitto_loop_start(mosq);

        if (!cfg.host.empty())
        {
            mosquitto_connect_async(mosq, cfg.host.c_str(), cfg.port != 0 ? cfg.port : 0, cfg.keepalive);
            mosquitto_reconnect_delay_set(mosq, 3, 30, false);
        }

    }
}
void MQTTClient::mosq_free()
{
    if (mosq != NULL)
    {
        mosquitto_disconnect(mosq);
        mosquitto_loop_stop(mosq, false);
        sleep(1);//延时
        mosquitto_destroy(mosq);
        mosq = NULL;
    }
}


void MQTTClient::MQTT_on_log(struct mosquitto *mosq, void *obj, int level, const char *msg)
{
    LOGINFO("MQTTClient %08X (%08X),level %d,msg=%s", (int)reinterpret_cast<uint64_t>(obj), (int)reinterpret_cast<uint64_t>((void *)mosq), level, msg);
}

void MQTTClient::MQTT_on_connect(struct mosquitto *mosq, void *obj, int rc)
{
    if (mosq != NULL && obj != NULL)
    {
        MQTTClient &client = *(MQTTClient *)obj;

        if (!client.cfg.subscribe.subtopic.empty())
        {
            mosquitto_subscribe(mosq, NULL, client.cfg.subscribe.subtopic.c_str(), client.cfg.subscribe.qos);
        }

        if (client.cb.connect != NULL)
        {
            client.cb.connect(client);
        }
    }
}
void MQTTClient::MQTT_on_disconnect(struct mosquitto *mosq, void *obj, int rc)
{
    if (mosq != NULL && obj != NULL)
    {
        MQTTClient &client = *(MQTTClient *)obj;

        if (client.cb.disconnect != NULL)
        {
            client.cb.disconnect(client);
        }
    }
}
void MQTTClient::MQTT_on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
    if (mosq != NULL && obj != NULL)
    {
        MQTTClient &client = *(MQTTClient *)obj;

        MQTT_Message_Ptr_t ptr = std::make_shared<MQTT_Message_t>();
        ptr->topic = msg->topic;
        if (msg->payloadlen != 0 && msg->payload != NULL)
        {
            ptr->payload = std::string((char *)msg->payload, msg->payloadlen);
        }
        ptr->qos = msg->qos;
        ptr->retain = msg->retain;

        if (client.cb.onmessage != NULL)
        {
            client.cb.onmessage(client, ptr);
        }
    }
}
