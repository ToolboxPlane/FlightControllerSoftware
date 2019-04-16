/**
 * @file communication.h
 * @author paul
 * @date 16.04.19
 * @brief communication @TODO
 */

#ifndef FLIGHTCONTROLLER_COMMUNICATION_H
#define FLIGHTCONTROLLER_COMMUNICATION_H

#include "../state.h"
#include "../control.h"

void communication_init(void);
void communication_send_status(const state_t *state, const control_t *control); //@TODO arg

#endif //FLIGHTCONTROLLER_COMMUNICATION_H
