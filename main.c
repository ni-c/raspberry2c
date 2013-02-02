/**
 * Raspberry2C (https://github.com/ni-c/raspberry2c)
 *
 * @file    main.c
 * @brief   Main program
 * @author  Willi Thiel (ni-c@ni-c.de)
 * @date    Feb 2, 2013
 */

#include 	<stdlib.h>
#include 	<avr/io.h>
#include 	<avr/interrupt.h>
#include 	<avr/pgmspace.h>

#include 	"i2cslave.h"
#define 	I2CADDRESS 0x34

#ifndef 	F_CPU
#define 	F_CPU 1000000UL
#endif

void init(void) {
    // Disable interrupts
    cli();

    // TWI slave init
    i2c_init(I2CADDRESS);

    // Re-enable interrupts
    sei();

    for (int i = 0; i < buffer_size; i++)
        i2c_buffer[i] = 0;
}

int main(void) {

    init();

    // Set port 1 to output
    DDRD |= (1 << DDD0);

	while (1) {
	    if (i2c_buffer[0] == 0) {
	        // port d0 low
            PORTD &= ~(1 << PD0);
		} else {
		    // port d0 high
            PORTD |= (1 << PD0);
		}
	}
}
