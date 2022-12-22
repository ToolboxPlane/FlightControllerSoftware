/**
 * @file error_handler.c
 * @author Paul Nykiel
 * @date 27.11.22
 * @brief Implementation of the error handler application.
 */
#include "error_handler.h"

#include <avr/io.h>
#include <avr/wdt.h>
#include <stdbool.h>

void error_handler_init(void) {
    DDRL = 0xFF;
    PORTL = 0;
}

void error_handler_handle_error(error_group_t group, uint8_t error_id) {
    uint8_t code = (error_id & 0x0F) << 4 | (group & 0x0F);
    PORTL = code;
    while (true) {
        wdt_reset();
    }
}

void error_handler_handle_warning(error_group_t group, uint8_t error_id) {
    uint8_t code = (error_id & 0x0F) << 4 | (group & 0x0F);
    PORTL = code;
}
