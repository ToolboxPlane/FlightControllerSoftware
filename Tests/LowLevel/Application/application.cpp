#include <Mock/actuators.hpp>
#include <Mock/controller.hpp>
#include <Mock/error_handler.hpp>
#include <Mock/flight_computer.hpp>
#include <Mock/imu.hpp>
#include <Mock/mode_handler.hpp>
#include <Mock/remote.hpp>
#include <Mock/system.hpp>
#include <gtest/gtest.h>

extern "C" {
#include <Application/application.h>
}

TEST(TEST_NAME, init) {
    auto actuatorsHandle = mock::actuators.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();
    auto flightComputerHandle = mock::flight_computer.getHandle();
    auto imuHandle = mock::imu.getHandle();
    auto modeHandlerHandle = mock::mode_handler.getHandle();
    auto remoteHandlerHandle = mock::remote.getHandle();
    auto systemHandle = mock::system.getHandle();

    systemHandle.overrideFunc<system_reset_watchdog>(
            []() { throw std::runtime_error{"EXCEPTION TO BREAK LOOP FOR TESTS"}; });

    EXPECT_THROW(application_init(), std::runtime_error);

    EXPECT_TRUE(systemHandle.functionGotCalled<system_pre_init>(std::ignore));
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_init>());
    EXPECT_TRUE(imuHandle.functionGotCalled<imu_init>());
    EXPECT_TRUE(remoteHandlerHandle.functionGotCalled<remote_init>());
    EXPECT_TRUE(flightComputerHandle.functionGotCalled<flight_computer_init>());
    EXPECT_TRUE(actuatorsHandle.functionGotCalled<actuators_init>());
    EXPECT_TRUE(modeHandlerHandle.functionGotCalled<mode_handler_init>());
    EXPECT_TRUE(systemHandle.functionGotCalled<system_post_init>());
}

TEST(TEST_NAME, timer_mode_fcp) {
    auto actuatorsHandle = mock::actuators.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();
    auto flightComputerHandle = mock::flight_computer.getHandle();
    auto imuHandle = mock::imu.getHandle();
    auto modeHandlerHandle = mock::mode_handler.getHandle();
    auto remoteHandlerHandle = mock::remote.getHandle();
    auto systemHandle = mock::system.getHandle();
    auto controllerHandle = mock::controller.getHandle();

    system_timer_16_384ms_callback timer_callback = nullptr;
    systemHandle.overrideFunc<system_pre_init>(
            [&timer_callback](system_timer_16_384ms_callback callback) { timer_callback = callback; });
    systemHandle.overrideFunc<system_reset_watchdog>(
            []() { throw std::runtime_error{"EXCEPTION TO BREAK LOOP FOR TESTS"}; });

    EXPECT_THROW(application_init(), std::runtime_error);

    EXPECT_TRUE(systemHandle.functionGotCalled<system_pre_init>(std::ignore));
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_init>());
    EXPECT_TRUE(imuHandle.functionGotCalled<imu_init>());
    EXPECT_TRUE(remoteHandlerHandle.functionGotCalled<remote_init>());
    EXPECT_TRUE(flightComputerHandle.functionGotCalled<flight_computer_init>());
    EXPECT_TRUE(actuatorsHandle.functionGotCalled<actuators_init>());
    EXPECT_TRUE(modeHandlerHandle.functionGotCalled<mode_handler_init>());
    EXPECT_TRUE(systemHandle.functionGotCalled<system_post_init>());

    // Actual test
    modeHandlerHandle.overrideFunc<mode_handler_handle>(
            [](imu_data_t *imu_data, remote_data_t *remote_data, flight_computer_set_point_t *flightcomputer_setpoint) {
                imu_data->heading_mul_16 = 17;
                imu_data->pitch_mul_16 = 18;
                imu_data->roll_mul_16 = 19;
                imu_data->d_heading_mul_16 = 20;
                imu_data->d_pitch_mul_16 = 21;
                imu_data->d_roll_mul_16 = 22;
                imu_data->acc_x_mul_100 = 23;
                imu_data->acc_y_mul_100 = 24;
                imu_data->acc_z_mul_100 = 25;
                imu_data->imu_ok = true;
                remote_data->throttle_mixed = 26;
                remote_data->elevon_left_mixed = 27;
                remote_data->elevon_right_mixed = 28;
                remote_data->remote_ok = true;
                remote_data->override_active = false;
                remote_data->is_armed = true;
                flightcomputer_setpoint->roll = 29;
                flightcomputer_setpoint->pitch = 30;
                flightcomputer_setpoint->motor = 31;
                return MODE_FLIGHTCOMPUTER;
            });

    controllerHandle.overrideFunc<controller_update>(
            [](const imu_data_t *imu_data, int16_t roll_setpoint, int16_t pitch_setpoint) {
                EXPECT_EQ(imu_data->heading_mul_16, 17);
                EXPECT_EQ(imu_data->pitch_mul_16, 18);
                EXPECT_EQ(imu_data->roll_mul_16, 19);
                EXPECT_EQ(imu_data->d_heading_mul_16, 20);
                EXPECT_EQ(imu_data->d_pitch_mul_16, 21);
                EXPECT_EQ(imu_data->d_roll_mul_16, 22);
                EXPECT_EQ(imu_data->acc_x_mul_100, 23);
                EXPECT_EQ(imu_data->acc_y_mul_100, 24);
                EXPECT_EQ(imu_data->acc_z_mul_100, 25);
                EXPECT_EQ(imu_data->imu_ok, true);
                EXPECT_EQ(roll_setpoint, 29);
                EXPECT_EQ(pitch_setpoint, 30);
                return controller_result_t{.elevon_left = 32, .elevon_right = 33};
            });

    actuatorsHandle.overrideFunc<actuators_set>([](const actuator_cmd_t *actuator_cmd) {
        EXPECT_EQ(actuator_cmd->elevon_left, 32);
        EXPECT_EQ(actuator_cmd->elevon_right, 33);
        EXPECT_EQ(actuator_cmd->motor, 31);
    });
    timer_callback();
    EXPECT_TRUE(modeHandlerHandle.functionGotCalled<mode_handler_handle>());
    EXPECT_TRUE(controllerHandle.functionGotCalled<controller_update>());
    EXPECT_TRUE(actuatorsHandle.functionGotCalled<actuators_set>());
    EXPECT_TRUE(imuHandle.functionGotCalled<imu_start_sampling>());
}

