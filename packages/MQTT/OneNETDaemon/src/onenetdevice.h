#ifndef ONENETDEVICE_H
#define ONENETDEVICE_H
#ifdef __cplusplus
#include "onenetonejson.h"
#include <map>
#include <string>
/*
此文件只包含常用功能，详细接口请查看onenetonejson.h的接口
*/
class OneNETDevice;

class OneNETSubDevice
{
public:
    OneNETSubDevice();
    virtual ~OneNETSubDevice();

    //设置子设备信息
    void SetDev(std::string _productID, std::string _deviceName);

    //运行(一般由Context调用)
    void RunOne();

private:
    std::string productID;
    std::string deviceName;
    friend class OneNETDevice;

    std::map<std::string, Json::Value> PropertyCache;

    /** \brief 属性值设置
     *
     * \param key std::string 属性名称
     * \param value Json::Value 属性值，可以为任意Json类型
     * \return bool 是否成功
     *
     */
    bool OnPropertySet(std::string key, Json::Value value);

    /** \brief 属性值获取
     *
     * \param key std::string 属性名称
     * \param value Json::Value& 属性值，可以为任意Json类型
     * \return bool 是否成功
     *
     */
    bool OnPropertyGet(std::string key, Json::Value &value);
};

#define ONENET_MAX_ID_LENGTH 13

class OneNETDevice
{
public:
    OneNETDevice();
    virtual ~OneNETDevice();

    //设置OneNETOneJson
    void SetOneJson(OneNETOneJson *_onejson);

    //设置是否为网关设备
    void SetIsGateWay(bool _IsGateWay);

    //运行(一般由Context调用)
    void RunOne();

    //MQTT接口(一般由Context调用)
    void OnMQTTConnected();
    void OnMQTTDisconnected();

    //获得主动获取时需要的的ID参数
    std::string GetDeviceNextID();
    std::string GetSubDeviceNextID();


private:

    //设备属性相关参数
    std::function<void(std::string, int, std::string)> GetOnPropertyPostReply();
    std::function<bool(std::string, Json::Value)> GetOnPropertySet();
    std::function<bool(std::string, Json::Value &)> GetOnPropertyGet();
    std::function<void(std::string, int, std::string)> GetOnEventPostReply();

    //子设备上下线
    bool SubLogin(std::string productID, std::string deviceName);
    bool SubLogout(std::string productID, std::string deviceName);


    //拓扑管理
    std::function<void(std::vector<OneNETOneJson::SubTopoDevInfo>, std::string, int, std::string)> GetOnSubTopoGetReply();
    std::function<void(std::vector<OneNETOneJson::SubTopoDevInfo>)> GetOnSubTopoChange();

    //子设备数据交互
    std::function<bool(std::string, std::string, std::string, Json::Value &)> GetOnSubPropertyGet();
    std::function<bool(std::string, std::string, std::string, Json::Value)> GetOnSubPropertySet();


    //是否为网关设备
    bool IsGateWay;

    //OneJson指针
    OneNETOneJson *onejson;

    //内部ID计数
    uint32_t IDCount;

    /** \brief 获得下一个ID
     *
     * \param prefix std::string ID前缀
     * \return std::string ID字符串
     *
     */
    std::string GetNextID(std::string prefix);

    /** \brief 属性上报回复
     *
     * \param id std::string ID字符串
     * \param code int 代码
     * \param msg std::string 消息字符串
     *
     */
    void OnPropertyPostReply(std::string id, int code, std::string msg);


    //属性缓存
    std::map<std::string, Json::Value> PropertyCache;

    /** \brief 属性值设置
     *
     * \param key std::string 属性名称
     * \param value Json::Value 属性值，可以为任意Json类型
     * \return bool 是否成功
     *
     */
    bool OnPropertySet(std::string key, Json::Value value);

    /** \brief 属性值获取
     *
     * \param key std::string 属性名称
     * \param value Json::Value& 属性值，可以为任意Json类型
     * \return bool 是否成功
     *
     */
    bool OnPropertyGet(std::string key, Json::Value &value);

    /** \brief 事件上报回复
     *
     * \param id std::string ID字符串
     * \param code int 代码
     * \param msg std::string 消息字符串
     *
     */
    void OnEventPostReply(std::string id, int code, std::string msg);


    std::string GetSubDeviceString(std::string _productID, std::string _deviceName);
    std::map<std::string, OneNETSubDevice> SubDeviceMap;
    void SubDeviceMapAdd(std::string _productID, std::string _deviceName);
    void SubDeviceMapRemove(std::string _productID, std::string _deviceName);

    //拓扑管理

    /** \brief 设备表查询结果
     *
     * \param devinfo std::vector<SubTopoDevInfo> 设备信息
     * \param id std::string ID字符串
     * \param code int 代码
     * \param msg std::string 消息字符串
     *
     */
    void OnSubTopoGetReply(std::vector<OneNETOneJson::SubTopoDevInfo> devinfo, std::string id, int code, std::string msg);

    /** \brief  设备表改变（由应用或者控制台下发）
     *
     * \param devinfo std::vector<OneNETOneJson::SubTopoDevInfo> 设备信息
     *
     */
    void OnSubTopoChange(std::vector<OneNETOneJson::SubTopoDevInfo> devinfo);

    //子设备数据交互

    /** \brief  子设备属性获取
     *
     * \param productID std::string 产品ID
     * \param deviceName std::string 设备名称
     * \param key std::string 属性名称
     * \param value Json::Value& 属性值，可以为任意Json类型
     * \return bool  是否成功
     *
     */
    bool OnSubPropertyGet(std::string productID, std::string deviceName, std::string key, Json::Value &value);

    /** \brief  子设备属性设置
     *
     * \param productID std::string 产品ID
     * \param deviceName std::string 设备名称
     * \param key std::string 属性名称
     * \param value Json::Value 属性值，可以为任意Json类型
     * \return bool  是否成功
     *
     */
    bool OnSubPropertySet(std::string productID, std::string deviceName, std::string key, Json::Value value);

};

OneNETDevice   &OneNETDeviceDefault();

void OneNETDeviceInit();
#endif // __cplusplus
#endif // ONENETDEVICE_H
