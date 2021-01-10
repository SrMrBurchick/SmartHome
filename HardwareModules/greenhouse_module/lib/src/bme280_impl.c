#include <stdint.h>
#include <string.h>

#include "bme280.h"
#include "bme280_defs.h"
#include "bme280_impl_api.h"
#include "cmsis_os.h"
#include "logger_api.h"
#include "main.h"

static SPI_HandleTypeDef *st_spiHndl_p = NULL;
static uint8_t st_writeBuffer[256] = {0};
static uint8_t st_readBuffer[256] = {0};

static void st_setCsPin(uint8_t id) {
	switch ((bmeDevices_t)id) {
		case eBmeSensor0:
			HAL_GPIO_WritePin(BOSCH_BM_0_CS_GPIO_Port, BOSCH_BM_0_CS_Pin, GPIO_PIN_RESET);
			break;
		case eBmeSensor1:
			HAL_GPIO_WritePin(BOSCH_BM_1_CS_GPIO_Port, BOSCH_BM_1_CS_Pin, GPIO_PIN_RESET);
			break;
		case eBmeSensor2:
			HAL_GPIO_WritePin(BOSCH_BM_2_CS_GPIO_Port, BOSCH_BM_2_CS_Pin, GPIO_PIN_RESET);
			break;

		default:
			break;
	}
}

static void st_resetCsPin(uint8_t id) {
	switch ((bmeDevices_t)id) {
		case eBmeSensor0:
			HAL_GPIO_WritePin(BOSCH_BM_0_CS_GPIO_Port, BOSCH_BM_0_CS_Pin, GPIO_PIN_SET);
			break;
		case eBmeSensor1:
			HAL_GPIO_WritePin(BOSCH_BM_1_CS_GPIO_Port, BOSCH_BM_1_CS_Pin, GPIO_PIN_SET);
			break;
		case eBmeSensor2:
			HAL_GPIO_WritePin(BOSCH_BM_2_CS_GPIO_Port, BOSCH_BM_2_CS_Pin, GPIO_PIN_SET);
			break;

		default:
			break;
	}
}

static void st_printSensorData(uint8_t dev_id, struct bme280_data *comp_data) {
	int32_t temp, press, hum;
	temp = comp_data->temperature;
	press = comp_data->pressure;
	hum = comp_data->humidity;
	logInfoMsg("{Sensor %u}: %d,%d deg C, %d hPa, %d,%d proc", dev_id, temp / 100, temp % 100, press, hum / 1024, hum % 1024);
}

result_t bmeInit(SPI_HandleTypeDef *spi_p) {
	if (NULL == spi_p) {
		return eResultFailed;
	}
	st_spiHndl_p = spi_p;
	for (uint8_t i = 0; i < eBmeSonsorsCount; i++) {
		st_resetCsPin(i);
	}

	return eResultSucces;
}

int8_t bmeSpiRead(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr) {
	int8_t rslt = BME280_OK; /* Return 0 for Success, non-zero for failure */
	struct identifier id;
	memset(st_writeBuffer, 0, 256);
	memset(st_readBuffer, 0, 256);

	id = *((struct identifier *)intf_ptr);

	st_setCsPin(id.dev_addr);
	st_writeBuffer[0] = reg_addr;
	if (HAL_OK != HAL_SPI_TransmitReceive(st_spiHndl_p, st_writeBuffer, st_readBuffer, len + 1, 1000)) {
		rslt = -1;
	}

	memcpy(data, st_readBuffer + 1, len);
	st_resetCsPin(id.dev_addr);

	return rslt;
}

void bmeDelayUs(uint32_t period, void *intf_ptr) { osDelay(period); }

int8_t bmeSpiWrite(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr) {
	int8_t rslt = BME280_OK; /* Return 0 for Success, non-zero for failure */
	struct identifier id;
	memset(st_writeBuffer, 0, 256);

	id = *((struct identifier *)intf_ptr);
	st_setCsPin(id.dev_addr);
	st_writeBuffer[0] = reg_addr;
	memcpy(st_writeBuffer + 1, data, len);
	if (HAL_OK != HAL_SPI_Transmit(st_spiHndl_p, st_writeBuffer, len + 1, 1000)) {
		rslt = -1;
	}
	st_resetCsPin(id.dev_addr);
	return rslt;
}

int8_t bmeStreamSensorDataForcedMode(struct bme280_dev *dev_p, struct bme280_data *comp_data_p) {
	int8_t rslt = BME280_E_COMM_FAIL;
	uint32_t req_delay = 0;
	struct identifier *id_p;
	if (NULL == comp_data_p || NULL == dev_p) {
		logErrorMsg("%s. Invalid parameters", __FUNCTION__);
		return rslt;
	}

	/*Calculate the minimum delay required between consecutive measurement based upon the sensor enabled
	 *  and the oversampling configuration. */
	req_delay = bme280_cal_meas_delay(&dev_p->settings);
	/* Continuously stream sensor data */
	/* Wait for the measurement to complete and print data @25Hz */
	rslt = bme280_set_sensor_mode(BME280_FORCED_MODE, dev_p);
	if (rslt != BME280_OK) {
		logErrorMsg("Failed to set sensor mode (code %+d).", rslt);
		return rslt;
	}
	dev_p->delay_us(req_delay, dev_p->intf_ptr);
	id_p = (struct identifier *)dev_p->intf_ptr;
	rslt = bme280_get_sensor_data(BME280_ALL, comp_data_p, dev_p);

	return rslt;
}

int8_t bmeSetSensorSettings(struct bme280_dev *dev_p) {
	int8_t rslt;
	uint8_t settings_sel;
	uint32_t req_delay = 0;

	/* Recommended mode of operation: Indoor navigation */
	dev_p->settings.osr_h = BME280_OVERSAMPLING_1X;
	dev_p->settings.osr_p = BME280_OVERSAMPLING_16X;
	dev_p->settings.osr_t = BME280_OVERSAMPLING_2X;
	dev_p->settings.filter = BME280_FILTER_COEFF_16;
	settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;

	rslt = bme280_set_sensor_settings(settings_sel, dev_p);

	return rslt;
}