#ifndef __NETLIB_H__
#define __NETLIB_H__
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#ifndef MICROTOSTR
#define MICROTOSTR(x) #x
#endif // MICROTOSTR
#ifndef MICROTOSTR2
#define MICROTOSTR2(x) MICROTOSTR(x)
#endif // MICROTOSTR2

/** \brief 获取库版本
 *
 * \return const char* 版本字符串
 *
 */
const char *netlib_getversion();



#ifdef __cplusplus
}
#endif // __cplusplus

#ifdef __cplusplus
#include <string>
#include <vector>

#endif // __cplusplus

#endif // __NETLIB_H__
