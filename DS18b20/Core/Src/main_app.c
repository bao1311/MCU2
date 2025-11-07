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
/*
 * API prototypes
 */
void TIM2_Init();
void SystemClock_Config(uint8_t SYSCLKFreq);
void GPIO_Init();
uint8_t DS18B20_ReadTemp();
void delay_us(uint32_t microsec);
void OneWire_WriteBit(uint8_t bit);
void OneWire_WriteByte(uint8_t byte);
void UART2_Init();
uint8_t OneWire_ReadBit();
uint8_t OneWire_ReadByte();
//-----------------------------------------

/*
 * @fn					- TIM2_Init
 * @brief				- This function initializes Timer 2
 * @param				-
 * @return				- void
 */
void TIM2_Init()
{
	htim2.

}

/*
 * @fn					- SystemClock_Config
 * @brief				- This function modify the System Clock Frequency
 * @param				- SYSCLKFreq
 * @return				- void
 */
void SystemClock_Config(uint8_t SYSCLKFreq)
{
	RCC_OscInitTypeDef osc_init;
	RCC_ClkInitTypeDef clk_init;
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
			clk_init.APB1CLKDivider = RCC_SYSCLK_DIV2;
			clk_init.APB2CLKDivider = RCC_SYSCLK_DIV2;
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
			clk_init.APB1CLKDivider = RCC_SYSCLK_DIV2;
			clk_init.APB2CLKDivider = RCC_SYSCLK_DIV2;
			if (HAL_RCC_ClockConfig(&clk_init, 2) != HAL_OK)
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
			clk_init.APB1CLKDivider = RCC_SYSCLK_DIV4;
			clk_init.APB2CLKDivider = RCC_SYSCLK_DIV2;
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
	GPIO_Init();
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
		delay_us(1000);
	}


}
/*
 * Read Bit/Byte API
 */
uint8_t OneWire_ReadBit()
{
	uint8_t bit = 0;
	// Recovery time (1us)
	delay_us(3);
	// Master device pulling the 1-Wire bus low
	HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);
	// let the master device pulling low for 1us
	delay_us(3);
	// Master device release the bus
	HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_SET);
	// Wait a little before sampling
	delay_us(10); // Wait ~10-15us
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
		uint8_t bit = OneWire_ReadBit();
		byte = (byte << 1) | bit;
	}
	return byte;

}

// One wire write byte protocol
void OneWire_WriteByte(uint8_t byte)
{
	// Transfer bit by bit through the wire
	for (uint8_t i = 0; i < 8; ++i)
	{
		// Extract the innermost bit
		OneWire_WriteBit((byte >> i) & 0x01);
	}
}
// One wire write bit protocol
void OneWire_WriteBit(uint8_t bit)
{
	// Write time slots are initiated by master pulling
	// the line low
	HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);
	delay_us(1);
	if (bit)
	{
		// Generate Write 1 Time slot
		HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_SET);
		delay_us(15);


	}
	else
	{
		// Generate Write 0 Time slot
		HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);
		delay_us(60);
	}
}
// Delay with microsecond version
void delay_us(uint32_t microsec)
{
	for (uint32_t i = 0; i < microsec * 10; ++i);
}

// OneWire_Initialization for the one-wire communication
void OneWire_Initialization()
{
	// Status of the presence of DS18B20
	uint8_t presence = 0;
	// Sending a reset pulse by pulling the line low
	// from master
	HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);
	// Wait for 480 microsecond
	delay_us(480);
	// Master release the bus
	HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_SET);
	// Wait 15-60 microsec
	delay_us(60);
	// Extract the presence pulse
	presence = HAL_GPIO_ReadPin(DS18B20_PORT, DS18B20_PIN);
	// Check whether the DS18B20 module exist on the line or not
	if (presence == 0)
	{
		Error_Handler();
	}


	return presence;
}

void Error_Handler()
{
	while (1)
	{
		;
	}
}

uint8_t DS18B20_ReadTemp()
{
	uint32_t temp = 0;
	// Initialization
	OneWire_Initialization();
	// Sending ROM command
	OneWire_WriteByte(Skip_ROM);
	// Sending Function Commands
	OneWire_WriteByte(Convert_T);
	// Extract the temperature conversion
	temp = OneWire_ReadByte();
	// Data is transferred in LSB mode
	return temp;


}

void GPIO_Init()
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

