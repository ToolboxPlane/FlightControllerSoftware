#include <fstream>
#include <gtest/gtest.h>

extern "C" {
#include <Messages/MessageEncoding.h>
}

#include "FlightControllerSetpoint.pb.h"

TEST(TEST_NAME, encode__buffer_format) {
    /*
     * Encode message with all zero data and id 17, expect:
     *  * Returned length: 3
     *  * First byte: 0x0F
     *  * Second byte: 17
     *  * Third byte: 0xF0
     */
    std::array<uint8_t, ToolboxPlaneMessages_FlightControllerSetpoint_size + 3> buf{};
    ToolboxPlaneMessages_FlightControllerSetpoint data{.motor = 0, .pitch = 0, .roll = 0};
    EXPECT_EQ(message_encode(buf.data(), buf.size(), ToolboxPlaneMessages_FlightControllerSetpoint_fields, &data, 17),
              3);
    EXPECT_EQ(buf[0], 0x0F);
    EXPECT_EQ(buf[1], 17);
    EXPECT_EQ(buf[2], 0xF0);
}

// TODO actual message encoding, decoding tests
