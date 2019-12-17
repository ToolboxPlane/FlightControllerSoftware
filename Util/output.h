/**
 * @file output.h
 * @author paul
 * @date 18.04.19
 * @brief output @TODO
 */

#ifndef FLIGHTCONTROLLER_OUTPUT_H
#define FLIGHTCONTROLLER_OUTPUT_H

#include <stdbool.h>
#include "../out_state.h"

typedef enum {
    on, off, toggle, keep
} led_t;

void output_init(void);
void output_set(const out_state_t *out_state);
void output_led(uint8_t led, led_t state);

#endif //FLIGHTCONTROLLER_OUTPUT_H
