/*
 * msp.c
 *
 *  Created on: Oct 11, 2025
 *      Author: gphi1
 */

#include "main_app.h"


void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
	GPIO_InitTypeDef hgpio;
	// Enable TIM2 Clock
	__HAL_RCC_TIM2_CLK_ENABLE();
	// Enable GPIO Clock
	__HAL_RCC_GPIOA_CLK_ENABLE();
	// Config gpio A pin 0 to behave like a timer
	hgpio.Alternate = 1;
	hgpio.Mode = GPIO_MODE_AF_PP;
	hgpio.Pin = GPIO_PIN_0;
	hgpio.Pull = GPIO_NOPULL;
	// Enable GPIO pin
	HAL_GPIO_Init(GPIOA, &hgpio);

	// Enable IRQ of Timer 2 Set NVIC priority for Timer 2
	HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 15, 0);
	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
}
