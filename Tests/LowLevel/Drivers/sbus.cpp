#include <Mock/Drivers/ring_buffer.hpp>
#include <Mock/HAL/uart.hpp>
#include <gtest/gtest.h>

extern "C" {
#include <Drivers/sbus.h>
}

TEST(TEST_NAME, init) {
    auto uartHandle = mock::uart.getHandle();
    auto ringBufferHandle = mock::ring_buffer.getHandle();
    ringBufferHandle.overrideFunc<ring_buffer_init>([]() { return ring_buffer_data_t{}; });
    ringBufferHandle.overrideFunc<ring_buffer_put>(
            [](ring_buffer_data_t * /*ring_buffer_data*/, uint8_t /*data*/) { return true; });
    sbus_init();
    /*
     * https://github.com/bolderflight/sbus/blob/main/README.md:
     * The SBUS protocol uses an inverted serial logic with a baud rate of 100000, 8 data bits, even parity, and 2 stop
     * bits
     */
    EXPECT_TRUE(uartHandle.functionGotCalled<uart_init>(2, 100000U, EVEN, 2, std::ignore));
    EXPECT_TRUE(ringBufferHandle.functionGotCalled<ring_buffer_init>());
}

TEST(TEST_NAME, rx_fill_buffer) {
    auto uartHandle = mock::uart.getHandle();
    auto ringBufferHandle = mock::ring_buffer.getHandle();
    ringBufferHandle.overrideFunc<ring_buffer_init>([]() { return ring_buffer_data_t{}; });
    ringBufferHandle.overrideFunc<ring_buffer_put>(
            [](ring_buffer_data_t * /*ring_buffer_data*/, uint8_t /*data*/) { return true; });

    uart_callback_t uartCallback = nullptr;
    uartHandle.overrideFunc<uart_init>([&uartCallback](uint8_t /*id*/, uint16_t /*baud*/, uart_parity_t /*parity*/,
                                                       uint8_t /*stop_bits*/,
                                                       uart_callback_t callback) { uartCallback = callback; });
    sbus_init();
    EXPECT_TRUE(uartHandle.functionGotCalled<uart_init>());

    uartCallback(17);
    EXPECT_TRUE(ringBufferHandle.functionGotCalled<ring_buffer_put>(std::ignore, 17));

    uartCallback(38);
    EXPECT_TRUE(ringBufferHandle.functionGotCalled<ring_buffer_put>(std::ignore, 38));

    uartCallback(54);
    EXPECT_TRUE(ringBufferHandle.functionGotCalled<ring_buffer_put>(std::ignore, 54));
}

TEST(TEST_NAME, available_read_buffer) {
    auto uartHandle = mock::uart.getHandle();
    auto ringBufferHandle = mock::ring_buffer.getHandle();
    ringBufferHandle.overrideFunc<ring_buffer_init>([]() { return ring_buffer_data_t{}; });
    sbus_init();
    EXPECT_TRUE(uartHandle.functionGotCalled<uart_init>());

    std::size_t count = 0;
    ringBufferHandle.overrideFunc<ring_buffer_get>([&count](ring_buffer_data_t *ringBufferData, uint8_t *out) {
        EXPECT_NE(ringBufferData, nullptr);
        count += 1;
        *out = count;
        return count <= 4;
    });
}

