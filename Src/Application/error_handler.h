/**
 * @file error_handler.h
 * @author paul
 * @date 27.11.22
 * Description here TODO
 */
#ifndef FLIGHTCONTROLLER_ERROR_HANDLER_H
#define FLIGHTCONTROLLER_ERROR_HANDLER_H

#include <stdint.h>

typedef enum { APPLICATION, SYSTEM, IMU, FLIGHTCOMPUTER, REMOTE } error_group_t;

void error_handler_init(void);

void error_handler_handle_error(error_group_t group, uint8_t error_id);

void error_handler_handle_warning(error_group_t group, uint8_t error_id);

#endif // FLIGHTCONTROLLER_ERROR_HANDLER_H
