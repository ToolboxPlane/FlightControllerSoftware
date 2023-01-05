#include <Mock/error_handler.hpp>
#include <Mock/flight_computer.hpp>
#include <Mock/imu.hpp>
#include <Mock/remote.hpp>
#include <gtest/gtest.h>

extern "C" {
#include <Application/mode_handler.h>
}

TEST(TEST_NAME, imu_initial_timeout) {
    auto imuHandle = mock::imu.getHandle();
    auto flightComputerHandle = mock::flight_computer.getHandle();
    auto remoteHandle = mock::remote.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    imuHandle.overrideFunc<imu_data_available>([]() { return false; });
    imuHandle.overrideFunc<imu_get_latest_data>([]() {
        imu_data_t res{};
        res.imu_ok = true;
        return res;
    });
    remoteHandle.overrideFunc<remote_data_available>([]() { return true; });
    remoteHandle.overrideFunc<remote_get_data>([]() {
        remote_data_t res{};
        res.is_armed = true;
        res.override_active = false;
        res.remote_ok = true;
        return res;
    });
    flightComputerHandle.overrideFunc<flight_computer_set_point_available>([]() { return true; });
    flightComputerHandle.overrideFunc<flight_computer_get_set_point>([]() { return flight_computer_set_point_t{}; });

    mode_handler_init();

    imu_data_t imuData;
    remote_data_t remoteData;
    flight_computer_set_point_t flightcomputerSetpoint;

    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_REMOTE);
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                   MODE_HANDLER_ERROR_NO_IMU_DATA));
}

TEST(TEST_NAME, imu_initial_nok) {
    auto imuHandle = mock::imu.getHandle();
    auto flightComputerHandle = mock::flight_computer.getHandle();
    auto remoteHandle = mock::remote.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    imuHandle.overrideFunc<imu_data_available>([]() { return true; });
    imuHandle.overrideFunc<imu_get_latest_data>([]() {
        imu_data_t res{};
        res.imu_ok = false;
        return res;
    });
    remoteHandle.overrideFunc<remote_data_available>([]() { return true; });
    remoteHandle.overrideFunc<remote_get_data>([]() {
        remote_data_t res{};
        res.is_armed = true;
        res.override_active = false;
        res.remote_ok = true;
        return res;
    });
    flightComputerHandle.overrideFunc<flight_computer_set_point_available>([]() { return true; });
    flightComputerHandle.overrideFunc<flight_computer_get_set_point>([]() { return flight_computer_set_point_t{}; });

    mode_handler_init();

    imu_data_t imuData;
    remote_data_t remoteData;
    flight_computer_set_point_t flightcomputerSetpoint;

    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_REMOTE);
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                   MODE_HANDLER_ERROR_NO_IMU_DATA));
}

TEST(TEST_NAME, imu_timeout) {
    auto imuHandle = mock::imu.getHandle();
    auto flightComputerHandle = mock::flight_computer.getHandle();
    auto remoteHandle = mock::remote.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    std::size_t frameCounter = 0;
    imuHandle.overrideFunc<imu_data_available>([&frameCounter]() {
        frameCounter += 1;
        return frameCounter < 2 or frameCounter > 7;
    });
    imuHandle.overrideFunc<imu_get_latest_data>([]() {
        imu_data_t res{};
        res.imu_ok = true;
        return res;
    });
    remoteHandle.overrideFunc<remote_data_available>([]() { return true; });
    remoteHandle.overrideFunc<remote_get_data>([]() {
        remote_data_t res{};
        res.is_armed = true;
        res.override_active = false;
        res.remote_ok = true;
        return res;
    });
    flightComputerHandle.overrideFunc<flight_computer_set_point_available>([]() { return true; });
    flightComputerHandle.overrideFunc<flight_computer_get_set_point>([]() { return flight_computer_set_point_t{}; });

    mode_handler_init();

    imu_data_t imuData;
    remote_data_t remoteData;
    flight_computer_set_point_t flightcomputerSetpoint;

    // 1 - data available
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_IMU_DATA));

    // 2 - no data available (1. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_IMU_DATA));

    // 3 - no data available (2. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_IMU_DATA));

    // 4 - no data available (3. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_IMU_DATA));

    // 5 - no data available (4. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_IMU_DATA));

    // 6 - no data available (5. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_IMU_DATA));

    // 7 - no data available (6. frame) -> timeout
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_REMOTE);
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                   MODE_HANDLER_ERROR_NO_IMU_DATA));

    // 8 - data available
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_IMU_DATA));
}

