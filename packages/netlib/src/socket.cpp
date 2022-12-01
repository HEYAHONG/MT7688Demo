#include "netlib.h"
#include <unistd.h>
#include <sys/socket.h>

/** \brief 获取套接字(AF_INET),所有的网络操作（包括对网络信息的查询）均需要套接字作为参数。
 *
 * \return int 套接字
 *
 */
int netlib_socket()
{
    return socket(AF_INET, SOCK_DGRAM, 0);
}

/** \brief 获取套接字（AF_INET6）所有的网络操作（包括对网络信息的查询）均需要套接字作为参数。
 *
 * \return int 套接字
 *
 */
int netlib_socket6()
{
    return socket(AF_INET, SOCK_DGRAM, 0);
}


/** \brief 关闭套接字
 *
 *
 */
void  netlib_close(int sock)
{
    close(sock);
}




