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

void output_init(void);
void output_set(const out_state_t *out_state);
void output_led(uint8_t led, bool state);

#endif //FLIGHTCONTROLLER_OUTPUT_H
