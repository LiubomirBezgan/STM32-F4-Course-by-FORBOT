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
#include "fatfs.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <fatfs_sd.h>
#include <string.h>
#include <stdio.h>
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

// to send the data to the uart
void send_uart (char * string);

// to find the size of data in the buffer
int bufsize (char * buf);

// to clear the buffer
void bufclear (void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
FATFS fs;			// file system
FIL fil;			// file
FRESULT fresult;	// to store the result
char buffer[1024];	// to store data

UINT br, bw;		// file read/write count

// capacity related variables
FATFS * pfs;
DWORD fre_clust;
uint32_t total, free_space;


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
  MX_USART2_UART_Init();
  MX_SPI2_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */

  // Mount SD Card
  fresult = f_mount(&fs, "", 0);
  if (FR_OK != fresult)
  {
	  send_uart("error in mounting SD CARD...\r\n");
  }
  else
  {
	  send_uart("SD CARD mounted successfully...\r\n");
  }

  /*** Card capacity details ***/

  // check free space
  f_getfree("", &fre_clust, &pfs);

  total = (uint32_t) ( (pfs->n_fatent - 2) * pfs->csize * 0.5);
  sprintf(buffer, "SD CARD Total Size: \t%lu\r\n", total);
  send_uart(buffer);
  bufclear();
  free_space = (uint32_t) (fre_clust * pfs->csize * 0.5);
  sprintf(buffer, "SD CARD Free Space: \t%lu\r\n", free_space);
  send_uart(buffer);

  /*** The folowwing operation is using PUTS and GETS ***/

  // Open file to write/ create a file if it doesn't exist
  fresult = f_open(&fil, "file1.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);

  // Writing text
  fresult = f_puts("This data is from the First FILE\n\r\n\r", &fil);
  send_uart("file1.txt created and the data is written.\r\n");

  // Close file
  fresult = f_close(&fil);

  // Open file to read
  fresult = f_open(&fil, "file1.txt", FA_READ);

  // Read string from the file
  f_gets(buffer, sizeof(buffer), &fil);
  send_uart(buffer);

  // Close file
  f_close(&fil);
  bufclear();

  /*** The folowwing operation is using f_write and f_read ***/

  // Create second file with read\write access and open it
  fresult = f_open(&fil, "file2.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);

  // Writing text
  strcpy(buffer, "This is File 2 and it says Hello from SKALEN.Gaming");
  fresult = f_write(&fil, buffer, bufsize(buffer), &bw);
  send_uart("\n\rFile2.txt created and data is written\n\r");

  // Close file
  f_close(&fil);
  bufclear();

  //Open second file to read
  fresult = f_open(&fil, "file2.txt", FA_READ);

  // Read data from the file
  f_read(&fil, buffer, sizeof(buffer), &br);
  send_uart(buffer);

  // Close file
  f_close(&fil);
  bufclear();

  /*** Updating an existing file ***/

  // Open the file with write access
  fresult = f_open(&fil, "file2.txt", FA_OPEN_ALWAYS | FA_WRITE);

  // Move offset to the end of file
  fresult = f_lseek(&fil, fil.fptr);

  // Write a string to the file
  fresult = f_puts("\rThis is updated data and it should be in the end.\r\n", &fil);

  f_close(&fil);

  // Open to read the file
  fresult = f_open(&fil, "file2.txt", FA_READ);

  // Read string from the file
  f_read(&fil, buffer, sizeof(buffer), &br);
  send_uart(buffer);

  // Close file
  f_close(&fil);
  bufclear();

  /*** Removing files from the directory ***/

  fresult = f_unlink("/file4.txt");
  if (FR_OK == fresult)
  {
	  send_uart("file4.txt removed successfully...\n\r");
  }

  fresult = f_unlink("/file3.txt");
  if (FR_OK == fresult)
  {
	  send_uart("file3.txt removed successfully...\n\r");
  }

  // Unmount SD CARD
  fresult = f_mount(NULL, "", 1);
  if (FR_OK == fresult)
  {
	  send_uart("SD CARD UNMOUNTED successfully...\n\r");
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
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
void send_uart (char * string)
{
	uint8_t len = strlen(string);
	HAL_UART_Transmit(&huart2, (uint8_t *) string, len, 2000); // transmit in blocking mode
}

int bufsize (char * buf)
{
	int i = 0;
	while ('\0' != *buf++)
	{
		i++;
	}
	return i;
}

void bufclear (void)
{
	for (int i = 0; i < 1024; i++)
	{
		buffer[i] = '\0';
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
