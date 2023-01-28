/**
 * @file bno055_uart.h
 * @author Paul Nykiel
 * @date 17.04.19
 * @brief Definition of the library functions for the UART communication with the Bosch Sensortec BNO-055.
 * @ingroup Drivers
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
    invalid_sync
} bno055_response_t;

/**
 * Type of the function called after a transaction is completed.
 */
typedef void (*bno055_callback_t)(bno055_response_t);

/**
 * @brief Initialize the uart interface of the sensor.
 *
 * This function initializes the uart with the following values:
 *  * ID: 1
 *  * Baud rate: 115200
 *  * No Parity bits
 *  * 1 Stop Bit
 *  * An internal function as callback
 *
 * The internal callback function decodes data in accordance with the following state machine:
 * \dot
 *
 * digraph {
 *      rankdir = "LR";
 *      INIT -> READ_SUCCESS [
 *          label = "data=0xBB"
 *      ]
 *
 *      INIT -> READ_ERROR_OR_WRITE_RESPONSE [
 *          label = "data=0xEE"
 *      ]
 *
 *      INIT -> INIT [
 *          label = "otherwise/\ncallback(invalid_sync)"
 *      ]
 *
 *      READ_ERROR_OR_WRITE_RESPONSE -> INIT [
 *          label = "/callback(data)";
 *      ]
 *
 *      READ_SUCCESS -> IN_DATA [
 *          label = "/length=data\nindex=0";
 *      ]
 *
 *      IN_DATA -> IN_DATA [
 *          label = "index < length/\nbuffer[index] = data\nindex += 1";
 *      ]
 *
 *      IN_DATA -> INIT [
 *          label = "index >= length/\ncallback(read_success)"
 *      ]
 * }
 *
 * \enddot
 *
 * @see https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bno055-ds000.pdf Chapter 4.7
 * @note The initialization of the sensor itself (operation mode, units...) is not
 * done here as it is application dependent.
 */
void bno055_uart_init(void);

/**
 * @brief Write to a register of the sensor via UART.
 *
 * This function performs the following tasks:
 *  * Build the message in accordance with the BNO055-UART protocol:
 * | Byte | Description | Value |
 * | --- | --- | --- |
 * | 1   | Start byte | 0xAA |
 * | 2   | Write command | 0x00 |
 * | 3   | Register address | The value of the "reg" argument |
 * | 4   | Length | The value of the "len" argument |
 * | 5..5+len | Data | The "len" bytes pointed to by the argument "data" |
 *  * Send the message via UART to ID 2.
 *  * Store the argument "callback" such that the internal uart callback can call this function
 *
 * @param reg the address of the register
 * @param data pointer to the data to write
 * @param len the number of bytes to write
 * @param callback a function to call once the sensor sends a reply
 * @see https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bno055-ds000.pdf Chapter 4.7
 */
void bno055_uart_write_register(uint8_t reg, const uint8_t *data, uint8_t len, bno055_callback_t callback);

/**
 * @brief Read data from a register via UART.
 *
 * This function performs the following tasks:
 *  * Build the message in accordance with the BNO055-UART protocol:
 * | Byte | Description | Value |
 * | --- | --- | --- |
 * | 1   | Start byte | 0xAA |
 * | 2   | Read command | 0x01 |
 * | 3   | Register address | The value of the "reg" argument |
 * | 4   | Length | The value of the "len" argument |
 *  * Send the message via UART to ID 2.
 *  * Store the arguments "callback" and "out" such that the internal uart callback can call this function and write
 *    result data
 *
 * @param reg the address of the register to read from
 * @param len the number of bytes to read
 * @param callback a function to call once the transaction is finished
 * @param result the location at which to write the result
 * @see https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bno055-ds000.pdf Chapter 4.7
 */
void bno055_uart_read_register(uint8_t reg, uint8_t len, bno055_callback_t callback, void *result);


#endif // FLIGHTCONTROLLER_BNO055_UART_H
