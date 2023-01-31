#include <fstream>
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

TEST(TEST_NAME, decode__fcs_decode) {
    /*
     * Flight-Controller-Set-Point min power test:
     *  * Initialize with message_id as 0
     *  * Feed 0xF0 to transition to INITIAL_END_FOUND, expect false
     *  * Feed 0x0F to transition to START_FOUND, expect false
     *  * Feed 0 to transition to IN_DATA, expect false
     *  * Feed data from fcs_min_power.bin, expect false for every byte
     *  * Feed 0xF0 to transition to IN_DATA_END_FOUND, expect false
     *  * Feed 0x0F to transition to START_FOUND, expect true
     *  * Check return message, expect:
     *      * motor: -2147483648
     *      * pitch: 0
     *      * roll: 0
     */
    message_decoding_data_t decodingData;
    message_decoding_init(&decodingData, 0, ToolboxPlaneMessages_FlightControllerSetpoint_fields);

    EXPECT_FALSE(message_decoding_decode(&decodingData, 0xF0, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0x0F, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0, nullptr));

    std::ifstream stream{"fcs_decode.bin"};
    using c_stream_it = std::istreambuf_iterator<char>;

    for (auto it = c_stream_it{stream}; it != c_stream_it{}; ++it) {
        EXPECT_FALSE(message_decoding_decode(&decodingData, *it, nullptr));
    }

    EXPECT_FALSE(message_decoding_decode(&decodingData, 0xF0, nullptr));
    ToolboxPlaneMessages_FlightControllerSetpoint buf;
    EXPECT_TRUE(message_decoding_decode(&decodingData, 0x0F, &buf));

    EXPECT_EQ(buf.motor, 17);
    EXPECT_EQ(buf.pitch, -18);
    EXPECT_EQ(buf.roll, 19);
}

TEST(TEST_NAME, decode__fcs_min_power) {
    /*
     * Flight-Controller-Set-Point min power test:
     *  * Initialize with message_id as 0
     *  * Feed 0xF0 to transition to INITIAL_END_FOUND, expect false
     *  * Feed 0x0F to transition to START_FOUND, expect false
     *  * Feed 0 to transition to IN_DATA, expect false
     *  * Feed data from fcs_min_power.bin, expect false for every byte
     *  * Feed 0xF0 to transition to IN_DATA_END_FOUND, expect false
     *  * Feed 0x0F to transition to START_FOUND, expect true
     *  * Check return message, expect:
     *      * motor: -2147483648
     *      * pitch: 0
     *      * roll: 0
     */
    message_decoding_data_t decodingData;
    message_decoding_init(&decodingData, 0, ToolboxPlaneMessages_FlightControllerSetpoint_fields);

    EXPECT_FALSE(message_decoding_decode(&decodingData, 0xF0, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0x0F, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0, nullptr));

    std::ifstream stream{"fcs_min_power.bin"};
    using c_stream_it = std::istreambuf_iterator<char>;

    for (auto it = c_stream_it{stream}; it != c_stream_it{}; ++it) {
        EXPECT_FALSE(message_decoding_decode(&decodingData, *it, nullptr));
    }

    EXPECT_FALSE(message_decoding_decode(&decodingData, 0xF0, nullptr));
    ToolboxPlaneMessages_FlightControllerSetpoint buf;
    EXPECT_TRUE(message_decoding_decode(&decodingData, 0x0F, &buf));

    EXPECT_EQ(buf.motor, -2147483648);
    EXPECT_EQ(buf.pitch, 0);
    EXPECT_EQ(buf.roll, 0);
}

