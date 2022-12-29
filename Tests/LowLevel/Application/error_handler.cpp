#include <Mock/avr/io.hpp>
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
