#include "onenetdevice.h"
#include "onenetonejson.h"
#include "onenetmqtt.h"

OneNETDevice::OneNETDevice()
{

}
OneNETDevice::~OneNETDevice()
{

}

std::function<bool(std::string,Json::Value)> OneNETDevice::GetOnPropertySet()
{
    return std::bind(&OneNETDevice::OnPropertySet,this,std::placeholders::_1,std::placeholders::_2);
}

std::function<bool(std::string,Json::Value&)> OneNETDevice::GetOnPropertyGet()
{
    return std::bind(&OneNETDevice::OnPropertyGet,this,std::placeholders::_1,std::placeholders::_2);
}

bool OneNETDevice::OnPropertySet(std::string key,Json::Value value)
{
    PropertyCache[key]=value;
    return true;
}

bool OneNETDevice::OnPropertyGet(std::string key,Json::Value& value)
{
    if(PropertyCache.find(key)!=PropertyCache.end())
    {
        value=PropertyCache[key];
        return true;
    }

    return false;
}

static OneNETDevice g_device;

OneNETDevice &  OneNETDeviceDefault()
{
    return g_device;
}

void OneNETDeviceInit()
{
    //关联OneJson回调函数
    OneNETOneJsonDefault().SetOnPropertySet(g_device.GetOnPropertySet());
    OneNETOneJsonDefault().SetOnPropertyGet(g_device.GetOnPropertyGet());
}
