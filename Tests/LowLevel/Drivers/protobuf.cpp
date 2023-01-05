#include <Mock/MessageDecoding.hpp>
#include <Mock/MessageEncoding.hpp>
#include <Mock/ring_buffer.hpp>
#include <Mock/uart.hpp>
#include <gtest/gtest.h>
#include <queue>

extern "C" {
#include <Drivers/protobuf.h>
}

TEST(TEST_NAME, init) {
    auto uartHandle = mock::uart.getHandle();
    auto decodeHandle = mock::MessageDecoding.getHandle();
    auto ringBufferHandle = mock::ring_buffer.getHandle();
    ringBufferHandle.overrideFunc<ring_buffer_init>([]() { return ring_buffer_data_t{}; });

    protobuf_init();

    EXPECT_TRUE(uartHandle.functionGotCalled<uart_init>(0, 115200U, NONE, 1, std::ignore));
    EXPECT_TRUE(ringBufferHandle.functionGotCalled<ring_buffer_init>());
    EXPECT_TRUE(decodeHandle.functionGotCalled<message_decoding_init>(std::ignore, 0x40));
}

TEST(TEST_NAME, send_fc) {
    auto uartHandle = mock::uart.getHandle();
    auto encodingHandle = mock::MessageEncoding.getHandle();
    auto decodeHandle = mock::MessageDecoding.getHandle();
    auto ringBufferHandle = mock::ring_buffer.getHandle();
    ringBufferHandle.overrideFunc<ring_buffer_init>([]() { return ring_buffer_data_t{}; });

    protobuf_init();
    EXPECT_TRUE(uartHandle.functionGotCalled<uart_init>());
    EXPECT_TRUE(decodeHandle.functionGotCalled<message_decoding_init>());

    fc_message_t to_send{};

    encodingHandle.overrideFunc<message_encode>(
            [&to_send](uint8_t *buf, uint16_t size, const pb_msgdesc_t *fields, const void *message, uint8_t id) {
                EXPECT_GE(size, ToolboxPlaneMessages_FlightController_size + 3);
                EXPECT_FALSE(buf == nullptr);
                EXPECT_EQ(fields, &ToolboxPlaneMessages_FlightController_msg);
                EXPECT_EQ(message, &to_send);
                EXPECT_EQ(id, 0x10);

                buf[0] = 201;
                buf[1] = 202;
                buf[2] = 203;
                buf[3] = 204;
                buf[4] = 205;
                return 5;
            });

    uartHandle.overrideFunc<uart_send_buf>([](uint8_t id, const uint8_t *data, uint16_t size) {
        EXPECT_EQ(id, 0);
        EXPECT_EQ(size, 5);
        EXPECT_EQ(data[0], 201);
        EXPECT_EQ(data[1], 202);
        EXPECT_EQ(data[2], 203);
        EXPECT_EQ(data[3], 204);
        EXPECT_EQ(data[4], 205);
    });

    protobuf_send(&to_send);

    EXPECT_TRUE(encodingHandle.functionGotCalled<message_encode>());
    EXPECT_TRUE(uartHandle.functionGotCalled<uart_send_buf>());
}

TEST(TEST_NAME, rx_fill_buffer) {
    auto uartHandle = mock::uart.getHandle();
    auto decodeHandle = mock::MessageDecoding.getHandle();
    auto ringBufferHandle = mock::ring_buffer.getHandle();
    ringBufferHandle.overrideFunc<ring_buffer_init>([]() { return ring_buffer_data_t{}; });
    ringBufferHandle.overrideFunc<ring_buffer_put>(
            [](ring_buffer_data_t * /*ring_buffer_data*/, uint8_t /*data*/) { return true; });

    uart_callback_t uartCallback = nullptr;
    uartHandle.overrideFunc<uart_init>([&uartCallback](uint8_t /*id*/, uint16_t /*baud*/, uart_parity_t /*parity*/,
                                                       uint8_t /*stop_bits*/,
                                                       uart_callback_t callback) { uartCallback = callback; });
    protobuf_init();
    EXPECT_TRUE(decodeHandle.functionGotCalled<message_decoding_init>());

    uartCallback(17);
    EXPECT_TRUE(ringBufferHandle.functionGotCalled<ring_buffer_put>(std::ignore, 17));

    uartCallback(38);
    EXPECT_TRUE(ringBufferHandle.functionGotCalled<ring_buffer_put>(std::ignore, 38));

    uartCallback(54);
    EXPECT_TRUE(ringBufferHandle.functionGotCalled<ring_buffer_put>(std::ignore, 54));
}

