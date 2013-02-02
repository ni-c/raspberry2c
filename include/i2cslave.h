/*
 *
 * Name: USI TWI Slave driver - I2C/TWI-EEPROM
 * Version: 1.3  - Stable
 * autor: Martin Junghans	jtronics@gmx.de
 * page: www.jtronics.de
 * License: GNU General Public License
 *
 * Created from Atmel source files for Application Note AVR312:
 * Using the USI Module as an I2C slave like an I2C-EEPROM.
 *
 * LICENSE: Copyright (C) 2010 Marin Junghans
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details
 *
 * 16.11.2010 - reworked by Markus Schatzl
 *
 * 02.02.2013 - reworked by Willi Thiel
 */

#ifndef _I2CSLAVE_H_
#define _I2CSLAVE_H_

/**
 * Initialize slave
 *
 * @param address The address of the I2C slave device
 */
void i2c_init(uint8_t address);

#define buffer_size 16 /*!< I2C buffer size (in bytes (2..254)) */
volatile uint8_t i2c_buffer[buffer_size]; /*!< I2C buffer */
volatile uint8_t buffer_adr; /*!< Virtual buffer address register */

#if 	(buffer_size > 254)
#error Buffer to big! 254 bytes max.

#elif 	(buffer_size < 2)
#error Buffer to small! 2 bytes min.
#endif

#if     defined( __AVR_ATtiny2313__ )
#define DDR_USI             DDRB
#define PORT_USI            PORTB
#define PIN_USI             PINB
#define PORT_USI_SDA        PB5
#define PORT_USI_SCL        PB7
#define PIN_USI_SDA         PINB5
#define PIN_USI_SCL         PINB7
#define USI_START_COND_INT  USISIF
#define USI_START_VECTOR    USI_START_vect
#define USI_OVERFLOW_VECTOR USI_OVERFLOW_vect

#elif   defined( __AVR_ATtiny24A__ ) | \
        defined( __AVR_ATtiny44A__ ) | \
        defined( __AVR_ATtiny24__ )  | \
        defined( __AVR_ATtiny44__ )  | \
        defined( __AVR_ATtiny84__ )
#define DDR_USI             DDRA
#define PORT_USI            PORTA
#define PIN_USI             PINA
#define PORT_USI_SDA        PA6
#define PORT_USI_SCL        PA4
#define PIN_USI_SDA         PINA6
#define PIN_USI_SCL         PINA4
#define USI_START_COND_INT  USISIF
#define USI_START_VECTOR    USI_STR_vect
#define USI_OVERFLOW_VECTOR USI_OVF_vect

#elif   defined( __AVR_ATtiny25__ ) | \
        defined( __AVR_ATtiny45__ ) | \
        defined( __AVR_ATtiny85__ )
#define DDR_USI             DDRB
#define PORT_USI            PORTB
#define PIN_USI             PINB
#define PORT_USI_SDA        PB0
#define PORT_USI_SCL        PB2
#define PIN_USI_SDA         PINB0
#define PIN_USI_SCL         PINB2
#define USI_START_COND_INT  USISIF
#define USI_START_VECTOR    USI_START_vect
#define USI_OVERFLOW_VECTOR USI_OVF_vect

#elif   defined( __AVR_ATtiny26__ )
#define DDR_USI             DDRB
#define PORT_USI            PORTB
#define PIN_USI             PINB
#define PORT_USI_SDA        PB0
#define PORT_USI_SCL        PB2
#define PIN_USI_SDA         PINB0
#define PIN_USI_SCL         PINB2
#define USI_START_COND_INT  USISIF
#define USI_START_VECTOR    USI_STRT_vect
#define USI_OVERFLOW_VECTOR USI_OVF_vect

#elif   defined( __AVR_ATtiny261__ ) | \
        defined( __AVR_ATtiny461__ ) | \
        defined( __AVR_ATtiny861__ )
#define DDR_USI             DDRB
#define PORT_USI            PORTB
#define PIN_USI             PINB
#define PORT_USI_SDA        PB0
#define PORT_USI_SCL        PB2
#define PIN_USI_SDA         PINB0
#define PIN_USI_SCL         PINB2
#define USI_START_COND_INT  USISIF
#define USI_START_VECTOR    USI_START_vect
#define USI_OVERFLOW_VECTOR USI_OVF_vect

#elif   defined( __AVR_ATmega165__ ) | \
        defined( __AVR_ATmega325__ ) | \
        defined( __AVR_ATmega3250__ ) | \
        defined( __AVR_ATmega645__ ) | \
        defined( __AVR_ATmega6450__ ) | \
        defined( __AVR_ATmega329__ ) | \
        defined( __AVR_ATmega3290__ )
#define DDR_USI             DDRE
#define PORT_USI            PORTE
#define PIN_USI             PINE
#define PORT_USI_SDA        PE5
#define PORT_USI_SCL        PE4
#define PIN_USI_SDA         PINE5
#define PIN_USI_SCL         PINE4
#define USI_START_COND_INT  USISIF
#define USI_START_VECTOR    USI_START_vect
#define USI_OVERFLOW_VECTOR USI_OVERFLOW_vect

#elif   defined( __AVR_ATmega169__ )
#define DDR_USI             DDRE
#define PORT_USI            PORTE
#define PIN_USI             PINE
#define PORT_USI_SDA        PE5
#define PORT_USI_SCL        PE4
#define PIN_USI_SDA         PINE5
#define PIN_USI_SCL         PINE4
#define USI_START_COND_INT  USISIF
#define USI_START_VECTOR    USI_START_vect
#define USI_OVERFLOW_VECTOR USI_OVERFLOW_vect

#elif   defined( __AVR_ATtiny24__ ) | \
        defined( __AVR_ATtiny44__ ) | \
        defined( __AVR_ATtiny84__ )
#define DDR_USI             DDRA
#define PORT_USI            PORTA
#define PIN_USI             PINA
#define PORT_USI_SDA        PA6
#define PORT_USI_SCL        PA4
#define PIN_USI_SDA         PINA6
#define PIN_USI_SCL         PINA4
#define USI_START_COND_INT  USISIF
#define USI_START_VECTOR    USI_STR_vect
#define USI_OVERFLOW_VECTOR USI_OVF_vect

#else
#error "no USI-Slave definition for MCU available"

#endif

#endif  // ifndef _I2CSLAVE_H_
