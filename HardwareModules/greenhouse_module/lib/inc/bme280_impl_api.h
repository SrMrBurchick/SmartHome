#ifndef _BME_280_IMPL_API_H_
#define _BME_280_IMPL_API_H_

#include <stdint.h>

#include "common_result.h"
#include "stm32f1xx_hal.h"

/* Structure that contains identifier details */
struct identifier {
	/* Variable to hold device address */
	uint8_t dev_addr;

	/* Variable that contains file descriptor */
	int8_t fd;
};

typedef enum {
	eBmeSensor0 = 0,
	eBmeSensor1,
	eBmeSensor2,
	eBmeSonsorsCount,
} bmeDevices_t;

/******************************************************************************/
/*!                           Functions                                       */

result_t bmeInit(SPI_HandleTypeDef *spi_p);

/*!
 *  @brief Function for reading the sensor's registers through I2C bus.
 *
 *  @param[in] reg_addr       : Register address.
 *  @param[out] data          : Pointer to the data buffer to store the read data.
 *  @param[in] len            : No of bytes to read.
 *  @param[in, out] intf_ptr  : Void pointer that can enable the linking of descriptors
 *                                  for interface related call backs.
 *
 *  @return Status of execution
 *
 *  @retval 0 -> Success
 *  @retval > 0 -> Failure Info
 *
 */
int8_t bmeSpiRead(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr);

/*!
 *  @brief Function that creates a mandatory delay required in some of the APIs.
 *
 * @param[in] period              : Delay in microseconds.
 * @param[in, out] intf_ptr       : Void pointer that can enable the linking of descriptors
 *                                  for interface related call backs
 *  @return void.
 *
 */

void bmeDelayUs(uint32_t period, void *intf_ptr);

/*!
 *  @brief Function for writing the sensor's registers through I2C bus.
 *
 *  @param[in] reg_addr       : Register address.
 *  @param[in] data           : Pointer to the data buffer whose value is to be written.
 *  @param[in] len            : No of bytes to write.
 *  @param[in, out] intf_ptr  : Void pointer that can enable the linking of descriptors
 *                                  for interface related call backs
 *
 *  @return Status of execution
 *
 *  @retval BME280_OK -> Success
 *  @retval BME280_E_COMM_FAIL -> Communication failure.
 *
 */
int8_t bmeSpiWrite(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr);

/*!
 * @brief This API reads the sensor temperature, pressure and humidity data in forced mode.
 *
 *  @param[in] dev       : Pointer to device structure.
 * 	@param[out] comp_data_p  : Pointer to comp data
 *
 *  @return Status of execution
 *
 *  @retval BME280_OK -> Success
 *  @retval BME280_E_COMM_FAIL -> Communication failure.
 */
int8_t bmeStreamSensorDataForcedMode(struct bme280_dev *dev_p, struct bme280_data *comp_data_p);

/*!
 * @brief This API reads the sensor temperature, pressure and humidity data in forced mode.
 *  @param[in] dev       : Pointer to device structure.
 *
 *  @return Status of execution
 *
 *  @retval BME280_OK -> Success
 *  @retval BME280_E_COMM_FAIL -> Communication failure.
 */
int8_t bmeSetSensorSettings(struct bme280_dev *dev);

#endif /*_BME_280_IMPL_API_H_*/