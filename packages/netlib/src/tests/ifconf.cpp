#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "netlib.h"

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
        }

    }

    return 0;
}
