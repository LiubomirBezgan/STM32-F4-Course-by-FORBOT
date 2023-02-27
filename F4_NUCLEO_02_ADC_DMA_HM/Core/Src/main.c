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
#include "adc.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "stdbool.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/*choose the debouncing method*/
//#define DEBOUNCING_IMR
#define DEBOUNCING_TICK
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
float Temperature;
float Vsense;
const float alarm_temp = 30.0;		// [°C]
const float T25 = 25.0;				// [°C]
const float V25 = 0.76;				// [V]
const float Avg_slope = 0.0025;		// [V/°C]
const float SupplyVoltage = 3.3;	// [V]
const float ADCResolution = 4095.0;

const char nm[] = "\rNORMAL MODE\n";
const char im[] = "\rINVERSION MODE\n";
const char ar[] = "\rALARM RESET\n";
char message_array[20];
uint16_t Joystick_HW[3];
uint16_t message_length;
volatile bool inversion_mode;
volatile bool alarm_reset;

#ifdef DEBOUNCING_IMR
	volatile bool s_gpio_pressed;
#endif

#ifdef DEBOUNCING_TICK
uint32_t previousMillis = 0;
uint32_t currentMillis = 0;
uint32_t counterOutside = 0;	//For testing only
uint32_t counterInside = 0;		//For testing only
#endif
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
uint16_t LB_NormalMode(uint16_t * meas_val, char * message);
uint16_t LB_InversionMode(uint16_t * meas_val, char * message);
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
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
HAL_ADC_Start_DMA(&hadc1, (uint32_t*) Joystick_HW, 3);
inversion_mode = false;
alarm_reset = false;
HAL_UART_Transmit_IT(&huart2, (uint8_t *) nm, strlen(nm));
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  Vsense = (SupplyVoltage * Joystick_HW[2]) / ADCResolution;	// przeliczenie wartosci zmierzonej na napiecie
	  Temperature = ((Vsense - V25) / Avg_slope) + T25;				// obliczenie temperatury
	  if (Temperature > alarm_temp && !alarm_reset)
	  {
		  message_length = sprintf(message_array, "\rALARM\n");
	  }
	  else
	  {
		  if (inversion_mode)
		 	  {
		 		  message_length = LB_InversionMode(Joystick_HW, message_array);
		 	  }
		 	  else
		 	  {
		 	  	  message_length = LB_NormalMode(Joystick_HW, message_array);
		 	  }
	  }

	  HAL_UART_Transmit_IT(&huart2, (uint8_t *) message_array, message_length);
	  HAL_Delay(100);
	  #ifdef DEBOUNCING_IMR
		  if (s_gpio_pressed)
		  {
			  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
			  HAL_Delay(20);
			  __HAL_PWR_PVD_EXTI_ENABLE_IT();
			  s_gpio_pressed = false;
		  }
	  #endif
	  if(Temperature < alarm_temp - 2 && alarm_reset)
	  {
		  alarm_reset = false;
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
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (Blue_Push_Button_Pin == GPIO_Pin)
	{
		if (!alarm_reset && Temperature > alarm_temp)
		{
			HAL_UART_Transmit_IT(&huart2, (uint8_t *) ar, strlen(ar));
			alarm_reset = true;
		}
	}
	else if (Joystick_Button_Pin == GPIO_Pin && (Temperature < alarm_temp || alarm_reset))
	{
	  #ifdef DEBOUNCING_TICK
		  counterOutside++; //For testing only
		  currentMillis = HAL_GetTick();
		  if (GPIO_Pin == Joystick_Button_Pin && (currentMillis - previousMillis > 250))
		  {
			counterInside++; //For testing only
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
			previousMillis = currentMillis;
			if (inversion_mode)
			{
				HAL_UART_Transmit_IT(&huart2, (uint8_t *) nm, strlen(nm));
				inversion_mode = false;
			}
			else
			{
				HAL_UART_Transmit_IT(&huart2, (uint8_t *) im, strlen(im));
				inversion_mode = true;
			}
		  }
	  #endif

	  #ifdef DEBOUNCING_IMR
		  if (Joystick_Button_Pin == GPIO_Pin)
		  {
			s_gpio_pressed = true;
			__HAL_PWR_PVD_EXTI_DISABLE_IT();
		  }
	  #endif
	}
}

uint16_t LB_NormalMode(uint16_t * meas_val, char * message)
{
	  if (meas_val[0] > 2500 && meas_val[1] < 2500 && meas_val[1] > 1500)
	  {
		  return (uint16_t) sprintf(message, "\rRIGHT\n");
	  }
	  else if (meas_val[0] > 2500 && meas_val[1] > 2500)
	  {
		  return (uint16_t) sprintf(message, "\rDOWN-RIGHT\n");
	  }
	  else if (meas_val[0] > 2500 && meas_val[1] < 1500)
	  {
		  return (uint16_t) sprintf(message, "\rUP-RIGHT\n");
	  }
	  else if (meas_val[0] < 1500 && meas_val[1] < 2500 && meas_val[1] > 1500)
	  {
		  return (uint16_t) sprintf(message, "\rLEFT\n");
	  }
	  else if (meas_val[0] < 1500 && meas_val[1] > 2500)
	  {
		  return (uint16_t) sprintf(message, "\rDOWN-LEFT\n");
	  }
	  else if (meas_val[0] < 1500 && meas_val[1] < 1500)
	  {
		  return (uint16_t) sprintf(message, "\rUP-LEFT\n");
	  }
	  else if (meas_val[0] > 1500  && meas_val[0] < 2500 && meas_val[1] < 1500)
	  {
		  return (uint16_t) sprintf(message, "\rUP\n");
	  }
	  else if (meas_val[0] > 1500  && meas_val[0] < 2500 && meas_val[1] > 2500)
	  {
		  return (uint16_t) sprintf(message, "\rDOWN\n");
	  }
	  else
	  {
		  return (uint16_t) sprintf(message, "\rHOLD\n");
	  }
}

uint16_t LB_InversionMode(uint16_t * meas_val, char * message)
{
	  if (meas_val[0] > 2500 && meas_val[1] < 2500 && meas_val[1] > 1500)
	  {
		  return (uint16_t) sprintf(message, "\rLEFT\n");
	  }
	  else if (meas_val[0] > 2500 && meas_val[1] > 2500)
	  {
		  return (uint16_t) sprintf(message, "\rUP-LEFT\n");//
	  }
	  else if (meas_val[0] > 2500 && meas_val[1] < 1500)
	  {
		  return (uint16_t) sprintf(message, "\rDOWN-LEFT\n");
	  }
	  else if (meas_val[0] < 1500 && meas_val[1] < 2500 && meas_val[1] > 1500)
	  {
		  return (uint16_t) sprintf(message, "\rRIGHT\n");
	  }
	  else if (meas_val[0] < 1500 && meas_val[1] > 2500)
	  {
		  return (uint16_t) sprintf(message, "\rUP-RIGHT\n");//
	  }
	  else if (meas_val[0] < 1500 && meas_val[1] < 1500)
	  {
		  return (uint16_t) sprintf(message, "\rDOWN-RIGHT\n");
	  }
	  else if (meas_val[0] > 1500  && meas_val[0] < 2500 && meas_val[1] < 1500)
	  {
		  return (uint16_t) sprintf(message, "\rDOWN\n");
	  }
	  else if (meas_val[0] > 1500  && meas_val[0] < 2500 && meas_val[1] > 2500)
	  {
		  return (uint16_t) sprintf(message, "\rUP\n");//
	  }
	  else
	  {
		  return (uint16_t) sprintf(message, "\rHOLD\n");
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
