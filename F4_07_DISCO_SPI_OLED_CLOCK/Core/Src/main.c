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
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "SSD1331.h"
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum {
	print_time,
	set_time_h,
	set_time_m,
	set_time_s
} clock_state_e;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SECONDS 60
#define MINUTES 60
#define HOURS 24

#define CENTER_X 15
#define CENTER_Y 25
#define LINE_OFFSET_X 0
#define LINE_OFFSET_Y 17
#define LINE_PITCH_X 24
#define LINE_LENGTH_X 15
#define END_OF_THE_LINE ( (CENTER_X) + ( (LINE_PITCH_X) * 2 ) + LINE_LENGTH_X)

#define H_LINE_OFFSET_X (CENTER_X + LINE_OFFSET_X)
#define H_LINE_OFFSET_Y (CENTER_Y + LINE_OFFSET_Y)
#define M_LINE_OFFSET_X (CENTER_X + LINE_OFFSET_X + LINE_PITCH_X)
#define M_LINE_OFFSET_Y (CENTER_Y + LINE_OFFSET_Y)
#define S_LINE_OFFSET_X (CENTER_X + LINE_OFFSET_X + (LINE_PITCH_X * 2))
#define S_LINE_OFFSET_Y (CENTER_Y + LINE_OFFSET_Y)

#define JOYSTICK_LOWER_LIMIT 1050
#define JOYSTICK_UPPER_LIMIT 3050
#define JOYSTICK_DELAY 268
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t message[9];
uint8_t time[3] = {10, 34, 21};		// 2 - hours, 1 - minutes, 0 - seconds
uint16_t Joystick[2];				// 0 - X, 1 - Y
clock_state_e state = print_time;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

void LB_PrintfTime(void);
void LB_SetHours(void);
void LB_SetMinutes(void);
void LB_SetSeconds(void);
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
  MX_SPI1_Init();
  MX_TIM10_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */

  ssd1331_init();
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*) Joystick, 2);
  ssd1331_clear_screen(BLACK);
  HAL_Delay(1000);
  HAL_TIM_Base_Start_IT(&htim10);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  switch (state)
	  {
	  case print_time:
		  LB_PrintfTime();
		  break;
	  case set_time_h:
		  LB_SetHours();
		  break;
	  case set_time_m:
		  LB_SetMinutes();
		  break;
	  case set_time_s:
		  LB_SetSeconds();
		  break;

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
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

	if (TIM10 == htim->Instance && print_time == state)
	{
		ssd1331_draw_line(H_LINE_OFFSET_X, H_LINE_OFFSET_Y, END_OF_THE_LINE, H_LINE_OFFSET_Y, BLACK);
		ssd1331_display_string(CENTER_X, CENTER_Y, message, FONT_1608, BLACK);
		++time[0];
		if (SECONDS == time[0])
		{
			time[0] = 0;
			++time[1];
		}
		if (MINUTES == time[1])
		{
			time[1] = 0;
			++time[2];
		}
		if (HOURS == time[2])
		{
			time[2] = 0;
		}
		sprintf((char *) message, "%02d:%02d:%02d", time[2], time[1], time[0]);
		ssd1331_display_string(CENTER_X, CENTER_Y, message, FONT_1608, GREEN);
	}

}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (Joystick_button_Pin == GPIO_Pin || Button_Pin == GPIO_Pin)
	{
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		if (set_time_h == state || set_time_m == state || set_time_s == state)
		{
			state = print_time;
		}
		else
		{
			state = set_time_s;
		}
	}
}

void LB_PrintfTime(void)
{
	;
}

void LB_SetHours(void)
{
	  ssd1331_draw_line(H_LINE_OFFSET_X, H_LINE_OFFSET_Y, H_LINE_OFFSET_X + LINE_LENGTH_X, H_LINE_OFFSET_Y, GREEN);
	  if (Joystick[0] < JOYSTICK_LOWER_LIMIT)
	  {
		  state = set_time_s;
		  ssd1331_draw_line(H_LINE_OFFSET_X, H_LINE_OFFSET_Y, H_LINE_OFFSET_X + LINE_LENGTH_X, H_LINE_OFFSET_Y, BLACK);
		  HAL_Delay(JOYSTICK_DELAY);
	  }
	  else if (Joystick[0] > JOYSTICK_UPPER_LIMIT)
	  {
		  state = set_time_m;
		  ssd1331_draw_line(H_LINE_OFFSET_X, H_LINE_OFFSET_Y, H_LINE_OFFSET_X + LINE_LENGTH_X, H_LINE_OFFSET_Y, BLACK);
		  HAL_Delay(JOYSTICK_DELAY);
	  }
	  if (Joystick[1] < JOYSTICK_LOWER_LIMIT)
	  {
		  ssd1331_display_string(CENTER_X, CENTER_Y, message, FONT_1608, BLACK);
		  if (HOURS == ++time[2] )
		  {
			  time[2] = 0;
		  }
		  sprintf((char *) message, "%02d:%02d:%02d", time[2], time[1], time[0]);
		  ssd1331_display_string(CENTER_X, CENTER_Y, message, FONT_1608, GREEN);
		  HAL_Delay(JOYSTICK_DELAY);
	  }
	  else if (Joystick[1] > JOYSTICK_UPPER_LIMIT)
	  {

		  ssd1331_display_string(CENTER_X, CENTER_Y, message, FONT_1608, BLACK);
		  if (0 == (time[2])--)
		  {
			  time[2] = HOURS - 1;
		  }
		  sprintf((char *) message, "%02d:%02d:%02d", time[2], time[1], time[0]);
		  ssd1331_display_string(CENTER_X, CENTER_Y, message, FONT_1608, GREEN);
		  HAL_Delay(JOYSTICK_DELAY);
	  }
}

