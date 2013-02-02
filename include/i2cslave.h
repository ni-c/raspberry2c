/**
 * Raspberry2C
 *
 * @file 	i2cslave.h
 * @brief   I2C slave definitions
 * @author 	Willi Thiel (ni-c@ni-c.de)
 * @date 	Feb 2, 2013
 */

#ifndef I2CSLAVE_H_
#define I2CSLAVE_H_

#include <inttypes.h>

#define I2C_BUFFER_SIZE 4

volatile uint8_t i2c_buffer[I2C_BUFFER_SIZE + 1]; /*!< The I2C Buffer */

/**
 * Initialize the device as I2C slave with the given address
 *
 * @param addr The address of the I2C slave
 */
void i2c_slave_init(uint8_t addr);

#endif /* I2CSLAVE_H_ */
