/**
 * @file control.h
 * @author paul
 * @date 16.04.19
 * @brief control @TODO
 */

#ifndef FLIGHTCONTROLLER_OUT_STATE_H
#define FLIGHTCONTROLLER_OUT_STATE_H

#include <stdint.h>

typedef struct {
    uint16_t aileron_r, aileron_l;
    uint16_t vtail_r, vtail_l;
    uint16_t motor;
} out_state_t;

#endif //FLIGHTCONTROLLER_OUT_STATE_H
