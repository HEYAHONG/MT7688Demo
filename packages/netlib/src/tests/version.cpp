#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "netlib.h"

int main(int argc, char *argv[])
{
    return printf("netlib_version:%s\r\n", netlib_getversion());
}