TEST(TEST_NAME, timer_mode_remote) {
    auto actuatorsHandle = mock::actuators.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();
    auto flightComputerHandle = mock::flight_computer.getHandle();
    auto imuHandle = mock::imu.getHandle();
    auto modeHandlerHandle = mock::mode_handler.getHandle();
    auto remoteHandlerHandle = mock::remote.getHandle();
    auto systemHandle = mock::system.getHandle();
    auto controllerHandle = mock::controller.getHandle();

    system_timer_16_384ms_callback timer_callback = nullptr;
    systemHandle.overrideFunc<system_pre_init>(
            [&timer_callback](system_timer_16_384ms_callback callback) { timer_callback = callback; });
    systemHandle.overrideFunc<system_reset_watchdog>(
            []() { throw std::runtime_error{"EXCEPTION TO BREAK LOOP FOR TESTS"}; });

    EXPECT_THROW(application_init(), std::runtime_error);

    EXPECT_TRUE(systemHandle.functionGotCalled<system_pre_init>(std::ignore));
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_init>());
    EXPECT_TRUE(imuHandle.functionGotCalled<imu_init>());
    EXPECT_TRUE(remoteHandlerHandle.functionGotCalled<remote_init>());
    EXPECT_TRUE(flightComputerHandle.functionGotCalled<flight_computer_init>());
    EXPECT_TRUE(actuatorsHandle.functionGotCalled<actuators_init>());
    EXPECT_TRUE(modeHandlerHandle.functionGotCalled<mode_handler_init>());
    EXPECT_TRUE(systemHandle.functionGotCalled<system_post_init>());

    // Actual test
    modeHandlerHandle.overrideFunc<mode_handler_handle>(
            [](imu_data_t *imu_data, remote_data_t *remote_data, flight_computer_set_point_t *flightcomputer_setpoint) {
                imu_data->heading_mul_16 = 17;
                imu_data->pitch_mul_16 = 18;
                imu_data->roll_mul_16 = 19;
                imu_data->d_heading_mul_16 = 20;
                imu_data->d_pitch_mul_16 = 21;
                imu_data->d_roll_mul_16 = 22;
                imu_data->acc_x_mul_100 = 23;
                imu_data->acc_y_mul_100 = 24;
                imu_data->acc_z_mul_100 = 25;
                imu_data->imu_ok = true;
                remote_data->throttle_mixed = 26;
                remote_data->elevon_left_mixed = 27;
                remote_data->elevon_right_mixed = 28;
                remote_data->remote_ok = true;
                remote_data->override_active = false;
                remote_data->is_armed = true;
                flightcomputer_setpoint->roll = 29;
                flightcomputer_setpoint->pitch = 30;
                flightcomputer_setpoint->motor = 31;
                return MODE_REMOTE;
            });

    actuatorsHandle.overrideFunc<actuators_set>([](const actuator_cmd_t *actuator_cmd) {
        EXPECT_EQ(actuator_cmd->elevon_left, 27 - 500);
        EXPECT_EQ(actuator_cmd->elevon_right, 28 - 500);
        EXPECT_EQ(actuator_cmd->motor, 26);
    });
    timer_callback();
    EXPECT_TRUE(modeHandlerHandle.functionGotCalled<mode_handler_handle>());
    EXPECT_TRUE(actuatorsHandle.functionGotCalled<actuators_set>());
    EXPECT_TRUE(imuHandle.functionGotCalled<imu_start_sampling>());
}