TEST(TEST_NAME, decode__fcs_max_power) {
    /*
     * Flight-Controller-Set-Point min power test:
     *  * Initialize with message_id as 0
     *  * Feed 0xF0 to transition to INITIAL_END_FOUND, expect false
     *  * Feed 0x0F to transition to START_FOUND, expect false
     *  * Feed 0 to transition to IN_DATA, expect false
     *  * Feed data from fcs_max_power.bin, expect false for every byte
     *  * Feed 0xF0 to transition to IN_DATA_END_FOUND, expect false
     *  * Feed 0x0F to transition to START_FOUND, expect true
     *  * Check return message, expect:
     *      * motor: 2147483647
     *      * pitch: 0
     *      * roll: 0
     */
    message_decoding_data_t decodingData;
    message_decoding_init(&decodingData, 0, ToolboxPlaneMessages_FlightControllerSetpoint_fields);

    EXPECT_FALSE(message_decoding_decode(&decodingData, 0xF0, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0x0F, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0, nullptr));

    std::ifstream stream{"fcs_max_power.bin"};
    using c_stream_it = std::istreambuf_iterator<char>;

    for (auto it = c_stream_it{stream}; it != c_stream_it{}; ++it) {
        EXPECT_FALSE(message_decoding_decode(&decodingData, *it, nullptr));
    }

    EXPECT_FALSE(message_decoding_decode(&decodingData, 0xF0, nullptr));
    ToolboxPlaneMessages_FlightControllerSetpoint buf;
    EXPECT_TRUE(message_decoding_decode(&decodingData, 0x0F, &buf));

    EXPECT_EQ(buf.motor, 2147483647);
    EXPECT_EQ(buf.pitch, 0);
    EXPECT_EQ(buf.roll, 0);
}

TEST(TEST_NAME, decode__fcs_min_pitch) {
    /*
     * Flight-Controller-Set-Point min power test:
     *  * Initialize with message_id as 0
     *  * Feed 0xF0 to transition to INITIAL_END_FOUND, expect false
     *  * Feed 0x0F to transition to START_FOUND, expect false
     *  * Feed 0 to transition to IN_DATA, expect false
     *  * Feed data from fcs_min_pitch.bin, expect false for every byte
     *  * Feed 0xF0 to transition to IN_DATA_END_FOUND, expect false
     *  * Feed 0x0F to transition to START_FOUND, expect true
     *  * Check return message, expect:
     *      * motor: 0
     *      * pitch: -2147483648
     *      * roll: 0
     */
    message_decoding_data_t decodingData;
    message_decoding_init(&decodingData, 0, ToolboxPlaneMessages_FlightControllerSetpoint_fields);

    EXPECT_FALSE(message_decoding_decode(&decodingData, 0xF0, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0x0F, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0, nullptr));

    std::ifstream stream{"fcs_min_pitch.bin"};
    using c_stream_it = std::istreambuf_iterator<char>;

    for (auto it = c_stream_it{stream}; it != c_stream_it{}; ++it) {
        EXPECT_FALSE(message_decoding_decode(&decodingData, *it, nullptr));
    }

    EXPECT_FALSE(message_decoding_decode(&decodingData, 0xF0, nullptr));
    ToolboxPlaneMessages_FlightControllerSetpoint buf;
    EXPECT_TRUE(message_decoding_decode(&decodingData, 0x0F, &buf));

    EXPECT_EQ(buf.motor, 0);
    EXPECT_EQ(buf.pitch, -2147483648);
    EXPECT_EQ(buf.roll, 0);
}

TEST(TEST_NAME, decode__fcs_max_pitch) {
    /*
     * Flight-Controller-Set-Point min power test:
     *  * Initialize with message_id as 0
     *  * Feed 0xF0 to transition to INITIAL_END_FOUND, expect false
     *  * Feed 0x0F to transition to START_FOUND, expect false
     *  * Feed 0 to transition to IN_DATA, expect false
     *  * Feed data from fcs_max_pitch.bin, expect false for every byte
     *  * Feed 0xF0 to transition to IN_DATA_END_FOUND, expect false
     *  * Feed 0x0F to transition to START_FOUND, expect true
     *  * Check return message, expect:
     *      * motor: 0
     *      * pitch: 2147483647
     *      * roll: 0
     */
    message_decoding_data_t decodingData;
    message_decoding_init(&decodingData, 0, ToolboxPlaneMessages_FlightControllerSetpoint_fields);

    EXPECT_FALSE(message_decoding_decode(&decodingData, 0xF0, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0x0F, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0, nullptr));

    std::ifstream stream{"fcs_max_pitch.bin"};
    using c_stream_it = std::istreambuf_iterator<char>;

    for (auto it = c_stream_it{stream}; it != c_stream_it{}; ++it) {
        EXPECT_FALSE(message_decoding_decode(&decodingData, *it, nullptr));
    }

    EXPECT_FALSE(message_decoding_decode(&decodingData, 0xF0, nullptr));
    ToolboxPlaneMessages_FlightControllerSetpoint buf;
    EXPECT_TRUE(message_decoding_decode(&decodingData, 0x0F, &buf));

    EXPECT_EQ(buf.motor, 0);
    EXPECT_EQ(buf.pitch, 2147483647);
    EXPECT_EQ(buf.roll, 0);
}

