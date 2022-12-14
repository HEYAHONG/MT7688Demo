#include "netlib.h"
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/if_ether.h>
#include <net/if.h>
#include <linux/sockios.h>


/** \brief 获取网络接口数量
 *
 * \param sock int  套接字
 * \return size_t 网络接口数量
 *
 */
#ifndef MAX_INTERFACE_NUMBER
#define MAX_INTERFACE_NUMBER 32 //此数量一定要多于实际的网络接口数量
#endif // MAX_INTERFACE_NUMBER
size_t netlib_ifconf_getsize(int sock)
{
    size_t ret = 0;
    if (sock < 0)
    {
        return ret;
    }

    struct ifconf ifc = {0};

    ifc.ifc_len = sizeof(struct ifreq) * MAX_INTERFACE_NUMBER;
    ifc.ifc_req = (struct ifreq *)malloc(ifc.ifc_len);
    memset(ifc.ifc_req, 0, ifc.ifc_len);


    if (ioctl(sock, SIOCGIFCONF, &ifc) >= 0)
    {
        if (ifc.ifc_len > 0)
        {
            ret = ifc.ifc_len / sizeof(struct ifreq);
        }
    }

    if (ifc.ifc_req != NULL)
    {
        free(ifc.ifc_req);
    }

    return ret;

}

/** \brief 获取网络接口数量
 *
 * \param sock int  套接字
 * \return size_t 网络接口数量
 *
 */
size_t netlib_ifconf_getsize()
{
    int sock = netlib_socket();
    size_t ret = netlib_ifconf_getsize(sock);
    netlib_close(sock);
    return ret;
}

/** \brief 通过index获取网络接口名字
 *
 * \param index size_t 从0开始的index
 * \return const char* 网络接口名字,失败返回NULL
 *
 */
static thread_local char ifname[IF_NAMESIZE] = {0};
const char *netlib_ifconf_getifname(size_t index)
{
    //所有编号加1
    index += 1;

    memset(ifname, 0, sizeof(ifname));
    if_indextoname(index, ifname);
    if (strlen(ifname) > 0)
    {
        return ifname;
    }
    return NULL;
}

/** \brief 获取hwaddr（即mac地址）
 *
 * \param sock int  套接字
 * \param ifname const char* 网络接口名字
 * \return uint8_t* mac地址(6字节)，失败返回NULL
 *
 */
static thread_local uint8_t mac[NETLIB_IFHWADDR_SIZE] = {0};
uint8_t *netlib_ifconfig_getifhwaddr(int sock, const char *ifname)
{
    if (ifname == NULL || strlen(ifname) == 0)
    {
        return NULL;
    }

    struct ifreq req = {0};
    strncpy(req.ifr_name, ifname, sizeof(req.ifr_name) - 1);
    req.ifr_hwaddr.sa_family = ARPHRD_ETHER;

    if (ioctl(sock, SIOCGIFHWADDR, &req) >= 0)
    {
        memcpy(mac, req.ifr_hwaddr.sa_data, ETH_ALEN);
        return mac;
    }

    return NULL;
}

/** \brief 获取hwaddr（即mac地址）
 *
 * \param ifname const char* 网络接口名字
 * \return uint8_t* mac地址(6字节)，失败返回NULL
 *
 */
uint8_t *netlib_ifconfig_getifhwaddr(const char *ifname)
{
    int sock = netlib_socket();
    uint8_t *ret = netlib_ifconfig_getifhwaddr(sock, ifname);
    netlib_close(sock);
    return ret;
}

/** \brief 获取地址信息
 *
 * \param sock int 套接字
 * \param ifname const char* 网络接口名字
 * \return netlib_addr_t 地址信息，失败返回NULL
 *
 */
static thread_local netlib_addr_t addr = {0};
netlib_addr_t *netlib_ifconfig_getifaddr(int sock, const char *ifname)
{
    if (ifname == NULL || strlen(ifname) == 0)
    {
        return NULL;
    }

    struct ifreq req = {0};
    strncpy(req.ifr_name, ifname, sizeof(req.ifr_name) - 1);

    if (ioctl(sock, SIOCGIFADDR, &req) >= 0)
    {
        addr = req.ifr_addr;
        return &addr;
    }

    return NULL;
}

