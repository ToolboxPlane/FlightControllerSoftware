#include <Mock/ring_buffer.hpp>
#include <Mock/uart.hpp>
#include <gtest/gtest.h>

extern "C" {
#include <Drivers/sbus.h>
}

TEST(TEST_NAME, init) {
    auto uartHandle = mock::uart.getHandle();
    auto ringBufferHandle = mock::ring_buffer.getHandle();
    ringBufferHandle.overrideFunc<ring_buffer_init>([]() { return ring_buffer_data_t{}; });
    sbus_init();

    EXPECT_TRUE(uartHandle.functionGotCalled<uart_init>(2, 100000U, EVEN, 2, std::ignore));
    EXPECT_TRUE(ringBufferHandle.functionGotCalled<ring_buffer_init>());
    EXPECT_TRUE(sbus_get_latest_data().frame_lost);
    EXPECT_TRUE(sbus_get_latest_data().failsafe);
}

TEST(TEST_NAME, init__callback) {
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

TEST(TEST_NAME, data_available__no_data) {
    auto uartHandle = mock::uart.getHandle();
    auto ringBufferHandle = mock::ring_buffer.getHandle();
    ringBufferHandle.overrideFunc<ring_buffer_init>([]() { return ring_buffer_data_t{}; });

    ringBufferHandle.overrideFunc<ring_buffer_get>([](ring_buffer_data_t *ringBufferData, uint8_t * /*out*/) {
        EXPECT_NE(ringBufferData, nullptr);
        return false;
    });

    sbus_init();
    EXPECT_TRUE(uartHandle.functionGotCalled<uart_init>());

    EXPECT_FALSE(sbus_data_available());
    EXPECT_TRUE(sbus_get_latest_data().failsafe);
    EXPECT_TRUE(sbus_get_latest_data().frame_lost);
}

TEST(TEST_NAME, data_available__till_init) {
    auto uartHandle = mock::uart.getHandle();
    auto ringBufferHandle = mock::ring_buffer.getHandle();
    ringBufferHandle.overrideFunc<ring_buffer_init>([]() { return ring_buffer_data_t{}; });

    std::size_t count = 0;
    ringBufferHandle.overrideFunc<ring_buffer_get>([&count](ring_buffer_data_t *ringBufferData, uint8_t *out) {
        EXPECT_NE(ringBufferData, nullptr);
        if (count == 0) {
            *out = 0x0F; // Start byte
        }
        count += 1;
        return count <= 1;
    });

    sbus_init();
    EXPECT_TRUE(uartHandle.functionGotCalled<uart_init>());

    EXPECT_FALSE(sbus_data_available());
    EXPECT_TRUE(sbus_get_latest_data().failsafe);
    EXPECT_TRUE(sbus_get_latest_data().frame_lost);
}

TEST(TEST_NAME, data_available__till_data_0) {
    auto uartHandle = mock::uart.getHandle();
    auto ringBufferHandle = mock::ring_buffer.getHandle();
    ringBufferHandle.overrideFunc<ring_buffer_init>([]() { return ring_buffer_data_t{}; });

    std::size_t count = 0;
    ringBufferHandle.overrideFunc<ring_buffer_get>([&count](ring_buffer_data_t *ringBufferData, uint8_t *out) {
        EXPECT_NE(ringBufferData, nullptr);
        if (count == 0) {
            *out = 0x0F; // Start byte
        } else {         // First data byte
            *out = 0x00;
        }
        count += 1;
        return count <= 2;
    });

    sbus_init();
    EXPECT_TRUE(uartHandle.functionGotCalled<uart_init>());

    EXPECT_FALSE(sbus_data_available());
    EXPECT_TRUE(sbus_get_latest_data().failsafe);
    EXPECT_TRUE(sbus_get_latest_data().frame_lost);
}

TEST(TEST_NAME, data_available__till_data_22) {
    auto uartHandle = mock::uart.getHandle();
    auto ringBufferHandle = mock::ring_buffer.getHandle();
    ringBufferHandle.overrideFunc<ring_buffer_init>([]() { return ring_buffer_data_t{}; });

    std::size_t count = 0;
    ringBufferHandle.overrideFunc<ring_buffer_get>([&count](ring_buffer_data_t *ringBufferData, uint8_t *out) {
        EXPECT_NE(ringBufferData, nullptr);
        if (count == 0) {
            *out = 0x0F;          // Start byte
        } else if (count == 22) { // Last data byte
            *out = 0x00;
        } else { // Data bytes
            *out = 0x00;
        }
        count += 1;
        return count <= 23;
    });

    sbus_init();
    EXPECT_TRUE(uartHandle.functionGotCalled<uart_init>());

    EXPECT_FALSE(sbus_data_available());
    EXPECT_TRUE(sbus_get_latest_data().failsafe);
    EXPECT_TRUE(sbus_get_latest_data().frame_lost);
}

TEST(TEST_NAME, data_available__till_flags) {
    auto uartHandle = mock::uart.getHandle();
    auto ringBufferHandle = mock::ring_buffer.getHandle();
    ringBufferHandle.overrideFunc<ring_buffer_init>([]() { return ring_buffer_data_t{}; });

    std::size_t count = 0;
    ringBufferHandle.overrideFunc<ring_buffer_get>([&count](ring_buffer_data_t *ringBufferData, uint8_t *out) {
        EXPECT_NE(ringBufferData, nullptr);
        if (count == 0) {
            *out = 0x0F; // Start byte
        } else if (count == 23) {
            *out = 0x08; // Flags
        } else {         // Data bytes
            *out = 0x00;
        }
        count += 1;
        return count <= 24;
    });

    sbus_init();
    EXPECT_TRUE(uartHandle.functionGotCalled<uart_init>());

    EXPECT_FALSE(sbus_data_available());
    EXPECT_TRUE(sbus_get_latest_data().failsafe);
    EXPECT_TRUE(sbus_get_latest_data().frame_lost);
}

TEST(TEST_NAME, data_available__no_end) {
    auto uartHandle = mock::uart.getHandle();
    auto ringBufferHandle = mock::ring_buffer.getHandle();
    ringBufferHandle.overrideFunc<ring_buffer_init>([]() { return ring_buffer_data_t{}; });

    std::size_t count = 0;
    ringBufferHandle.overrideFunc<ring_buffer_get>([&count](ring_buffer_data_t *ringBufferData, uint8_t *out) {
        EXPECT_NE(ringBufferData, nullptr);
        if (count == 0) {
            *out = 0x0F; // Start byte
        } else if (count == 23) {
            *out = 0x08;          // Flags
        } else if (count == 24) { // End byte
            *out = 0xFF;          // Invalid end byte
        } else {                  // Data bytes
            *out = 0x00;
        }
        count += 1;
        return count <= 25;
    });

    sbus_init();
    EXPECT_TRUE(uartHandle.functionGotCalled<uart_init>());

    // TODO check warning

    EXPECT_FALSE(sbus_data_available());
    EXPECT_TRUE(sbus_get_latest_data().failsafe);
    EXPECT_TRUE(sbus_get_latest_data().frame_lost);
}

TEST(TEST_NAME, data_available__multiple_init) {
    auto uartHandle = mock::uart.getHandle();
    auto ringBufferHandle = mock::ring_buffer.getHandle();
    ringBufferHandle.overrideFunc<ring_buffer_init>([]() { return ring_buffer_data_t{}; });

    std::size_t count = 0;
    ringBufferHandle.overrideFunc<ring_buffer_get>([&count](ring_buffer_data_t *ringBufferData, uint8_t *out) {
        EXPECT_NE(ringBufferData, nullptr);
        if (count == 0 + 5) {
            *out = 0x0F; // Start byte
        } else if (count == 23 + 5) {
            *out = 0x00;              // Flags
        } else if (count == 24 + 5) { // End byte
            *out = 0x00;
        } else { // Data bytes
            *out = 0x00;
        }
        count += 1;
        return count <= 25 + 5;
    });

    sbus_init();
    EXPECT_TRUE(uartHandle.functionGotCalled<uart_init>());

    EXPECT_TRUE(sbus_data_available());
    EXPECT_FALSE(sbus_get_latest_data().failsafe);
    EXPECT_FALSE(sbus_get_latest_data().frame_lost);

    for (const uint16_t channel : sbus_get_latest_data().channel) {
        EXPECT_EQ(channel, 0);
    }
}

TEST(TEST_NAME, data_available__multiple_data) {
    auto uartHandle = mock::uart.getHandle();
    auto ringBufferHandle = mock::ring_buffer.getHandle();
    ringBufferHandle.overrideFunc<ring_buffer_init>([]() { return ring_buffer_data_t{}; });

    std::size_t count = 0;
    ringBufferHandle.overrideFunc<ring_buffer_get>([&count](ring_buffer_data_t *ringBufferData, uint8_t *out) {
        EXPECT_NE(ringBufferData, nullptr);
        if (count % 25 == 0) {
            *out = 0x0F; // Start byte
        } else if (count % 25 == 23) {
            *out = 0x00;               // Flags
        } else if (count % 25 == 24) { // End byte
            *out = 0x00;
        } else { // Data bytes
            if (count < 25) {
                *out = 0x00;
            } else {
                *out = 0xFF;
            }
        }
        count += 1;
        return count <= 50;
    });

    sbus_init();
    EXPECT_TRUE(uartHandle.functionGotCalled<uart_init>());

    EXPECT_TRUE(sbus_data_available());
    EXPECT_FALSE(sbus_get_latest_data().failsafe);
    EXPECT_FALSE(sbus_get_latest_data().frame_lost);

    for (const uint16_t channel : sbus_get_latest_data().channel) {
        EXPECT_EQ(channel, 2047);
    }
}

TEST(TEST_NAME, data_available__decode_result) {
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
    const std::vector<uint8_t> data = {/*  0 */ 0x0F,
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

TEST(TEST_NAME, data_available__decode_result_failsave) {
    auto uartHandle = mock::uart.getHandle();
    auto ringBufferHandle = mock::ring_buffer.getHandle();
    ringBufferHandle.overrideFunc<ring_buffer_init>([]() { return ring_buffer_data_t{}; });

    std::size_t count = 0;
    ringBufferHandle.overrideFunc<ring_buffer_get>([&count](ring_buffer_data_t *ringBufferData, uint8_t *out) {
        EXPECT_NE(ringBufferData, nullptr);
        if (count == 0) {
            *out = 0x0F; // Start byte
        } else if (count == 23) {
            *out = 0x08;          // Flags
        } else if (count == 24) { // End byte
            *out = 0x00;
        } else { // Data bytes
            *out = 0x00;
        }
        count += 1;
        return count <= 25;
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

TEST(TEST_NAME, data_available__decode_result_framelost) {
    auto uartHandle = mock::uart.getHandle();
    auto ringBufferHandle = mock::ring_buffer.getHandle();
    ringBufferHandle.overrideFunc<ring_buffer_init>([]() { return ring_buffer_data_t{}; });

    std::size_t count = 0;
    ringBufferHandle.overrideFunc<ring_buffer_get>([&count](ring_buffer_data_t *ringBufferData, uint8_t *out) {
        EXPECT_NE(ringBufferData, nullptr);
        if (count == 0) {
            *out = 0x0F; // Start byte
        } else if (count == 23) {
            *out = 0x04;          // Flags
        } else if (count == 24) { // End byte
            *out = 0x00;
        } else { // Data bytes
            *out = 0x00;
        }
        count += 1;
        return count <= 25;
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
