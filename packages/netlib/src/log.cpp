#include "netlib.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdarg.h"
#include "functional"

static std::function<netlib_log_cb> g_cb;
static thread_local std::function<netlib_log_cb> t_cb;

/** \brief 日志函数（通常不直接调用，只调用宏）
 *
 * \param file const char * 文件名
 * \param line int 行号
 * \param log const char* 日志内容
 * \param ... 可变参数，配合log使用
 *
 */
void netlib_log_real(const char *file, int line, const char *log, ...)
{
    va_list args;
    va_start(args, log);
    std::string logmsg;
    logmsg.resize(4096);
    logmsg.assign(4096, '\0');
    vsnprintf((char *)logmsg.c_str(), logmsg.size(), log, args);
    bool IsOutPut = false;
    if (t_cb != NULL)
    {
        //线程回调函数不为空
        t_cb(file, line, logmsg.c_str());
        IsOutPut = true;
    }
    if (g_cb != NULL)
    {
        //全局回调函数不为空
        g_cb(file, line, logmsg.c_str());
        IsOutPut = true;
    }

    if (!IsOutPut)
    {
        printf("%s:%d %s", file, line, logmsg.c_str());
    }
}

/** \brief 设置日志回调函数
 *
 * \param cb netlib_log_cb_t  日志回调函数
 * \param isthread bool 是否为线程设置
 *
 */
void netlib_log_callback_set(netlib_log_cb_t cb, bool isthread)
{
    if (isthread)
    {
        t_cb = cb;
    }
    else
    {
        g_cb = cb;
    }
}
/** \brief 设置日志回调函数
 *
 * \param cb std::function<netlib_log_cb>  日志回调函数
 * \param isthread bool 是否为线程设置
 *
 */
void netlib_log_callback_set(std::function<netlib_log_cb> cb, bool isthread)
{
    if (isthread)
    {
        t_cb = cb;
    }
    else
    {
        g_cb = cb;
    }
}
