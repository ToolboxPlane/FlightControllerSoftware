/**
 * @file error_handler.c
 * @author paul
 * @date 27.11.22
 * Description here TODO
 */
#include "error_handler.h"

#include <avr/io.h>
#include <avr/wdt.h>
#include <stdbool.h>

void error_handler_init(void) {
    DDRL = 0xFF;
}

void error_handler_handle_error(error_group_t group, uint8_t error_id) {
    uint8_t code = (group & 0x0F) << 4 | (error_id & 0x0F);
    PORTL = code;
    while (true) {
        wdt_reset();
    }
}

void error_handler_handle_warning(error_group_t group, uint8_t error_id) {
    uint8_t code = (group & 0x0F) << 4 | (error_id & 0x0F);
    PORTL = code;
}
