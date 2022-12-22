/**
 * @file controller.c
 * @author Paul Nykiel
 * @date 27.11.22
 * @brief Implementation of the controller application.
 */
#include "controller.h"

enum { MAX_ANGLE = 180 };

enum { MAX_INT16 = 0x7FFF };

enum { ROLL_KP = 12 };
enum { PITCH_KP = 20 };

enum { PITCH_L_SIGN = -1 };
enum { PITCH_R_SIGN = 1 };
enum { ROLL_L_SIGN = -1 };
enum { ROLL_R_SIGN = -1 };

enum { CONTROL_LIMIT = 500 };

static int16_t clamp(int16_t val, int16_t min, int16_t max) {
    if (val < min) {
        return min;
    }
    if (val > max) {
        return max;
    }
    return val;
}

controller_result_t controller_update(const imu_data_t *imu_data, int16_t roll_setpoint, int16_t pitch_setpoint) {
    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)

    // Calculate the error
    int16_t delta_roll_mul_8 = (roll_setpoint * 8 - imu_data->roll_mul_16 / 2) % (8 * MAX_ANGLE);
    int16_t delta_pitch_mul_8 = (pitch_setpoint * 8 - imu_data->pitch_mul_16 / 2) % (8 * MAX_ANGLE);

    _Static_assert((MAX_ANGLE * 8L * ROLL_KP) < MAX_INT16, "Potential overflow in roll controller");
    _Static_assert((MAX_ANGLE * 8L * PITCH_KP) < MAX_INT16, "Potential overflow in pitch controller");

    // Calculate the pseudo command
    int16_t roll_cmd_mul_8 = delta_roll_mul_8 * ROLL_KP;
    int16_t pitch_cmd_mul_8 = delta_pitch_mul_8 * PITCH_KP;

    // Channel mixing
    int16_t elevon_left_mul_4 = ROLL_L_SIGN * (roll_cmd_mul_8 / 2) + PITCH_L_SIGN * (pitch_cmd_mul_8 / 2);
    int16_t elevon_right_mul_4 = ROLL_R_SIGN * (roll_cmd_mul_8 / 2) + PITCH_R_SIGN * (pitch_cmd_mul_8 / 2);

    // Control limits
    controller_result_t result = {.elevon_left = clamp(elevon_left_mul_4 / 4, -CONTROL_LIMIT, CONTROL_LIMIT),
                                  .elevon_right = clamp(elevon_right_mul_4 / 4, -CONTROL_LIMIT, CONTROL_LIMIT)};
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

    return result;
}
