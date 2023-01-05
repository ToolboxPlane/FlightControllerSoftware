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
    uint16_t motor;       ///< The motor command in [0, 1000].
    int16_t elevon_left;  ///< The command for the left elevon in [-500, 500].
    int16_t elevon_right; ///< The command for the right elevon in [-500, 500].
} actuator_cmd_t;

/**
 * @brief Initialize the actuators module.
 *
 * The initialization consists of the following tasks:
 *  * Call ::ppm_init
 */
void actuators_init(void);

/**
 * @brief Set the actuators.
 *
 * This function sets the actuators, using ::ppm_set, according to the following mapping:
 *  * elevon-left mapped from [-500, 500] to [0, 1000] to channel 1
 *  * throttle to channel 2
 *  * elevon-right mapped from [-500, 500] to [0, 1000] to channel 3
 *
 * @param actuator_cmd the command to set.
 */
void actuators_set(const actuator_cmd_t *actuator_cmd);

#endif // FLIGHTCONTROLLER_ACTUATORS_H
