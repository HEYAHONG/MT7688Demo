#include "log.h"

class log_gaurd
{
public:
    log_gaurd()
    {
        openlog(SYSLOG_IDENT, LOG_NDELAY | LOG_NOWAIT | LOG_PID, LOG_DAEMON);
    }
    ~log_gaurd()
    {
        closelog();
    }
};
static log_gaurd __log;
