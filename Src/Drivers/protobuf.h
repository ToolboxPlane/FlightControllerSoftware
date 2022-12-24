/**
 * @file protobuf.h
 * @author Paul Nykiel
 * @date 19.11.22
 * @brief Definition of the library functions for protobuf encoding, transmission and reception.
 * @ingroup Drivers
 */
#ifndef FLIGHTCONTROLLER_PROTOBUF_H
#define FLIGHTCONTROLLER_PROTOBUF_H

#include "FlightController.pb.h"
#include "FlightControllerSetpoint.pb.h"

/**
 * Type of the messages sent to the flightcomputer.
 */
typedef ToolboxPlaneMessages_FlightController fc_message_t;

/**
 * Type of the messages received from the flightcomputer.
 */
typedef ToolboxPlaneMessages_FlightControllerSetpoint setpoint_message_t;

/**
 * Initialize the protobuf connection.
 */
void protobuf_init(void);

/**
 * Send a message to the flightcomputer.
 * @param message the message to send.
 */
void protobuf_send(const fc_message_t *message);

bool protobuf_setpoint_available(void);

setpoint_message_t protobuf_get_setpoint(void);

#endif // FLIGHTCONTROLLER_PROTOBUF_H
