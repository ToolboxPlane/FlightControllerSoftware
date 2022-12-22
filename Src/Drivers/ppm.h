/**
 * @file ppm.h
 * @author Paul Nykiel
 * @date 27.11.22
 * @brief Definition of the library functions for the PPM pins on the FC.
 */
#ifndef FLIGHTCONTROLLER_PPM_H
#define FLIGHTCONTROLLER_PPM_H

#include <stdint.h>

typedef enum {
    CHANNEL_1,
    CHANNEL_2,
    CHANNEL_3,
    CHANNEL_4,
    CHANNEL_5,
    CHANNEL_6,
    CHANNEL_7,
    CHANNEL_8,
} ppm_channel_id;

void ppm_init(void);

void ppm_channel_set(ppm_channel_id channel_id, uint16_t setpoint);

#endif // FLIGHTCONTROLLER_PPM_H
