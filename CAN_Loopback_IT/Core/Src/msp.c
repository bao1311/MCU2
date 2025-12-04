/*
 * msp.c
 *
 *  Created on: Oct 11, 2025
 *      Author: gphi1
 */

#include "main.h"

/*
 * @fn					- HAL_CAN_MspInit
 * @brief				- Low level initialization of CAN bus
 * @param				-
 * @return				-
 */
void HAL_CAN_MspInit(CAN_HandleTypeDef *hcan)
{
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_CAN1_CLK_ENABLE();

	GPIO_InitTypeDef can;
	can.Mode = GPIO_MODE_AF_PP;
	can.Pull = GPIO_PULLUP;
	can.Alternate = GPIO_AF9_CAN1;
	can.Pin = GPIO_PIN_8;
	// PB8 -> CAN1_RX
	HAL_GPIO_Init(GPIOB, &can);

	// PB9 -> CAN1_TX
	can.Pin = GPIO_PIN_9;

	HAL_GPIO_Init(GPIOB, &can);

}


void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{

	/*
	 * Low level configuration
	 */

	// Enable USART2 Clock
	__HAL_RCC_USART2_CLK_ENABLE();
	// Enable GPIO Clock
	__HAL_RCC_GPIOA_CLK_ENABLE();

	__HAL_RCC_GPIOD_CLK_ENABLE();
	// Pin Configurations
	GPIO_InitTypeDef gpio;
	gpio.Alternate = GPIO_AF7_USART2;
	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Pull = GPIO_PULLUP;
	gpio.Speed = GPIO_SPEED_FAST;

	// USART2_TX -> PA2
	gpio.Pin = GPIO_PIN_5;
	HAL_GPIO_Init(GPIOD, &gpio);

	// USART2_RX -> PA3
	gpio.Pin = GPIO_PIN_6;
	HAL_GPIO_Init(GPIOD, &gpio);

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


void HAL_TIM_OC_MspInit(TIM_HandleTypeDef *htim)
{
	__HAL_RCC_TIM2_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef gpio;
	// PA0, PA1 configuration
	memset(&gpio,0,sizeof(gpio));
	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Alternate = GPIO_AF1_TIM2;
	gpio.Pin = GPIO_PIN_0 | GPIO_PIN_1;
	gpio.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &gpio);
	// PB10, PB11 configuration
	memset(&gpio,0,sizeof(gpio));
	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Alternate = GPIO_AF1_TIM2;
	gpio.Pin = GPIO_PIN_10 | GPIO_PIN_11;
	gpio.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &gpio);

	if (htim->Instance == TIM2)
	{
		HAL_NVIC_SetPriority(TIM2_IRQn, 15, 0);
		HAL_NVIC_EnableIRQ(TIM2_IRQn);

	}
}
