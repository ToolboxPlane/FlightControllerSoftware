#include <Mock/bno055.hpp>
#include <Mock/delay.hpp>
#include <Mock/error_handler.hpp>
#include <gtest/gtest.h>

extern "C" {
#include <Components/imu.h>
}

TEST(TEST_NAME, init__config_mode_error) {
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno055_callback_t callback) { callback(write_fail); });

    imu_init();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(ERROR_HANDLER_GROUP_BNO055,
                                                                                 write_fail + 1));
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
    EXPECT_FALSE(imu_data_available());
}

TEST(TEST_NAME, init__config_mode_timeout) {
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    imu_init();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_write_opr_mode>(BNO055_OPR_MODE_CONFIG_MODE, std::ignore));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(ERROR_HANDLER_GROUP_IMU,
                                                                                 IMU_ERROR_INIT_TIMEOUT));
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
    EXPECT_FALSE(imu_data_available());
}

TEST(TEST_NAME, init__self_test_error) {
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](bno055_self_test_result_t *out, bno055_callback_t callback) {
        out->mcu_passed = true;
        out->acc_passed = true;
        out->gyr_passed = true;
        out->mag_passed = true;
        callback(read_fail);
    });

    imu_init();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(ERROR_HANDLER_GROUP_BNO055,
                                                                                 read_fail + 1));
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
    EXPECT_FALSE(imu_data_available());
}

TEST(TEST_NAME, init__self_test_mcu_fail) {
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](bno055_self_test_result_t *out, bno055_callback_t callback) {
        out->mcu_passed = false;
        out->acc_passed = true;
        out->gyr_passed = true;
        out->mag_passed = true;
        callback(read_success);
    });

    imu_init();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(ERROR_HANDLER_GROUP_IMU,
                                                                                 IMU_ERROR_INIT_SELF_TEST));
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
    EXPECT_FALSE(imu_data_available());
}

TEST(TEST_NAME, init__self_test_acc_fail) {
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](bno055_self_test_result_t *out, bno055_callback_t callback) {
        out->mcu_passed = true;
        out->acc_passed = false;
        out->gyr_passed = true;
        out->mag_passed = true;
        callback(read_success);
    });

    imu_init();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(ERROR_HANDLER_GROUP_IMU,
                                                                                 IMU_ERROR_INIT_SELF_TEST));
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
    EXPECT_FALSE(imu_data_available());
}

TEST(TEST_NAME, init__self_test_gyr_fail) {
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](bno055_self_test_result_t *out, bno055_callback_t callback) {
        out->mcu_passed = true;
        out->acc_passed = true;
        out->gyr_passed = false;
        out->mag_passed = true;
        callback(read_success);
    });

    imu_init();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(ERROR_HANDLER_GROUP_IMU,
                                                                                 IMU_ERROR_INIT_SELF_TEST));
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
    EXPECT_FALSE(imu_data_available());
}

TEST(TEST_NAME, init__self_test_mag_fail) {
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](bno055_self_test_result_t *out, bno055_callback_t callback) {
        out->mcu_passed = true;
        out->acc_passed = true;
        out->gyr_passed = true;
        out->mag_passed = false;
        callback(read_success);
    });

    imu_init();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(ERROR_HANDLER_GROUP_IMU,
                                                                                 IMU_ERROR_INIT_SELF_TEST));
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
    EXPECT_FALSE(imu_data_available());
}

TEST(TEST_NAME, init__self_test_timeout) {
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno055_callback_t callback) { callback(write_success); });

    imu_init();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_self_test>());
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(ERROR_HANDLER_GROUP_IMU,
                                                                                 IMU_ERROR_INIT_TIMEOUT));
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
    EXPECT_FALSE(imu_data_available());
}

TEST(TEST_NAME, init__unit_sel_error) {
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](bno055_self_test_result_t *out, bno055_callback_t callback) {
        out->mcu_passed = true;
        out->acc_passed = true;
        out->gyr_passed = true;
        out->mag_passed = true;
        callback(read_success);
    });
    bnoHandle.overrideFunc<bno055_write_unit_selection>(
            [](bno055_unit_sel_acc /*acc_unit*/, bno055_unit_sel_angular_rate /*angular_rate_unit*/,
               bno055_unit_sel_euler_angles /*euler_angles_unit*/, bno055_unit_sel_temperature /*temperature_unit*/,
               bno055_unit_sel_orientation_def /*orientation_def*/,
               bno055_callback_t callback) { callback(write_fail); });

    imu_init();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(ERROR_HANDLER_GROUP_BNO055,
                                                                                 write_fail + 1));
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
    EXPECT_FALSE(imu_data_available());
}

TEST(TEST_NAME, init__unit_sel_timeout) {
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](bno055_self_test_result_t *out, bno055_callback_t callback) {
        out->mcu_passed = true;
        out->acc_passed = true;
        out->gyr_passed = true;
        out->mag_passed = true;
        callback(read_success);
    });

    imu_init();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_write_unit_selection>());
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(ERROR_HANDLER_GROUP_IMU,
                                                                                 IMU_ERROR_INIT_TIMEOUT));
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
    EXPECT_FALSE(imu_data_available());
}

TEST(TEST_NAME, init__remap_error) {
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](bno055_self_test_result_t *out, bno055_callback_t callback) {
        out->mcu_passed = true;
        out->acc_passed = true;
        out->gyr_passed = true;
        out->mag_passed = true;
        callback(read_success);
    });
    bnoHandle.overrideFunc<bno055_write_unit_selection>(
            [](bno055_unit_sel_acc /*acc_unit*/, bno055_unit_sel_angular_rate /*angular_rate_unit*/,
               bno055_unit_sel_euler_angles /*euler_angles_unit*/, bno055_unit_sel_temperature /*temperature_unit*/,
               bno055_unit_sel_orientation_def /*orientation_def*/,
               bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis>(
            [](bno055_axis_remap_axis_t /*new_x*/, bno055_axis_remap_axis_t /*new_y*/,
               bno055_axis_remap_axis_t /*new_z*/, bno055_callback_t callback) { callback(write_fail); });

    imu_init();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(ERROR_HANDLER_GROUP_BNO055,
                                                                                 write_fail + 1));
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
    EXPECT_FALSE(imu_data_available());
}