/** \brief 获取地址信息
 *
 * \param ifname const char* 网络接口名字
 * \return netlib_addr_t 地址信息，失败返回NULL
 *
 */
netlib_addr_t *netlib_ifconfig_getifaddr(const char *ifname)
{
    int sock = netlib_socket();
    netlib_addr_t *ret = netlib_ifconfig_getifaddr(sock, ifname);
    netlib_close(sock);
    return ret;
}

/** \brief 获取地址掩码信息
 *
 * \param sock int 套接字
 * \param ifname const char* 网络接口名字
 * \return netlib_addr_t 地址掩码信息，失败返回NULL
 *
 */
static thread_local netlib_addr_t netmask = {0};
netlib_addr_t *netlib_ifconfig_getifnetmask(int sock, const char *ifname)
{
    if (ifname == NULL || strlen(ifname) == 0)
    {
        return NULL;
    }

    struct ifreq req = {0};
    strncpy(req.ifr_name, ifname, sizeof(req.ifr_name) - 1);

    if (ioctl(sock, SIOCGIFNETMASK, &req) >= 0)
    {
        netmask = req.ifr_netmask;
        return &netmask;
    }

    return NULL;
}

/** \brief 获取地址掩码信息
 *
 * \param ifname const char* 网络接口名字
 * \return netlib_addr_t 地址掩码信息，失败返回NULL
 *
 */
netlib_addr_t *netlib_ifconfig_getifnetmask(const char *ifname)
{
    int sock = netlib_socket();
    netlib_addr_t *ret = netlib_ifconfig_getifnetmask(sock, ifname);
    netlib_close(sock);
    return ret;
}


static uint32_t __netlib_getifflags(int sock, const char *ifname)
{
    uint32_t ret = 0;
    if (ifname == NULL || strlen(ifname) == 0)
    {
        return ret;
    }

    struct ifreq req = {0};
    strncpy(req.ifr_name, ifname, sizeof(req.ifr_name) - 1);

    if (ioctl(sock, SIOCGIFFLAGS, &req) >= 0)
    {
        ret = req.ifr_flags;
    }

    return ret;
}

/** \brief 检查是否有RUNNING标志
 *
 * \param sock int 套接字
 * \param ifname const char* 网络接口名字
 * \return bool 是返回true
 *
 */
bool netlib_ifconfig_isrunning(int sock, const char *ifname)
{
    return (__netlib_getifflags(sock, ifname)&IFF_RUNNING) != 0;
}

/** \brief 检查是否有RUNNING标志
 *
 * \param ifname const char* 网络接口名字
 * \return bool 是返回true
 *
 */
bool netlib_ifconfig_isrunning(const char *ifname)
{
    int sock = netlib_socket();
    bool ret = netlib_ifconfig_isrunning(sock, ifname);
    netlib_close(sock);
    return ret;
}

/** \brief 检查是否有UP标志
 *
 * \param sock int 套接字
 * \param ifname const char* 网络接口名字
 * \return bool 是返回true
 *
 */
bool netlib_ifconfig_isup(int sock, const char *ifname)
{
    return (__netlib_getifflags(sock, ifname)&IFF_UP) != 0;
}

/** \brief 检查是否有UP标志
 *
 * \param ifname const char* 网络接口名字
 * \return bool 是返回true
 *
 */
bool netlib_ifconfig_isup(const char *ifname)
{
    int sock = netlib_socket();
    bool ret = netlib_ifconfig_isup(sock, ifname);
    netlib_close(sock);
    return ret;
}

/** \brief 检查是否有LOOPBACK标志
 *
 * \param sock int 套接字
 * \param ifname const char* 网络接口名字
 * \return bool 是返回true
 *
 */
bool netlib_ifconfig_isloopback(int sock, const char *ifname)
{
    return (__netlib_getifflags(sock, ifname)&IFF_LOOPBACK) != 0;
}

/** \brief 检查是否有LOOPBACK标志
 *
 * \param ifname const char* 网络接口名字
 * \return bool 是返回true
 *
 */
bool netlib_ifconfig_isloopback(const char *ifname)
{
    int sock = netlib_socket();
    bool ret = netlib_ifconfig_isloopback(sock, ifname);
    netlib_close(sock);
    return ret;
}
