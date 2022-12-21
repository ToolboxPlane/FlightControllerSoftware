/**
 * @file ppm.c
 * @author Paul Nykiel
 * @date 27.11.22
 * @brief Implementation of the library functions for the PPM pins on the FC.
 */
#include "ppm.h"

#include <HAL/pwm16bit.h>
#include <avr/io.h>

enum { PPM_MAX_VAL = 40000U };
enum { PPM_ZERO_VAL = 1000U };

void ppm_init(void) {
    // All PWM pins as outputs
    DDRB |= 0b11100000u;
    DDRE |= 0b00111000u;
    DDRH |= 0b00111000u;

    /*
     * CLK: 16Mhz. The prescaler 8 yields a timer frequency of 2Mhz (0.5us per tick).
     * Thus, a max value of 40000 yields a period of 20ms and each LSB corresponds
     * to 0.5us
     */
    pwm_init(1, prescaler_8, PPM_MAX_VAL);
    pwm_init(3, prescaler_8, PPM_MAX_VAL);
    pwm_init(4, prescaler_8, PPM_MAX_VAL);
}

void ppm_channel_set(ppm_channel_id channel_id, uint16_t setpoint) {
    uint16_t compare_value = (setpoint + PPM_ZERO_VAL) * 2U;
    switch (channel_id) {
        case CHANNEL_1:
            pwm_set_out_a(3, compare_value);
            break;
        case CHANNEL_2:
            pwm_set_out_b(3, compare_value);
            break;
        case CHANNEL_3:
            pwm_set_out_c(3, compare_value);
            break;
        case CHANNEL_4:
            pwm_set_out_a(4, compare_value);
            break;
        case CHANNEL_5:
            pwm_set_out_b(4, compare_value);
            break;
        case CHANNEL_6:
            pwm_set_out_a(1, compare_value);
            break;
        case CHANNEL_7:
            pwm_set_out_b(1, compare_value);
            break;
        case CHANNEL_8:
            pwm_set_out_c(1, compare_value);
            break;
    }
}
