/**
 * @file flight_computer.c
 * @author Paul Nykiel
 * @date 14.11.22
 * @brief Implementation of the Flight-Computer interface component.
 * @ingroup Components
 */
#include "flight_computer.h"

#include <Drivers/protobuf.h>

void flight_computer_init(void) {
    protobuf_init();
}

void flight_computer_send(const imu_data_t *imu_data, const remote_data_t *remote_data,
                          const actuator_cmd_t *actuator_cmd) {
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
                                       .isArmed = remote_data->is_armed,
                                       .overrideActive = remote_data->override_active,
                                       .remote_ok = remote_data->remote_ok},
                            .motor = actuator_cmd->motor,
                            .servoLeft = actuator_cmd->elevon_left,
                            .servoRight = actuator_cmd->elevon_right};

    protobuf_send(&message);
}

bool flight_computer_set_point_available(void) {
    return protobuf_available();
}

flight_computer_set_point_t flight_computer_get_set_point(void) {
    set_point_message_t message = protobuf_get();
    flight_computer_set_point_t setpoint = {.motor = message.motor,
                                            .pitch = (int16_t) message.pitch,
                                            .roll = (int16_t) message.roll};

    return setpoint;
}