TEST(TEST_NAME, init__remap_timeout) {
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](bno055_self_test_result_t *out, bno055_callback_t callback) {
        out->mcu_passed = true;
        out->acc_passed = true;
        out->gyr_passed = true;
        out->mag_passed = true;
        callback(read_success);
    });
    bnoHandle.overrideFunc<bno055_write_unit_selection>(
            [](bno055_unit_sel_acc /*acc_unit*/, bno055_unit_sel_angular_rate /*angular_rate_unit*/,
               bno055_unit_sel_euler_angles /*euler_angles_unit*/, bno055_unit_sel_temperature /*temperature_unit*/,
               bno055_unit_sel_orientation_def /*orientation_def*/,
               bno055_callback_t callback) { callback(write_success); });

    imu_init();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_write_remap_axis>());
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(ERROR_HANDLER_GROUP_IMU,
                                                                                 IMU_ERROR_INIT_TIMEOUT));
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
    EXPECT_FALSE(imu_data_available());
}

TEST(TEST_NAME, init__remap_sign_error) {
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](bno055_self_test_result_t *out, bno055_callback_t callback) {
        out->mcu_passed = true;
        out->acc_passed = true;
        out->gyr_passed = true;
        out->mag_passed = true;
        callback(read_success);
    });
    bnoHandle.overrideFunc<bno055_write_unit_selection>(
            [](bno055_unit_sel_acc /*acc_unit*/, bno055_unit_sel_angular_rate /*angular_rate_unit*/,
               bno055_unit_sel_euler_angles /*euler_angles_unit*/, bno055_unit_sel_temperature /*temperature_unit*/,
               bno055_unit_sel_orientation_def /*orientation_def*/,
               bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis>(
            [](bno055_axis_remap_axis_t /*new_x*/, bno055_axis_remap_axis_t /*new_y*/,
               bno055_axis_remap_axis_t /*new_z*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis_sign>(
            [](bno055_axis_remap_sign_t /*new_x_sign*/, bno055_axis_remap_sign_t /*new_y_sign*/,
               bno055_axis_remap_sign_t /*new_z_sign*/, bno055_callback_t callback) { callback(write_fail); });

    imu_init();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(ERROR_HANDLER_GROUP_BNO055,
                                                                                 write_fail + 1));
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
}

TEST(TEST_NAME, init__remap_sign_timeout) {
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](bno055_self_test_result_t *out, bno055_callback_t callback) {
        out->mcu_passed = true;
        out->acc_passed = true;
        out->gyr_passed = true;
        out->mag_passed = true;
        callback(read_success);
    });
    bnoHandle.overrideFunc<bno055_write_unit_selection>(
            [](bno055_unit_sel_acc /*acc_unit*/, bno055_unit_sel_angular_rate /*angular_rate_unit*/,
               bno055_unit_sel_euler_angles /*euler_angles_unit*/, bno055_unit_sel_temperature /*temperature_unit*/,
               bno055_unit_sel_orientation_def /*orientation_def*/,
               bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis>(
            [](bno055_axis_remap_axis_t /*new_x*/, bno055_axis_remap_axis_t /*new_y*/,
               bno055_axis_remap_axis_t /*new_z*/, bno055_callback_t callback) { callback(write_success); });

    imu_init();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_write_remap_axis_sign>());
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(ERROR_HANDLER_GROUP_IMU,
                                                                                 IMU_ERROR_INIT_TIMEOUT));
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
}

TEST(TEST_NAME, init__ndof_error) {
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>([](bno055_opr_mode_t opr_mode, bno055_callback_t callback) {
        if (opr_mode == BNO055_OPR_MODE_NDOF_FMC_OFF) {
            callback(write_fail);
        } else {
            callback(write_success);
        }
    });
    bnoHandle.overrideFunc<bno055_read_self_test>([](bno055_self_test_result_t *out, bno055_callback_t callback) {
        out->mcu_passed = true;
        out->acc_passed = true;
        out->gyr_passed = true;
        out->mag_passed = true;
        callback(read_success);
    });
    bnoHandle.overrideFunc<bno055_write_unit_selection>(
            [](bno055_unit_sel_acc /*acc_unit*/, bno055_unit_sel_angular_rate /*angular_rate_unit*/,
               bno055_unit_sel_euler_angles /*euler_angles_unit*/, bno055_unit_sel_temperature /*temperature_unit*/,
               bno055_unit_sel_orientation_def /*orientation_def*/,
               bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis>(
            [](bno055_axis_remap_axis_t /*new_x*/, bno055_axis_remap_axis_t /*new_y*/,
               bno055_axis_remap_axis_t /*new_z*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis_sign>(
            [](bno055_axis_remap_sign_t /*new_x_sign*/, bno055_axis_remap_sign_t /*new_y_sign*/,
               bno055_axis_remap_sign_t /*new_z_sign*/, bno055_callback_t callback) { callback(write_success); });

    imu_init();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(ERROR_HANDLER_GROUP_BNO055,
                                                                                 write_fail + 1));
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
    EXPECT_FALSE(imu_data_available());
}

TEST(TEST_NAME, init__ndof_timeout) {
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>([](bno055_opr_mode_t opr_mode, bno055_callback_t callback) {
        if (opr_mode != BNO055_OPR_MODE_NDOF_FMC_OFF) {
            callback(write_success);
        }
    });
    bnoHandle.overrideFunc<bno055_read_self_test>([](bno055_self_test_result_t *out, bno055_callback_t callback) {
        out->mcu_passed = true;
        out->acc_passed = true;
        out->gyr_passed = true;
        out->mag_passed = true;
        callback(read_success);
    });
    bnoHandle.overrideFunc<bno055_write_unit_selection>(
            [](bno055_unit_sel_acc /*acc_unit*/, bno055_unit_sel_angular_rate /*angular_rate_unit*/,
               bno055_unit_sel_euler_angles /*euler_angles_unit*/, bno055_unit_sel_temperature /*temperature_unit*/,
               bno055_unit_sel_orientation_def /*orientation_def*/,
               bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis>(
            [](bno055_axis_remap_axis_t /*new_x*/, bno055_axis_remap_axis_t /*new_y*/,
               bno055_axis_remap_axis_t /*new_z*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis_sign>(
            [](bno055_axis_remap_sign_t /*new_x_sign*/, bno055_axis_remap_sign_t /*new_y_sign*/,
               bno055_axis_remap_sign_t /*new_z_sign*/, bno055_callback_t callback) { callback(write_success); });

    imu_init();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(ERROR_HANDLER_GROUP_IMU,
                                                                                 IMU_ERROR_INIT_TIMEOUT));
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
    EXPECT_FALSE(imu_data_available());
}

TEST(TEST_NAME, init__success) {
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](bno055_self_test_result_t *out, bno055_callback_t callback) {
        out->mcu_passed = true;
        out->acc_passed = true;
        out->gyr_passed = true;
        out->mag_passed = true;
        callback(read_success);
    });
    bnoHandle.overrideFunc<bno055_write_unit_selection>(
            [](bno055_unit_sel_acc /*acc_unit*/, bno055_unit_sel_angular_rate /*angular_rate_unit*/,
               bno055_unit_sel_euler_angles /*euler_angles_unit*/, bno055_unit_sel_temperature /*temperature_unit*/,
               bno055_unit_sel_orientation_def /*orientation_def*/,
               bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis>(
            [](bno055_axis_remap_axis_t /*new_x*/, bno055_axis_remap_axis_t /*new_y*/,
               bno055_axis_remap_axis_t /*new_z*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis_sign>(
            [](bno055_axis_remap_sign_t /*new_x_sign*/, bno055_axis_remap_sign_t /*new_y_sign*/,
               bno055_axis_remap_sign_t /*new_z_sign*/, bno055_callback_t callback) { callback(write_success); });

    imu_init();
    EXPECT_FALSE(imu_get_latest_data().imu_ok);

    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_write_opr_mode>(BNO055_OPR_MODE_CONFIG_MODE, std::ignore));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_self_test>(std::ignore, std::ignore));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_write_unit_selection>(
            BNO055_UNIT_SEL_ACC_MPS2, BNO055_UNIT_SEL_ANGULAR_RATE_DPS, BNO055_UNIT_SEL_EULER_ANGLES_DEGREES,
            BNO055_UNIT_SEL_TEMPERATURE_CELSIUS, BNO055_UNIT_SEL_ORIENTATION_DEF_WINDOWS, std::ignore));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_write_remap_axis>(BNO055_AXIS_REMAP_Y_AXIS, BNO055_AXIS_REMAP_X_AXIS,
                                                                     BNO055_AXIS_REMAP_Z_AXIS, std::ignore));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_write_remap_axis_sign>(
            BNO055_AXIS_REMAP_SIGN_POSITIVE, BNO055_AXIS_REMAP_SIGN_POSITIVE, BNO055_AXIS_REMAP_SIGN_NEGATIVE,
            std::ignore));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_write_opr_mode>(BNO055_OPR_MODE_NDOF_FMC_OFF, std::ignore));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
}

