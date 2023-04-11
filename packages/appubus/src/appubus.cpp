#include "appubus.h"
#include "libubus.h"
extern "C"
{
#include "libubox/blobmsg_json.h"
#include <libubox/ustream.h>
}


#define MAX_CALLBACK_HANDLER_NUMBER 32

static std::string ubus_cli_get_monitor_data(struct blob_attr *data)
{
    struct blob_attr_info policy[UBUS_ATTR_MAX] = {0};
    policy[UBUS_ATTR_STATUS].type = BLOB_ATTR_INT32;
    policy[UBUS_ATTR_OBJPATH].type = BLOB_ATTR_STRING;
    policy[UBUS_ATTR_OBJID].type = BLOB_ATTR_INT32;
    policy[UBUS_ATTR_METHOD].type = BLOB_ATTR_STRING ;
    policy[UBUS_ATTR_OBJTYPE].type = BLOB_ATTR_INT32 ;
    policy[UBUS_ATTR_SIGNATURE].type = BLOB_ATTR_NESTED;
    policy[UBUS_ATTR_DATA].type = BLOB_ATTR_NESTED ;
    policy[UBUS_ATTR_ACTIVE].type = BLOB_ATTR_INT8 ;
    policy[UBUS_ATTR_NO_REPLY].type = BLOB_ATTR_INT8 ;
    policy[UBUS_ATTR_USER].type = BLOB_ATTR_STRING ;
    policy[UBUS_ATTR_GROUP].type = BLOB_ATTR_STRING ;

    const char   *names[UBUS_ATTR_MAX] = {0};
    names[UBUS_ATTR_STATUS] = "status";
    names[UBUS_ATTR_OBJPATH] = "objpath";
    names[UBUS_ATTR_OBJID] = "objid";
    names[UBUS_ATTR_METHOD] = "method";
    names[UBUS_ATTR_OBJTYPE] = "objtype";
    names[UBUS_ATTR_SIGNATURE] = "signature";
    names[UBUS_ATTR_DATA] = "data";
    names[UBUS_ATTR_ACTIVE] = "active";
    names[UBUS_ATTR_NO_REPLY] = "no_reply";
    names[UBUS_ATTR_USER] = "user";
    names[UBUS_ATTR_GROUP] = "group";

    struct blob_attr *tb[UBUS_ATTR_MAX];
    blob_parse(data, tb, policy, UBUS_ATTR_MAX);

    int i;
    struct blob_buf b = {0};
    blob_buf_init(&b, 0);
    for (i = 0; i < UBUS_ATTR_MAX; i++)
    {
        const char *n = names[i];
        struct blob_attr *v = tb[i];

        if (!tb[i] || !n)
            continue;

        switch (policy[i].type)
        {
        case BLOB_ATTR_INT32:
            blobmsg_add_u32(&b, n, blob_get_int32(v));
            break;
        case BLOB_ATTR_STRING:
            blobmsg_add_string(&b, n, (const char *)blob_data(v));
            break;
        case BLOB_ATTR_INT8:
            blobmsg_add_u8(&b, n, !!blob_get_int8(v));
            break;
        case BLOB_ATTR_NESTED:
            blobmsg_add_field(&b, BLOBMSG_TYPE_TABLE, n, blobmsg_data(v), blobmsg_data_len(v));
            break;
        }
    }

    char *jsonstr = blobmsg_format_json(b.head, true);
    std::string ret;
    if (jsonstr != NULL)
    {
        ret = jsonstr;
        free(jsonstr);
    }
    blob_buf_free(&b);
    return ret;
}

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

    //monitor回调函数
    bool IsInMonitor;
    std::map<uint32_t, std::function<void(ubus_cli_monitor_item &)>> OnUbusMonitorCallback;
    std::mutex OnUbusMonitorCallbackLock;
    static void monitor(struct ubus_context *ctx, uint32_t seq, struct blob_attr *data);
    void OnUbusMonitor(struct ubus_context *ctx, uint32_t seq, struct blob_attr *msg)
    {
        struct blob_attr_info policy[UBUS_MONITOR_MAX] = {0};
        policy[UBUS_MONITOR_CLIENT].type = BLOB_ATTR_INT32 ;
        policy[UBUS_MONITOR_PEER].type = BLOB_ATTR_INT32 ;
        policy[UBUS_MONITOR_SEND].type = BLOB_ATTR_INT8 ;
        policy[UBUS_MONITOR_TYPE].type = BLOB_ATTR_INT32 ;
        policy[UBUS_MONITOR_DATA].type = BLOB_ATTR_NESTED ;
        struct blob_attr *tb[UBUS_MONITOR_MAX];
        blob_parse_untrusted(msg, blob_raw_len(msg), tb, policy, UBUS_MONITOR_MAX);

        if (!tb[UBUS_MONITOR_CLIENT] ||
                !tb[UBUS_MONITOR_PEER] ||
                !tb[UBUS_MONITOR_SEND] ||
                !tb[UBUS_MONITOR_TYPE] ||
                !tb[UBUS_MONITOR_DATA])
        {
            return;
        }

        bool send = blob_get_int32(tb[UBUS_MONITOR_SEND]);
        uint32_t client = blob_get_int32(tb[UBUS_MONITOR_CLIENT]);
        uint32_t peer = blob_get_int32(tb[UBUS_MONITOR_PEER]);
        uint32_t type = blob_get_int32(tb[UBUS_MONITOR_TYPE]);
        std::string data = ubus_cli_get_monitor_data(tb[UBUS_MONITOR_DATA]);
        ubus_cli_monitor_item item;
        item.client = client;
        item.peer = peer;
        item.send = send;
        item.seq = seq;
        item.type = type;
        {
            Json::Reader reader;
            reader.parse(data, item.data);
        }

        std::map<uint32_t, std::function<void(ubus_cli_monitor_item &)>> cb;
        {
            std::lock_guard<std::mutex> lock(OnUbusMonitorCallbackLock);
            cb =  OnUbusMonitorCallback;
        }
        for (auto callback : cb)
        {
            if (callback.second != NULL)
            {
                try
                {
                    callback.second(item);
                }
                catch (...)
                {

                }
            }
        }
    }

    //ubus listen
    class ubus_listen_item_t
    {
        typedef struct
        {
            struct ubus_event_handler evt_handler;
            ubus_listen_item_t *ptr;
        } ubus_event_handler_with_ptr;
        std::function<void(std::string, Json::Value &)> event_data_callback;
        std::shared_ptr<ubus_event_handler_with_ptr> evt;
        UbusStartUp *startup;
        void clear()
        {
            event_data_callback = NULL;
            if (evt.get() != NULL)
            {
                memset(evt.get(), 0, sizeof(ubus_event_handler_with_ptr));
            }
        }
    public:
        ubus_listen_item_t(UbusStartUp *_startup = NULL): evt(std::make_shared<ubus_event_handler_with_ptr>()), startup(_startup)
        {
            clear();
        };
        ubus_listen_item_t &operator=(ubus_listen_item_t &others) = default;
        ubus_listen_item_t &operator=(ubus_listen_item_t &&others) = default;
        bool setcallback(std::string pattem, std::function<void(std::string, Json::Value &)> cb)
        {
            if (pattem.empty())
            {
                pattem = "*";
            }
            if (isvalid())
            {
                //清除以前的事件注册
                if (startup != NULL)
                {
                    struct ubus_event_handler &evt_handler = (*(struct ubus_event_handler *)evt.get());
                    ubus_unregister_event_handler(startup->ctx, &evt_handler);
                }
                clear();
            }
            event_data_callback = cb;
            if (event_data_callback != NULL)
            {
                //回调函数不为空
                evt.get()->ptr = this;
                struct ubus_event_handler &evt_handler = (*(struct ubus_event_handler *)evt.get());
                evt_handler.cb = [](struct ubus_context * ctx, struct ubus_event_handler * ev, const char *type, struct blob_attr * msg)
                {
                    char *json_str = blobmsg_format_json(msg, true);
                    if (json_str != NULL)
                    {
                        Json::Value value;
                        Json::Reader reader;
                        if (reader.parse(json_str, value))
                        {
                            try
                            {
                                ubus_event_handler_with_ptr *evt = (ubus_event_handler_with_ptr *)ev;
                                if (evt != NULL && evt->ptr != NULL)
                                {
                                    std::function<void(std::string, Json::Value &)> &event_data_callback = evt->ptr->event_data_callback;
                                    if (event_data_callback != NULL)
                                    {
                                        event_data_callback(type, value);
                                    }
                                }
                            }
                            catch (...)
                            {

                            }
                        }
                        free(json_str);
                    }
                };
                if (startup != NULL)
                {
                    if (UBUS_STATUS_OK == ubus_register_event_handler(startup->ctx, &evt_handler, pattem.c_str()))
                    {
                        return true;
                    }
                }
            }

            return false;
        }
        bool isvalid()
        {
            return event_data_callback != NULL;
        }
    };
    std::map<uint32_t, ubus_listen_item_t> UbusListenList;
    std::mutex UbusListenListLock;




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
                    ctx->monitor_cb = monitor;
                    IsInMonitor = false;
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
                    IsInMonitor = false;
                    {
                        //断开连接后listen失效
                        std::lock_guard<std::mutex> lock(UbusListenListLock);
                        UbusListenList.clear();
                    }
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
public:
    //构造函数（启动Ubus连接）
    UbusStartUp(): ctx(NULL), loop_timer({0}), IsInMonitor(false)
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

    //ubus list
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
    bool UbusList(std::string path, std::function<void(ubus_cli_list_object_item &)> result, std::function<void()> error)
    {
        if (result != NULL && IsConnected())
        {
            auto cb = [path, result, error, this]()
            {
                const  char *lookup_path = NULL;
                if (path.length() > 0)
                {
                    lookup_path = path.c_str();
                }
                if (UBUS_STATUS_INVALID_ARGUMENT == ubus_lookup(ctx, lookup_path, ubus_lookup_handler, (void *)&result))
                {
                    if (error != NULL)
                    {
                        try
                        {
                            error();
                        }
                        catch (...)
                        {

                        }
                    }
                }
            };
            AddLoopAction(cb);
            return true;
        }
        return false;
    }

    //monitor
    bool UbusMonitorStart()
    {
        if (!IsConnected())
        {
            return false;
        }
        if (!IsInMonitor)
        {
            if (UBUS_STATUS_OK == ubus_monitor_start(ctx))
            {
                IsInMonitor = true;
            }
            else
            {
                return false;
            }
        }
        return true;
    }

    bool UbusMonitorStop()
    {
        if (!IsConnected())
        {
            return false;
        }
        if (IsInMonitor)
        {
            if (UBUS_STATUS_OK == ubus_monitor_stop(ctx))
            {
                IsInMonitor = false;
            }
            else
            {
                return false;
            }

        }

        return true;
    }

    bool UbusIsInMonitor()
    {
        return IsInMonitor;
    }
    uint32_t RegisterOnUbusMonitor(std::function<void(ubus_cli_monitor_item &)> _cb)
    {
        for (uint32_t i = 0; i <= MAX_CALLBACK_HANDLER_NUMBER; i++)
        {
            if (OnUbusMonitorCallback.find(i) != OnUbusMonitorCallback.end())
            {
                if (OnUbusMonitorCallback[i] == NULL)
                {
                    std::lock_guard<std::mutex> lock(OnUbusMonitorCallbackLock);
                    OnUbusMonitorCallback[i] = _cb;
                    return i;
                }
            }
            else
            {
                std::lock_guard<std::mutex> lock(OnUbusMonitorCallbackLock);
                OnUbusMonitorCallback[i] = _cb;
                return i;
            }
        }
        return UBUS_INVALID_CALLBACK_ID;
    }
    void UnRegisterOnUbusMonitor(uint32_t id)
    {
        if (OnUbusMonitorCallback.find(id) != OnUbusMonitorCallback.end())
        {
            std::lock_guard<std::mutex> lock(OnUbusMonitorCallbackLock);
            OnUbusMonitorCallback[id] = NULL;
        }
    }


    //ubus call
    static void ubus_call_receive_call_result_data(struct ubus_request *req, int type, struct blob_attr *msg)
    {
        std::function<void(Json::Value)> &result = *(std::function<void(Json::Value)> *)req->priv;
        char *json_str = blobmsg_format_json(msg, true);
        if (json_str != NULL)
        {
            Json::Value value;
            Json::Reader reader;
            if (reader.parse(json_str, value))
            {
                if (result != NULL)
                {
                    try
                    {
                        result(value);
                    }
                    catch (...)
                    {

                    }
                }
            }
            free(json_str);
        }
    }
    bool UbusCall(std::string path, std::string method, Json::Value msg, std::function<void(Json::Value)> result, size_t timeout_ms, std::function<void()> error)
    {
        if (path.empty() || method.empty())
        {
            return false;
        }

        if (!IsConnected())
        {
            return false;
        }

        auto cb = [ =, this]()
        {
            uint32_t id = 0;
            if (UBUS_STATUS_OK != ubus_lookup_id(ctx, path.c_str(), &id))
            {
                if (error != NULL)
                {
                    try
                    {
                        error();
                    }
                    catch (...)
                    {

                    }
                }
                return;
            }
            blob_buf b = {0};
            blob_buf_init(&b, 0);
            if (msg.isObject())
            {
                Json::FastWriter writer;
                blobmsg_add_json_from_string(&b, writer.write(msg).c_str());
            }
            int ret = ubus_invoke(ctx, id, method.c_str(), b.head, ubus_call_receive_call_result_data, (void *)&result, timeout_ms);
            blob_buf_free(&b);
            if (ret != UBUS_STATUS_OK)
            {
                if (error != NULL)
                {
                    try
                    {
                        error();
                    }
                    catch (...)
                    {

                    }
                }
            }
        };
        AddLoopAction(cb);

        return true;
    }

    //ubus send
    bool UbusSend(std::string id, Json::Value msg, std::function<void()> error)
    {
        if (id.empty())
        {
            return false;
        }
        if (!IsConnected())
        {
            return false;
        }
        auto cb = [this, id, msg, error]()
        {
            blob_buf b = {0};
            blob_buf_init(&b, 0);
            if (msg.isObject())
            {
                Json::FastWriter writer;
                blobmsg_add_json_from_string(&b, writer.write(msg).c_str());
            }
            int ret = ubus_send_event(ctx, id.c_str(), b.head);
            blob_buf_free(&b);
            if (ret != UBUS_STATUS_OK)
            {
                if (error != NULL)
                {
                    try
                    {
                        error();
                    }
                    catch (...)
                    {

                    }
                }
            }
        };
        AddLoopAction(cb);
        return false;
    }

    //ubus listen
    uint32_t RegisterUbusListen(std::string pattem, std::function<void(std::string, Json::Value &)> callback)
    {
        if (callback == NULL)
        {
            return UBUS_INVALID_CALLBACK_ID;
        }
        for (uint32_t i = 0; i <= MAX_CALLBACK_HANDLER_NUMBER; i++)
        {
            if (UbusListenList.find(i) != UbusListenList.end())
            {
                if (!UbusListenList[i].isvalid())
                {
                    auto cb = [this, pattem, callback, i]()
                    {
                        UbusListenList[i].setcallback(pattem, callback);
                    };
                    AddLoopAction(cb);
                    return i;
                }
            }
            else
            {
                {
                    std::lock_guard<std::mutex> lock(UbusListenListLock);
                    UbusListenList[i] = ubus_listen_item_t(this);
                }
                auto cb = [this, pattem, callback, i]()
                {
                    UbusListenList[i].setcallback(pattem, callback);
                };
                AddLoopAction(cb);
                return i;
            }
        }
        return UBUS_INVALID_CALLBACK_ID;
    }

    void UnRegisterUbusListen(uint32_t id)
    {
        if (UbusListenList.find(id) != UbusListenList.end())
        {
            auto cb = [this, id]()
            {
                UbusListenList[id].setcallback("", NULL);
            };
            AddLoopAction(cb);
        }
    }

} g_ubus;

