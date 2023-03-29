#include "onenetdevice.h"
#include "onenetcontext.h"
#include "onenetonejson.h"
#include "onenetmqtt.h"
#include "log.h"

static const char *TAG = "OneNET Device";

OneNETSubDevice::OneNETSubDevice()
{

}
OneNETSubDevice::~OneNETSubDevice()
{

}

void OneNETSubDevice::SetDev(std::string _productID, std::string _deviceName)
{
    productID = _productID;
    deviceName = _deviceName;
}


void OneNETSubDevice::RunOne()
{

}

bool OneNETSubDevice::OnPropertySet(std::string key, Json::Value value)
{
    PropertyCache[key] = value;
    return true;
}

bool OneNETSubDevice::OnPropertyGet(std::string key, Json::Value &value)
{
    if (PropertyCache.find(key) != PropertyCache.end())
    {
        value = PropertyCache[key];
        return true;
    }
    return false;
}

OneNETDevice::OneNETDevice(): IsGateWay(false), onejson(NULL), IDCount(0)
{

}
OneNETDevice::~OneNETDevice()
{

}

void OneNETDevice::SetOneJson(OneNETOneJson *_onejson)
{
    onejson = _onejson;
    if (onejson != NULL)
    {
        onejson->SetOnPropertyPostReply(GetOnPropertyPostReply());
        onejson->SetOnPropertySet(GetOnPropertySet());
        onejson->SetOnPropertyGet(GetOnPropertyGet());
        onejson->SetOnEventPostReply(GetOnEventPostReply());
        onejson->SetOnSubTopoGetReply(GetOnSubTopoGetReply());
        onejson->SetOnSubTopoChange(GetOnSubTopoChange());
        onejson->SetOnSubPropertyGet(GetOnSubPropertyGet());
        onejson->SetOnSubPropertySet(GetOnSubPropertySet());
    }

}

void OneNETDevice::SetIsGateWay(bool _IsGateWay)
{
    IsGateWay = _IsGateWay;
}

void OneNETDevice::RunOne()
{
    //调用子设备的RunOne
    for (auto SubDevice : SubDeviceMap)
    {
        SubDevice.second.RunOne();
    }
}

void OneNETDevice::OnMQTTConnected()
{
    if (IsGateWay)
    {
        //读取子设备表
        if (onejson != NULL)
        {
            onejson->SubTopoGet(GetDeviceNextID());
        }
    }
}
void OneNETDevice::OnMQTTDisconnected()
{

}

