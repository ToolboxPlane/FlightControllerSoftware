#include <Mock/HAL/uart.hpp>
#include <Mock/Messages/MessageDecoding.hpp>
#include <Mock/Messages/MessageEncoding.hpp>
#include <gtest/gtest.h>
#include <queue>

extern "C" {
#include <Drivers/protobuf.h>
}

TEST(TEST_NAME, init) {
    auto uartHandle = mock::uart.getHandle();

    protobuf_init();

    EXPECT_TRUE(uartHandle.functionGotCalled<uart_init>(0, 115200U, ODD, 1, std::ignore));
    EXPECT_FALSE(protobuf_setpoint_available());
}

TEST(TEST_NAME, send_fc) {
    auto uartHandle = mock::uart.getHandle();
    auto encodingHandle = mock::MessageEncoding.getHandle();

    protobuf_init();

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

    protobuf_send_fc(&to_send);

    EXPECT_TRUE(encodingHandle.functionGotCalled<message_encode>());
    EXPECT_TRUE(uartHandle.functionGotCalled<uart_send_buf>());
}

TEST(TEST_NAME, receive_singler_buffer) {
    auto uartHandle = mock::uart.getHandle();
    auto decodeHandle = mock::MessageDecoding.getHandle();
    uart_callback_t uartCallback;
    uartHandle.overrideFunc<uart_init>([&uartCallback](uint8_t /*id*/, uint16_t /*baud*/, uart_parity_t /*parity*/,
                                                       uint8_t /*stop_bits*/,
                                                       uart_callback_t callback) { uartCallback = callback; });
    decodeHandle.overrideFunc<message_decode>([](message_decoding_data_t * /*messageDecodingData*/, uint8_t /*data*/,
                                                 pb_istream_s * /*istream*/) -> bool { return false; });

    protobuf_init();

    uint8_t bufData;

    for (auto c = 0; c < 10; ++c) {
        bufData = c * 11;
        uartCallback(bufData);
        protobuf_setpoint_available();
        EXPECT_TRUE(decodeHandle.functionGotCalled<message_decode>(std::ignore, bufData, std::ignore));
    }
}

TEST(TEST_NAME, receive_200bytes) {
    auto uartHandle = mock::uart.getHandle();
    auto decodeHandle = mock::MessageDecoding.getHandle();
    uart_callback_t uartCallback;
    uartHandle.overrideFunc<uart_init>([&uartCallback](uint8_t /*id*/, uint16_t /*baud*/, uart_parity_t /*parity*/,
                                                       uint8_t /*stop_bits*/,
                                                       uart_callback_t callback) { uartCallback = callback; });
    int receiveIndex = 10U;
    decodeHandle.overrideFunc<message_decode>([&receiveIndex](message_decoding_data_t * /*messageDecodingData*/,
                                                              uint8_t data, pb_istream_s * /*istream*/) -> bool {
        EXPECT_EQ(receiveIndex, data);
        receiveIndex += 1;
        return false;
    });

    protobuf_init();

    for (auto c = 10U; c < 210U; ++c) {
        uartCallback(c);
    }
    protobuf_setpoint_available();
    EXPECT_EQ(receiveIndex, 210U);


    for (auto c = 20U; c < 220U; ++c) {
        uartCallback(c);
    }
    receiveIndex = 20U;
    protobuf_setpoint_available();
    EXPECT_EQ(receiveIndex, 220U);
}

TEST(TEST_NAME, receive_pb_decode) {
    auto uartHandle = mock::uart.getHandle();
    auto decodeHandle = mock::MessageDecoding.getHandle();
    uart_callback_t uartCallback;
    uartHandle.overrideFunc<uart_init>([&uartCallback](uint8_t /*id*/, uint16_t /*baud*/, uart_parity_t /*parity*/,
                                                       uint8_t /*stop_bits*/,
                                                       uart_callback_t callback) { uartCallback = callback; });

    protobuf_init();

    // echo "power: 1337, pitch: 17, roll: 34 | protoc
    // --encode=ToolboxPlaneMessages.FlightControllerSetpoint Src/Messages/Definitions/FlightControllerSetpoint.proto
    // | xxd
    std::vector<uint8_t> callbackData{{0x08, 0xb9, 0x0a, 0x10, 0x11, 0x18, 0x22}};
    std::size_t dataSendIndex = 0;
    std::size_t dataDecodeIndex = 0;

    decodeHandle.overrideFunc<message_decode>(
            [&callbackData, &dataDecodeIndex](message_decoding_data_t *messageDecodingData, uint8_t data,
                                              pb_istream_s *istream) -> bool {
                EXPECT_EQ(callbackData[dataDecodeIndex], data);
                EXPECT_NE(messageDecodingData, nullptr);
                EXPECT_NE(istream, nullptr);
                dataDecodeIndex += 1;
                if (dataDecodeIndex == callbackData.size()) {
                    *istream = pb_istream_from_buffer(callbackData.data(), callbackData.size());
                    return true;
                }
                return false;
            });

    for (; dataSendIndex < callbackData.size(); ++dataSendIndex) {
        EXPECT_FALSE(protobuf_setpoint_available());
        uartCallback(callbackData[dataSendIndex]);
        if (dataSendIndex == callbackData.size() - 1) {
            EXPECT_TRUE(protobuf_setpoint_available());

            auto setpoint = protobuf_get_setpoint();
            EXPECT_EQ(setpoint.motor, 1337);
            EXPECT_EQ(setpoint.pitch, 17);
            EXPECT_EQ(setpoint.roll, 34);
        } else {
            EXPECT_FALSE(protobuf_setpoint_available());
        }
    }
}
