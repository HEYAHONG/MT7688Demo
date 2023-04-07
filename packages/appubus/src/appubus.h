#ifndef APPUBUS_H
#define APPUBUS_H

#include "stdlib.h"
#include "stdint.h"

//无效回调ID
#define UBUS_INVALID_CALLBACK_ID ((uint32_t)-1)

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
#include "stdbool.h"

/** \brief 是否连接到ubus
 *
 * \return bool 是否连接
 *
 */
bool ubus_cli_is_connected();

#ifdef __cplusplus
}
#endif // __cplusplus

#ifdef __cplusplus
#include <functional>
#include <memory>
#include <thread>
#include <chrono>
#include <mutex>
#include <map>
#include <queue>
#include <json/json.h>

/** \brief 注册ubus连接回调
 *
 * \param _cb  std::function<void()>  回调函数
 * \return uint32_t 回调ID,用于反注册，失败返回UBUS_INVALID_CALLBACK_ID
 *
 */
uint32_t ubus_cli_register_onconnected(std::function<void()> _cb);



/** \brief   反注册ubus连接回调
 *
 * \param id uint32_t 回调ID
 *
 */
void ubus_cli_unregister_onconnected(uint32_t id);



/** \brief 注册ubus断开连接回调
 *
 * \param _cb  std::function<void()>  回调函数
 * \return uint32_t 回调ID,用于反注册，失败返回UBUS_INVALID_CALLBACK_ID
 *
 */
uint32_t ubus_cli_register_ondisconnected(std::function<void()> _cb);



/** \brief   反注册ubus断开连接回调
 *
 * \param id uint32_t 回调ID
 *
 */
void ubus_cli_unregister_ondisconnected(uint32_t id);

typedef struct
{
    uint32_t id;
    std::string path;
    Json::Value signature; //结构Json，当json值为数字时,表示BLOBMSG_TYPE_*（详见blobmsg.h）
} ubus_cli_list_object_item;

/** \brief ubus list(异步)
 *
 * \param path std::string 路径
 * \param result std::function<void(ubus_cli_list_object_item &)> 结果回调（为空将不执行）
 * \param error std::function<void()> 失败回调，当执行失败时调用。
 * \return bool 是否加入执行队列
 *
 */
bool ubus_cli_list(std::string path, std::function<void(ubus_cli_list_object_item &)> result, std::function<void()> error = NULL);

/** \brief 是否处于monitor中
 *
 * \return bool 是否处于monitor中
 *
 */
bool ubus_cli_is_in_monitor();

/** \brief 启动monitor(必须在ubus成功连接后使用)
 *
 *
 * \return bool 是否成功
 */
bool ubus_cli_start_monitor();

/** \brief 停止monitor
 *
 *
 * \return bool 是否成功
 */
bool ubus_cli_stop_monitor();

typedef struct
{
    int32_t client;
    int32_t peer;
    bool send;
    int32_t seq;
    int32_t type;//值为UBUS_MSG_*(详见ubusmsg.h)
    Json::Value data;
} ubus_cli_monitor_item;

/** \brief 注册monitor回调函数
 *
 * \param  _cb std::function<void(ubus_cli_monitor_item &)> 回调函数
 * \return uint32_t 回调ID
 *
 */
uint32_t ubus_cli_register_monitor(std::function<void(ubus_cli_monitor_item &)> _cb);

/** \brief 反注册monitor回调函数
 *
 * \param id uint32_t 回调ID
 *
 */
void ubus_cli_unregister_monitor(uint32_t id);


/** \brief ubus call(异步)
 *
 * \param path std::string 路径,不可为空
 * \param method std::string 方法,不可为空
 * \param msg Json::Value 当为object时有效
 * \param result std::function<void(Json::Value )> 结果回调
 * \param timeout_ms size_t 调用超时
 * \param error std::function<void()> 失败回调（当失败时调用）
 * \return bool 是否加入执行队列
 *
 */
bool ubus_cli_call(std::string path, std::string method, Json::Value msg, std::function<void(Json::Value)> result, size_t timeout_ms = 3000, std::function<void()> error = NULL);


#endif // __cplusplus

#endif // APPUBUS_H
