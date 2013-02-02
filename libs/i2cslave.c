/**
 * Name: USI TWI Slave driver - I2C/TWI-EEPROM
 * Version: 1.3  - Stable
 * Autor: Martin Junghans	jtronics@gmx.de
 * Page: www.jtronics.de
 * License: GNU General Public License
 *
 * Update: 09.09.2011 - ATtiny24, ATtiny44, ATtiny88
 *
 * Created from Atmel source files for Application Note AVR312:
 * Using the USI Module as an I2C slave like an I2C-EEPROM.

 * 02.02.2013 - reworked by Willi Thiel
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "i2cslave.h"

#define SET_USI_TO_SEND_ACK( ) 	{ USIDR = 0; \
								DDR_USI |= ( 1 << PORT_USI_SDA ); \
								USISR = ( 0 << USI_START_COND_INT ) | \
								( 1 << USIOIF ) | ( 1 << USIPF ) | \
								( 1 << USIDC )| \
								( 0x0E << USICNT0 );} /*!< Prepare ACK; Set SDA as output; Clear all interrupt flags, except Start Cond; Set USI counter to shift 1 bit */

#define SET_USI_TO_READ_ACK( ) 	{ USIDR = 0; \
								DDR_USI &= ~( 1 << PORT_USI_SDA ); \
								USISR = ( 0 << USI_START_COND_INT ) | \
								( 1 << USIOIF) | \
								( 1 << USIPF ) | \
								( 1 << USIDC ) | \
								( 0x0E << USICNT0 );} /*!< Set SDA as input; Prepare ACK; Clear all interrupt flags, except Start Cond; Set USI counter to shift 1 bit */

#define SET_USI_TO_TWI_START_CONDITION_MODE( ) { \
								USICR = ( 1 << USISIE ) | ( 0 << USIOIE ) | \
								( 1 << USIWM1 ) | ( 0 << USIWM0 ) | \
								( 1 << USICS1 ) | ( 0 << USICS0 ) | ( 0 << USICLK ) | \
								( 0 << USITC ); \
								USISR = ( 0 << USI_START_COND_INT ) | ( 1 << USIOIF ) | ( 1 << USIPF ) | \
								( 1 << USIDC ) | ( 0x0 << USICNT0 ); } /*!< Enable Start Condition Interrupt, disable Overflow Interrupt; Set USI in Two-wire mode, no USI Counter overflow hold; Shift Register Clock Source = External, positive edge; 4-Bit Counter Source = external, both edges; No toggle clock-port pin; Clear all interrupt flags, except Start Cond */

#define SET_USI_TO_SEND_DATA( ) { DDR_USI |=  ( 1 << PORT_USI_SDA ); \
								USISR = ( 0 << USI_START_COND_INT ) | ( 1 << USIOIF ) | ( 1 << USIPF ) | \
								( 1 << USIDC) | \
								( 0x0 << USICNT0 ); \
								} /*!< Set SDA as output; Clear all interrupt flags, except Start Cond; Set USI to shift out 8 bits */

#define SET_USI_TO_READ_DATA( ) { DDR_USI &= ~( 1 << PORT_USI_SDA ); \
								USISR =	( 0 << USI_START_COND_INT ) | ( 1 << USIOIF ) | \
								( 1 << USIPF ) | ( 1 << USIDC ) | \
								( 0x0 << USICNT0 ); \
								} /*!< Set SDA as input; Clear all interrupt flags, except Start Cond; Set USI to shift out 8 bits */

typedef enum {
    USI_SLAVE_CHECK_ADDRESS = 0x00, USI_SLAVE_SEND_DATA = 0x01, USI_SLAVE_REQUEST_REPLY_FROM_SEND_DATA = 0x02, USI_SLAVE_CHECK_REPLY_FROM_SEND_DATA = 0x03, USI_SLAVE_REQUEST_DATA = 0x04, USI_SLAVE_GET_DATA_AND_SEND_ACK = 0x05
} overflow_state_t;

