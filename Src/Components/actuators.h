/**
 * @file actuators.h
 * @author Paul Nykiel
 * @date 27.11.22
 * @brief Declaration of the actuators interface component.
 * @ingroup Components
 */
#ifndef FLIGHTCONTROLLER_ACTUATORS_H
#define FLIGHTCONTROLLER_ACTUATORS_H

#include <stdint.h>

/**
 * Data to write to the actuators.
 */
typedef struct {
    uint16_t motor;
    int16_t servo_left, servo_right;
} actuator_cmd_t;

void actuators_init(void);

void actuators_set(const actuator_cmd_t *actuator_cmd);

#endif // FLIGHTCONTROLLER_ACTUATORS_H
