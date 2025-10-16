/*
 * it.c
 *
 *  Created on: Oct 11, 2025
 *      Author: gphi1
 */


void SysTick_Handler()
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}


