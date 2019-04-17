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

#define FC_TRANSMITTER_ID 23
#define SBUS_TRANSMITTER_ID 56

static void (*_setpoint_callback)(setpoint_t) = 0;
static void (*_failsave_callback)(void) = 0;

void usb_callback(uint8_t data) {
    static rc_lib_package_t pkg;
    if (rc_lib_decode(&pkg, data)) {
        if (_setpoint_callback != 0) {
            setpoint_t setpoint = {
                .power = pkg.channel_data[0],
                .pitch = pkg.channel_data[1] - 180,
                .roll = pkg.channel_data[2] - 180
            };

            (*_setpoint_callback)(setpoint);
        }
    }
}

void sbus_callback(uint8_t data) {
    if (sbus_parse(&data, 1)) {
        if (sbus_latest_data.failsave) {
            (*_failsave_callback)();
        } else {
            rc_lib_package_t pkg;
            pkg.channel_count = 16;
            pkg.resolution = 10;
            pkg.mesh = false;
            for (uint8_t c=0; c<16; c++) {
                pkg.channel_data[c] = sbus_latest_data.channel[c];
            }

            rc_lib_transmitter_id = SBUS_TRANSMITTER_ID;
            uint8_t len = rc_lib_encode(&pkg);
            uart0_send_buf(pkg.buffer, len);
        }
    }
}

void communication_init(void (*setpoint_callback)(setpoint_t), void (*failsave_callback)(void)) {
    _setpoint_callback = setpoint_callback;
    _failsave_callback = failsave_callback;
    rc_lib_global_package_uid = 0;
    rc_lib_error_count = 0;

    uart0_init(9600, &usb_callback);
    //uart1_init(98000, &sbus_callback);
}

void communication_send_status(const state_t *state, const out_state_t *out_state) {
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
    pkg.channel_data[11] = out_state->aileron_r + 500;
    pkg.channel_data[12] = out_state->vtail_r + 500;
    pkg.channel_data[13] = out_state->motor + 500;
    pkg.channel_data[14] = out_state->vtail_l + 500;
    pkg.channel_data[15] = out_state->aileron_l + 500;

    rc_lib_transmitter_id = FC_TRANSMITTER_ID;
    uint8_t len = rc_lib_encode(&pkg);
    uart0_send_buf(pkg.buffer, len);
}