std::function<void(std::string, int, std::string)> OneNETDevice::GetOnPropertyPostReply()
{
    return std::bind(&OneNETDevice::OnPropertyPostReply, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

std::function<bool(std::string, Json::Value)> OneNETDevice::GetOnPropertySet()
{
    return std::bind(&OneNETDevice::OnPropertySet, this, std::placeholders::_1, std::placeholders::_2);
}

std::function<bool(std::string, Json::Value &)> OneNETDevice::GetOnPropertyGet()
{
    return std::bind(&OneNETDevice::OnPropertyGet, this, std::placeholders::_1, std::placeholders::_2);
}

std::function<void(std::string, int, std::string)> OneNETDevice::GetOnEventPostReply()
{
    return std::bind(&OneNETDevice::OnEventPostReply, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

bool OneNETDevice::SubLogin(std::string productID, std::string deviceName)
{
    if (onejson != NULL)
    {
        return onejson->SubLogin(productID, deviceName, GetSubDeviceNextID());
    }
    return false;
}
bool OneNETDevice::SubLogout(std::string productID, std::string deviceName)
{
    if (onejson != NULL)
    {
        return onejson->SubLogin(productID, deviceName, GetSubDeviceNextID());
    }
    return false;
}

std::function<void(std::vector<OneNETOneJson::SubTopoDevInfo>, std::string, int, std::string)> OneNETDevice::GetOnSubTopoGetReply()
{
    return std::bind(&OneNETDevice::OnSubTopoGetReply, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
}

std::function<void(std::vector<OneNETOneJson::SubTopoDevInfo>)> OneNETDevice::GetOnSubTopoChange()
{
    return std::bind(&OneNETDevice::OnSubTopoChange, this, std::placeholders::_1);
}

std::function<bool(std::string, std::string, std::string, Json::Value &)> OneNETDevice::GetOnSubPropertyGet()
{
    return std::bind(&OneNETDevice::OnSubPropertyGet, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
}

std::function<bool(std::string, std::string, std::string, Json::Value)> OneNETDevice::GetOnSubPropertySet()
{
    return std::bind(&OneNETDevice::OnSubPropertySet, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
}

std::string OneNETDevice::GetDeviceNextID()
{
    //直连设备/网关的前缀为1
    return GetNextID("1");
}
std::string OneNETDevice::GetSubDeviceNextID()
{
    //子设备的前缀为2
    return GetNextID("2");
}

std::string OneNETDevice::GetNextID(std::string prefix)
{
    if (prefix.length() >= ONENET_MAX_ID_LENGTH)
    {
        return prefix.substr(0, ONENET_MAX_ID_LENGTH);
    }
    size_t countlength = ONENET_MAX_ID_LENGTH - prefix.length();
    std::string strcount = std::to_string(IDCount++);
    strcount = strcount.substr(countlength > strcount.length() ? 0 : strcount.length() - countlength);
    return prefix + strcount;
}

void OneNETDevice::OnPropertyPostReply(std::string id, int code, std::string msg)
{

}

bool OneNETDevice::OnPropertySet(std::string key, Json::Value value)
{
    PropertyCache[key] = value;
    return true;
}

bool OneNETDevice::OnPropertyGet(std::string key, Json::Value &value)
{
    if (PropertyCache.find(key) != PropertyCache.end())
    {
        value = PropertyCache[key];
        return true;
    }

    return false;
}

void OneNETDevice::OnEventPostReply(std::string id, int code, std::string msg)
{

}

std::string OneNETDevice::GetSubDeviceString(std::string _productID, std::string _deviceName)
{
    return _productID + _deviceName;
}

void OneNETDevice::SubDeviceMapAdd(std::string _productID, std::string _deviceName)
{
    std::string devstring = GetSubDeviceString(_productID, _deviceName);
    if (SubDeviceMap.find(devstring) == SubDeviceMap.end())
    {
        SubDeviceMap[devstring] = OneNETSubDevice();
        SubDeviceMap[devstring].SetDev(_productID, _deviceName);
    }
}
void OneNETDevice::SubDeviceMapRemove(std::string _productID, std::string _deviceName)
{
    std::string devstring = GetSubDeviceString(_productID, _deviceName);
    if (SubDeviceMap.find(devstring) != SubDeviceMap.end())
    {
        SubDeviceMap.erase(SubDeviceMap.find(devstring));
    }
}

void OneNETDevice::OnSubTopoGetReply(std::vector<OneNETOneJson::SubTopoDevInfo> devinfo, std::string id, int code, std::string msg)
{
    LOGINFO("%s->Sub Device Get:id=%s,code=%d,msg=%s", TAG, id.c_str(), code, msg.c_str());
    for (const OneNETOneJson::SubTopoDevInfo dev : devinfo)
    {
        LOGINFO("%s->Sub Device Get:productid=%s,devicename=%s", TAG, dev.productid.c_str(), dev.devicename.c_str());
        SubDeviceMapAdd(dev.productid, dev.devicename);
        //默认网关上线子设备就上线
        SubLogin(dev.productid, dev.devicename);
    }
}

void OneNETDevice::OnSubTopoChange(std::vector<OneNETOneJson::SubTopoDevInfo> devinfo)
{
    IsGateWay = true;
    std::vector<std::string> devstring;
    for (const OneNETOneJson::SubTopoDevInfo dev : devinfo)
    {
        LOGINFO("%s->Sub Device Change:productid=%s,devicename=%s", TAG, dev.productid.c_str(), dev.devicename.c_str());
        devstring.push_back(GetSubDeviceString(dev.productid, dev.devicename));
        SubDeviceMapAdd(dev.productid, dev.devicename);
    }

    {
        std::vector<std::string> devtoremove;
        //遍历设备表,移除不存在的设备
        for (auto SubDevice : SubDeviceMap)
        {
            if (std::find(devstring.begin(), devstring.end(), SubDevice.first) == devstring.end())
            {
                devtoremove.push_back(SubDevice.first);
            }
        }

        for (auto dev : devtoremove)
        {
            auto it = SubDeviceMap.find(dev);
            if (it != SubDeviceMap.end())
            {
                SubDeviceMap.erase(it);
            }
        }

    }
}

bool OneNETDevice::OnSubPropertyGet(std::string productID, std::string deviceName, std::string key, Json::Value &value)
{
    std::string devstring = GetSubDeviceString(productID, deviceName);
    if (SubDeviceMap.find(devstring) != SubDeviceMap.end())
    {
        return SubDeviceMap[devstring].OnPropertyGet(key, value);
    }
    return false;
}

bool OneNETDevice::OnSubPropertySet(std::string productID, std::string deviceName, std::string key, Json::Value value)
{
    std::string devstring = GetSubDeviceString(productID, deviceName);
    if (SubDeviceMap.find(devstring) != SubDeviceMap.end())
    {
        return SubDeviceMap[devstring].OnPropertySet(key, value);
    }
    return false;
}

static OneNETDevice g_device;

OneNETDevice   &OneNETDeviceDefault()
{
    return g_device;
}

void OneNETDeviceInit()
{
    //设置为网关设备
    g_device.SetIsGateWay(true);
    //关联OneJson回调函数
    g_device.SetOneJson(&OneNETOneJsonDefault());
    //关联Context
    OneNETContextDefault().RegisterEvent(OneNETContext::CONTEXT_MQTT_CONNECTED_CLASS, []()
    {
        g_device.OnMQTTConnected();
    });
    OneNETContextDefault().RegisterEvent(OneNETContext::CONTEXT_MQTT_DISCONNECTED_CLASS, []()
    {
        g_device.OnMQTTDisconnected();
    });
    OneNETContextDefault().RegisterEvent(OneNETContext::CONTEXT_LOOP_CLASS, []()
    {
        g_device.RunOne();
    });
}
