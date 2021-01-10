#include "bme280_sensor.h"
#include "cmsis_os.h"
#include "logger_api.h"
#include "main.h"
#include "spi.h"
#include "usart.h"
#include "verison.h"

static bme280SensorHandle_t st_firstSensor_p = NULL;
static bme280SensorHandle_t st_secondSensor_p = NULL;
static bme280SensorHandle_t st_thirdSensor_p = NULL;
volatile uint8_t st_isLoggerInit = 0;

static void st_printMeasurements() {
	if (NULL != st_firstSensor_p) {
		logInfoMsg("Sensor 1: temperature: %d, humidity: %u", bme280SensorGetTemperature(st_firstSensor_p),
				   bme280SensorGetHumidity(st_firstSensor_p));
	}
	if (NULL != st_secondSensor_p) {
		logInfoMsg("Sensor 2: temperature: %d, humidity: %u", bme280SensorGetTemperature(st_secondSensor_p),
				   bme280SensorGetHumidity(st_secondSensor_p));
	}
	if (NULL != st_thirdSensor_p) {
		logInfoMsg("Sensor 3: temperature: %d, humidity: %u", bme280SensorGetTemperature(st_thirdSensor_p),
				   bme280SensorGetHumidity(st_thirdSensor_p));
	}
}

result_t sensorMonitor() {
	while (!st_isLoggerInit) {
	}

	result_t result = bmeInit(&hspi2);
	logInfoMsg("Init result %u", result);
	st_firstSensor_p = bme280SensorCreateHandle(eBmeSensor0);
	st_secondSensor_p = bme280SensorCreateHandle(eBmeSensor1);
	st_thirdSensor_p = bme280SensorCreateHandle(eBmeSensor2);
	while (1) {
		st_printMeasurements();
		osDelay(500);
	}
}

result_t startLogger() {
	loggerInit(&huart1);
	st_isLoggerInit = 1;
	loggerStart("*****************Green House Module*****************\n\r"
				"*                  Version:  %d.%02d                  *\n\r"
				"****************************************************\n\r\n\r", VERSION_MAJOR, VERSION_MINOR);
}


void heartBeat() {
	while (1)
	{
		HAL_GPIO_TogglePin(HEART_BEAT_LED_GPIO_Port, HEART_BEAT_LED_Pin);
		osDelay(500);
	}	
}
