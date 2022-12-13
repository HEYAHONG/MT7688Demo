#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "netlib.h"
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    {
        netlib_addr_t *addr = NULL;
        if ((addr = netlib_nslookup(false, "localhost")) != NULL)
        {
            if (addr->sa_family = AF_INET)
            {
                char buff[64] = {0};
                struct sockaddr_in *addr4 = (struct sockaddr_in *)addr;
                printf("localhost(ipv4):%s\r\n", inet_ntop(AF_INET, &addr4->sin_addr, buff, sizeof(buff) - 1));
            }
        }

        if ((addr = netlib_nslookup(true, "localhost")) != NULL)
        {
            if (addr->sa_family = AF_INET6)
            {
                char buff[64] = {0};
                struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)addr;
                printf("localhost(ipv6): %s\r\n", inet_ntop(AF_INET6, &addr6->sin6_addr, buff, sizeof(buff) - 1));
            }
        }

        if ((addr = netlib_nslookup(false, "www.baidu.com")) != NULL)
        {
            if (addr->sa_family = AF_INET)
            {
                char buff[64] = {0};
                struct sockaddr_in *addr4 = (struct sockaddr_in *)addr;
                printf("www.baidu.com(ipv4):%s\r\n", inet_ntop(AF_INET, &addr4->sin_addr, buff, sizeof(buff) - 1));
            }
        }

        if ((addr = netlib_nslookup(true, "www.baidu.com")) != NULL)
        {
            if (addr->sa_family = AF_INET6)
            {
                char buff[64] = {0};
                struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)addr;
                printf("www.baidu.com(ipv6): %s\r\n", inet_ntop(AF_INET6, &addr6->sin6_addr, buff, sizeof(buff) - 1));
            }
        }

        if ((addr = netlib_nslookup(false, "192.168.1.1")) != NULL)
        {
            if (addr->sa_family = AF_INET)
            {
                char buff[64] = {0};
                struct sockaddr_in *addr4 = (struct sockaddr_in *)addr;
                printf("192.168.1.1(ipv4):%s\r\n", inet_ntop(AF_INET, &addr4->sin_addr, buff, sizeof(buff) - 1));
            }
        }

        if ((addr = netlib_nslookup(true, "12:34:56:78:9A:BC:DE:F0")) != NULL)
        {
            if (addr->sa_family = AF_INET6)
            {
                char buff[64] = {0};
                struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)addr;
                printf("12:34:56:78:9A:BC:DE:F0(ipv6): %s\r\n", inet_ntop(AF_INET6, &addr6->sin6_addr, buff, sizeof(buff) - 1));
            }
        }
    }

    return 0;
}
