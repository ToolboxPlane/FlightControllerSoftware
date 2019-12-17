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
    int16_t heading; // in [0, 360]
    int16_t pitch, roll; // in [-180,180]
    int16_t heading_deriv, pitch_deriv, roll_deriv;
    int16_t acc_forward, acc_side, acc_updown;
    bno055_error_t bno_error;
    bno055_status_t bno_state;
    uint8_t bno_calib;
} state_t;

#endif //FLIGHTCONTROLLER_STATE_H
