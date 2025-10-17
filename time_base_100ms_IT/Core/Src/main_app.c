/*
 * main_app.c
 *
 *  Created on: Oct 15, 2025
 *      Author: gphi1
 */
#include "main_app.h"
#include <string.h>
GPIO_InitTypeDef gpioLed;
TIM_HandleTypeDef htimer6;
#define SYSCLK_FREQ_50_MHz			0
#define SYSCLK_FREQ_84_MHz			1
#define SYSCLK_FREQ_120_MHz			2
#define SYSCLK_FREQ_168_MHz			3
void TIMER6_Init();
void GPIO_Led_Init();
void SystemClockConfig(uint8_t SYSCLKFreq);
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
int main()
{
	HAL_Init();
	GPIO_Led_Init();

	TIMER6_Init();
//	HAL_TIM_Base_Start(&htimer6);
	// Begin the Timer Interrupt
	HAL_TIM_Base_Start_IT(&htimer6);
	while (1)
	{
		;
	}
	return 0;
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
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

	// Configure the NVIC Priority
	HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 2, 0);
	// Enable the IRQ number
	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);


}


void SystemClockConfig(uint8_t SYSCLKFreq)
{
	// Initial configuration of the clock and oscillator
	RCC_ClkInitTypeDef clk_init;
	RCC_OscInitTypeDef osc_init;

	// Reset the osc_init to avoid garbage value
	memset(&osc_init,0,sizeof(osc_init));
	// Config for the oscillator (PLL, with clock source HSI)
	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	osc_init.HSEState = RCC_HSE_ON;

	switch (SYSCLKFreq) {
		case SYSCLK_FREQ_50_MHz:
			osc_init.PLL.PLLState = RCC_PLL_ON;
			osc_init.PLL.PLLM = 8;
			osc_init.PLL.PLLN = 50;
			osc_init.PLL.PLLP = 2;
			if (HAL_RCC_OscConfig(&osc_init) != HAL_OK)
			{
				Err_Handler();
			}
			clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
								RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
			clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
			clk_init.AHBCLKDivider = RCC_SYSCLK_DIV2;
			clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
			clk_init.APB2CLKDivider = RCC_HCLK_DIV1;
			HAL_RCC_ClockConfig(&clk_init,0);
			break;
		case SYSCLK_FREQ_84_MHz:
			osc_init.PLL.PLLState = RCC_PLL_ON;
			osc_init.PLL.PLLM = 16;
			osc_init.PLL.PLLN = 168;
			osc_init.PLL.PLLP = 2;
			osc_init.PLL.PLLQ = 2;
			osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSI;
			if (HAL_RCC_OscConfig(&osc_init) != HAL_OK)
			{
				Err_Handler();
			}

			clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
								RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
			clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
			clk_init.AHBCLKDivider = RCC_SYSCLK_DIV2;
			clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
			clk_init.APB2CLKDivider = RCC_HCLK_DIV1;
			// Remember the different wait state (Can double check in reference manual)
			HAL_RCC_ClockConfig(&clk_init,1);
			break;
		case SYSCLK_FREQ_168_MHz:
			osc_init.PLL.PLLState = RCC_PLL_ON;
			osc_init.PLL.PLLM = 8;
			osc_init.PLL.PLLN = 336;
			osc_init.PLL.PLLP = 2;
			osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSE;
			osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE;
			if (HAL_RCC_OscConfig(&osc_init) != HAL_OK)
			{
				Err_Handler();
			}
			clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
								RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
			clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
			clk_init.AHBCLKDivider = RCC_SYSCLK_DIV2;
			clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
			clk_init.APB2CLKDivider = RCC_HCLK_DIV1;

		// Configure the clock so it satisfies the requirement
		// The wait state depends on the Clock frequency so please check it
		// in the reference manual
			HAL_RCC_ClockConfig(&clk_init,5);
			break;
		case SYSCLK_FREQ_120_MHz:
			osc_init.PLL.PLLState = RCC_PLL_ON;
			osc_init.PLL.PLLM = 8;
			osc_init.PLL.PLLN = 120;
			osc_init.PLL.PLLP = 2;
			if (HAL_RCC_OscConfig(&osc_init) != HAL_OK)
			{
				Err_Handler();
			}

			clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
								RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
			clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
			clk_init.AHBCLKDivider = RCC_SYSCLK_DIV2;
			clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
			clk_init.APB2CLKDivider = RCC_HCLK_DIV1;

		// Configure the clock so it satisfies the requirement
		// The wait state depends on the Clock frequency so please check it
		// in the reference manual
			HAL_RCC_ClockConfig(&clk_init,3);
			break;

		default:
			break;

	}


}


