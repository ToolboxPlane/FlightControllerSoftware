/**
 * @file servo_motor.h
 * @author Paul Nykiel
 * @date 27.11.22
 * @brief Declaration of the servo and motor interface component.
 */
#ifndef FLIGHTCONTROLLER_SERVO_MOTOR_H
#define FLIGHTCONTROLLER_SERVO_MOTOR_H

#include <stdint.h>

typedef struct {
    uint16_t motor;
    int16_t servo_left, servo_right;
} servo_motor_cmd_t;

void servo_motor_init(void);

void servo_motor_set(const servo_motor_cmd_t *servo_motor_cmd);

#endif // FLIGHTCONTROLLER_SERVO_MOTOR_H
