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


/** \brief 获取套接字(AF_INET),所有的网络操作（包括对网络信息的查询）均需要套接字作为参数。
 *
 * \return int 套接字
 *
 */
int netlib_socket();

/** \brief 获取套接字（AF_INET6）所有的网络操作（包括对网络信息的查询）均需要套接字作为参数。
 *
 * \return int 套接字
 *
 */
int netlib_socket6();


/** \brief 关闭套接字
 *
 * \param sock int 套接字
 *
 */
void  netlib_close(int sock);


/** \brief 获取网络接口数量
 *
 * \param sock int  套接字
 * \return size_t 网络接口数量
 *
 */
size_t netlib_ifconf_getsize(int sock);


/** \brief 通过index获取网络接口名字
 *
 * \param index size_t 从0开始的index
 * \return const char* 网络接口名字,失败返回NULL
 *
 */
const char *netlib_ifconf_getifname(size_t index);

/** \brief 获取hwaddr（即mac地址）
 *
 * \param sock int  套接字
 * \param ifname const char* 网络接口名字
 * \return uint8_t* mac地址(6字节)，失败返回NULL
 *
 */
#define NETLIB_IFHWADDR_SIZE 6
uint8_t *netlib_ifconfig_getifhwaddr(int sock, const char *ifname);

#ifdef __cplusplus
}
#endif // __cplusplus

#ifdef __cplusplus
#include <string>
#include <vector>

/** \brief 获取网络接口数量
 *
 * \return size_t 网络接口数量
 *
 */
size_t netlib_ifconf_getsize();


/** \brief 获取hwaddr（即mac地址）
 *
 * \param ifname const char* 网络接口名字
 * \return uint8_t* mac地址(6字节)，失败返回NULL
 *
 */
uint8_t *netlib_ifconfig_getifhwaddr(const char *ifname);


#endif // __cplusplus

#endif // __NETLIB_H__
