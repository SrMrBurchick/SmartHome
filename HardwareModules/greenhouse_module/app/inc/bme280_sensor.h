#ifndef _BME280_SENSOR_H_
#define _BME280_SENSOR_H_

#include <stdint.h>
#include "bme280.h"
#include "bme280_defs.h"
#include "bme280_impl_api.h"

struct bme280Sensor;

typedef struct bme280Sensor* bme280SensorHandle_t;

bme280SensorHandle_t bme280SensorCreateHandle(bmeDevices_t deviceId);

int32_t bme280SensorGetTemperature(bme280SensorHandle_t sensorHandle_p);

uint32_t bme280SensorGetHumidity(bme280SensorHandle_t sensorHandle_p);

uint32_t bme280SensorGetPressure(bme280SensorHandle_t sensorHandle_p);

#endif /*_BME280_SENSOR_H_*/