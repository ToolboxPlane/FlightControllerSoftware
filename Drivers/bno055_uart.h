/**
 * @file bno_055_uart.h
 * @author paul
 * @date 17.04.19
 * @brief bno_055_uart @TODO
 */

#ifndef FLIGHTCONTROLLER_BNO055_UART_H
#define FLIGHTCONTROLLER_BNO055_UART_H

#include <stdint.h>

void bno055_init(void);

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