TEST(TEST_NAME, start_sampling__no_response) {
    /*
     * Init
     */
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](bno055_self_test_result_t *out, bno055_callback_t callback) {
        out->mcu_passed = true;
        out->acc_passed = true;
        out->gyr_passed = true;
        out->mag_passed = true;
        callback(read_success);
    });
    bnoHandle.overrideFunc<bno055_write_unit_selection>(
            [](bno055_unit_sel_acc /*acc_unit*/, bno055_unit_sel_angular_rate /*angular_rate_unit*/,
               bno055_unit_sel_euler_angles /*euler_angles_unit*/, bno055_unit_sel_temperature /*temperature_unit*/,
               bno055_unit_sel_orientation_def /*orientation_def*/,
               bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis>(
            [](bno055_axis_remap_axis_t /*new_x*/, bno055_axis_remap_axis_t /*new_y*/,
               bno055_axis_remap_axis_t /*new_z*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis_sign>(
            [](bno055_axis_remap_sign_t /*new_x_sign*/, bno055_axis_remap_sign_t /*new_y_sign*/,
               bno055_axis_remap_sign_t /*new_z_sign*/, bno055_callback_t callback) { callback(write_success); });

    imu_init();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));

    /*
     * Test:
     *  * Call start sampling, expect call to euler-angles
     *  * Call start sampling without calling callback, expect warning
     */

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_eul_xyz_2_mul_16>());

    imu_start_sampling();
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(ERROR_HANDLER_GROUP_IMU,
                                                                                   IMU_ERROR_READ_TIMEOUT));
}

TEST(TEST_NAME, start_sampling__read_success_system_status_invalid) {
    /*
     * Init
     */
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](bno055_self_test_result_t *out, bno055_callback_t callback) {
        out->mcu_passed = true;
        out->acc_passed = true;
        out->gyr_passed = true;
        out->mag_passed = true;
        callback(read_success);
    });
    bnoHandle.overrideFunc<bno055_write_unit_selection>(
            [](bno055_unit_sel_acc /*acc_unit*/, bno055_unit_sel_angular_rate /*angular_rate_unit*/,
               bno055_unit_sel_euler_angles /*euler_angles_unit*/, bno055_unit_sel_temperature /*temperature_unit*/,
               bno055_unit_sel_orientation_def /*orientation_def*/,
               bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis>(
            [](bno055_axis_remap_axis_t /*new_x*/, bno055_axis_remap_axis_t /*new_y*/,
               bno055_axis_remap_axis_t /*new_z*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis_sign>(
            [](bno055_axis_remap_sign_t /*new_x_sign*/, bno055_axis_remap_sign_t /*new_y_sign*/,
               bno055_axis_remap_sign_t /*new_z_sign*/, bno055_callback_t callback) { callback(write_success); });

    imu_init();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));

    /*
     * Test:
     *  * Call start sampling, expect call to euler-angles
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to gyro
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to acc
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to system status
     *  * Set system status to system idle
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to calib-status
     *  * Set calib status to all calibrated
     *  * Call provided callback with read success
     *  * Call start sampling to process result
     *  * Expect new data available
     *  * Expect imu_ok is false
     */

    bno055_callback_t callback = nullptr;
    bnoHandle.overrideFunc<bno055_read_eul_xyz_2_mul_16>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_gyr_xyz_mul_16>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_acc_xyz_mul_100>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_system_status>([&callback](bno055_status_t *out, bno055_callback_t callback_) {
        *out = BNO055_STATUS_SYSTEM_IDLE;
        callback = callback_;
    });
    bnoHandle.overrideFunc<bno055_read_calib_status>(
            [&callback](bno055_calib_status_t *out, bno055_callback_t callback_) {
                out->sys_status = 3;
                out->gyr_status = 3;
                out->acc_status = 3;
                out->mag_status = 3;
                callback = callback_;
            });

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_eul_xyz_2_mul_16>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_gyr_xyz_mul_16>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_acc_xyz_mul_100>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_system_status>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_calib_status>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();

    EXPECT_TRUE(imu_data_available());
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
}

