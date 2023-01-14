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
 * Type of the messages sent to the flight-computer.
 */
typedef ToolboxPlaneMessages_FlightController fc_message_t;

/**
 * Type of the messages received from the flight-computer.
 */
typedef ToolboxPlaneMessages_FlightControllerSetpoint set_point_message_t;

/**
 * Initialize the protobuf connection.
 */
void protobuf_init(void);

/**
 * Send a message to the flight-computer.
 * @param message the message to send.
 */
void protobuf_send(const fc_message_t *message);

/**
 * Decodes all data received since the last call and returns whether the data contains a full new message.
 * @return true if a new message was received, otherwise false.
 */
bool protobuf_available(void);

/**
 * Get the last message that was received.
 * @return a copy of the last message.
 */
set_point_message_t protobuf_get(void);

#endif // FLIGHTCONTROLLER_PROTOBUF_H
