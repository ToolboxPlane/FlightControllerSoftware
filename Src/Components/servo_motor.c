/**
 * @file servo_motor.c
 * @author Paul Nykiel
 * @date 27.11.22
 * @brief Implementation of the servo and motor interface component.
 */
#include "servo_motor.h"

#include <Drivers/ppm.h>

enum { SERVO_CENTRE_POSITION = 500U };

void servo_motor_init(void) {
    ppm_init();
}
void servo_motor_set(const servo_motor_cmd_t *servo_motor_cmd) {
    ppm_channel_set(1, servo_motor_cmd->servo_left + SERVO_CENTRE_POSITION);
    ppm_channel_set(2, servo_motor_cmd->motor);
    ppm_channel_set(3, servo_motor_cmd->servo_right + SERVO_CENTRE_POSITION);
}
