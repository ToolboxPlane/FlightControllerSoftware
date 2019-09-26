/**
 * @file bno_055_uart.h
 * @author paul
 * @date 17.04.19
 * @brief bno_055_uart @TODO
 */

#ifndef FLIGHTCONTROLLER_BNO055_UART_H
#define FLIGHTCONTROLLER_BNO055_UART_H

#include <stdint.h>

#define BNO055_STATUS_SYSTEM_IDLE 0
#define BNO055_STATUS_SYSTEM_ERROR 1
#define BNO055_STATUS_INITIALIZING_PERIPHERALS 2
#define BNO055_STATUS_SYSTEM_INITIALIZATION 3
#define BNO055_STATUS_EXECUTING_SELFTEST 4
#define BNO055_STATUS_SENSOR_FUSION_ALGORITHM_RUNNING 5
#define BNO055_STATUS_SYSTEM_RUNNING_WITHOUT_FUSION_ALGORITHM 6

#define BNO055_ERROR_NO_ERROR 0
#define BNO055_ERROR_PERIPHERAL_INITIALIZATION_ERROR 1
#define BNO055_ERROR_SYSTEM_INITIALIZATION_ERROR 2
#define BNO055_ERROR_SELF_TEST_RESULT_FAILED 3
#define BNO055_ERROR_REGISTER_MAP_VALUE_OUT_OF_RANGE 4
#define BNO055_ERROR_REGISTER_MAP_ADDRESS_OUT_OF_RANGE 5
#define BNO055_ERROR_REGISTER_MAP_WRITE_ERROR 6
#define BNO055_ERROR_BNO_LOW_POWER_MODE_NOT_AVAILABLE_FOR_SELECTED_OPERATION_MODE 7
#define BNO055_ERROR_ACCELEROMETER_POWER_MODE_NOT_AVAILABLE 8
#define BNO055_ERROR_FUSION_ALGORITHM_CONFIGURATION_ERROR 9
#define BNO055_ERROR_SENSOR_CONFIGURATION_ERROR 0xA

void bno055_init(void);

void bno055_reset(void);

uint8_t bno055_status(void);
uint8_t bno055_error(void);

int16_t bno055_acc_x(void);
int16_t bno055_acc_y(void);
int16_t bno055_acc_z(void);

int16_t bno055_mag_x(void);
int16_t bno055_mag_y(void);
int16_t bno055_mag_z(void);

int16_t bno055_gyr_x(void);
int16_t bno055_gyr_y(void);
int16_t bno055_gyr_z(void);

int16_t bno055_eul_x(void);
int16_t bno055_eul_y(void);
int16_t bno055_eul_z(void);

int16_t bno055_linear_acc_x(void);
int16_t bno055_linear_acc_y(void);
int16_t bno055_linear_acc_z(void);

int16_t bno055_grv_x(void);
int16_t bno055_grv_y(void);
int16_t bno055_grv_z(void);

uint8_t bno055_calib_stat(void);

#endif //FLIGHTCONTROLLER_BNO055_UART_H
