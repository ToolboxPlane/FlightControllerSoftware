#include <Mock/error_handler.hpp>
#include <Mock/interrupt.hpp>
#include <Mock/io.hpp>
#include <Mock/timer8bit.hpp>
#include <Mock/wdt.hpp>
#include <gtest/gtest.h>

extern "C" {
#include <Components/system.h>
}

bool high_prio_got_called = false;
void high_prio_callback(void) {
    high_prio_got_called = true;
}

bool low_prio_got_called = false;
void low_prio_callback(void) {
    low_prio_got_called = true;
}

TEST(TEST_NAME, pre_init__ok) {
    auto interruptHandle = mock::interrupt.getHandle();
    auto wdtHandle = mock::wdt.getHandle();

    system_pre_init(low_prio_callback, high_prio_callback);

    EXPECT_TRUE(interruptHandle.functionGotCalled<cli>());
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_250MS));
    EXPECT_TRUE(interruptHandle.functionGotCalled<sei>());
    EXPECT_EQ(MCUSR, 0);
}

TEST(TEST_NAME, pre_init__watchdog) {
    auto interruptHandle = mock::interrupt.getHandle();
    auto wdtHandle = mock::wdt.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    MCUSR |= (1U << WDRF);

    system_pre_init(low_prio_callback, high_prio_callback);
    EXPECT_TRUE(interruptHandle.functionGotCalled<cli>());
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_250MS));
    EXPECT_TRUE(interruptHandle.functionGotCalled<sei>());
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_SYSTEM,
                                                                                   SYSTEM_ERROR_WATCHDOG));
}

TEST(TEST_NAME, pre_init__brownout) {
    auto interruptHandle = mock::interrupt.getHandle();
    auto wdtHandle = mock::wdt.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    MCUSR |= (1U << BORF);

    system_pre_init(low_prio_callback, high_prio_callback);
    EXPECT_TRUE(interruptHandle.functionGotCalled<cli>());
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_250MS));
    EXPECT_TRUE(interruptHandle.functionGotCalled<sei>());
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_SYSTEM,
                                                                                   SYSTEM_ERROR_BROWNOUT));
}

TEST(TEST_NAME, post_init) {
    auto interruptHandle = mock::interrupt.getHandle();
    auto wdtHandle = mock::wdt.getHandle();
    auto timerHandle = mock::timer8bit.getHandle();

    system_pre_init(low_prio_callback, high_prio_callback);
    EXPECT_TRUE(interruptHandle.functionGotCalled<cli>());
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_250MS));
    EXPECT_TRUE(interruptHandle.functionGotCalled<sei>());

    system_post_init();

    EXPECT_TRUE(timerHandle.functionGotCalled<timer_8bit_init>(TIMER_ID_0, prescaler_1024, std::ignore));
    EXPECT_TRUE(timerHandle.functionGotCalled<timer_8bit_init>(TIMER_ID_2, prescaler_256, std::ignore));
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_30MS));
}

TEST(TEST_NAME, timer_low_prio_callback) {
    auto interruptHandle = mock::interrupt.getHandle();
    auto wdtHandle = mock::wdt.getHandle();
    auto timerHandle = mock::timer8bit.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    timer_8bit_callback_t internalCallback = nullptr;
    timerHandle.overrideFunc<timer_8bit_init>([&internalCallback](timer_8bit_id_t timerId,
                                                                  timer_8bit_clock_option_t /*clock_option*/,
                                                                  timer_8bit_callback_t timerCallback) {
        if (timerId == TIMER_ID_0) {
            internalCallback = timerCallback;
        }
    });
    timerHandle.overrideFunc<timer_8bit_get_count>([](timer_8bit_id_t /*timerId*/) { return 0; });

    system_pre_init(low_prio_callback, high_prio_callback);
    EXPECT_TRUE(interruptHandle.functionGotCalled<cli>());
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_250MS));
    EXPECT_TRUE(interruptHandle.functionGotCalled<sei>());

    system_post_init();
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_30MS));

    low_prio_got_called = false;
    internalCallback();
    EXPECT_TRUE(low_prio_got_called);
    EXPECT_TRUE(SREG & (1 << SREG_I));
}

TEST(TEST_NAME, timer_low_prio_runtime_0) {
    auto interruptHandle = mock::interrupt.getHandle();
    auto wdtHandle = mock::wdt.getHandle();
    auto timerHandle = mock::timer8bit.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    timer_8bit_callback_t internalCallback = nullptr;
    timerHandle.overrideFunc<timer_8bit_init>([&internalCallback](timer_8bit_id_t timerId,
                                                                  timer_8bit_clock_option_t /*clock_option*/,
                                                                  timer_8bit_callback_t timerCallback) {
        if (timerId == TIMER_ID_0) {
            internalCallback = timerCallback;
        }
    });
    timerHandle.overrideFunc<timer_8bit_get_count>([](timer_8bit_id_t /*timerId*/) { return 0; });

    system_pre_init(low_prio_callback, high_prio_callback);
    EXPECT_TRUE(interruptHandle.functionGotCalled<cli>());
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_250MS));
    EXPECT_TRUE(interruptHandle.functionGotCalled<sei>());

    system_post_init();
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_30MS));

    internalCallback();
}

