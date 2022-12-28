#include <gtest/gtest.h>

extern "C" {
#include <Application/controller.h>
}

TEST(TEST_NAME, no_error) {
    imu_data_t imuData{};
    imuData.roll_mul_16 = 37 * 16;
    imuData.pitch_mul_16 = -49 * 16;
    auto controllerCmd = controller_update(&imuData, 37, -49);
    EXPECT_EQ(controllerCmd.elevon_right, 0);
    EXPECT_EQ(controllerCmd.elevon_left, 0);
}

TEST(TEST_NAME, need_roll_right) {
    imu_data_t imuData{};
    imuData.roll_mul_16 = -90 * 16;
    imuData.pitch_mul_16 = -49 * 16;
    auto controllerCmd = controller_update(&imuData, 90, -49);
    EXPECT_EQ(controllerCmd.elevon_right, 500);
    EXPECT_EQ(controllerCmd.elevon_left, -500);
}

TEST(TEST_NAME, need_roll_left) {
    imu_data_t imuData{};
    imuData.roll_mul_16 = 90 * 16;
    imuData.pitch_mul_16 = -49 * 16;
    auto controllerCmd = controller_update(&imuData, -90, -49);
    EXPECT_EQ(controllerCmd.elevon_right, -500);
    EXPECT_EQ(controllerCmd.elevon_left, 500);
}

TEST(TEST_NAME, need_pitch_up) {
    imu_data_t imuData{};
    imuData.roll_mul_16 = 37 * 16;
    imuData.pitch_mul_16 = 90 * 16;
    auto controllerCmd = controller_update(&imuData, 37, -90);
    EXPECT_EQ(controllerCmd.elevon_right, 500);
    EXPECT_EQ(controllerCmd.elevon_left, 500);
}

TEST(TEST_NAME, need_pitch_down) {
    imu_data_t imuData{};
    imuData.roll_mul_16 = 37 * 16;
    imuData.pitch_mul_16 = -90 * 16;
    auto controllerCmd = controller_update(&imuData, 37, 90);
    EXPECT_EQ(controllerCmd.elevon_right, -500);
    EXPECT_EQ(controllerCmd.elevon_left, -500);
}
