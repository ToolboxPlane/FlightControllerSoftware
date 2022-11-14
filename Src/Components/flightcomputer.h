/**
 * @file flightcomputer.h
 * @author paul
 * @date 14.11.22
 * Description here TODO
 */
#ifndef FLIGHTCONTROLLER_FLIGHTCOMPUTER_H
#define FLIGHTCONTROLLER_FLIGHTCOMPUTER_H

typedef struct {

} flightcomputer_send_data_t;

typedef struct {

} flightcomputer_receive_data_t;

void flightcomputer_init(void);

void flightcomputer_send(const flightcomputer_send_data_t *data);

bool flightcomputer_data_available();

flightcomputer_receive_data_t flightcomputer_get_data();

#endif // FLIGHTCONTROLLER_FLIGHTCOMPUTER_H
