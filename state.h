/**
 * @file state.h
 * @author paul
 * @date 16.04.19
 * @brief state @TODO
 */

#ifndef FLIGHTCONTROLLER_STATE_H
#define FLIGHTCONTROLLER_STATE_H

#include <stdint.h>

typedef int16_t angle_t;

typedef struct {
    angle_t heading, pitch, roll;
    int16_t acc_forward, acc_side, acc_updown;
} state_t;

#endif //FLIGHTCONTROLLER_STATE_H