volatile uint8_t slave_address; /*!< Address of the I2C slave */
volatile overflow_state_t overflow_state; /*!< overflow state of the I2C slave */

/**
 * Initialize slave
 *
 * @param address The address of the I2C slave device
 */
void i2c_init(uint8_t address) {
    slave_address = address;

    // In Two Wire mode (USIWM1, USIWM0 = 1X), the slave USI will pull SCL
    // low when a start condition is detected or a counter overflow (only
    // for USIWM1, USIWM0 = 11).  This inserts a wait state. SCL is released
    // by the ISRs (USI_START_vect and USI_OVERFLOW_vect).

    // Set SCL and SDA as output
    DDR_USI |= (1 << PORT_USI_SCL) | (1 << PORT_USI_SDA);

    // Set SCL high
    PORT_USI |= (1 << PORT_USI_SCL);

    // Set SDA high
    PORT_USI |= (1 << PORT_USI_SDA);

    // Set SDA as input
    DDR_USI &= ~(1 << PORT_USI_SDA);

    USICR = (1 << USISIE) |  // Enable Start Condition Interrupt
            (0 << USIOIE) |  // Disable Overflow Interrupt
            (1 << USIWM1) | (0 << USIWM0) |  // Set USI in Two-wire mode, no USI Counter overflow hold
            (1 << USICS1) | (0 << USICS0) | (0 << USICLK) |  // Shift Register Clock Source = external, positive edge 4-Bit Counter Source = external, both edges
            (0 << USITC);  // No toggle clock-port pin

    // clear all interrupt flags and reset overflow counter
    USISR = (1 << USI_START_COND_INT) | (1 << USIOIF) | (1 << USIPF) | (1 << USIDC);
}

/**
 * USI Start Condition ISR
 */
ISR( USI_START_VECTOR) {

    // Set default starting conditions for new TWI package
    overflow_state = USI_SLAVE_CHECK_ADDRESS;

    // Set SDA as input
    DDR_USI &= ~(1 << PORT_USI_SDA);

    // Wait for SCL to go low to ensure the Start Condition has completed (the
    // Start detector will hold SCL low ) - if a Stop Condition arises then leave
    // The interrupt to prevent waiting forever - don't use USISR to test for Stop
    // Condition as in Application Note AVR312 because the Stop Condition Flag is
    // going to be set from the last TWI sequence

    while ((PIN_USI & (1 << PIN_USI_SCL)) && !((PIN_USI & (1 << PIN_USI_SDA))))
        ;	// SCL his high and SDA is low

    // A Stop Condition did not occur
    if (!(PIN_USI & (1 << PIN_USI_SDA))) {
        USICR = (1 << USISIE) |  // Keep Start Condition Interrupt enabled to detect RESTART
                (1 << USIOIE) |  // Enable Overflow Interrupt
                (1 << USIWM1) | (1 << USIWM0) |  // Set USI in Two-wire mode, hold SCL low on USI Counter overflow
                (1 << USICS1) | (0 << USICS0) | (0 << USICLK) |  // 4-Bit Counter Source = external, both edges; Clock Source = External, positive edge
                (0 << USITC);  // No toggle clock-port pin

    } else {
        // A Stop Condition did occur
        USICR = (1 << USISIE) |  // Enable Start Condition Interrupt
                (0 << USIOIE) |  // Disable Overflow Interrupt
                (1 << USIWM1) | (0 << USIWM0) |  // Set USI in Two-wire mode, no USI Counter overflow hold
                (1 << USICS1) | (0 << USICS0) | (0 << USICLK) |  // 4-Bit Counter Source = external, both edges; Clock Source = external, positive edge
                (0 << USITC);  // No toggle clock-port pin
    }

    USISR = (1 << USI_START_COND_INT) | (1 << USIOIF) |  // Clear interrupt flags - resetting the Start Condition Flag will release SCL
            (1 << USIPF) | (1 << USIDC) | (0x0 << USICNT0);  // Set USI to sample 8 bits (count 16 external SCL pin toggles)
}

