#include <Mock/Application/error_handler.hpp>
#include <Mock/Components/flightcomputer.hpp>
#include <Mock/Components/imu.hpp>
#include <Mock/Components/remote.hpp>
#include <gtest/gtest.h>

extern "C" {
#include <Application/mode_handler.h>
}

TEST(TEST_NAME, all_ok) {
    auto imuHandle = mock::imu.getHandle();
    auto flightcomputerHandle = mock::flightcomputer.getHandle();
    auto remoteHandle = mock::remote.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    imuHandle.overrideFunc<imu_data_available>([]() { return true; });
    imuHandle.overrideFunc<imu_get_latest_data>([]() { return imu_data_t{.imu_ok = true}; });
    remoteHandle.overrideFunc<remote_data_available>([]() { return true; });
    remoteHandle.overrideFunc<remote_get_data>(
            []() { return remote_data_t{.is_armed = true, .override_active = false, .remote_ok = true}; });
    flightcomputerHandle.overrideFunc<flightcomputer_setpoint_available>([]() { return true; });
    flightcomputerHandle.overrideFunc<flightcomputer_get_setpoint>([]() { return flightcomputer_setpoint_t{}; });

    mode_handler_init();

    imu_data_t imuData;
    remote_data_t remoteData;
    flightcomputer_setpoint_t flightcomputerSetpoint;

    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
}

TEST(TEST_NAME, imu_initial_timeout) {
    auto imuHandle = mock::imu.getHandle();
    auto flightcomputerHandle = mock::flightcomputer.getHandle();
    auto remoteHandle = mock::remote.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    imuHandle.overrideFunc<imu_data_available>([]() { return false; });
    imuHandle.overrideFunc<imu_get_latest_data>([]() { return imu_data_t{.imu_ok = true}; });
    remoteHandle.overrideFunc<remote_data_available>([]() { return true; });
    remoteHandle.overrideFunc<remote_get_data>(
            []() { return remote_data_t{.is_armed = true, .override_active = false, .remote_ok = true}; });
    flightcomputerHandle.overrideFunc<flightcomputer_setpoint_available>([]() { return true; });
    flightcomputerHandle.overrideFunc<flightcomputer_get_setpoint>([]() { return flightcomputer_setpoint_t{}; });

    mode_handler_init();

    imu_data_t imuData;
    remote_data_t remoteData;
    flightcomputer_setpoint_t flightcomputerSetpoint;

    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_REMOTE);
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                   MODE_HANDLER_ERROR_NO_IMU_DATA));
}

TEST(TEST_NAME, imu_initial_nok) {
    auto imuHandle = mock::imu.getHandle();
    auto flightcomputerHandle = mock::flightcomputer.getHandle();
    auto remoteHandle = mock::remote.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    imuHandle.overrideFunc<imu_data_available>([]() { return true; });
    imuHandle.overrideFunc<imu_get_latest_data>([]() { return imu_data_t{.imu_ok = false}; });
    remoteHandle.overrideFunc<remote_data_available>([]() { return true; });
    remoteHandle.overrideFunc<remote_get_data>(
            []() { return remote_data_t{.is_armed = true, .override_active = false, .remote_ok = true}; });
    flightcomputerHandle.overrideFunc<flightcomputer_setpoint_available>([]() { return true; });
    flightcomputerHandle.overrideFunc<flightcomputer_get_setpoint>([]() { return flightcomputer_setpoint_t{}; });

    mode_handler_init();

    imu_data_t imuData;
    remote_data_t remoteData;
    flightcomputer_setpoint_t flightcomputerSetpoint;

    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_REMOTE);
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                   MODE_HANDLER_ERROR_NO_IMU_DATA));
}

