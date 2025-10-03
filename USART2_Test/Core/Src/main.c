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

void UART2_Init(void)
{
	huart2.Instance = USART2_BASE;
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
