#ifndef APPUBUS_H
#define APPUBUS_H
#ifdef __cplusplus
#include <functional>
#include <memory>

#endif // __cplusplus
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
bool ubus_is_connected();

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // APPUBUS_H
