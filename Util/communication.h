/**
 * @file communication.h
 * @author paul
 * @date 16.04.19
 * @brief communication @TODO
 */

#ifndef FLIGHTCONTROLLER_COMMUNICATION_H
#define FLIGHTCONTROLLER_COMMUNICATION_H

#include "../state.h"
#include "../out_state.h"
#include "../setpoint.h"

void communication_init(void (*setpoint_callback)(setpoint_t), void (*failsave_callback)(void));
void communication_send_status(const state_t *state, const out_state_t *out_state);

#endif //FLIGHTCONTROLLER_COMMUNICATION_H
