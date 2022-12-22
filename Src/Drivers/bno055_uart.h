/**
 * @file bno055_uart.h
 * @author Paul Nykiel
 * @date 17.04.19
 * @brief Definition of the library functions for the UART communication with the Bosch Sensortec BNO-055.
 */

#ifndef FLIGHTCONTROLLER_BNO055_UART_H
#define FLIGHTCONTROLLER_BNO055_UART_H

#include <stdint.h>

/**
 * Possible results transmitted by the sensor.
 */
typedef enum {
    // From BNO
    read_success = 0x00,
    write_success = 0x01,
    read_fail = 0x02,
    write_fail = 0x03,
    regmap_invalid_address = 0x04,
    regmap_write_disabled = 0x05,
    wrong_start_byte = 0x06,
    bus_over_run_error = 0x07,
    max_length_error = 0x08,
    min_length_error = 0x09,
    receive_char_timeout = 0x0A,

    // From Library
    callback_buffer_invalid,
    invalid_sync
} bno055_response_t;

/**
 * Type of the function called after a transaction is completed.
 */
typedef void (*bno_callback_t)(bno055_response_t);

/**
 * Initialize the uart interface of the sensor.
 *
 * The initialization of the sensor itself (operation mode, units...) is not
 * done here as it is application dependent.
 */
void bno055_uart_init(void);

/**
 * Write to a register of the sensor via UART.
 *
 * This will build the uart message, send it and report the result
 * with the provided callback.
 *
 * @param reg the address of the register
 * @param data pointer to the data to write
 * @param len the number of bytes to write
 * @param callback a function to call once the sensor sends a reply
 */
void bno055_uart_write_register(uint8_t reg, const uint8_t *data, uint8_t len, bno_callback_t callback);

/**
 * Read data from a register via UART.
 *
 * This will build the uart message, send it, decode the incoming
 * response, write the result to the provided address and then call the callback.
 *
 * @param reg the address of the register to read from
 * @param len the number of bytes to read
 * @param callback a function to call once the transaction is finished
 * @param result the location at which to write the result
 */
void bno055_uart_read_register(uint8_t reg, uint8_t len, bno_callback_t callback, void *result);


#endif // FLIGHTCONTROLLER_BNO055_UART_H
