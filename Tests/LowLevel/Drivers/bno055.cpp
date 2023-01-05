#include <Mock/bno055_uart.hpp>
#include <gtest/gtest.h>

extern "C" {
#include <Drivers/bno055.h>
}


// TODO check macros

TEST(TEST_NAME, init) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();
    bno055_init();
    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_init>());
}

TEST(TEST_NAME, write_reset) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();
    auto callbackFunc = reinterpret_cast<bno055_callback_t>(117);

    bnoUartHandle.overrideFunc<bno055_uart_write_register>(
            [callbackFunc](uint8_t reg, const uint8_t *data, uint8_t len, bno055_callback_t callback) {
                // [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.51]
                EXPECT_EQ(reg, 0x3F);
                ASSERT_EQ(len, 1);
                EXPECT_EQ(*data, 0x20);
                EXPECT_EQ(callback, callbackFunc);
            });


    bno055_write_reset(callbackFunc);
    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_write_register>());
}

TEST(TEST_NAME, read_system_status) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();
    auto callbackFunc = reinterpret_cast<bno055_callback_t>(117);
    bno055_read_system_status(nullptr, callbackFunc);
    // [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.51]
    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_read_register>(0x39, 1, callbackFunc, nullptr));
}

TEST(TEST_NAME, read_system_error) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();
    auto callbackFunc = reinterpret_cast<bno055_callback_t>(117);
    bno055_read_system_error(nullptr, callbackFunc);
    // [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.51]
    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_read_register>(0x3A, 1, callbackFunc, nullptr));
}

TEST(TEST_NAME, read_calib_status) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();
    auto callbackFunc = reinterpret_cast<bno055_callback_t>(117);
    bno055_read_calib_status(nullptr, callbackFunc);
    // [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.51]
    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_read_register>(0x35, 1, callbackFunc, nullptr));
}

TEST(TEST_NAME, read_calib_status_bitfield_packing) {
    bno055_calib_status_t calibStatus{};
    EXPECT_EQ(reinterpret_cast<const uint8_t &>(calibStatus), 0x00);
    calibStatus.mag_status = 3;
    EXPECT_EQ(reinterpret_cast<const uint8_t &>(calibStatus), 0x03);
    calibStatus.acc_status = 3;
    EXPECT_EQ(reinterpret_cast<const uint8_t &>(calibStatus), 0x0F);
    calibStatus.gyr_status = 3;
    EXPECT_EQ(reinterpret_cast<const uint8_t &>(calibStatus), 0x3F);
    calibStatus.sys_status = 3;
    EXPECT_EQ(reinterpret_cast<const uint8_t &>(calibStatus), 0xFF);
}

TEST(TEST_NAME, write_opr_mode) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();
    auto callbackFunc = reinterpret_cast<bno055_callback_t>(117);

    bnoUartHandle.overrideFunc<bno055_uart_write_register>(
            [callbackFunc](uint8_t reg, const uint8_t *data, uint8_t len, bno055_callback_t callback) {
                // [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.51]
                EXPECT_EQ(reg, 0x3D);
                ASSERT_EQ(len, 1);
                EXPECT_EQ(*data, 37);
                EXPECT_EQ(callback, callbackFunc);
            });


    bno055_write_opr_mode(bno055_opr_mode_t(37), callbackFunc);
    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_write_register>());
}

TEST(TEST_NAME, write_unit_selection) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();
    auto callbackFunc = reinterpret_cast<bno055_callback_t>(117);

    bnoUartHandle.overrideFunc<bno055_uart_write_register>(
            [callbackFunc](uint8_t reg, const uint8_t *data, uint8_t len, bno055_callback_t callback) {
                // [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.51]
                EXPECT_EQ(reg, 0x3B);
                ASSERT_EQ(len, 1);
                EXPECT_EQ(*data, 0b10010111);
                EXPECT_EQ(callback, callbackFunc);
            });


    bno055_write_unit_selection(BNO055_UNIT_SEL_ACC_MG, BNO055_UNIT_SEL_ANGULAR_RATE_RPS,
                                BNO055_UNIT_SEL_EULER_ANGLES_RADIANS, BNO055_UNIT_SEL_TEMPERATURE_FAHRENHEIT,
                                BNO055_UNIT_SEL_ORIENTATION_DEF_ANDROID, callbackFunc);
    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_write_register>());
}

