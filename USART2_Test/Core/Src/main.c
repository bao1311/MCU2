/*
 * main.c
 *
 *  Created on: Oct 2, 2025
 *      Author: gphi1
 */
#include "stm32f4xx.h"
#include "stdint.h"
#include "string.h"
void UART2_Init(void);
void Err_Handler(void);
void SystemClockConfig(void);
UART_HandleTypeDef huart2;
char *msg = "I am Baobao and I am so handsome hehe\r\n";
uint8_t capitalize(uint8_t c)
{
	if ('a' <= c && c <= 'z')
		return c - ('a' - 'A');
	else
		return c;
}
void delay()
{
	for (int i = 0; i < 1000; ++i)
	{
		;
	}
}
int main(void)
{
	HAL_Init();
//	SystemClockConfig();
	UART2_Init();
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

	uint8_t rcvd_data;
	uint8_t rcvd_buffer[100];
	uint32_t count=0;
	while(1)
	{
		HAL_UART_Receive(&huart2, &rcvd_data, 1, HAL_MAX_DELAY);
		if (rcvd_data == '\r')
		{
			break;
		}
		else
		{
			rcvd_buffer[count++] = capitalize(rcvd_data);
		}
	}
	rcvd_buffer[count++] = '\r';
	HAL_UART_Transmit(&huart2, rcvd_buffer, count, HAL_MAX_DELAY);

	while (1)
	{
		;
	}
	return 0;
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
void UART2_Init(void)
{

	// High level configuration
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
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
