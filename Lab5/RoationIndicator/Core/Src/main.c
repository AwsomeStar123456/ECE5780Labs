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
void I2C_Init(void);
void readsinglebyte(uint32_t reg);
void readx(void);
void ready(void);
void gyroInit(void);
void gyroInit2(void);

int16_t xData, yData;
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
  SystemClock_Config();

	GPIO_Init();
	I2C_Init();
	
	xData = 0;
	yData = 0;
	
	//Enable Gryo x and y & turn off sleep mode
	gyroInit2();
	gyroInit(); //uint32_t RXData = I2C2->RXDR;
	
  while (1)
  {
		readx();
		ready();
		
		if(xData > 5000) {
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
		} else if(xData < -5000) {
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
		} else {
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
		}
		
		if(yData > 5000) {
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
		} else if(yData < -5000) {
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
		} else {
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
		}
		
		HAL_Delay(100);
  }
}

void readx(void) {
	xData = 0;

	//WRITE
	//Set the slave address
	I2C2->CR2 |= (0x69 << 1);
	//Clear # of Bytes
	I2C2->CR2 &= ~(0xFF << 16);
	//Set the # of bytes to 1
	I2C2->CR2 |= (1 << 16);
	//Enable a write
	I2C2->CR2 &= ~(1 << 10);
	//Enable the start bit
	I2C2->CR2 |= (1 << 13);
	
	//Wait for the TXIS flag
	while(1)
	{
		if((I2C2->ISR & (1 << 1))) {
			break;
		}
		
		if((I2C2->ISR & (1 << 4))) {
			while(1) {
				//Enable red LED if there was an error
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
			}
		}
	}
	
	//Set the transmit data
	I2C2->TXDR = 0xA8;
	
	//Wait for the transmit complete flag
	while(!(I2C2->ISR & (1 << 6)))
	{
		
	}
	
	//READ
	//Set the slave address
	I2C2->CR2 |= (0x69 << 1);
	//Clear # of Bytes
	I2C2->CR2 &= ~(0xFF << 16);
	//Set the # of bytes to 2
	I2C2->CR2 |= (1 << 17);
	//Enable a read
	I2C2->CR2 |= (1 << 10);
	//Enable the start bit
	I2C2->CR2 |= (1 << 13);
	
	//Wait for the RXNE Flag
	while(1)
	{
		if((I2C2->ISR & (1 << 2))) {
			break;
		}
		
		if((I2C2->ISR & (1 << 4))) {
			while(1) {
				//Enable red LED if there was an error
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
			}
		}
	}
	
	xData |= I2C2->RXDR;
	
	//Wait for the RXNE Flag
	while(1)
	{
		if((I2C2->ISR & (1 << 2))) {
			break;
		}
		
		if((I2C2->ISR & (1 << 4))) {
			while(1) {
				//Enable red LED if there was an error
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
			}
		}
	}
	
	xData |= ((I2C2->RXDR) << 8);
	
	//Wait for the transmit complete flag
	while(!(I2C2->ISR & (1 << 6)))
	{
		
	}
	
	//Send the stop bit
	I2C2->CR2 |= (1 << 14);
	
	//Wait for the stop condition to be generated
  while(I2C2->CR2 & (1 << 14))
  {
        
  }

  //Clear the stop bit
  I2C2->CR2 &= ~(1 << 14);
	
}

void ready(void) {
	yData = 0;

	//WRITE
	//Set the slave address
	I2C2->CR2 |= (0x69 << 1);
	//Clear # of Bytes
	I2C2->CR2 &= ~(0xFF << 16);
	//Set the # of bytes to 1
	I2C2->CR2 |= (1 << 16);
	//Enable a write
	I2C2->CR2 &= ~(1 << 10);
	//Enable the start bit
	I2C2->CR2 |= (1 << 13);
	
	//Wait for the TXIS flag
	while(1)
	{
		if((I2C2->ISR & (1 << 1))) {
			break;
		}
		
		if((I2C2->ISR & (1 << 4))) {
			while(1) {
				//Enable red LED if there was an error
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
			}
		}
	}
	
	//Set the transmit data
	I2C2->TXDR = 0xAA;
	
	//Wait for the transmit complete flag
	while(!(I2C2->ISR & (1 << 6)))
	{
		
	}
	
	//READ
	//Set the slave address
	I2C2->CR2 |= (0x69 << 1);
	//Clear # of Bytes
	I2C2->CR2 &= ~(0xFF << 16);
	//Set the # of bytes to 2
	I2C2->CR2 |= (1 << 17);
	//Enable a read
	I2C2->CR2 |= (1 << 10);
	//Enable the start bit
	I2C2->CR2 |= (1 << 13);
	
	//Wait for the RXNE Flag
	while(1)
	{
		if((I2C2->ISR & (1 << 2))) {
			break;
		}
		
		if((I2C2->ISR & (1 << 4))) {
			while(1) {
				//Enable red LED if there was an error
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
			}
		}
	}
	
	yData |= I2C2->RXDR;
	
	//Wait for the RXNE Flag
	while(1)
	{
		if((I2C2->ISR & (1 << 2))) {
			break;
		}
		
		if((I2C2->ISR & (1 << 4))) {
			while(1) {
				//Enable red LED if there was an error
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
			}
		}
	}
	
	yData |= ((I2C2->RXDR) << 8);
	
	//Wait for the transmit complete flag
	while(!(I2C2->ISR & (1 << 6)))
	{
		
	}
	
	//Send the stop bit
	I2C2->CR2 |= (1 << 14);
	
	//Wait for the stop condition to be generated
  while(I2C2->CR2 & (1 << 14))
  {
        
  }

  //Clear the stop bit
  I2C2->CR2 &= ~(1 << 14);
	
}