TEST(TEST_NAME, imu_timeout) {
    auto imuHandle = mock::imu.getHandle();
    auto flightcomputerHandle = mock::flightcomputer.getHandle();
    auto remoteHandle = mock::remote.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    std::size_t frameCounter = 0;
    imuHandle.overrideFunc<imu_data_available>([&frameCounter]() {
        frameCounter += 1;
        return frameCounter < 2 or frameCounter > 7;
    });
    imuHandle.overrideFunc<imu_get_latest_data>([]() { return imu_data_t{.imu_ok = true}; });
    remoteHandle.overrideFunc<remote_data_available>([]() { return true; });
    remoteHandle.overrideFunc<remote_get_data>(
            []() { return remote_data_t{.is_armed = true, .override_active = false, .remote_ok = true}; });
    flightcomputerHandle.overrideFunc<flightcomputer_setpoint_available>([]() { return true; });
    flightcomputerHandle.overrideFunc<flightcomputer_get_setpoint>([]() { return flightcomputer_setpoint_t{}; });

    mode_handler_init();

    imu_data_t imuData;
    remote_data_t remoteData;
    flightcomputer_setpoint_t flightcomputerSetpoint;

    // 1 - data available
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_IMU_DATA));

    // 2 - no data available (1. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_IMU_DATA));

    // 3 - no data available (2. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_IMU_DATA));

    // 4 - no data available (3. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_IMU_DATA));

    // 5 - no data available (4. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_IMU_DATA));

    // 6 - no data available (5. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_IMU_DATA));

    // 7 - no data available (6. frame) -> timeout
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_REMOTE);
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                   MODE_HANDLER_ERROR_NO_IMU_DATA));

    // 8 - data available
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_IMU_DATA));
}

TEST(TEST_NAME, remote_initial_timeout) {
    auto imuHandle = mock::imu.getHandle();
    auto flightcomputerHandle = mock::flightcomputer.getHandle();
    auto remoteHandle = mock::remote.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    imuHandle.overrideFunc<imu_data_available>([]() { return true; });
    imuHandle.overrideFunc<imu_get_latest_data>([]() { return imu_data_t{.imu_ok = true}; });
    remoteHandle.overrideFunc<remote_data_available>([]() { return false; });
    remoteHandle.overrideFunc<remote_get_data>(
            []() { return remote_data_t{.is_armed = true, .override_active = false, .remote_ok = true}; });
    flightcomputerHandle.overrideFunc<flightcomputer_setpoint_available>([]() { return true; });
    flightcomputerHandle.overrideFunc<flightcomputer_get_setpoint>([]() { return flightcomputer_setpoint_t{}; });

    mode_handler_init();

    imu_data_t imuData;
    remote_data_t remoteData;
    flightcomputer_setpoint_t flightcomputerSetpoint;

    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_STABILISED_FAILSAVE);
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                   MODE_HANDLER_ERROR_NO_REMOTE_DATA));
}

TEST(TEST_NAME, remote_initial_nok) {
    auto imuHandle = mock::imu.getHandle();
    auto flightcomputerHandle = mock::flightcomputer.getHandle();
    auto remoteHandle = mock::remote.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    imuHandle.overrideFunc<imu_data_available>([]() { return true; });
    imuHandle.overrideFunc<imu_get_latest_data>([]() { return imu_data_t{.imu_ok = true}; });
    remoteHandle.overrideFunc<remote_data_available>([]() { return true; });
    remoteHandle.overrideFunc<remote_get_data>(
            []() { return remote_data_t{.is_armed = true, .override_active = false, .remote_ok = false}; });
    flightcomputerHandle.overrideFunc<flightcomputer_setpoint_available>([]() { return true; });
    flightcomputerHandle.overrideFunc<flightcomputer_get_setpoint>([]() { return flightcomputer_setpoint_t{}; });

    mode_handler_init();

    imu_data_t imuData;
    remote_data_t remoteData;
    flightcomputer_setpoint_t flightcomputerSetpoint;

    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_STABILISED_FAILSAVE);
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                   MODE_HANDLER_ERROR_NO_REMOTE_DATA));
}

