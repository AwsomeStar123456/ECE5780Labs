/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
void GPIO_Init(void);
void USART_Init(void);
void USART_SingleChar(char charVal);
void USART_StringChar(char *string);
char USART_ReadChar(void);
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
  HAL_Init();
	GPIO_Init();
	USART_Init();
	

  while (1)
  {
		//Single Character
		//USART_SingleChar('c');
		
		//String
		char string[] = "Hello, world!";
    USART_StringChar(string);
		
		//if(USART_ReadChar() == 'g')
		//{
		//	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);
		//}
		
	
		//5 Second Delay
		HAL_Delay(500);
		
		//LED toggle to ensure we aren't stuck anywhere.
		//HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
  }
}

/**
  * @brief  Initiates GPIO Pins
  * @retval None
  */
void GPIO_Init(void)
{
	//Enable GPIOB and GPIOC Clock
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
	//Init the RX pin PB11
	GPIO_InitTypeDef rxTypeDef = {GPIO_PIN_11, GPIO_MODE_AF_PP, GPIO_SPEED_FREQ_LOW, GPIO_NOPULL, GPIO_AF4_USART3};
	HAL_GPIO_Init(GPIOB, &rxTypeDef);
	
	//Init the TX pin PB10
	GPIO_InitTypeDef txTypeDef = {GPIO_PIN_10, GPIO_MODE_AF_PP, GPIO_SPEED_FREQ_LOW, GPIO_NOPULL, GPIO_AF4_USART3};
	HAL_GPIO_Init(GPIOB, &txTypeDef);		
	
	//Enable LED Pins
	GPIO_InitTypeDef ledTypeDef = {GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9,
	GPIO_MODE_OUTPUT_PP, GPIO_SPEED_FREQ_LOW, GPIO_NOPULL};
	HAL_GPIO_Init(GPIOC, &ledTypeDef);
	
	//Enable RED LED
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
	
	return;
}

/**
  * @brief  Initiates the USART
  * @retval None
  */
void USART_Init(void)
{
	//Enable the USART 3 CLK
	//Baud Rate: 115200
	//PB10 TX | PB11 RX
	__HAL_RCC_USART3_CLK_ENABLE();
	
	//Set Baud Rate
	USART3->BRR = HAL_RCC_GetHCLKFreq() / 115200;
	
	//Enable Tx and Rx
	USART3->CR1 |= ((1 << 3) | (1 << 2));
	
	//Enable the USART3
	USART3->CR1 |= (1 << 0);
	
	return;
}

/**
  * @brief  Method for sending a character through a UART connection.
  * @retval None
  */
void USART_SingleChar(char charVal)
{
	//Make sure TX data register is empty before writing to it.
	while(!(USART3->ISR & (1 << 7)));
	
	//Write data to send.
	USART3->TDR = charVal;
	
	return;
}

/**
  * @brief  Method for sending a string through a UART connection.
  * @retval None
  */
void USART_StringChar(char *string)
{
	int i = 0;
	
	//While we havent hit a termination character.
	while(string[i] != '\0')
	{
		USART_SingleChar(string[i]);
		i++;
	}
	
	return;
}

/**
  * @brief  Waits for the read reg to not be empty and reads its value.
  * @retval Value that was read from the USART.
  */
char USART_ReadChar(void)
{
	//Wait until the read register is not empty.
	while(!(USART3->ISR & (1 << 5)));
	
	//Return value of return register.
	return USART3->RDR;
}


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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