TEST(TEST_NAME, start_sampling__read_success_acc_not_calibrated) {
    /*
     * Init
     */
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](bno055_self_test_result_t *out, bno055_callback_t callback) {
        out->mcu_passed = true;
        out->acc_passed = true;
        out->gyr_passed = true;
        out->mag_passed = true;
        callback(read_success);
    });
    bnoHandle.overrideFunc<bno055_write_unit_selection>(
            [](bno055_unit_sel_acc /*acc_unit*/, bno055_unit_sel_angular_rate /*angular_rate_unit*/,
               bno055_unit_sel_euler_angles /*euler_angles_unit*/, bno055_unit_sel_temperature /*temperature_unit*/,
               bno055_unit_sel_orientation_def /*orientation_def*/,
               bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis>(
            [](bno055_axis_remap_axis_t /*new_x*/, bno055_axis_remap_axis_t /*new_y*/,
               bno055_axis_remap_axis_t /*new_z*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis_sign>(
            [](bno055_axis_remap_sign_t /*new_x_sign*/, bno055_axis_remap_sign_t /*new_y_sign*/,
               bno055_axis_remap_sign_t /*new_z_sign*/, bno055_callback_t callback) { callback(write_success); });

    imu_init();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));

    /*
     * Test:
     *  * Call start sampling, expect call to euler-angles
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to gyro
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to acc
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to system status
     *  * Set system status to sensor fusion algorithm running
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to calib-status
     *  * Set calib status to all but acc calibrated
     *  * Call provided callback with read success
     *  * Call start sampling to process result
     *  * Expect new data available
     *  * Expect imu_ok is false
     */

    bno055_callback_t callback = nullptr;
    bnoHandle.overrideFunc<bno055_read_eul_xyz_2_mul_16>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_gyr_xyz_mul_16>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_acc_xyz_mul_100>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_system_status>([&callback](bno055_status_t *out, bno055_callback_t callback_) {
        *out = BNO055_STATUS_SENSOR_FUSION_ALGORITHM_RUNNING;
        callback = callback_;
    });
    bnoHandle.overrideFunc<bno055_read_calib_status>(
            [&callback](bno055_calib_status_t *out, bno055_callback_t callback_) {
                out->sys_status = 3;
                out->gyr_status = 3;
                out->acc_status = 0;
                out->mag_status = 3;
                callback = callback_;
            });

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_eul_xyz_2_mul_16>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_gyr_xyz_mul_16>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_acc_xyz_mul_100>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_system_status>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_calib_status>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();

    EXPECT_TRUE(imu_data_available());
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
}

TEST(TEST_NAME, start_sampling__read_success_mag_not_calibrated) {
    /*
     * Init
     */
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](bno055_self_test_result_t *out, bno055_callback_t callback) {
        out->mcu_passed = true;
        out->acc_passed = true;
        out->gyr_passed = true;
        out->mag_passed = true;
        callback(read_success);
    });
    bnoHandle.overrideFunc<bno055_write_unit_selection>(
            [](bno055_unit_sel_acc /*acc_unit*/, bno055_unit_sel_angular_rate /*angular_rate_unit*/,
               bno055_unit_sel_euler_angles /*euler_angles_unit*/, bno055_unit_sel_temperature /*temperature_unit*/,
               bno055_unit_sel_orientation_def /*orientation_def*/,
               bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis>(
            [](bno055_axis_remap_axis_t /*new_x*/, bno055_axis_remap_axis_t /*new_y*/,
               bno055_axis_remap_axis_t /*new_z*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis_sign>(
            [](bno055_axis_remap_sign_t /*new_x_sign*/, bno055_axis_remap_sign_t /*new_y_sign*/,
               bno055_axis_remap_sign_t /*new_z_sign*/, bno055_callback_t callback) { callback(write_success); });

    imu_init();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));

    /*
     * Test:
     *  * Call start sampling, expect call to euler-angles
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to gyro
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to acc
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to system status
     *  * Set system status to sensor fusion algorithm running
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to calib-status
     *  * Set calib status to all but mag calibrated
     *  * Call provided callback with read success
     *  * Call start sampling to process result
     *  * Expect new data available
     *  * Expect imu_ok is false
     */

    bno055_callback_t callback = nullptr;
    bnoHandle.overrideFunc<bno055_read_eul_xyz_2_mul_16>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_gyr_xyz_mul_16>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_acc_xyz_mul_100>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_system_status>([&callback](bno055_status_t *out, bno055_callback_t callback_) {
        *out = BNO055_STATUS_SENSOR_FUSION_ALGORITHM_RUNNING;
        callback = callback_;
    });
    bnoHandle.overrideFunc<bno055_read_calib_status>(
            [&callback](bno055_calib_status_t *out, bno055_callback_t callback_) {
                out->sys_status = 3;
                out->gyr_status = 3;
                out->acc_status = 3;
                out->mag_status = 0;
                callback = callback_;
            });

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_eul_xyz_2_mul_16>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_gyr_xyz_mul_16>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_acc_xyz_mul_100>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_system_status>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_calib_status>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();

    EXPECT_TRUE(imu_data_available());
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
}

TEST(TEST_NAME, start_sampling__read_success_gyr_not_calibrated) {
    /*
     * Init
     */
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](bno055_self_test_result_t *out, bno055_callback_t callback) {
        out->mcu_passed = true;
        out->acc_passed = true;
        out->gyr_passed = true;
        out->mag_passed = true;
        callback(read_success);
    });
    bnoHandle.overrideFunc<bno055_write_unit_selection>(
            [](bno055_unit_sel_acc /*acc_unit*/, bno055_unit_sel_angular_rate /*angular_rate_unit*/,
               bno055_unit_sel_euler_angles /*euler_angles_unit*/, bno055_unit_sel_temperature /*temperature_unit*/,
               bno055_unit_sel_orientation_def /*orientation_def*/,
               bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis>(
            [](bno055_axis_remap_axis_t /*new_x*/, bno055_axis_remap_axis_t /*new_y*/,
               bno055_axis_remap_axis_t /*new_z*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis_sign>(
            [](bno055_axis_remap_sign_t /*new_x_sign*/, bno055_axis_remap_sign_t /*new_y_sign*/,
               bno055_axis_remap_sign_t /*new_z_sign*/, bno055_callback_t callback) { callback(write_success); });

    imu_init();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));

    /*
     * Test:
     *  * Call start sampling, expect call to euler-angles
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to gyro
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to acc
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to system status
     *  * Set system status to sensor fusion algorithm running
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to calib-status
     *  * Set calib status to all but gyr calibrated
     *  * Call provided callback with read success
     *  * Call start sampling to process result
     *  * Expect new data available
     *  * Expect imu_ok is false
     */

    bno055_callback_t callback = nullptr;
    bnoHandle.overrideFunc<bno055_read_eul_xyz_2_mul_16>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_gyr_xyz_mul_16>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_acc_xyz_mul_100>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_system_status>([&callback](bno055_status_t *out, bno055_callback_t callback_) {
        *out = BNO055_STATUS_SENSOR_FUSION_ALGORITHM_RUNNING;
        callback = callback_;
    });
    bnoHandle.overrideFunc<bno055_read_calib_status>(
            [&callback](bno055_calib_status_t *out, bno055_callback_t callback_) {
                out->sys_status = 3;
                out->gyr_status = 0;
                out->acc_status = 3;
                out->mag_status = 3;
                callback = callback_;
            });

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_eul_xyz_2_mul_16>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_gyr_xyz_mul_16>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_acc_xyz_mul_100>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_system_status>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_calib_status>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();

    EXPECT_TRUE(imu_data_available());
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
}

