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

// TODO check macros

TEST(TEST_NAME, init) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();
    bno055_init();
    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_init>());
}

TEST(TEST_NAME, write_reset) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();

    bnoUartHandle.overrideFunc<bno055_uart_write_register>(
            [](uint8_t reg, const uint8_t *data, uint8_t len, bno_callback_t /*callback*/) {
                // [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.51]
                EXPECT_EQ(reg, 0x3F);
                ASSERT_EQ(len, 1);
                EXPECT_EQ(*data, 0x20);
            });


    bno055_write_reset(nullptr);
    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_write_register>());
}

TEST(TEST_NAME, read_system_status) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();
    bno055_read_system_status(nullptr, nullptr);
    // [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.51]
    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_read_register>(0x39, 1, nullptr, nullptr, 1));
}

TEST(TEST_NAME, read_system_error) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();
    bno055_read_system_error(nullptr, nullptr);
    // [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.51]
    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_read_register>(0x3A, 1, nullptr, nullptr, 1));
}

TEST(TEST_NAME, read_calib_status) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();
    bno055_read_calib_status(nullptr, nullptr);
    // [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.51]
    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_read_register>(0x35, 1, nullptr, nullptr, 1));
}

TEST(TEST_NAME, write_opr_mode) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();

    bnoUartHandle.overrideFunc<bno055_uart_write_register>(
            [](uint8_t reg, const uint8_t *data, uint8_t len, bno_callback_t /*callback*/) {
                // [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.51]
                EXPECT_EQ(reg, 0x3D);
                ASSERT_EQ(len, 1);
                EXPECT_EQ(*data, 37);
            });


    bno055_write_opr_mode(bno055_opr_mode_t(37), nullptr);
    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_write_register>());
}

TEST(TEST_NAME, write_unit_selection) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();

    bnoUartHandle.overrideFunc<bno055_uart_write_register>(
            [](uint8_t reg, const uint8_t *data, uint8_t len, bno_callback_t /*callback*/) {
                // [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.51]
                EXPECT_EQ(reg, 0x3B);
                ASSERT_EQ(len, 1);
                EXPECT_EQ(*data, 0b10010111);
            });


    bno055_write_unit_selection(mg, rps, radians, fahrenheit, android, callback);
    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_write_register>());
}

TEST(TEST_NAME, write_remap_axis) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();

    bnoUartHandle.overrideFunc<bno055_uart_write_register>(
            [](uint8_t reg, const uint8_t *data, uint8_t len, bno_callback_t /*callback*/) {
                // [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.24, P.51]
                EXPECT_EQ(reg, 0x41);
                ASSERT_EQ(len, 1);
                EXPECT_EQ(*data, 0b00001001);
            });

    bno055_write_remap_axis(y_axis, z_axis, x_axis, callback);

    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_write_register>());
}

TEST(TEST_NAME, write_remap_axis_sign) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();

    bnoUartHandle.overrideFunc<bno055_uart_write_register>(
            [](uint8_t reg, const uint8_t *data, uint8_t len, bno_callback_t /*callback*/) {
                // [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.24, P.51]
                EXPECT_EQ(reg, 0x42);
                ASSERT_EQ(len, 1);
                EXPECT_EQ(*data, 0b00000011);
            });

    bno055_write_remap_axis_sign(positive, neg, neg, callback);

    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_write_register>());
}

TEST(TEST_NAME, read_self_test) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();
    bno055_read_self_test(nullptr, nullptr);
    // [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.51]
    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_read_register>(0x36, 1, nullptr, nullptr, 1));
}

TEST(TEST_NAME, read_acc_x_mul_100) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();
    bno055_read_acc_x_mul_100(nullptr, nullptr);
    // [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.51]
    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_read_register>(0x08, 2, nullptr, nullptr, 1));
}

TEST(TEST_NAME, read_acc_y_mul_100) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();
    bno055_read_acc_y_mul_100(nullptr, nullptr);
    // [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.51]
    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_read_register>(0x0A, 2, nullptr, nullptr, 1));
}

TEST(TEST_NAME, read_acc_z_mul_100) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();
    bno055_read_acc_z_mul_100(nullptr, nullptr);
    // [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.51]
    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_read_register>(0x0C, 2, nullptr, nullptr, 1));
}

TEST(TEST_NAME, read_gyr_x_mul_16) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();
    bno055_read_gyr_x_mul_16(nullptr, nullptr);
    // [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.51]
    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_read_register>(0x14, 2, nullptr, nullptr, 1));
}

TEST(TEST_NAME, read_gyr_y_mul_16) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();
    bno055_read_gyr_y_mul_16(nullptr, nullptr);
    // [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.51]
    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_read_register>(0x16, 2, nullptr, nullptr, 1));
}

TEST(TEST_NAME, read_gyr_z_mul_16) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();
    bno055_read_gyr_z_mul_16(nullptr, nullptr);
    // [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.51]
    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_read_register>(0x18, 2, nullptr, nullptr, 1));
}

TEST(TEST_NAME, read_eul_x_2_mul_16) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();
    bno055_read_eul_x_2_mul_16(nullptr, nullptr);
    // [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.51]
    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_read_register>(0x1A, 2, nullptr, nullptr, 1));
}

TEST(TEST_NAME, read_eul_y_2_mul_16) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();
    bno055_read_eul_y_2_mul_16(nullptr, nullptr);
    // [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.51]
    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_read_register>(0x1C, 2, nullptr, nullptr, 1));
}

TEST(TEST_NAME, read_eul_z_2_mul_16) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();
    bno055_read_eul_z_2_mul_16(nullptr, nullptr);
    // [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.51]
    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_read_register>(0x1E, 2, nullptr, nullptr, 1));
}
