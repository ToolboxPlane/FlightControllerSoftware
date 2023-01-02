/**
 * @file error_handler.c
 * @author Paul Nykiel
 * @date 27.11.22
 * @brief Implementation of the error handler application.
 * @ingroup Application
 */
#include "error_handler.h"

#include <Components/system.h>
#include <avr/io.h>
#include <stdbool.h>

enum { DDRL_INIT = 0xFF };
enum { GROUP_MASK = 0x0FU };

void error_handler_init(void) {
    DDRL = DDRL_INIT;
    PORTL = 0;
}

void error_handler_handle_error(error_group_t group, uint8_t error_id) {
    error_handler_handle_warning(group, error_id);
    while (true) {
        system_reset_watchdog();
    }
}

void error_handler_handle_warning(error_group_t group, uint8_t error_id) {
    uint8_t code = ((uint8_t) error_id & GROUP_MASK) << 4U | (group & GROUP_MASK);
    PORTL = code;
}
