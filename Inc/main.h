/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */
#include <stdint.h>

#define BNO055_HEADING (((uint16_t)currBnoBuffer[0x1B] << 8 | currBnoBuffer[0x1A])/16.0f)
#define BNO055_ROLL ((int16_t)((uint16_t)currBnoBuffer[0x1D] << 8 | currBnoBuffer[0x1C])/16.0f)
#define BNO055_HEADING_PM (BNO055_HEADING > 180 ? BNO055_HEADING-360 : BNO055_HEADING);
#define BNO055_PITCH ((int16_t)((uint16_t)currBnoBuffer[0x1F] << 8 | currBnoBuffer[0x1E])/16.0f)

#define BNO055_GYRO_X ((int16_t)((uint16_t)currBnoBuffer[0x19] << 8 | currBnoBuffer[0x18]))
#define BNO055_GYRO_Y ((int16_t)((uint16_t)currBnoBuffer[0x17] << 8 | currBnoBuffer[0x16]))
#define BNO055_GYRO_Z ((int16_t)((uint16_t)currBnoBuffer[0x15] << 8 | currBnoBuffer[0x14]))

#define BNO055_TEMP (currBnoBuffer[0x34]);
#define BNO055_CALIBSTATUS (currBnoBuffer[0x35]);
#define BNO055_OPMODE (currBnoBuffer[0x3D]&(uint8_t)0b1111)
#define BNO055_SYSERR (currBnoBuffer[0x3A])

#define MPL_HEIGHT (((uint16_t) currMplBuffer[1] << 8 | currMplBuffer[2]) + (currMplBuffer[3] >> 4) / 16.0f)
#define MPL_TEMP (currMplBuffer[4] + (currMplBuffer[5] >> 4) / 16.0f)

extern uint8_t *currBnoBuffer;
extern int16_t servoPosition[5];

enum Servos {
    VTAIL_R,
    VTAIL_L,
    AILERON_L,
    AILERON_R,
    MOTOR
};
/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define MCO_Pin GPIO_PIN_0
#define MCO_GPIO_Port GPIOA
#define PPM_AILERON_R_Pin GPIO_PIN_1
#define PPM_AILERON_R_GPIO_Port GPIOA
#define VCP_TX_Pin GPIO_PIN_2
#define VCP_TX_GPIO_Port GPIOA
#define VCP_RX_Pin GPIO_PIN_3
#define VCP_RX_GPIO_Port GPIOA
#define PIN_A3_Pin GPIO_PIN_4
#define PIN_A3_GPIO_Port GPIOA
#define PIN_A4_Pin GPIO_PIN_5
#define PIN_A4_GPIO_Port GPIOA
#define PPM_VTAIL_L_Pin GPIO_PIN_6
#define PPM_VTAIL_L_GPIO_Port GPIOA
#define PPM_AILERON_L_Pin GPIO_PIN_7
#define PPM_AILERON_L_GPIO_Port GPIOA
#define PPM_VTAIL_R_Pin GPIO_PIN_0
#define PPM_VTAIL_R_GPIO_Port GPIOB
#define PPM_MOTOR_Pin GPIO_PIN_1
#define PPM_MOTOR_GPIO_Port GPIOB
#define SBUS_TX_Pin GPIO_PIN_9
#define SBUS_TX_GPIO_Port GPIOA
#define SBUS_RX_Pin GPIO_PIN_10
#define SBUS_RX_GPIO_Port GPIOA
#define POWER_DST_CS_Pin GPIO_PIN_11
#define POWER_DST_CS_GPIO_Port GPIOA
#define PIN_D2_Pin GPIO_PIN_12
#define PIN_D2_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define I2C_SCL_Pin GPIO_PIN_6
#define I2C_SCL_GPIO_Port GPIOB
#define I2C_SDA_Pin GPIO_PIN_7
#define I2C_SDA_GPIO_Port GPIOB

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */

void controller_tick();
void handle_usart();

/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
