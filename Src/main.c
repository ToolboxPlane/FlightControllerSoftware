/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l4xx_hal.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "rc_lib.h"
#include "controller.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
uint8_t bnoBuffer0[64], bnoBuffer1[64];
uint8_t *currBnoBuffer;
uint8_t mplBuffer0[6], mplBuffer1[6];
uint8_t *currMplBuffer;

int16_t servoPosition[5]; ///< Values  between -500 and 500

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
uint8_t buf[6];

bool handle_i2c() {
    static uint8_t state = 0;

    if(hi2c1.State != HAL_I2C_STATE_READY) {
        return true;
    }

    switch (state++) {
        case 0:
        {
            uint8_t reg = 0;
            HAL_I2C_Master_Transmit_DMA(&hi2c1, 0x28 << 1, &reg, sizeof(reg));
        }
            break;
        case 1:
            if(currBnoBuffer == bnoBuffer0) {
                currBnoBuffer = bnoBuffer1;
                HAL_I2C_Master_Receive_DMA(&hi2c1, 0x28 << 1, bnoBuffer0, sizeof(bnoBuffer0));
            } else {
                currBnoBuffer = bnoBuffer0;
                HAL_I2C_Master_Receive_DMA(&hi2c1, 0x28 << 1, bnoBuffer1, sizeof(bnoBuffer1));
            }
            break;
        case 2:
        {
            uint8_t reg = 0;
            HAL_I2C_Master_Transmit_DMA(&hi2c1, 0xC0, &reg, sizeof(reg));
        }
            break;
        case 3:
            if (currMplBuffer ==  mplBuffer0) {
                currMplBuffer = mplBuffer1;
                HAL_I2C_Master_Receive_DMA(&hi2c1, 0xC0, mplBuffer0, sizeof(mplBuffer0));
            } else {
                currMplBuffer = mplBuffer0;
                HAL_I2C_Master_Receive_DMA(&hi2c1, 0xC0, mplBuffer1, sizeof(mplBuffer1));
            }
            break;
        default:
            state = 0;
            return true;
    }
    return false;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  MX_TIM16_Init();
  MX_TIM1_Init();
  MX_SPI3_Init();
  /* USER CODE BEGIN 2 */

    //BNO-055 Konfigurieren
    uint8_t cmd[2];

    /* cmd[0] = 0x3F; // SYS_TRIGGER
     cmd[1] = 0b1 << 5; //RST_SYS
     HAL_I2C_Master_Transmit_DMA(&hi2c1, 0x28  << 1, cmd, sizeof(cmd));
     while (hi2c1.State != HAL_I2C_STATE_READY);*/

    HAL_Delay(1000);

    cmd[0] = 0x3B; //UNIT_SEL
    cmd[1] = 0 << 0 | 1 << 1 | 0 << 2
             | 0 << 4 | 0 << 7;

    HAL_I2C_Master_Transmit_DMA(&hi2c1, 0x28  << 1, cmd, sizeof(cmd));
    while (hi2c1.State != HAL_I2C_STATE_READY);

    cmd[0] = 0x3D;  //OPR_MODE
    cmd[1] = 0b1011;

    HAL_I2C_Master_Transmit_DMA(&hi2c1, 0x28  << 1, cmd, sizeof(cmd));
    while (hi2c1.State != HAL_I2C_STATE_READY);

    currBnoBuffer = bnoBuffer0;
    currMplBuffer = mplBuffer0;

    HAL_Delay(20);

    // MPL konfigurieren
    cmd[0] = 0x26; // CTRL-Reg
    cmd[1] = (0b1 << 7 | 0b0 << 3); // Oversampling x1/Altimeter
    HAL_I2C_Master_Transmit_DMA(&hi2c1, 0xC0, cmd, sizeof(cmd));
    while (hi2c1.State != HAL_I2C_STATE_READY);

    // Enable Data Ready Flags
    cmd[0] = 0x13;
    cmd[1] = 0b111;
    HAL_I2C_Master_Transmit_DMA(&hi2c1, 0xC0, cmd, sizeof(cmd));
    while (hi2c1.State != HAL_I2C_STATE_READY);

    // Set active
    cmd[0] = 0x26;
    cmd[1] = 0xB9;
    HAL_I2C_Master_Transmit_DMA(&hi2c1, 0xC0, cmd, sizeof(cmd));
    HAL_Delay(1000);

    // Timer starten
    HAL_TIM_Base_Start(&htim1);
    HAL_TIM_Base_Start(&htim2);
    HAL_TIM_Base_Start(&htim16);

    HAL_TIM_Base_Start_IT(&htim1);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

    rc_lib_package_t transmit_package;
    transmit_package.mesh = false;
    transmit_package.channel_count = 16;
    transmit_package.resolution = 1024;
    transmit_package.routing_length = 0;

    rc_lib_transmitter_id = 23;

    init_all_controller();

    int16_t headingTarget = 0;

    while (1) {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
        if(handle_i2c()) {
            transmit_package.channel_data[0] = (uint16_t)(BNO055_HEADING);
            transmit_package.channel_data[1] = (uint16_t)(BNO055_ROLL+180);
            transmit_package.channel_data[2] = (uint16_t)(BNO055_PITCH+180);
            transmit_package.channel_data[3] = (uint16_t)17;
            transmit_package.channel_data[4] = (uint16_t)MPL_HEIGHT;
            transmit_package.channel_data[5] = BNO055_CALIBSTATUS;
            transmit_package.channel_data[6] = 0;
            transmit_package.channel_data[7] = 0;
            transmit_package.channel_data[8] = 0;
            transmit_package.channel_data[9] = 0;
            transmit_package.channel_data[10] = 0;
            transmit_package.channel_data[11] = 0;
            transmit_package.channel_data[12] = 0;
            transmit_package.channel_data[13] = 0;
            transmit_package.channel_data[14] = 0;
            transmit_package.channel_data[15] = 0;

            uint8_t buf[] = {1, 2, 3, 0};
            HAL_GPIO_WritePin(POWER_DST_CS_GPIO_Port, POWER_DST_CS_Pin, GPIO_PIN_RESET);
            HAL_SPI_TransmitReceive(&hspi3, buf, buf, sizeof(buf), 1000);
            HAL_GPIO_WritePin(POWER_DST_CS_GPIO_Port, POWER_DST_CS_Pin, GPIO_PIN_SET);

            transmit_package.channel_data[12] = buf[0];
            transmit_package.channel_data[13] = buf[1];
            transmit_package.channel_data[14] = buf[2];
            transmit_package.channel_data[15] = buf[3];

            uint16_t length = rc_lib_encode(&transmit_package);
            HAL_UART_Transmit_DMA(&huart2, transmit_package.buffer, length);

            update_all_controller();
        }

        /*sweepState+=1;
        sweepState %= 1000;

        servoPosition[0] = servoPosition[1] = servoPosition[2] = servoPosition[3] =
            servoPosition[4] = sweepState - (int16_t)500;*/

        pitch_controller.target_value = 0;
        int16_t angleDiff = (int16_t) ((headingTarget - (int16_t)BNO055_HEADING + 360) % 360);
        if(angleDiff > 180) {
            angleDiff -= 180;
        }
        if(angleDiff < 0) {
            roll_controller.target_value = -angleDiff > 30 ? 30 : -angleDiff;
        } else {
            roll_controller.target_value = angleDiff > 30 ? 30 : angleDiff;
        }

        update_all_controller();

        TIM2->CCR2 = (uint32_t)1500 + servoPosition[AILERON_R];
        TIM16->CCR1 = (uint32_t)1500 + servoPosition[VTAIL_L];
    }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Configure LSE Drive Capability 
    */
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the main internal regulator output voltage 
    */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    /**Enable MSI Auto calibration 
    */
  HAL_RCCEx_EnableMSIPLLMode();

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    while (1) {
    }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
