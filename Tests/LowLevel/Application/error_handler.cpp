#include <Mock/io.hpp>
#include <Mock/system.hpp>
#include <gtest/gtest.h>

extern "C" {
#include <Application/error_handler.h>
}

TEST(TEST_NAME, init) {
    error_handler_init();
    EXPECT_EQ(DDRL, 0xFF);
    EXPECT_EQ(PORTL, 0x00);
}

TEST(TEST_NAME, handle_warning) {
    error_handler_handle_warning(static_cast<error_group_t>(6), 11);
    EXPECT_EQ(PORTL, 16 * 11 + 6);
}

TEST(TEST_NAME, handle_error) {
    auto systemHandle = mock::system.getHandle();
    std::size_t count = 0;
    systemHandle.overrideFunc<system_reset_watchdog>([&count]() {
        EXPECT_EQ(PORTL, 16 * 11 + 6);
        count += 1;
        if (count >= 10) { // This solves the halting problem...
            throw std::runtime_error{"EXCEPTION TO BREAK LOOP FOR TESTS"};
        }
    });
    EXPECT_THROW(error_handler_handle_error(static_cast<error_group_t>(6), 11), std::runtime_error);
}
