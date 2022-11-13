/**
 * @file input.cpp
 * @author paul
 * @date 09.11.22
 * Description here TODO
 */
#include <Modules/Drivers/bno055.hpp>
#include <Modules/Tests/Mock/System/util/delay.hpp>
#include <gtest/gtest.h>

extern "C" {
#include <Util/input.h>
}

TEST(TEST_NAME, init__success) {
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();

    bnoHandle.overrideFunc<bno055_op_mode>([](auto /*op_mode*/, bno_callback_t callback) { callback(write_success); });

    bnoHandle.overrideFunc<bno055_unit_set>([](auto /*op_mode*/, bno_callback_t callback) { callback(write_success); });

    EXPECT_TRUE(input_init());

    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_unit_set>());
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_op_mode>(ndof_fmc_off, std::ignore));
}

TEST(TEST_NAME, init__bno_error) {
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();

    bnoHandle.overrideFunc<bno055_op_mode>([](auto /*op_mode*/, bno_callback_t callback) { callback(write_fail); });

    bnoHandle.overrideFunc<bno055_unit_set>([](auto /*op_mode*/, bno_callback_t callback) { callback(write_success); });

    EXPECT_FALSE(input_init());
}

TEST(TEST_NAME, init__timeout) {
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();

    EXPECT_FALSE(input_init());
}

TEST(TEST_NAME, read__full_read) {
    auto bnoHandle = mock::bno055.getHandle();
    bool alreadyCalled = false;

    bnoHandle.overrideFunc<bno055_eul_x_2>([](int16_t *out, bno_callback_t callback) {
        *out = 101;
        callback(read_success);
    });

    bnoHandle.overrideFunc<bno055_eul_y_2>([&alreadyCalled](int16_t *out, bno_callback_t callback) {
        if (not alreadyCalled) {
            *out = 102;
            alreadyCalled = true;
            callback(read_success);
        } else {
            callback(read_fail); // Break the sampling process
        }
    });

    bnoHandle.overrideFunc<bno055_eul_z_2>([](int16_t *out, bno_callback_t callback) {
        *out = 103;
        callback(read_success);
    });

    bnoHandle.overrideFunc<bno055_gyr_x>([](int16_t *out, bno_callback_t callback) {
        *out = 104;
        callback(read_success);
    });

    bnoHandle.overrideFunc<bno055_gyr_y>([](int16_t *out, bno_callback_t callback) {
        *out = 105;
        callback(read_success);
    });

    bnoHandle.overrideFunc<bno055_gyr_z>([](int16_t *out, bno_callback_t callback) {
        *out = 106;
        callback(read_success);
    });

    bnoHandle.overrideFunc<bno055_acc_x>([](int16_t *out, bno_callback_t callback) {
        *out = 107;
        callback(read_success);
    });

    bnoHandle.overrideFunc<bno055_acc_y>([](int16_t *out, bno_callback_t callback) {
        *out = 108;
        callback(read_success);
    });

    bnoHandle.overrideFunc<bno055_acc_z>([](int16_t *out, bno_callback_t callback) {
        *out = 109;
        callback(read_success);
    });

    bnoHandle.overrideFunc<bno055_system_status>([](bno055_status_t *out, bno_callback_t callback) {
        *out = bno055_status_t::sensor_fusion_algorithm_running;
        callback(read_success);
    });

    bnoHandle.overrideFunc<bno055_calib_stat>([](uint8_t *out, bno_callback_t callback) {
      *out = 110;
      callback(read_success);
    });

    EXPECT_NO_THROW(input_start_sampling());

    auto state = input_get_state();

    EXPECT_EQ(state.bno_state, bno055_status_t::sensor_fusion_algorithm_running);
}
