#include <gtest/gtest.h>

extern "C" {
#include <Application/controller.h>
}

TEST(TEST_NAME, no_error) {
    /*
     * Setpoint equal current attitude
     */
    imu_data_t imuData{};
    imuData.roll_mul_16 = 37 * 16;
    imuData.pitch_mul_16 = -49 * 16;
    auto controllerCmd = controller_update(&imuData, 37, -49);
    EXPECT_EQ(controllerCmd.elevon_right, 0);
    EXPECT_EQ(controllerCmd.elevon_left, 0);
}

TEST(TEST_NAME, need_roll_right) {
    /*
     * Plane is banked left (89°), should be banked right (89°).
     * Left elevon should be down, right up.
     */
    imu_data_t imuData{};
    imuData.roll_mul_16 = -89 * 16;
    imuData.pitch_mul_16 = -49 * 16;
    auto controllerCmd = controller_update(&imuData, 89, -49);
    EXPECT_EQ(controllerCmd.elevon_right, 500);
    EXPECT_EQ(controllerCmd.elevon_left, -500);
}

TEST(TEST_NAME, need_roll_left) {
    /*
     * Plane is banked right (89°), should be banked left (89°).
     * Left elevon should be up, right down.
     */
    imu_data_t imuData{};
    imuData.roll_mul_16 = 89 * 16;
    imuData.pitch_mul_16 = -49 * 16;
    auto controllerCmd = controller_update(&imuData, -89, -49);
    EXPECT_EQ(controllerCmd.elevon_right, -500);
    EXPECT_EQ(controllerCmd.elevon_left, 500);
}

TEST(TEST_NAME, need_pitch_up) {
    /*
     * Plane is pitched down (89°), should be pitched up (89°).
     * Left and right elevon should be up.
     */
    imu_data_t imuData{};
    imuData.roll_mul_16 = 37 * 16;
    imuData.pitch_mul_16 = 89 * 16;
    auto controllerCmd = controller_update(&imuData, 37, -89);
    EXPECT_EQ(controllerCmd.elevon_right, 500);
    EXPECT_EQ(controllerCmd.elevon_left, 500);
}

TEST(TEST_NAME, need_pitch_down) {
    /*
     * Plane is pitched up (89°), should be pitched down (89°).
     * Left and right elevon should be down.
     */
    imu_data_t imuData{};
    imuData.roll_mul_16 = 37 * 16;
    imuData.pitch_mul_16 = -89 * 16;
    auto controllerCmd = controller_update(&imuData, 37, 89);
    EXPECT_EQ(controllerCmd.elevon_right, -500);
    EXPECT_EQ(controllerCmd.elevon_left, -500);
}

TEST(TEST_NAME, need_roll_left_inverted) {
    /*
     * Plane is banked left (91°), should be banked right (91°), the shortest path is via inverted roll.
     * Left elevon should be up, right down.
     */
    imu_data_t imuData{};
    imuData.roll_mul_16 = -91 * 16;
    imuData.pitch_mul_16 = -49 * 16;
    auto controllerCmd = controller_update(&imuData, 91, -49);
    EXPECT_EQ(controllerCmd.elevon_right, -500);
    EXPECT_EQ(controllerCmd.elevon_left, 500);
}

TEST(TEST_NAME, need_roll_right_inverted) {
    /*
     * Plane is banked right (91°), should be banked left (91°), the shortest path is via inverted roll.
     * Left elevon should be down, right up.
     */
    imu_data_t imuData{};
    imuData.roll_mul_16 = 91 * 16;
    imuData.pitch_mul_16 = -49 * 16;
    auto controllerCmd = controller_update(&imuData, -91, -49);
    EXPECT_EQ(controllerCmd.elevon_right, 500);
    EXPECT_EQ(controllerCmd.elevon_left, -500);
}

TEST(TEST_NAME, need_pitch_down_inverted) {
    /*
     * Plane is pitched down (91°), should be pitched up (91°), the shortest path is via half-looping.
     * Left and right elevon should be down.
     */
    imu_data_t imuData{};
    imuData.roll_mul_16 = 37 * 16;
    imuData.pitch_mul_16 = 91 * 16;
    auto controllerCmd = controller_update(&imuData, 37, -91);
    EXPECT_EQ(controllerCmd.elevon_right, -500);
    EXPECT_EQ(controllerCmd.elevon_left, -500);
}

TEST(TEST_NAME, need_pitch_up_inverted) {
    /*
     * Plane is pitched up (91°), should be pitched down (91°), the shortest path is via half-looping.
     * Left and right elevon should be up.
     */
    imu_data_t imuData{};
    imuData.roll_mul_16 = 37 * 16;
    imuData.pitch_mul_16 = -91 * 16;
    auto controllerCmd = controller_update(&imuData, 37, 91);
    EXPECT_EQ(controllerCmd.elevon_right, 500);
    EXPECT_EQ(controllerCmd.elevon_left, 500);
}

TEST(TEST_NAME, no_error_roll_wrap_pos) {
    imu_data_t imuData{};
    imuData.roll_mul_16 = (37 + 4 * 360) * 16;
    imuData.pitch_mul_16 = -49 * 16;
    auto controllerCmd = controller_update(&imuData, 37, -49);
    EXPECT_EQ(controllerCmd.elevon_right, 0);
    EXPECT_EQ(controllerCmd.elevon_left, 0);
}

TEST(TEST_NAME, no_error_pitch_wrap_pos) {
    imu_data_t imuData{};
    imuData.roll_mul_16 = 37 * 16;
    imuData.pitch_mul_16 = (-49 + 4 * 360) * 16;
    auto controllerCmd = controller_update(&imuData, 37, -49);
    EXPECT_EQ(controllerCmd.elevon_right, 0);
    EXPECT_EQ(controllerCmd.elevon_left, 0);
}

TEST(TEST_NAME, no_error_roll_wrap_neg) {
    imu_data_t imuData{};
    imuData.roll_mul_16 = (37 - 4 * 360) * 16;
    imuData.pitch_mul_16 = -49 * 16;
    auto controllerCmd = controller_update(&imuData, 37, -49);
    EXPECT_EQ(controllerCmd.elevon_right, 0);
    EXPECT_EQ(controllerCmd.elevon_left, 0);
}

TEST(TEST_NAME, no_error_pitch_wrap_neg) {
    imu_data_t imuData{};
    imuData.roll_mul_16 = 37 * 16;
    imuData.pitch_mul_16 = (-49 - 4 * 360) * 16;
    auto controllerCmd = controller_update(&imuData, 37, -49);
    EXPECT_EQ(controllerCmd.elevon_right, 0);
    EXPECT_EQ(controllerCmd.elevon_left, 0);
}
