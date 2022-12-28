/**
 * @file uart_buffer.h
 * @author Paul Nykiel
 * @date 27.12.22
 * @brief Description here TODO
 */
#ifndef FLIGHTCONTROLLER_RING_BUFFER_H
#define FLIGHTCONTROLLER_RING_BUFFER_H

#include <stdbool.h>
#include <stdint.h>

enum { RING_BUFFER_SIZE = 512 };

typedef struct {
    uint8_t buf[RING_BUFFER_SIZE]; ///< Buffer for the data
    uint16_t read_index;           ///< Index of the next data to read (i.e. oldest data)
    uint16_t len;                  ///< Number of elements
} ring_buffer_data_t;

ring_buffer_data_t ring_buffer_init(void);

bool ring_buffer_put(ring_buffer_data_t *ring_buffer_data, uint8_t data);

bool ring_buffer_get(ring_buffer_data_t *ring_buffer_data, uint8_t *out);

#endif // FLIGHTCONTROLLER_RING_BUFFER_H
