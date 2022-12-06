/**
 * @file servo_motor.c
 * @author Paul Nykiel
 * @date 27.11.22
 * Description here TODO
 */
#include "servo_motor.h"

#include <Drivers/ppm.h>

void servo_motor_init(void) {
    ppm_init();
}
void servo_motor_set(const servo_motor_cmd_t *servo_motor_cmd) {
    ppm_channel_set(1, servo_motor_cmd->servo_left + 500U);
    ppm_channel_set(2, servo_motor_cmd->motor);
    ppm_channel_set(3, servo_motor_cmd->servo_right + 500U);
}
