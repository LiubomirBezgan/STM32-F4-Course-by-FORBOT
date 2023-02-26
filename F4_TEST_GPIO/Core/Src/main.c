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
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdbool.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define R LED_Red_Pin
#define OR (LED_Orange_Pin | LED_Red_Pin)
#define GOR (LED_Green_Pin | LED_Orange_Pin | LED_Red_Pin)
#define BGOR (LED_Blue_Pin | LED_Green_Pin | LED_Orange_Pin | LED_Red_Pin)
#define BGO (LED_Blue_Pin | LED_Green_Pin | LED_Orange_Pin)
#define BG (LED_Blue_Pin | LED_Green_Pin)
#define B LED_Blue_Pin

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile bool s_gpio_pressed;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
uint8_t LB_GPIO_IfSet(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
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
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if (s_gpio_pressed)
    {
    	if (GPIO_PIN_SET == LB_GPIO_IfSet(GPIOD, BGOR))
    	{
    		HAL_GPIO_WritePin(GPIOD, LED_Blue_Pin, RESET);
    	}
    	else if(GPIO_PIN_SET == LB_GPIO_IfSet(GPIOD, GOR))
    	{
    		HAL_GPIO_WritePin(GPIOD, LED_Green_Pin, RESET);
    	}
    	else if(GPIO_PIN_SET == LB_GPIO_IfSet(GPIOD, OR))
    	{
    		HAL_GPIO_WritePin(GPIOD, LED_Orange_Pin, RESET);
    	}
    	else if(GPIO_PIN_SET == LB_GPIO_IfSet(GPIOD, R))
    	{
    		HAL_GPIO_WritePin(GPIOD, LED_Red_Pin, RESET);
    	}
    	else if(GPIO_PIN_SET == LB_GPIO_IfSet(GPIOD, BGO))
    	{
    		HAL_GPIO_WritePin(GPIOD, LED_Red_Pin, SET);
    	}
    	else if(GPIO_PIN_SET == LB_GPIO_IfSet(GPIOD, BG))
    	{
    		HAL_GPIO_WritePin(GPIOD, LED_Orange_Pin, SET);
    	}
    	else if(GPIO_PIN_SET == LB_GPIO_IfSet(GPIOD, B))
    	{
    		HAL_GPIO_WritePin(GPIOD, LED_Green_Pin, SET);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOD, LED_Blue_Pin, SET);
		}
		HAL_Delay(100);
		__HAL_PWR_PVD_EXTI_ENABLE_IT();
		s_gpio_pressed = false;
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
uint8_t LB_GPIO_IfSet(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	uint8_t result;
	if (GPIO_Pin == (GPIO_Pin & GPIOx->IDR))
	{
		result = 0x1;
	}
	else
	{
		result = 0x0;
	}
	return result;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (Button_Pin == GPIO_Pin)
	{
		s_gpio_pressed = true;
		__HAL_PWR_PVD_EXTI_DISABLE_IT();
	}
}
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
