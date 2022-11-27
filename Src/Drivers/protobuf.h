/**
 * @file protobuf_coding.h
 * @author paul
 * @date 19.11.22
 * Description here TODO
 */
#ifndef FLIGHTCONTROLLER_PROTOBUF_H
#define FLIGHTCONTROLLER_PROTOBUF_H

#include "FlightController.pb.h"
#include "FlightControllerSetpoint.pb.h"

typedef ToolboxPlaneMessages_FlightController fc_message_t;
typedef ToolboxPlaneMessages_FlightControllerSetpoint setpoint_message_t;

void protobuf_init(void);

void protobuf_send_fc(const fc_message_t *message);

bool protobuf_setpoint_available(void);

setpoint_message_t protobuf_get_setpoint(void);

#endif // FLIGHTCONTROLLER_PROTOBUF_H
