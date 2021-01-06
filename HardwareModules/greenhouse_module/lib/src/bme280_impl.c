#include <string.h>
#include <stdint.h>
#include "bme280_impl_api.h"
#include "main.h"
#include "cmsis_os.h"
#include "bme280_defs.h"
#include "logger_api.h"

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

result_t bmeInit(SPI_HandleTypeDef *spi_p) {
	if (NULL == spi_p) {
		return eResultFailed;
	}
	st_spiHndl_p = spi_p;
    for (uint8_t i = 0; i < eBmeSonsorsCount; i++)
    {
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
    if(HAL_OK != HAL_SPI_TransmitReceive(st_spiHndl_p, st_writeBuffer, st_readBuffer, len + 1, 1000)) {
        rslt = -1;
    }
	
    memcpy(data, st_readBuffer + 1, len);
    st_resetCsPin(id.dev_addr);

	return rslt;
}

void bmeDelayUs(uint32_t period, void *intf_ptr) {
    osDelay(period);
}

int8_t bmeSpiWrite(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr) {
	int8_t rslt = BME280_OK; /* Return 0 for Success, non-zero for failure */
	struct identifier id;
    memset(st_writeBuffer, 0, 256);

	id = *((struct identifier *)intf_ptr);
	st_setCsPin(id.dev_addr);
    st_writeBuffer[0] = reg_addr;
    memcpy(st_writeBuffer + 1, data, len);
	if(HAL_OK != HAL_SPI_Transmit(st_spiHndl_p, st_writeBuffer, len + 1, 1000)) {
        rslt = -1;
    }
    st_resetCsPin(id.dev_addr);
	return rslt;
}
