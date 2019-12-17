/**
 * @file state.h
 * @author paul
 * @date 16.04.19
 * @brief state @TODO
 */

#ifndef FLIGHTCONTROLLER_STATE_H
#define FLIGHTCONTROLLER_STATE_H

#include <stdint.h>
#include "Drivers/bno055_uart.h"

typedef struct {
    int16_t heading, pitch, roll; // in [-180,180]
    int16_t heading_deriv, pitch_deriv, roll_deriv;
    bno055_status_t bno_state;
} state_t;

#endif //FLIGHTCONTROLLER_STATE_H
