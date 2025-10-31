/*
 * msp.c
 *
 *  Created on: Oct 2, 2025
 *      Author: gphi1
 */

#include "stm32f4xx_hal.h"
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
void HAL_MspInit(void)
{
	// This file will do the low level processor specific inits

	// 1. Set up the priority grouping of arm cortex mx processor
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	// 2. Enable the required system exceptions of the arm cortex mx processor
	SCB->SHCSR |= (0x7 << 16);
	// 3. Configure the pri:ority for the system exceptions
	HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
	HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
	HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
}
