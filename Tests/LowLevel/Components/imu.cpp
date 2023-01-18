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
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(ERROR_HANDLER_GROUP_BNO055, write_fail + 1));
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
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(ERROR_HANDLER_GROUP_IMU, IMU_ERROR_INIT_TIMEOUT));
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
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(ERROR_HANDLER_GROUP_BNO055, read_fail + 1));
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
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(ERROR_HANDLER_GROUP_IMU, IMU_ERROR_INIT_SELF_TEST));
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
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(ERROR_HANDLER_GROUP_IMU, IMU_ERROR_INIT_SELF_TEST));
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
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(ERROR_HANDLER_GROUP_IMU, IMU_ERROR_INIT_SELF_TEST));
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
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(ERROR_HANDLER_GROUP_IMU, IMU_ERROR_INIT_SELF_TEST));
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
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(ERROR_HANDLER_GROUP_IMU, IMU_ERROR_INIT_TIMEOUT));
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
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(ERROR_HANDLER_GROUP_BNO055, write_fail + 1));
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
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(ERROR_HANDLER_GROUP_IMU, IMU_ERROR_INIT_TIMEOUT));
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
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(ERROR_HANDLER_GROUP_BNO055, write_fail + 1));
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
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(ERROR_HANDLER_GROUP_IMU, IMU_ERROR_INIT_TIMEOUT));
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
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(ERROR_HANDLER_GROUP_BNO055, write_fail + 1));
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
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(ERROR_HANDLER_GROUP_IMU, IMU_ERROR_INIT_TIMEOUT));
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
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(ERROR_HANDLER_GROUP_BNO055, write_fail + 1));
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
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(ERROR_HANDLER_GROUP_IMU, IMU_ERROR_INIT_TIMEOUT));
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
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_write_unit_selection>(BNO055_UNIT_SEL_ACC_MPS2, BNO055_UNIT_SEL_ANGULAR_RATE_DPS, BNO055_UNIT_SEL_EULER_ANGLES_DEGREES,
            BNO055_UNIT_SEL_TEMPERATURE_CELSIUS, BNO055_UNIT_SEL_ORIENTATION_DEF_WINDOWS,
                                                                         std::ignore));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_write_remap_axis>(BNO055_AXIS_REMAP_Y_AXIS, BNO055_AXIS_REMAP_X_AXIS,
                                                                     BNO055_AXIS_REMAP_Z_AXIS, std::ignore));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_write_remap_axis_sign>(
            BNO055_AXIS_REMAP_SIGN_POSITIVE, BNO055_AXIS_REMAP_SIGN_POSITIVE, BNO055_AXIS_REMAP_SIGN_NEGATIVE, std::ignore));
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

    EXPECT_TRUE(false);
}

TEST(TEST_NAME, start_sampling__read_error) {
    /*
     * Init
     */
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
     *  * Call the provided callback with read-error as argument
     *  * Call start sampling, expect warning and call to euler-angles
     */

    EXPECT_TRUE(false);
}

TEST(TEST_NAME, start_sampling__full_cycle) {
    /*
     * Init
     */
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
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_init>());
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));
    EXPECT_TRUE(delayHandle.functionGotCalled<_delay_ms>(20));

    /*
     * Test:
     *  * Call start sampling, expect call to read euler-angles
     *  * Write euler-angles to (17, 18, 19)
     *  * Call the provided callback with read-success as argument
     *  * expect no data available
     *  * Call start sampling, expect call to read gyro-rates
     *  * Write gyro-rates to (20, 21, 22)
     *  * Call the provided callback with read-success as argument
     *  * expect no data available
     *  * Call start sampling, expect call to read acceleration
     *  * Write acceleration to (23, 24, 25)
     *  * Call the provided callback with read-success as argument
     *  * expect no data available
     *  * Call start sampling, expect call to read system state
     *  * Write system state to "Sensor Fusion Algorithm Running"
     *  * Call the provided callback with read-success as argument
     *  * expect no data available
     *  * Call start sampling, expect call to read calibration status
     *  * Write calibration status to all fully calibration
     *  * Call the provided callback with read-success as argument
     *  * expect data available
     *  * check data
     */

    EXPECT_TRUE(false);
}

TEST(TEST_NAME, start_sampling__status_read_error) {
    /*
     * Init
     */
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
     *  * Call the provided callback with read-error as argument
     *  * Call start sampling, expect warning and call to euler-angles
     */

    EXPECT_TRUE(false);
}

TEST(TEST_NAME, start_sampling__acc_not_calib) {
    /*
     * Init
     */
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
     *  * Call the provided callback with read-error as argument
     *  * Call start sampling, expect warning and call to euler-angles
     */

    EXPECT_TRUE(false);
}

TEST(TEST_NAME, start_sampling__mag_not_calib) {
    /*
     * Init
     */
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
     *  * Call the provided callback with read-error as argument
     *  * Call start sampling, expect warning and call to euler-angles
     */

    EXPECT_TRUE(false);
}

TEST(TEST_NAME, start_sampling__gyr_not_calib) {
    /*
     * Init
     */
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
     *  * Call the provided callback with read-error as argument
     *  * Call start sampling, expect warning and call to euler-angles
     */

    EXPECT_TRUE(false);
}

TEST(TEST_NAME, start_sampling__sys_not_calib) {
    /*
     * Init
     */
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
     *  * Call the provided callback with read-error as argument
     *  * Call start sampling, expect warning and call to euler-angles
     */

    EXPECT_TRUE(false);
}
