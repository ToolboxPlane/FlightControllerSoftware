#include <Mock/Drivers/sbus.hpp>
#include <gtest/gtest.h>

extern "C" {
#include <Components/remote.h>
}

TEST(TEST_NAME, init) {
    auto handle = mock::sbus.getHandle();
    remote_init();
    EXPECT_TRUE(handle.functionGotCalled<sbus_init>());
}

TEST(TEST_NAME, data_available) {
    auto handle = mock::sbus.getHandle();
    handle.overrideFunc<sbus_data_available>([]() { return true; });
    EXPECT_TRUE(remote_data_available());
}

TEST(TEST_NAME, no_data_available) {
    auto handle = mock::sbus.getHandle();
    handle.overrideFunc<sbus_data_available>([]() { return false; });
    EXPECT_FALSE(remote_data_available());
}

TEST(TEST_NAME, get_data_ok) {
    auto handle = mock::sbus.getHandle();
    remote_init();

    handle.overrideFunc<sbus_get_latest_data>([]() {
        return sbus_data_t{
                .channel = {172, 1811, 172, 1811, 172, 1811, 172, 1811},
                .failsave = false,
                .frame_lost = false,
        };
    });

    auto data = remote_get_data();

    EXPECT_TRUE(data.remote_ok);
    EXPECT_LE(data.throttle_mixed, 0 + 1);
    EXPECT_GE(data.elevon_left_mixed, 1000 - 1);
    EXPECT_LE(data.elevon_right_mixed, 0 + 1);
    EXPECT_GE(data.throttle_raw, 1000 - 1);
    EXPECT_LE(data.pitch_raw, 0 + 1);
    EXPECT_GE(data.roll_raw, 1000 - 1);
    EXPECT_TRUE(data.is_armed);
    EXPECT_TRUE(data.override_active);
}

TEST(TEST_NAME, get_data_failsve) {
    auto handle = mock::sbus.getHandle();
    remote_init();

    handle.overrideFunc<sbus_get_latest_data>([]() {
        return sbus_data_t{
                .channel = {1000},
                .failsave = true,
                .frame_lost = false,
        };
    });

    auto data = remote_get_data();

    EXPECT_FALSE(data.remote_ok);
}

TEST(TEST_NAME, get_data_frame_lost) {
    auto handle = mock::sbus.getHandle();
    remote_init();

    handle.overrideFunc<sbus_get_latest_data>([]() {
        return sbus_data_t{
                .channel = {1000},
                .failsave = false,
                .frame_lost = true,
        };
    });

    auto data = remote_get_data();

    EXPECT_FALSE(data.remote_ok);
}
