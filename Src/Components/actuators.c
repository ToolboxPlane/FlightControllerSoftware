/**
 * @file actuators.c
 * @author Paul Nykiel
 * @date 27.11.22
 * @brief Implementation of the actuators interface component.
 * @ingroup Components
 */
#include <Drivers/ppm.h>

#include "actuators.h"

enum { SERVO_CENTRE_POSITION = 500U };

void actuators_init(void) {
    ppm_init();
}
void actuators_set(const servo_motor_cmd_t *servo_motor_cmd) {
    ppm_channel_set(CHANNEL_1, servo_motor_cmd->servo_left + SERVO_CENTRE_POSITION);
    ppm_channel_set(CHANNEL_2, servo_motor_cmd->motor);
    ppm_channel_set(CHANNEL_3, servo_motor_cmd->servo_right + SERVO_CENTRE_POSITION);
}
