#include "appubus.h"
#include "libubus.h"
extern "C"
{
#include "libubox/blobmsg_json.h"
#include <libubox/ustream.h>
}


#define MAX_CALLBACK_HANDLER_NUMBER 32

/*
通常只需要一个ubus客户端线程，因此采用单例模式
*/
static class UbusStartUp
{
    //ubus上下文(C)
    struct ubus_context *ctx;
    static void ubus_connection_lost_callback(struct ubus_context *ctx)
    {
        uloop_end();
    }

    //连接回调
    std::map<uint32_t, std::function<void()>> OnUbusConnectedCallback;
    std::mutex OnUbusConnectedCallbackLock;
    void OnUbusConnected()
    {
        std::map<uint32_t, std::function<void()>> cb;
        {
            std::lock_guard<std::mutex> lock(OnUbusConnectedCallbackLock);
            cb = OnUbusConnectedCallback;
        }
        for (auto callback : cb)
        {
            if (callback.second != NULL)
            {
                try
                {
                    callback.second();
                }
                catch (...)
                {

                }

            }
        }
    }

    //断开连接回调
    std::map<uint32_t, std::function<void()>> OnUbusDisconnectedCallback;
    std::mutex OnUbusDisconnectedCallbackLock;
    void OnUbusDisconnected()
    {
        std::map<uint32_t, std::function<void()>> cb;
        {
            std::lock_guard<std::mutex> lock(OnUbusDisconnectedCallbackLock);
            cb = OnUbusDisconnectedCallback;
        }
        for (auto callback : cb)
        {
            if (callback.second != NULL)
            {
                try
                {
                    callback.second();
                }
                catch (...)
                {

                }
            }
        }
    }

    //loop定时器回调
    std::queue<std::function<void()>> LoopActionCache;
    std::mutex LoopActionCacheLock;
    struct uloop_timeout loop_timer;
    static void uloop_timeout_handler(struct uloop_timeout *t);
    void OnUbusLoopTimer(struct uloop_timeout *t)
    {
        std::queue<std::function<void()>> actions;
        {
            std::lock_guard<std::mutex> lock(LoopActionCacheLock);
            while (!LoopActionCache.empty())
            {
                actions.push(LoopActionCache.front());
                LoopActionCache.pop();
            }
        }
        while (!actions.empty())
        {
            std::function<void()> cb = actions.front();
            if (cb != NULL)
            {
                try
                {
                    cb();
                }
                catch (...)
                {

                }
            }
            actions.pop();
        }
        uloop_timeout_set(&loop_timer, 10);
    }

