#ifndef _LOGGER_API_H_
#define _LOGGER_API_H_

#include "console_api.h"

typedef enum {
    eLogLevelError = 0,
    eLogLevelWarning,
    eLogLevelInfo,
    eLogLevelDebug,
    eLogLevelTrace,
    eLogLevelsCount
}logLevel_t;

result_t loggerInit(UART_HandleTypeDef* uartHandle_p);

result_t loggerSetLogLevel(logLevel_t level);
void logDebugMsg(const char* fmt, ...);
void logInfoMsg(const char* fmt, ...);
void logErrorMsg(const char* fmt, ...);
void logWarningMsg(const char* fmt, ...);
void logTraceMsg(const char* fmt, ...);
void loggerStart(const char* fmt, ...);


#endif /*_LOGGER_API_H_*/