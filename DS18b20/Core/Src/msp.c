/*
 * msp.c
 *
 *  Created on: Oct 31, 2025
 *      Author: gphi1
 */

#include "main_app.h"


/*
 * @fn					- HAL_TIM_Base_MspInit
 * @brief				- Set up the low level of timer 2
 * @param				- TIM_HandleTypeDef
 * @return				- void
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	// Enable the clock for Timer 2
	__HAL_RCC_TIM2_CLK_ENABLE();

	// Config the NVIC enable interrupt
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
	// Config the NVIC set the priority
	HAL_NVIC_SetPriority(TIM2_IRQn, 15, 0);

}
/*
 * Configuration details:
 * - PA2: USART2_TX
 * - PA3: USART2_RX
 */

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	// Enable Clock for USART2
	__HAL_RCC_USART2_CLK_ENABLE();
	// Enable Clock for GPIOA
	__HAL_RCC_GPIOA_CLK_ENABLE();
	// Handle Type Def for GPIO
	GPIO_InitTypeDef gpio;
	// Configure for PA2
	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Alternate = GPIO_AF7_USART2;
	gpio.Pull = GPIO_PULLUP;

	gpio.Pin = GPIO_PIN_2;
	HAL_GPIO_Init(GPIOA, &gpio);
	// Configure for PA3
	gpio.Pin = GPIO_PIN_3;
	HAL_GPIO_Init(GPIOA, &gpio);
	// Config the NVIC enable interrupt
	HAL_NVIC_EnableIRQ(USART2_IRQn);
	// Config the NVIC set the priority
	HAL_NVIC_SetPriority(USART2_IRQn, 15, 0);
}


/**
  * @brief  Initialize the I2C MSP.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *         the configuration information for the specified I2C.
  * @retval None
  */
void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
	if (hi2c->Instance == I2C1)
	{
		// Enable clock of GPIOB
		__HAL_RCC_GPIOB_CLK_ENABLE();
		/*
		 * Initialize GPIO (PB6,PB7) for I2C1
		 */
		GPIO_InitTypeDef lcd = {0};
		lcd.Mode = GPIO_MODE_AF_OD;
		lcd.Pull = GPIO_NOPULL;
		lcd.Alternate = GPIO_AF4_I2C1;
		/*
		 * Pin Info:
		 * - PB6
		 * - SCL
		 */
		lcd.Pin = GPIO_PIN_6;
		HAL_GPIO_Init(GPIOB, &lcd);

		/*
		 * Pin Info:
		 * - PB7
		 * - SDA
		 */
		lcd.Pin = GPIO_PIN_7;
		HAL_GPIO_Init(GPIOB, &lcd);
	}
}
