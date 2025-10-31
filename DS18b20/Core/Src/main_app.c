/*
 * main_app.c
 *
 *  Created on: Oct 31, 2025
 *      Author: gphi1
 */
#include "main_app.h"
#include <stdio.h>
#define DS18B20_PORT	GPIOA
#define DS18B20_PIN		GPIO_PIN_1
void GPIO_Init();
void DS18B20_ReadTemp();
void delay_us(int microsec);
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
	// Master release the bus
	// Wait 15-60 microsec


}

void DS18B20_ReadTemp()
{
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