TEST(TEST_NAME, remote_initial_timeout) {
    auto imuHandle = mock::imu.getHandle();
    auto flightComputerHandle = mock::flight_computer.getHandle();
    auto remoteHandle = mock::remote.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    imuHandle.overrideFunc<imu_data_available>([]() { return true; });
    imuHandle.overrideFunc<imu_get_latest_data>([]() {
        imu_data_t res{};
        res.imu_ok = true;
        return res;
    });
    remoteHandle.overrideFunc<remote_data_available>([]() { return false; });
    remoteHandle.overrideFunc<remote_get_data>([]() {
        remote_data_t res{};
        res.is_armed = true;
        res.override_active = false;
        res.remote_ok = true;
        return res;
    });
    flightComputerHandle.overrideFunc<flight_computer_set_point_available>([]() { return true; });
    flightComputerHandle.overrideFunc<flight_computer_get_set_point>([]() { return flight_computer_set_point_t{}; });

    mode_handler_init();

    imu_data_t imuData;
    remote_data_t remoteData;
    flight_computer_set_point_t flightcomputerSetpoint;

    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_STABILISED_FAILSAFE);
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                   MODE_HANDLER_ERROR_NO_REMOTE_DATA));
}

TEST(TEST_NAME, remote_initial_nok) {
    auto imuHandle = mock::imu.getHandle();
    auto flightComputerHandle = mock::flight_computer.getHandle();
    auto remoteHandle = mock::remote.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    imuHandle.overrideFunc<imu_data_available>([]() { return true; });
    imuHandle.overrideFunc<imu_get_latest_data>([]() {
        imu_data_t res{};
        res.imu_ok = true;
        return res;
    });
    remoteHandle.overrideFunc<remote_data_available>([]() { return true; });
    remoteHandle.overrideFunc<remote_get_data>([]() {
        remote_data_t res{};
        res.is_armed = true;
        res.override_active = false;
        res.remote_ok = false;
        return res;
    });
    flightComputerHandle.overrideFunc<flight_computer_set_point_available>([]() { return true; });
    flightComputerHandle.overrideFunc<flight_computer_get_set_point>([]() { return flight_computer_set_point_t{}; });

    mode_handler_init();

    imu_data_t imuData;
    remote_data_t remoteData;
    flight_computer_set_point_t flightcomputerSetpoint;

    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_STABILISED_FAILSAFE);
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                   MODE_HANDLER_ERROR_NO_REMOTE_DATA));
}

TEST(TEST_NAME, remote_timeout) {
    auto imuHandle = mock::imu.getHandle();
    auto flightComputerHandle = mock::flight_computer.getHandle();
    auto remoteHandle = mock::remote.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    std::size_t frameCounter = 0;
    imuHandle.overrideFunc<imu_data_available>([]() { return true; });
    imuHandle.overrideFunc<imu_get_latest_data>([]() {
        imu_data_t res{};
        res.imu_ok = true;
        return res;
    });
    remoteHandle.overrideFunc<remote_data_available>([&frameCounter]() {
        frameCounter += 1;
        return frameCounter < 2 or frameCounter > 7;
    });
    remoteHandle.overrideFunc<remote_get_data>([]() {
        remote_data_t res{};
        res.is_armed = true;
        res.override_active = false;
        res.remote_ok = true;
        return res;
    });
    flightComputerHandle.overrideFunc<flight_computer_set_point_available>([]() { return true; });
    flightComputerHandle.overrideFunc<flight_computer_get_set_point>([]() { return flight_computer_set_point_t{}; });

    mode_handler_init();

    imu_data_t imuData;
    remote_data_t remoteData;
    flight_computer_set_point_t flightcomputerSetpoint;

    // 1 - data available
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_REMOTE_DATA));

    // 2 - no data available (1. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_REMOTE_DATA));

    // 3 - no data available (2. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_REMOTE_DATA));

    // 4 - no data available (3. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_REMOTE_DATA));

    // 5 - no data available (4. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_REMOTE_DATA));

    // 6 - no data available (5. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_REMOTE_DATA));

    // 7 - no data available (6. frame) -> timeout
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_STABILISED_FAILSAFE);
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                   MODE_HANDLER_ERROR_NO_REMOTE_DATA));

    // 8 - data available
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_REMOTE_DATA));
}

