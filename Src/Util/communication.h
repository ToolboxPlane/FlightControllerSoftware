/**
 * @file communication.h
 * @author paul
 * @date 16.04.19
 * @brief communication @TODO
 */

#ifndef FLIGHTCONTROLLER_COMMUNICATION_H
#define FLIGHTCONTROLLER_COMMUNICATION_H

#include <stdbool.h>

#include "../Components/imu.h"
#include "../Drivers/sbus.h"
#include "../out_state.h"
#include "../setpoint.h"

/**
 * Initialize the communication, this enables uart0 (usb) and uart2 (sbus)
 * @param setpoint_callback a function that gets called for every new package from the flightcomputer
 * @param sbus_callback a function that gets called for every new package from the sbus receiver
 */
void communication_init(void (*setpoint_callback)(setpoint_t), void (*sbus_callback)(sbus_data_t));

/**
 * Send the current state to the flightcomputer
 * @param state the state of the airplane
 * @param out_state the state of the actors
 */
void communication_send_status(volatile const imu_data_t *state, volatile const out_state_t *out_state);

/**
 * Get the sbus failsave data.
 * @return true if last sbus package had the failsave bit set
 */
bool communication_is_failsave(void);

void communication_handle(void);

#endif // FLIGHTCONTROLLER_COMMUNICATION_H
