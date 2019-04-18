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

void setpoint_update(setpoint_t setpoint) {
    if (!communication_is_failsave()) {
        curr_setpoint = setpoint;
    } else {
        curr_setpoint.roll = 0;
        curr_setpoint.pitch = 0;
        curr_setpoint.power = 0;
    }
}

void failsave() {
    curr_setpoint.roll = 0;
    curr_setpoint.pitch = 0;
    curr_setpoint.power = 0;
}

void timer_tick() {
    out_state_t out_state;
    controller_update(&curr_state, &curr_setpoint, &out_state);
    out_state.motor = curr_setpoint.power;
    communication_send_status(&curr_state, &out_state);
}

int main(void) {
    cli();
    output_init();
    input_init();
    controller_init(4);
    communication_init(&setpoint_update, &failsave);
    // Runs at 244.14Hz (4.096ms), the BNO055 provides data at 100Hz, the output can be updated at 50Hz
    timer0_init(prescaler_256, &timer_tick);
    sei();

    while (true) {
        input_get_state(&curr_state);
        wdt_reset();
    }
}

#pragma clang diagnostic pop