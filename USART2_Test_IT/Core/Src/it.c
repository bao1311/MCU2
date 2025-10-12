/*
 * it.c
 *
 *  Created on: Oct 2, 2025
 *      Author: gphi1
 */
#include "main.h"
extern UART_HandleTypeDef huart2;
void SysTick_Handler(void)
{
	// Increase the global systick value
	HAL_IncTick();
	// Call the Interrupt handler
	HAL_SYSTICK_IRQHandler();

}

void USART2_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart2);
}


