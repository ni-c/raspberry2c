/**
* Raspberry2C
*
* @file main.c
* @brief Main program
* @author Willi Thiel (ni-c@ni-c.de)
* @date Feb 2, 2013
*/

#include <avr/io.h>

#include "i2cslave.h"

/**
* interrupt function handling received bytes
*
* @param addr The address of the last received bytes
*/
void i2c_receive(uint8_t addr) {

}

/**
* The main function.
*/
int main(void) {

    /* LED on */
    DDRD |= (1 << DDD0);
    PORTD |= (1 << PD0);

    /* Initialize I2C slave */
    i2c_slave_init(0x20);

    /* Our loop */
    while (1) {
        if (i2c_buffer[0x00]!=0) {
            PORTD &= ~(1 << PD0);
        }
    }

    /* Finally. (Never ever) */
    return 0;
}
