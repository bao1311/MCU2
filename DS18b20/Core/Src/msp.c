/*
 * msp.c
 *
 *  Created on: Oct 31, 2025
 *      Author: gphi1
 */

#include "main_app.h"

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
	if (HAL_GPIO_Init(GPIOA, &gpio) != HAL_OK)
	{
		Error_Handler();
	}
	// Configure for PA3
	gpio.Pin = GPIO_PIN_3;
	if (HAL_GPIO_Init(GPIOA, &gpio) != HAL_OK)
	{
		Error_Handler();
	}
	// Config the NVIC enable interrupt
	HAL_NVIC_EnableIRQ(USART2_IRQn);
	// Config the NVIC set the priority
	HAL_NVIC_SetPriority(USART2_IRQn, 15, 0);
}
