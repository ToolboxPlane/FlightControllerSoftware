#include <gtest/gtest.h>

#include <Modules/HAL/uart.hpp>


extern "C" {
#include <Drivers/bno055_uart.h>
}

TEST(TEST_NAME, init) {
    auto handle = mock::uart.getHandle();
    bno055_uart_init();
    EXPECT_TRUE(handle.functionGotCalled<uart_init>(std::ignore, 115200U, std::ignore, std::ignore, std::ignore));
}

std::optional<bno055_response_t> bnoResponse;
void bnoCallback(bno055_response_t response) {
    bnoResponse = response;
}

TEST(TEST_NAME, write_register__success) {
    auto handle = mock::uart.getHandle();
    // Setup to capture uart-Callback
    uart_callback_t uartCallback;
    handle.overrideFunc<uart_init>([&uartCallback](uint8_t /*id*/, uint16_t /*baud*/, uart_parity_t /*parity*/,
                                                   uint8_t /*stop*/,
                                                   uart_callback_t callback) -> void { uartCallback = callback; });
    bnoResponse.reset();

    bno055_uart_init();

    // Actual test
    uint8_t data[] = {38, 45};

    bno055_uart_write_register(17, data, 2, bnoCallback, nullptr, 0);
    EXPECT_TRUE(handle.functionGotCalled<uart_send_buf>()); // TODO check args

    // Datasheet P.93
    uartCallback(0xEE);
    uartCallback(0x01);

    ASSERT_TRUE(bnoResponse.has_value());
    EXPECT_TRUE(bnoResponse.value() == write_success);
}