TEST(TEST_NAME, timer_mode_stabilised_failsafe) {
    auto actuatorsHandle = mock::actuators.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();
    auto flightComputerHandle = mock::flight_computer.getHandle();
    auto imuHandle = mock::imu.getHandle();
    auto modeHandlerHandle = mock::mode_handler.getHandle();
    auto remoteHandlerHandle = mock::remote.getHandle();
    auto systemHandle = mock::system.getHandle();
    auto controllerHandle = mock::controller.getHandle();

    system_timer_16_384ms_callback timer_callback = nullptr;
    systemHandle.overrideFunc<system_pre_init>(
            [&timer_callback](system_timer_16_384ms_callback callback) { timer_callback = callback; });
    systemHandle.overrideFunc<system_reset_watchdog>(
            []() { throw std::runtime_error{"EXCEPTION TO BREAK LOOP FOR TESTS"}; });

    EXPECT_THROW(application_init(), std::runtime_error);

    EXPECT_TRUE(systemHandle.functionGotCalled<system_pre_init>(std::ignore));
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_init>());
    EXPECT_TRUE(imuHandle.functionGotCalled<imu_init>());
    EXPECT_TRUE(remoteHandlerHandle.functionGotCalled<remote_init>());
    EXPECT_TRUE(flightComputerHandle.functionGotCalled<flight_computer_init>());
    EXPECT_TRUE(actuatorsHandle.functionGotCalled<actuators_init>());
    EXPECT_TRUE(modeHandlerHandle.functionGotCalled<mode_handler_init>());
    EXPECT_TRUE(systemHandle.functionGotCalled<system_post_init>());

    // Actual test
    modeHandlerHandle.overrideFunc<mode_handler_handle>(
            [](imu_data_t *imu_data, remote_data_t *remote_data, flight_computer_set_point_t *flightcomputer_setpoint) {
                imu_data->heading_mul_16 = 17;
                imu_data->pitch_mul_16 = 18;
                imu_data->roll_mul_16 = 19;
                imu_data->d_heading_mul_16 = 20;
                imu_data->d_pitch_mul_16 = 21;
                imu_data->d_roll_mul_16 = 22;
                imu_data->acc_x_mul_100 = 23;
                imu_data->acc_y_mul_100 = 24;
                imu_data->acc_z_mul_100 = 25;
                imu_data->imu_ok = true;
                remote_data->throttle_mixed = 26;
                remote_data->elevon_left_mixed = 27;
                remote_data->elevon_right_mixed = 28;
                remote_data->remote_ok = true;
                remote_data->override_active = false;
                remote_data->is_armed = true;
                flightcomputer_setpoint->roll = 29;
                flightcomputer_setpoint->pitch = 30;
                flightcomputer_setpoint->motor = 31;
                return MODE_STABILISED_FAILSAFE;
            });

    controllerHandle.overrideFunc<controller_update>(
            [](const imu_data_t *imu_data, int16_t roll_setpoint, int16_t pitch_setpoint) {
                EXPECT_EQ(imu_data->heading_mul_16, 17);
                EXPECT_EQ(imu_data->pitch_mul_16, 18);
                EXPECT_EQ(imu_data->roll_mul_16, 19);
                EXPECT_EQ(imu_data->d_heading_mul_16, 20);
                EXPECT_EQ(imu_data->d_pitch_mul_16, 21);
                EXPECT_EQ(imu_data->d_roll_mul_16, 22);
                EXPECT_EQ(imu_data->acc_x_mul_100, 23);
                EXPECT_EQ(imu_data->acc_y_mul_100, 24);
                EXPECT_EQ(imu_data->acc_z_mul_100, 25);
                EXPECT_EQ(imu_data->imu_ok, true);
                EXPECT_EQ(roll_setpoint, 0);
                EXPECT_EQ(pitch_setpoint, 0);
                return controller_result_t{.elevon_left = 32, .elevon_right = 33};
            });

    actuatorsHandle.overrideFunc<actuators_set>([](const actuator_cmd_t *actuator_cmd) {
        EXPECT_EQ(actuator_cmd->elevon_left, 32);
        EXPECT_EQ(actuator_cmd->elevon_right, 33);
        EXPECT_EQ(actuator_cmd->motor, 0);
    });
    timer_callback();
    EXPECT_TRUE(modeHandlerHandle.functionGotCalled<mode_handler_handle>());
    EXPECT_TRUE(controllerHandle.functionGotCalled<controller_update>());
    EXPECT_TRUE(actuatorsHandle.functionGotCalled<actuators_set>());
    EXPECT_TRUE(imuHandle.functionGotCalled<imu_start_sampling>());
}

