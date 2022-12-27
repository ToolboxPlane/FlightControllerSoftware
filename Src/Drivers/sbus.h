/**
 * @file sbus.h
 * @author Paul Nykiel
 * @date 14.04.18
 * @brief Definition of the library functions for sbus reception and decoding.
 * @ingroup Drivers
 */

#ifndef FLIGHTCONTROLLER_SBUS_H
#define FLIGHTCONTROLLER_SBUS_H

#include <stdbool.h>
#include <stdint.h>

/**
 * Number of channels transmitted via a single SBUS packet.
 */
enum { SBUS_NUM_CHANNELS = 16 };

/**
 * Data for a single packet received from the SBUS receiver.
 */
typedef struct {
    uint16_t channel[SBUS_NUM_CHANNELS]; ///< The channel data, each channel is in [0, 2047] (11 bit)
    bool failsafe; ///< Fail-safe bit of the package, set by the receiver
    bool frame_lost; ///< Frame lost bit of the package set by the receiver
} sbus_data_t;

/**
 * Initializes the SBUS module by initializing all internal data and the UART connection.
 */
void sbus_init(void);

/**
 * Decodes all data received since the last call and returns whether the data contains a full new packet.
 * @return true if a new packet was received, otherwise false.
 */
bool sbus_data_available(void);

/**
 * Get the last packet that was received.
 * @return a copy of the last packet.
 */
sbus_data_t sbus_get_latest_data(void);


#endif // FLIGHTCONTROLLER_SBUS_H
