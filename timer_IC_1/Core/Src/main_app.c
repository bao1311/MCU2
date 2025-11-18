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
TIM_HandleTypeDef htimer2;
#define SYSCLK_FREQ_50_MHz			0
#define SYSCLK_FREQ_84_MHz			1
#define SYSCLK_FREQ_120_MHz			2
#define SYSCLK_FREQ_168_MHz			3
#define TRUE						1
#define FALSE						0
uint32_t arr[2] = {0};
uint8_t count = 1;
uint8_t done = 0;
void TIMER6_Init();
void TIMER2_Init();
void GPIO_Led_Init();
void LSE_Configuration();
void SystemClockConfig(uint8_t SYSCLKFreq);
void Err_Handler(void);
UART_HandleTypeDef huart2;
void UART2_Init(void);
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
int main()
{
	HAL_Init();
	SystemClockConfig(SYSCLK_FREQ_50_MHz);
	// Debugging tool
	UART2_Init();
	uint32_t sys_freq = HAL_RCC_GetSysClockFreq();
	char debug_msg[100];
	sprintf(debug_msg, "Frequency of SYSCLK is: %lu Hz \r\n", sys_freq);
 	HAL_UART_Transmit(&huart2, &debug_msg, strlen(debug_msg), HAL_MAX_DELAY);

	GPIO_Led_Init();
	//LSE_Configuration();
	TIMER2_Init();
	TIMER6_Init();
	uint32_t hsi_cnt = 0;

	char usr_msg[200] = "";
	double pclk2 = HAL_RCC_GetPCLK2Freq();
	double tim6_freq = pclk2 / (1 + htimer6.Init.Prescaler);
	sprintf(usr_msg, "TIM6 frequency is %.2f Hz (expected: 2500000 Hz)\r\n", tim6_freq);
	HAL_UART_Transmit(&huart2, (uint8_t*)usr_msg, strlen(usr_msg), HAL_MAX_DELAY);
//SystemClockConfig(SYSCLK_FREQ_50_MHz);
//	HAL_TIM_Base_Start_IT(&htimer6);
	// Enable Input Capture Interrupt of Timer 2
	// Toggle LED every 50kHz
	HAL_TIM_Base_Start_IT(&htimer6);
	//
//	HAL_TIM_IC_Start_IT(&htimer2, TIM_CHANNEL_1);
//	char* usr_msg = "Hi, I am here!\r\n";
//	HAL_UART_Transmit(&huart2, usr_msg, strlen(usr_msg), HAL_MAX_DELAY);

	while (1)
	{
		if (done)
		{
			if (arr[1] > arr[0])
			{
				hsi_cnt = arr[1] - arr[0];
			}
			else
			{
				hsi_cnt = 0xFFFFFFFF - arr[0] + arr[1];
			}
			done = FALSE;
			double tim2_freq = HAL_RCC_GetPCLK1Freq() / (htimer2.Init.Prescaler + 1);
			double tim2_res = 1 / tim2_freq;
			double hsi_time = tim2_res * hsi_cnt;
			double hsi_freq = 1 / hsi_time;

			sprintf(usr_msg, "HSI Frequency: %.2f Hz (expected: 16000000 Hz)\r\n", hsi_freq);
			HAL_UART_Transmit(&huart2, (uint8_t*)usr_msg, strlen(usr_msg), HAL_MAX_DELAY);

			sprintf(usr_msg, "------------------------------------------\r\n");
			HAL_UART_Transmit(&huart2, (uint8_t*)usr_msg, strlen(usr_msg), HAL_MAX_DELAY);


		}
	}
	return 0;
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


void LSE_Configuration()
{
	RCC_OscInitTypeDef osc_init;
	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSI;
//	osc_init.LSEState = RCC_LSE_BYPASS;
	osc_init.HSIState = RCC_HSI_ON;
	if (HAL_RCC_OscConfig(&osc_init) != HAL_OK)
	{
		Err_Handler();
	}

	// Link PA8 to HSI DIV 5
	HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCOSOURCE_HSI, RCC_MCODIV_4);

}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (!done)
	{
		if (count == 1)
		{
			arr[0] = __HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_1);
			count++;
		}
		else
		{
			arr[1] = __HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_1);
			done = TRUE;

			// stop the interrupt
//            HAL_TIM_IC_Stop_IT(htim, TIM_CHANNEL_1);

		}
	}
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
	gpioLed.Pin = GPIO_PIN_12;
	gpioLed.Pull = GPIO_NOPULL;


	HAL_GPIO_Init(GPIOD, &gpioLed);
}

void TIMER2_Init()
{
	// Reset htimer2 to default state
	memset(&htimer2, 0, sizeof(htimer2));


	// Config TIMER2 configuration for Input Capture
	htimer2.Instance = TIM2;
	/*
	 * Stats:
	 * + Prescaler: 1
	 * + Period: 0xFFFFFFFF
	 * + CounterMode: UP
	 */
	htimer2.Init.Prescaler = 0;
	htimer2.Init.Period = 0xFFFFFFFF;
	htimer2.Init.CounterMode = TIM_COUNTERMODE_UP;
	// Init TIMER2
	if (HAL_TIM_IC_Init(&htimer2) != HAL_OK)
	{
		Err_Handler();
	}
	TIM_IC_InitTypeDef Tim_IC_Init;
	memset(&Tim_IC_Init, 0, sizeof(Tim_IC_Init));
	Tim_IC_Init.ICFilter = 0;
	Tim_IC_Init.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
	Tim_IC_Init.ICPrescaler = TIM_ICPSC_DIV1;
	Tim_IC_Init.ICSelection = TIM_ICSELECTION_DIRECTTI;

	if (HAL_TIM_IC_ConfigChannel(&htimer2, &Tim_IC_Init, TIM_CHANNEL_1) != HAL_OK)
	{

		Err_Handler();

	}

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
	htimer6.Init.Prescaler = 25000-1;
	htimer6.Init.CounterMode = TIM_COUNTERMODE_UP;
	// Deduct 1 because of the one more up cycle
	htimer6.Init.Period = 50-1;
	HAL_TIM_Base_Init(&htimer6);



}


void SystemClockConfig(uint8_t SYSCLKFreq)
{
	// Initial configuration of the clock and oscillator
	RCC_ClkInitTypeDef clk_init;
	RCC_OscInitTypeDef osc_init;

	// Reset the osc_init to avoid garbage value
	memset(&osc_init,0,sizeof(osc_init));
	// Reset clk_init to avoid garbage value
	memset(&clk_init,0,sizeof(clk_init));
	// Config for the oscillator (PLL, with clock source HSI)
	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_HSI;
	osc_init.HSEState = RCC_HSE_ON;
	osc_init.HSIState = RCC_HSI_ON;
	// Config for LSEState (Does not apply since there is no LSE on stm32f4 discovery board)
	//osc_init.LSEState = RCC_LSE_ON;

	switch (SYSCLKFreq) {
		case SYSCLK_FREQ_50_MHz:
			osc_init.PLL.PLLState = RCC_PLL_ON;
			osc_init.PLL.PLLM = 8;
			osc_init.PLL.PLLN = 50;
			osc_init.PLL.PLLP = 2;
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
			clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
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

void Err_Handler(void)
{
	while (1)
	{
		;
	}
}