void gyroInit(void) {
	//Set the slave address
	I2C2->CR2 |= (0x69 << 1);
	//Clear # of Bytes
	I2C2->CR2 &= ~(0xFF << 16);
	//Set the # of bytes to 2
	I2C2->CR2 |= (2 << 16);
	//Enable a write
	I2C2->CR2 &= ~(1 << 10);
	//Enable the start bit
	I2C2->CR2 |= (1 << 13);
	
	//Wait for the TXIS flag
	while(1)
	{
		if((I2C2->ISR & (1 << 1))) {
			break;
		}
		
		if((I2C2->ISR & (1 << 4))) {
			while(1) {
				//Enable red LED if there was an error
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
			}
		}
	}
	
	//Set the transmit data
	I2C2->TXDR = 0x20;
	
	//Wait for the TXIS flag
	while(1)
	{
		if((I2C2->ISR & (1 << 1))) {
			break;
		}
		
		if((I2C2->ISR & (1 << 4))) {
			while(1) {
				//Enable red LED if there was an error
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
			}
		}
	}
	
	I2C2->TXDR = 0x0B;
	
	//Wait for the transmit complete flag
	while(!(I2C2->ISR & (1 << 6)))
	{
		
	}
	
	//Send the stop bit
	I2C2->CR2 |= (1 << 14);
	
	//Wait for the stop condition to be generated
  while(I2C2->CR2 & (1 << 14))
  {
        
  }

  //Clear the stop bit
  I2C2->CR2 &= ~(1 << 14);
	
}

void gyroInit2(void) {
	//WRITE
	//Set the slave address
	I2C2->CR2 |= (0x69 << 1);
	//Clear # of Bytes
	I2C2->CR2 &= ~(0xFF << 16);
	//Set the # of bytes to 1
	I2C2->CR2 |= (1 << 16);
	//Enable a write
	I2C2->CR2 &= ~(1 << 10);
	//Enable the start bit
	I2C2->CR2 |= (1 << 13);
	
	//Wait for the TXIS flag
	while(1)
	{
		if((I2C2->ISR & (1 << 1))) {
			break;
		}
		
		if((I2C2->ISR & (1 << 4))) {
			while(1) {
				//Enable red LED if there was an error
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
			}
		}
	}
	
	//Set the transmit data
	I2C2->TXDR = 0x0F;
	
	//Wait for the transmit complete flag
	while(!(I2C2->ISR & (1 << 6)))
	{
		
	}
	
	//READ
	I2C2->CR2 |= (0x69 << 1);
	//Clear # of Bytes
	I2C2->CR2 &= ~(0xFF << 16);
	//Set the # of bytes to 1
	I2C2->CR2 |= (1 << 16);
	//Enable a read
	I2C2->CR2 |= (1 << 10);
	//Enable the start bit
	I2C2->CR2 |= (1 << 13);
	
	//Wait for the TXIS flag
	while(1)
	{
		if((I2C2->ISR & (1 << 2))) {
			break;
		}
		
		if((I2C2->ISR & (1 << 4))) {
			while(1) {
				//Enable red LED if there was an error
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
			}
		}
	}

	uint32_t RXData = I2C2->RXDR;
	
	//Wait for the transmit complete flag
	while(!(I2C2->ISR & (1 << 6)))
	{
		
	}
	
	//Send the stop bit
	I2C2->CR2 |= (1 << 14);

	//Wait for the stop condition to be generated
  while(I2C2->CR2 & (1 << 14))
  {
        
  }

  //Clear the stop bit
  I2C2->CR2 &= ~(1 << 14);
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
	
	//Init the SDA pin PB11
	GPIO_InitTypeDef PB11 = {GPIO_PIN_11, GPIO_MODE_AF_OD, GPIO_SPEED_FREQ_LOW, GPIO_NOPULL, GPIO_AF1_I2C2};
	HAL_GPIO_Init(GPIOB, &PB11);
	
	//Init the SCL pin PB13
	GPIO_InitTypeDef PB13 = {GPIO_PIN_13, GPIO_MODE_AF_OD, GPIO_SPEED_FREQ_LOW, GPIO_NOPULL, GPIO_AF5_I2C2};
	HAL_GPIO_Init(GPIOB, &PB13);
	
	//Init PB14 Set High
	GPIO_InitTypeDef PB14 = {GPIO_PIN_14, GPIO_MODE_OUTPUT_PP, GPIO_SPEED_FREQ_LOW, GPIO_NOPULL};
	HAL_GPIO_Init(GPIOB, &PB14);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
	
	//Init PC0 Set High
	GPIO_InitTypeDef PC0 = {GPIO_PIN_0, GPIO_MODE_OUTPUT_PP, GPIO_SPEED_FREQ_LOW, GPIO_NOPULL};
	HAL_GPIO_Init(GPIOC, &PC0);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
	
	//Enable LED Pins
	GPIO_InitTypeDef ledTypeDef = {GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9,
	GPIO_MODE_OUTPUT_PP, GPIO_SPEED_FREQ_LOW, GPIO_NOPULL};
	HAL_GPIO_Init(GPIOC, &ledTypeDef);
}

void I2C_Init(void)
{
	//Enable the CLK to the I2C2 peripheral
	__HAL_RCC_I2C2_CLK_ENABLE();
	
	//Set all of the timer bits based on FIG 5.4
	I2C2->TIMINGR |= ((1 << 28) | (1 << 4) | (1 << 0) | (1 << 1) | (1 << 11) | (1 << 10) | (1 << 9) | (1 << 8) | (1 << 17) | (1 << 22));
	
	//Enable the I2C peripheral
	I2C2->CR1 |= (1 << 0);
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
