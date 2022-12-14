#ifndef __NETLIB_H__
#define __NETLIB_H__
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/socket.h>
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



/** \brief 日志回调函数定义
 */
typedef void netlib_log_cb(const char *file, int line, const char *log);
/** \brief 日志回调函数指针
 */
typedef netlib_log_cb *netlib_log_cb_t;

/** \brief 日志函数（通常不直接调用，只调用宏）
 *
 * \param file const char * 文件名
 * \param line int 行号
 * \param log const char* 日志内容
 * \param ... 可变参数，配合log使用
 *
 */
void netlib_log_real(const char *file, int line, const char *log, ...);

/** \brief 日志函数
 *
 *
 */
#ifndef NETLIB_LOG
#define NETLIB_LOG(fmt,...) {netlib_log_real(__FILE__,__LINE__,fmt,##__VA_ARGS__ );}
#endif // NETLIB_LOG


/** \brief 设置日志回调函数
 *
 * \param cb netlib_log_cb_t  日志回调函数
 * \param isthread bool 是否为线程设置
 *
 */
void netlib_log_callback_set(netlib_log_cb_t cb, bool isthread);


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



typedef struct sockaddr netlib_addr_t; /**< 地址信息定义 */


/** \brief 获取地址信息
 *
 * \param sock int 套接字
 * \param ifname const char* 网络接口名字
 * \return netlib_addr_t 地址信息，失败返回NULL
 *
 */
netlib_addr_t *netlib_ifconfig_getifaddr(int sock, const char *ifname);

/** \brief 获取地址掩码信息
 *
 * \param sock int 套接字
 * \param ifname const char* 网络接口名字
 * \return netlib_addr_t 地址掩码信息，失败返回NULL
 *
 */
netlib_addr_t *netlib_ifconfig_getifnetmask(int sock, const char *ifname);


/** \brief 检查是否有RUNNING标志
 *
 * \param sock int 套接字
 * \param ifname const char* 网络接口名字
 * \return bool 是返回true
 *
 */
bool netlib_ifconfig_isrunning(int sock, const char *ifname);

/** \brief 检查是否有UP标志
 *
 * \param sock int 套接字
 * \param ifname const char* 网络接口名字
 * \return bool 是返回true
 *
 */
bool netlib_ifconfig_isup(int sock, const char *ifname);

/** \brief 检查是否有LOOPBACK标志
 *
 * \param sock int 套接字
 * \param ifname const char* 网络接口名字
 * \return bool 是返回true
 *
 */
bool netlib_ifconfig_isloopback(int sock, const char *ifname);


/** \brief 通过主机名获取网络地址（第一个有效地址）
 *
 * \param isipv6 bool 是否为IPV6
 * \param host const char* 主机名
 * \return netlib_addr_t* 网络地址,失败返回NULL
 *
 */
netlib_addr_t *netlib_nslookup(bool isipv6, const char *host);

#ifdef __cplusplus
}
#endif // __cplusplus

#ifdef __cplusplus
#include <string>
#include <vector>
#include <functional>

/** \brief 设置日志回调函数
 *
 * \param cb std::function<netlib_log_cb>  日志回调函数
 * \param isthread bool 是否为线程设置
 *
 */
void netlib_log_callback_set(std::function<netlib_log_cb> cb, bool isthread = true);

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

/** \brief 获取地址信息
 *
 * \param ifname const char* 网络接口名字
 * \return netlib_addr_t 地址信息，失败返回NULL
 *
 */
netlib_addr_t *netlib_ifconfig_getifaddr(const char *ifname);

/** \brief 获取地址信息
 *
 * \param ifname const char* 网络接口名字
 * \return netlib_addr_t 地址信息，失败返回NULL
 *
 */
netlib_addr_t *netlib_ifconfig_getifnetmask(const char *ifname);

/** \brief 检查是否有RUNNING标志
 *
 * \param ifname const char* 网络接口名字
 * \return bool 是返回true
 *
 */
bool netlib_ifconfig_isrunning(const char *ifname);
/** \brief 检查是否有UP标志
 *
 * \param ifname const char* 网络接口名字
 * \return bool 是返回true
 *
 */
bool netlib_ifconfig_isup(const char *ifname);
/** \brief 检查是否有LOOPBACK标志
 *
 * \param ifname const char* 网络接口名字
 * \return bool 是返回true
 *
 */
bool netlib_ifconfig_isloopback(const char *ifname);

#endif // __cplusplus

#endif // __NETLIB_H__
