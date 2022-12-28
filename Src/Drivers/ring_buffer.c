/**
 * @file uart_buffer.c
 * @author Paul Nykiel
 * @date 27.12.22
 * @brief Description here TODO
 */
#include "ring_buffer.h"

ring_buffer_data_t ring_buffer_init(void) {
    ring_buffer_data_t uart_buffer_data = {.buf = {0}, .read_index = 0, .len = 0};
    return uart_buffer_data;
}

bool ring_buffer_put(ring_buffer_data_t *ring_buffer_data, uint8_t data) {
    if (ring_buffer_data->len == RING_BUFFER_SIZE) {
        return false;
    }
    uint16_t index = (ring_buffer_data->read_index + ring_buffer_data->len) % RING_BUFFER_SIZE;
    ring_buffer_data->buf[index] = data;
    ring_buffer_data->len += 1;
    return true;
}

bool ring_buffer_get(ring_buffer_data_t *ring_buffer_data, uint8_t *out) {
    if (ring_buffer_data->len > 0) {
        *out = ring_buffer_data->buf[ring_buffer_data->read_index];
        ring_buffer_data->read_index = (ring_buffer_data->read_index + 1) % RING_BUFFER_SIZE;
        ring_buffer_data->len -= 1;
        return true;
    }
    return false;
}
