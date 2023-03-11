/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <limits.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define AXES 2							// the number of axes (1 - X, 2 - XY, 3 - XYZ)
#define LSM303_ACC_ADRESS (0x19 << 1)	// adres akcelerometru: 0011 001x
#define LSM303_ACC_CTRL_REG1_A 0x20		// rejestr ustawien 1
#define LSM303_ACC_OUT_X_L_A 0x28		// 0010 1000
#define LSM303_ACC_XY_L_A_MULTI_READ (LSM303_ACC_OUT_X_L_A | 0x80)

// CTRL_REG1_A = [ODR3][ODR2][ODR1][ODR0][LPEN][ZEN][YEN][XEN]
#define LSM303_ACC_YX_ENABLE 0x03		// 0000 0011
#define LSM303_ACC_100HZ 0x50			// 0101 0000

#define LSM303_ACC_RESOLUTION 2.0		// Maksymalna wartosc przyspieszenia [g]

#define PITCH 1200
#define ROLL 1200
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t Settings = LSM303_ACC_YX_ENABLE | LSM303_ACC_100HZ;
uint8_t Data[AXES * 2];
int16_t Axes[AXES];
float Axes_g[AXES];
int i, j;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

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
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  HAL_I2C_Mem_Write(&hi2c1, LSM303_ACC_ADRESS, LSM303_ACC_CTRL_REG1_A, 1, &Settings, 1, 100);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  HAL_I2C_Mem_Read(&hi2c1, LSM303_ACC_ADRESS, LSM303_ACC_XY_L_A_MULTI_READ, 1, Data, AXES * 2, 100);
	  for (i = 0; i < AXES; i++)	// 0 - X, 1 - Y, 2 - Z
	  {
		  j = i * 2;
		  Axes[i] = (Data[j + 1] << 8 | Data[j]);
		  Axes_g[i] = ((float) Axes[i] * LSM303_ACC_RESOLUTION) / (float) INT16_MAX;
	  }


	  if (Axes[0] > PITCH)
	  {
		  HAL_GPIO_WritePin(LED_X_NEGATIVE_GPIO_Port, LED_X_NEGATIVE_Pin, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(LED_X_POSITIVE_GPIO_Port, LED_X_POSITIVE_Pin, GPIO_PIN_RESET);
	  }
	  else if ( Axes[0] < (int16_t) (PITCH * -1) )
	  {
		  HAL_GPIO_WritePin(LED_X_POSITIVE_GPIO_Port, LED_X_POSITIVE_Pin, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(LED_X_NEGATIVE_GPIO_Port, LED_X_NEGATIVE_Pin, GPIO_PIN_RESET);
	  }
	  else
	  {
		  HAL_GPIO_WritePin(LED_X_POSITIVE_GPIO_Port, LED_X_POSITIVE_Pin, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(LED_X_NEGATIVE_GPIO_Port, LED_X_NEGATIVE_Pin, GPIO_PIN_RESET);
	  }

	  if (Axes[1] > ROLL)
	  {
		  HAL_GPIO_WritePin(LED_Y_NEGATIVE_GPIO_Port, LED_Y_NEGATIVE_Pin, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(LED_Y_POSITIVE_GPIO_Port, LED_Y_POSITIVE_Pin, GPIO_PIN_RESET);
	  }
	  else if ( Axes[1] < (int16_t) (ROLL * -1) )
	  {
		  HAL_GPIO_WritePin(LED_Y_POSITIVE_GPIO_Port, LED_Y_POSITIVE_Pin, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(LED_Y_NEGATIVE_GPIO_Port, LED_Y_NEGATIVE_Pin, GPIO_PIN_RESET);
	  }
	  else
	  {
		  HAL_GPIO_WritePin(LED_Y_POSITIVE_GPIO_Port, LED_Y_POSITIVE_Pin, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(LED_Y_NEGATIVE_GPIO_Port, LED_Y_NEGATIVE_Pin, GPIO_PIN_RESET);
	  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
