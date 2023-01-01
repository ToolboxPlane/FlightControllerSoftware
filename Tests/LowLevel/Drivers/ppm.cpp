#include <Mock/pwm16bit.hpp>
#include <gtest/gtest.h>

extern "C" {
#include <Drivers/ppm.h>
}

TEST(TEST_NAME, init) {
    auto handle = mock::pwm16bit.getHandle();

    ppm_init();
    EXPECT_TRUE(handle.functionGotCalled<pwm_init>(1, prescaler_8, 40000U));
    EXPECT_TRUE(handle.functionGotCalled<pwm_init>(3, prescaler_8, 40000U));
    EXPECT_TRUE(handle.functionGotCalled<pwm_init>(4, prescaler_8, 40000U));
}

TEST(TEST_NAME, ppm_channel_set_1) {
    auto handle = mock::pwm16bit.getHandle();

    ppm_channel_set(CHANNEL_1, 0);

    // Connector 1 -> PWM 4 -> P3A
    EXPECT_TRUE(handle.functionGotCalled<pwm_set_out_a>(3, std::ignore));
}

TEST(TEST_NAME, ppm_channel_set_2) {
    auto handle = mock::pwm16bit.getHandle();

    ppm_channel_set(CHANNEL_2, 0);

    // Connector 2 -> PWM 5 -> P3B
    EXPECT_TRUE(handle.functionGotCalled<pwm_set_out_b>(3, std::ignore));
}

TEST(TEST_NAME, ppm_channel_set_3) {
    auto handle = mock::pwm16bit.getHandle();

    ppm_channel_set(CHANNEL_3, 0);

    // Connector 3 -> PWM 6 -> P3C
    EXPECT_TRUE(handle.functionGotCalled<pwm_set_out_c>(3, std::ignore));
}

TEST(TEST_NAME, ppm_channel_set_4) {
    auto handle = mock::pwm16bit.getHandle();

    ppm_channel_set(CHANNEL_4, 0);

    // Connector 4 -> PWM 7 -> P4A
    EXPECT_TRUE(handle.functionGotCalled<pwm_set_out_a>(4, std::ignore));
}
TEST(TEST_NAME, ppm_channel_set_5) {
    auto handle = mock::pwm16bit.getHandle();

    ppm_channel_set(CHANNEL_5, 0);

    // Connector 5 -> PWM 8 -> P4B
    EXPECT_TRUE(handle.functionGotCalled<pwm_set_out_b>(4, std::ignore));
}
TEST(TEST_NAME, ppm_channel_set_6) {
    auto handle = mock::pwm16bit.getHandle();

    ppm_channel_set(CHANNEL_6, 0);

    // Connector 6 -> PWM 1 -> P1A
    EXPECT_TRUE(handle.functionGotCalled<pwm_set_out_a>(1, std::ignore));
}
TEST(TEST_NAME, ppm_channel_set_7) {
    auto handle = mock::pwm16bit.getHandle();

    ppm_channel_set(CHANNEL_7, 0);

    // Connector 7 -> PWM 2 -> P1B
    EXPECT_TRUE(handle.functionGotCalled<pwm_set_out_b>(1, std::ignore));
}
TEST(TEST_NAME, ppm_channel_set_8) {
    auto handle = mock::pwm16bit.getHandle();

    ppm_channel_set(CHANNEL_8, 0);

    // Connector 8 -> PWM 2 -> P1C
    EXPECT_TRUE(handle.functionGotCalled<pwm_set_out_c>(1, std::ignore));
}

TEST(TEST_NAME, ppm_channel_set_val_0) {
    auto handle = mock::pwm16bit.getHandle();

    ppm_channel_set(CHANNEL_1, 0);

    EXPECT_TRUE(handle.functionGotCalled<pwm_set_out_a>(std::ignore, 2000));
}

TEST(TEST_NAME, ppm_channel_set_val_500) {
    auto handle = mock::pwm16bit.getHandle();

    ppm_channel_set(CHANNEL_1, 500);

    EXPECT_TRUE(handle.functionGotCalled<pwm_set_out_a>(std::ignore, 3000));
}

TEST(TEST_NAME, ppm_channel_set_val_1000) {
    auto handle = mock::pwm16bit.getHandle();

    ppm_channel_set(CHANNEL_1, 1000);

    EXPECT_TRUE(handle.functionGotCalled<pwm_set_out_a>(std::ignore, 4000));
}
