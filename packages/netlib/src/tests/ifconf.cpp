#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "netlib.h"
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    printf("number of interface:%d\r\n", (int)netlib_ifconf_getsize());

    for (size_t i = 0; i < netlib_ifconf_getsize(); i++)
    {
        const char *ifname = netlib_ifconf_getifname(i);
        if (ifname != NULL)
        {
            printf("\t%d: %s:\r\n", (int)i + 1, ifname);
            uint8_t *mac = netlib_ifconfig_getifhwaddr(ifname);
            if (mac != NULL)
            {
                printf("\t\tmac: %02X:%02X:%02X:%02X:%02X:%02X\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            }

            {
                netlib_addr_t *addr = netlib_ifconfig_getifaddr(ifname);
                {
                    if (addr != NULL)
                    {
                        if (addr->sa_family == AF_INET)
                        {
                            //IPV4
                            in_addr ip_address = (*(sockaddr_in *)addr).sin_addr;
                            printf("\t\tinet:%s\r\n", inet_ntoa(*(struct in_addr *)&ip_address));
                        }
                    }
                }
            }

            {
                netlib_addr_t *addr = netlib_ifconfig_getifnetmask(ifname);
                {
                    if (addr != NULL)
                    {
                        if (addr->sa_family == AF_INET)
                        {
                            //IPV4 NETMASK
                            in_addr ip_address = (*(sockaddr_in *)addr).sin_addr;
                            printf("\t\tinet netmask:%s\r\n", inet_ntoa(*(struct in_addr *)&ip_address));
                        }
                    }
                }
            }
            {
                if (netlib_ifconfig_isup(ifname))
                {
                    printf("\t\tUP\r\n");
                }
            }
            {
                if (netlib_ifconfig_isrunning(ifname))
                {
                    printf("\t\tRUNNING\r\n");
                }
            }
            {
                if (netlib_ifconfig_isloopback(ifname))
                {
                    printf("\t\tLOOPBACK\r\n");
                }
            }
        }

    }

    return 0;
}
