/*
 * it.c
 *
 *  Created on: Oct 2, 2025
 *      Author: gphi1
 */

void SysTick_Handler(void)
{
	// Increase the global systick value
	HAL_IncTick();
	// Call the Interrupt handler
	HAL_SYSTICK_IRQHandler();

}

