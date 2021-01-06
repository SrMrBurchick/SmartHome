#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
// #include "FreeRTOS.h"
// #include "task.h"
// #include "timers.h"
// #include "queue.h"
// #include "semphr.h"
// #include "event_groups.h"
#include "logger_api.h"

#define QUEUE_LENGTH    (128)
#define ITEM_SIZE       (sizeof(uint8_t*))
#define MSG_SIZE        (256)
#define LOGGER_QUEUE_TIMEOUT (1000)

#define TRACE "Trace"
#define DEBUG "Debug"
#define INFO "Info"
#define WARNING "Warning"
#define ERROR "Error"

// static StaticQueue_t xStaticQueue;
// static xQueueHandle st_loggerQueue_p; 
// static uint8_t st_loggerQueueBuffer[QUEUE_LENGTH * ITEM_SIZE];
// static xSemaphoreHandle st_semaphore_p = NULL;
// static StaticSemaphore_t st_semaphoreBuffer;

static logLevel_t st_logLevel = eLogLevelInfo;
static uint64_t st_ticksToMs(uint64_t ticks) {
    return ticks * 2;
}

static void st_sendMsg(const char* log, const char* fmt, va_list vargs) {
    //uint8_t* newMsg = pvPortMalloc(MSG_SIZE);
    static uint8_t newMsg[MSG_SIZE] = {0};
    uint64_t hours = 0;
    uint64_t minutes = 0;
    uint64_t seconds = 0;
    uint64_t mseconds = 0;
    uint64_t currentTime = 0;//st_ticksToMs(xTaskGetTickCount());
    // if(NULL == newMsg) {
    //     write_to_console("Pizda!\n\r", sizeof("Pizda!\n\r"));
    //     return;
    // }
    mseconds = currentTime;
    seconds = mseconds / 1000;
    minutes = seconds / 60;
    hours = minutes / 60;
    if(minutes > 60) {
        minutes = minutes % 60;
    }
    if(seconds > 60) {
        seconds = seconds % 60;
    }
    if(mseconds > 1000) {
        mseconds = mseconds % 1000;
    }
	memset(newMsg, 0, MSG_SIZE);
    // sprintf((char*)newMsg, "[%02u:%02u:%02u:%04u] {%s}", hours, minutes, seconds, mseconds, log);
	vsprintf((char*)newMsg + strlen((char*)newMsg), (char*)fmt, vargs);
    memcpy((char*)(newMsg + strlen(newMsg)), "\n\r", sizeof("\n\r"));
    // if( pdPASS != xQueueSend(st_loggerQueue_p, &newMsg, LOGGER_QUEUE_TIMEOUT)) {
    //     vPortFree(newMsg);
    // }
    write_to_console(newMsg, strlen(newMsg) + 1);
    //vPortFree(newMsg);
}

result_t loggerInit(UART_HandleTypeDef* uartHandle_p) {
    // st_semaphore_p = xSemaphoreCreateBinaryStatic(&st_semaphoreBuffer);
    // if(NULL == st_semaphore_p) {
    //     return eResultFailed;
    // }
    // st_loggerQueue_p = xQueueCreateStatic( QUEUE_LENGTH,
    //                              ITEM_SIZE,
    //                              st_loggerQueueBuffer,
    //                              &xStaticQueue );
    // if(NULL == st_loggerQueue_p) {
    //     return eResultFailed;
    // }
    return init_console(uartHandle_p);
}

result_t loggerSetLogLevel(logLevel_t level) {
    if( eLogLevelsCount <= level) {
        return eResultFailed;
    }
    // if(pdPASS != xSemaphoreTake(st_semaphore_p, LOGGER_QUEUE_TIMEOUT)) {
    //     return eResultFailed;
    // }
    st_logLevel = level;
    // xSemaphoreGive(st_semaphore_p);
    return eResultSucces;

}

void logDebugMsg(const char* fmt, ...) {
    if(eLogLevelDebug >= st_logLevel) {
        va_list valist;
        va_start(valist, fmt);
        st_sendMsg(DEBUG, fmt, valist);
        va_end(valist);
    }
}

void logInfoMsg(const char* fmt, ...) {
    if(eLogLevelInfo >= st_logLevel) {
        va_list valist;
        va_start(valist, fmt);
        st_sendMsg(INFO, fmt, valist);
        va_end(valist);
    }
}

void logErrorMsg(const char* fmt, ...){
    va_list valist;
	va_start(valist, fmt);
	st_sendMsg(ERROR, fmt, valist);
	va_end(valist);
}

void logWarningMsg(const char* fmt, ...) {
    if(eLogLevelWarning >= st_logLevel) {
        va_list valist;
        va_start(valist, fmt);
        st_sendMsg(WARNING, fmt, valist);
        va_end(valist);
    }
}

void logTraceMsg(const char* fmt, ...) {
    if(eLogLevelTrace == st_logLevel) {
        va_list valist;
        va_start(valist, fmt);
        st_sendMsg(TRACE, fmt, valist);
        va_end(valist);
    }
}

void loggerStart(const char* helloMsg) {
    // uint8_t* newMsg_p = pvPortMalloc(MSG_SIZE);
    // memset(newMsg_p, 0, MSG_SIZE);
    // sprintf((char*)newMsg_p, "%s\n\r", helloMsg);
    write_to_console(helloMsg, strlen(helloMsg) + 1);
    while (1)
    {
        // if(NULL != newMsg_p) {
        //     vPortFree(newMsg_p);
        // }
        // newMsg_p = NULL;
        // if(pdPASS == xQueueReceive(st_loggerQueue_p, &newMsg_p, LOGGER_QUEUE_TIMEOUT)) {
        //     write_to_console(newMsg_p, strlen(newMsg_p) + 1);
        // }
        vTaskDelay(100);
    }
    
}
