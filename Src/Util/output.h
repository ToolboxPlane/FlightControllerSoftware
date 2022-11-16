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

typedef enum { on, off, toggle, keep } led_t;

void output_init(void);
void output_set(volatile const out_state_t *out_state);
void output_led(uint8_t led, led_t state);
void output_debug(uint8_t id, led_t state);

#ifdef NDEBUG
    #define TEST_ON(id)
    #define TEST_OFF(id)
#else
    #define TEST_ON(id) output_debug(id, on);
    #define TEST_OFF(id) output_debug(id, off);
#endif


#endif // FLIGHTCONTROLLER_OUTPUT_H
