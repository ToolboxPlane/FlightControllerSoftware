/**
 * @file flightcomputer.h
 * @author paul
 * @date 14.11.22
 * Description here TODO
 */
#ifndef FLIGHTCONTROLLER_FLIGHTCOMPUTER_H
#define FLIGHTCONTROLLER_FLIGHTCOMPUTER_H

#include <stdbool.h>
#include <stdint.h>

#include "imu.h"
#include "remote.h"

typedef struct {
    uint16_t motor;
    int16_t pitch, roll;
} flightcomputer_setpoint_t;

void flightcomputer_init(void);

void flightcomputer_send(const imu_data_t *imu_data, const remote_data_t *remote_data);

bool flightcomputer_setpoint_available(void);

flightcomputer_setpoint_t flightcomputer_get_setpoint(void);

#endif // FLIGHTCONTROLLER_FLIGHTCOMPUTER_H
