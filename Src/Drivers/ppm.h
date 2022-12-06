/**
 * @file ppm.h
 * @author Paul Nykiel
 * @date 27.11.22
 * @brief Definition of the library functions for the PPM pins on the FC.
 */
#ifndef FLIGHTCONTROLLER_PPM_H
#define FLIGHTCONTROLLER_PPM_H

#include <stdint.h>

void ppm_init(void);

void ppm_channel_set(uint8_t channel_id, uint16_t setpoint);

#endif // FLIGHTCONTROLLER_PPM_H
