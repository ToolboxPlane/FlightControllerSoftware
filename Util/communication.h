/**
 * @file communication.h
 * @author paul
 * @date 16.04.19
 * @brief communication @TODO
 */

#ifndef FLIGHTCONTROLLER_COMMUNICATION_H
#define FLIGHTCONTROLLER_COMMUNICATION_H

#include <stdbool.h>
#include "../state.h"
#include "../out_state.h"
#include "../setpoint.h"

/**
 * Initialize the communication, this enables uart0 (usb) and uart2 (sbus)
 * @param setpoint_callback a function that gets called for every new package from the flightcomputer
 * @param failsave_callback a function that gets called on failsave
 */
void communication_init(void (*setpoint_callback)(setpoint_t), void (*failsave_callback)(void));

/**
 * Send the current state to the flightcomputer
 * @param state the state of the airplane
 * @param out_state the state of the actors
 */
void communication_send_status(const state_t *state, const out_state_t *out_state);

/**
 * Get the sbus failsave data.
 * @return true if last sbus package had the failsave bit set
 */
bool communication_is_failsave(void);

#endif //FLIGHTCONTROLLER_COMMUNICATION_H
