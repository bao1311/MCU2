/*
 * main_app.c
 *
 *  Created on: Oct 31, 2025
 *      Author: gphi1
 */
#include "main_app.h"
#include <stdio.h>
/*
 * DS18B20 PORT & PIN Specification
 */
#define DS18B20_PORT	GPIOA
#define DS18B20_PIN		GPIO_PIN_1
/*
 * DS18B20 ROM and Function Commands Macros
 */
// ---------- Function Commands ---------------
#define Convert_T		0x44

// ---------- ROM Commands ---------------
#define Skip_ROM		0xCC

void GPIO_Init();
void DS18B20_ReadTemp();
void delay_us(int microsec);
void OneWire_WriteBit(uint8_t bit);
int main(void)
{
	HAL_Init();
	SystemClock_Config();
	// GPIO Init for the DS18b20 on STM32 discovery board
	GPIO_Init();

	while (1)
	{
		float temp = DS18B20_ReadTemp();
		printf("Current temperature is: %.2f Celsius\r\n",temp);
		delay();
	}


}

// One wire write byte protocol
void OneWire_WriteByte(uint8_t byte)
{
	// Transfer bit by bit through the wire
	while (byte)
	{
		// Extract the innermost bit
		uint8_t cur = byte & 1;
		OneWire_WriteBit(cur);
		byte >>= 1;
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
	if (presence != 0)
	{
		Err_Handler();
	}


	return presence;
}

void Err_Handler()
{
	while (1)
	{
		;
	}
}

void DS18B20_ReadTemp()
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

	// Init
	HAL_GPIO_Init(GPIOA, &gpio);
}

