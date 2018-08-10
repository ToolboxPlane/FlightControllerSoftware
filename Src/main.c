/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l4xx_hal.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "rc_lib.h"
#include "controller.h"
#include "sbus.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
uint8_t bnoBuffer0[64], bnoBuffer1[64];
uint8_t *currBnoBuffer;
uint8_t mplBuffer0[6], mplBuffer1[6];
uint8_t *currMplBuffer;
uint8_t sBusReceiveBuffer[25];
uint8_t flightComputerReceiveBuffer[16];

int16_t servoPosition[5]; ///< Values  between -500 and 500
uint32_t new_adc_reading_pressure;
uint32_t adc_reading_pressure;
rc_lib_package_t flight_computer_package;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

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
                // Check if values are possible
                if(bnoBuffer1[0] == 0xA0 && bnoBuffer1[1] == 0xFB && bnoBuffer1[2] == 0x32 && bnoBuffer1[3] == 0x0F) {
                    currBnoBuffer = bnoBuffer1;
                    HAL_I2C_Master_Receive_DMA(&hi2c1, 0x28 << 1, bnoBuffer0, sizeof(bnoBuffer0));
                } else {
                    HAL_I2C_Master_Receive_DMA(&hi2c1, 0x28 << 1, bnoBuffer1, sizeof(bnoBuffer1));
                }
            } else {
                if(bnoBuffer0[0] == 0xA0 && bnoBuffer0[1] == 0xFB && bnoBuffer0[2] == 0x32 && bnoBuffer0[3] == 0x0F) {
                    currBnoBuffer = bnoBuffer0;
                    HAL_I2C_Master_Receive_DMA(&hi2c1, 0x28 << 1, bnoBuffer1, sizeof(bnoBuffer1));
                } else {
                    HAL_I2C_Master_Receive_DMA(&hi2c1, 0x28 << 1, bnoBuffer0, sizeof(bnoBuffer0));
                }
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

int16_t sbusValueToServo(uint16_t sbusValue) {
    return (int16_t)((sbusValue - 168)/(1811.0-168.0) * 1000) - 500;
}

void controller_tick() {
    if(sbus_latest_data.failsave) {
        pitch_controller.target_value = 0;
        roll_controller.target_value = 0;
        servoPosition[MOTOR] = -500;
        update_all_controller();
    } else if(sbusValueToServo(sbus_latest_data.channel[10]) < -250) { // Manual
        servoPosition[MOTOR] = sbusValueToServo(sbus_latest_data.channel[0]);
        servoPosition[AILERON_L] = sbusValueToServo(sbus_latest_data.channel[1]);
        servoPosition[AILERON_R] = sbusValueToServo(sbus_latest_data.channel[2]);
        servoPosition[VTAIL_L] = sbusValueToServo(sbus_latest_data.channel[3]);
        servoPosition[VTAIL_R] = sbusValueToServo(sbus_latest_data.channel[4]);
    } else if(sbusValueToServo(sbus_latest_data.channel[10]) < 250) { // Level
        pitch_controller.target_value = -sbusValueToServo(sbus_latest_data.channel[7])/9;
        roll_controller.target_value = sbusValueToServo(sbus_latest_data.channel[8])/6;
        servoPosition[MOTOR] = sbusValueToServo(sbus_latest_data.channel[0]);
        update_all_controller();
    } else {
        servoPosition[MOTOR] = flight_computer_package.channel_data[0];
        pitch_controller.target_value = flight_computer_package.channel_data[1]-180;
        roll_controller.target_value = flight_computer_package.channel_data[2]-180;
        update_all_controller();
    }

    TIM1->CCR1 = (uint32_t)(18500 - servoPosition[AILERON_L]);
    TIM1->CCR2 = (uint32_t)(18500 - servoPosition[VTAIL_R]);
    TIM1->CCR3 = (uint32_t)(18500 - servoPosition[MOTOR]);
    TIM2->CCR2 = (uint32_t)(1500 + servoPosition[AILERON_R]);
    TIM16->CCR1 = (uint32_t)(1500 + servoPosition[VTAIL_L]);
}

void handle_usart() {
    static uint8_t send = 0;
    static rc_lib_package_t transmit_package;
    transmit_package.mesh = false;
    transmit_package.channel_count = 16;
    transmit_package.resolution = 1024;

    static rc_lib_package_t power_package;
    power_package.mesh = false;
    power_package.channel_count = 8;
    power_package.resolution = 256;


    send = (send+1) % 2;
    if(send == 0) {
        uint8_t powDstBuf[6];
        powDstBuf[0] = 1;
        powDstBuf[1] = 2;
        powDstBuf[2] = 3;
        powDstBuf[3] = 4;
        powDstBuf[4] = 5;
        powDstBuf[5] = 0;

        HAL_GPIO_WritePin(POWER_DST_CS_GPIO_Port, POWER_DST_CS_Pin, GPIO_PIN_RESET);
        HAL_SPI_TransmitReceive(&hspi3, powDstBuf, powDstBuf, sizeof(powDstBuf), 1000);
        HAL_GPIO_WritePin(POWER_DST_CS_GPIO_Port, POWER_DST_CS_Pin, GPIO_PIN_SET);

        for (uint8_t c = 0; c < 6; c++) {
            power_package.channel_data[c] = powDstBuf[c];
        }
        power_package.channel_data[6] = power_package.channel_data[7] = 0;

        uint8_t temp = rc_lib_transmitter_id;
        rc_lib_transmitter_id = 74;
        uint16_t length = rc_lib_encode(&power_package);
        rc_lib_transmitter_id = temp;
        HAL_UART_Transmit_DMA(&huart2, power_package.buffer, length);
    } else {
        volatile float adc_voltage = adc_reading_pressure*5.24f/4096.0f - 0.99f;
        volatile float airspeed = sqrtf(2*(adc_voltage)/1.2041756858f*1000);

        transmit_package.channel_data[0] = (uint16_t) (BNO055_HEADING);
        transmit_package.channel_data[1] = (uint16_t) (180 + BNO055_PITCH);
        transmit_package.channel_data[2] = (uint16_t) (180 - BNO055_ROLL);
        transmit_package.channel_data[3] = (uint16_t) (0); // Distance to Ground
        transmit_package.channel_data[4] = (uint16_t) MPL_HEIGHT;
        transmit_package.channel_data[5] = (uint16_t)(airspeed);
        transmit_package.channel_data[6] = (uint16_t) (BNO055_ACC_X + 500);
        transmit_package.channel_data[7] = (uint16_t) (BNO055_ACC_Y + 500);
        transmit_package.channel_data[8] = (uint16_t) (BNO055_ACC_Z + 500);
        transmit_package.channel_data[9] = 0;
        transmit_package.channel_data[10] = 0;
        transmit_package.channel_data[11] = (uint16_t) (servoPosition[AILERON_R] + 500);
        transmit_package.channel_data[12] = (uint16_t) (servoPosition[VTAIL_R] + 500);
        transmit_package.channel_data[13] = (uint16_t) (servoPosition[MOTOR] + 500);
        transmit_package.channel_data[14] = (uint16_t) (servoPosition[VTAIL_L] + 500);
        transmit_package.channel_data[15] = (uint16_t) (servoPosition[AILERON_L] + 500);

        uint16_t length = rc_lib_encode(&transmit_package);
        HAL_UART_Transmit_DMA(&huart2, transmit_package.buffer, length);
    }
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
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */

    //BNO-055 Konfigurieren
    uint8_t cmd[2];

    /* cmd[0] = 0x3F; // SYS_TRIGGER
     cmd[1] = 0b1 << 5; //RST_SYS
     HAL_I2C_Master_Transmit_DMA(&hi2c1, 0x28  << 1, cmd, sizeof(cmd));
     while (hi2c1.State != HAL_I2C_STATE_READY);*/

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

    // Timer starten
    HAL_TIM_Base_Start(&htim1);
    HAL_TIM_Base_Start(&htim2);
    HAL_TIM_Base_Start(&htim16);
    HAL_TIM_Base_Start(&htim6);
    HAL_TIM_Base_Start(&htim7);

    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1);

    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);

    HAL_TIM_Base_Start_IT(&htim6);
    HAL_TIM_Base_Start_IT(&htim7);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"


    rc_lib_package_t sbus_package;
    sbus_package.mesh = false;
    sbus_package.channel_count = 16;
    sbus_package.resolution = 2048;

    rc_lib_transmitter_id = 23;

    rc_lib_package_t flight_computer_package_receiving;

    init_all_controller();

    HAL_UART_Receive_DMA(&huart1, sBusReceiveBuffer, sizeof(sBusReceiveBuffer));
    HAL_UART_Receive_DMA(&huart2, flightComputerReceiveBuffer, sizeof(flightComputerReceiveBuffer));


    while (1) {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

        if (HAL_UART_GetState(&huart1) == HAL_UART_STATE_READY) {
            if(sbus_parse(sBusReceiveBuffer, sizeof(sBusReceiveBuffer))){
                for(uint8_t c=0; c<16; c++) {
                    sbus_package.channel_data[c] = sbus_latest_data.channel[c];
                }
                uint8_t tmp = rc_lib_transmitter_id;
                rc_lib_transmitter_id = 56;
                uint8_t  length = rc_lib_encode(&sbus_package);
                HAL_UART_Transmit_DMA(&huart2, sbus_package.buffer, length);
                rc_lib_transmitter_id = tmp;
            }
            HAL_UART_Receive_DMA(&huart1, sBusReceiveBuffer, sizeof(sBusReceiveBuffer));
        } else if(HAL_UART_GetState(&huart2) == HAL_UART_STATE_READY) {
            for(uint8_t b=0; b< sizeof(flightComputerReceiveBuffer); b++) {
                if(rc_lib_decode(&flight_computer_package_receiving, flightComputerReceiveBuffer[b])) {
                    flight_computer_package.resolution = flight_computer_package_receiving.resolution;
                    flight_computer_package.channel_count = flight_computer_package_receiving.channel_count;
                    for(uint8_t c=0; c<flight_computer_package.channel_count; c++) {
                        flight_computer_package.channel_data[c] = flight_computer_package_receiving.channel_data[c];
                    }
                }
            }
        }
        handle_i2c();

        HAL_ADC_Stop_DMA(&hadc1);
        adc_reading_pressure = new_adc_reading_pressure;
        HAL_ADC_Start_DMA(&hadc1, &new_adc_reading_pressure, sizeof(new_adc_reading_pressure));
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
                              |RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_ADC;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 16;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_ADC1CLK;
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
