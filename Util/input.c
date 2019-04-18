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

void input_get_state(state_t *state) {
    state->heading = bno055_eul_z();
    state->pitch = bno055_eul_y();
    state->roll = bno055_eul_x();
    state->acc_forward = bno055_linear_acc_x();
    state->acc_side = bno055_linear_acc_y();
    state->acc_updown = bno055_linear_acc_z();
}