TEST(TEST_NAME, fcp_initial_timeout) {
    auto imuHandle = mock::imu.getHandle();
    auto flightComputerHandle = mock::flight_computer.getHandle();
    auto remoteHandle = mock::remote.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    imuHandle.overrideFunc<imu_data_available>([]() { return true; });
    imuHandle.overrideFunc<imu_get_latest_data>([]() {
        imu_data_t res{};
        res.imu_ok = true;
        return res;
    });
    remoteHandle.overrideFunc<remote_data_available>([]() { return true; });
    remoteHandle.overrideFunc<remote_get_data>([]() {
        remote_data_t res{};
        res.is_armed = true;
        res.override_active = false;
        res.remote_ok = true;
        return res;
    });
    flightComputerHandle.overrideFunc<flight_computer_set_point_available>([]() { return false; });
    flightComputerHandle.overrideFunc<flight_computer_get_set_point>([]() { return flight_computer_set_point_t{}; });

    mode_handler_init();

    imu_data_t imuData;
    remote_data_t remoteData;
    flight_computer_set_point_t flightcomputerSetpoint;

    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_REMOTE);
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                   MODE_HANDLER_ERROR_NO_FCP_DATA));
}

TEST(TEST_NAME, fcp_timeout) {
    auto imuHandle = mock::imu.getHandle();
    auto flightComputerHandle = mock::flight_computer.getHandle();
    auto remoteHandle = mock::remote.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    std::size_t frameCounter = 0;
    imuHandle.overrideFunc<imu_data_available>([]() { return true; });
    imuHandle.overrideFunc<imu_get_latest_data>([]() {
        imu_data_t res{};
        res.imu_ok = true;
        return res;
    });
    remoteHandle.overrideFunc<remote_data_available>([]() { return true; });
    remoteHandle.overrideFunc<remote_get_data>([]() {
        remote_data_t res{};
        res.is_armed = true;
        res.override_active = false;
        res.remote_ok = true;
        return res;
    });
    flightComputerHandle.overrideFunc<flight_computer_set_point_available>([&frameCounter]() {
        frameCounter += 1;
        return frameCounter < 2 or frameCounter > 13;
    });
    flightComputerHandle.overrideFunc<flight_computer_get_set_point>([]() { return flight_computer_set_point_t{}; });

    mode_handler_init();

    imu_data_t imuData;
    remote_data_t remoteData;
    flight_computer_set_point_t flightcomputerSetpoint;

    // 1 - data available
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_FCP_DATA));

    // 2 - no data available (1. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_FCP_DATA));

    // 3 - no data available (2. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_FCP_DATA));

    // 4 - no data available (3. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_FCP_DATA));

    // 5 - no data available (4. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_FCP_DATA));

    // 6 - no data available (5. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_FCP_DATA));

    // 7 - no data available (6. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_FCP_DATA));

    // 8 - no data available (7. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_FCP_DATA));

    // 9 - no data available (8. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_FCP_DATA));

    // 10 - no data available (9. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_FCP_DATA));

    // 11 - no data available (10. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_FCP_DATA));

    // 12 - no data available (11. frame)
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_FCP_DATA));

    // 13 - no data available (12. frame) -> timeout
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_REMOTE);
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                   MODE_HANDLER_ERROR_NO_FCP_DATA));

    // 14 - data available
    EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), MODE_FLIGHTCOMPUTER);
    EXPECT_FALSE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_MODE_HANDLER,
                                                                                    MODE_HANDLER_ERROR_NO_FCP_DATA));
}

