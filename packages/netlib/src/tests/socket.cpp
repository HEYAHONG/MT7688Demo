#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "netlib.h"

int main(int argc, char *argv[])
{
    int sock = netlib_socket();
    printf("socket:%d\r\n", sock);
    netlib_close(sock);

    sock = netlib_socket6();
    printf("socket:%d\r\n", sock);
    netlib_close(sock);

    return 0;
}
