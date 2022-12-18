/**
 * @file flightcomputer.c
 * @author Paul Nykiel
 * @date 14.11.22
 * Description here TODO
 */
#include "flightcomputer.h"

#include <Drivers/protobuf.h>

void flightcomputer_init(void) {
    protobuf_init();
}

void flightcomputer_send(const imu_data_t *imu_data, const remote_data_t *remote_data,
                         const servo_motor_cmd_t *servo_motor_cmd) {
    fc_message_t message = {.has_imu = true,
                            .imu =
                                    {
                                            .imu_ok = imu_data->imu_ok,
                                            .roll_mul_16 = imu_data->roll_mul_16,
                                            .pitch_mul_16 = imu_data->pitch_mul_16,
                                            .yaw_mul_16 = imu_data->heading_mul_16,
                                            .dRoll_mul_16 = imu_data->d_roll_mul_16,
                                            .dPitch_mul_16 = imu_data->d_pitch_mul_16,
                                            .dYaw_mul_16 = imu_data->d_heading_mul_16,
                                            .accX_mul_100 = imu_data->acc_x_mul_100,
                                            .accY_mul_100 = imu_data->acc_y_mul_100,
                                            .accZ_mul_100 = imu_data->acc_z_mul_100,
                                    },
                            .has_remote = true,
                            .remote = {.throttleMixed = remote_data->throttle_mixed,
                                       .elevonLeftMixed = remote_data->elevon_left_mixed,
                                       .elevonRightMixed = remote_data->elevon_right_mixed,
                                       .throttleRaw = remote_data->throttle_raw,
                                       .pitchRaw = remote_data->pitch_raw,
                                       .rollRaw = remote_data->roll_raw,
                                       .isArmed = remote_data->is_armed,
                                       .overrideActive = remote_data->override_active,
                                       .remote_ok = remote_data->remote_ok},
                            .motor = servo_motor_cmd->motor,
                            .servoLeft = servo_motor_cmd->servo_left,
                            .servoRight = servo_motor_cmd->servo_right};

    protobuf_send_fc(&message);
}

bool flightcomputer_setpoint_available(void) {
    return protobuf_setpoint_available();
}

flightcomputer_setpoint_t flightcomputer_get_setpoint(void) {
    setpoint_message_t message = protobuf_get_setpoint();
    flightcomputer_setpoint_t setpoint = {.motor = message.motor, .pitch = message.pitch, .roll = message.roll};

    return setpoint;
}
