/**
 * @file communication.c
 * @author paul
 * @date 16.04.19
 * @brief communication @TODO
 */

#include "communication.h"

#include <pb_encode.h>

#include "../Drivers/sbus.h"
#include "../HAL/uart.h"
#include "FlightController.pb.h"
#include "Taranis.pb.h"
#include "output.h"

#define FC_ID 0x10
#define SBUS_ID 0x20

#define BUFF_SIZE 512

/*
 * (x-172)/(1811-172)*1000
 *
 * 1000/(1811-172) = 1000/1639 \approx 1000/1640 = 25/41
 */
#define NORMALIZE_TARANIS(x) ((uint16_t) (((x) -172) * 25 / 41))

static void (*_setpoint_callback)(setpoint_t) = 0;
static void (*_sbus_callback)(sbus_data_t) = 0;
static volatile bool failsave = false;
volatile sbus_data_t sbus_data;

// Nomenclature: Data is written into the buf at head and read at tail
static uint8_t usb_buff[BUFF_SIZE];
static uint16_t usb_head, usb_tail;

static uint8_t sbus_buff[BUFF_SIZE];
static uint16_t sbus_head, sbus_tail;


void usb_callback(uint8_t data) {
    usb_buff[usb_head] = data;
    usb_head = (usb_head + 1) % BUFF_SIZE;
}

void sbus_receive(uint8_t data) {
    sbus_buff[sbus_head] = data;
    sbus_head = (sbus_head + 1) % BUFF_SIZE;
}

void communication_init(void (*setpoint_callback)(setpoint_t), void (*sbus_callback)(sbus_data_t)) {
    _setpoint_callback = setpoint_callback;
    _sbus_callback = sbus_callback;

    uart_init(0, 115200, NONE, 1, &usb_callback);
    uart_init(2, 100000, EVEN, 2, &sbus_receive);
}

void communication_send_status(volatile const imu_data_t *state, volatile const out_state_t *out_state) {
    static uint8_t buf[ToolboxPlaneMessages_FlightController_size + 2];

    ToolboxPlaneMessages_FlightController messagesFlightController;
    messagesFlightController.roll_mul_16 = state->roll_mul_16;
    messagesFlightController.pitch_mul_16 = state->pitch_mul_16;
    messagesFlightController.yaw_mul_16 = state->heading_mul_16;
    messagesFlightController.dRoll_mul_16 = state->d_roll_mul_16;
    messagesFlightController.dPitch_mul_16 = state->d_pitch_mul_16;
    messagesFlightController.dYaw_mul_16 = state->d_heading_mul_16;
    messagesFlightController.accX_mul_100 = state->acc_x_mul_100;
    messagesFlightController.accY_mul_100 = state->acc_y_mul_100;
    messagesFlightController.accZ_mul_100 = state->acc_z_mul_100;
    messagesFlightController.imu_ok = state->imu_ok;
    messagesFlightController.motor = out_state->motor;
    messagesFlightController.servoLeft = out_state->elevon_l + 500;
    messagesFlightController.servoRight = out_state->elevon_r + 500;

    pb_ostream_t ostream = pb_ostream_from_buffer(buf + 2, sizeof(buf) - 3);
    pb_encode(&ostream, ToolboxPlaneMessages_FlightController_fields, &messagesFlightController);
    buf[0] = 0x0F;
    buf[1] = FC_ID;
    buf[ostream.bytes_written + 2] = 0xFF;
    uart_send_buf(0, buf, ostream.bytes_written + 3);
}

bool communication_is_failsave(void) {
    return failsave;
}

void communication_handle(void) {
    static uint8_t sbus_send_mux = 0;
    static uint8_t buf[ToolboxPlaneMessages_Taranis_size + 3];

    while (usb_tail != usb_head) {
        //uint8_t data = usb_buff[usb_tail];
        usb_tail = (usb_tail + 1) % BUFF_SIZE;
        /*if (rc_lib_decode(&usb_rx_pkg, data)) {
            if (_setpoint_callback != 0) {
                setpoint_t setpoint = {.power = usb_rx_pkg.channel_data[0],
                                       .pitch = usb_rx_pkg.channel_data[1] - 180,
                                       .roll = usb_rx_pkg.channel_data[2] - 180};

                (*_setpoint_callback)(setpoint);
            }
        }*/
    }

    while (sbus_tail != sbus_head) {
        uint8_t data = sbus_buff[sbus_tail];
        sbus_tail = (sbus_tail + 1) % BUFF_SIZE;

        if (sbus_parse(&data, 1)) {
            for (uint8_t c = 0; c < 16; ++c) {
                sbus_data.channel[c] = NORMALIZE_TARANIS(sbus_latest_data.channel[c]);
            }
            sbus_data.failsave = sbus_latest_data.failsave;
            sbus_data.frame_lost = sbus_latest_data.frame_lost;

            if (++sbus_send_mux >= 64) {
                sbus_send_mux = 0;

                ToolboxPlaneMessages_Taranis taranis;
                taranis.throttleMixed = sbus_data.channel[0];
                taranis.elevonLeftMixed = sbus_data.channel[1];
                taranis.elevonRightMixed = sbus_data.channel[2];
                taranis.throttleRaw = sbus_data.channel[3];
                taranis.pitchRaw = sbus_data.channel[4];
                taranis.rollRaw = sbus_data.channel[5];
                taranis.isArmed = sbus_data.channel[6] < 400;
                taranis.overrideActive = sbus_data.channel[7] > 600;
                taranis.failsave = sbus_data.failsave;
                taranis.frameLost = sbus_data.frame_lost;

                pb_ostream_t ostream = pb_ostream_from_buffer(buf + 2, sizeof(buf) - 3);
                pb_encode(&ostream, ToolboxPlaneMessages_Taranis_fields, &taranis);
                buf[0] = 0x0F;
                buf[1] = SBUS_ID;
                buf[ostream.bytes_written + 2] = 0xFF;
                uart_send_buf(0, buf, ostream.bytes_written + 3);
            }
            (*_sbus_callback)(sbus_data);
        }
    }
}
