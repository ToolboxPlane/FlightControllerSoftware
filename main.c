#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

#include <stdbool.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "Util/communication.h"
#include "Util/controller.h"
#include "Util/output.h"
#include "HAL/timer8bit.h"
#include "Util/input.h"
#include "Drivers/bno055_uart.h"
#include "HAL/uart.h"

#define NORMALIZE_TARANIS(x) ((uint16_t)(((x)-172)*(1000.0F/(1811-172))))

typedef enum {
    failsave, remote, flightcomputer
} setpoint_source_t;

volatile state_t curr_state;
volatile setpoint_t flightcomputer_setpoint;
volatile out_state_t out_state = {
    .elevon_r = 500, .elevon_l = 500, .motor = 0
};
volatile uint8_t usbTimeout = 0, sbusTimeout = 0;
volatile sbus_data_t last_valid_sbus_package;

volatile setpoint_source_t setpoint_source = failsave;
const setpoint_t failsave_setpoint = {
    .roll = 0, .pitch = 0, .power = 0
};

void setpoint_update(setpoint_t setpoint) {
    flightcomputer_setpoint.roll = setpoint.roll;
    flightcomputer_setpoint.pitch = setpoint.pitch;
    flightcomputer_setpoint.power = setpoint.power;
    output_led(4, toggle);
    usbTimeout = 0;
}

void sbus_event(sbus_data_t sbus_data) {
    if (sbus_data.failsave || sbus_data.frame_lost) {
        setpoint_source = failsave;
    } else {
        last_valid_sbus_package = sbus_data;

        if (sbus_data.channel[7] < 500) {
            setpoint_source = remote;
        } else {
            setpoint_source = flightcomputer;
        }
    }
    output_led(5, toggle);
    sbusTimeout = 0;
}

void timer_tick(void) {
    // Timeout equals 500ms
    if (++sbusTimeout >= 31) {
        sbusTimeout = 31;
        setpoint_source = failsave;
    } else if (++usbTimeout >= 31) {
        usbTimeout = 31;
        if (setpoint_source == flightcomputer) {
            setpoint_source = remote;
        }
    }

    // Set outputs
    switch (setpoint_source) {
        case failsave:
            controller_update(&curr_state, &failsave_setpoint, &out_state);
            out_state.motor = failsave_setpoint.power;
            output_led(6, off);
            output_led(7, off);
            break;
        case remote:
            if (last_valid_sbus_package.channel[0] != 0) {
                out_state.motor = NORMALIZE_TARANIS(last_valid_sbus_package.channel[0]);
            } else {
                out_state.motor = 0;
            }
            out_state.elevon_l = NORMALIZE_TARANIS(last_valid_sbus_package.channel[1]) - 500;
            out_state.elevon_r = NORMALIZE_TARANIS(last_valid_sbus_package.channel[2]) - 500;
            output_led(6, off);
            output_led(7, on);
            break;
        case flightcomputer:
            controller_update(&curr_state, &flightcomputer_setpoint, &out_state);
            out_state.motor = flightcomputer_setpoint.power;
            output_led(6, on);
            output_led(7, on);
            break;
        default:
            break;
    }

    output_led(3, toggle);
    output_set(&out_state);
}

int main(void) {
    cli();
    output_init();
    output_led(0, on);
    output_led(1, MCUSR & (1u << WDRF) ? off : on); // Watchdog
    output_led(2, MCUSR & (1u << BORF) ? off : on); // Brownout
    MCUSR = 0;

    communication_init(&setpoint_update, &sbus_event);
    controller_init(16);
    // Runs at 16.384ms interval, the BNO055 provides data at 100Hz, the output can be updated at 50Hz
    timer_8bit_init(prescaler_1024, &timer_tick);
    sei();

    wdt_enable(WDTO_250MS);

    input_init();

    uint8_t mux = 0;

    while (true) {
        wdt_reset();
        curr_state = input_get_state();
        if (++mux >= 10) {
            communication_send_status(&curr_state, &out_state);
            mux = 0;
        }
        communication_handle();
        output_led(0, toggle);
        _delay_ms(10);
    }
}

#pragma clang diagnostic pop
