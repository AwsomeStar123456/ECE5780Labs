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
#include <stdio.h>

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
void ADC_Init(void);
void DAC_Init(void);
void USART_Init(void);
void USART_SingleChar(char charVal);
void USART_StringChar(char *string);
/* USER CODE BEGIN PFP */

// Sine Wave: 8-bit, 32 samples/cycle
const uint8_t sine_table[32] = {127,151,175,197,216,232,244,251,254,251,244,
232,216,197,175,151,127,102,78,56,37,21,9,2,0,2,9,21,37,56,78,102};
// Triangle Wave: 8-bit, 32 samples/cycle
const uint8_t triangle_table[32] = {0,15,31,47,63,79,95,111,127,142,158,174,
190,206,222,238,254,238,222,206,190,174,158,142,127,111,95,79,63,47,31,15};
// Sawtooth Wave: 8-bit, 32 samples/cycle
const uint8_t sawtooth_table[32] = {0,7,15,23,31,39,47,55,63,71,79,87,95,103,
111,119,127,134,142,150,158,166,174,182,190,198,206,214,222,230,238,246};
// Square Wave: 8-bit, 32 samples/cycle
const uint8_t square_table[32] = {254,254,254,254,254,254,254,254,254,254,
254,254,254,254,254,254,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};



uint32_t currentIndex = 0;


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
	
	SystemClock_Config();
	
	GPIO_Init();
	//USART_Init();
	ADC_Init();
	DAC_Init();

  while (1)
  {
		
		//ADC
		
		uint8_t ADCData = (ADC1->DR & 0xFF);
		
		if(ADCData > 51) {
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
		}
		
		if(ADCData > 102) {
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
		}
		
		if(ADCData > 153) {
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
		}
		
		if(ADCData > 204) {
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
		} else {
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
		}
		
		//DAC
		
		//Set the current table index in the DAC and increment index
		DAC1->DHR8R1 = sawtooth_table[currentIndex];
		currentIndex++;
		
		if(currentIndex > 31) {
			currentIndex = 0;
		}
		
		HAL_Delay(1);
  }
}

/**
  * @brief  Initiates the ADC
  * @retval None
  */
void ADC_Init(void)
{
	//Enable the ADC1 in the RCC
	__HAL_RCC_ADC1_CLK_ENABLE();

	//**Right Align Default
	//**External Trigger Disabled Default
	//Set 8 Bit Resolution | 10b in RES[1:0]
	ADC1->CFGR1 |= (1 << 4);
	
	//Enable Continuous Conversion Mode
	ADC1->CFGR1 |= (1 << 13);
	
	//Select Channel 14 which is pin PC4
	ADC1->CHSELR |= (1 << 14);
	
	//Start Calibration
	ADC1->CR |= (1U << 31);
	
	//Wait for Calibration to Finish
	while(1)
	{
			if(~(ADC1->CR) & (1U << 31)) { break; }
	}
	
	//Enable the ADC
	ADC1->CR |= (0x1);

	while(1) {
		if(ADC1->ISR & (0x1)) {
			break;
		}
	}
	
	//Start the ADC
	ADC1->CR |= (1 << 2);
}

/**
  * @brief  Initiates the DAC
  * @retval None
  */
void DAC_Init(void)
{
	//Pin PA4 **DAC_OUT1
	//Enable the DAC in the RCC
	__HAL_RCC_DAC1_CLK_ENABLE();
	
	//Enable Software Trigger for Channel 1
	DAC1->CR |= ((1 << 5) | (1 << 4) | (1 << 3));
	
	//Enable Channel 1
	DAC1->CR |= (1 << 0);
}

/**
  * @brief  Initiates GPIO Pins
  * @retval None
  */
void GPIO_Init(void)
{
	//Enable GPIOB and GPIOC Clock
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	//Enable ADC Input Pin PC4 **ADC_IN1
	GPIO_InitTypeDef adcTypeDef = {GPIO_PIN_4, GPIO_MODE_ANALOG, GPIO_SPEED_FREQ_LOW, GPIO_NOPULL};
	HAL_GPIO_Init(GPIOC, &adcTypeDef);
	
	//Enable DAC Output Pin PA4 **DAC_OUT1
	GPIO_InitTypeDef dacTypeDef = {GPIO_PIN_4, GPIO_MODE_ANALOG, GPIO_SPEED_FREQ_LOW, GPIO_NOPULL};
	HAL_GPIO_Init(GPIOA, &dacTypeDef);
	
	//Enable LED Pins
	GPIO_InitTypeDef ledTypeDef = {GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9,
	GPIO_MODE_OUTPUT_PP, GPIO_SPEED_FREQ_LOW, GPIO_NOPULL};
	HAL_GPIO_Init(GPIOC, &ledTypeDef);
	
	//Enable UART Degbugging Pins
	//Init the RX pin PB11
	GPIO_InitTypeDef rxTypeDef = {GPIO_PIN_11, GPIO_MODE_AF_PP, GPIO_SPEED_FREQ_LOW, GPIO_NOPULL, GPIO_AF4_USART3};
	HAL_GPIO_Init(GPIOB, &rxTypeDef);
	
	//Init the TX pin PB10
	GPIO_InitTypeDef txTypeDef = {GPIO_PIN_10, GPIO_MODE_AF_PP, GPIO_SPEED_FREQ_LOW, GPIO_NOPULL, GPIO_AF4_USART3};
	HAL_GPIO_Init(GPIOB, &txTypeDef);	
}

/**
  * @brief  Initiates the USART
  * @retval None
  */
void USART_Init(void)
{
	//Enable the USART 3 CLK
	//Baud Rate: 115200
	//PB10 TX | PB11 RX | ON BOARD NOT USART => INVERT USART
	__HAL_RCC_USART3_CLK_ENABLE();
	
	//Set Baud Rate
	USART3->BRR = HAL_RCC_GetHCLKFreq() / 115200;
	
	//Enable Tx and Rx
	USART3->CR1 |= ((1 << 3) | (1 << 2));
	
	//Enable the USART3
	USART3->CR1 |= (1 << 0);
	
	USART_StringChar("USART Initiated");
	
	//Code for debugging the ADC Values via USART
	//char adcDataStr[4]; // Array to hold the string. Size depends on the maximum possible value of ADCData.
	//sprintf(adcDataStr, "%d", ADCData); // Convert ADCData to string.
	//USART_StringChar("ADCDATA: ");
	//USART_StringChar(adcDataStr);
	
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