TEST(TEST_NAME, timer_low_prio_runtime_max) {
    auto interruptHandle = mock::interrupt.getHandle();
    auto wdtHandle = mock::wdt.getHandle();
    auto timerHandle = mock::timer8bit.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    timer_8bit_callback_t internalCallback = nullptr;
    timerHandle.overrideFunc<timer_8bit_init>([&internalCallback](timer_8bit_id_t timerId,
                                                                  timer_8bit_clock_option_t /*clock_option*/,
                                                                  timer_8bit_callback_t timerCallback) {
        if (timerId == TIMER_ID_0) {
            internalCallback = timerCallback;
        }
    });
    timerHandle.overrideFunc<timer_8bit_get_count>([](timer_8bit_id_t /*timerId*/) { return 255; });

    system_pre_init(low_prio_callback, high_prio_callback);
    EXPECT_TRUE(interruptHandle.functionGotCalled<cli>());
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_250MS));
    EXPECT_TRUE(interruptHandle.functionGotCalled<sei>());

    system_post_init();
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_30MS));

    internalCallback();
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_SYSTEM,
                                                                                   SYSTEM_ERROR_RUNTIME_LOW_PRIO));
}

TEST(TEST_NAME, timer_low_prio_runtime_under_limit) {
    auto interruptHandle = mock::interrupt.getHandle();
    auto wdtHandle = mock::wdt.getHandle();
    auto timerHandle = mock::timer8bit.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    timer_8bit_callback_t internalCallback = nullptr;
    timerHandle.overrideFunc<timer_8bit_init>([&internalCallback](timer_8bit_id_t timerId,
                                                                  timer_8bit_clock_option_t /*clock_option*/,
                                                                  timer_8bit_callback_t timerCallback) {
        if (timerId == TIMER_ID_0) {
            internalCallback = timerCallback;
        }
    });

    // 12/16,384*255=186.767578125
    timerHandle.overrideFunc<timer_8bit_get_count>([](timer_8bit_id_t /*timerId*/) { return 186; });

    system_pre_init(low_prio_callback, high_prio_callback);
    EXPECT_TRUE(interruptHandle.functionGotCalled<cli>());
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_250MS));
    EXPECT_TRUE(interruptHandle.functionGotCalled<sei>());

    system_post_init();
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_30MS));

    internalCallback();
}

TEST(TEST_NAME, timer_low_prio_runtime_over_limit) {
    auto interruptHandle = mock::interrupt.getHandle();
    auto wdtHandle = mock::wdt.getHandle();
    auto timerHandle = mock::timer8bit.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    timer_8bit_callback_t internalCallback = nullptr;
    timerHandle.overrideFunc<timer_8bit_init>([&internalCallback](timer_8bit_id_t timerId,
                                                                  timer_8bit_clock_option_t /*clock_option*/,
                                                                  timer_8bit_callback_t timerCallback) {
        if (timerId == TIMER_ID_0) {
            internalCallback = timerCallback;
        }
    });

    // 12/16,384*255=186.767578125
    timerHandle.overrideFunc<timer_8bit_get_count>([](timer_8bit_id_t /*timerId*/) { return 187; });

    system_pre_init(low_prio_callback, high_prio_callback);
    EXPECT_TRUE(interruptHandle.functionGotCalled<cli>());
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_250MS));
    EXPECT_TRUE(interruptHandle.functionGotCalled<sei>());

    system_post_init();
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_30MS));

    internalCallback();
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_SYSTEM,
                                                                                   SYSTEM_ERROR_RUNTIME_LOW_PRIO));
}

TEST(TEST_NAME, timer_high_prio_callback) {
    auto interruptHandle = mock::interrupt.getHandle();
    auto wdtHandle = mock::wdt.getHandle();
    auto timerHandle = mock::timer8bit.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    timer_8bit_callback_t internalCallback = nullptr;
    timerHandle.overrideFunc<timer_8bit_init>([&internalCallback](timer_8bit_id_t timerId,
                                                                  timer_8bit_clock_option_t /*clock_option*/,
                                                                  timer_8bit_callback_t timerCallback) {
        if (timerId == TIMER_ID_2) {
            internalCallback = timerCallback;
        }
    });
    timerHandle.overrideFunc<timer_8bit_get_count>([](timer_8bit_id_t /*timerId*/) { return 0; });

    system_pre_init(high_prio_callback, high_prio_callback);
    EXPECT_TRUE(interruptHandle.functionGotCalled<cli>());
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_250MS));
    EXPECT_TRUE(interruptHandle.functionGotCalled<sei>());

    system_post_init();
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_30MS));

    high_prio_got_called = false;
    internalCallback();
    EXPECT_TRUE(high_prio_got_called);
}

