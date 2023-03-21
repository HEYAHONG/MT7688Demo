#include "onenetconfig.h"
#include "args.h"
#include "log.h"
#include <netlib.h>

static const char *TAG="OneNET Config";

static OneNETConfig g_cfg;

OneNETConfig &OneNETConfigDefault()
{
    return g_cfg;
}

static void OneNETConfigDefaultInit()
{
#ifdef OPENWRT
    {
        uint8_t *mac = netlib_ifconfig_getifhwaddr("br-lan");
        if (mac != NULL)
        {
            char macstr[20] = {0};
            snprintf(macstr, sizeof(macstr), "%02X%02X%02X%02X%02X%02X", (int)mac[0], (int)mac[1], (int)mac[2], (int)mac[3], (int)mac[4], (int)mac[5]);
            if (strlen(macstr) != 0)
            {
                LOGINFO("%s->OneNETConfig:mac is %s",TAG,macstr);
                g_cfg.OneNET.devicename=macstr;
            }
        }
    }
#endif // OPENWRT
}

static void OneNETConfigDefaultInitFromArg()
{
    try
    {
        const char *_val=NULL;
        if((_val=args_get("MQTTAddress"))!=NULL)
        {
            std::string val(_val);
            if (val.find(":") != std::string::npos)
            {
                g_cfg.MQTT.serveraddr = val.substr(0, val.find(":"));
                g_cfg.MQTT.serverport = std::stoul(val.substr(val.find(":") + 1));
            }
            else
            {
                g_cfg.MQTT.serveraddr = val;
                g_cfg.MQTT.serverport = 1883;
            }
        }
        if ((_val = args_get("MQTTKeepalive")) != NULL)
        {
            std::string val(_val);
            g_cfg.MQTT.keeplive = std::stoul(val);
        }

        if ((_val = args_get("MQTTSSL")) != NULL)
        {
            std::string val(_val);
            if (std::stoul(val) != 0)
            {
                g_cfg.MQTT.ssl=true;
            }
            else
            {
                g_cfg.MQTT.ssl=false;
            }
        }

        if ((_val = args_get("MQTTCAFile")) != NULL)
        {
            std::string val(_val);
            if (!val.empty())
            {
                g_cfg.MQTT.ca = val;
            }
        }
        if ((_val = args_get("MQTTCheckServerName")) != NULL)
        {
            std::string val(_val);
            if (std::stoul(val) != 0)
            {
                g_cfg.MQTT.checkservername = true;
            }
            else
            {
                g_cfg.MQTT.checkservername = false;
            }

        }


        if ((_val = args_get("OneNETProductID")) != NULL)
        {
            std::string val(_val);
            if (!val.empty())
            {
                g_cfg.OneNET.productid = val;
            }
        }
        if ((_val = args_get("OneNETAccessKey")) != NULL)
        {
            std::string val(_val);
            if (!val.empty())
            {
                g_cfg.OneNET.accesskey = val;
            }
        }
        if ((_val = args_get("OneNETDeviceName")) != NULL)
        {
            std::string val(_val);
            if (!val.empty())
            {
                g_cfg.OneNET.devicename = val;
            }
        }

    }
    catch(...)
    {
        LOGINFO("%s->OneNET Config is invalid!",TAG);
    }
}

static void OneNETConfigDefaultPrint()
{
    if(!g_cfg.MQTT.serveraddr.empty())
    {
        LOGINFO("%s->Host:%s,Port:%u,Keeplive:%d!",TAG,g_cfg.MQTT.serveraddr.c_str(),g_cfg.MQTT.serverport,g_cfg.MQTT.keeplive);
        if(!g_cfg.MQTT.ca.empty())
        {
            LOGINFO("%s->SSL:%s,CA:%s,CheckServerName:%s",TAG,g_cfg.MQTT.ssl?"true":"false",g_cfg.MQTT.ca.c_str(),g_cfg.MQTT.checkservername?"true":"false");
        }
    }
    if(!g_cfg.OneNET.productid.empty() && !g_cfg.OneNET.accesskey.empty() && !g_cfg.OneNET.devicename.empty())
    {
        LOGINFO("%s->DeviceName:%s,ProductID:%s,AccessKey:%s!",TAG,g_cfg.OneNET.devicename.c_str(),g_cfg.OneNET.productid.c_str(),g_cfg.OneNET.accesskey.c_str());
    }
}

void OneNETConfigInit()
{
    OneNETConfigDefaultInit();

    OneNETConfigDefaultInitFromArg();

    OneNETConfigDefaultPrint();
}