TEST(TEST_NAME, available_read_buffer) {
    auto uartHandle = mock::uart.getHandle();
    auto decodeHandle = mock::MessageDecoding.getHandle();
    auto ringBufferHandle = mock::ring_buffer.getHandle();
    ringBufferHandle.overrideFunc<ring_buffer_init>([]() { return ring_buffer_data_t{}; });
    protobuf_init();
    EXPECT_TRUE(uartHandle.functionGotCalled<uart_init>());
    EXPECT_TRUE(decodeHandle.functionGotCalled<message_decoding_init>());

    std::size_t count = 0;
    ringBufferHandle.overrideFunc<ring_buffer_get>([&count](ring_buffer_data_t *ringBufferData, uint8_t *out) {
        EXPECT_NE(ringBufferData, nullptr);
        count += 1;
        *out = count;
        return count <= 4;
    });
    decodeHandle.overrideFunc<message_decoding_decode>([](message_decoding_data_t * /*messageDecodingData*/,
                                                          uint8_t /*data*/, const pb_msgdesc_t * /*fields*/,
                                                          void * /*message*/) -> bool { return false; });

    protobuf_available();
    EXPECT_TRUE(decodeHandle.functionGotCalled<message_decoding_decode>(
            std::ignore, 1, &ToolboxPlaneMessages_FlightControllerSetpoint_msg, std::ignore));
    EXPECT_TRUE(decodeHandle.functionGotCalled<message_decoding_decode>(
            std::ignore, 2, &ToolboxPlaneMessages_FlightControllerSetpoint_msg, std::ignore));
    EXPECT_TRUE(decodeHandle.functionGotCalled<message_decoding_decode>(
            std::ignore, 3, &ToolboxPlaneMessages_FlightControllerSetpoint_msg, std::ignore));
    EXPECT_TRUE(decodeHandle.functionGotCalled<message_decoding_decode>(
            std::ignore, 4, &ToolboxPlaneMessages_FlightControllerSetpoint_msg, std::ignore));
}

TEST(TEST_NAME, receive_pb_no_decode) {
    auto uartHandle = mock::uart.getHandle();
    auto decodeHandle = mock::MessageDecoding.getHandle();
    auto ringBufferHandle = mock::ring_buffer.getHandle();
    ringBufferHandle.overrideFunc<ring_buffer_init>([]() { return ring_buffer_data_t{}; });

    protobuf_init();
    EXPECT_TRUE(uartHandle.functionGotCalled<uart_init>());
    EXPECT_TRUE(decodeHandle.functionGotCalled<message_decoding_init>());

    std::size_t count = 0;
    ringBufferHandle.overrideFunc<ring_buffer_get>(
            [&count](ring_buffer_data_t * /*ringBufferData*/, uint8_t * /*out*/) {
                count += 1;
                return count == 1;
            });

    decodeHandle.overrideFunc<message_decoding_decode>([](message_decoding_data_t * /*messageDecodingData*/,
                                                          uint8_t /*data*/, const pb_msgdesc_t * /*fields*/,
                                                          void * /*message*/) -> bool { return false; });

    EXPECT_FALSE(protobuf_available());
}

TEST(TEST_NAME, receive_pb_yes_decode) {
    auto uartHandle = mock::uart.getHandle();
    auto decodeHandle = mock::MessageDecoding.getHandle();
    auto ringBufferHandle = mock::ring_buffer.getHandle();
    ringBufferHandle.overrideFunc<ring_buffer_init>([]() { return ring_buffer_data_t{}; });

    protobuf_init();
    EXPECT_TRUE(uartHandle.functionGotCalled<uart_init>());
    EXPECT_TRUE(decodeHandle.functionGotCalled<message_decoding_init>());

    std::size_t count = 0;
    ringBufferHandle.overrideFunc<ring_buffer_get>(
            [&count](ring_buffer_data_t * /*ringBufferData*/, uint8_t * /*out*/) {
                count += 1;
                return count == 1;
            });

    decodeHandle.overrideFunc<message_decoding_decode>([](message_decoding_data_t * /*messageDecodingData*/,
                                                          uint8_t /*data*/, const pb_msgdesc_t * /*fields*/,
                                                          void * /*message*/) -> bool { return true; });

    EXPECT_TRUE(protobuf_available());
}

TEST(TEST_NAME, receive_pb_decode_data) {
    auto uartHandle = mock::uart.getHandle();
    auto decodeHandle = mock::MessageDecoding.getHandle();
    auto ringBufferHandle = mock::ring_buffer.getHandle();
    ringBufferHandle.overrideFunc<ring_buffer_init>([]() { return ring_buffer_data_t{}; });

    protobuf_init();
    EXPECT_TRUE(uartHandle.functionGotCalled<uart_init>());
    EXPECT_TRUE(decodeHandle.functionGotCalled<message_decoding_init>());

    std::size_t count = 0;
    ringBufferHandle.overrideFunc<ring_buffer_get>(
            [&count](ring_buffer_data_t * /*ringBufferData*/, uint8_t * /*out*/) {
                count += 1;
                return count == 1;
            });

    decodeHandle.overrideFunc<message_decoding_decode>([](message_decoding_data_t * /*messageDecodingData*/,
                                                          uint8_t /*data*/, const pb_msgdesc_t * /*fields*/,
                                                          void *message) -> bool {
        auto fcData = static_cast<ToolboxPlaneMessages_FlightControllerSetpoint *>(message);
        fcData->motor = 1337;
        fcData->pitch = 17;
        fcData->roll = 34;
        return true;
    });

    EXPECT_TRUE(protobuf_available());
    EXPECT_EQ(protobuf_get().motor, 1337);
    EXPECT_EQ(protobuf_get().pitch, 17);
    EXPECT_EQ(protobuf_get().roll, 34);
}
