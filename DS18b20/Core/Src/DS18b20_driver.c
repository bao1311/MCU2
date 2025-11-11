/*
 * DS18b20_driver.c
 *
 *  Created on: Nov 10, 2025
 *      Author: gphi1
 */


#include "main_app.h"


/*
 * *****************************
 * API Prototype
 * *****************************
 */
uint8_t OneWire_Initialization();
void DS18B20_Init_OUTPUT();
void DS18B20_Init_INPUT();
float DS18B20_ReadTempC();
float DS18B20_ConvertToF(float C);
void OneWire_WriteBit(uint8_t bit);
void OneWire_WriteByte(uint8_t byte);
uint8_t OneWire_ReadByte();
uint8_t OneWire_ReadBit();

/*
 * *****************************
 * API Implementation
 * *****************************
 */
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

float DS18B20_ConvertToF(float C)
{
	return ((C * 9.0f) / 5.0f) + 32;

}
float DS18B20_ReadTempC()
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