TEST(TEST_NAME, decode__fcs_min_roll) {
    /*
     * Flight-Controller-Set-Point min power test:
     *  * Initialize with message_id as 0
     *  * Feed 0xF0 to transition to INITIAL_END_FOUND, expect false
     *  * Feed 0x0F to transition to START_FOUND, expect false
     *  * Feed 0 to transition to IN_DATA, expect false
     *  * Feed data from fcs_min_roll.bin, expect false for every byte
     *  * Feed 0xF0 to transition to IN_DATA_END_FOUND, expect false
     *  * Feed 0x0F to transition to START_FOUND, expect true
     *  * Check return message, expect:
     *      * motor: 0
     *      * pitch: 0
     *      * roll: -2147483648
     */
    message_decoding_data_t decodingData;
    message_decoding_init(&decodingData, 0, ToolboxPlaneMessages_FlightControllerSetpoint_fields);

    EXPECT_FALSE(message_decoding_decode(&decodingData, 0xF0, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0x0F, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0, nullptr));

    std::ifstream stream{"fcs_min_roll.bin"};
    using c_stream_it = std::istreambuf_iterator<char>;

    for (auto it = c_stream_it{stream}; it != c_stream_it{}; ++it) {
        EXPECT_FALSE(message_decoding_decode(&decodingData, *it, nullptr));
    }

    EXPECT_FALSE(message_decoding_decode(&decodingData, 0xF0, nullptr));
    ToolboxPlaneMessages_FlightControllerSetpoint buf;
    EXPECT_TRUE(message_decoding_decode(&decodingData, 0x0F, &buf));

    EXPECT_EQ(buf.motor, 0);
    EXPECT_EQ(buf.pitch, 0);
    EXPECT_EQ(buf.roll, -2147483648);
}

TEST(TEST_NAME, decode__fcs_max_roll) {
    /*
     * Flight-Controller-Set-Point min power test:
     *  * Initialize with message_id as 0
     *  * Feed 0xF0 to transition to INITIAL_END_FOUND, expect false
     *  * Feed 0x0F to transition to START_FOUND, expect false
     *  * Feed 0 to transition to IN_DATA, expect false
     *  * Feed data from fcs_max_roll.bin, expect false for every byte
     *  * Feed 0xF0 to transition to IN_DATA_END_FOUND, expect false
     *  * Feed 0x0F to transition to START_FOUND, expect true
     *  * Check return message, expect:
     *      * motor: 0
     *      * pitch: 0
     *      * roll: 2147483647
     */
    message_decoding_data_t decodingData;
    message_decoding_init(&decodingData, 0, ToolboxPlaneMessages_FlightControllerSetpoint_fields);

    EXPECT_FALSE(message_decoding_decode(&decodingData, 0xF0, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0x0F, nullptr));
    EXPECT_FALSE(message_decoding_decode(&decodingData, 0, nullptr));

    std::ifstream stream{"fcs_max_roll.bin"};
    using c_stream_it = std::istreambuf_iterator<char>;

    for (auto it = c_stream_it{stream}; it != c_stream_it{}; ++it) {
        EXPECT_FALSE(message_decoding_decode(&decodingData, *it, nullptr));
    }

    EXPECT_FALSE(message_decoding_decode(&decodingData, 0xF0, nullptr));
    ToolboxPlaneMessages_FlightControllerSetpoint buf;
    EXPECT_TRUE(message_decoding_decode(&decodingData, 0x0F, &buf));

    EXPECT_EQ(buf.motor, 0);
    EXPECT_EQ(buf.pitch, 0);
    EXPECT_EQ(buf.roll, 2147483647);
}
