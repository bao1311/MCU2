/*
 * main_app.c
 *
 *  Created on: Oct 31, 2025
 *      Author: gphi1
 */
#include "main_app.h"
#include <stdio.h>
#include <string.h>

// ---------- Clock Frequency Macros --------------
#define SYSCLK_FREQ_50_MHz		0
#define SYSCLK_FREQ_84_MHz		1
#define SYSCLK_FREQ_120_MHz		2

/*
 * Btn Macros
 */
#define BTN_DEBOUNCE_TIME		25

/*
 * Global variable
 */
UART_HandleTypeDef huart2;
TIM_HandleTypeDef htim2;
I2C_HandleTypeDef hi2c1;
GPIO_InitTypeDef btnGpio;
/*
 * API prototypes
 */
void TIM2_Init();
void SystemClock_Config(uint8_t SYSCLKFreq);
void ShowTemp();
void delay_us(uint32_t microsec);
void UART2_Init();
void Btn_Init();
int main(void);
/*
 * Button state
 */
volatile uint8_t button_pressed = 0;
volatile uint32_t button_pressed_ts = 0;

/*
 * Enum of available state for the app
 */
typedef enum
{
	INIT = 0,
	WELCOME,
	BTN_DEBOUNCE,
	SHOW_TEMP
} app_state_t;
/*
 * Application state
 */
static volatile app_state_t cur_state = WELCOME;
static uint32_t last_temp_update_ts = 0;

/*
 * @fn					- ShowTemp
 * @brief				- Display temperature on LCD
 * @param				-
 * @return				-
 */
void ShowTemp()
{
		float tempC = DS18B20_ReadTempC();
		float tempF = DS18B20_ConvertToF(tempC);

		//		sprintf(debug_msg, "Current temperature is: %.2f Celsius\r\n", temp);
//		HAL_UART_Transmit(&huart2, debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
		char printedMsg[16] = "";
		sprintf(printedMsg, "%.2f%cF(%.2f%cC)", tempF, DEGREE, tempC, DEGREE);
		// Clear screen
		LCD1602_ClearScreen();
		// LCD set cursor in the first row
		LCD1602_SetCursor(0, 2);
		// LCD send data first row ask to click on the button of stm board
		LCD1602_SendStr((uint8_t*)"Current temp");
		 // LCD set cursor in the second row
		LCD1602_SetCursor(1, 0);
		LCD1602_SendStr((uint8_t*)printedMsg);
}

static volatile uint8_t state_entry = 1;
void change_state(app_state_t s)
{
	cur_state = s;
	state_entry = 1;
}
/*
 * @fn					- fsm
 * @brief				-
 * @param				-
 * @return				-
 */
void fsm()
{
	uint32_t now = HAL_GetTick();
	switch (cur_state) {
		case INIT:
			if (state_entry)
			{
				change_state(WELCOME);
				state_entry = 1;
			}
			break;
		case WELCOME:
			if (state_entry)
			{
				Welcome();
				state_entry = 0;
			}
			if (button_pressed)
			{
				// Move to DEBOUNCE state
				change_state(BTN_DEBOUNCE);
			}
			else
			{
				__WFI(); // Sleep until catch an interrupt
			}
			break;
		case BTN_DEBOUNCE:
			// Wait until debounce already pass
			if (now - button_pressed_ts >= BTN_DEBOUNCE_TIME)
			{
				// Check if Button is pressed (PA0 == 1)?
				if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET)
				{
					// Move to SHOW_TEMP event
					change_state(SHOW_TEMP);
					// Reset the button_press state
					button_pressed = 0;
				}
			}
			else
			{
				// False Debounce signal
				__WFI();
			}
			break;
		case SHOW_TEMP:
			// Draw the temperature once
			if (state_entry)
			{
				ShowTemp();
				last_temp_update_ts = HAL_GetTick();
				state_entry = 0;
			}
			if (HAL_GetTick() - last_temp_update_ts >= 1000)
			{

				ShowTemp();
				last_temp_update_ts = HAL_GetTick();
			}
			// Check for another Button Press
			if (button_pressed == 1)
			{
				// Get back to WELCOME State
				change_state(WELCOME);
				// Reset the button pressed flag
				button_pressed = 0;
			}
			else
			{
				// Wait for interrupt
				__WFI();
			}
			break;
		default:
			break;
	}

}

/*
 * @fn					- Welcome
 * @brief				- Welcome LCD screen
 * @param				-
 * @return				-
 */
void Welcome()
{
	// Clear screen
	LCD1602_ClearScreen();
	// LCD set cursor in the first row
	LCD1602_SetCursor(0, 0);
	// LCD send data first row ask to click on the button of stm board
	LCD1602_SendStr((uint8_t*)"Hello, press for");
	// LCD set cursor in the second row
	LCD1602_SetCursor(1, 0);
	// LCD send string to the row
	LCD1602_SendStr((uint8_t*)"the temperature");

}
/*
 * @fn					- go_to_state
 * @brief				- State entry action
 * @param				-
 * @return				-
 */
void go_to_state(app_state_t app_state)
{
	app_state_t next_state = app_state;
	switch (app_state) {
		case INIT:
			// Transition to WELCOME screen
			go_to_state(WELCOME);
			break;
		case WELCOME:
			Welcome();
			next_state = WELCOME;
			break;
		case BTN_DEBOUNCE:
			break;
		case SHOW_TEMP:
			ShowTemp();
			next_state = SHOW_TEMP;
			break;
		default:
			break;
	}
	cur_state = next_state;
}
//-----------------------------------------

/*
 * @fn					- Btn_Init
 * @brief				- Initialize button
 * @param				-
 * @return				-
 */
