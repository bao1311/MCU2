#include "main.h"

/*
 * ********* Pulse value for each channel *********
 */
uint32_t ch1_pulse = 500;
uint32_t ch2_pulse = 12500;
uint32_t ch3_pulse = 6250;
uint32_t ch4_pulse = 3125;

/*
 * ********* Clock frequency macros ***********
 */
#define SYSCLK_FREQ_50_MHz		0
#define SYSCLK_FREQ_84_MHz		1
#define SYSCLK_FREQ_120_MHz		2
/*
 * ********* Global handle **********
 */
UART_HandleTypeDef huart2;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim4;
/*
 * ********* API Prototype **********
 */
void UART2_Init();
void SystemClock_Config(uint8_t SYSCLKFreq);
void delay_us(uint32_t microsec);
void Error_Handler();
void TIM2_Init();
void TIM4_Init();
void Btn_Init();

/*
 * ********** Function Implementation **********
 */

/*
 * @fn					- TIM2_Init
 * @brief				- Set up Timer 2 peripheral
 * @param				-
 * @return				-
 */
void TIM4_Init()
{
	htim4.Instance = TIM4;
	htim4.Init.Prescaler = 50-1;
	htim4.Init.Period = 1000 - 1;
	htim4.Init.CounterMode = TIM_COUNTERMODE_UP;

	if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
	{
		Error_Handler();
	}

	TIM_OC_InitTypeDef tim4OC_Init;
	memset(&tim4OC_Init, 0,sizeof(tim4OC_Init));
	tim4OC_Init.OCMode = TIM_OCMODE_PWM1;
	tim4OC_Init.OCPolarity = TIM_OCPOLARITY_HIGH;
	tim4OC_Init.Pulse = 0;


	// Config Channel 1 of TIM2
	if (HAL_TIM_PWM_ConfigChannel(&htim4, &tim4OC_Init, TIM_CHANNEL_2) != HAL_OK)
	{
		Error_Handler();
	}
}

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	// On every call back we have to update the CCR1
	// CCR1: Capture/Compare 1 Register
	// Channel 1 Callback
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
	{
		uint32_t ccr = __HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_1);
		__HAL_TIM_SET_COMPARE(htim,TIM_CHANNEL_1,ccr+ch1_pulse);
	}
	// Channel 2 Callback
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
	{
		uint32_t ccr = __HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_2);
		__HAL_TIM_SET_COMPARE(htim,TIM_CHANNEL_2,ccr+ch2_pulse);

	}
	// Channel 3 Callback
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
	{

		uint32_t ccr = __HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_3);
		__HAL_TIM_SET_COMPARE(htim,TIM_CHANNEL_3,ccr+ch3_pulse);

	}
	// Channel 4 Callback
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
	{

		uint32_t ccr = __HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_4);
		__HAL_TIM_SET_COMPARE(htim,TIM_CHANNEL_4,ccr+ch4_pulse);

	}
}

/*
 * @fn					- TIM2_Init
 * @brief				- Set up Timer 2 peripheral
 * @param				-
 * @return				-
 */
