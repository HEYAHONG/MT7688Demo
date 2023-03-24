#ifndef ONENETDEVICE_H
#define ONENETDEVICE_H
#ifdef __cplusplus
#include "onenetonejson.h"
#include <map>
#include <string>
/*
此文件只包含常用功能，详细接口请查看onenetonejson.h的接口
*/
class OneNETDevice
{
public:
    OneNETDevice();
    virtual ~OneNETDevice();

    std::function<bool(std::string,Json::Value)> GetOnPropertySet();
    std::function<bool(std::string,Json::Value&)> GetOnPropertyGet();

private:
    std::map<std::string,Json::Value> PropertyCache;

    /** \brief 属性值设置
     *
     * \param key std::string 属性名称
     * \param value Json::Value 属性值，可以为任意Json类型
     * \return bool 是否成功
     *
     */
    bool OnPropertySet(std::string key,Json::Value value);

    /** \brief 属性值获取
     *
     * \param key std::string 属性名称
     * \param value Json::Value& 属性值，可以为任意Json类型
     * \return bool 是否成功
     *
     */
    bool OnPropertyGet(std::string key,Json::Value& value);

};

OneNETDevice &  OneNETDeviceDefault();

void OneNETDeviceInit();
#endif // __cplusplus
#endif // ONENETDEVICE_H
