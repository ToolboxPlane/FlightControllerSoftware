#include <Mock/Drivers/ppm.hpp>
#include <gtest/gtest.h>

extern "C" {
#include <Components/servo_motor.h>
}

TEST(TEST_NAME, init) {
    auto handle = mock::ppm.getHandle();

    servo_motor_init();
    EXPECT_TRUE(handle.functionGotCalled<ppm_init>());
}

TEST(TEST_NAME, set) {
    auto handle = mock::ppm.getHandle();

    servo_motor_cmd_t servoMotorCmd{.motor = 700, .servo_left = -100, .servo_right = 100};

    servo_motor_set(&servoMotorCmd);

    EXPECT_TRUE(handle.functionGotCalled<ppm_channel_set>(CHANNEL_1, 400));
    EXPECT_TRUE(handle.functionGotCalled<ppm_channel_set>(CHANNEL_2, 700));
    EXPECT_TRUE(handle.functionGotCalled<ppm_channel_set>(CHANNEL_3, 600));
}
