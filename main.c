#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

#include <avr/interrupt.h>
#include <avr/delay.h>

#include <stdbool.h>

int main(void) {
    cli();

    sei();
    while (true) {
    }
}

#pragma clang diagnostic pop