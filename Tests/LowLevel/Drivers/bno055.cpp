/**
 * @file bno055.cpp
 * @author paul
 * @date 08.11.22
 * Description here TODO
 */
#include <Modules/Drivers/bno055_uart.hpp>
#include <gtest/gtest.h>

extern "C" {
#include <Drivers/bno055.h>
}

void callback(bno055_response_t) {
}

TEST(TEST_NAME, init) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();
    bno055_init();
    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_init>());
}

TEST(TEST_NAME, reset) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();

    bnoUartHandle.overrideFunc<bno055_uart_write_register>(
            [](uint8_t reg, const uint8_t *data, uint8_t len, bno_callback_t /*callback*/) {
                // [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.51]
                EXPECT_EQ(reg, 0x3F);
                ASSERT_EQ(len, 1);
                EXPECT_EQ(*data, 0x20);
            });


    bno055_reset(nullptr);
    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_write_register>());
}

TEST(TEST_NAME, system_status) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();
    bno055_system_status(nullptr, nullptr);
    // [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.51]
    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_read_register>(0x39, 1, nullptr, nullptr, 1));
}
