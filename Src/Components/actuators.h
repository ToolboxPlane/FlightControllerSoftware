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
    /**
     * The motor command in [0, 1000].
     */
    uint16_t motor;

    /**
     * The elevon commands in [-500, 500].
     */
    int16_t elevon_left, elevon_right;
} actuator_cmd_t;

/**
 * Initialize the actuators module by initializing the PPM module.
 */
void actuators_init(void);

/**
 * Set the actuators.
 * @param actuator_cmd the command to set.
 */
void actuators_set(const actuator_cmd_t *actuator_cmd);

#endif // FLIGHTCONTROLLER_ACTUATORS_H