TEST(TEST_NAME, start_sampling__read_success_sys_not_calibrated) {
    /*
     * Init
     */
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](bno055_self_test_result_t *out, bno055_callback_t callback) {
        out->mcu_passed = true;
        out->acc_passed = true;
        out->gyr_passed = true;
        out->mag_passed = true;
        callback(read_success);
    });
    bnoHandle.overrideFunc<bno055_write_unit_selection>(
            [](bno055_unit_sel_acc /*acc_unit*/, bno055_unit_sel_angular_rate /*angular_rate_unit*/,
               bno055_unit_sel_euler_angles /*euler_angles_unit*/, bno055_unit_sel_temperature /*temperature_unit*/,
               bno055_unit_sel_orientation_def /*orientation_def*/,
               bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis>(
            [](bno055_axis_remap_axis_t /*new_x*/, bno055_axis_remap_axis_t /*new_y*/,
               bno055_axis_remap_axis_t /*new_z*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis_sign>(
            [](bno055_axis_remap_sign_t /*new_x_sign*/, bno055_axis_remap_sign_t /*new_y_sign*/,
               bno055_axis_remap_sign_t /*new_z_sign*/, bno055_callback_t callback) { callback(write_success); });

    imu_init();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));

    /*
     * Test:
     *  * Call start sampling, expect call to euler-angles
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to gyro
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to acc
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to system status
     *  * Set system status to sensor fusion algorithm running
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to calib-status
     *  * Set calib status to all but sys calibrated
     *  * Call provided callback with read success
     *  * Call start sampling to process result
     *  * Expect new data available
     *  * Expect imu_ok is false
     */

    bno055_callback_t callback = nullptr;
    bnoHandle.overrideFunc<bno055_read_eul_xyz_2_mul_16>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_gyr_xyz_mul_16>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_acc_xyz_mul_100>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_system_status>([&callback](bno055_status_t *out, bno055_callback_t callback_) {
        *out = BNO055_STATUS_SENSOR_FUSION_ALGORITHM_RUNNING;
        callback = callback_;
    });
    bnoHandle.overrideFunc<bno055_read_calib_status>(
            [&callback](bno055_calib_status_t *out, bno055_callback_t callback_) {
                out->sys_status = 0;
                out->gyr_status = 3;
                out->acc_status = 3;
                out->mag_status = 3;
                callback = callback_;
            });

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_eul_xyz_2_mul_16>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_gyr_xyz_mul_16>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_acc_xyz_mul_100>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_system_status>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_calib_status>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();

    EXPECT_TRUE(imu_data_available());
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
}

TEST(TEST_NAME, start_sampling__read_success_all_ok) {
    /*
     * Init
     */
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](bno055_self_test_result_t *out, bno055_callback_t callback) {
        out->mcu_passed = true;
        out->acc_passed = true;
        out->gyr_passed = true;
        out->mag_passed = true;
        callback(read_success);
    });
    bnoHandle.overrideFunc<bno055_write_unit_selection>(
            [](bno055_unit_sel_acc /*acc_unit*/, bno055_unit_sel_angular_rate /*angular_rate_unit*/,
               bno055_unit_sel_euler_angles /*euler_angles_unit*/, bno055_unit_sel_temperature /*temperature_unit*/,
               bno055_unit_sel_orientation_def /*orientation_def*/,
               bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis>(
            [](bno055_axis_remap_axis_t /*new_x*/, bno055_axis_remap_axis_t /*new_y*/,
               bno055_axis_remap_axis_t /*new_z*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis_sign>(
            [](bno055_axis_remap_sign_t /*new_x_sign*/, bno055_axis_remap_sign_t /*new_y_sign*/,
               bno055_axis_remap_sign_t /*new_z_sign*/, bno055_callback_t callback) { callback(write_success); });

    imu_init();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));

    /*
     * Test:
     *  * Call start sampling, expect call to euler-angles
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to gyro
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to acc
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to system status
     *  * Set system status to sensor fusion algorithm running
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to calib-status
     *  * Set calib status to all calibrated
     *  * Call provided callback with read success
     *  * Call start sampling to process result
     *  * Expect new data available
     *  * Expect imu_ok is true
     */

    bno055_callback_t callback = nullptr;
    bnoHandle.overrideFunc<bno055_read_eul_xyz_2_mul_16>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_gyr_xyz_mul_16>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_acc_xyz_mul_100>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_system_status>([&callback](bno055_status_t *out, bno055_callback_t callback_) {
        *out = BNO055_STATUS_SENSOR_FUSION_ALGORITHM_RUNNING;
        callback = callback_;
    });
    bnoHandle.overrideFunc<bno055_read_calib_status>(
            [&callback](bno055_calib_status_t *out, bno055_callback_t callback_) {
                out->sys_status = 3;
                out->gyr_status = 3;
                out->acc_status = 3;
                out->mag_status = 3;
                callback = callback_;
            });

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_eul_xyz_2_mul_16>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_gyr_xyz_mul_16>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_acc_xyz_mul_100>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_system_status>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_calib_status>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();

    EXPECT_TRUE(imu_data_available());
    EXPECT_TRUE(imu_get_latest_data().imu_ok);
}

