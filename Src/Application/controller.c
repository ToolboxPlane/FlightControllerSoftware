/**
 * @file controller.c
 * @author Paul Nykiel
 * @date 27.11.22
 * @brief Implementation of the controller application.
 * @ingroup Application
 */
#include "controller.h"

enum { MAX_ANGLE = 180 };

enum { MAX_INT16 = 0x7FFF };

enum { ROLL_KP = 12 };
enum { PITCH_KP = 20 };

enum { CONTROL_LIMIT = 500 };

static int16_t apply_control_limit(int16_t val) {
    if (val < -CONTROL_LIMIT) {
        return -CONTROL_LIMIT;
    }
    if (val > CONTROL_LIMIT) {
        return CONTROL_LIMIT;
    }
    return val;
}

static int16_t wrap_to_pm180_mul_8(int16_t angle) {
    angle %= 8 * 360;
    if (angle > 8 * 180) {
        return angle - 8 * 360;
    }
    if (angle < 8 * -180) {
        return angle + 8 * 360;
    }
    return angle;
}

controller_result_t controller_update(const imu_data_t *imu_data, int16_t roll_setpoint, int16_t pitch_setpoint) {
    /*
     * Calculate the error:
     *  * a positive roll error should result in the plane banking right
     *  * a positive pitch error should result in the plane pitching down
     */
    int16_t delta_roll_mul_8 = wrap_to_pm180_mul_8(roll_setpoint * 8 - imu_data->roll_mul_16 / 2);
    int16_t delta_pitch_mul_8 = wrap_to_pm180_mul_8(pitch_setpoint * 8 - imu_data->pitch_mul_16 / 2);

    _Static_assert((MAX_ANGLE * 8L * ROLL_KP) < MAX_INT16, "Potential overflow in roll controller");
    _Static_assert((MAX_ANGLE * 8L * PITCH_KP) < MAX_INT16, "Potential overflow in pitch controller");

    // Calculate the pseudo command
    int16_t roll_cmd_mul_8 = delta_roll_mul_8 * ROLL_KP;
    int16_t pitch_cmd_mul_8 = delta_pitch_mul_8 * PITCH_KP;

    /*
     * Channel mixing:
     *  * To bank right:
     *      * the left elevon needs to deflect downwards (i.e. negative)
     *      * the right elevon needs to deflect upwards (i.e. positive)
     *  * To pitch down:
     *      * the left and right elevon need to deflect downwards (i.e. negative)
     */
    int16_t elevon_left_mul_4 = -(roll_cmd_mul_8 / 2) - (pitch_cmd_mul_8 / 2);
    int16_t elevon_right_mul_4 = (roll_cmd_mul_8 / 2) - (pitch_cmd_mul_8 / 2);

    // TODO fix sign, currently positive values should result in deflection upwards

    // Control limits
    controller_result_t result = {.elevon_left = apply_control_limit(elevon_left_mul_4 / 4),
                                  .elevon_right = apply_control_limit(elevon_right_mul_4 / 4)};

    return result;
}