TEST(TEST_NAME, timer_high_prio_runtime_0) {
    auto interruptHandle = mock::interrupt.getHandle();
    auto wdtHandle = mock::wdt.getHandle();
    auto timerHandle = mock::timer8bit.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    timer_8bit_callback_t internalCallback = nullptr;
    timerHandle.overrideFunc<timer_8bit_init>([&internalCallback](timer_8bit_id_t timerId,
                                                                  timer_8bit_clock_option_t /*clock_option*/,
                                                                  timer_8bit_callback_t timerCallback) {
        if (timerId == TIMER_ID_2) {
            internalCallback = timerCallback;
        }
    });
    timerHandle.overrideFunc<timer_8bit_get_count>([](timer_8bit_id_t /*timerId*/) { return 0; });

    system_pre_init(high_prio_callback, high_prio_callback);
    EXPECT_TRUE(interruptHandle.functionGotCalled<cli>());
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_250MS));
    EXPECT_TRUE(interruptHandle.functionGotCalled<sei>());

    system_post_init();
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_30MS));

    internalCallback();
}

TEST(TEST_NAME, timer_high_prio_runtime_max) {
    auto interruptHandle = mock::interrupt.getHandle();
    auto wdtHandle = mock::wdt.getHandle();
    auto timerHandle = mock::timer8bit.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    timer_8bit_callback_t internalCallback = nullptr;
    timerHandle.overrideFunc<timer_8bit_init>([&internalCallback](timer_8bit_id_t timerId,
                                                                  timer_8bit_clock_option_t /*clock_option*/,
                                                                  timer_8bit_callback_t timerCallback) {
        if (timerId == TIMER_ID_2) {
            internalCallback = timerCallback;
        }
    });
    timerHandle.overrideFunc<timer_8bit_get_count>([](timer_8bit_id_t /*timerId*/) { return 255; });

    system_pre_init(high_prio_callback, high_prio_callback);
    EXPECT_TRUE(interruptHandle.functionGotCalled<cli>());
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_250MS));
    EXPECT_TRUE(interruptHandle.functionGotCalled<sei>());

    system_post_init();
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_30MS));

    internalCallback();
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_SYSTEM,
                                                                                   SYSTEM_ERROR_RUNTIME_HIGH_PRIO));
}

TEST(TEST_NAME, timer_high_prio_runtime_under_limit) {
    auto interruptHandle = mock::interrupt.getHandle();
    auto wdtHandle = mock::wdt.getHandle();
    auto timerHandle = mock::timer8bit.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    timer_8bit_callback_t internalCallback = nullptr;
    timerHandle.overrideFunc<timer_8bit_init>([&internalCallback](timer_8bit_id_t timerId,
                                                                  timer_8bit_clock_option_t /*clock_option*/,
                                                                  timer_8bit_callback_t timerCallback) {
        if (timerId == TIMER_ID_2) {
            internalCallback = timerCallback;
        }
    });

    // 1/4.096*255=62.255
    timerHandle.overrideFunc<timer_8bit_get_count>([](timer_8bit_id_t /*timerId*/) { return 62; });

    system_pre_init(high_prio_callback, high_prio_callback);
    EXPECT_TRUE(interruptHandle.functionGotCalled<cli>());
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_250MS));
    EXPECT_TRUE(interruptHandle.functionGotCalled<sei>());

    system_post_init();
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_30MS));

    internalCallback();
}

TEST(TEST_NAME, timer_high_prio_runtime_over_limit) {
    auto interruptHandle = mock::interrupt.getHandle();
    auto wdtHandle = mock::wdt.getHandle();
    auto timerHandle = mock::timer8bit.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    timer_8bit_callback_t internalCallback = nullptr;
    timerHandle.overrideFunc<timer_8bit_init>([&internalCallback](timer_8bit_id_t timerId,
                                                                  timer_8bit_clock_option_t /*clock_option*/,
                                                                  timer_8bit_callback_t timerCallback) {
        if (timerId == TIMER_ID_2) {
            internalCallback = timerCallback;
        }
    });

    // 1/4.096*255=62.255
    timerHandle.overrideFunc<timer_8bit_get_count>([](timer_8bit_id_t /*timerId*/) { return 63; });

    system_pre_init(high_prio_callback, high_prio_callback);
    EXPECT_TRUE(interruptHandle.functionGotCalled<cli>());
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_250MS));
    EXPECT_TRUE(interruptHandle.functionGotCalled<sei>());

    system_post_init();
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_enable>(WDTO_30MS));

    internalCallback();
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_SYSTEM,
                                                                                   SYSTEM_ERROR_RUNTIME_HIGH_PRIO));
}

TEST(TEST_NAME, reset_watchdog) {
    auto wdtHandle = mock::wdt.getHandle();
    system_reset_watchdog();
    EXPECT_TRUE(wdtHandle.functionGotCalled<wdt_reset>());
}