TEST(TEST_NAME, start_sampling__reread_euler) {
    /*
     * Init
     */
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](bno055_self_test_result_t *out, bno055_callback_t callback) {
        out->mcu_passed = true;
        out->acc_passed = true;
        out->gyr_passed = true;
        out->mag_passed = true;
        callback(read_success);
    });
    bnoHandle.overrideFunc<bno055_write_unit_selection>(
            [](bno055_unit_sel_acc /*acc_unit*/, bno055_unit_sel_angular_rate /*angular_rate_unit*/,
               bno055_unit_sel_euler_angles /*euler_angles_unit*/, bno055_unit_sel_temperature /*temperature_unit*/,
               bno055_unit_sel_orientation_def /*orientation_def*/,
               bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis>(
            [](bno055_axis_remap_axis_t /*new_x*/, bno055_axis_remap_axis_t /*new_y*/,
               bno055_axis_remap_axis_t /*new_z*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis_sign>(
            [](bno055_axis_remap_sign_t /*new_x_sign*/, bno055_axis_remap_sign_t /*new_y_sign*/,
               bno055_axis_remap_sign_t /*new_z_sign*/, bno055_callback_t callback) { callback(write_success); });

    imu_init();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));

    /*
     * Test:
     *  * Call start sampling, expect call to euler-angles
     *  * Call provided callback with read fail
     *  * Call start sampling, expect call to euler-angles
     *  * Call provided callback with read fail
     */

    bno055_callback_t callback = nullptr;
    bnoHandle.overrideFunc<bno055_read_eul_xyz_2_mul_16>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_gyr_xyz_mul_16>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_acc_xyz_mul_100>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_system_status>([&callback](bno055_status_t *out, bno055_callback_t callback_) {
        *out = BNO055_STATUS_SENSOR_FUSION_ALGORITHM_RUNNING;
        callback = callback_;
    });
    bnoHandle.overrideFunc<bno055_read_calib_status>(
            [&callback](bno055_calib_status_t *out, bno055_callback_t callback_) {
                out->sys_status = 3;
                out->gyr_status = 3;
                out->acc_status = 3;
                out->mag_status = 3;
                callback = callback_;
            });

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_eul_xyz_2_mul_16>());
    EXPECT_FALSE(imu_data_available());

    callback(read_fail);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_eul_xyz_2_mul_16>());
    EXPECT_FALSE(imu_data_available());
}

TEST(TEST_NAME, start_sampling__reread_gyro) {
    /*
     * Init
     */
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](bno055_self_test_result_t *out, bno055_callback_t callback) {
        out->mcu_passed = true;
        out->acc_passed = true;
        out->gyr_passed = true;
        out->mag_passed = true;
        callback(read_success);
    });
    bnoHandle.overrideFunc<bno055_write_unit_selection>(
            [](bno055_unit_sel_acc /*acc_unit*/, bno055_unit_sel_angular_rate /*angular_rate_unit*/,
               bno055_unit_sel_euler_angles /*euler_angles_unit*/, bno055_unit_sel_temperature /*temperature_unit*/,
               bno055_unit_sel_orientation_def /*orientation_def*/,
               bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis>(
            [](bno055_axis_remap_axis_t /*new_x*/, bno055_axis_remap_axis_t /*new_y*/,
               bno055_axis_remap_axis_t /*new_z*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis_sign>(
            [](bno055_axis_remap_sign_t /*new_x_sign*/, bno055_axis_remap_sign_t /*new_y_sign*/,
               bno055_axis_remap_sign_t /*new_z_sign*/, bno055_callback_t callback) { callback(write_success); });

    imu_init();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));

    /*
     * Test:
     *  * Call start sampling, expect call to euler-angles
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to gyro
     *  * Call provided callback with read fail
     *  * Call start sampling, expect call to gyro
     *  * Call provided callback with read fail
     */

    bno055_callback_t callback = nullptr;
    bnoHandle.overrideFunc<bno055_read_eul_xyz_2_mul_16>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_gyr_xyz_mul_16>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_acc_xyz_mul_100>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_system_status>([&callback](bno055_status_t *out, bno055_callback_t callback_) {
        *out = BNO055_STATUS_SENSOR_FUSION_ALGORITHM_RUNNING;
        callback = callback_;
    });
    bnoHandle.overrideFunc<bno055_read_calib_status>(
            [&callback](bno055_calib_status_t *out, bno055_callback_t callback_) {
                out->sys_status = 3;
                out->gyr_status = 3;
                out->acc_status = 3;
                out->mag_status = 3;
                callback = callback_;
            });

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_eul_xyz_2_mul_16>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_gyr_xyz_mul_16>());
    EXPECT_FALSE(imu_data_available());

    callback(read_fail);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_gyr_xyz_mul_16>());
    EXPECT_FALSE(imu_data_available());
}

TEST(TEST_NAME, start_sampling__reread_acc) {
    /*
     * Init
     */
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](bno055_self_test_result_t *out, bno055_callback_t callback) {
        out->mcu_passed = true;
        out->acc_passed = true;
        out->gyr_passed = true;
        out->mag_passed = true;
        callback(read_success);
    });
    bnoHandle.overrideFunc<bno055_write_unit_selection>(
            [](bno055_unit_sel_acc /*acc_unit*/, bno055_unit_sel_angular_rate /*angular_rate_unit*/,
               bno055_unit_sel_euler_angles /*euler_angles_unit*/, bno055_unit_sel_temperature /*temperature_unit*/,
               bno055_unit_sel_orientation_def /*orientation_def*/,
               bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis>(
            [](bno055_axis_remap_axis_t /*new_x*/, bno055_axis_remap_axis_t /*new_y*/,
               bno055_axis_remap_axis_t /*new_z*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis_sign>(
            [](bno055_axis_remap_sign_t /*new_x_sign*/, bno055_axis_remap_sign_t /*new_y_sign*/,
               bno055_axis_remap_sign_t /*new_z_sign*/, bno055_callback_t callback) { callback(write_success); });

    imu_init();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));

    /*
     * Test:
     *  * Call start sampling, expect call to euler-angles
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to gyro
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to acc
     *  * Call provided callback with read fail
     *  * Call start sampling, expect call to acc
     */

    bno055_callback_t callback = nullptr;
    bnoHandle.overrideFunc<bno055_read_eul_xyz_2_mul_16>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_gyr_xyz_mul_16>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_acc_xyz_mul_100>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_system_status>([&callback](bno055_status_t *out, bno055_callback_t callback_) {
        *out = BNO055_STATUS_SENSOR_FUSION_ALGORITHM_RUNNING;
        callback = callback_;
    });
    bnoHandle.overrideFunc<bno055_read_calib_status>(
            [&callback](bno055_calib_status_t *out, bno055_callback_t callback_) {
                out->sys_status = 3;
                out->gyr_status = 3;
                out->acc_status = 3;
                out->mag_status = 3;
                callback = callback_;
            });

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_eul_xyz_2_mul_16>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_gyr_xyz_mul_16>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_acc_xyz_mul_100>());
    EXPECT_FALSE(imu_data_available());

    callback(read_fail);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_acc_xyz_mul_100>());
    EXPECT_FALSE(imu_data_available());
}