void Btn_Init()
{
	// Enable clock of GPIOA
	__HAL_RCC_GPIOA_CLK_ENABLE();
	// Initialize button GPIO at PA0
	memset(&btnGpio,0,sizeof(btnGpio));
	btnGpio.Mode = GPIO_MODE_IT_RISING;
	btnGpio.Pin = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOA, &btnGpio);
	// Set up the interrupt for button
	HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

/*
 * @fn					- TIM2_Init
 * @brief				- This function initializes Timer 2
 * @param				-
 * @return				- void
 */
void TIM2_Init()
{
	/*
	 * APB1 frequency: 42MHz
	 * Timer 2 configuration detail:
	 * - Prescaler: 42 - 1
	 * - Period: 0xFFFFFFFFU
	 * - Countermode: UP
	 */
	uint32_t pclk1 = HAL_RCC_GetPCLK1Freq();
	uint32_t tim_clk = ((RCC->CFGR & RCC_CFGR_PPRE1) != RCC_CFGR_PPRE1_DIV1) ? pclk1 * 2U : pclk1;
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = (tim_clk / 1000000U) - 1UL;
	htim2.Init.Period = 0xFFFFFFFFU;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;

	// Init the TIM 2
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_TIM_Base_Start(&htim2) != HAL_OK)
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
	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	osc_init.HSIState = RCC_HSI_ON;

	switch (SYSCLKFreq) {
		case SYSCLK_FREQ_50_MHz:
			osc_init.PLL.PLLM = 8;
			osc_init.PLL.PLLN = 50;
			osc_init.PLL.PLLP = 2;
			osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSI;
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
			osc_init.PLL.PLLN = 84;
			osc_init.PLL.PLLP = 2;
			osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSI;
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
			osc_init.PLL.PLLN = 120;
			osc_init.PLL.PLLP = 2;
			osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSI;
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
int main(void)
{
	char debug_msg[80] = "";
	HAL_Init();
	SystemClock_Config(SYSCLK_FREQ_84_MHz);
	// Timer 2 Initialization
	TIM2_Init();
	// GPIO Init for the DS18b20 on STM32 discovery board
	UART2_Init();
	// Test the frequency of PCLK1
	sprintf(debug_msg, "Clock frequency of PCLK1 is: %lu\r\n", HAL_RCC_GetPCLK1Freq());
	HAL_UART_Transmit(&huart2, debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
	DS18B20_Init_OUTPUT();
	// Button Init
	Btn_Init();
	// Check if timer 2 really works at 42MHz
	uint32_t t1 = __HAL_TIM_GET_COUNTER(&htim2);
	HAL_Delay(1000);
	uint32_t t2 = __HAL_TIM_GET_COUNTER(&htim2);
	uint32_t ticks = (t2 - t1) & 0xFFFFFFFFU;
	sprintf(debug_msg, "TIM2 ticks in 1 sec = %lu\r\n", (unsigned long)ticks);
	HAL_UART_Transmit(&huart2, debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
	// Send out Hello world
	strcpy(debug_msg, "Hello, world\r\n");
	HAL_UART_Transmit(&huart2, debug_msg, strlen(debug_msg), HAL_MAX_DELAY);

	// I2C1 Init
	I2C1Init();
	// LCD Init phase
	LCD1602_Init();
	// Assign INIT state
	cur_state = INIT;

	while (1)
	{
		fsm();
//		static uint8_t welcome_display = 0;
//		if (!welcome_display)
//		{
//			welcome_display = 1;
//		}
//		// Check if button is pressed
//		if (button_pressed)
//		{
//			// Check for debouncing
//			// Skip button debouncing logic  && HAL_GetTick() - button_pressed_ts >= 10U
//			if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET)
//			{
//
//				float tempC = DS18B20_ReadTempC();
//				float tempF = DS18B20_ConvertToF(tempC);
//
//		//		sprintf(debug_msg, "Current temperature is: %.2f Celsius\r\n", temp);
//		//		HAL_UART_Transmit(&huart2, debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
//				char printedMsg[16] = "";
//				sprintf(printedMsg, "%.2f%cF(%.2f%cC)", tempF, DEGREE, tempC, DEGREE);
//				// Clear screen
//				LCD1602_ClearScreen();
//				// LCD set cursor in the first row
//				LCD1602_SetCursor(0, 2);
//				// LCD send data first row ask to click on the button of stm board
//				LCD1602_SendStr((uint8_t*)"Current temp");
//		 		// LCD set cursor in the second row
//				LCD1602_SetCursor(1, 0);
//				LCD1602_SendStr((uint8_t*)printedMsg);
//				HAL_Delay(3000);
//
//			}
//			button_pressed = 0;
//			welcome_display = 0;
//		}
//		else
//		{
//			__WFI();
//		}
//		// wait for the button to be pressed
//		while (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0));
//		float tempC = DS18B20_ReadTempC();
//		float tempF = DS18B20_ConvertToF(tempC);
//
////		sprintf(debug_msg, "Current temperature is: %.2f Celsius\r\n", temp);
////		HAL_UART_Transmit(&huart2, debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
//		char printedMsg[16] = "";
//		sprintf(printedMsg, "%.2f%cF(%.2f%cC)", tempF, DEGREE, tempC, DEGREE);
//		// Clear screen
//		LCD1602_ClearScreen();
//		// LCD set cursor in the first row
//		LCD1602_SetCursor(0, 2);
//		// LCD send data first row ask to click on the button of stm board
//		LCD1602_SendStr((uint8_t*)"Current temp");
// 		// LCD set cursor in the second row
//		LCD1602_SetCursor(1, 0);
//		LCD1602_SendStr((uint8_t*)printedMsg);
//		delay_us(1000000);
	}
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

