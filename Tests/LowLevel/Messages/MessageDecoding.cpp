#include <gtest/gtest.h>

extern "C" {
#include <Messages/MessageDecoding.h>
}

#include "FlightControllerSetpoint.pb.h"

TEST(TEST_NAME, decode__nominal_decode) {
    /*
     * Nominal decode test.
     * Test:
     *  * Initialize with message_id as 17
     *  * Feed 0xF0 to transition to INITIAL_END_FOUND, expect false
     *  * Feed 0x0F to transition to START_FOUND, expect false
     *  * Feed 17 to transition to IN_DATA, expect false
     *  * Feed 0xF0 to transition to IN_DATA_END_FOUND, expect false
     *  * Feed 0x0F to transition to START_FOUND, expect true
     */
    message_decoding_data_t decodingData;
    message_decoding_init(&decodingData, 17, ToolboxPlaneMessages_FlightControllerSetpoint_fields);

    EXPECT_FALSE(message_decoding_decode(&decodingData, 0xF0, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0x0F, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 17, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0xF0, nullptr));
    ToolboxPlaneMessages_FlightControllerSetpoint buf;
    EXPECT_TRUE(message_decoding_decode(&decodingData, 0x0F, &buf));
}

TEST(TEST_NAME, decode__premature_end) {
    /*
     * Premature end test.
     * Test:
     *  * Initialize with message_id as 17
     *  * Feed 0xF0 to transition to INITIAL_END_FOUND, expect false
     *  * Feed 0x0F to transition to START_FOUND, expect false
     *  * Feed 17 to transition to IN_DATA, expect false
     *  * Feed 0x08 to keep in IN_DATA, expect false
     *  * Feed 0xF0 to transition to IN_DATA_END_FOUND, expect false
     *  * Feed 0x01 to transition to IN_DATA, expect false
     *  * Feed 0xF0 to transition to IN_DATA_END_FOUND, expect false
     *  * Feed 0x0F to transition to START_FOUND, expect true
     */
    message_decoding_data_t decodingData;
    message_decoding_init(&decodingData, 17, ToolboxPlaneMessages_FlightControllerSetpoint_fields);

    EXPECT_FALSE(message_decoding_decode(&decodingData, 0xF0, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0x0F, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 17, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0x08, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0xF0, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0x01, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0xF0, nullptr));
    ToolboxPlaneMessages_FlightControllerSetpoint buf;
    EXPECT_TRUE(message_decoding_decode(&decodingData, 0x0F, &buf));
}

TEST(TEST_NAME, decode__wrong_data) {
    /*
     * Wrong data test.
     * Test:
     *  * Initialize with message_id as 17
     *  * Feed 0xF0 to transition to INITIAL_END_FOUND, expect false
     *  * Feed 0x0F to transition to START_FOUND, expect false
     *  * Feed 18 to transition to IN_DATA, expect false
     *  * Feed 0xF0 to transition to IN_DATA_END_FOUND, expect false
     *  * Feed 0x0F to transition to START_FOUND, expect false
     *  * Feed 17 to transition to IN_DATA, expect false
     *  * Feed 0xF0 to transition to IN_DATA_END_FOUND, expect false
     *  * Feed 0x0F to transition to START_FOUND, expect true
     */
    message_decoding_data_t decodingData;
    message_decoding_init(&decodingData, 17, ToolboxPlaneMessages_FlightControllerSetpoint_fields);

    EXPECT_FALSE(message_decoding_decode(&decodingData, 0xF0, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0x0F, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 18, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0xF0, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0x0F, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 17, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0xF0, nullptr));
    ToolboxPlaneMessages_FlightControllerSetpoint buf;
    EXPECT_TRUE(message_decoding_decode(&decodingData, 0x0F, &buf));
}

