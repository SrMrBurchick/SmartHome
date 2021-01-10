#include "bme280_sensor.h"

#include "FreeRTOS.h"
#include "logger_api.h"

struct bme280Sensor {
	struct bme280_dev dev;
	struct identifier id;
};

static int8_t st_getSensorMeasurementsData(bme280SensorHandle_t sensorHandle_p, struct bme280_data* sensorData_p) {
	if (BME280_OK != bmeSetSensorSettings(&sensorHandle_p->dev)) {
		return BME280_E_COMM_FAIL;
	}
	return bmeStreamSensorDataForcedMode(&sensorHandle_p->dev, sensorData_p);
}

static result_t st_bme280SensorInit(bme280SensorHandle_t sensorHandle_p, bmeDevices_t deviceId) {
	sensorHandle_p->id.dev_addr = deviceId;
	sensorHandle_p->dev.intf = BME280_SPI_INTF;
	sensorHandle_p->dev.read = bmeSpiRead;
	sensorHandle_p->dev.write = bmeSpiWrite;
	sensorHandle_p->dev.delay_us = bmeDelayUs;
	sensorHandle_p->dev.intf_ptr = &sensorHandle_p->id;
	return BME280_OK != bme280_init(&sensorHandle_p->dev) ? eResultFailed : eResultSucces;
}

bme280SensorHandle_t bme280SensorCreateHandle(bmeDevices_t deviceId) {
	bme280SensorHandle_t sensor_p = (bme280SensorHandle_t)pvPortMalloc(sizeof(struct bme280Sensor));
	if (NULL == sensor_p) {
		logErrorMsg("{Bme280Sensor}: Failed to create sensor");
		return sensor_p;
	}
	if (eResultSucces != st_bme280SensorInit(sensor_p, deviceId)) {
		logErrorMsg("{Bme280Sensor}: Failed to init sensor");
		vPortFree(sensor_p);
		return NULL;
	}
	return sensor_p;
}

int32_t bme280SensorGetTemperature(bme280SensorHandle_t sensorHandle_p) {
	struct bme280_data sensorData = {};
	st_getSensorMeasurementsData(sensorHandle_p, &sensorData);
	return sensorData.temperature;
}

uint32_t bme280SensorGetHumidity(bme280SensorHandle_t sensorHandle_p) {
	struct bme280_data sensorData = {};
	st_getSensorMeasurementsData(sensorHandle_p, &sensorData);
	return sensorData.humidity;
}

uint32_t bme280SensorGetPressure(bme280SensorHandle_t sensorHandle_p) {
	struct bme280_data sensorData = {};
	st_getSensorMeasurementsData(sensorHandle_p, &sensorData);
	return sensorData.pressure;
}
