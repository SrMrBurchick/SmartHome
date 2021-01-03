#ifndef _CONSOLE_API_H_
#define _CONSOLE_API_H_

#include "stm32f1xx_hal.h"
#include "common_result.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct console_handle *console_handle_t;

result_t init_console(UART_HandleTypeDef* uartHandle_p);
result_t write_to_console(const char* msg, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /*_CONSOLE_API_H_*/