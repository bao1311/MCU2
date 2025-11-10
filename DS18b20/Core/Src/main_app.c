/*
 * main_app.c
 *
 *  Created on: Oct 31, 2025
 *      Author: gphi1
 */
#include "main_app.h"
#include <stdio.h>
#include <string.h>
/*
 * DS18B20 PORT & PIN Specification
 */
#define DS18B20_PORT	GPIOA
#define DS18B20_PIN		GPIO_PIN_1
/*
 * DS18B20 ROM and Function Commands Macros
 */
// ---------- Function Commands Macros ---------------
#define Convert_T			0x44
#define Write_Scratchpad	0x4E
#define Read_Scratchpad		0xBE
#define Copy_Scratchpad		0x48
#define Recall_E_2			0xB8
#define Read_Power_Supply	0xB4

// ---------- ROM Commands Macros ---------------
#define Search_ROM		0xF0
#define Read_ROM		0x33
#define Match_ROM		0x55
#define Skip_ROM		0xCC
#define Alarm_Search	0xEC

// ---------- Clock Frequency Macros --------------
#define SYSCLK_FREQ_50_MHz		0
#define SYSCLK_FREQ_84_MHz		1
#define SYSCLK_FREQ_120_MHz		2

/*
 * Global variable
 */
UART_HandleTypeDef huart2;
TIM_HandleTypeDef htim2;
I2C_HandleTypeDef hi2c1;
/*
 * API prototypes
 */
void TIM2_Init();
void SystemClock_Config(uint8_t SYSCLKFreq);
void DS18B20_Init_OUTPUT();
void DS18B20_Init_INPUT();
float DS18B20_ReadTemp();
void delay_us(uint32_t microsec);
void OneWire_WriteBit(uint8_t bit);
void OneWire_WriteByte(uint8_t byte);
void UART2_Init();
uint8_t OneWire_ReadBit();
uint8_t OneWire_ReadByte();
int main(void);
//-----------------------------------------

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
	// GPIO Init for the DS18b20 on STM32 discovery board
	UART2_Init();
	// Test the frequency of PCLK1
	sprintf(debug_msg, "Clock frequency of PCLK1 is: %lu\r\n", HAL_RCC_GetPCLK1Freq());
	HAL_UART_Transmit(&huart2, debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
	DS18B20_Init_OUTPUT();
	// Timer 2 Initialization
	TIM2_Init();
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
	while (1)
	{
		float temp = DS18B20_ReadTemp();
		sprintf(debug_msg, "Current temperature is: %.2f Celsius\r\n", temp);
		HAL_UART_Transmit(&huart2, debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
		HAL_Delay(3000);
//		delay_us(1000000);
	}
}
/*
 * Read Bit/Byte API
 */
uint8_t OneWire_ReadBit()
{
	uint8_t bit = 0;
	// Change bus control to master
	DS18B20_Init_OUTPUT();
	// Master device pulling the 1-Wire bus low
	HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);
	// let the master device pulling low for 1us
	delay_us(2);
	// Master device release the bus
	DS18B20_Init_INPUT();
	// Wait a bit for DS18B20 to send data
	delay_us(5);
	// Read the bit
	bit = HAL_GPIO_ReadPin(DS18B20_PORT, DS18B20_PIN);
	delay_us(60); // Ensure the total of the duration of read time slot (60us);
	return bit;

}
uint8_t OneWire_ReadByte()
{
	uint8_t byte = 0;
	for (uint8_t i = 0; i < 8; ++i)
	{
		// recovery time (1ms)
		delay_us(1);
		uint8_t bit = OneWire_ReadBit();
		byte = (bit << i) | byte;
	}
	return byte;

}

// One wire write byte protocol
void OneWire_WriteByte(uint8_t byte)
{
	// Transfer bit by bit through the wire
	for (uint8_t i = 0; i < 8; ++i)
	{
		// delay 1 microsec for recovery time
		delay_us(1);
		// Extract the innermost bit
		OneWire_WriteBit((byte >> i) & 0x01);
	}
}
// One wire write bit protocol
void OneWire_WriteBit(uint8_t bit)
{
	// Control the bus
	DS18B20_Init_OUTPUT();
	// Write time slots are initiated by master pulling
	// the line low
	HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);
	if (bit)
	{
		// Delay for 1 microsec
		delay_us(1);
		// Release the 1 Wire bus
		DS18B20_Init_INPUT();
		// Delay for 45 micro sec for the pull up
		// pull the bus high
		delay_us(45);
	}
	else
	{
		// Continue holding the bus low for 60 microsec
		delay_us(60);
		// Release the bus
		DS18B20_Init_INPUT();
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

// OneWire_Initialization for the one-wire communication
uint8_t OneWire_Initialization()
{
	// Status of the presence of DS18B20
	uint8_t presence = 0;
	// Sending a reset pulse by pulling the line low
	// from master
	DS18B20_Init_OUTPUT();
	HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);
	// Wait for 480 microsecond
	delay_us(480);
	// Master release the bus
	DS18B20_Init_INPUT();
	// delay for 80 microsec
	delay_us(80);
	// Extract the presence pulse
	presence = HAL_GPIO_ReadPin(DS18B20_PORT, DS18B20_PIN);
	// Check whether the DS18B20 module exist on the line or not
	if (presence != 0)
	{
		Error_Handler();
	}

	delay_us(480);
	return presence;
}

void Error_Handler()
{
	while (1)
	{
		;
	}
}

float DS18B20_ReadTemp()
{
	float temp = 0;
	// Master Reset Pulse
	OneWire_Initialization();
	// Sending ROM command
	OneWire_WriteByte(Skip_ROM);
	// Sending Convert T
	OneWire_WriteByte(Convert_T);
	// Delay 750 millisec
//	HAL_Delay(750);
	// Master Reset Pulse
	OneWire_Initialization();
	// Sending ROM command
	OneWire_WriteByte(Skip_ROM);
	// Sending Function Commands
	OneWire_WriteByte(Read_Scratchpad);
	// Extract the temperature conversion
	// temperature is 2 bytes
	uint8_t lsb = OneWire_ReadByte();
	uint8_t msb = OneWire_ReadByte();

	int16_t cur = (msb << 8) | lsb;
	temp = (float)cur;
	// Convert to Celsius
	temp = cur * 0.0625f;
	// Data is transferred in LSB mode
	return temp;


}

void DS18B20_Init_INPUT()
{
	// Use PA1 for the one wire protocol
	// Config the GPIOA Clock
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitTypeDef gpio;

	/*
	 * GPIO Info:
	 * Port: A
	 * Pin: 1
	 */
	gpio.Mode = GPIO_MODE_INPUT;
	gpio.Pin = GPIO_PIN_1;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_LOW;

	// Init
	HAL_GPIO_Init(GPIOA, &gpio);
}

/*
 * @fn					- GPIO_Init_OUTPUT
 * @brief				- This function configure DS18B20
 * 						Pin as OUTPUT Open Drain
 * @param				-
 * @return				-
 */
void DS18B20_Init_OUTPUT()
{
	// Use PA1 for the one wire protocol
	// Config the GPIOA Clock
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitTypeDef gpio;

	/*
	 * GPIO Info:
	 * Port: A
	 * Pin: 1
	 */
	gpio.Mode = GPIO_MODE_OUTPUT_OD;
	gpio.Pin = GPIO_PIN_1;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_LOW;

	// Init
	HAL_GPIO_Init(GPIOA, &gpio);
}

