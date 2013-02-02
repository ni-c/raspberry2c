/*##############################################################################

 Name	: USI TWI Slave driver - I2C/TWI-EEPROM
 Version	: 1.3  - Stable
 autor	: Martin Junghans	jtronics@gmx.de
 page	: www.jtronics.de
 License	: GNU General Public License

 Created from Atmel source files for Application Note AVR312:
 Using the USI Module as an I2C slave like a I2C-EEPROM.

 //############################################################################*/

#include 	<stdlib.h>
#include 	<avr/io.h>
#include 	<avr/interrupt.h>
#include 	<avr/pgmspace.h>

//################################################################## USI-TWI-I2C

#include 	"i2cslave.h"

// Note: The LSB is the I2C r/w flag and must not be used for addressing!
#define 	SLAVE_ADDR_ATTINY       0x34

#ifndef 	F_CPU
#define 	F_CPU 1000000UL
#endif

//####################################################################### Macros

#define uniq(LOW,HEIGHT)	((HEIGHT << 8)|LOW)			  // Create 16 bit number from two bytes
#define LOW_BYTE(x)        	(x & 0xff)					    // Get low byte from 16 bit number
#define HIGH_BYTE(x)       	((x >> 8) & 0xff)			  // Get high byte from 16 bit number
#define sbi(ADDRESS,BIT) 	((ADDRESS) |= (1<<(BIT)))	// Set bit
#define cbi(ADDRESS,BIT) 	((ADDRESS) &= ~(1<<(BIT)))// Clear bit
#define	toggle(ADDRESS,BIT)	((ADDRESS) ^= (1<<BIT))	// Toggle bit
#define	bis(ADDRESS,BIT)	(ADDRESS & (1<<BIT))		  // Is bit set?
#define	bic(ADDRESS,BIT)	(!(ADDRESS & (1<<BIT)))		// Is bit clear?
//#################################################################### Variables

uint16_t word = 0;				// Counter
uint8_t byte1, byte2;
uint16_t buffer;
uint8_t high, low = 0;	// Variables used with uniq (high and low byte)

//################################################################# Main routine
int main(void) {

	cli();  // Disable interrupts

	usiTwiSlaveInit(SLAVE_ADDR_ATTINY);	// TWI slave init

	sei();  // Re-enable interrupts

	for (int i = 0; i < 10; i++)  // Fill buffers with arbitrary data
		txbuffer[i] = i + 10;

	for (int i = 0; i < 4; i++)
		rxbuffer[i] = i + 20;

	while (1) {
		//############################################ Read data from reception buffer

		// 8 bit variables
		byte1 = rxbuffer[0];
		byte2 = rxbuffer[1];

		// 2 8 bit variables converted into a 16 bit number
		low = rxbuffer[2];
		high = rxbuffer[3];
		word = uniq(low,high);

		//########################################## Write data to transmission buffer

		// 8 bit variables
		txbuffer[0] = byte1;
		txbuffer[1] = byte2;

		// 16 bit variable broken up into its high and low byte
		buffer = word;
		txbuffer[2] = LOW_BYTE(buffer);
		txbuffer[3] = HIGH_BYTE(buffer);

		//############################################################################

	} //end.while
} //end.main