TEST(TEST_NAME, timer_mode_failsafe) {
    auto actuatorsHandle = mock::actuators.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();
    auto flightComputerHandle = mock::flight_computer.getHandle();
    auto imuHandle = mock::imu.getHandle();
    auto modeHandlerHandle = mock::mode_handler.getHandle();
    auto remoteHandlerHandle = mock::remote.getHandle();
    auto systemHandle = mock::system.getHandle();
    auto controllerHandle = mock::controller.getHandle();

    system_timer_16_384ms_callback timer_callback = nullptr;
    systemHandle.overrideFunc<system_pre_init>(
            [&timer_callback](system_timer_16_384ms_callback callback) { timer_callback = callback; });
    systemHandle.overrideFunc<system_reset_watchdog>(
            []() { throw std::runtime_error{"EXCEPTION TO BREAK LOOP FOR TESTS"}; });

    EXPECT_THROW(application_init(), std::runtime_error);

    EXPECT_TRUE(systemHandle.functionGotCalled<system_pre_init>(std::ignore));
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_init>());
    EXPECT_TRUE(imuHandle.functionGotCalled<imu_init>());
    EXPECT_TRUE(remoteHandlerHandle.functionGotCalled<remote_init>());
    EXPECT_TRUE(flightComputerHandle.functionGotCalled<flight_computer_init>());
    EXPECT_TRUE(actuatorsHandle.functionGotCalled<actuators_init>());
    EXPECT_TRUE(modeHandlerHandle.functionGotCalled<mode_handler_init>());
    EXPECT_TRUE(systemHandle.functionGotCalled<system_post_init>());

    // Actual test
    modeHandlerHandle.overrideFunc<mode_handler_handle>(
            [](imu_data_t *imu_data, remote_data_t *remote_data, flight_computer_set_point_t *flightcomputer_setpoint) {
                imu_data->heading_mul_16 = 17;
                imu_data->pitch_mul_16 = 18;
                imu_data->roll_mul_16 = 19;
                imu_data->d_heading_mul_16 = 20;
                imu_data->d_pitch_mul_16 = 21;
                imu_data->d_roll_mul_16 = 22;
                imu_data->acc_x_mul_100 = 23;
                imu_data->acc_y_mul_100 = 24;
                imu_data->acc_z_mul_100 = 25;
                imu_data->imu_ok = true;
                remote_data->throttle_mixed = 26;
                remote_data->elevon_left_mixed = 27;
                remote_data->elevon_right_mixed = 28;
                remote_data->remote_ok = true;
                remote_data->override_active = false;
                remote_data->is_armed = true;
                flightcomputer_setpoint->roll = 29;
                flightcomputer_setpoint->pitch = 30;
                flightcomputer_setpoint->motor = 31;
                return MODE_FAILSAFE;
            });


    actuatorsHandle.overrideFunc<actuators_set>([](const actuator_cmd_t *actuator_cmd) {
        EXPECT_EQ(actuator_cmd->elevon_left, 0);
        EXPECT_EQ(actuator_cmd->elevon_right, 0);
        EXPECT_EQ(actuator_cmd->motor, 0);
    });
    timer_callback();
    EXPECT_TRUE(modeHandlerHandle.functionGotCalled<mode_handler_handle>());
    EXPECT_TRUE(actuatorsHandle.functionGotCalled<actuators_set>());
    EXPECT_TRUE(imuHandle.functionGotCalled<imu_start_sampling>());
}

