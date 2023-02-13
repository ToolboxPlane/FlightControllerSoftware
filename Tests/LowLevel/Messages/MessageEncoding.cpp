#include <fstream>
#include <gtest/gtest.h>

extern "C" {
#include <Messages/MessageEncoding.h>
}

#include "FlightController.pb.h"

TEST(TEST_NAME, encode__buffer_format) {
    /*
     * Encode message with all zero data and id 17, expect:
     *  * Returned length: 3
     *  * First byte: 0x0F
     *  * Second byte: 17
     *  * Third byte: 0xF0
     */
    std::array<uint8_t, ToolboxPlaneMessages_FlightController_size + 3> buf{};
    ToolboxPlaneMessages_FlightController data{};
    EXPECT_EQ(message_encode(buf.data(), buf.size(), ToolboxPlaneMessages_FlightController_fields, &data, 17), 3);
    EXPECT_EQ(buf[0], 0x0F);
    EXPECT_EQ(buf[1], 17);
    EXPECT_EQ(buf[2], 0xF0);
}

TEST(TEST_NAME, encode__fc_encode) {
    /*
     * Encode message with all zero data and id 17, expect:
     *  * Returned length: 3
     *  * First byte: 0x0F
     *  * Second byte: 17
     *  * Third byte: 0xF0
     */
    ToolboxPlaneMessages_FlightController data{.has_imu = true,
                                               .imu = {.imu_ok = true,
                                                       .roll_mul_16 = 17,
                                                       .pitch_mul_16 = -18,
                                                       .yaw_mul_16 = 19,
                                                       .dRoll_mul_16 = -20,
                                                       .dPitch_mul_16 = 21,
                                                       .dYaw_mul_16 = -22,
                                                       .accX_mul_100 = 23,
                                                       .accY_mul_100 = -24,
                                                       .accZ_mul_100 = 25},
                                               .has_remote = true,
                                               .remote =
                                                       {
                                                               .remote_ok = false,
                                                               .throttleMixed = -26,
                                                               .elevonLeftMixed = 27,
                                                               .elevonRightMixed = -28,
                                                               .isArmed = true,
                                                               .overrideActive = false,
                                                       },
                                               .motor = 29,
                                               .servoLeft = 30,
                                               .servoRight = 31};

    std::ifstream stream{"fc_encode.bin"};
    using c_stream_it = std::istreambuf_iterator<char>;
    std::vector<uint8_t> expectedData;
    expectedData.emplace_back(0x0F);
    expectedData.emplace_back(17);
    for (auto it = c_stream_it{stream}; it != c_stream_it{}; ++it) {
        expectedData.emplace_back(*it);
    }
    expectedData.emplace_back(0xF0);

    EXPECT_LT(expectedData.size(), ToolboxPlaneMessages_FlightController_size + 3);
    std::array<uint8_t, ToolboxPlaneMessages_FlightController_size + 3> buf{};
    EXPECT_EQ(message_encode(buf.data(), buf.size(), ToolboxPlaneMessages_FlightController_fields, &data, 17),
              expectedData.size());
    for (auto idx = 0U; idx < expectedData.size(); ++idx) {
        EXPECT_EQ(buf[idx], expectedData[idx]);
    }
}
