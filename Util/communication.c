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
#include "output.h"

#define FC_TRANSMITTER_ID 23
#define SBUS_TRANSMITTER_ID 56

#define USB_BUFF_SIZE 512

static void (*_setpoint_callback)(setpoint_t) = 0;
static void (*_sbus_callback)(sbus_data_t) = 0;
static volatile bool failsave = false;
static volatile rc_lib_package_t usb_rx_pkg;

// Nomenclature: Data is written into the buf at head and read at tail
static uint8_t usb_buff[USB_BUFF_SIZE];
uint16_t head, tail;


void usb_callback(uint8_t data) {
    output_led(1, toggle);
    usb_buff[head] = data;
    head = (head + 1) % USB_BUFF_SIZE;
}

void sbus_receive(uint8_t data) {
    static uint8_t mux = 0;
    if (sbus_parse(&data, 1)) {
        if (!sbus_latest_data.failsave && ++mux >= 64) {
            mux = 0;
            rc_lib_package_t pkg;
            rc_lib_init_tx(&pkg, 2048, 16);

            for (uint8_t c = 0; c < 16; c++) {
                pkg.channel_data[c] = sbus_latest_data.channel[c];
            }

            rc_lib_transmitter_id = SBUS_TRANSMITTER_ID;
            uint8_t len = rc_lib_encode(&pkg);
            uart_send_buf(0, pkg.buffer, len);
        }
        (*_sbus_callback)(sbus_latest_data);
    }
}

void communication_init(void (*setpoint_callback)(setpoint_t), void (*sbus_callback)(sbus_data_t)) {
    _setpoint_callback = setpoint_callback;
    _sbus_callback = sbus_callback;
    rc_lib_init_rx(&usb_rx_pkg);
    rc_lib_global_package_uid = 0;
    rc_lib_error_count = 0;

    uart_init(0, 115200, NONE, 1, &usb_callback);
    uart_init(2, 100000, EVEN, 2, &sbus_receive);
}

void communication_send_status(volatile const state_t *state, volatile const out_state_t *out_state) {
    rc_lib_package_t pkg;
    rc_lib_init_tx(&pkg, 1024, 16);

    pkg.channel_data[0] = state->bno_state;
    pkg.channel_data[1] = state->roll + 500;
    pkg.channel_data[2] = state->pitch + 500;
    pkg.channel_data[3] = state->heading + 500;
    pkg.channel_data[4] = state->roll_deriv + 500;
    pkg.channel_data[5] = state->pitch_deriv + 500;
    pkg.channel_data[6] = state->heading_deriv + 500;
    pkg.channel_data[7] = state->acc_x + 500;
    pkg.channel_data[8] = state->acc_y + 500;
    pkg.channel_data[9] = state->acc_z + 500;
    pkg.channel_data[10] = 0;
    pkg.channel_data[11] = 0;
    pkg.channel_data[12] = 0;
    pkg.channel_data[13] = out_state->motor;
    pkg.channel_data[14] = out_state->elevon_l + 500;
    pkg.channel_data[15] = out_state->elevon_r + 500;

    rc_lib_transmitter_id = FC_TRANSMITTER_ID;
    uint8_t len = rc_lib_encode(&pkg);
    uart_send_buf(0, pkg.buffer, len);
}

bool communication_is_failsave(void) {
    return failsave;
}

void communication_handle_usb(void) {
    while (tail != head) {
        uint8_t data = usb_buff[tail];
        tail = (tail + 1) % USB_BUFF_SIZE;
        if (rc_lib_decode(&usb_rx_pkg, data)) {
            if (_setpoint_callback != 0) {
                setpoint_t setpoint = {
                        .power = usb_rx_pkg.channel_data[0],
                        .pitch = usb_rx_pkg.channel_data[1] - 180,
                        .roll = usb_rx_pkg.channel_data[2] - 180
                };

                (*_setpoint_callback)(setpoint);
            }
        }
    }
}

