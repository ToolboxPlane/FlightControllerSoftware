#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

#include <stdbool.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/delay.h>

#include "Util/communication.h"
#include "Util/controller.h"
#include "Util/output.h"
#include "HAL/timer8bit.h"
#include "Util/input.h"
#include "Drivers/bno055_uart.h"
#include "HAL/uart.h"

volatile state_t curr_state;
volatile setpoint_t curr_setpoint;
volatile out_state_t out_state;
volatile uint8_t usbTimout = 0, sbusTimeout = 0;

typedef enum {
    failsave, remote, flightcomputer
} setpoint_source_t;

volatile setpoint_source_t setpoint_source;

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
        setpoint_source = failsave;
    } else if (sbus_data.channel[11] > 600) { //@TODO check value
        setpoint_source = remote;
    } else {
        setpoint_source = flightcomputer;
    }
    output_led(5, toggle);
    sbusTimeout = 0;
}

void timer_tick() {
    // Timeout equals 500ms
    if (++sbusTimeout >= 31) {
        sbusTimeout = 31;
        setpoint_source = failsave;
    } else if (++usbTimout >= 31) {
        usbTimout = 31;
        setpoint_source = remote;
    }

    if (setpoint_source == flightcomputer) {
        controller_update(&curr_state, &curr_setpoint, &out_state);
        out_state.motor = curr_setpoint.power;
    } else if (setpoint_source == failsave) {
        out_state.motor = 0;
        out_state.aileron_l = 0;
        out_state.aileron_r = 0;
        out_state.vtail_l = 0;
        out_state.vtail_r = 0;
    } else {
        out_state.motor = sbus_latest_data.channel[0];
        out_state.aileron_l = sbus_latest_data.channel[1] - 500;
        out_state.aileron_r = sbus_latest_data.channel[2] - 500;
        out_state.vtail_l = sbus_latest_data.channel[3] - 500;
        out_state.vtail_r = sbus_latest_data.channel[4] - 500;
    }
    output_set(&out_state);
    output_led(4, toggle);

    switch (setpoint_source) {
        case failsave:
            output_led(6, off);
            output_led(7, off);
            break;
        case remote:
            output_led(6, off);
            output_led(7, on);
            break;
        case flightcomputer:
            output_led(6, on);
            output_led(7, on);
            break;
        default:
            break;
    }
}

uint8_t bno055_read_byte(uint8_t reg);

uint8_t bno055_write_byte(uint8_t reg, uint8_t byte);

int main(void) {
    cli();
    output_init();
    output_led(0, on);
    output_led(1, MCUSR & (1 << WDRF) ? off : on); // Watchdog
    output_led(2, MCUSR & (1 << BORF) ? off : on); // Brownout
    MCUSR = 0;

    communication_init(&setpoint_update, &sbus_event);
    controller_init(16);
    // Runs at 16.384ms interval, the BNO055 provides data at 100Hz, the output can be updated at 50Hz
    //timer0_init(prescaler_1024, &timer_tick);
    sei();

    wdt_enable(WDTO_8S);

    input_init();
    _delay_ms(100);

    while (true) {
        wdt_reset();
        bno055_status_t status = bno055_read_byte(0x39);
        bno055_error_t error = bno055_read_byte(0x3A);
        uint8_t buf[] = {'S', status, 'E', error};
        uart_send_buf(0, buf, sizeof(buf));

        if (status == system_error) {
            output_led(7, on);
        } else {
            output_led(7, off);
        }

        output_led(0, toggle);
        _delay_ms(100);
    }
}

#pragma clang diagnostic pop
