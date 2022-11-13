#include <Modules/HAL/uart.hpp>
#include <gtest/gtest.h>


extern "C" {
#include <Drivers/bno055_uart.h>
}

TEST(TEST_NAME, init) {
    auto handle = mock::uart.getHandle();
    bno055_uart_init();

    /*
     * The BNO055 supports UART interface with the following settings: 115200 bps, 8N1 (8 data bits, no parity bit, one
     * stop bit) [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P. 93]
     */
    EXPECT_TRUE(handle.functionGotCalled<uart_init>(BNO_UART_ID, 115200U, uart_parity_t::NONE, 1, std::ignore));
}

TEST(TEST_NAME, write_register__buffer_format) {
    auto handle = mock::uart.getHandle();

    // Actual test
    uint8_t data[] = {38, 45};

    handle.overrideFunc<uart_send_buf>([](uint8_t id, const uint8_t *buf, uint16_t size) {
        EXPECT_EQ(id, BNO_UART_ID);

        /*
         * Format [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.93]:
         * | Byte 1     | Byte 2 | Byte 3   | Byte 4 | Byte 5 | ... | Byte (n+4) |
         * | Start Byte | Write  | Reg addr | Length | Data 1 | ... | Data n     |
         * | 0xAA       | 0x00   | <..>     | <..>   | <..>   | ... | <..>       |
         */
        EXPECT_EQ(size, 4+2);
        EXPECT_EQ(buf[0], 0xAA);
        EXPECT_EQ(buf[1], 0x00);
        EXPECT_EQ(buf[2], 17);
        EXPECT_EQ(buf[3], 2);
        EXPECT_EQ(buf[4], 38);
        EXPECT_EQ(buf[5], 45);
    });

    bno055_uart_write_register(17, data, 2, nullptr, nullptr, 0);
    EXPECT_TRUE(handle.functionGotCalled<uart_send_buf>());
}

TEST(TEST_NAME, read_register__buffer_format) {
    auto handle = mock::uart.getHandle();
    // Setup to capture uart-Callback
    uart_callback_t uartCallback;
    handle.overrideFunc<uart_init>([&uartCallback](uint8_t /*id*/, uint16_t /*baud*/, uart_parity_t /*parity*/,
                                                   uint8_t /*stop*/,
                                                   uart_callback_t callback) -> void { uartCallback = callback; });

    bno055_uart_init();

    // Actual test
    handle.overrideFunc<uart_send_buf>([](uint8_t id, const uint8_t *buf, uint16_t size) {
      EXPECT_EQ(id, BNO_UART_ID);

      /*
       * Format [https://cdn-shop.adafruit.com/datasheets/BST_BNO055_DS000_12.pdf P.93]:
       * | Byte 1     | Byte 2 | Byte 3   | Byte 4 |
       * | Start Byte | Write  | Reg addr | Length |
       * | 0xAA       | 0x01   | <..>     | <..>   |
       */
      EXPECT_EQ(size, 4);
      EXPECT_EQ(buf[0], 0xAA);
      EXPECT_EQ(buf[1], 0x01);
      EXPECT_EQ(buf[2], 17);
      EXPECT_EQ(buf[3], 34);
    });

    bno055_uart_read_register(17, 34, nullptr, nullptr, 0);
    EXPECT_TRUE(handle.functionGotCalled<uart_send_buf>());
}

std::optional<bno055_response_t> bnoResponse;
void bnoCallback(bno055_response_t response) {
    bnoResponse = response;
}

TEST(TEST_NAME, write_register__write_success) {
    auto handle = mock::uart.getHandle();
    // Setup to capture uart-Callback
    uart_callback_t uartCallback;
    handle.overrideFunc<uart_init>([&uartCallback](uint8_t /*id*/, uint16_t /*baud*/, uart_parity_t /*parity*/,
                                                   uint8_t /*stop*/,
                                                   uart_callback_t callback) -> void { uartCallback = callback; });

    bno055_uart_init();
    bnoResponse.reset();

    // Actual test
    uint8_t data[] = {38, 45};

    bno055_uart_write_register(0, data, 2, bnoCallback, nullptr, 0);

    uartCallback(0xEE);
    uartCallback(0x01);

    ASSERT_TRUE(bnoResponse.has_value());
    EXPECT_EQ(bnoResponse.value(), bno055_response_t::write_success);
}

