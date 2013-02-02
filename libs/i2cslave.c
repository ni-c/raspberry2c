/**
 * Raspberry2C
 *
 * @file 	i2cslave.c
 * @brief 	I2C slave functions
 * @author 	Willi Thiel (ni-c@ni-c.de)
 * @date 	Feb 2, 2013
 */

#include "i2cslave.h"

#include <inttypes.h>

#include <util/twi.h>
#include <avr/interrupt.h>

volatile uint8_t i2c_buffer_addr; /*!< TX buffer address register */

#define TWCR_ACK TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(0<<TWWC); /*!< send ACK after receiving data / wait for ACK after sending data */

#define TWCR_NACK TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)|(0<<TWWC); /*!< send NACK after receiving data / wait for ACK after sending data */

#define TWCR_RESET TWCR = (1<<TWEN)|(1<<TWIE)|(1<<TWINT)|(1<<TWEA)|(0<<TWSTA)|(1<<TWSTO)|(0<<TWWC); /*!< switch to the non adressed slave mode */

/**
 * interrupt funcition handling received bytes
 *
 * @param addr The address of the last received bytes
 */
extern void i2c_receive(uint8_t addr);

/**
 * Initialize the device as I2C slave with the given address
 *
 * @param addr The address of the I2C slave
 */
void i2c_slave_init(uint8_t addr) {
	TWAR = addr;
	TWCR &= ~(1 << TWSTA) | (1 << TWSTO);
	TWCR |= (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
	i2c_buffer_addr = 0xFF;
}

/**
 * TWI ISR
 */
ISR( TWI_vect) {
	uint8_t data = 0;
	switch (TW_STATUS) {
	// Slave Receiver
	case TW_SR_SLA_ACK: // 0x60 slave receiver, slave is addressed
		TWCR_ACK
		; // receive next byte, send ACK
		i2c_buffer_addr = 0xFF; // bufferposition is undefined
		break;

	case TW_SR_DATA_ACK: // 0x80 slave receiver, received a byte
		data = TWDR; // read byte
		if (i2c_buffer_addr == 0xFF) {
			if (data < I2C_BUFFER_SIZE) {
				i2c_buffer_addr = data;
			} else {
				i2c_buffer_addr = 0;
			}
		} else {
			if (i2c_buffer_addr < I2C_BUFFER_SIZE) {
				i2c_buffer[i2c_buffer_addr] = data;  // write data to buffer
			}
			i2c_buffer_addr++;
		}
		TWCR_ACK
		;
		i2c_receive(i2c_buffer_addr - 1);
		break;

    //Slave transmitter
	case TW_ST_SLA_ACK:  //0xA8 slave is addressed for read and answered ACK
	case TW_ST_DATA_ACK:  //0xB8 slave transmitter, data requested
		if (i2c_buffer_addr == 0xFF) {
			i2c_buffer_addr = 0;
		}
		if (i2c_buffer_addr < I2C_BUFFER_SIZE) {
			TWDR = i2c_buffer[i2c_buffer_addr];  // send byte
			i2c_buffer_addr++;  // bufferaddress for next byte
		} else {
			TWDR = 0;  // no more data
		}
		TWCR_ACK
		;
		break;

	case TW_SR_STOP:  // 0xA0 received STOP
		TWCR_ACK
		;
		break;
	case TW_ST_DATA_NACK:  // 0xC0 no more data
	case TW_SR_DATA_NACK:  // 0x88
	case TW_ST_LAST_DATA: // 0xC8 last data byte in TWDR has been transmitted (TWEA = “0”); ACK has been received
	default:
		TWCR_RESET
		;
		break;

	}
}
