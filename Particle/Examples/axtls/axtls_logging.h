#ifndef AXTLS_LOGGING_H
#define AXTLS_LOGGING_H
#include "application.h"
extern Logger appLog;
#define debug_tls( fmt, ... ) appLog.info(fmt, ##__VA_ARGS__)
#endif
