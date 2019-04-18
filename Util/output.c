/**
 * @file output.c
 * @author paul
 * @date 18.04.19
 * @brief output @TODO
 */

#include <avr/io.h>
#include "output.h"
#include "../HAL/pwm16bit.h"

void output_init(void) {
    /*
     * This combination of prescaler and max value yields a frequency of 50Hz.
     * Each LSB corresponds to 0.5us
     */
    pwm_init(1, prescaler_8, 40000);
    pwm_init(3, prescaler_8, 40000);

    DDRL = 0xFF; // All as outputs
}

void output_set(const out_state_t *out_state) {
    pwm_set_out_a(0, 2 * (1000 + out_state->aileron_r));
    pwm_set_out_b(0, 2 * (1000 + out_state->vtail_r));
    pwm_set_out_c(0, 2 * (1000 + out_state->motor));
    pwm_set_out_a(1, 2 * (1000 + out_state->vtail_l));
    pwm_set_out_b(1, 2 * (1000 + out_state->aileron_l));
}

void output_led(uint8_t led, bool state) {
    if (state) {
        PORTL |= (1 << led);
    } else {
        PORTL &= ~(1 << led);
    }
}
