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
    pwm_init(4, prescaler_8, 40000);

    DDRB |= 0b11100000;
    DDRE |= 0b00111000;
    DDRH |= 0b00111000;
    DDRL = 0xFF; // All as outputs
}

void output_set(const out_state_t *out_state) {
    pwm_set_out_a(3, 2 * (1500 + out_state->elevon_r));
    pwm_set_out_b(3, 2 * (1000 + out_state->motor));
    pwm_set_out_c(3, 2 * (1500 + out_state->elevon_l));
    pwm_set_out_a(4, 2 * (1500));
    pwm_set_out_b(4, 2 * (1500));
    pwm_set_out_c(4, 2 * (1500));
    pwm_set_out_a(1, 2 * (1500));
    pwm_set_out_b(1, 2 * (1500));
    pwm_set_out_c(1, 2 * (1000));
}

void output_led(uint8_t led, led_t state) {
    switch (state) {
        case on:
            PORTL |= (1 << led);
            break;
        case off:
            PORTL &= ~(1 << led);
            break;
        case toggle:
            PORTL ^= (1 << led);
            break;
        default:
            break;
    }
}
