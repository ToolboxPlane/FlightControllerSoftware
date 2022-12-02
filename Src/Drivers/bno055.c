/**
 * @file bno055.c
 * @author paul
 * @date 01.11.22
 * Description here TODO
 */
#include "bno055.h"

enum {
    BNO055_CHIP_ID_ADDR = 0x00,
    BNO055_ACCEL_REV_ID_ADDR = 0x01,
    BNO055_MAG_REV_ID_ADDR = 0x02,
    BNO055_GYRO_REV_ID_ADDR = 0x03,
    BNO055_SW_REV_ID_LSB_ADDR = 0x04,
    BNO055_SW_REV_ID_MSB_ADDR = 0x05,
    BNO055_BL_REV_ID_ADDR = 0x06,
    BNO055_PAGE_ID_ADDR = 0x07,
    BNO055_ACCEL_DATA_X_LSB_ADDR = 0x08,
    BNO055_ACCEL_DATA_X_MSB_ADDR = 0x09,
    BNO055_ACCEL_DATA_Y_LSB_ADDR = 0x0A,
    BNO055_ACCEL_DATA_Y_MSB_ADDR = 0x0B,
    BNO055_ACCEL_DATA_Z_LSB_ADDR = 0x0C,
    BNO055_ACCEL_DATA_Z_MSB_ADDR = 0x0D,
    BNO055_MAG_DATA_X_LSB_ADDR = 0x0E,
    BNO055_MAG_DATA_X_MSB_ADDR = 0x0F,
    BNO055_MAG_DATA_Y_LSB_ADDR = 0x10,
    BNO055_MAG_DATA_Y_MSB_ADDR = 0x11,
    BNO055_MAG_DATA_Z_LSB_ADDR = 0x12,
    BNO055_MAG_DATA_Z_MSB_ADDR = 0x13,
    BNO055_GYRO_DATA_X_LSB_ADDR = 0x14,
    BNO055_GYRO_DATA_X_MSB_ADDR = 0x15,
    BNO055_GYRO_DATA_Y_LSB_ADDR = 0x16,
    BNO055_GYRO_DATA_Y_MSB_ADDR = 0x17,
    BNO055_GYRO_DATA_Z_LSB_ADDR = 0x18,
    BNO055_GYRO_DATA_Z_MSB_ADDR = 0x19,
    BNO055_EULER_X_LSB_ADDR = 0x1A,
    BNO055_EULER_X_MSB_ADDR = 0x1B,
    BNO055_EULER_Y_LSB_ADDR = 0x1C,
    BNO055_EULER_Y_MSB_ADDR = 0x1D,
    BNO055_EULER_Z_LSB_ADDR = 0x1E,
    BNO055_EULER_Z_MSB_ADDR = 0x1F,
    BNO055_QUATERNION_DATA_W_LSB_ADDR = 0x20,
    BNO055_QUATERNION_DATA_W_MSB_ADDR = 0x21,
    BNO055_QUATERNION_DATA_X_LSB_ADDR = 0x22,
    BNO055_QUATERNION_DATA_X_MSB_ADDR = 0x23,
    BNO055_QUATERNION_DATA_Y_LSB_ADDR = 0x24,
    BNO055_QUATERNION_DATA_Y_MSB_ADDR = 0x25,
    BNO055_QUATERNION_DATA_Z_LSB_ADDR = 0x26,
    BNO055_QUATERNION_DATA_Z_MSB_ADDR = 0x27,
    BNO055_LINEAR_ACCEL_DATA_X_LSB_ADDR = 0x28,
    BNO055_LINEAR_ACCEL_DATA_X_MSB_ADDR = 0x29,
    BNO055_LINEAR_ACCEL_DATA_Y_LSB_ADDR = 0x2A,
    BNO055_LINEAR_ACCEL_DATA_Y_MSB_ADDR = 0x2B,
    BNO055_LINEAR_ACCEL_DATA_Z_LSB_ADDR = 0x2C,
    BNO055_LINEAR_ACCEL_DATA_Z_MSB_ADDR = 0x2D,
    BNO055_GRAVITY_DATA_X_LSB_ADDR = 0x2E,
    BNO055_GRAVITY_DATA_X_MSB_ADDR = 0x2F,
    BNO055_GRAVITY_DATA_Y_LSB_ADDR = 0x30,
    BNO055_GRAVITY_DATA_Y_MSB_ADDR = 0x31,
    BNO055_GRAVITY_DATA_Z_LSB_ADDR = 0x32,
    BNO055_GRAVITY_DATA_Z_MSB_ADDR = 0x33,
    BNO055_TEMP_ADDR = 0x34,
    BNO055_CALIB_STAT_ADDR = 0x35,
    BNO055_SELFTEST_RESULT_ADDR = 0x36,
    BNO055_INTR_STAT_ADDR = 0x37,
    BNO055_SYS_CLK_STAT_ADDR = 0x38,
    BNO055_SYS_STAT_ADDR = 0x39,
    BNO055_SYS_ERR_ADDR = 0x3A,
    BNO055_UNIT_SEL_ADDR = 0x3B,
    BNO055_DATA_SELECT_ADDR = 0x3C,
    BNO055_OPR_MODE_ADDR = 0x3D,
    BNO055_PWR_MODE_ADDR = 0x3E,
    BNO055_SYS_TRIGGER_ADDR = 0x3F,
    BNO055_TEMP_SOURCE_ADDR = 0x40,
    BNO055_AXIS_MAP_CONFIG_ADDR = 0x41,
    BNO055_AXIS_MAP_SIGN_ADDR = 0x42,
    BNO055_SIC_MATRIX_0_LSB_ADDR = 0x43,
    BNO055_SIC_MATRIX_0_MSB_ADDR = 0x44,
    BNO055_SIC_MATRIX_1_LSB_ADDR = 0x45,
    BNO055_SIC_MATRIX_1_MSB_ADDR = 0x46,
    BNO055_SIC_MATRIX_2_LSB_ADDR = 0x47,
    BNO055_SIC_MATRIX_2_MSB_ADDR = 0x48,
    BNO055_SIC_MATRIX_3_LSB_ADDR = 0x49,
    BNO055_SIC_MATRIX_3_MSB_ADDR = 0x4A,
    BNO055_SIC_MATRIX_4_LSB_ADDR = 0x4B,
    BNO055_SIC_MATRIX_4_MSB_ADDR = 0x4C,
    BNO055_SIC_MATRIX_5_LSB_ADDR = 0x4D,
    BNO055_SIC_MATRIX_5_MSB_ADDR = 0x4E,
    BNO055_SIC_MATRIX_6_LSB_ADDR = 0x4F,
    BNO055_SIC_MATRIX_6_MSB_ADDR = 0x50,
    BNO055_SIC_MATRIX_7_LSB_ADDR = 0x51,
    BNO055_SIC_MATRIX_7_MSB_ADDR = 0x52,
    BNO055_SIC_MATRIX_8_LSB_ADDR = 0x53,
    BNO055_SIC_MATRIX_8_MSB_ADDR = 0x54,
    ACCEL_OFFSET_X_LSB_ADDR = 0x55,
    ACCEL_OFFSET_X_MSB_ADDR = 0x56,
    ACCEL_OFFSET_Y_LSB_ADDR = 0x57,
    ACCEL_OFFSET_Y_MSB_ADDR = 0x58,
    ACCEL_OFFSET_Z_LSB_ADDR = 0x59,
    ACCEL_OFFSET_Z_MSB_ADDR = 0x5A,
    MAG_OFFSET_X_LSB_ADDR = 0x5B,
    MAG_OFFSET_X_MSB_ADDR = 0x5C,
    MAG_OFFSET_Y_LSB_ADDR = 0x5D,
    MAG_OFFSET_Y_MSB_ADDR = 0x5E,
    MAG_OFFSET_Z_LSB_ADDR = 0x5F,
    MAG_OFFSET_Z_MSB_ADDR = 0x60,
    GYRO_OFFSET_X_LSB_ADDR = 0x61,
    GYRO_OFFSET_X_MSB_ADDR = 0x62,
    GYRO_OFFSET_Y_LSB_ADDR = 0x63,
    GYRO_OFFSET_Y_MSB_ADDR = 0x64,
    GYRO_OFFSET_Z_LSB_ADDR = 0x65,
    GYRO_OFFSET_Z_MSB_ADDR = 0x66,
    ACCEL_RADIUS_LSB_ADDR = 0x67,
    ACCEL_RADIUS_MSB_ADDR = 0x68,
    MAG_RADIUS_LSB_ADDR = 0x69,
    MAG_RADIUS_MSB_ADDR = 0x6A
};

