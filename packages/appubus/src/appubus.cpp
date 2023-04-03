#include "stdlib.h"
#include "libubus.h"
#include "libubox/blobmsg_json.h"
#include <libubox/ustream.h>
#include <thread>
#include <chrono>
#include <mutex>
#include "appubus.h"

/*
通常只需要一个ubus客户端线程，因此采用单例模式
*/
static class UbusStartUp
{
    struct ubus_context *ctx;
    static void ubus_connection_lost_callback(struct ubus_context *ctx)
    {
        uloop_end();
    }
    void run()
    {
        /*
        采用UBUS_SOCKET环境变量作为ubus连接参数
        */
        const char *ubus_socket = getenv("UBUS_SOCKET");
        if (ubus_socket != NULL && strlen(ubus_socket) == 0)
        {
            ubus_socket = NULL;
        }
        while (true)
        {
            if (ctx == NULL)
            {
                ctx = ubus_connect(ubus_socket);
                if (ctx != NULL)
                {
                    ubus_add_uloop(ctx);
                    ctx->connection_lost = ubus_connection_lost_callback;
                }
            }
            else
            {
                //运行uloop
                uloop_run_timeout(1);
                //处理套接字错误
                if (ctx->sock.error)
                {
                    if (ctx->sock.registered)
                    {
                        uloop_fd_delete(&ctx->sock);
                    }
                    ubus_free(ctx);
                    ctx = NULL;
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
public:
    UbusStartUp(): ctx(NULL)
    {
        uloop_init();
        //启动线程
        std::thread([this]()
        {
            run();
        }).detach();
    }
    ~UbusStartUp()
    {
        if (ctx == NULL)
        {
            ubus_free(ctx);
        }
        uloop_done();
    }
    bool IsConnected()
    {
        return ctx != NULL;
    }
} g_ubus;



bool ubus_is_connected()
{
    return g_ubus.IsConnected();
}


