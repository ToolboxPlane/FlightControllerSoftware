/**
 * @file ppm.c
 * @author paul
 * @date 27.11.22
 * Description here TODO
 */
#include "ppm.h"

#include <HAL/pwm16bit.h>
#include <avr/io.h>

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
    pwm_init(1, prescaler_8, 40000);
    pwm_init(3, prescaler_8, 40000);
    pwm_init(4, prescaler_8, 40000);
}

void ppm_channel_set(uint8_t channel_id, uint16_t setpoint) {
    uint16_t compare_value = (setpoint + 1000U) * 2U;
    switch (channel_id) {
        case 0:
            pwm_set_out_a(3, compare_value);
            break;
        case 1:
            pwm_set_out_b(3, compare_value);
            break;
        case 2:
            pwm_set_out_c(3, compare_value);
            break;
        case 3:
            pwm_set_out_a(4, compare_value);
            break;
        case 4:
            pwm_set_out_b(4, compare_value);
            break;
        case 5:
            pwm_set_out_c(4, compare_value);
            break;
        case 6:
            pwm_set_out_a(1, compare_value);
            break;
        case 7:
            pwm_set_out_b(1, compare_value);
            break;
        case 8:
            pwm_set_out_c(1, compare_value);
            break;
        default:
            break;
            // TODO error handler
    }
}
