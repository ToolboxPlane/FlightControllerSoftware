/**
 * @file sbus.h
 * @author Paul Nykiel
 * @date 14.04.18
 * @brief Definition of the library functions for sbus reception and decoding.
 */

#ifndef FLIGHTCONTROLLER_SBUS_H
#define FLIGHTCONTROLLER_SBUS_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint16_t channel[16];
    bool failsave, frame_lost;
} sbus_data_t;

void sbus_init(void);

sbus_data_t sbus_get_latest_data(void);

bool sbus_data_available(void);

#endif // FLIGHTCONTROLLER_SBUS_H
