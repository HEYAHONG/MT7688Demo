#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <netlib.h>
#include <unistd.h>
#include "log.h"
#include <chrono>
#include "args.h"
#include "onenet.h"

static void welcome()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    LOGINFO("Started at %s", std::asctime(std::localtime(&in_time_t)));
}

int main(int argc, char *argv[])
{
    //输出欢迎信息
    welcome();

    //检查参数
    args_parse(argc, argv);

    //打印参数
    args_print();

    //初始化OneNET
    OneNETInit();

    //openwrt中Daemon程序不用转入后台，但不能退出。
    while (true)
    {
        sleep(5);
    }
}
