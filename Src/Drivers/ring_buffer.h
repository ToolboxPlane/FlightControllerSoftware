/**
 * @file uart_buffer.h
 * @author Paul Nykiel
 * @date 27.12.22
 * @brief Utility fixed-size ringbuffer for single bytes.
 * @ingroup Drivers
 */
#ifndef FLIGHTCONTROLLER_RING_BUFFER_H
#define FLIGHTCONTROLLER_RING_BUFFER_H

#include <stdbool.h>
#include <stdint.h>

/**
 * Size of the ring-buffer, i.e. number of elements/bytes.
 */
enum { RING_BUFFER_SIZE = 512 };

/**
 * Internal data for the ring buffer
 */
typedef struct {
    uint8_t buf[RING_BUFFER_SIZE]; ///< Buffer for the data
    uint16_t read_index;           ///< Index of the next data to read (i.e. oldest data)
    uint16_t len;                  ///< Number of elements
} ring_buffer_data_t;

/**
 * Create a correctly initialized ring_buffer_data_t object.
 * @return an ring_buffer_data_t for usage with the other functions.
 */
ring_buffer_data_t ring_buffer_init(void);

/**
 * Write a byte to the ring buffer.
 * @param ring_buffer_data the internal data of the ring-buffer
 * @param data the byte to add
 * @return true if the object could be added, false if the buffer is already full
 */
bool ring_buffer_put(ring_buffer_data_t *ring_buffer_data, uint8_t data);

/**
 * Get a byte from the ring buffer if available.
 * @param ring_buffer_data the internal data of the ring-buffer
 * @param out the byte that got read
 * @return true if a byte was available, otherwise false
 */
bool ring_buffer_get(ring_buffer_data_t *ring_buffer_data, uint8_t *out);

#endif // FLIGHTCONTROLLER_RING_BUFFER_H
