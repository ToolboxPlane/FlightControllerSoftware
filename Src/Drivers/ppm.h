/**
 * @file ppm.h
 * @author Paul Nykiel
 * @date 27.11.22
 * @brief Definition of the library functions for the PPM pins on the FC.
 * @ingroup Drivers
 */
#ifndef FLIGHTCONTROLLER_PPM_H
#define FLIGHTCONTROLLER_PPM_H

#include <stdint.h>

/**
 * Hardware PPM channels on the flightcontroller board.
 */
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

/**
 * Initialize PPM for all channels. After initialization all channels are off and will be activated with the
 * first call to ppm_channel_set.
 */
void ppm_init(void);

/**
 * Set the value of a PPM channel
 * @param channel_id the ID of the channel, as numbered on the PCB.
 * @param setpoint the setpoint (in microseconds) for the actuator, needs to be in [0, 1000]
 */
void ppm_channel_set(ppm_channel_id channel_id, uint16_t setpoint);

#endif // FLIGHTCONTROLLER_PPM_H