void bno055_init(void) {
    bno055_uart_init();
}

void bno055_write_opr_mode(bno055_opr_mode_t op_mode, bno_callback_t callback) {
    uint8_t op_mode_cmd = (uint8_t) op_mode;
    bno055_uart_write_register(BNO055_OPR_MODE_ADDR, &op_mode_cmd, 1, callback);
}

void bno055_write_unit_selection(bno055_unit_sel_acc acc_unit, bno055_unit_sel_angular_rate angular_rate_unit,
                                 bno055_unit_sel_euler_angles euler_angles_unit,
                                 bno055_unit_sel_temperature temperature_unit,
                                 bno055_unit_sel_orientation_def orientation_def, bno_callback_t callback) {
    uint8_t unit_sel = acc_unit | (angular_rate_unit << 1U) | (euler_angles_unit << 2U) | (temperature_unit << 4U) |
                       (orientation_def << 7);
    bno055_uart_write_register(BNO055_UNIT_SEL_ADDR, &unit_sel, 1, callback);
}

void bno055_write_reset(bno_callback_t callback) {
    uint8_t cmd = 1U << 5U;
    bno055_uart_write_register(BNO055_SYS_TRIGGER_ADDR, &cmd, 1, callback);
}

void bno055_read_system_status(bno055_status_t *out, bno_callback_t callback) {
    bno055_uart_read_register(BNO055_SYS_STAT_ADDR, 1, callback, out, 1);
}

