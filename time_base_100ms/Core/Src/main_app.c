/*
 * main_app.c
 *
 *  Created on: Oct 15, 2025
 *      Author: gphi1
 */
#include "main_app.h"
#include "stm32f407xx.h"
TIM_HandleTypeDef htimer6;
int main()
{
	HAL_Init();


	TIMER6_Init();
	return 0;
}

void TIMER6_Init()
{
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
	htimer6.Init.Prescaler = 24;
	htimer6.Init.CounterMode = TIM_COUNTERMODE_UP;
	// Deduct 1 because of the one more up cycle
	htimer6.Init.Period = 64000-1;

}