/**
 * ISR( USI_OVERFLOW_VECTOR )
 *
 * Handles all the communication. Only disabled when waiting for a new Start Condition.
 */
ISR( USI_OVERFLOW_VECTOR) {
    uint8_t data = 0;
    switch (overflow_state) {

        /**
         * Address mode: check address and send ACK (and next USI_SLAVE_SEND_DATA) if OK, else reset USI
         */
        case USI_SLAVE_CHECK_ADDRESS:
            // If adress is either 0 or own address
            if (USIDR == 0 || (USIDR & ~1) == slave_address) {
                if (USIDR & 0x01) {
                    // Master Write Data Mode - Slave transmit
                    overflow_state = USI_SLAVE_SEND_DATA;
                } else {
                    // Master Read Data Mode - Slave receive
                    overflow_state = USI_SLAVE_REQUEST_DATA;
                    // Buffer position undefined
                    buffer_adr = 0xFF;
                }  // end if
                SET_USI_TO_SEND_ACK()
            } else {
                SET_USI_TO_TWI_START_CONDITION_MODE()
            }
            break;

            /**
             * Master Write Data Mode - Slave transmit
             */
            // Check reply and goto USI_SLAVE_SEND_DATA if OK, else reset USI
        case USI_SLAVE_CHECK_REPLY_FROM_SEND_DATA:
            if (USIDR) {
                // If NACK, the master does not want more data
                SET_USI_TO_TWI_START_CONDITION_MODE()
                return;
            }

            // From here we just drop straight into USI_SLAVE_SEND_DATA if the master sent an ACK
        case USI_SLAVE_SEND_DATA:
            // No buffer position given, set buffer address to 0
            if (buffer_adr == 0xFF) {
                buffer_adr = 0;
            }

            // Send data byte
            USIDR = i2c_buffer[buffer_adr];

            // Increment buffer address for next byte
            buffer_adr++;

            overflow_state = USI_SLAVE_REQUEST_REPLY_FROM_SEND_DATA;
            SET_USI_TO_SEND_DATA()
            break;

            // Set USI to sample reply from master
            // Next USI_SLAVE_CHECK_REPLY_FROM_SEND_DATA
        case USI_SLAVE_REQUEST_REPLY_FROM_SEND_DATA:
            overflow_state = USI_SLAVE_CHECK_REPLY_FROM_SEND_DATA;
            SET_USI_TO_READ_ACK()
            break;

            /**
             * Master Read Data Mode - Slave receive
             */
            // Set USI to sample data from master,
            // Next USI_SLAVE_GET_DATA_AND_SEND_ACK
        case USI_SLAVE_REQUEST_DATA:
            overflow_state = USI_SLAVE_GET_DATA_AND_SEND_ACK;
            SET_USI_TO_READ_DATA( )
            break;

            // Copy data from USIDR and send ACK
            // Next USI_SLAVE_REQUEST_DATA
        case USI_SLAVE_GET_DATA_AND_SEND_ACK:
            // Read data received
            data = USIDR;

            // First access, read buffer position
            if (buffer_adr == 0xFF) {
                // Check if address within buffer size
                if (data <= buffer_size) {
                    // Set position as received
                    buffer_adr = data;
                } else {
                    // Set address to 0
                    buffer_adr = 0;
                }
            } else {
                // Ongoing access, receive data

                // Write data to buffer
                i2c_buffer[buffer_adr] = data;

                // Increment buffer address for next write access
                buffer_adr++;
            }
            // Next USI_SLAVE_REQUEST_DATA
            overflow_state = USI_SLAVE_REQUEST_DATA;
            SET_USI_TO_SEND_ACK( )
            break;
    }	// end switch
}	// end ISR( USI_OVERFLOW_VECTOR )