TEST(TEST_NAME, write_register__write_wrong_start_byte) {
    auto handle = mock::uart.getHandle();
    // Setup to capture uart-Callback
    uart_callback_t uartCallback;
    handle.overrideFunc<uart_init>([&uartCallback](uint8_t /*id*/, uint16_t /*baud*/, uart_parity_t /*parity*/,
                                                   uint8_t /*stop*/,
                                                   uart_callback_t callback) -> void { uartCallback = callback; });

    bno055_uart_init();
    bnoResponse.reset();

    // Actual test
    uint8_t data[] = {38, 45};

    bno055_uart_write_register(0, data, 2, bnoCallback, nullptr, 0);

    uartCallback(0xEE);
    uartCallback(0x06);

    ASSERT_TRUE(bnoResponse.has_value());
    EXPECT_EQ(bnoResponse.value(), bno055_response_t::wrong_start_byte);
}

TEST(TEST_NAME, read_register__read_success_8bit_div1) {
    auto handle = mock::uart.getHandle();
    // Setup to capture uart-Callback
    uart_callback_t uartCallback;
    handle.overrideFunc<uart_init>([&uartCallback](uint8_t /*id*/, uint16_t /*baud*/, uart_parity_t /*parity*/,
                                                   uint8_t /*stop*/,
                                                   uart_callback_t callback) -> void { uartCallback = callback; });

    bno055_uart_init();
    bnoResponse.reset();

    // Actual test
    uint8_t result;
    bno055_uart_read_register(0, 1, bnoCallback, &result, 1);

    uartCallback(0xBB);
    uartCallback(0x01);
    uartCallback(37);

    ASSERT_TRUE(bnoResponse.has_value());
    EXPECT_EQ(bnoResponse.value(), bno055_response_t::read_success);
    EXPECT_EQ(result, 37);
}

TEST(TEST_NAME, read_register__read_success_8bit_div8) {
    auto handle = mock::uart.getHandle();
    // Setup to capture uart-Callback
    uart_callback_t uartCallback;
    handle.overrideFunc<uart_init>([&uartCallback](uint8_t /*id*/, uint16_t /*baud*/, uart_parity_t /*parity*/,
                                                   uint8_t /*stop*/,
                                                   uart_callback_t callback) -> void { uartCallback = callback; });

    bno055_uart_init();
    bnoResponse.reset();

    // Actual test
    uint8_t result;
    bno055_uart_read_register(0, 1, bnoCallback, &result, 8);

    uartCallback(0xBB);
    uartCallback(0x01);
    uartCallback(128);

    ASSERT_TRUE(bnoResponse.has_value());
    EXPECT_EQ(bnoResponse.value(), bno055_response_t::read_success);
    EXPECT_EQ(result, 128/8);
}

TEST(TEST_NAME, read_register__read_success_16bit_div1) {
    auto handle = mock::uart.getHandle();
    // Setup to capture uart-Callback
    uart_callback_t uartCallback;
    handle.overrideFunc<uart_init>([&uartCallback](uint8_t /*id*/, uint16_t /*baud*/, uart_parity_t /*parity*/,
                                                   uint8_t /*stop*/,
                                                   uart_callback_t callback) -> void { uartCallback = callback; });

    bno055_uart_init();
    bnoResponse.reset();

    // Actual test
    uint16_t result;
    bno055_uart_read_register(0, 2, bnoCallback, &result, 1);

    uartCallback(0xBB);
    uartCallback(0x02);
    uartCallback(37); // LSB
    uartCallback(9); // MSB

    ASSERT_TRUE(bnoResponse.has_value());
    EXPECT_EQ(bnoResponse.value(), bno055_response_t::read_success);
    EXPECT_EQ(result, 37 + 9 * 256);
}

