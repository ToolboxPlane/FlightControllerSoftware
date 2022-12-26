#include <Mock/Drivers/ppm.hpp>
#include <gtest/gtest.h>

extern "C" {
#include <Components/actuators.h>
}

TEST(TEST_NAME, init) {
    auto handle = mock::ppm.getHandle();

    actuators_init();
    EXPECT_TRUE(handle.functionGotCalled<ppm_init>());
}

TEST(TEST_NAME, set) {
    auto handle = mock::ppm.getHandle();

    actuator_cmd_t actuatorCmd{.motor = 700, .elevon_left = -100, .elevon_right = 100};

    actuators_set(&actuatorCmd);

    EXPECT_TRUE(handle.functionGotCalled<ppm_channel_set>(CHANNEL_1, 400));
    EXPECT_TRUE(handle.functionGotCalled<ppm_channel_set>(CHANNEL_2, 700));
    EXPECT_TRUE(handle.functionGotCalled<ppm_channel_set>(CHANNEL_3, 600));
}
