/**
 * @file output.c
 * @author paul
 * @date 18.04.19
 * @brief output @TODO
 */

#include "output.h"

#include <avr/io.h>

#include "../HAL/pwm16bit.h"

void output_init(void) {
    /*
     * This combination of prescaler and max value yields a frequency of 50Hz.
     * Each LSB corresponds to 0.5us
     */
#ifdef NDEBUG
    pwm_init(1, prescaler_8, 40000);
    pwm_init(3, prescaler_8, 40000);
    pwm_init(4, prescaler_8, 40000);
#endif

    DDRL = 0xFFu; // All as outputs
}

void output_set(volatile const out_state_t *out_state) {
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
            PORTL |= (1u << led);
            break;
        case off:
            PORTL &= ~(1u << led);
            break;
        case toggle:
            PORTL ^= (1u << led);
            break;
        default:
            break;
    }
}
void output_debug(uint8_t id, led_t state) {
    volatile uint8_t *port = 0;
    uint8_t bit = 0;
    switch (id) {
        case 0:
            port = &PORTE;
            bit = 3;
            break;
        case 1:
            port = &PORTE;
            bit = 4;
            break;
        case 2:
            port = &PORTE;
            bit = 5;
            break;
        case 3:
            port = &PORTH;
            bit = 3;
            break;
        case 4:
            port = &PORTH;
            bit = 4;
            break;
        case 5:
            port = &PORTB;
            bit = 5;
            break;
        case 6:
            port = &PORTB;
            bit = 6;
            break;
        case 7:
            port = &PORTB;
            bit = 7;
            break;
        default:
            // We shouldn't be here...
            break;
    }

    if (port != 0) {
        switch (state) {
            case on:
                *port |= (1u << bit);
                break;
            case off:
                *port &= ~(1u << bit);
                break;
            case toggle:
                *port ^= (1u << bit);
                break;
            default:
                break;
        }
    }
}