void TIM2_Init()
{
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 50000-1;
	htim2.Init.Period = 1000 - 1;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;

	if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
	{
		Error_Handler();
	}

	TIM_OC_InitTypeDef tim2OC_Init;
	memset(&tim2OC_Init, 0,sizeof(tim2OC_Init));
	tim2OC_Init.OCMode = TIM_OCMODE_PWM1;
	tim2OC_Init.OCPolarity = TIM_OCPOLARITY_HIGH;
	tim2OC_Init.Pulse = 0;


	// Config Channel 1 of TIM2
	if (HAL_TIM_PWM_ConfigChannel(&htim2, &tim2OC_Init, TIM_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}
}
/*
 * @fn					- SystemClock_Config
 * @brief				- This function modify the System Clock Frequency
 * @param				- SYSCLKFreq
 * @return				- void
 */
void SystemClock_Config(uint8_t SYSCLKFreq)
{
	RCC_OscInitTypeDef osc_init = {0};
	RCC_ClkInitTypeDef clk_init = {0};
	// Initialize osc_init
	/*
	 * PLL
	 * Src_Clock: HSI
	 */
	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSE;
	osc_init.HSIState = RCC_HSI_ON;
	osc_init.HSEState = RCC_HSE_ON;

	switch (SYSCLKFreq) {
		case SYSCLK_FREQ_50_MHz:
			osc_init.PLL.PLLM = 8;
			osc_init.PLL.PLLN = 100;
			osc_init.PLL.PLLP = 2;
			osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSE;
			osc_init.PLL.PLLState = RCC_PLL_ON;

			// Initialize osc_init
			if (HAL_RCC_OscConfig(&osc_init) != HAL_OK)
			{
				Error_Handler();
			}

			// Configure that Clock Init
			clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
			clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK  | \
									RCC_CLOCKTYPE_PCLK1  | RCC_CLOCKTYPE_PCLK2;
			clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
			clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
			clk_init.APB2CLKDivider = RCC_HCLK_DIV2;
			if (HAL_RCC_ClockConfig(&clk_init, 2) != HAL_OK)
			{
				Error_Handler();
			}
			break;

		case SYSCLK_FREQ_84_MHz:
			osc_init.PLL.PLLM = 8;
			osc_init.PLL.PLLN = 168;
			osc_init.PLL.PLLP = 2;
			osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSE;
			osc_init.PLL.PLLState = RCC_PLL_ON;
			// Initialize osc_init
			if (HAL_RCC_OscConfig(&osc_init) != HAL_OK)
			{
				Error_Handler();
			}

			// Configure that Clock Init
			clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
			clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK  | \
									RCC_CLOCKTYPE_PCLK1  | RCC_CLOCKTYPE_PCLK2;
			clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
			clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
			clk_init.APB2CLKDivider = RCC_HCLK_DIV2;
			if (HAL_RCC_ClockConfig(&clk_init, FLASH_ACR_LATENCY_2WS) != HAL_OK)
			{
				Error_Handler();
			}
			break;
		case SYSCLK_FREQ_120_MHz:
			osc_init.PLL.PLLM = 8;
			osc_init.PLL.PLLN = 240;
			osc_init.PLL.PLLP = 2;
			osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSE;
			osc_init.PLL.PLLState = RCC_PLL_ON;
			// Initialize osc_init
			if (HAL_RCC_OscConfig(&osc_init) != HAL_OK)
			{
				Error_Handler();
			}

			// Configure that Clock Init
			clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
			clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK  | \
									RCC_CLOCKTYPE_PCLK1  | RCC_CLOCKTYPE_PCLK2;
			clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
			clk_init.APB1CLKDivider = RCC_HCLK_DIV4;
			clk_init.APB2CLKDivider = RCC_HCLK_DIV2;
			if (HAL_RCC_ClockConfig(&clk_init, 3) != HAL_OK)
			{
				Error_Handler();
			}
			break;

			break;
		default:
			break;
		}


}
/*
 * @name: 		- UART2_Init
 */
void UART2_Init()
{
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.OverSampling = UART_OVERSAMPLING_8;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		Error_Handler();
	}


}

void LED_Init()
{
	GPIO_InitTypeDef led;
	__HAL_RCC_GPIOD_CLK_ENABLE();
	led.Mode = GPIO_MODE_OUTPUT_PP;
	led.Pin = GPIO_PIN_13;
	HAL_GPIO_Init(GPIOD, &led);
}

int main(void)
{
	uint32_t brightness = 0;
	char debug_msg[80] = "";
	HAL_Init();
	SystemClock_Config(SYSCLK_FREQ_50_MHz);

	// System clock check
	 printf("System clock: %lu Hz\r\n", HAL_RCC_GetSysClockFreq());
	// LED Init
//	LED_Init();
	// Verify if the orange led is turned on
//	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
	// Timer 2 Initialization
	TIM2_Init();
	// TIM4
	TIM4_Init();
	// GPIO Init for the DS18b20 on STM32 discovery board
	UART2_Init();
	// Start the Output Capture
	// PA0
//	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);

	// LED bright == 0
	while (1)
	{

		// Brighten the light for 1 second
		while (brightness < htim4.Init.Period)
		{
			__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, brightness);
			brightness+= 10;
			HAL_Delay(1);
		}

		// Dim the light for 1 second
		while (brightness > 0)
		{
			__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, brightness);
			brightness-=10;
			HAL_Delay(1);
		}
	}
	return 0;
}

// Delay with microsecond version
void delay_us(uint32_t microsec)
{
	uint32_t tick1 = __HAL_TIM_GET_COUNTER(&htim2);
	while ((( __HAL_TIM_GET_COUNTER(&htim2) - tick1 ) & 0xFFFFFFFFU) < microsec)
	{
		;
	}
}


void Error_Handler()
{
	while (1)
	{
		;
	}
}

