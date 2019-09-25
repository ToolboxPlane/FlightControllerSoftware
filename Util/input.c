/**
 * @file input.c
 * @author paul
 * @date 18.04.19
 * @brief input @TODO
 */

#include "input.h"
#include "../Drivers/bno055_uart.h"
#include <avr/delay.h>

void input_init(void) {
    bno055_init();
}

void input_get_state(volatile state_t *state) {
    state->heading = bno055_eul_z(); // @TODO check mapping
    _delay_ms(1);
    state->pitch = bno055_eul_x();
    _delay_ms(1);
    state->roll = -bno055_eul_y();
    _delay_ms(1);
    state->heading_deriv = bno055_gyr_z();
    _delay_ms(1);
    state->pitch_deriv = bno055_gyr_x();
    _delay_ms(1);
    state->roll_deriv = -bno055_gyr_y();
    _delay_ms(1);
    state->acc_forward = -bno055_linear_acc_y();
    _delay_ms(1);
    state->acc_side = bno055_linear_acc_x();
    _delay_ms(1);
    state->acc_updown = bno055_linear_acc_z();
    _delay_ms(1);
}
