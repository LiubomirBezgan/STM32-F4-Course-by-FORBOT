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
#include "dma.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile bool upper_limit = false;
volatile bool lower_limit = false;
volatile uint16_t Duty = 0;									// Wypelnienie generowanego sygnalu PWM

// Zmienne obslugujace wejsciowy sygnal PWM
volatile uint16_t InputPWMPeriod_cycles;					// Czas trwania calego okresu sygnalu [w cyklach zegara]
volatile uint16_t InputPWMPeriod_miliseconds;				// Czas trwania calego okresu sygnalu [ w milisekundach]
volatile uint16_t InputPWMDuty_cycles;						// Czas trwania stanu wysokiego [w cyklach zegara]
volatile uint16_t InputPWMDuty_percent;						// Wspolczynnik wypelnienia sygnalu PWM [w procentach]

const uint16_t InputTimerResolution_cycles = 2000;			// Rozdzielczosc zegara odczytujacego sygnal PWM [w cyklach zegara]
const uint16_t InputTimerPeriodDuration_miliseconds = 20;	// Czas trwania jednego okresu timera odczytujacego sygnal PWM [w milisekundach

// Zmienne do obslugi enkodera
volatile uint16_t pulse_count;								// liczba zliczonych impulsow
uint16_t pulse_count_previous;
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
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_IC_Start(&htim2, TIM_CHANNEL_1);
  HAL_TIM_IC_Start(&htim2, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start_DMA(&htim4, TIM_CHANNEL_3, (uint32_t *) &Duty, 1);
  HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  // Odczytanie wartosci enkodera i zeskalowanie do liczby "stabilnych pozycji"
	  pulse_count = (TIM1->CNT) / 4;

	  // Przypadek 1 BEGIN
	  // Ustawienie wypelnienia rownego odczytanej liczbie impulsow
	  //Duty = pulse_count;
	  // Przypadek 1 END

	  // Przypadek 2 BEGIN
	  // Ustawienie wypelnienia w zaleznosci od kierunku obrotu pokretla (dlaczego dodaje 4 przy przepelnieniu licznika)
	  if (pulse_count != pulse_count_previous) // ???
	  {
		  if (100 == Duty && !lower_limit)
		  {
			  upper_limit = true;

		  }
		  else if (0 == Duty && !upper_limit)
		  {
			  lower_limit = true;
		  }

		  if (upper_limit && ((pulse_count_previous < pulse_count) || (100 == pulse_count_previous && 0 == pulse_count)) )
		  {
			  Duty = 100;
		  }
		  else if (upper_limit && ((pulse_count_previous > pulse_count) || (0 == pulse_count_previous && 100 == pulse_count)) )
		  {
			  upper_limit = false;
			  Duty--;
		  }
		  else if (lower_limit && ((pulse_count_previous > pulse_count) || (0 == pulse_count_previous && 100 == pulse_count)) )
		  {
			  Duty = 0;
		  }
		  else if (lower_limit && ((pulse_count_previous < pulse_count) || (100 == pulse_count_previous && 0 == pulse_count)) )
		  {
			  lower_limit = false;
			  Duty++;
		  }
		  else if ( ( (pulse_count_previous < pulse_count) || (100 == pulse_count_previous && 0 == pulse_count) ) && !((0 == pulse_count_previous && 100 == pulse_count)) )
		  {
			  Duty++;
		  }
		  else if ( ( (pulse_count_previous > pulse_count) || (0 == pulse_count_previous && 100 == pulse_count) ) && !(100 == pulse_count_previous && 0 == pulse_count) )
		  {
			  Duty--;
		  }

	  }
	  pulse_count_previous = pulse_count;
	  // Przypadek 2 END

	  // odczytanie dlugosci trwania okresu sygnalu (w cyklach zegara)
	  InputPWMPeriod_cycles = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_1) + 1;

	  // odczytanie dlugosci trwania stanu wysopkiego w sygnale (w cyklach zegara)
	  InputPWMDuty_cycles = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_2) + 1;

	  // Przeliczenie dlugosci trwania okresu sygnalu na milisekundy
	  InputPWMPeriod_miliseconds = (uint16_t) (InputPWMPeriod_cycles * InputTimerPeriodDuration_miliseconds) / InputTimerResolution_cycles;

	  // Obliczenie procentu wypelnienia sygnalu PWM
	  InputPWMDuty_percent = (InputPWMDuty_cycles * 100) / InputPWMPeriod_cycles;
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