void bno055_read_system_error(bno055_error_t *out, bno_callback_t callback) {
    bno055_uart_read_register(BNO055_SYS_ERR_ADDR, 1, callback, out, 1);
}

void bno055_read_calib_status(uint8_t *out, bno_callback_t callback) {
    bno055_uart_read_register(BNO055_CALIB_STAT_ADDR, 1, callback, out, 1);
}

void bno055_write_remap_axis(bno055_axis_remap_axis_t new_x, bno055_axis_remap_axis_t new_y,
                             bno055_axis_remap_axis_t new_z, bno_callback_t callback) {
    uint8_t remap_reg = (new_z << 4) | (new_y << 2) | new_x;
    bno055_uart_write_register(BNO055_AXIS_MAP_CONFIG_ADDR, &remap_reg, 1, callback);
}

void bno055_write_remap_axis_sign(bno055_axis_remap_sign_t new_x_sign, bno055_axis_remap_sign_t new_y_sign,
                                  bno055_axis_remap_sign_t new_z_sign, bno_callback_t callback) {
    uint8_t remap_reg = (new_x_sign << 2) | (new_y_sign << 1) | new_z_sign;
    bno055_uart_write_register(BNO055_AXIS_MAP_SIGN_ADDR, &remap_reg, 1, callback);
}

void bno055_read_self_test(uint8_t *out, bno_callback_t callback) {
    bno055_uart_read_register(BNO055_SELFTEST_RESULT_ADDR, 1, callback, out, 1);
}

void bno055_read_acc_x_mul_100(int16_t *out, bno_callback_t callback) {
    bno055_uart_read_register(BNO055_ACCEL_DATA_X_LSB_ADDR, 2, callback, out, 1);
}

void bno055_read_acc_y_mul_100(int16_t *out, bno_callback_t callback) {
    bno055_uart_read_register(BNO055_ACCEL_DATA_Y_LSB_ADDR, 2, callback, out, 1);
}

void bno055_read_acc_z_mul_100(int16_t *out, bno_callback_t callback) {
    bno055_uart_read_register(BNO055_ACCEL_DATA_Z_LSB_ADDR, 2, callback, out, 1);
}

void bno055_read_gyr_x_mul_16(int16_t *out, bno_callback_t callback) {
    bno055_uart_read_register(BNO055_GYRO_DATA_X_LSB_ADDR, 2, callback, out, 1);
}

void bno055_read_gyr_y_mul_16(int16_t *out, bno_callback_t callback) {
    bno055_uart_read_register(BNO055_GYRO_DATA_Y_LSB_ADDR, 2, callback, out, 1);
}

void bno055_read_gyr_z_mul_16(int16_t *out, bno_callback_t callback) {
    bno055_uart_read_register(BNO055_GYRO_DATA_Z_LSB_ADDR, 2, callback, out, 1);
}

void bno055_read_eul_x_2_mul_16(int16_t *out, bno_callback_t callback) {
    bno055_uart_read_register(BNO055_EULER_X_LSB_ADDR, 2, callback, out, 1);
}

void bno055_read_eul_y_2_mul_16(int16_t *out, bno_callback_t callback) {
    bno055_uart_read_register(BNO055_EULER_Y_LSB_ADDR, 2, callback, out, 1);
}

void bno055_read_eul_z_2_mul_16(int16_t *out, bno_callback_t callback) {
    bno055_uart_read_register(BNO055_EULER_Z_LSB_ADDR, 2, callback, out, 1);
}
