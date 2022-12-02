#ifndef LOG_H
#define LOG_H

#include "syslog.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#define SYSLOG_IDENT "MQTTDaemon"

#ifndef LOGINFO
#include "syslog.h"
#define LOGINFO_REAL(...)  {syslog(LOG_DAEMON|LOG_INFO,__VA_ARGS__);}
#define LOGINFO(fmt,...) LOGINFO_REAL(fmt,##__VA_ARGS__)
#endif // LOGINFO

#ifndef LOGDEBUG
#include "syslog.h"
#define LOGDEBUG_REAL(...)  {syslog(LOG_DAEMON|LOG_DEBUG,__VA_ARGS__);}
#define LOGDEBUG(fmt,...) LOGDEBUG_REAL(fmt,##__VA_ARGS__)
#endif // LOGDEBUG


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LOG_H
