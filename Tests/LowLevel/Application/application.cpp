#include <Mock/actuators.hpp>
#include <Mock/error_handler.hpp>
#include <Mock/flightcomputer.hpp>
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
    auto flighcomputerHandle = mock::flightcomputer.getHandle();
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
    EXPECT_TRUE(flighcomputerHandle.functionGotCalled<flightcomputer_init>());
    EXPECT_TRUE(actuatorsHandle.functionGotCalled<actuators_init>());
    EXPECT_TRUE(modeHandlerHandle.functionGotCalled<mode_handler_init>());
    EXPECT_TRUE(systemHandle.functionGotCalled<system_post_init>());
}

TEST(TEST_NAME, timer) {
    auto actuatorsHandle = mock::actuators.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();
    auto flighcomputerHandle = mock::flightcomputer.getHandle();
    auto imuHandle = mock::imu.getHandle();
    auto modeHandlerHandle = mock::mode_handler.getHandle();
    auto remoteHandlerHandle = mock::remote.getHandle();
    auto systemHandle = mock::system.getHandle();

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
    EXPECT_TRUE(flighcomputerHandle.functionGotCalled<flightcomputer_init>());
    EXPECT_TRUE(actuatorsHandle.functionGotCalled<actuators_init>());
    EXPECT_TRUE(modeHandlerHandle.functionGotCalled<mode_handler_init>());
    EXPECT_TRUE(systemHandle.functionGotCalled<system_post_init>());

    // Actual test
}
