/**
 * @file actuators.c
 * @author Paul Nykiel
 * @date 27.11.22
 * @brief Implementation of the actuators interface component.
 * @ingroup Components
 */
#include "actuators.h"

#include <Drivers/ppm.h>

enum { SERVO_CENTRE_POSITION = 500U };

void actuators_init(void) {
    ppm_init();
}
void actuators_set(const actuator_cmd_t *actuator_cmd) {
    ppm_channel_set(CHANNEL_1, actuator_cmd->elevon_left + SERVO_CENTRE_POSITION);
    ppm_channel_set(CHANNEL_2, actuator_cmd->motor);
    ppm_channel_set(CHANNEL_3, actuator_cmd->elevon_right + SERVO_CENTRE_POSITION);
}