TEST(TEST_NAME, modeselection) {
    auto imuHandle = mock::imu.getHandle();
    auto flightComputerHandle = mock::flight_computer.getHandle();
    auto remoteHandle = mock::remote.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    imuHandle.overrideFunc<imu_data_available>([]() { return true; });
    remoteHandle.overrideFunc<remote_data_available>([]() { return true; });
    flightComputerHandle.overrideFunc<flight_computer_get_set_point>([]() { return flight_computer_set_point_t{}; });

    struct availability_values {
        bool imu, remote, fcp, override, arm;
    };

    const std::vector<std::pair<availability_values, mode_handler_mode_t>> decisionTable{
            {{false, false, false, false, false}, MODE_FAILSAFE},
            {{false, false, false, false, true}, MODE_FAILSAFE},
            {{false, false, false, true, false}, MODE_FAILSAFE},
            {{false, false, false, true, true}, MODE_FAILSAFE},
            {{false, false, true, false, false}, MODE_FAILSAFE},
            {{false, false, true, false, true}, MODE_FAILSAFE},
            {{false, false, true, true, false}, MODE_FAILSAFE},
            {{false, false, true, true, true}, MODE_FAILSAFE},
            {{false, true, false, false, false}, MODE_REMOTE},
            {{false, true, false, false, true}, MODE_REMOTE},
            {{false, true, false, true, false}, MODE_REMOTE},
            {{false, true, false, true, true}, MODE_REMOTE},
            {{false, true, true, false, false}, MODE_REMOTE},
            {{false, true, true, false, true}, MODE_REMOTE},
            {{false, true, true, true, false}, MODE_REMOTE},
            {{false, true, true, true, true}, MODE_REMOTE},
            {{true, false, false, false, false}, MODE_STABILISED_FAILSAFE},
            {{true, false, false, false, true}, MODE_STABILISED_FAILSAFE},
            {{true, false, false, true, false}, MODE_STABILISED_FAILSAFE},
            {{true, false, false, true, true}, MODE_STABILISED_FAILSAFE},
            {{true, false, true, false, false}, MODE_STABILISED_FAILSAFE},
            {{true, false, true, false, true}, MODE_STABILISED_FAILSAFE},
            {{true, false, true, true, false}, MODE_STABILISED_FAILSAFE},
            {{true, false, true, true, true}, MODE_STABILISED_FAILSAFE},
            {{true, true, false, false, false}, MODE_REMOTE},
            {{true, true, false, false, true}, MODE_REMOTE},
            {{true, true, false, true, false}, MODE_REMOTE},
            {{true, true, false, true, true}, MODE_REMOTE},
            {{true, true, true, false, false}, MODE_STABILISED_FAILSAFE},
            {{true, true, true, false, true}, MODE_FLIGHTCOMPUTER},
            {{true, true, true, true, false}, MODE_REMOTE},
            {{true, true, true, true, true}, MODE_REMOTE},
    };

    for (auto row : decisionTable) {
        /*
         * Should be a structure binding but clang < 16 does not support capturing of structure bindings
         * as those are by definition no variables...
         */
        auto availability_value = row.first;
        auto expected_mode = row.second;
        mode_handler_init();
        imuHandle.overrideFunc<imu_get_latest_data>([availability_value]() {
            imu_data_t res{};
            res.imu_ok = availability_value.imu;
            return res;
        });
        remoteHandle.overrideFunc<remote_get_data>([availability_value]() {
            remote_data_t res{};
            res.is_armed = availability_value.arm;
            res.override_active = availability_value.override;
            res.remote_ok = availability_value.remote;
            return res;
        });
        flightComputerHandle.overrideFunc<flight_computer_set_point_available>(
                [availability_value]() { return availability_value.fcp; });

        imu_data_t imuData;
        remote_data_t remoteData;
        flight_computer_set_point_t flightcomputerSetpoint;
        EXPECT_EQ(mode_handler_handle(&imuData, &remoteData, &flightcomputerSetpoint), expected_mode);
        if (!availability_value.remote) {
            EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(
                    ERROR_HANDLER_GROUP_MODE_HANDLER, MODE_HANDLER_ERROR_NO_REMOTE_DATA));
        }
        if (!availability_value.imu) {
            EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(
                    ERROR_HANDLER_GROUP_MODE_HANDLER, MODE_HANDLER_ERROR_NO_IMU_DATA));
        }
        if (!availability_value.fcp) {
            EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(
                    ERROR_HANDLER_GROUP_MODE_HANDLER, MODE_HANDLER_ERROR_NO_FCP_DATA));
        }
    }
}