TEST(TEST_NAME, read_register__read_success_16bit_div8) {
    auto handle = mock::uart.getHandle();
    // Setup to capture uart-Callback
    uart_callback_t uartCallback;
    handle.overrideFunc<uart_init>([&uartCallback](uint8_t /*id*/, uint16_t /*baud*/, uart_parity_t /*parity*/,
                                                   uint8_t /*stop*/,
                                                   uart_callback_t callback) -> void { uartCallback = callback; });

    bno055_uart_init();
    bnoResponse.reset();

    // Actual test
    uint16_t result;
    bno055_uart_read_register(0, 2, bnoCallback, &result, 8);

    uartCallback(0xBB);
    uartCallback(0x02);
    uartCallback(128); // LSB
    uartCallback(13); // MSB

    ASSERT_TRUE(bnoResponse.has_value());
    EXPECT_EQ(bnoResponse.value(), bno055_response_t::read_success);
    EXPECT_EQ(result, (128 + 256*13)/8);
}

TEST(TEST_NAME, read_register__buffer_invalid) {
    auto handle = mock::uart.getHandle();
    // Setup to capture uart-Callback
    uart_callback_t uartCallback;
    handle.overrideFunc<uart_init>([&uartCallback](uint8_t /*id*/, uint16_t /*baud*/, uart_parity_t /*parity*/,
                                                   uint8_t /*stop*/,
                                                   uart_callback_t callback) -> void { uartCallback = callback; });

    bno055_uart_init();
    bnoResponse.reset();

    // Actual test
    bno055_uart_read_register(0, 1, bnoCallback, nullptr, 1);

    uartCallback(0xBB);
    uartCallback(0x01);
    uartCallback(17);

    ASSERT_TRUE(bnoResponse.has_value());
    EXPECT_EQ(bnoResponse.value(), bno055_response_t::callback_buffer_invalid);
}

TEST(TEST_NAME, read_register__length_invalid) {
    auto handle = mock::uart.getHandle();
    // Setup to capture uart-Callback
    uart_callback_t uartCallback;
    handle.overrideFunc<uart_init>([&uartCallback](uint8_t /*id*/, uint16_t /*baud*/, uart_parity_t /*parity*/,
                                                   uint8_t /*stop*/,
                                                   uart_callback_t callback) -> void { uartCallback = callback; });

    bno055_uart_init();
    bnoResponse.reset();

    // Actual test
    uint32_t buf;
    bno055_uart_read_register(0, 4, bnoCallback, &buf, 1);

    uartCallback(0xBB);
    uartCallback(0x04);
    uartCallback(17);
    uartCallback(17);
    uartCallback(17);
    uartCallback(17);

    ASSERT_TRUE(bnoResponse.has_value());
    EXPECT_EQ(bnoResponse.value(), bno055_response_t::callback_length_invalid);
}

TEST(TEST_NAME, read_register__read_fail) {
    auto handle = mock::uart.getHandle();
    // Setup to capture uart-Callback
    uart_callback_t uartCallback;
    handle.overrideFunc<uart_init>([&uartCallback](uint8_t /*id*/, uint16_t /*baud*/, uart_parity_t /*parity*/,
                                                   uint8_t /*stop*/,
                                                   uart_callback_t callback) -> void { uartCallback = callback; });

    bno055_uart_init();
    bnoResponse.reset();

    // Actual test
    bno055_uart_read_register(0, 1, bnoCallback, nullptr, 1);

    uartCallback(0xEE);
    uartCallback(0x02);

    ASSERT_TRUE(bnoResponse.has_value());
    EXPECT_EQ(bnoResponse.value(), bno055_response_t::read_fail);
}
