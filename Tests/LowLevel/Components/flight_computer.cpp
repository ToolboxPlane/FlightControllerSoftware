#include <Mock/protobuf.hpp>
#include <gtest/gtest.h>

extern "C" {
#include <Components/flight_computer.h>
}

TEST(TEST_NAME, init) {
    auto handle = mock::protobuf.getHandle();

    flight_computer_init();
    EXPECT_TRUE(handle.functionGotCalled<protobuf_init>());
}

TEST(TEST_NAME, send) {
    auto handle = mock::protobuf.getHandle();

    flight_computer_init();
    EXPECT_TRUE(handle.functionGotCalled<protobuf_init>());

    handle.overrideFunc<protobuf_send>([](const fc_message_t *message) {
        EXPECT_TRUE(message->has_imu);
        EXPECT_TRUE(message->has_remote);

        EXPECT_EQ(message->imu.yaw_mul_16, 1);
        EXPECT_EQ(message->imu.pitch_mul_16, 2);
        EXPECT_EQ(message->imu.roll_mul_16, 3);
        EXPECT_EQ(message->imu.dYaw_mul_16, 4);
        EXPECT_EQ(message->imu.dPitch_mul_16, 5);
        EXPECT_EQ(message->imu.dRoll_mul_16, 6);
        EXPECT_EQ(message->imu.accX_mul_100, 7);
        EXPECT_EQ(message->imu.accY_mul_100, 8);
        EXPECT_EQ(message->imu.accZ_mul_100, 9);
        EXPECT_EQ(message->imu.imu_ok, true);

        EXPECT_EQ(message->remote.throttleMixed, 10);
        EXPECT_EQ(message->remote.elevonLeftMixed, 11);
        EXPECT_EQ(message->remote.elevonRightMixed, 12);
        EXPECT_EQ(message->remote.isArmed, true);
        EXPECT_EQ(message->remote.overrideActive, false);
        EXPECT_EQ(message->remote.remote_ok, true);

        EXPECT_EQ(message->motor, 17);
        EXPECT_EQ(message->servoLeft, 18);
        EXPECT_EQ(message->servoRight, 19);
    });

    imu_data_t imuData = {.heading_mul_16 = 1,
                          .pitch_mul_16 = 2,
                          .roll_mul_16 = 3,
                          .d_heading_mul_16 = 4,
                          .d_pitch_mul_16 = 5,
                          .d_roll_mul_16 = 6,
                          .acc_x_mul_100 = 7,
                          .acc_y_mul_100 = 8,
                          .acc_z_mul_100 = 9,
                          .imu_ok = true};
    remote_data_t remoteData = {.throttle_mixed = 10,
                                .elevon_left_mixed = 11,
                                .elevon_right_mixed = 12,
                                .is_armed = true,
                                .override_active = false,
                                .remote_ok = true};
    actuator_cmd_t actuatorCmd = {.motor = 17, .elevon_left = 18, .elevon_right = 19};
    flight_computer_send(&imuData, &remoteData, &actuatorCmd);

    EXPECT_TRUE(handle.functionGotCalled<protobuf_send>());
}

TEST(TEST_NAME, data_available_true) {
    auto handle = mock::protobuf.getHandle();

    flight_computer_init();
    EXPECT_TRUE(handle.functionGotCalled<protobuf_init>());

    handle.overrideFunc<protobuf_available>([]() { return true; });

    EXPECT_TRUE(flight_computer_set_point_available());
}

TEST(TEST_NAME, data_available_false) {
    auto handle = mock::protobuf.getHandle();

    flight_computer_init();
    EXPECT_TRUE(handle.functionGotCalled<protobuf_init>());

    handle.overrideFunc<protobuf_available>([]() { return false; });

    EXPECT_FALSE(flight_computer_set_point_available());
}

TEST(TEST_NAME, get_setpoint) {
    auto handle = mock::protobuf.getHandle();

    flight_computer_init();
    EXPECT_TRUE(handle.functionGotCalled<protobuf_init>());

    handle.overrideFunc<protobuf_get>([]() {
        return ToolboxPlaneMessages_FlightControllerSetpoint{
                .motor = 37,
                .pitch = 39,
                .roll = 38,
        };
    });

    auto sp = flight_computer_get_set_point();
    EXPECT_EQ(sp.motor, 37);
    EXPECT_EQ(sp.pitch, 39);
    EXPECT_EQ(sp.roll, 38);
}