TEST(TEST_NAME, write_remap_axis) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();
    auto callbackFunc = reinterpret_cast<bno055_callback_t>(117);

    bnoUartHandle.overrideFunc<bno055_uart_write_register>(
            [callbackFunc](uint8_t reg, const uint8_t *data, uint8_t len, bno055_callback_t callback) {
                // [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.24, P.51]
                EXPECT_EQ(reg, 0x41);
                ASSERT_EQ(len, 1);
                EXPECT_EQ(*data, 0b00001001);
                EXPECT_EQ(callback, callbackFunc);
            });

    bno055_write_remap_axis(BNO055_AXIS_REMAP_Y_AXIS, BNO055_AXIS_REMAP_Z_AXIS, BNO055_AXIS_REMAP_X_AXIS, callbackFunc);

    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_write_register>());
}

TEST(TEST_NAME, write_remap_axis_sign) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();
    auto callbackFunc = reinterpret_cast<bno055_callback_t>(117);

    bnoUartHandle.overrideFunc<bno055_uart_write_register>(
            [callbackFunc](uint8_t reg, const uint8_t *data, uint8_t len, bno055_callback_t callback) {
                // [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.24, P.51]
                EXPECT_EQ(reg, 0x42);
                ASSERT_EQ(len, 1);
                EXPECT_EQ(*data, 0b00000011);
                EXPECT_EQ(callback, callbackFunc);
            });

    bno055_write_remap_axis_sign(BNO055_AXIS_REMAP_SIGN_POSITIVE, BNO055_AXIS_REMAP_SIGN_NEGATIVE,
                                 BNO055_AXIS_REMAP_SIGN_NEGATIVE, callbackFunc);

    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_write_register>());
}

TEST(TEST_NAME, read_self_test) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();
    auto callbackFunc = reinterpret_cast<bno055_callback_t>(117);
    bno055_read_self_test(nullptr, callbackFunc);
    // [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.51]
    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_read_register>(0x36, 1, callbackFunc, nullptr));
}

TEST(TEST_NAME, read_self_test_bit_packing) {
    bno055_self_test_result_t selfTestResult{};
    EXPECT_EQ(reinterpret_cast<const uint8_t &>(selfTestResult), 0x0);
    selfTestResult.acc_passed = true;
    EXPECT_EQ(reinterpret_cast<const uint8_t &>(selfTestResult), 0x1);
    selfTestResult.mag_passed = true;
    EXPECT_EQ(reinterpret_cast<const uint8_t &>(selfTestResult), 0x3);
    selfTestResult.gyr_passed = true;
    EXPECT_EQ(reinterpret_cast<const uint8_t &>(selfTestResult), 0x7);
    selfTestResult.mcu_passed = true;
    EXPECT_EQ(reinterpret_cast<const uint8_t &>(selfTestResult), 0xF);
}

TEST(TEST_NAME, read_acc_xyz_mul_100) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();
    auto callbackFunc = reinterpret_cast<bno055_callback_t>(117);
    bno055_read_acc_xyz_mul_100(nullptr, callbackFunc);
    // [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.51]
    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_read_register>(0x08, 6, callbackFunc, nullptr));
}

TEST(TEST_NAME, read_gyr_x_mul_16) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();
    auto callbackFunc = reinterpret_cast<bno055_callback_t>(117);
    bno055_read_gyr_xyz_mul_16(nullptr, callbackFunc);
    // [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.51]
    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_read_register>(0x14, 6, callbackFunc, nullptr));
}

TEST(TEST_NAME, read_eul_x_2_mul_16) {
    auto bnoUartHandle = mock::bno055_uart.getHandle();
    auto callbackFunc = reinterpret_cast<bno055_callback_t>(117);
    bno055_read_eul_xyz_2_mul_16(nullptr, callbackFunc);
    // [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.51]
    EXPECT_TRUE(bnoUartHandle.functionGotCalled<bno055_uart_read_register>(0x1A, 6, callbackFunc, nullptr));
}
