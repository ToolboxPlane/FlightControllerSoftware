/**
 * @file bno055.cpp
 * @author paul
 * @date 08.11.22
 * Description here TODO
 */
#include <gtest/gtest.h>

#include <Drivers/bno055_uart.hpp>

extern "C" {
#include "../../Drivers/bno055.h"
}

void callback(bno055_response_t) {
}

TEST(TEST_NAME, eul_x_2) {
    auto bnoHandle = mock::bno055_uart.getHandle();

    int16_t out;
    bno055_eul_x_2(&out, callback);

    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_uart_read_register>(std::ignore, std::ignore, callback, std::ignore,
                                                                    std::ignore));
}
