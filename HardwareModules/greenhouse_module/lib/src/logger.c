#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "event_groups.h"
#include "logger_api.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"

#define QUEUE_LENGTH (128)
#define ITEM_SIZE (sizeof(uint8_t*))
#define MSG_SIZE (285)
#define LOGGER_QUEUE_TIMEOUT (1000)

#define TRACE "Trace"
#define DEBUG "Debug"
#define INFO "Info"
#define WARNING "Warning"
#define ERROR "Error"

#define NEXT_LINE "\n\r"
#define MSECONDS_PRESCALER (1000)
#define SECONDS_PRESCALER (60)
#define MINUTES_PRESCALER (60)

static const char* st_logPrefix = "[%02d:%02d:%02d:%04d] [%s]: ";
static const uint32_t st_loggerTaskTimeoutMs = 100;

static StaticQueue_t xStaticQueue;
static xQueueHandle st_loggerQueue_p;
static uint8_t st_loggerQueueBuffer[QUEUE_LENGTH * ITEM_SIZE];
static xSemaphoreHandle st_semaphore_p = NULL;
static StaticSemaphore_t st_semaphoreBuffer;

static logLevel_t st_logLevel = eLogLevelInfo;
static uint32_t st_ticksToMs(uint32_t ticks) { return (ticks * MSECONDS_PRESCALER / configTICK_RATE_HZ); }

static void st_sendMsg(const char* log, const char* fmt, va_list vargs) {
	uint8_t* newMsg_p = pvPortMalloc(MSG_SIZE);
	uint32_t hours = 0;
	uint32_t minutes = 0;
	uint32_t seconds = 0;
	uint32_t mseconds = 0;
	uint32_t currentTime = 0;
	currentTime = st_ticksToMs(xTaskGetTickCount());
	mseconds = currentTime;
	seconds = mseconds / MSECONDS_PRESCALER;
	minutes = seconds / SECONDS_PRESCALER;
	hours = minutes / MINUTES_PRESCALER;
	if (minutes >= MINUTES_PRESCALER) {
		minutes = minutes % MINUTES_PRESCALER;
	}
	if (seconds >= SECONDS_PRESCALER) {
		seconds = seconds % SECONDS_PRESCALER;
	}
	if (mseconds >= MSECONDS_PRESCALER) {
		mseconds = mseconds % MSECONDS_PRESCALER;
	}
	memset(newMsg_p, 0, MSG_SIZE);
	sprintf(newMsg_p, st_logPrefix, hours, minutes, seconds, mseconds, log);
	vsprintf(newMsg_p + strlen(newMsg_p), (char*)fmt, vargs);

	if (pdPASS != xQueueSend(st_loggerQueue_p, (void*)&newMsg_p, LOGGER_QUEUE_TIMEOUT)) {
		vPortFree(newMsg_p);
		return;
	}
}

result_t loggerInit(UART_HandleTypeDef* uartHandle_p) {
	st_semaphore_p = xSemaphoreCreateBinaryStatic(&st_semaphoreBuffer);
	if (NULL == st_semaphore_p) {
		return eResultFailed;
	}
	st_loggerQueue_p = xQueueCreateStatic(QUEUE_LENGTH, ITEM_SIZE, st_loggerQueueBuffer, &xStaticQueue);
	if (NULL == st_loggerQueue_p) {
		return eResultFailed;
	}
	return init_console(uartHandle_p);
}

result_t loggerSetLogLevel(logLevel_t level) {
	if (eLogLevelsCount <= level) {
		return eResultFailed;
	}
	if (pdPASS != xSemaphoreTake(st_semaphore_p, LOGGER_QUEUE_TIMEOUT)) {
		return eResultFailed;
	}
	st_logLevel = level;
	xSemaphoreGive(st_semaphore_p);
	return eResultSucces;
}

void logDebugMsg(const char* fmt, ...) {
	if (eLogLevelDebug >= st_logLevel) {
		va_list valist;
		va_start(valist, fmt);
		st_sendMsg(DEBUG, fmt, valist);
		va_end(valist);
	}
}

void logInfoMsg(const char* fmt, ...) {
	if (eLogLevelInfo >= st_logLevel) {
		va_list valist;
		va_start(valist, fmt);
		st_sendMsg(INFO, fmt, valist);
		va_end(valist);
	}
}

void logErrorMsg(const char* fmt, ...) {
	va_list valist;
	va_start(valist, fmt);
	st_sendMsg(ERROR, fmt, valist);
	va_end(valist);
}

void logWarningMsg(const char* fmt, ...) {
	if (eLogLevelWarning >= st_logLevel) {
		va_list valist;
		va_start(valist, fmt);
		st_sendMsg(WARNING, fmt, valist);
		va_end(valist);
	}
}

void logTraceMsg(const char* fmt, ...) {
	if (eLogLevelTrace == st_logLevel) {
		va_list valist;
		va_start(valist, fmt);
		st_sendMsg(TRACE, fmt, valist);
		va_end(valist);
	}
}

void loggerStart(const char* fmt, ...) {
	uint8_t* newMsg_p = pvPortMalloc(MSG_SIZE);
	va_list valist;
	va_start(valist, fmt);
	vsprintf(newMsg_p + strlen(newMsg_p), (char*)fmt, valist);
	va_end(valist);
	write_to_console(newMsg_p, strlen(newMsg_p) + 1);
	while (1) {
		if (NULL != newMsg_p) {
			vPortFree(newMsg_p);
			newMsg_p = NULL;
		}
		if (pdPASS == xQueueReceive(st_loggerQueue_p, &newMsg_p, LOGGER_QUEUE_TIMEOUT)) {
			write_to_console(newMsg_p, strlen(newMsg_p) + 1);
			write_to_console(NEXT_LINE, strlen(NEXT_LINE) + 1);
		}
		vTaskDelay(st_loggerTaskTimeoutMs);
	}
}