TEST(TEST_NAME, timer_send_period) {
    auto actuatorsHandle = mock::actuators.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();
    auto flightComputerHandle = mock::flight_computer.getHandle();
    auto imuHandle = mock::imu.getHandle();
    auto modeHandlerHandle = mock::mode_handler.getHandle();
    auto remoteHandlerHandle = mock::remote.getHandle();
    auto systemHandle = mock::system.getHandle();
    auto controllerHandle = mock::controller.getHandle();

    system_timer_16_384ms_callback timer_callback = nullptr;
    systemHandle.overrideFunc<system_pre_init>(
            [&timer_callback](system_timer_16_384ms_callback callback) { timer_callback = callback; });
    systemHandle.overrideFunc<system_reset_watchdog>(
            []() { throw std::runtime_error{"EXCEPTION TO BREAK LOOP FOR TESTS"}; });

    EXPECT_THROW(application_init(), std::runtime_error);

    EXPECT_TRUE(systemHandle.functionGotCalled<system_pre_init>(std::ignore));
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_init>());
    EXPECT_TRUE(imuHandle.functionGotCalled<imu_init>());
    EXPECT_TRUE(remoteHandlerHandle.functionGotCalled<remote_init>());
    EXPECT_TRUE(flightComputerHandle.functionGotCalled<flight_computer_init>());
    EXPECT_TRUE(actuatorsHandle.functionGotCalled<actuators_init>());
    EXPECT_TRUE(modeHandlerHandle.functionGotCalled<mode_handler_init>());
    EXPECT_TRUE(systemHandle.functionGotCalled<system_post_init>());

    // Actual test
    modeHandlerHandle.overrideFunc<mode_handler_handle>(
            [](imu_data_t *imu_data, remote_data_t *remote_data, flight_computer_set_point_t *flightcomputer_setpoint) {
                imu_data->heading_mul_16 = 17;
                imu_data->pitch_mul_16 = 18;
                imu_data->roll_mul_16 = 19;
                imu_data->d_heading_mul_16 = 20;
                imu_data->d_pitch_mul_16 = 21;
                imu_data->d_roll_mul_16 = 22;
                imu_data->acc_x_mul_100 = 23;
                imu_data->acc_y_mul_100 = 24;
                imu_data->acc_z_mul_100 = 25;
                imu_data->imu_ok = true;
                remote_data->throttle_mixed = 26;
                remote_data->elevon_left_mixed = 27;
                remote_data->elevon_right_mixed = 28;
                remote_data->remote_ok = true;
                remote_data->override_active = false;
                remote_data->is_armed = true;
                flightcomputer_setpoint->roll = 29;
                flightcomputer_setpoint->pitch = 30;
                flightcomputer_setpoint->motor = 31;
                return MODE_FLIGHTCOMPUTER;
            });

    controllerHandle.overrideFunc<controller_update>(
            [](const imu_data_t *imu_data, int16_t roll_setpoint, int16_t pitch_setpoint) {
                EXPECT_EQ(imu_data->heading_mul_16, 17);
                EXPECT_EQ(imu_data->pitch_mul_16, 18);
                EXPECT_EQ(imu_data->roll_mul_16, 19);
                EXPECT_EQ(imu_data->d_heading_mul_16, 20);
                EXPECT_EQ(imu_data->d_pitch_mul_16, 21);
                EXPECT_EQ(imu_data->d_roll_mul_16, 22);
                EXPECT_EQ(imu_data->acc_x_mul_100, 23);
                EXPECT_EQ(imu_data->acc_y_mul_100, 24);
                EXPECT_EQ(imu_data->acc_z_mul_100, 25);
                EXPECT_EQ(imu_data->imu_ok, true);
                EXPECT_EQ(roll_setpoint, 29);
                EXPECT_EQ(pitch_setpoint, 30);
                return controller_result_t{.elevon_left = 32, .elevon_right = 33};
            });

    actuatorsHandle.overrideFunc<actuators_set>([](const actuator_cmd_t *actuator_cmd) {
        EXPECT_EQ(actuator_cmd->elevon_left, 32);
        EXPECT_EQ(actuator_cmd->elevon_right, 33);
        EXPECT_EQ(actuator_cmd->motor, 31);
    });

    flightComputerHandle.overrideFunc<flight_computer_send>(
            [](const imu_data_t *imu_data, const remote_data_t *remote_data, const actuator_cmd_t *actuator_cmd) {
                EXPECT_EQ(imu_data->heading_mul_16, 17);
                EXPECT_EQ(imu_data->pitch_mul_16, 18);
                EXPECT_EQ(imu_data->roll_mul_16, 19);
                EXPECT_EQ(imu_data->d_heading_mul_16, 20);
                EXPECT_EQ(imu_data->d_pitch_mul_16, 21);
                EXPECT_EQ(imu_data->d_roll_mul_16, 22);
                EXPECT_EQ(imu_data->acc_x_mul_100, 23);
                EXPECT_EQ(imu_data->acc_y_mul_100, 24);
                EXPECT_EQ(imu_data->acc_z_mul_100, 25);
                EXPECT_EQ(imu_data->imu_ok, true);

                EXPECT_EQ(remote_data->throttle_mixed, 26);
                EXPECT_EQ(remote_data->elevon_left_mixed, 27);
                EXPECT_EQ(remote_data->elevon_right_mixed, 28);
                EXPECT_EQ(remote_data->remote_ok, true);
                EXPECT_EQ(remote_data->override_active, false);
                EXPECT_EQ(remote_data->is_armed, true);

                EXPECT_EQ(actuator_cmd->elevon_left, 32);
                EXPECT_EQ(actuator_cmd->elevon_right, 33);
                EXPECT_EQ(actuator_cmd->motor, 31);
            });

    for (auto c = 0; c < 5; ++c) {
        timer_callback();
        EXPECT_TRUE(modeHandlerHandle.functionGotCalled<mode_handler_handle>());
        EXPECT_TRUE(controllerHandle.functionGotCalled<controller_update>());
        EXPECT_TRUE(actuatorsHandle.functionGotCalled<actuators_set>());
        EXPECT_FALSE(flightComputerHandle.functionGotCalled<flight_computer_send>());
        EXPECT_TRUE(imuHandle.functionGotCalled<imu_start_sampling>());
    }

    timer_callback();
    EXPECT_TRUE(modeHandlerHandle.functionGotCalled<mode_handler_handle>());
    EXPECT_TRUE(controllerHandle.functionGotCalled<controller_update>());
    EXPECT_TRUE(actuatorsHandle.functionGotCalled<actuators_set>());
    EXPECT_TRUE(flightComputerHandle.functionGotCalled<flight_computer_send>());
    EXPECT_TRUE(imuHandle.functionGotCalled<imu_start_sampling>());

    timer_callback();
    EXPECT_TRUE(modeHandlerHandle.functionGotCalled<mode_handler_handle>());
    EXPECT_TRUE(controllerHandle.functionGotCalled<controller_update>());
    EXPECT_TRUE(actuatorsHandle.functionGotCalled<actuators_set>());
    EXPECT_FALSE(flightComputerHandle.functionGotCalled<flight_computer_send>());
    EXPECT_TRUE(imuHandle.functionGotCalled<imu_start_sampling>());
}
