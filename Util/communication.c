/**
 * @file communication.c
 * @author paul
 * @date 16.04.19
 * @brief communication @TODO
 */

#include "communication.h"
#include "../HAL/uart.h"
#include "../Drivers/sbus.h"
#include "../Drivers/rc_lib/rc_lib.h"

void usb_callback(uint8_t data) {

}

void communication_init(void) {
    rc_lib_transmitter_id = 23;
    rc_lib_global_package_uid = 0;
    rc_lib_error_count = 0;

    uart0_init(9600, &usb_callback);
}

void communication_send_status(const state_t *state, const control_t *control) {
    rc_lib_package_t pkg;
    pkg.channel_count = 16;
    pkg.resolution = 10;
    pkg.mesh = false;

    pkg.channel_data[0] = state->heading;
    pkg.channel_data[1] = state->roll + 180;
    pkg.channel_data[2] = state->pitch + 180;
    pkg.channel_data[3] = state->ultrasonic_height;
    pkg.channel_data[4] = state->baro_height;
    pkg.channel_data[5] = state->airspeed;
    pkg.channel_data[6] = state->acc_forward + 500;
    pkg.channel_data[7] = state->acc_side + 500;
    pkg.channel_data[8] = state->acc_updown + 500;
    pkg.channel_data[9] = 0;
    pkg.channel_data[10] = 0;
    pkg.channel_data[11] = control->aileron_r + 500;
    pkg.channel_data[12] = control->vtail_r + 500;
    pkg.channel_data[13] = control->motor + 500;
    pkg.channel_data[14] = control->vtail_l + 500;
    pkg.channel_data[15] = control->aileron_l + 500;

    uint8_t len = rc_lib_encode(&pkg);
    uart0_send_buf(pkg.buffer, len);
}
