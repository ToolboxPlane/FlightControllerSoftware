/**
 * @file bno_055_uart.c
 * @author paul
 * @date 17.04.19
 * @brief bno_055_uart @TODO
 */

#include "bno055_uart.h"
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    read_success = 0x00,
    write_success = 0x01,
    read_fail = 0x02,
    write_fail = 0x03,
    regmap_invalid_address = 0x04,
    regmap_write_disabled = 0x05,
    wrong_start_byte = 0x06,
    bus_over_run_error = 0x07,
    max_length_error = 0x08,
    min_length_error = 0x09,
    receive_char_timeout = 0x0A
} bno055_response_t;

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

bool transaction_finished = false;
uint8_t *receive_buf;
bno055_response_t response;

void uart_callback(uint8_t data) {
    static uint8_t byte_in_message = 0;
    static bool acknowledge_or_failure = false;
    static uint8_t still_to_read = 0;

    switch (byte_in_message) {
        case 0: // Header
            if (data == 0xEE) {
                acknowledge_or_failure = true;
                byte_in_message = 1;
            } else if (data == 0xBB) {
                acknowledge_or_failure = false;
                byte_in_message = 1;
            } else {
                byte_in_message = 0;
            }
            break;
        case 1:
            if (acknowledge_or_failure) {
                response = data;
                byte_in_message = 0;
            } else {
                response = read_success;
                still_to_read = data;
                byte_in_message = 2;
            }
            break;
        default:
            receive_buf[byte_in_message - 2] = data;
            --still_to_read;
            ++byte_in_message;
            if (still_to_read == 0) {
                byte_in_message = 0;
                transaction_finished = true;
            }
            break;

    }
}

bool bno055_write_register(uint8_t reg, const uint8_t *data, uint8_t len) {
    uint8_t buf[len + 4];
    buf[0] = 0xAA;
    buf[1] = 0x00;
    buf[2] = reg;
    buf[3] = len;
    for (uint8_t c = 0; c<len; c++) {
        buf[c + 4] = data[c];
    }
    transaction_finished = false;
    //uart2_send_buf(buf, len+4);
    while (!transaction_finished);
    return response == write_success;
}

bool bno055_read_register(uint8_t reg, uint8_t *data, uint8_t len) {
    receive_buf = data;
    uint8_t buf[4] = {0xAA, 0x01, reg, len};
    transaction_finished = false;
    //uart2_send_buf(buf, 4);
    while (!transaction_finished);
    return response == read_success;
}

bool bno055_read_word(uint8_t reg) {
    uint8_t buf[2];
    bno055_read_register(reg, buf, 2);
    return buf[1] << 8 | buf[0];
}

bool bno055_write_byte(uint8_t reg, uint8_t byte) {
    return bno055_write_register(reg, &byte, 1);
}

void bno055_init(void) {
    //uart2_init(115200, &uart_callback);
    /*
     * Units:
     *  * Acceleration: m/s^2
     *  * Angular-Rate: Dps
     *  * Angles: Degrees
     *  * Temperature: Celsius
     *  * Data output format: Android //@TODO don't know if right
     */
    bno055_write_byte(BNO055_UNIT_SEL_ADDR, 0b0000000);
    bno055_write_byte(BNO055_OPR_MODE_ADDR, 0b0001011); // Switch to NDOF-FMC-OFF Mode
}

int16_t bno055_acc_x(void) {
    return bno055_read_word(BNO055_ACCEL_DATA_X_LSB_ADDR);
}

int16_t bno055_acc_y(void) {
    return bno055_read_word(BNO055_ACCEL_DATA_Y_LSB_ADDR);
}

int16_t bno055_acc_z(void) {
    return bno055_read_word(BNO055_ACCEL_DATA_Z_LSB_ADDR);
}

int16_t bno055_mag_x(void) {
    return bno055_read_word(BNO055_MAG_DATA_X_LSB_ADDR);
}

int16_t bno055_mag_y(void) {
    return bno055_read_word(BNO055_MAG_DATA_Y_LSB_ADDR);
}

int16_t bno055_mag_z(void) {
    return bno055_read_word(BNO055_MAG_DATA_Z_LSB_ADDR);
}

int16_t bno055_gyr_x(void) {
    return bno055_read_word(BNO055_GYRO_DATA_X_LSB_ADDR);
}

int16_t bno055_gyr_y(void) {
    return bno055_read_word(BNO055_GYRO_DATA_Y_LSB_ADDR);
}

int16_t bno055_gyr_z(void) {
    return bno055_read_word(BNO055_GYRO_DATA_Z_LSB_ADDR);
}

int16_t bno055_eul_x(void) {
    return bno055_read_word(BNO055_EULER_X_LSB_ADDR) / 16;
}

int16_t bno055_eul_y(void) {
    return bno055_read_word(BNO055_EULER_Y_LSB_ADDR) / 16;
}

int16_t bno055_eul_z(void) {
    return bno055_read_word(BNO055_EULER_Z_LSB_ADDR) / 16;
}

int16_t bno055_linear_acc_x(void) {
    return bno055_read_word(BNO055_LINEAR_ACCEL_DATA_X_LSB_ADDR) / 100;
}

int16_t bno055_linear_acc_y(void) {
    return bno055_read_word(BNO055_LINEAR_ACCEL_DATA_Y_LSB_ADDR) / 100;
}

int16_t bno055_linear_acc_z(void) {
    return bno055_read_word(BNO055_LINEAR_ACCEL_DATA_Z_LSB_ADDR) / 100;
}

int16_t bno055_grv_x(void) {
    return bno055_read_word(BNO055_GRAVITY_DATA_X_LSB_ADDR) / 100;
}

int16_t bno055_grv_y(void) {
    return bno055_read_word(BNO055_GRAVITY_DATA_Y_LSB_ADDR) / 100;
}

int16_t bno055_grv_z(void) {
    return bno055_read_word(BNO055_GRAVITY_DATA_Z_LSB_ADDR) / 100;
}

uint8_t bno055_calib_stat(void) {
    uint8_t res;
    bno055_read_register(BNO055_CALIB_STAT_ADDR, &res, 1);
    return res;
}