TEST(TEST_NAME, start_sampling__reread_system_state) {
    /*
     * Init
     */
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](bno055_self_test_result_t *out, bno055_callback_t callback) {
        out->mcu_passed = true;
        out->acc_passed = true;
        out->gyr_passed = true;
        out->mag_passed = true;
        callback(read_success);
    });
    bnoHandle.overrideFunc<bno055_write_unit_selection>(
            [](bno055_unit_sel_acc /*acc_unit*/, bno055_unit_sel_angular_rate /*angular_rate_unit*/,
               bno055_unit_sel_euler_angles /*euler_angles_unit*/, bno055_unit_sel_temperature /*temperature_unit*/,
               bno055_unit_sel_orientation_def /*orientation_def*/,
               bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis>(
            [](bno055_axis_remap_axis_t /*new_x*/, bno055_axis_remap_axis_t /*new_y*/,
               bno055_axis_remap_axis_t /*new_z*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis_sign>(
            [](bno055_axis_remap_sign_t /*new_x_sign*/, bno055_axis_remap_sign_t /*new_y_sign*/,
               bno055_axis_remap_sign_t /*new_z_sign*/, bno055_callback_t callback) { callback(write_success); });

    imu_init();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));

    /*
     * Test:
     *  * Call start sampling, expect call to euler-angles
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to gyro
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to acc
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to system status
     *  * Call provided callback with read fail
     *  * Call start sampling, expect call to system status
     */

    bno055_callback_t callback = nullptr;
    bnoHandle.overrideFunc<bno055_read_eul_xyz_2_mul_16>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_gyr_xyz_mul_16>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_acc_xyz_mul_100>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_system_status>([&callback](bno055_status_t *out, bno055_callback_t callback_) {
        *out = BNO055_STATUS_SENSOR_FUSION_ALGORITHM_RUNNING;
        callback = callback_;
    });
    bnoHandle.overrideFunc<bno055_read_calib_status>(
            [&callback](bno055_calib_status_t *out, bno055_callback_t callback_) {
                out->sys_status = 3;
                out->gyr_status = 3;
                out->acc_status = 3;
                out->mag_status = 3;
                callback = callback_;
            });

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_eul_xyz_2_mul_16>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_gyr_xyz_mul_16>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_acc_xyz_mul_100>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_system_status>());
    EXPECT_FALSE(imu_data_available());

    callback(read_fail);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_system_status>());
    EXPECT_FALSE(imu_data_available());
}

TEST(TEST_NAME, start_sampling__rearead_calib_stat) {
    /*
     * Init
     */
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](bno055_self_test_result_t *out, bno055_callback_t callback) {
        out->mcu_passed = true;
        out->acc_passed = true;
        out->gyr_passed = true;
        out->mag_passed = true;
        callback(read_success);
    });
    bnoHandle.overrideFunc<bno055_write_unit_selection>(
            [](bno055_unit_sel_acc /*acc_unit*/, bno055_unit_sel_angular_rate /*angular_rate_unit*/,
               bno055_unit_sel_euler_angles /*euler_angles_unit*/, bno055_unit_sel_temperature /*temperature_unit*/,
               bno055_unit_sel_orientation_def /*orientation_def*/,
               bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis>(
            [](bno055_axis_remap_axis_t /*new_x*/, bno055_axis_remap_axis_t /*new_y*/,
               bno055_axis_remap_axis_t /*new_z*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis_sign>(
            [](bno055_axis_remap_sign_t /*new_x_sign*/, bno055_axis_remap_sign_t /*new_y_sign*/,
               bno055_axis_remap_sign_t /*new_z_sign*/, bno055_callback_t callback) { callback(write_success); });

    imu_init();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));

    /*
     * Test:
     *  * Call start sampling, expect call to euler-angles
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to gyro
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to acc
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to system status
     *  * Set system status to sensor fusion algorithm running
     *  * Call provided callback with read success
     *  * Call start sampling, expect call to calib-status
     *  * Set calib status to all calibrated
     *  * Call provided callback with read fail
     *  * Call start sampling, expect call to calib-status
     */

    bno055_callback_t callback = nullptr;
    bnoHandle.overrideFunc<bno055_read_eul_xyz_2_mul_16>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_gyr_xyz_mul_16>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_acc_xyz_mul_100>(
            [&callback](int16_t * /*out*/, bno055_callback_t callback_) { callback = callback_; });
    bnoHandle.overrideFunc<bno055_read_system_status>([&callback](bno055_status_t *out, bno055_callback_t callback_) {
        *out = BNO055_STATUS_SENSOR_FUSION_ALGORITHM_RUNNING;
        callback = callback_;
    });
    bnoHandle.overrideFunc<bno055_read_calib_status>(
            [&callback](bno055_calib_status_t *out, bno055_callback_t callback_) {
                out->sys_status = 3;
                out->gyr_status = 3;
                out->acc_status = 3;
                out->mag_status = 3;
                callback = callback_;
            });

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_eul_xyz_2_mul_16>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_gyr_xyz_mul_16>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_acc_xyz_mul_100>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_system_status>());
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_calib_status>());
    EXPECT_FALSE(imu_data_available());

    callback(read_fail);

    imu_start_sampling();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_calib_status>());
    EXPECT_FALSE(imu_data_available());
}

