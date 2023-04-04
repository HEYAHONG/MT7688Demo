#include <stdio.h>
#include <thread>
#include <chrono>
#include "appubus.h"
int main()
{
    printf("main started\n");
    {
        ubus_cli_register_onconnected([]()
        {
            printf("ubus onconnected callback\n");
        });
        ubus_cli_register_ondisconnected([]()
        {
            printf("ubus ondisconnected callback\n");
        });
    }
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        printf("ubus is %s\n", ubus_cli_is_connected() ? "connected" : "not connected");

        if (ubus_cli_is_connected())
        {
            ubus_cli_list(std::string(), [](ubus_cli_list_object_item & item)
            {
                Json::StyledWriter writer;
                printf("ubus list\n%s:%08X\n%s\n", item.path.c_str(), item.id, writer.write(item.signature).c_str());
            });
        }
    }
}
