#include <Mock/Application/error_handler.hpp>
#include <Mock/HAL/timer8bit.hpp>
#include <Mock/avr/interrupt.hpp>
#include <Mock/avr/io.hpp>
#include <Mock/avr/wdt.hpp>
#include <gtest/gtest.h>

extern "C" {
#include <Components/system.h>
}

bool timer_got_called = false;
void timer_callback(void) {
    timer_got_called = true;
}

TEST(TEST_NAME, pre_init_ok) {
    auto interruptHandle = mock::interrupt.getHandle();
    auto wdtHandle = mock::wdt.getHandle();

    system_pre_init(timer_callback);

    EXPECT_TRUE(interruptHandle.functionGotCalled<cli>());
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_250MS));
    EXPECT_TRUE(interruptHandle.functionGotCalled<sei>());
    EXPECT_EQ(MCUSR, 0);
}

TEST(TEST_NAME, pre_init_watchdog) {
    auto interruptHandle = mock::interrupt.getHandle();
    auto wdtHandle = mock::wdt.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    MCUSR |= (1U << WDRF);

    system_pre_init(timer_callback);
    EXPECT_TRUE(interruptHandle.functionGotCalled<cli>());
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_250MS));
    EXPECT_TRUE(interruptHandle.functionGotCalled<sei>());
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(SYSTEM, SYSTEM_ERROR_WATCHDOG));
}

TEST(TEST_NAME, pre_init_brownout) {
    auto interruptHandle = mock::interrupt.getHandle();
    auto wdtHandle = mock::wdt.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    MCUSR |= (1U << BORF);

    system_pre_init(timer_callback);
    EXPECT_TRUE(interruptHandle.functionGotCalled<cli>());
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_250MS));
    EXPECT_TRUE(interruptHandle.functionGotCalled<sei>());
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(SYSTEM, SYSTEM_ERROR_BROWNOUT));
}

TEST(TEST_NAME, post_init) {
    auto interruptHandle = mock::interrupt.getHandle();
    auto wdtHandle = mock::wdt.getHandle();
    auto timerHandle = mock::timer8bit.getHandle();

    system_pre_init(timer_callback);
    EXPECT_TRUE(interruptHandle.functionGotCalled<cli>());
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_250MS));
    EXPECT_TRUE(interruptHandle.functionGotCalled<sei>());

    system_post_init();

    EXPECT_TRUE(timerHandle.functionGotCalled<timer_8bit_init>(prescaler_1024, std::ignore));
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_30MS));
}

TEST(TEST_NAME, timer_runtime_0) {
    auto interruptHandle = mock::interrupt.getHandle();
    auto wdtHandle = mock::wdt.getHandle();
    auto timerHandle = mock::timer8bit.getHandle();

    timer_8bit_callback_t internalCallback = nullptr;
    timerHandle.overrideFunc<timer_8bit_init>(
            [&internalCallback](timer_8bit_clock_option_t /*clock_option*/, system_timer_callback_t timerCallback) {
                internalCallback = timerCallback;
            });
    timerHandle.overrideFunc<timer_8bit_get_count>([]() { return 0; });

    system_pre_init(timer_callback);
    EXPECT_TRUE(interruptHandle.functionGotCalled<cli>());
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_250MS));
    EXPECT_TRUE(interruptHandle.functionGotCalled<sei>());

    system_post_init();
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_30MS));

    timer_got_called = false;
    internalCallback();
    EXPECT_TRUE(timer_got_called);
    EXPECT_TRUE(timerHandle.functionGotCalled<timer_8bit_get_count>());
}

TEST(TEST_NAME, timer_runtime_max) {
    auto interruptHandle = mock::interrupt.getHandle();
    auto wdtHandle = mock::wdt.getHandle();
    auto timerHandle = mock::timer8bit.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    timer_8bit_callback_t internalCallback = nullptr;
    timerHandle.overrideFunc<timer_8bit_init>(
            [&internalCallback](timer_8bit_clock_option_t /*clock_option*/, system_timer_callback_t timerCallback) {
                internalCallback = timerCallback;
            });
    timerHandle.overrideFunc<timer_8bit_get_count>([]() { return 255; });

    system_pre_init(timer_callback);
    EXPECT_TRUE(interruptHandle.functionGotCalled<cli>());
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_250MS));
    EXPECT_TRUE(interruptHandle.functionGotCalled<sei>());

    system_post_init();
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_30MS));

    timer_got_called = false;
    internalCallback();
    EXPECT_TRUE(timer_got_called);
    EXPECT_TRUE(timerHandle.functionGotCalled<timer_8bit_get_count>());
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(SYSTEM, SYSTEM_ERROR_TIMER_RUNTIME));
}

TEST(TEST_NAME, timer_runtime_under_limit) {
    auto interruptHandle = mock::interrupt.getHandle();
    auto wdtHandle = mock::wdt.getHandle();
    auto timerHandle = mock::timer8bit.getHandle();

    timer_8bit_callback_t internalCallback = nullptr;
    timerHandle.overrideFunc<timer_8bit_init>(
            [&internalCallback](timer_8bit_clock_option_t /*clock_option*/, system_timer_callback_t timerCallback) {
                internalCallback = timerCallback;
            });
    // 12/16,384*255=186.767578125
    timerHandle.overrideFunc<timer_8bit_get_count>([]() { return 186; });

    system_pre_init(timer_callback);
    EXPECT_TRUE(interruptHandle.functionGotCalled<cli>());
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_250MS));
    EXPECT_TRUE(interruptHandle.functionGotCalled<sei>());

    system_post_init();
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_30MS));

    timer_got_called = false;
    internalCallback();
    EXPECT_TRUE(timer_got_called);
    EXPECT_TRUE(timerHandle.functionGotCalled<timer_8bit_get_count>());
}

TEST(TEST_NAME, timer_runtime_over_limit) {
    auto interruptHandle = mock::interrupt.getHandle();
    auto wdtHandle = mock::wdt.getHandle();
    auto timerHandle = mock::timer8bit.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    timer_8bit_callback_t internalCallback = nullptr;
    timerHandle.overrideFunc<timer_8bit_init>(
            [&internalCallback](timer_8bit_clock_option_t /*clock_option*/, system_timer_callback_t timerCallback) {
                internalCallback = timerCallback;
            });
    // 12/16,384*255=186.767578125
    timerHandle.overrideFunc<timer_8bit_get_count>([]() { return 187; });

    system_pre_init(timer_callback);
    EXPECT_TRUE(interruptHandle.functionGotCalled<cli>());
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_250MS));
    EXPECT_TRUE(interruptHandle.functionGotCalled<sei>());

    system_post_init();
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_30MS));

    timer_got_called = false;
    internalCallback();
    EXPECT_TRUE(timer_got_called);
    EXPECT_TRUE(timerHandle.functionGotCalled<timer_8bit_get_count>());
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(SYSTEM, SYSTEM_ERROR_TIMER_RUNTIME));
}

TEST(TEST_NAME, timer_reset_watchdog) {
    auto wdtHandle = mock::wdt.getHandle();
    system_reset_watchdog();
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_reset>());
}
