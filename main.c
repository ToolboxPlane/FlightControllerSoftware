#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

#include <stdbool.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "Util/communication.h"
#include "Util/controller.h"
#include "Util/output.h"
#include "HAL/timer8bit.h"
#include "Util/input.h"

state_t curr_state;
setpoint_t curr_setpoint;
uint8_t usbTimout = 0, sbusTimeout = 0;

typedef enum {
    failsave, remote, flightcomputer
} setpoint_source_t;

setpoint_source_t setpoint_source;

void setpoint_update(setpoint_t setpoint) {
    if (setpoint_source == flightcomputer) {
        curr_setpoint.roll = 0;
        curr_setpoint.pitch = 0;
        curr_setpoint.power = 0;
    }
    output_led(3, toggle);
    usbTimout = 0;
}

void sbus_event(sbus_data_t sbus_data) {
    if (sbus_data.failsave) {
        curr_setpoint.roll = 0;
        curr_setpoint.pitch = 0;
        curr_setpoint.power = 0;
        setpoint_source = failsave;
        output_led(7, off);
    } else if (sbus_data.channel[11] > 600) { //@TODO check value
        setpoint_source = remote;
        output_led(7, on);
        output_led(6, off);
    } else {
        setpoint_source = flightcomputer;
        output_led(7, on);
        output_led(6, on);
    }
    output_led(5, toggle);
    sbusTimeout = 0;
}

void timer_tick() {
    out_state_t out_state;
    if (setpoint_source != remote) {
        controller_update(&curr_state, &curr_setpoint, &out_state);
        out_state.motor = curr_setpoint.power;
    } else {
        out_state.motor = sbus_latest_data.channel[0];
        out_state.aileron_l = sbus_latest_data.channel[1] - 500;
        out_state.aileron_r = sbus_latest_data.channel[2] - 500;
        out_state.vtail_l = sbus_latest_data.channel[3] - 500;
        out_state.vtail_r = sbus_latest_data.channel[4] - 500;
    }
    output_set(&out_state);
    communication_send_status(&curr_state, &out_state);
    output_led(4, toggle);

    // Timeout equals 500ms
    if (++usbTimout >= 125) {
        usbTimout = 125;
        setpoint_source = remote;
        output_led(7, on);
        output_led(6, off);
    }
    if (++sbusTimeout >= 125) {
        sbusTimeout = 125;
        setpoint_source = failsave;
        output_led(7, on);
        output_led(6, on);
    }
}

int main(void) {
    cli();
    output_init();
    output_led(0, on);
    if (!(MCUSR & 0b01000)) { // Watchdog
        output_led(1, on);
    } else {
        output_led(1, off);
    }
    if (!(MCUSR & 0b00100)) { // Brownout
        output_led(2, on);
    } else {
        output_led(2, off);
    }

    input_init();
    controller_init(4);
    communication_init(&setpoint_update, &sbus_event);
    // Runs at 244.14Hz (4.096ms), the BNO055 provides data at 100Hz, the output can be updated at 50Hz
    timer0_init(prescaler_256, &timer_tick);
    wdt_enable(WDTO_120MS);
    sei();

    while (true) {
        wdt_reset();
        input_get_state(&curr_state);
        output_led(0, toggle);
    }
}

#pragma clang diagnostic pop
