/*
 * main.c
 *
 *  Created on: Oct 11, 2025
 *      Author: gphi1
 */
#include "main.h"
#include "stdio.h"
#include "string.h"
UART_HandleTypeDef huart2;
void Err_Handler();
void UART2_Init(void);
int main(void)
{
	HAL_Init();

	char msg[100];

	/*
	 * Test seeing the Clock frequency when clk_source is HSI
	 */
	UART2_Init();
	memset(msg, 0, sizeof(msg));
	sprintf(msg, "SysCLK: %ld\r\n", HAL_RCC_GetSysClockFreq());
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

	memset(msg, 0, sizeof(msg));
	sprintf(msg, "HCLK: %ld\r\n", HAL_RCC_GetHCLKFreq());
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);


	memset(msg, 0, sizeof(msg));
	sprintf(msg, "PCLK1: %ld\r\n", HAL_RCC_GetPCLK1Freq());
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

	memset(msg, 0, sizeof(msg));
	sprintf(msg, "PCLK2: %ld\r\n", HAL_RCC_GetPCLK2Freq());
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
	// Initial configuration of the clock and oscillator
	RCC_ClkInitTypeDef clk_init;
	RCC_OscInitTypeDef osc_init;

	// Reset the osc_init to avoid garbage value
	memset(&osc_init,0,sizeof(osc_init));
	// Config for the oscillator (hse, hse_bypass mode)
	osc_init.HSEState = RCC_HSE_BYPASS;
	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	if (HAL_RCC_OscConfig(&osc_init) != HAL_OK)
	{
		Err_Handler();
	}
	/*
	 * Task: Use HSE to config clock
	 * - AHB == 4MHz (HCLK)
	 * - APB1 -- 2MHz (PCLK1)
	 * - APB2 -- 2MHz (PCLK2)
	 */
	clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
						RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
	clk_init.AHBCLKDivider = RCC_SYSCLK_DIV2;
	clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
	clk_init.APB2CLKDivider = RCC_HCLK_DIV2;
	// Configure the clock so it satisfies the requirement
	HAL_RCC_ClockConfig(&clk_init,0);

	// ------------------------------------------------
	/* After this line, HSE is enabled*/
	// ------------------------------------------------
	// Initialize uart2 since we got a new clock source (4Mhz)
	UART2_Init();

	// Disable HSI to save some power
	__HAL_RCC_HSI_DISABLE();
	// Reconfigure the SYSTICK (configure the ticks)
	SysTick_Config(HAL_RCC_GetHCLKFreq()/1000);
	// Cortex timer configuration
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
	memset(msg, 0, sizeof(msg));
	sprintf(msg, "SysCLK: %ld\r\n", HAL_RCC_GetSysClockFreq());
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

	memset(msg, 0, sizeof(msg));
	sprintf(msg, "HCLK: %ld\r\n", HAL_RCC_GetHCLKFreq());
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);


	memset(msg, 0, sizeof(msg));
	sprintf(msg, "PCLK1: %ld\r\n", HAL_RCC_GetPCLK1Freq());
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

	memset(msg, 0, sizeof(msg));
	sprintf(msg, "PCLK2: %ld\r\n", HAL_RCC_GetPCLK2Freq());
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
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


