/*
 * main.c
 *
 *  Created on: Oct 2, 2025
 *      Author: gphi1
 */
#include "stm32f4xx.h"

void UART2_Init(void);
void Err_Handler(void);
void SystemClockConfig(void);
UART_HandleTypeDef huart2;
int main(void)
{
	HAL_Init();
	SystemClockConfig();
	UART2_Init();
	return 0;
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{

	/*
	 * Low level configuration
	 */

	// Enable USART2 Clock
	__HAL_RCC_USART2_CLK_ENABLE();
	// Pin Configurations
	GPIO_InitTypeDef gpio;
	gpio.Alternate = GPIO_AF7_USART2;
	gpio.Mode = GPIO_MODE_OUTPUT_PP;
	gpio.Pull = GPIO_PULLUP;
	gpio.Speed = GPIO_SPEED_FAST;

	// USART2_TX -> PA2
	gpio.Pin = GPIO_PIN_2;
	HAL_GPIO_Init(GPIOA, &gpio);

	// USART2_TX -> PA3
	gpio.Pin = GPIO_PIN_3;
	HAL_GPIO_Init(GPIOA, &gpio);

	// NVIC Configuration Enable Interrupt
	HAL_NVIC_EnableIRQ(USART2_IRQn);

	// Interrupt priority set
	HAL_NVIC_SetPriority(USART2_IRQn, 15, 0);

}
void UART2_Init(void)
{

	// High level configuration
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = 1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		Err_Handler();
	}

}


void Err_Handler(void)
{
	while (1)
	{
		;
	}
}


void SystemClockConfig(void)
{

}
