#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "netlib.h"

int main(int argc, char *argv[])
{
    NETLIB_LOG("netlib_version:%s\r\n", netlib_getversion());

    {
        //回调测试
        auto cb = [](const char *file, int line, const char *log)
        {
            printf("CallBack \r\n%s:%d %s", file, line, log);
        };
        netlib_log_callback_set(cb);
        NETLIB_LOG("netlib_version:%s\r\n", netlib_getversion());
    }

    return 0;
}
