/**
 * @file input.cpp
 * @author paul
 * @date 09.11.22
 * Description here TODO
 */
#include <gtest/gtest.h>

#include <Modules/Drivers/bno055.hpp>
#include <Modules/Tests/Mock/System/util/delay.hpp>

extern "C" {
#include <Util/input.h>
}

TEST(TEST_NAME, init__success) {
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();

    bnoHandle.overrideFunc<bno055_op_mode>([](auto /*op_mode*/, bno_callback_t callback) {
        callback(write_success);
    });

    bnoHandle.overrideFunc<bno055_unit_set>([](auto /*op_mode*/, bno_callback_t callback) {
      callback(write_success);
    });

    input_init();

    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_unit_set>());
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_op_mode>(ndof_fmc_off, std::ignore));
}