TEST(TEST_NAME, remote_timeout) {
    auto imuHandle = mock::imu.getHandle();
    auto flightcomputerHandle = mock::flightcomputer.getHandle();
    auto remoteHandle = mock::remote.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    std::size_t frameCounter = 0;
    imuHandle.overrideFunc<imu_data_available>([]() { return true; });
    imuHandle.overrideFunc<imu_get_latest_data>([]() { return imu_data_t{.imu_ok = true}; });
    remoteHandle.overrideFunc<remote_data_available>([&frameCounter]() {
        frameCounter += 1;
        return frameCounter < 2 or frameCounter > 7;
    });
    remoteHandle.overrideFunc<remote_get_data>(
            []() { return remote_data_t{.is_armed = true, .override_active = false, .remote_ok = true}; });
    flightcomputerHandle.overrideFunc<flightcomputer_setpoint_available>([]() { return true; });
    flightcomputerHandle.overrideFunc<flightcomputer_get_setpoint>([]() { return flightcomputer_setpoint_t{}; });

    mode_handler_init();

    imu_data_t imuData;
    remote_data_t remoteData;
    flightcomputer_setpoint_t flightcomputerSetpoint;

    // 1 - data available
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_REMOTE_DATA));

    // 2 - no data available (1. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_REMOTE_DATA));

    // 3 - no data available (2. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_REMOTE_DATA));

    // 4 - no data available (3. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_REMOTE_DATA));

    // 5 - no data available (4. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_REMOTE_DATA));

    // 6 - no data available (5. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_REMOTE_DATA));

    // 7 - no data available (6. frame) -> timeout
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_STABILISED_FAILSAVE);
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                   MODE_HANDLER_ERROR_NO_REMOTE_DATA));

    // 8 - data available
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_REMOTE_DATA));
}

TEST(TEST_NAME, fcp_initial_timeout) {
    auto imuHandle = mock::imu.getHandle();
    auto flightcomputerHandle = mock::flightcomputer.getHandle();
    auto remoteHandle = mock::remote.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    imuHandle.overrideFunc<imu_data_available>([]() { return true; });
    imuHandle.overrideFunc<imu_get_latest_data>([]() { return imu_data_t{.imu_ok = true}; });
    remoteHandle.overrideFunc<remote_data_available>([]() { return true; });
    remoteHandle.overrideFunc<remote_get_data>(
            []() { return remote_data_t{.is_armed = true, .override_active = false, .remote_ok = true}; });
    flightcomputerHandle.overrideFunc<flightcomputer_setpoint_available>([]() { return false; });
    flightcomputerHandle.overrideFunc<flightcomputer_get_setpoint>([]() { return flightcomputer_setpoint_t{}; });

    mode_handler_init();

    imu_data_t imuData;
    remote_data_t remoteData;
    flightcomputer_setpoint_t flightcomputerSetpoint;

    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_REMOTE);
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                   MODE_HANDLER_ERROR_NO_FCP_DATA));
}

TEST(TEST_NAME, fcp_timeout) {
    auto imuHandle = mock::imu.getHandle();
    auto flightcomputerHandle = mock::flightcomputer.getHandle();
    auto remoteHandle = mock::remote.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    std::size_t frameCounter = 0;
    imuHandle.overrideFunc<imu_data_available>([]() { return true; });
    imuHandle.overrideFunc<imu_get_latest_data>([]() { return imu_data_t{.imu_ok = true}; });
    remoteHandle.overrideFunc<remote_data_available>([]() { return true; });
    remoteHandle.overrideFunc<remote_get_data>(
            []() { return remote_data_t{.is_armed = true, .override_active = false, .remote_ok = true}; });
    flightcomputerHandle.overrideFunc<flightcomputer_setpoint_available>([&frameCounter]() {
        frameCounter += 1;
        return frameCounter < 2 or frameCounter > 13;
    });
    flightcomputerHandle.overrideFunc<flightcomputer_get_setpoint>([]() { return flightcomputer_setpoint_t{}; });

    mode_handler_init();

    imu_data_t imuData;
    remote_data_t remoteData;
    flightcomputer_setpoint_t flightcomputerSetpoint;

    // 1 - data available
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_FCP_DATA));

    // 2 - no data available (1. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_FCP_DATA));

    // 3 - no data available (2. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_FCP_DATA));

    // 4 - no data available (3. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_FCP_DATA));

    // 5 - no data available (4. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_FCP_DATA));

    // 6 - no data available (5. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_FCP_DATA));

    // 7 - no data available (6. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_FCP_DATA));

    // 8 - no data available (7. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_FCP_DATA));

    // 9 - no data available (8. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_FCP_DATA));

    // 10 - no data available (9. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_FCP_DATA));

    // 11 - no data available (10. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_FCP_DATA));

    // 12 - no data available (11. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_FCP_DATA));

    // 13 - no data available (12. frame) -> timeout
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_REMOTE);
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                   MODE_HANDLER_ERROR_NO_FCP_DATA));

    // 14 - data available
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_FCP_DATA));
}
