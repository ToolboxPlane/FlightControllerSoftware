/**
 * @file flightcomputer.h
 * @author paul
 * @date 14.11.22
 * Description here TODO
 */
#ifndef FLIGHTCONTROLLER_FLIGHTCOMPUTER_H
#define FLIGHTCONTROLLER_FLIGHTCOMPUTER_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {

} flightcomputer_send_data_t;

typedef struct {

} flightcomputer_receive_data_t;

void flightcomputer_init(void);

void flightcomputer_send(const flightcomputer_send_data_t *data);

bool flightcomputer_data_available(void);

flightcomputer_receive_data_t flightcomputer_get_data(void);

#endif // FLIGHTCONTROLLER_FLIGHTCOMPUTER_H