TEST(TEST_NAME, fill_out_vars) {
    auto imuHandle = mock::imu.getHandle();
    auto flightComputerHandle = mock::flight_computer.getHandle();
    auto remoteHandle = mock::remote.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    imuHandle.overrideFunc<imu_data_available>([]() { return true; });
    imuHandle.overrideFunc<imu_get_latest_data>([]() {
        return imu_data_t{.heading_mul_16 = 17,
                          .pitch_mul_16 = 18,
                          .roll_mul_16 = 19,
                          .d_heading_mul_16 = 20,
                          .d_pitch_mul_16 = 21,
                          .d_roll_mul_16 = 22,
                          .acc_x_mul_100 = 23,
                          .acc_y_mul_100 = 24,
                          .acc_z_mul_100 = 25,
                          .imu_ok = true};
    });
    remoteHandle.overrideFunc<remote_data_available>([]() { return true; });
    remoteHandle.overrideFunc<remote_get_data>([]() {
        return remote_data_t{.throttle_mixed = 26,
                             .elevon_left_mixed = 27,
                             .elevon_right_mixed = 28,
                             .is_armed = true,
                             .override_active = false,
                             .remote_ok = true};
    });
    flightComputerHandle.overrideFunc<flight_computer_set_point_available>([]() { return true; });
    flightComputerHandle.overrideFunc<flight_computer_get_set_point>(
            []() { return flight_computer_set_point_t{.motor = 32, .pitch = 33, .roll = 34}; });

    mode_handler_init();

    imu_data_t imuData;
    remote_data_t remoteData;
    flight_computer_set_point_t flightComputerSetPoint;

    mode_handler_handle(&imuData, &remoteData, &flightComputerSetPoint);
    EXPECT_EQ(imuData.heading_mul_16, 17);
    EXPECT_EQ(imuData.pitch_mul_16, 18);
    EXPECT_EQ(imuData.roll_mul_16, 19);
    EXPECT_EQ(imuData.d_heading_mul_16, 20);
    EXPECT_EQ(imuData.d_pitch_mul_16, 21);
    EXPECT_EQ(imuData.d_roll_mul_16, 22);
    EXPECT_EQ(imuData.acc_x_mul_100, 23);
    EXPECT_EQ(imuData.acc_y_mul_100, 24);
    EXPECT_EQ(imuData.acc_z_mul_100, 25);
    EXPECT_EQ(imuData.imu_ok, true);
    EXPECT_EQ(remoteData.throttle_mixed, 26);
    EXPECT_EQ(remoteData.elevon_left_mixed, 27);
    EXPECT_EQ(remoteData.elevon_right_mixed, 28);
    EXPECT_EQ(remoteData.is_armed, true);
    EXPECT_EQ(remoteData.override_active, false);
    EXPECT_EQ(remoteData.remote_ok, true);
    EXPECT_EQ(flightComputerSetPoint.motor, 32);
    EXPECT_EQ(flightComputerSetPoint.pitch, 33);
    EXPECT_EQ(flightComputerSetPoint.roll, 34);
}
