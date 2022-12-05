#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <netlib.h>
#include <unistd.h>
#include "log.h"
#include <chrono>
#include "args.h"
#include "mqttclient.h"

static void welcome()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    LOGINFO("Started at %s", std::asctime(std::localtime(&in_time_t)));
}

int main(int argc, char *argv[])
{
    //输出欢迎信息
    welcome();

    //检查参数
    args_parse(argc, argv);

    //打印参数
    args_print();



    //MQTT消息
    MQTTClient client;
    {
//默认目标地址
#define DEFAULT_DEST_ADDR "DESTADDR"
        //启动设置回调
        MQTT_Callback_t cb = {0};
        cb.connect = [](MQTTClient & client)
        {
            //发送上线消息
            MQTT_Message_Ptr_t msg = std::make_shared<MQTT_Message_t>();
            msg->qos = 0;
            msg->retain = false;
            msg->topic = (std::string(DEFAULT_DEST_ADDR) + "/" + client.GetCfg().clientid);
            msg->payload = "Online";
            client.MQTTPublish(msg);

            LOGINFO("MQTT is connected!");
        };
        cb.disconnect = [](MQTTClient & client)
        {
            //离线
            LOGINFO("MQTT is disconnected!");
        };
        cb.onmessage = [](MQTTClient & client, MQTT_Message_Ptr_t msg)
        {
            //接收到消息

            //修改主题后原样回复
            size_t pos = 0;
            if ((pos = msg->topic.find("/")) != std::string::npos)
            {
                std::string srcaddr =    msg->topic.substr(pos + 1);

                if (srcaddr.empty())
                {
                    return;
                }
                msg->topic = (srcaddr + "/" + client.GetCfg().clientid);
                client.MQTTPublish(msg);
            }

        };
        client.SetCallback(cb);
    }
    {
        MQTT_Cfg_t cfg = client.GetCfg();

        //调整设置
#ifdef OPENWRT
        {
            uint8_t *mac = netlib_ifconfig_getifhwaddr("br-lan");
            if (mac != NULL)
            {
                char macstr[20] = {0};
                snprintf(macstr, sizeof(macstr), "%02X%02X%02X%02X%02X%02X", (int)mac[0], (int)mac[1], (int)mac[2], (int)mac[3], (int)mac[4], (int)mac[5]);
                if (strlen(macstr) != 0)
                {
                    LOGINFO("Main : mac is %s", macstr);
                    std::string sn = macstr;
                    cfg.clientid = sn;
                    cfg.auth.username = sn;
                    cfg.auth.password = sn;
                    cfg.subscribe.subtopic = sn + "/#";
                }
            }
        }
#endif // OPENWRT

        client.SetCfg(cfg);
    }
    client.Start();

    //openwrt中Daemon程序不用转入后台，但不能退出。
    while (true)
    {
        sleep(5);
    }
}
