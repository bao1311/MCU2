/*
 * main_app.c
 *
 *  Created on: Oct 15, 2025
 *      Author: gphi1
 */
#include "main_app.h"
GPIO_InitTypeDef gpioLed;
TIM_HandleTypeDef htimer6;
void TIMER6_Init();
void GPIO_Led_Init();
int main()
{
	HAL_Init();
	GPIO_Led_Init();

	TIMER6_Init();
//	HAL_TIM_Base_Start(&htimer6);
	// Hae
	HAL_TIM_Base_Start_IT(&htimer6);
	return 0;
}

void TIM6_DAC_IRQHandler()
{

}

void GPIO_Led_Init()
{
	// Enable clock of GPIOA
	__HAL_RCC_GPIOD_CLK_ENABLE();
	// LED info:
	/*
	 * Port: D
	 * No: 13
	 */
	gpioLed.Mode = GPIO_MODE_OUTPUT_PP;
	gpioLed.Pin = GPIO_PIN_13;
	gpioLed.Pull = GPIO_NOPULL;


	HAL_GPIO_Init(GPIOD, &gpioLed);
}

void TIMER6_Init()
{
	// Enable clock of TIMER 6
	__HAL_RCC_TIM6_CLK_ENABLE();
	// Reset TIMER6 handle variable to default
	memset(&htimer6, 0, sizeof(htimer6));
	// Initialize Timer 6
	htimer6.Instance = TIM6;

	// Configure Timer's Init field
	/*
	 * Three main attributes is:
	 * + Prescaler
	 * + Counter Mode
	 * + Period
	 */
	htimer6.Init.Prescaler = 1000;
	htimer6.Init.CounterMode = TIM_COUNTERMODE_UP;
	// Deduct 1 because of the one more up cycle
	htimer6.Init.Period = 32000-1;
	HAL_TIM_Base_Init(&htimer6);

}

