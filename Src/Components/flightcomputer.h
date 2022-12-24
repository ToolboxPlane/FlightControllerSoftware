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

typedef struct {
    uint16_t motor;
    int16_t pitch, roll;
} flightcomputer_setpoint_t;

void flightcomputer_init(void);

void flightcomputer_send(const imu_data_t *imu_data, const remote_data_t *remote_data,
                         const servo_motor_cmd_t *servo_motor_cmd);

bool flightcomputer_setpoint_available(void);

flightcomputer_setpoint_t flightcomputer_get_setpoint(void);

#endif // FLIGHTCONTROLLER_FLIGHTCOMPUTER_H