void LB_SetMinutes(void)
{
	  ssd1331_draw_line(M_LINE_OFFSET_X, M_LINE_OFFSET_Y, M_LINE_OFFSET_X + LINE_LENGTH_X, M_LINE_OFFSET_Y, GREEN);
	  if (Joystick[0] < JOYSTICK_LOWER_LIMIT)
	  {
		  state = set_time_h;
		  ssd1331_draw_line(M_LINE_OFFSET_X, M_LINE_OFFSET_Y, M_LINE_OFFSET_X + LINE_LENGTH_X, M_LINE_OFFSET_Y, BLACK);
		  HAL_Delay(JOYSTICK_DELAY);
	  }
	  else if (Joystick[0] > JOYSTICK_UPPER_LIMIT)
	  {
		  state = set_time_s;
		  ssd1331_draw_line(M_LINE_OFFSET_X, M_LINE_OFFSET_Y, M_LINE_OFFSET_X + LINE_LENGTH_X, M_LINE_OFFSET_Y, BLACK);
		  HAL_Delay(JOYSTICK_DELAY);
	  }
	  if (Joystick[1] < JOYSTICK_LOWER_LIMIT)
	  {
		  ssd1331_display_string(CENTER_X, CENTER_Y, message, FONT_1608, BLACK);
		  if (MINUTES == ++time[1] )
		  {
			  time[1] = 0;
		  }
		  sprintf((char *) message, "%02d:%02d:%02d", time[2], time[1], time[0]);
		  ssd1331_display_string(CENTER_X, CENTER_Y, message, FONT_1608, GREEN);
		  HAL_Delay(JOYSTICK_DELAY);
	  }
	  else if (Joystick[1] > JOYSTICK_UPPER_LIMIT)
	  {
		  ssd1331_display_string(CENTER_X, CENTER_Y, message, FONT_1608, BLACK);
		  if (0 == (time[1])--)
		  {
			  time[1] = MINUTES - 1;
		  }
		  sprintf((char *) message, "%02d:%02d:%02d", time[2], time[1], time[0]);
		  ssd1331_display_string(CENTER_X, CENTER_Y, message, FONT_1608, GREEN);
		  HAL_Delay(JOYSTICK_DELAY);
	  }
}
void LB_SetSeconds(void)
{
	  ssd1331_draw_line(S_LINE_OFFSET_X, S_LINE_OFFSET_Y, S_LINE_OFFSET_X + LINE_LENGTH_X, S_LINE_OFFSET_Y, GREEN);
	  if (Joystick[0] < JOYSTICK_LOWER_LIMIT)
	  {
		  state = set_time_m;
		  ssd1331_draw_line(S_LINE_OFFSET_X, S_LINE_OFFSET_Y, S_LINE_OFFSET_X + LINE_LENGTH_X, S_LINE_OFFSET_Y, BLACK);
		  HAL_Delay(JOYSTICK_DELAY);
	  }
	  else if (Joystick[0] > JOYSTICK_UPPER_LIMIT)
	  {
		  state = set_time_h;
		  ssd1331_draw_line(S_LINE_OFFSET_X, S_LINE_OFFSET_Y, S_LINE_OFFSET_X + LINE_LENGTH_X, S_LINE_OFFSET_Y, BLACK);
		  HAL_Delay(JOYSTICK_DELAY);
	  }
	  if (Joystick[1] < JOYSTICK_LOWER_LIMIT)
	  {
		  ssd1331_display_string(CENTER_X, CENTER_Y, message, FONT_1608, BLACK);
		  if (SECONDS == ++time[0] )
		  {
			  time[0] = 0;
		  }
		  sprintf((char *) message, "%02d:%02d:%02d", time[2], time[1], time[0]);
		  ssd1331_display_string(CENTER_X, CENTER_Y, message, FONT_1608, GREEN);
		  HAL_Delay(JOYSTICK_DELAY);
	  }
	  else if (Joystick[1] > JOYSTICK_UPPER_LIMIT)
	  {
		  ssd1331_display_string(CENTER_X, CENTER_Y, message, FONT_1608, BLACK);
		  if (0 == (time[0])--)
		  {
			  time[0] = SECONDS - 1;
		  }
		  sprintf((char *) message, "%02d:%02d:%02d", time[2], time[1], time[0]);
		  ssd1331_display_string(CENTER_X, CENTER_Y, message, FONT_1608, GREEN);
		  HAL_Delay(JOYSTICK_DELAY);
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
