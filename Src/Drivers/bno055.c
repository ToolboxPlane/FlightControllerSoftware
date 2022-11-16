/**
 * @file bno055.c
 * @author paul
 * @date 01.11.22
 * Description here TODO
 */
#include "bno055.h"

#define BNO055_CHIP_ID_ADDR 0x00
#define BNO055_ACCEL_REV_ID_ADDR 0x01
#define BNO055_MAG_REV_ID_ADDR 0x02
#define BNO055_GYRO_REV_ID_ADDR 0x03
#define BNO055_SW_REV_ID_LSB_ADDR 0x04
#define BNO055_SW_REV_ID_MSB_ADDR 0x05
#define BNO055_BL_REV_ID_ADDR 0x06
#define BNO055_PAGE_ID_ADDR 0x07
#define BNO055_ACCEL_DATA_X_LSB_ADDR 0x08
#define BNO055_ACCEL_DATA_X_MSB_ADDR 0x09
#define BNO055_ACCEL_DATA_Y_LSB_ADDR 0x0A
#define BNO055_ACCEL_DATA_Y_MSB_ADDR 0x0B
#define BNO055_ACCEL_DATA_Z_LSB_ADDR 0x0C
#define BNO055_ACCEL_DATA_Z_MSB_ADDR 0x0D
#define BNO055_MAG_DATA_X_LSB_ADDR 0x0E
#define BNO055_MAG_DATA_X_MSB_ADDR 0x0F
#define BNO055_MAG_DATA_Y_LSB_ADDR 0x10
#define BNO055_MAG_DATA_Y_MSB_ADDR 0x11
#define BNO055_MAG_DATA_Z_LSB_ADDR 0x12
#define BNO055_MAG_DATA_Z_MSB_ADDR 0x13
#define BNO055_GYRO_DATA_X_LSB_ADDR 0x14
#define BNO055_GYRO_DATA_X_MSB_ADDR 0x15
#define BNO055_GYRO_DATA_Y_LSB_ADDR 0x16
#define BNO055_GYRO_DATA_Y_MSB_ADDR 0x17
#define BNO055_GYRO_DATA_Z_LSB_ADDR 0x18
#define BNO055_GYRO_DATA_Z_MSB_ADDR 0x19
#define BNO055_EULER_X_LSB_ADDR 0x1A
#define BNO055_EULER_X_MSB_ADDR 0x1B
#define BNO055_EULER_Y_LSB_ADDR 0x1C
#define BNO055_EULER_Y_MSB_ADDR 0x1D
#define BNO055_EULER_Z_LSB_ADDR 0x1E
#define BNO055_EULER_Z_MSB_ADDR 0x1F
#define BNO055_QUATERNION_DATA_W_LSB_ADDR 0x20
#define BNO055_QUATERNION_DATA_W_MSB_ADDR 0x21
#define BNO055_QUATERNION_DATA_X_LSB_ADDR 0x22
#define BNO055_QUATERNION_DATA_X_MSB_ADDR 0x23
#define BNO055_QUATERNION_DATA_Y_LSB_ADDR 0x24
#define BNO055_QUATERNION_DATA_Y_MSB_ADDR 0x25
#define BNO055_QUATERNION_DATA_Z_LSB_ADDR 0x26
#define BNO055_QUATERNION_DATA_Z_MSB_ADDR 0x27
#define BNO055_LINEAR_ACCEL_DATA_X_LSB_ADDR 0x28
#define BNO055_LINEAR_ACCEL_DATA_X_MSB_ADDR 0x29
#define BNO055_LINEAR_ACCEL_DATA_Y_LSB_ADDR 0x2A
#define BNO055_LINEAR_ACCEL_DATA_Y_MSB_ADDR 0x2B
#define BNO055_LINEAR_ACCEL_DATA_Z_LSB_ADDR 0x2C
#define BNO055_LINEAR_ACCEL_DATA_Z_MSB_ADDR 0x2D
#define BNO055_GRAVITY_DATA_X_LSB_ADDR 0x2E
#define BNO055_GRAVITY_DATA_X_MSB_ADDR 0x2F
#define BNO055_GRAVITY_DATA_Y_LSB_ADDR 0x30
#define BNO055_GRAVITY_DATA_Y_MSB_ADDR 0x31
#define BNO055_GRAVITY_DATA_Z_LSB_ADDR 0x32
#define BNO055_GRAVITY_DATA_Z_MSB_ADDR 0x33
#define BNO055_TEMP_ADDR 0x34
#define BNO055_CALIB_STAT_ADDR 0x35
#define BNO055_SELFTEST_RESULT_ADDR 0x36
#define BNO055_INTR_STAT_ADDR 0x37
#define BNO055_SYS_CLK_STAT_ADDR 0x38
#define BNO055_SYS_STAT_ADDR 0x39
#define BNO055_SYS_ERR_ADDR 0x3A
#define BNO055_UNIT_SEL_ADDR 0x3B
#define BNO055_DATA_SELECT_ADDR 0x3C
#define BNO055_OPR_MODE_ADDR 0x3D
#define BNO055_PWR_MODE_ADDR 0x3E
#define BNO055_SYS_TRIGGER_ADDR 0x3F
#define BNO055_TEMP_SOURCE_ADDR 0x40
#define BNO055_AXIS_MAP_CONFIG_ADDR 0x41
#define BNO055_AXIS_MAP_SIGN_ADDR 0x42
#define BNO055_SIC_MATRIX_0_LSB_ADDR 0x43
#define BNO055_SIC_MATRIX_0_MSB_ADDR 0x44
#define BNO055_SIC_MATRIX_1_LSB_ADDR 0x45
#define BNO055_SIC_MATRIX_1_MSB_ADDR 0x46
#define BNO055_SIC_MATRIX_2_LSB_ADDR 0x47
#define BNO055_SIC_MATRIX_2_MSB_ADDR 0x48
#define BNO055_SIC_MATRIX_3_LSB_ADDR 0x49
#define BNO055_SIC_MATRIX_3_MSB_ADDR 0x4A
#define BNO055_SIC_MATRIX_4_LSB_ADDR 0x4B
#define BNO055_SIC_MATRIX_4_MSB_ADDR 0x4C
#define BNO055_SIC_MATRIX_5_LSB_ADDR 0x4D
#define BNO055_SIC_MATRIX_5_MSB_ADDR 0x4E
#define BNO055_SIC_MATRIX_6_LSB_ADDR 0x4F
#define BNO055_SIC_MATRIX_6_MSB_ADDR 0x50
#define BNO055_SIC_MATRIX_7_LSB_ADDR 0x51
#define BNO055_SIC_MATRIX_7_MSB_ADDR 0x52
#define BNO055_SIC_MATRIX_8_LSB_ADDR 0x53
#define BNO055_SIC_MATRIX_8_MSB_ADDR 0x54
#define ACCEL_OFFSET_X_LSB_ADDR 0x55
#define ACCEL_OFFSET_X_MSB_ADDR 0x56
#define ACCEL_OFFSET_Y_LSB_ADDR 0x57
#define ACCEL_OFFSET_Y_MSB_ADDR 0x58
#define ACCEL_OFFSET_Z_LSB_ADDR 0x59
#define ACCEL_OFFSET_Z_MSB_ADDR 0x5A
#define MAG_OFFSET_X_LSB_ADDR 0x5B
#define MAG_OFFSET_X_MSB_ADDR 0x5C
#define MAG_OFFSET_Y_LSB_ADDR 0x5D
#define MAG_OFFSET_Y_MSB_ADDR 0x5E
#define MAG_OFFSET_Z_LSB_ADDR 0x5F
#define MAG_OFFSET_Z_MSB_ADDR 0x60
#define GYRO_OFFSET_X_LSB_ADDR 0x61
#define GYRO_OFFSET_X_MSB_ADDR 0x62
#define GYRO_OFFSET_Y_LSB_ADDR 0x63
#define GYRO_OFFSET_Y_MSB_ADDR 0x64
#define GYRO_OFFSET_Z_LSB_ADDR 0x65
#define GYRO_OFFSET_Z_MSB_ADDR 0x66
#define ACCEL_RADIUS_LSB_ADDR 0x67
#define ACCEL_RADIUS_MSB_ADDR 0x68
#define MAG_RADIUS_LSB_ADDR 0x69
#define MAG_RADIUS_MSB_ADDR 0x6A

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
    uint8_t unit_sel = acc_unit | (angular_rate_unit << 1) | (euler_angles_unit << 2) | (temperature_unit << 4) |
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