TEST(TEST_NAME, decode__wrong_data_premature_end) {
    /*
     * Wrong Data Premature end test.
     * Test:
     *  * Initialize with message_id as 17
     *  * Feed 0xF0 to transition to INITIAL_END_FOUND, expect false
     *  * Feed 0x0F to transition to START_FOUND, expect false
     *  * Feed 18 to transition to IN_DATA, expect false
     *  * Feed 0x08 to keep in IN_DATA, expect false
     *  * Feed 0xF0 to transition to IN_DATA_END_FOUND, expect false
     *  * Feed 0x01 to transition to IN_DATA, expect false
     *  * Feed 0xF0 to transition to IN_DATA_END_FOUND, expect false
     *  * Feed 0x0F to transition to START_FOUND, expect false
     *  * Feed 17 to transition to IN_DATA, expect false
     *  * Feed 0xF0 to transition to IN_DATA_END_FOUND, expect false
     *  * Feed 0x0F to transition to START_FOUND, expect true
     */
    message_decoding_data_t decodingData;
    message_decoding_init(&decodingData, 17, ToolboxPlaneMessages_FlightControllerSetpoint_fields);

    EXPECT_FALSE(message_decoding_decode(&decodingData, 0xF0, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0x0F, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 18, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0x08, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0xF0, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0x01, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0xF0, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0x0F, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 17, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0xF0, nullptr));
    ToolboxPlaneMessages_FlightControllerSetpoint buf;
    EXPECT_TRUE(message_decoding_decode(&decodingData, 0x0F, &buf));
}

TEST(TEST_NAME, decode__multiple_init) {
    /*
     * Multiple init decode test.
     * Test:
     *  * Initialize with message_id as 17
     *  * Feed 0x00 to keep in initial
     *  * Feed 0xF0 to transition to INITIAL_END_FOUND, expect false
     *  * Feed 0x0F to transition to START_FOUND, expect false
     *  * Feed 17 to transition to IN_DATA, expect false
     *  * Feed 0xF0 to transition to IN_DATA_END_FOUND, expect false
     *  * Feed 0x0F to transition to START_FOUND, expect true
     */
    message_decoding_data_t decodingData;
    message_decoding_init(&decodingData, 17, ToolboxPlaneMessages_FlightControllerSetpoint_fields);

    EXPECT_FALSE(message_decoding_decode(&decodingData, 0x00, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0xF0, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0x0F, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 17, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0xF0, nullptr));
    ToolboxPlaneMessages_FlightControllerSetpoint buf;
    EXPECT_TRUE(message_decoding_decode(&decodingData, 0x0F, &buf));
}

TEST(TEST_NAME, decode__init_wrong_end) {
    /*
     * Init wrong end test.
     * Test:
     *  * Initialize with message_id as 17
     *  * Feed 0xF0 to transition to INITIAL_END_FOUND, expect false
     *  * Feed 0x00 to transition to INITIAL, expect false
     *  * Feed 0xF0 to transition to INITIAL_END_FOUND, expect false
     *  * Feed 0x0F to transition to START_FOUND, expect false
     *  * Feed 17 to transition to IN_DATA, expect false
     *  * Feed 0xF0 to transition to IN_DATA_END_FOUND, expect false
     *  * Feed 0x0F to transition to START_FOUND, expect true
     */
    message_decoding_data_t decodingData;
    message_decoding_init(&decodingData, 17, ToolboxPlaneMessages_FlightControllerSetpoint_fields);

    EXPECT_FALSE(message_decoding_decode(&decodingData, 0xF0, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0x00, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0xF0, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0x0F, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 17, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0xF0, nullptr));
    ToolboxPlaneMessages_FlightControllerSetpoint buf;
    EXPECT_TRUE(message_decoding_decode(&decodingData, 0x0F, &buf));
}

// echo "power: 1337, pitch: 17, roll: 34" | protoc --encode=ToolboxPlaneMessages.FlightControllerSetpoint
// Src/Messages/Definitions/FlightControllerSetpoint.proto | xxd


// @TODO test strategy for actual decoding
