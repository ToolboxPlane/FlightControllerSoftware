#include <gtest/gtest.h>

extern "C" {
#include <Messages/MessageEncoding.h>
}

#include "FlightControllerSetpoint.pb.h"

TEST(TEST_NAME, encode) {
    std::array<uint8_t, ToolboxPlaneMessages_FlightControllerSetpoint_size + 3> buf;
    ToolboxPlaneMessages_FlightControllerSetpoint data{.motor = 0, .pitch = 0, .roll = 0};
    EXPECT_EQ(message_encode(buf.data(), buf.size(), ToolboxPlaneMessages_FlightControllerSetpoint_fields, &data, 17),
              3);
    EXPECT_EQ(buf[0], 0x0F);
    EXPECT_EQ(buf[1], 17);
    EXPECT_EQ(buf[2], 0xF0);
}

// TODO actual message encoding, decoding tests
