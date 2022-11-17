#include <Mock/Drivers/bno055.hpp>
#include <Mock/Tests/Mock/System/util/delay.hpp>
#include <gtest/gtest.h>

extern "C" {
#include <Components/imu.h>
}

TEST(TEST_NAME, init__success) {
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](uint8_t *out, bno_callback_t callback) {
        *out = 0x0F;
        callback(read_success);
    });
    bnoHandle.overrideFunc<bno055_write_unit_selection>(
            [](bno055_unit_sel_acc /*acc_unit*/, bno055_unit_sel_angular_rate /*angular_rate_unit*/,
               bno055_unit_sel_euler_angles /*euler_angles_unit*/, bno055_unit_sel_temperature /*temperature_unit*/,
               bno055_unit_sel_orientation_def /*orientation_def*/,
               bno_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis>(
            [](bno055_axis_remap_axis_t /*new_x*/, bno055_axis_remap_axis_t /*new_y*/,
               bno055_axis_remap_axis_t /*new_z*/, bno_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis_sign>(
            [](bno055_axis_remap_sign_t /*new_x_sign*/, bno055_axis_remap_sign_t /*new_y_sign*/,
               bno055_axis_remap_sign_t /*new_z_sign*/, bno_callback_t callback) { callback(write_success); });

    EXPECT_FALSE(imu_get_latest_data().imu_ok);
    EXPECT_TRUE(imu_init());
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

TEST(TEST_NAME, init__opr_mode_error) {
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno_callback_t callback) { callback(write_fail); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](uint8_t *out, bno_callback_t callback) {
        *out = 0x0F;
        callback(read_success);
    });
    bnoHandle.overrideFunc<bno055_write_unit_selection>(
            [](bno055_unit_sel_acc /*acc_unit*/, bno055_unit_sel_angular_rate /*angular_rate_unit*/,
               bno055_unit_sel_euler_angles /*euler_angles_unit*/, bno055_unit_sel_temperature /*temperature_unit*/,
               bno055_unit_sel_orientation_def /*orientation_def*/,
               bno_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis>(
            [](bno055_axis_remap_axis_t /*new_x*/, bno055_axis_remap_axis_t /*new_y*/,
               bno055_axis_remap_axis_t /*new_z*/, bno_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis_sign>(
            [](bno055_axis_remap_sign_t /*new_x_sign*/, bno055_axis_remap_sign_t /*new_y_sign*/,
               bno055_axis_remap_sign_t /*new_z_sign*/, bno_callback_t callback) { callback(write_success); });

    EXPECT_FALSE(imu_get_latest_data().imu_ok);
    EXPECT_FALSE(imu_init());
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
    EXPECT_FALSE(imu_data_available());
}

TEST(TEST_NAME, init__self_test_error) {
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](uint8_t *out, bno_callback_t callback) {
        *out = 0x0F;
        callback(read_fail);
    });
    bnoHandle.overrideFunc<bno055_write_unit_selection>(
            [](bno055_unit_sel_acc /*acc_unit*/, bno055_unit_sel_angular_rate /*angular_rate_unit*/,
               bno055_unit_sel_euler_angles /*euler_angles_unit*/, bno055_unit_sel_temperature /*temperature_unit*/,
               bno055_unit_sel_orientation_def /*orientation_def*/,
               bno_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis>(
            [](bno055_axis_remap_axis_t /*new_x*/, bno055_axis_remap_axis_t /*new_y*/,
               bno055_axis_remap_axis_t /*new_z*/, bno_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis_sign>(
            [](bno055_axis_remap_sign_t /*new_x_sign*/, bno055_axis_remap_sign_t /*new_y_sign*/,
               bno055_axis_remap_sign_t /*new_z_sign*/, bno_callback_t callback) { callback(write_success); });

    EXPECT_FALSE(imu_get_latest_data().imu_ok);
    EXPECT_FALSE(imu_init());
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
    EXPECT_FALSE(imu_data_available());
}

TEST(TEST_NAME, init__self_test_fail) {
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](uint8_t *out, bno_callback_t callback) {
        *out = 0x07;
        callback(read_success);
    });
    bnoHandle.overrideFunc<bno055_write_unit_selection>(
            [](bno055_unit_sel_acc /*acc_unit*/, bno055_unit_sel_angular_rate /*angular_rate_unit*/,
               bno055_unit_sel_euler_angles /*euler_angles_unit*/, bno055_unit_sel_temperature /*temperature_unit*/,
               bno055_unit_sel_orientation_def /*orientation_def*/,
               bno_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis>(
            [](bno055_axis_remap_axis_t /*new_x*/, bno055_axis_remap_axis_t /*new_y*/,
               bno055_axis_remap_axis_t /*new_z*/, bno_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis_sign>(
            [](bno055_axis_remap_sign_t /*new_x_sign*/, bno055_axis_remap_sign_t /*new_y_sign*/,
               bno055_axis_remap_sign_t /*new_z_sign*/, bno_callback_t callback) { callback(write_success); });

    EXPECT_FALSE(imu_get_latest_data().imu_ok);
    EXPECT_FALSE(imu_init());
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
    EXPECT_FALSE(imu_data_available());
}

TEST(TEST_NAME, init__unit_sel_error) {
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](uint8_t *out, bno_callback_t callback) {
        *out = 0x0F;
        callback(read_success);
    });
    bnoHandle.overrideFunc<bno055_write_unit_selection>(
            [](bno055_unit_sel_acc /*acc_unit*/, bno055_unit_sel_angular_rate /*angular_rate_unit*/,
               bno055_unit_sel_euler_angles /*euler_angles_unit*/, bno055_unit_sel_temperature /*temperature_unit*/,
               bno055_unit_sel_orientation_def /*orientation_def*/, bno_callback_t callback) { callback(write_fail); });
    bnoHandle.overrideFunc<bno055_write_remap_axis>(
            [](bno055_axis_remap_axis_t /*new_x*/, bno055_axis_remap_axis_t /*new_y*/,
               bno055_axis_remap_axis_t /*new_z*/, bno_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis_sign>(
            [](bno055_axis_remap_sign_t /*new_x_sign*/, bno055_axis_remap_sign_t /*new_y_sign*/,
               bno055_axis_remap_sign_t /*new_z_sign*/, bno_callback_t callback) { callback(write_success); });

    EXPECT_FALSE(imu_get_latest_data().imu_ok);
    EXPECT_FALSE(imu_init());
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
    EXPECT_FALSE(imu_data_available());
}

TEST(TEST_NAME, init__remap_fail) {
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](uint8_t *out, bno_callback_t callback) {
        *out = 0x0F;
        callback(read_success);
    });
    bnoHandle.overrideFunc<bno055_write_unit_selection>(
            [](bno055_unit_sel_acc /*acc_unit*/, bno055_unit_sel_angular_rate /*angular_rate_unit*/,
               bno055_unit_sel_euler_angles /*euler_angles_unit*/, bno055_unit_sel_temperature /*temperature_unit*/,
               bno055_unit_sel_orientation_def /*orientation_def*/,
               bno_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis>(
            [](bno055_axis_remap_axis_t /*new_x*/, bno055_axis_remap_axis_t /*new_y*/,
               bno055_axis_remap_axis_t /*new_z*/, bno_callback_t callback) { callback(write_fail); });
    bnoHandle.overrideFunc<bno055_write_remap_axis_sign>(
            [](bno055_axis_remap_sign_t /*new_x_sign*/, bno055_axis_remap_sign_t /*new_y_sign*/,
               bno055_axis_remap_sign_t /*new_z_sign*/, bno_callback_t callback) { callback(write_success); });

    EXPECT_FALSE(imu_get_latest_data().imu_ok);
    EXPECT_FALSE(imu_init());
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
    EXPECT_FALSE(imu_data_available());
}

TEST(TEST_NAME, init__remap_sign_fail) {
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();

    bnoHandle.overrideFunc<bno055_write_opr_mode>(
            [](auto /*op_mode*/, bno_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_read_self_test>([](uint8_t *out, bno_callback_t callback) {
        *out = 0x0F;
        callback(read_success);
    });
    bnoHandle.overrideFunc<bno055_write_unit_selection>(
            [](bno055_unit_sel_acc /*acc_unit*/, bno055_unit_sel_angular_rate /*angular_rate_unit*/,
               bno055_unit_sel_euler_angles /*euler_angles_unit*/, bno055_unit_sel_temperature /*temperature_unit*/,
               bno055_unit_sel_orientation_def /*orientation_def*/,
               bno_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis>(
            [](bno055_axis_remap_axis_t /*new_x*/, bno055_axis_remap_axis_t /*new_y*/,
               bno055_axis_remap_axis_t /*new_z*/, bno_callback_t callback) { callback(write_success); });
    bnoHandle.overrideFunc<bno055_write_remap_axis_sign>(
            [](bno055_axis_remap_sign_t /*new_x_sign*/, bno055_axis_remap_sign_t /*new_y_sign*/,
               bno055_axis_remap_sign_t /*new_z_sign*/, bno_callback_t callback) { callback(write_fail); });

    EXPECT_FALSE(imu_get_latest_data().imu_ok);
    EXPECT_FALSE(imu_init());
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
}

TEST(TEST_NAME, init__timeout) {
    auto bnoHandle = mock::bno055.getHandle();
    auto delayHandle = mock::delay.getHandle();

    EXPECT_FALSE(imu_get_latest_data().imu_ok);
    EXPECT_FALSE(imu_init());
    EXPECT_FALSE(imu_get_latest_data().imu_ok);
    EXPECT_FALSE(imu_data_available());
}

TEST(TEST_NAME, read__full_read) {
    auto bnoHandle = mock::bno055.getHandle();
    bool alreadyCalled = false;

    bnoHandle.overrideFunc<bno055_read_eul_x_2_mul_16>([&alreadyCalled](int16_t *out, bno_callback_t callback) {
        if (not alreadyCalled) {
            *out = 101;
            alreadyCalled = true;
            callback(read_success);
        }
        // Break the sampling process
    });

    bnoHandle.overrideFunc<bno055_read_eul_y_2_mul_16>([](int16_t *out, bno_callback_t callback) {
        *out = 102;
        callback(read_success);
    });

    bnoHandle.overrideFunc<bno055_read_eul_z_2_mul_16>([](int16_t *out, bno_callback_t callback) {
        *out = 103;
        callback(read_success);
    });

    bnoHandle.overrideFunc<bno055_read_gyr_x_mul_16>([](int16_t *out, bno_callback_t callback) {
        *out = 104;
        callback(read_success);
    });

    bnoHandle.overrideFunc<bno055_read_gyr_y_mul_16>([](int16_t *out, bno_callback_t callback) {
        *out = 105;
        callback(read_success);
    });

    bnoHandle.overrideFunc<bno055_read_gyr_z_mul_16>([](int16_t *out, bno_callback_t callback) {
        *out = 106;
        callback(read_success);
    });

    bnoHandle.overrideFunc<bno055_read_acc_x_mul_100>([](int16_t *out, bno_callback_t callback) {
        *out = 107;
        callback(read_success);
    });

    bnoHandle.overrideFunc<bno055_read_acc_y_mul_100>([](int16_t *out, bno_callback_t callback) {
        *out = 108;
        callback(read_success);
    });

    bnoHandle.overrideFunc<bno055_read_acc_z_mul_100>([](int16_t *out, bno_callback_t callback) {
        *out = 109;
        callback(read_success);
    });

    bnoHandle.overrideFunc<bno055_read_system_status>([](bno055_status_t *out, bno_callback_t callback) {
        *out = bno055_status_t::sensor_fusion_algorithm_running;
        callback(read_success);
    });

    bnoHandle.overrideFunc<bno055_read_calib_status>([](uint8_t *out, bno_callback_t callback) {
        *out = 110;
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