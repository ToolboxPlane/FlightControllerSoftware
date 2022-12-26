/**
 * @file flightcomputer.h
 * @author Paul Nykiel
 * @date 14.11.22
 * @brief Declaration of the Flight-Computer interface component.
 * @ingroup Components
 */
#ifndef FLIGHTCONTROLLER_FLIGHTCOMPUTER_H
#define FLIGHTCONTROLLER_FLIGHTCOMPUTER_H

#include <stdbool.h>
#include <stdint.h>

#include "imu.h"
#include "remote.h"
#include "servo_motor.h"

/**
 * Setpoint as sent by the flightcomputer.
 */
typedef struct {
    uint16_t motor; ///< The desired motor velocity (in [0, 1000])
    int16_t pitch; ///< The desired pitch angle (in degree)
    int16_t roll; ///< The desired roll angle (in degree)
} flightcomputer_setpoint_t;

/**
 * Initialize the connection to the flightcomputer by initializing the protobuf module.
 */
void flightcomputer_init(void);

/**
 * Send a message to the flightcomputer by converting the structs to a single protobuf message
 * and sending the message using the protobuf module.
 * @param imu_data the IMU data to send
 * @param remote_data the remote control data to send
 * @param servo_motor_cmd the servo and motor command to send
 */
void flightcomputer_send(const imu_data_t *imu_data, const remote_data_t *remote_data,
                         const servo_motor_cmd_t *servo_motor_cmd);

/**
 * Checks whether the protobuf module decoded a new message.
 * @return true if a new message exists, otherwise false
 */
bool flightcomputer_setpoint_available(void);

/**
 * Gets the latest message from the protobuf module and converts it to a flightcomputer_setpoint_t
 * @return the converted setpoint
 */
flightcomputer_setpoint_t flightcomputer_get_setpoint(void);

#endif // FLIGHTCONTROLLER_FLIGHTCOMPUTER_H
