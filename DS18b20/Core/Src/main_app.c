/*
 * main_app.c
 *
 *  Created on: Oct 31, 2025
 *      Author: gphi1
 */
#include "main_app.h"
#include <stdio.h>
void GPIO_Init();
void DS18B20_ReadTemp();
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