void UbusStartUp::uloop_timeout_handler(struct uloop_timeout *t)
{
    g_ubus.OnUbusLoopTimer(t);
}

void UbusStartUp::monitor(struct ubus_context *ctx, uint32_t seq, struct blob_attr *data)
{
    g_ubus.OnUbusMonitor(ctx, seq, data);
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

bool ubus_cli_list(std::string path, std::function<void(ubus_cli_list_object_item &)> result, std::function<void()> error)
{
    return g_ubus.UbusList(path, result, error);
}

bool ubus_cli_is_in_monitor()
{
    return g_ubus.UbusIsInMonitor();
}

bool ubus_cli_start_monitor()
{
    return g_ubus.UbusMonitorStart();
}

bool ubus_cli_stop_monitor()
{
    return g_ubus.UbusMonitorStop();
}

uint32_t ubus_cli_register_monitor(std::function<void(ubus_cli_monitor_item &)> _cb)
{
    return g_ubus.RegisterOnUbusMonitor(_cb);
}

void ubus_cli_unregister_monitor(uint32_t id)
{
    g_ubus.UnRegisterOnUbusMonitor(id);
}

bool ubus_cli_call(std::string path, std::string method, Json::Value msg, std::function<void(Json::Value)> result, size_t timeout_ms, std::function<void()> error)
{
    return g_ubus.UbusCall(path, method, msg, result, timeout_ms, error);
}

bool ubus_cli_send(std::string id, Json::Value msg, std::function<void()> error)
{
    return g_ubus.UbusSend(id, msg, error);
}

uint32_t ubus_cli_register_listen(std::string pattem, std::function<void(std::string, Json::Value &)> callback)
{
    return g_ubus.RegisterUbusListen(pattem, callback);
}

void ubus_cli_unregister_listen(uint32_t id)
{
    g_ubus.UnRegisterUbusListen(id);
}
