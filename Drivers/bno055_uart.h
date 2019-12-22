/**
 * @file bno_055_uart.h
 * @author paul
 * @date 17.04.19
 * @brief bno_055_uart @TODO
 */

#ifndef FLIGHTCONTROLLER_BNO055_UART_H
#define FLIGHTCONTROLLER_BNO055_UART_H

#include <stdint.h>

typedef enum {
    system_idle = 0x0,
    system_error = 0x1,
    initializing_peripherals = 0x2,
    system_initialization = 0x3,
    executing_selftest = 0x4,
    sensor_fusion_algorithm_running = 0x5,
    system_running_without_fusion_algorithm = 0x6,
    unknown_status,
} bno055_status_t;

typedef enum {
    no_error = 0x0,
    peripheral_initialization_error = 0x1,
    system_initialization_error = 0x2,
    self_test_result_failed = 0x3,
    register_map_value_out_of_range = 0x4,
    register_map_address_out_of_range = 0x5,
    register_map_write_error = 0x6,
    bno_low_power_mode_not_available_for_selected_operation_mode = 0x7,
    accelerometer_power_mode_not_available = 0x8,
    fusion_algorithm_configuration_error = 0x9,
    sensor_configuration_error = 0xa,
    unknown_error
} bno055_error_t;

void bno055_init(void);

void bno055_reset(void);

bno055_status_t bno055_status(void);
bno055_error_t bno055_error(void);
uint8_t bno055_calib_stat(void);

int16_t bno055_acc_x(void);
int16_t bno055_acc_y(void);
int16_t bno055_acc_z(void);

int16_t bno055_mag_x(void);
int16_t bno055_mag_y(void);
int16_t bno055_mag_z(void);

int16_t bno055_gyr_x(void);
int16_t bno055_gyr_y(void);
int16_t bno055_gyr_z(void);

int16_t bno055_eul_x_2(void);
int16_t bno055_eul_y_2(void);
int16_t bno055_eul_z_2(void);

int16_t bno055_linear_acc_x(void);
int16_t bno055_linear_acc_y(void);
int16_t bno055_linear_acc_z(void);

int16_t bno055_grv_x(void);
int16_t bno055_grv_y(void);
int16_t bno055_grv_z(void);


#endif //FLIGHTCONTROLLER_BNO055_UART_H
