/**
 * @file flight_computer.h
 * @author Paul Nykiel
 * @date 14.11.22
 * @brief Declaration of the Flight-Computer interface component.
 * @ingroup Components
 */
#ifndef FLIGHTCONTROLLER_FLIGHT_COMPUTER_H
#define FLIGHTCONTROLLER_FLIGHT_COMPUTER_H

#include <stdbool.h>
#include <stdint.h>

#include "actuators.h"
#include "imu.h"
#include "remote.h"

/**
 * Set point as sent by the flight-computer.
 */
typedef struct {
    uint16_t motor; ///< The desired motor velocity (in [0, 1000])
    int16_t pitch;  ///< The desired pitch angle (in degree)
    int16_t roll;   ///< The desired roll angle (in degree)
} flight_computer_set_point_t;

/**
 * @brief Initialize the flight-computer module.
 *
 * The initialization consists of the following tasks:
 *  * call ::protobuf_init
 */
void flight_computer_init(void);

/**
 * @brief Send a message to the flight-computer
 *
 * Create a ToolboxPlane::FlightController message with the fields set as:
 *  * remote: the fields of remote_data
 *  * imu: the fields of imu_data
 *  * elevon_left, elevon_right, motor: the fields of actuator_cmd
 *
 * Call ::protobuf_send with the created message as argument.
 *
 * @param imu_data the IMU data to send
 * @param remote_data the remote control data to send
 * @param actuator_cmd the actuator command to send to the FCP
 */
void flight_computer_send(const imu_data_t *imu_data, const remote_data_t *remote_data,
                          const actuator_cmd_t *actuator_cmd);

/**
 * @brief Checks whether the protobuf module decoded a new message.
 *
 * Return the result of ::protobuf_available
 *
 * @return true if a new message exists, otherwise false
 */
bool flight_computer_set_point_available(void);

/**
 * @brief Gets the latest message from the protobuf module and converts it to a flight_computer_set_point_t.
 *
 * Call ::protobuf_get to receive the latest valid protobuf message.
 * Return a flight_computer_set_point_t with the fields set as their respective fields in the message.
 *
 * @return the converted set point
 */
flight_computer_set_point_t flight_computer_get_set_point(void);

#endif // FLIGHTCONTROLLER_FLIGHT_COMPUTER_H
