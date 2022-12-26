#include <Mock/Application/error_handler.hpp>
#include <Mock/Drivers/bno055.hpp>
#include <Mock/util/delay.hpp>
#include <gtest/gtest.h>

extern "C" {
#include <Components/imu.h>
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
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_write_opr_mode>(config_mode, std::ignore));
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_read_self_test>(std::ignore, std::ignore));
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_write_unit_selection>(mps2, dps, degrees, celsius, windows,
                                                                         std::ignore));
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_write_remap_axis>(y_axis, x_axis, z_axis, std::ignore));
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_write_remap_axis_sign>(positive, positive, neg, std::ignore));
    EXPECT_TRUE(bnoHandle.functionGotCalled<bno055_write_opr_mode>(ndof_fmc_off, std::ignore));
}

TEST(TEST_NAME, init__config_mode_error) {
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno055_callback_t callback) { callback(write_fail); });

    imu_init();
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(IMU, IMU_ERROR_INIT_CONFIG_MODE));
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
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(IMU, IMU_ERROR_INIT_SELF_TEST_WRITE));
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
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(IMU, IMU_ERROR_INIT_SELF_TEST));
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
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(IMU, IMU_ERROR_INIT_SELF_TEST));
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
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(IMU, IMU_ERROR_INIT_SELF_TEST));
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
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(IMU, IMU_ERROR_INIT_SELF_TEST));
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
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(IMU, IMU_ERROR_INIT_UNIT_SEL));
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
    EXPECT_FALSE(imu_data_available());
}

TEST(TEST_NAME, init__remap_fail) {
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
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(IMU, IMU_ERROR_INIT_REMAP_AXIS));
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
    EXPECT_FALSE(imu_data_available());
}

TEST(TEST_NAME, init__remap_sign_fail) {
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
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(IMU, IMU_ERROR_INIT_REMAP_AXIS_SIGN));
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
}