TEST(TEST_NAME, decode) {
    auto uartHandle = mock::uart.getHandle();
    auto ringBufferHandle = mock::ring_buffer.getHandle();
    ringBufferHandle.overrideFunc<ring_buffer_init>([]() { return ring_buffer_data_t{}; });

    /*
     * https://github.com/bolderflight/sbus/blob/main/README.md:
     *
     * The SBUS packet is 25 bytes long consisting of:
     *  - Byte[0]: SBUS header, 0x0F
     *  - Byte[1 -22]: 16 servo channels, 11 bits each
     *  - Byte[23]
     *      - Bit 0: channel 17 (0x01)
     *      - Bit 1: channel 18 (0x02)
     *      - Bit 2: frame lost (0x04)
     *      - Bit 3: failsafe activated (0x08)
     *  - Byte[24]: SBUS footer
     */
    std::vector<uint8_t> data = {/*  0 */ 0x0F,
                                 /*  1 */ (uint8_t) (1 << 0),
                                 /*  2 */ (uint8_t) (2 << 3 | 1 >> 8),
                                 /*  3 */ (uint8_t) (3 << 6 | 2 >> 5),
                                 /*  4 */ (uint8_t) (3 >> 2),
                                 /*  5 */ (uint8_t) (4 << 1 | 3 >> 10),
                                 /*  6 */ (uint8_t) (5 << 4 | 4 >> 7),
                                 /*  7 */ (uint8_t) (6 << 7 | 5 >> 4),
                                 /*  8 */ (uint8_t) (6 >> 1),
                                 /*  9 */ (uint8_t) (7 << 2 | 6 >> 9),
                                 /* 10 */ (uint8_t) (8 << 5 | 7 >> 6),
                                 /* 11 */ (uint8_t) (8 >> 3),
                                 /* 12 */ (uint8_t) (9 << 0),
                                 /* 13 */ (uint8_t) (10 << 3 | 9 >> 8),
                                 /* 14 */ (uint8_t) (11 << 6 | 10 >> 5),
                                 /* 15 */ (uint8_t) (11 >> 2),
                                 /* 16 */ (uint8_t) (12 << 1 | 11 >> 10),
                                 /* 17 */ (uint8_t) (13 << 4 | 12 >> 7),
                                 /* 18 */ (uint8_t) (14 << 7 | 13 >> 4),
                                 /* 19 */ (uint8_t) (14 >> 1),
                                 /* 20 */ (uint8_t) (15 << 2 | 14 >> 9),
                                 /* 21 */ (uint8_t) (16 << 5 | 15 >> 6),
                                 /* 22 */ (uint8_t) (16 >> 3),
                                 /* 23 */ 0b00,
                                 /* 24 */ 0x00};
    std::size_t count = 0;
    ringBufferHandle.overrideFunc<ring_buffer_get>([&count, data](ring_buffer_data_t *ringBufferData, uint8_t *out) {
        EXPECT_NE(ringBufferData, nullptr);
        *out = data[count];
        count += 1;
        return count <= data.size();
    });

    sbus_init();
    EXPECT_TRUE(uartHandle.functionGotCalled<uart_init>());

    EXPECT_TRUE(sbus_data_available());
    EXPECT_FALSE(sbus_get_latest_data().failsafe);
    EXPECT_FALSE(sbus_get_latest_data().frame_lost);

    for (auto channel = 0U; channel < 16U; ++channel) {
        EXPECT_EQ(sbus_get_latest_data().channel[channel], channel + 1);
    }
}

TEST(TEST_NAME, decode_failsave) {
    auto uartHandle = mock::uart.getHandle();
    auto ringBufferHandle = mock::ring_buffer.getHandle();
    ringBufferHandle.overrideFunc<ring_buffer_init>([]() { return ring_buffer_data_t{}; });

    std::vector<uint8_t> data(25);
    data[0] = 0x0F;
    data[23] = 0x08;
    data[24] = 0;

    std::size_t count = 0;
    ringBufferHandle.overrideFunc<ring_buffer_get>([&count, data](ring_buffer_data_t *ringBufferData, uint8_t *out) {
        EXPECT_NE(ringBufferData, nullptr);
        *out = data[count];
        count += 1;
        return count <= data.size();
    });

    sbus_init();
    EXPECT_TRUE(uartHandle.functionGotCalled<uart_init>());

    EXPECT_TRUE(sbus_data_available());
    EXPECT_TRUE(sbus_get_latest_data().failsafe);
    EXPECT_FALSE(sbus_get_latest_data().frame_lost);

    for (const uint16_t channel : sbus_get_latest_data().channel) {
        EXPECT_EQ(channel, 0);
    }
}

TEST(TEST_NAME, decode_framelost) {
    auto uartHandle = mock::uart.getHandle();
    auto ringBufferHandle = mock::ring_buffer.getHandle();
    ringBufferHandle.overrideFunc<ring_buffer_init>([]() { return ring_buffer_data_t{}; });

    std::vector<uint8_t> data(25);
    data[0] = 0x0F;
    data[23] = 0x04;
    data[24] = 0;

    std::size_t count = 0;
    ringBufferHandle.overrideFunc<ring_buffer_get>([&count, data](ring_buffer_data_t *ringBufferData, uint8_t *out) {
        EXPECT_NE(ringBufferData, nullptr);
        *out = data[count];
        count += 1;
        return count <= data.size();
    });
    uart_callback_t uartCallback = nullptr;
    uartHandle.overrideFunc<uart_init>([&uartCallback](uint8_t /*id*/, uint32_t /*baud*/, uart_parity_t /*parity*/,
                                                        uint8_t /*stop_bits*/,
                                                        uart_callback_t rx_callback) { uartCallback = rx_callback; });
    sbus_init();
    EXPECT_TRUE(uartHandle.functionGotCalled<uart_init>());

    EXPECT_TRUE(sbus_data_available());
    EXPECT_FALSE(sbus_get_latest_data().failsafe);
    EXPECT_TRUE(sbus_get_latest_data().frame_lost);

    for (const uint16_t channel : sbus_get_latest_data().channel) {
        EXPECT_EQ(channel, 0);
    }
}
