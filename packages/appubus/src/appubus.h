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
    Json::Value signature;
} ubus_cli_list_object_item;

/** \brief ubus list(异步)
 *
 * \param path std::string 路径
 * \param result std::function<void(ubus_cli_list_object_item &)> 结果回调（为空将不执行）
 * \return bool 是否加入执行队列
 *
 */
bool ubus_cli_list(std::string path, std::function<void(ubus_cli_list_object_item &)> result);

#endif // __cplusplus

#endif // APPUBUS_H
