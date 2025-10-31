/*
 * msp.c
 *
 *  Created on: Oct 11, 2025
 *      Author: gphi1
 */

#include "main_app.h"


void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{

	/*
	 * Low level configuration
	 */

	// Enable USART2 Clock
	__HAL_RCC_USART2_CLK_ENABLE();
	// Enable GPIO Clock
	__HAL_RCC_GPIOA_CLK_ENABLE();
	// Pin Configurations
	GPIO_InitTypeDef gpio;
	gpio.Alternate = GPIO_AF7_USART2;
	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Pull = GPIO_PULLUP;
	gpio.Speed = GPIO_SPEED_FAST;

	// USART2_TX -> PA2
	gpio.Pin = GPIO_PIN_2;
	HAL_GPIO_Init(GPIOA, &gpio);

	// USART2_RX -> PA3
	gpio.Pin = GPIO_PIN_3;
	HAL_GPIO_Init(GPIOA, &gpio);

	// NVIC Configuration Enable Interrupt
	HAL_NVIC_EnableIRQ(USART2_IRQn);

	// Interrupt priority set
	HAL_NVIC_SetPriority(USART2_IRQn, 15, 0);

}


void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	__HAL_RCC_TIM6_CLK_ENABLE();


	HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
}
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
	GPIO_InitTypeDef hgpio;
	// Enable TIM2 Clock
	__HAL_RCC_TIM2_CLK_ENABLE();
	// Enalbe TIM6 Clock
	__HAL_RCC_TIM6_CLK_ENABLE();
	// Enable GPIO Clock
	__HAL_RCC_GPIOA_CLK_ENABLE();
	// Config gpio A pin 0 to behave like a timer
	hgpio.Alternate = GPIO_AF1_TIM2;
	hgpio.Mode = GPIO_MODE_AF_PP;
	hgpio.Pin = GPIO_PIN_0;
	hgpio.Pull = GPIO_NOPULL;
	// Enable GPIO pin
	HAL_GPIO_Init(GPIOA, &hgpio);

	// Enable IRQ of Timer 2 Set NVIC priority for Timer 2
	if (htim->Instance == TIM2)
	{
		HAL_NVIC_SetPriority(TIM2_IRQn, 15, 0);
		HAL_NVIC_EnableIRQ(TIM2_IRQn);

	}
	else if(htim->Instance == TIM6)
	{
		HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 15, 0);
		HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
	}
}
