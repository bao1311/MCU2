/*
 * it.c
 *
 *  Created on: Oct 11, 2025
 *      Author: gphi1
 */
#include "main_app.h"
extern TIM_HandleTypeDef htimer6;
void SysTick_Handler()
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}


void TIM6_DAC_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htimer6);
}




