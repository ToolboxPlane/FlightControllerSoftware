#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

#include <stdbool.h>
#include <avr/interrupt.h>

#include "Util/communication.h"
#include "Util/controller.h"
#include "Util/output.h"

void setpoint_update(setpoint_t setpoint) {

}

void failsave() {

}

int main(void) {
    cli();
    controller_init();
    communication_init(&setpoint_update, &failsave);

    sei();
    while (true) {
    }
}

#pragma clang diagnostic pop