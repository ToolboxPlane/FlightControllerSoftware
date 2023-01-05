/**
 * @file error_handler.h
 * @author Paul Nykiel
 * @date 27.11.22
 * @brief Declaration of the error handler application.
 * @ingroup Application
 */
#ifndef FLIGHTCONTROLLER_ERROR_HANDLER_H
#define FLIGHTCONTROLLER_ERROR_HANDLER_H

#include <stdint.h>

/**
 * ID of the component the triggered the error handler.
 */
typedef enum {
    APPLICATION = 1,
    SYSTEM = 2,
    IMU = 3,
    FLIGHTCOMPUTER = 4,
    REMOTE = 5,
    MODE_HANDLER = 6,
    BNO055 = 7
} error_group_t;

/**
 * @brief Initialization of the error handler by initializing the LEDs used for signalling errors.
 *
 * The initialization consists of the following tasks:
 *  * Initializing the Port-L as output by setting all bits in Data-Direction-Register-L
 *  * Disabling all LEDs by setting all bits in Register PORT-L to 0
 */
void error_handler_init(void);

/**
 * @brief Handler for fatal errors, will signal the error and disable the flight-controller.
 *
 * This function performs the following tasks:
 *  * Set LED 1-4 to the binary encoded group ID
 *  * Set LED 5-8 to the binary encoded error ID
 *  * Call ::system_reset_watchdog in an infinite loop
 *
 * @param group the component that triggered the error_handler
 * @param error_id the id of the error of the component
 */
void error_handler_handle_error(error_group_t group, uint8_t error_id);

/**
 * @brief Handler for non-fatal errors, will signal the error and resume nominal operation.
 *
 * This function performs the following tasks:
 *  * Set LED 1-4 to the binary encoded group ID
 *  * Set LED 5-8 to the binary encoded error ID
 *
 * @param group the component that triggered the error_handler
 * @param error_id the id of the error of the component
 */
void error_handler_handle_warning(error_group_t group, uint8_t error_id);

#endif // FLIGHTCONTROLLER_ERROR_HANDLER_H
