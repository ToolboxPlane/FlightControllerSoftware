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
    int16_t aileron_r, aileron_l; // in [-500, 500]
    int16_t vtail_r, vtail_l; // in [-500, 500]
    uint16_t motor; // in [0, 1000]
} out_state_t;

#endif //FLIGHTCONTROLLER_OUT_STATE_H
