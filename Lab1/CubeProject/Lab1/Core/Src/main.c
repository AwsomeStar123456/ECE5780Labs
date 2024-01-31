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
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
	HAL_Init(); // Reset of all peripherals, init the Flash and Systick
	SystemClock_Config(); //Configure the system clock
	
	//Enable GPIOC and GPIOA clocks
	RCC->AHBENR |= (RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIOAEN);
	
	// Sets the 14th and 12th bits in the GPIOC_MODER register for general purpose mode.
	GPIOC->MODER |= (1 << 14) | (1 << 12);

	//Set the red LED on initially.
	GPIOC->ODR |= (1 << 6);
	
	//Enable bit 1 to set PURDR1 to pull down mode.
	GPIOA->PUPDR |= (1 << 1);
	
	//Initialize debouncer.
	uint32_t debouncer = 0;
	
	//While Loop
	while(1) {
	debouncer = (debouncer << 1); // Always shift every loop iteration
	
	//Save the value of the push button then clear the rest of the bits.
	uint32_t gpioval = GPIOA->IDR;
	gpioval = gpioval << 31;
	gpioval = gpioval >> 31;
	
	//If IDR value 1 is enabled enter the for loop as button is pressed.
	if (gpioval) { // If input signal is set/high
		debouncer |= 0x01; // Set lowest bit of bit-vector
		}
		if (debouncer == 0xFFFFFFFF) {
		// This code triggers repeatedly when button is steady high!
		}
		if (debouncer == 0x00000000) {
		// This code triggers repeatedly when button is steady low!
		}
		if (debouncer == 0x7FFFFFFF) {
		// This code triggers only once when transitioning to steady high!
		
			//If the blue LED is on turn it off and turn on the red LED.
			if(GPIOC->ODR >> 7 == 1) {
				GPIOC->ODR &= ~(1 << 7);
				GPIOC->ODR |= (1 << 6);
			} else { //If the blue LED is off turn it on and turn off the red LED.
				GPIOC->ODR &= ~(1 << 6);
				GPIOC->ODR |= (1 << 7);
			}
		}
	}
	
	
	/* Code for blinking without button ---------------------------------------------------------*/
	
	//GPIOC->MODER |= (1 << 14) | (1 << 12);
	//GPIOC->ODR |= (1 << 6);
	
	
	//while (1) {
		//HAL_Delay(500); // Delay 200ms
		// Toggle the output state of both PC8 and PC9
		
		//if(GPIOC->ODR >> 7 == 1) {
		//	GPIOC->ODR &= ~(1 << 7);
		//	GPIOC->ODR |= (1 << 6);
		//} else {
		//	GPIOC->ODR &= ~(1 << 6);
		//	GPIOC->ODR |= (1 << 7);
		//}
	//}
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