TEST(TEST_NAME, init__ndof_fail) {
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>([](bno055_opr_mode_t opr_mode, bno055_callback_t callback) {
        if (opr_mode == ndof_fmc_off) {
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
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(IMU, IMU_ERROR_INIT_NDOF_FMC_OFF));
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
    EXPECT_FALSE(imu_data_available());
}

TEST(TEST_NAME, init__timeout) {
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    imu_init();
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_error>(IMU, IMU_ERROR_INIT_CONFIG_MODE));
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
    EXPECT_FALSE(imu_data_available());
}

TEST(TEST_NAME, read__full_read) {
    auto bnoHandle = mock::bno055.getHandle();
    bool alreadyCalled = false;

    bnoHandle.overrideFunc<bno055_read_eul_xyz_2_mul_16>([&alreadyCalled](int16_t *out, bno055_callback_t callback) {
        if (not alreadyCalled) {
            out[0] = 101;
            out[1] = 102;
            out[2] = 103;
            alreadyCalled = true;
            callback(read_success);
        }
        // Break the sampling process
    });

    bnoHandle.overrideFunc<bno055_read_gyr_xyz_mul_16>([](int16_t *out, bno055_callback_t callback) {
        out[0] = 104;
        out[1] = 105;
        out[2] = 106;
        callback(read_success);
    });

    bnoHandle.overrideFunc<bno055_read_acc_xyz_mul_100>([](int16_t *out, bno055_callback_t callback) {
        out[0] = 107;
        out[1] = 108;
        out[2] = 109;
        callback(read_success);
    });

    bnoHandle.overrideFunc<bno055_read_system_status>([](bno055_status_t *out, bno055_callback_t callback) {
        *out = bno055_status_t::sensor_fusion_algorithm_running;
        callback(read_success);
    });

    bnoHandle.overrideFunc<bno055_read_calib_status>([](bno055_calib_status_t *out, bno055_callback_t callback) {
        out->acc_status = 0;
        out->gyr_status = 0;
        out->mag_status = 0;
        out->sys_status = 0;
        callback(read_success);
    });


    EXPECT_FALSE(imu_data_available());
    EXPECT_NO_THROW(imu_start_sampling());
    EXPECT_TRUE(imu_data_available());

    auto data = imu_get_latest_data();

    EXPECT_FALSE(imu_data_available());

    EXPECT_EQ(data.roll_mul_16, 103);
    EXPECT_EQ(data.pitch_mul_16, 102);
    EXPECT_EQ(data.heading_mul_16, 101);
    EXPECT_EQ(data.d_roll_mul_16, 106);
    EXPECT_EQ(data.d_pitch_mul_16, 105);
    EXPECT_EQ(data.d_heading_mul_16, 104);
    EXPECT_EQ(data.acc_x_mul_100, 107);
    EXPECT_EQ(data.acc_y_mul_100, 108);
    EXPECT_EQ(data.acc_z_mul_100, 109);
    EXPECT_EQ(data.imu_ok, true);
}

TEST(TEST_NAME, read__status_error) {
    auto bnoHandle = mock::bno055.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();
    bool alreadyCalled = false;

    bnoHandle.overrideFunc<bno055_read_eul_xyz_2_mul_16>(
            [&alreadyCalled](int16_t * /*out*/, bno055_callback_t callback) {
                if (not alreadyCalled) {
                    alreadyCalled = true;
                    callback(read_success);
                }
                // Break the sampling process
            });

    bnoHandle.overrideFunc<bno055_read_gyr_xyz_mul_16>(
            [](int16_t * /*out*/, bno055_callback_t callback) { callback(read_success); });

    bnoHandle.overrideFunc<bno055_read_acc_xyz_mul_100>(
            [](int16_t * /*out*/, bno055_callback_t callback) { callback(read_success); });

    bnoHandle.overrideFunc<bno055_read_system_status>([](bno055_status_t *out, bno055_callback_t callback) {
        *out = bno055_status_t::system_idle;
        callback(read_success);
    });

    bnoHandle.overrideFunc<bno055_read_calib_status>(
            [](bno055_calib_status_t * /*out*/, bno055_callback_t callback) { callback(read_success); });

    EXPECT_FALSE(imu_data_available());
    EXPECT_NO_THROW(imu_start_sampling());
    EXPECT_TRUE(imu_data_available());

    auto data = imu_get_latest_data();

    EXPECT_EQ(data.imu_ok, false);
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(IMU, IMU_ERROR_STATUS));
}

TEST(TEST_NAME, read__error_call_error_handler) {
    auto bnoHandle = mock::bno055.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();

    bnoHandle.overrideFunc<bno055_read_eul_xyz_2_mul_16>(
            [](int16_t * /*out*/, bno055_callback_t callback) { callback(read_fail); });

    EXPECT_FALSE(imu_data_available());
    EXPECT_NO_THROW(imu_start_sampling());
    EXPECT_FALSE(imu_data_available());

    auto data = imu_get_latest_data();

    EXPECT_EQ(data.imu_ok, false);
    EXPECT_TRUE(errorHandlerHandle.functionGotCalled<error_handler_handle_warning>(IMU, IMU_ERROR_UART));
}

TEST(TEST_NAME, read__error_restart) {
    auto bnoHandle = mock::bno055.getHandle();
    auto errorHandlerHandle = mock::error_handler.getHandle();
    int16_t *dataPtr = nullptr;

    bnoHandle.overrideFunc<bno055_read_eul_xyz_2_mul_16>(
            [](int16_t * /*out*/, bno055_callback_t callback) { callback(read_success); });

    bnoHandle.overrideFunc<bno055_read_gyr_xyz_mul_16>([&dataPtr](int16_t *out, bno055_callback_t callback) {
        EXPECT_NE(dataPtr, out);
        dataPtr = out;
        callback(read_fail);
    });

    EXPECT_FALSE(imu_data_available());
    EXPECT_NO_THROW(imu_start_sampling());
    EXPECT_NO_THROW(imu_start_sampling());
}