TEST(TEST_NAME, get_data__fields) {
    /*
     * Init
     */
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](bno055_self_test_result_t *out, bno055_callback_t callback) {
        out->mcu_passed = true;
        out->acc_passed = true;
        out->gyr_passed = true;
        out->mag_passed = true;
        callback(read_success);
    });
    bnoHandle.overrideFunc<bno055_write_unit_selection>(
            [](bno055_unit_sel_acc /*acc_unit*/, bno055_unit_sel_angular_rate /*angular_rate_unit*/,
               bno055_unit_sel_euler_angles /*euler_angles_unit*/, bno055_unit_sel_temperature /*temperature_unit*/,
               bno055_unit_sel_orientation_def /*orientation_def*/,
               bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis>(
            [](bno055_axis_remap_axis_t /*new_x*/, bno055_axis_remap_axis_t /*new_y*/,
               bno055_axis_remap_axis_t /*new_z*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis_sign>(
            [](bno055_axis_remap_sign_t /*new_x_sign*/, bno055_axis_remap_sign_t /*new_y_sign*/,
               bno055_axis_remap_sign_t /*new_z_sign*/, bno055_callback_t callback) { callback(write_success); });

    imu_init();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));

    /*
     * Test:
     *  * Call start sampling
     *  * Set euler values to 17, 18, 19
     *  * Call provided callback with read success
     *  * Call start sampling
     *  * Set gyro values to 20, 21, 22
     *  * Call provided callback with read success
     *  * Call start sampling
     *  * Set acc values to 23, 24, 25
     *  * Call provided callback with read success
     *  * Call start sampling
     *  * Set system status to sensor fusion algorithm running
     *  * Call provided callback with read success
     *  * Call start sampling
     *  * Set calib status to all calibrated
     *  * Call provided callback with read success
     *  * Call start sampling to process result
     *  * Expect new data available
     *  * Expect imu_ok is true
     *  * Check values in new data
     */

    bno055_callback_t callback = nullptr;
    bnoHandle.overrideFunc<bno055_read_eul_xyz_2_mul_16>([&callback](int16_t *out, bno055_callback_t callback_) {
        out[0] = 17;
        out[1] = 18;
        out[2] = 19;
        callback = callback_;
    });
    bnoHandle.overrideFunc<bno055_read_gyr_xyz_mul_16>([&callback](int16_t *out, bno055_callback_t callback_) {
        out[0] = 20;
        out[1] = 21;
        out[2] = 22;
        callback = callback_;
    });
    bnoHandle.overrideFunc<bno055_read_acc_xyz_mul_100>([&callback](int16_t *out, bno055_callback_t callback_) {
        out[0] = 23;
        out[1] = 24;
        out[2] = 25;
        callback = callback_;
    });
    bnoHandle.overrideFunc<bno055_read_system_status>([&callback](bno055_status_t *out, bno055_callback_t callback_) {
        *out = BNO055_STATUS_SENSOR_FUSION_ALGORITHM_RUNNING;
        callback = callback_;
    });
    bnoHandle.overrideFunc<bno055_read_calib_status>(
            [&callback](bno055_calib_status_t *out, bno055_callback_t callback_) {
                out->sys_status = 3;
                out->gyr_status = 3;
                out->acc_status = 3;
                out->mag_status = 3;
                callback = callback_;
            });

    imu_start_sampling();
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();

    EXPECT_TRUE(imu_data_available());

    auto data = imu_get_latest_data();
    EXPECT_TRUE(data.imu_ok);
    EXPECT_EQ(data.heading_mul_16, 17);
    EXPECT_EQ(data.pitch_mul_16, 18);
    EXPECT_EQ(data.roll_mul_16, 19);
    EXPECT_EQ(data.d_heading_mul_16, 20);
    EXPECT_EQ(data.d_pitch_mul_16, 21);
    EXPECT_EQ(data.d_roll_mul_16, 22);
    EXPECT_EQ(data.acc_x_mul_100, 23);
    EXPECT_EQ(data.acc_y_mul_100, 24);
    EXPECT_EQ(data.acc_z_mul_100, 25);
}

TEST(TEST_NAME, get_data__clear_on_read) {
    /*
     * Init
     */
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](bno055_self_test_result_t *out, bno055_callback_t callback) {
      out->mcu_passed = true;
      out->acc_passed = true;
      out->gyr_passed = true;
      out->mag_passed = true;
      callback(read_success);
    });
    bnoHandle.overrideFunc<bno055_write_unit_selection>(
            [](bno055_unit_sel_acc /*acc_unit*/, bno055_unit_sel_angular_rate /*angular_rate_unit*/,
               bno055_unit_sel_euler_angles /*euler_angles_unit*/, bno055_unit_sel_temperature /*temperature_unit*/,
               bno055_unit_sel_orientation_def /*orientation_def*/,
               bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis>(
            [](bno055_axis_remap_axis_t /*new_x*/, bno055_axis_remap_axis_t /*new_y*/,
               bno055_axis_remap_axis_t /*new_z*/, bno055_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis_sign>(
            [](bno055_axis_remap_sign_t /*new_x_sign*/, bno055_axis_remap_sign_t /*new_y_sign*/,
               bno055_axis_remap_sign_t /*new_z_sign*/, bno055_callback_t callback) { callback(write_success); });

    imu_init();
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));

    /*
     * Test:
     *  * Call start sampling
     *  * Set euler values to 17, 18, 19
     *  * Call provided callback with read success
     *  * Call start sampling
     *  * Set gyro values to 20, 21, 22
     *  * Call provided callback with read success
     *  * Call start sampling
     *  * Set acc values to 23, 24, 25
     *  * Call provided callback with read success
     *  * Call start sampling
     *  * Set system status to sensor fusion algorithm running
     *  * Call provided callback with read success
     *  * Call start sampling
     *  * Set calib status to all calibrated
     *  * Call provided callback with read success
     *  * Call start sampling to process result
     *  * Expect new data available
     *  * Read data
     *  * Expect no new data
     */

    bno055_callback_t callback = nullptr;
    bnoHandle.overrideFunc<bno055_read_eul_xyz_2_mul_16>([&callback](int16_t *out, bno055_callback_t callback_) {
      out[0] = 17;
      out[1] = 18;
      out[2] = 19;
      callback = callback_;
    });
    bnoHandle.overrideFunc<bno055_read_gyr_xyz_mul_16>([&callback](int16_t *out, bno055_callback_t callback_) {
      out[0] = 20;
      out[1] = 21;
      out[2] = 22;
      callback = callback_;
    });
    bnoHandle.overrideFunc<bno055_read_acc_xyz_mul_100>([&callback](int16_t *out, bno055_callback_t callback_) {
      out[0] = 23;
      out[1] = 24;
      out[2] = 25;
      callback = callback_;
    });
    bnoHandle.overrideFunc<bno055_read_system_status>([&callback](bno055_status_t *out, bno055_callback_t callback_) {
      *out = BNO055_STATUS_SENSOR_FUSION_ALGORITHM_RUNNING;
      callback = callback_;
    });
    bnoHandle.overrideFunc<bno055_read_calib_status>(
            [&callback](bno055_calib_status_t *out, bno055_callback_t callback_) {
              out->sys_status = 3;
              out->gyr_status = 3;
              out->acc_status = 3;
              out->mag_status = 3;
              callback = callback_;
            });

    imu_start_sampling();
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();
    EXPECT_FALSE(imu_data_available());

    callback(read_success);

    imu_start_sampling();

    EXPECT_TRUE(imu_data_available());

    imu_get_latest_data();

    EXPECT_FALSE(imu_data_available());
}
