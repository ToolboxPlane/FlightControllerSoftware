/**
 * @file input.c
 * @author paul
 * @date 18.04.19
 * @brief input @TODO
 */

#include "input.h"
#include "../Drivers/bno055_uart.h"

void input_init(void) {
    bno055_init();
}

void input_get_state(volatile state_t *state) {
    state->roll = -bno055_eul_y();
    state->pitch = bno055_eul_z();
    state->heading = bno055_eul_x();
    state->roll_deriv = -bno055_gyr_y();
    state->pitch_deriv = bno055_gyr_z();
    state->heading_deriv = bno055_gyr_x();
    state->bno_state = bno055_status();
}
