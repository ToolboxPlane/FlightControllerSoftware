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

void setpoint_update(setpoint_t setpoint) {

}

void failsave() {

}

void timer_tick() {

}

int main(void) {
    cli();
    output_init();
    input_init();
    controller_init();
    communication_init(&setpoint_update, &failsave);
    // Runs at 244.14Hz, the BNO055 provides data at 100Hz, the output can be updated at 50Hz
    timer0_init(prescaler_256, &timer_tick);
    sei();

    state_t state;

    while (true) {
        input_get_state(&state);
        wdt_reset();
    }
}

#pragma clang diagnostic pop