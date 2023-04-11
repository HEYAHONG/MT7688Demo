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
            if (ubus_cli_start_monitor())
            {
                ubus_cli_register_monitor([](ubus_cli_monitor_item & item)
                {
                    Json::StyledWriter writer;
                    printf("\nmonitor %s %08X %08X %d \n%s\n", item.send ? "S" : "R", item.client, item.peer, (int)item.type, writer.write(item.data).c_str());
                });
            }
            else
            {
                printf("start monitor failed\n");
            }

            ubus_cli_register_listen("test_event", [](std::string type, Json::Value & event)
            {
                Json::StyledWriter writer;
                printf("\nevent:%s\n%s\n", type.c_str(), writer.write(event).c_str());
            });
        });
        ubus_cli_register_ondisconnected([]()
        {
            printf("ubus ondisconnected callback\n");
        });
    }
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        printf("ubus is %s\n", ubus_cli_is_connected() ? "connected" : "not connected");

        if (ubus_cli_is_connected())
        {
            ubus_cli_list(std::string(), [](ubus_cli_list_object_item & item)
            {
                Json::StyledWriter writer;
                printf("ubus list\n%s:%08X\n%s\n", item.path.c_str(), item.id, writer.write(item.signature).c_str());
            });

            ubus_cli_call("network.interface.wan", "status", Json::Value(), [](Json::Value result)
            {
                Json::StyledWriter writer;
                printf("\nubus call network.interface.wan status\n%s\n", writer.write(result).c_str());
            });

            {
                ubus_cli_send("ubus_test_1", Json::Value());
                Json::Value msg;
                msg["data"] = 1;
                ubus_cli_send("ubus_test_2", msg);
            }
        }
    }
}
