#include <stdbool.h>

#include "Application/error_handler.h"
#include "Components/flightcomputer.h"
#include "Components/imu.h"
#include "Components/remote.h"
#include "Components/servo_motor.h"
#include "Components/system.h"

void timer_tick(void) {
    if (!imu_data_available()) {
        error_handler_handle_warning(IMU, 14);
    }

    servo_motor_cmd_t servo_motor_cmd;

    if (!remote_data_available()) {
        error_handler_handle_warning(REMOTE, 15);
        servo_motor_cmd.servo_left = 0;
        servo_motor_cmd.servo_right = 0;
        servo_motor_cmd.servo_right = 0;
    } else {
        remote_data_t remote_data = remote_get_data();

        if (!remote_data.remote_ok) {
            error_handler_handle_warning(REMOTE, 14);
            servo_motor_cmd.servo_left = 0;
            servo_motor_cmd.servo_right = 0;
            servo_motor_cmd.servo_right = 0;
        }
    }

    // Handle Ti
}

int main(void) {
    system_init(timer_tick);
    error_handler_init();

    imu_init();
    remote_init();
    flightcomputer_init();
    servo_motor_init();

    imu_start_sampling();

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while (true) {
        system_reset_watchdog();
    }
#pragma clang diagnostic pop
}
