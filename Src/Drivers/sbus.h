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
    bool failsafe;                       ///< Fail-safe bit of the package, set by the receiver
    bool frame_lost;                     ///< Frame lost bit of the package set by the receiver
} sbus_data_t;

/**
 * @brief Initialize the SBUS module.
 *
 * The initialization consists of the following tasks:
 *  * Initialize the internal ring buffer
 *  * Initialize the uart to the following parameters:
 *      * ID: 2
 *      * Baud Rate: 100 000
 *      * Parity: Even
 *      * Stop Bits: 2
 *      * Callback: pointer to the internal uart callback function
 *  * Initialize all internal state for decoding
 *
 * The internal uart callback function performs the following tasks:
 *  * Add the received byte to the internal ring-buffer
 *
 * @see https://github.com/bolderflight/sbus/blob/main/README.md
 */
void sbus_init(void);

/**
 * @brief Decodes all data received since the last call and returns whether the data contains a full new packet.
 *
 * This function iterates over all in the ring buffer and feeds the bytes, in the order they were received,
 * into the following state machine (initialized in the INIT state, with the state kept between calls to the
 * function):
 *
 * \dot
 * digraph {
 *      rankdir = "LR";
 *
 *      INIT -> INIT [
 *          label = "data!=0x0F/\nindex=0";
 *      ]
 *      INIT -> IN_DATA [
 *          label = "data=0x0F/";
 *      ]
 *
 *      IN_DATA -> IN_DATA [
 *          label = "index<22/\nbuffer[index]=data\nindex += 1";
 *      ]
 *
 *      IN_DATA -> AT_END [
 *          label = "index>=22/\nfailsave = data_3,\nframe-lost= data_2";
 *      ]
 *
 *      AT_END -> INIT [
 *          label = "data=0x00/\nDecode result";
 *      ]
 *
 *      AT_END -> INIT [
 *          label = "data=0x00/\nTrigger warning";
 *      ]
 *
 * }
 * \enddot
 *
 * **Decode result** consists of the following tasks:
 *  * For every bit in the (receive-) buffer calculate the overall index as the index in the byte plus the 8 times the
 *    index of the byte
 *  * Calculate the channel index as the result of the integer division of the (receive-) buffer index with 11 and
 *  * Set the channel bit index as the remainder of the integer division
 *  * Set the bit of the respective channel to the bit value
 *
 * After all data has been consumed the function shall return true if the **Decode result** was triggered, otherwise
 * it shall return false.
 *
 * @see https://github.com/bolderflight/sbus/blob/main/README.md
 * @return true if a new packet was received, otherwise false.
 */
bool sbus_data_available(void);

/**
 * @brief Get the last packet that was received.
 *
 * Returns the last fully received package, if no package was received both the failsafe and the frame-lost flag
 * shall be true.
 *
 * @return a copy of the last packet.
 */
sbus_data_t sbus_get_latest_data(void);


#endif // FLIGHTCONTROLLER_SBUS_H