    //运行主循环
    void run()
    {
        //设置timeout回调函数
        loop_timer.cb = uloop_timeout_handler;

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
                    OnUbusConnected();
                }
                else
                {
                    //减小连接频率
                    std::this_thread::sleep_for(std::chrono::seconds(3));
                }
            }
            else
            {
                //设置loop_timer
                uloop_timeout_set(&loop_timer, 10);
                //运行uloop
                uloop_run();
                //处理套接字错误
                if (ctx->sock.error)
                {
                    uloop_fd_delete(&ctx->sock);
                    OnUbusDisconnected();
                    ubus_free(ctx);
                    ctx = NULL;
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
public:
    //构造函数（启动Ubus连接）
    UbusStartUp(): ctx(NULL), loop_timer({0})
    {
        uloop_init();
        //启动线程
        std::thread([this]()
        {
            run();
        }).detach();
    }
    //析构函数
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


    //连接回调函数
    uint32_t RegisterOnUbusConnected(std::function<void()> _cb)
    {
        for (uint32_t i = 0; i <= MAX_CALLBACK_HANDLER_NUMBER; i++)
        {
            if (OnUbusConnectedCallback.find(i) != OnUbusConnectedCallback.end())
            {
                if (OnUbusConnectedCallback[i] == NULL)
                {
                    std::lock_guard<std::mutex> lock(OnUbusConnectedCallbackLock);
                    OnUbusConnectedCallback[i] = _cb;
                    return i;
                }
            }
            else
            {
                std::lock_guard<std::mutex> lock(OnUbusConnectedCallbackLock);
                OnUbusConnectedCallback[i] = _cb;
                return i;
            }
        }
        return UBUS_INVALID_CALLBACK_ID;
    }
    void UnRegisterOnUbusConnected(uint32_t id)
    {
        if (OnUbusConnectedCallback.find(id) != OnUbusConnectedCallback.end())
        {
            std::lock_guard<std::mutex> lock(OnUbusConnectedCallbackLock);
            OnUbusConnectedCallback[id] = NULL;
        }
    }

    //断开连接回调函数
    uint32_t RegisterOnUbusDisconnected(std::function<void()> _cb)
    {
        for (uint32_t i = 0; i <= MAX_CALLBACK_HANDLER_NUMBER; i++)
        {
            if (OnUbusDisconnectedCallback.find(i) != OnUbusDisconnectedCallback.end())
            {
                if (OnUbusDisconnectedCallback[i] == NULL)
                {
                    std::lock_guard<std::mutex> lock(OnUbusDisconnectedCallbackLock);
                    OnUbusDisconnectedCallback[i] = _cb;
                    return i;
                }
            }
            else
            {
                std::lock_guard<std::mutex> lock(OnUbusDisconnectedCallbackLock);
                OnUbusDisconnectedCallback[i] = _cb;
                return i;
            }
        }
        return UBUS_INVALID_CALLBACK_ID;
    }
    void UnRegisterOnUbusDisconnected(uint32_t id)
    {
        if (OnUbusDisconnectedCallback.find(id) != OnUbusDisconnectedCallback.end())
        {
            std::lock_guard<std::mutex> lock(OnUbusDisconnectedCallbackLock);
            OnUbusDisconnectedCallback[id] = NULL;
        }
    }


    //添加动作到循环
    void AddLoopAction(std::function<void()> cb)
    {
        std::lock_guard<std::mutex> lock(LoopActionCacheLock);
        LoopActionCache.push(cb);
    }

    static void ubus_lookup_handler(struct ubus_context *ctx, struct ubus_object_data *obj, void *priv)
    {
        std::function<void(ubus_cli_list_object_item &)> &result = (*(std::function<void(ubus_cli_list_object_item &)> *)priv);
        ubus_cli_list_object_item item;
        item.id = obj->id;
        item.path = obj->path;
        if (obj->signature != NULL)
        {
            char *json_str = blobmsg_format_json(obj->signature, true);
            if (json_str != NULL)
            {
                Json::Reader reader;
                reader.parse(json_str, item.signature);
                free(json_str);
            }
        }
        if (result != NULL)
        {
            result(item);
        }
    }
    bool UbusList(std::string path, std::function<void(ubus_cli_list_object_item &)> result)
    {
        if (result != NULL && IsConnected())
        {
            auto cb = [path, result, this]()
            {
                const  char *lookup_path = NULL;
                if (path.length() > 0)
                {
                    lookup_path = path.c_str();
                }
                ubus_lookup(ctx, lookup_path, ubus_lookup_handler, (void *)&result);
            };
            AddLoopAction(cb);
            return true;
        }
        return false;
    }

} g_ubus;

void UbusStartUp::uloop_timeout_handler(struct uloop_timeout *t)
{
    g_ubus.OnUbusLoopTimer(t);
}

bool ubus_cli_is_connected()
{
    return g_ubus.IsConnected();
}

uint32_t ubus_cli_register_onconnected(std::function<void()> _cb)
{
    return g_ubus.RegisterOnUbusConnected(_cb);
}

void ubus_cli_unregister_onconnected(uint32_t id)
{
    g_ubus.UnRegisterOnUbusConnected(id);
}

uint32_t ubus_cli_register_ondisconnected(std::function<void()> _cb)
{
    return g_ubus.RegisterOnUbusDisconnected(_cb);
}

void ubus_cli_unregister_ondisconnected(uint32_t id)
{
    g_ubus.UnRegisterOnUbusDisconnected(id);
}

bool ubus_cli_list(std::string path, std::function<void(ubus_cli_list_object_item &)> result)
{
    return g_ubus.UbusList(path, result);
}




