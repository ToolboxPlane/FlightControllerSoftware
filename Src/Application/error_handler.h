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
 * Initialization of the error handler by initializing the LEDs used for signalling errors.
 */
void error_handler_init(void);

/**
 * Handle any error occurred by the flight-controller. This will signal the error on the LEDs and
 * stops the flight-computer.
 * @param group the component that triggered the error_handler
 * @param error_id the id of the error of the component
 */
void error_handler_handle_error(error_group_t group, uint8_t error_id);

/**
 * Handle any error occurred by the flight-controller. This will signal the error on the LEDs and
 * but then resumes nominal operation.
 * @param group the component that triggered the error_handler
 * @param error_id the id of the error of the component
 */
void error_handler_handle_warning(error_group_t group, uint8_t error_id);

#endif // FLIGHTCONTROLLER_ERROR_HANDLER_H
