#include <stdio.h>
#include <thread>
#include <chrono>
#include "appubus.h"
int main()
{
    printf("main started\n");
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        printf("ubus is %s\n", ubus_is_connected() ? "connected" : "not connected");
    }
}
