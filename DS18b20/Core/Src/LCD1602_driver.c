/*
 * LCD1602_driver.c
 *
 *  Created on: Nov 9, 2025
 *      Author: gphi1
 */

#include "main_app.h"

#define LCD1602_ADDR	0x4E
extern I2C_HandleTypeDef hi2c1;


/*
 * ******************************************************
 * API Prototype
 * ******************************************************
 */
void I2C1Init();
void LCD1602_Init();
void LCD1602_SendCMD(uint8_t cmd);
void LCD1602_SendData(uint8_t data);
void LCD1602_SendStr(uint8_t* str);
void LCD1602_SetCursor(uint8_t row, uint8_t col);

/*
 * @fn					- LCD1602_SetCursor
 * @brief				- This function set the position of the cursor
 * @param				- row
 * @param				- col
 * @return				-
 */
void LCD1602_SetCursor(uint8_t row, uint8_t col)
{
	uint8_t cmd = 0x80;
	if (row == 0)
	{
		cmd |= col;
	}
	else if (row == 1)
	{
		cmd |= 0x40;
		cmd |= col;
	}
	LCD1602_SendCMD(cmd);
}

/*
 * @fn					- LCD1602_SendStr
 * @brief				- API for sending string over to LCD 1602
 * @param				-
 * @return				-
 */
void LCD1602_SendStr(uint8_t* str)
{
	while (*str)
	{
		LCD1602_SendData(*str);
		str++;
	}
}

/*
 * @fn					- LCD1602_SendData
 * @brief				- API for sending 1 byte Data over to LCD 1602
 * @param				-
 * @return				-
 */
void LCD1602_SendData(uint8_t data)
{
	// Extract upper and lower part of the command
	uint8_t upper = data & 0xF0;
	uint8_t lower = (data << 4) & 0xF0;
	// Store them in an array
	uint8_t arr[4] = {};
	// Changing the Enable bit of the upper 4 bits data
	arr[0] = upper | 0x0D; // E = 1; RS = 1; RW = 0 => P3P2P1P0 = 1101
	arr[1] = upper | 0x09; // E = 0; RS = 1; RW = 0 => P(3->0) = 1001
	// Changing the Enable bit of the lower 4 bits data
	arr[2] = lower | 0x0D; // E = 1; RS = 1; RW = 0 => P(3->0) = 1101
	arr[3] = lower | 0x09; // E = 0; RS = 1; RW = 0 => P(3->0) = 1001

	HAL_I2C_Master_Transmit(&hi2c1, LCD1602_ADDR, arr, 4, HAL_MAX_DELAY);
	HAL_Delay(1);


}
/*
 * @fn					- LCD1602_SendCMD
 * @brief				- API for sending Command over to LCD 1602
 * @param				-
 * @return				-
 */
void LCD1602_SendCMD(uint8_t cmd)
{
	// Extract upper and lower part of the command
	uint8_t upper = cmd & 0xF0;
	uint8_t lower = (cmd << 4) & 0xF0;
	// Store them in an array
	uint8_t data[4] = {};
	// Changing the Enable bit of the upper 4 bits data
	data[0] = upper | 0x0C; // E = 1; RS,RW = 0
	data[1] = upper | 0x08; // E = 0; RS,RW = 0
	// Changing the Enable bit of the lower 4 bits data
	data[2] = lower | 0x0C; // E = 1; RS,RW = 0
	data[3] = lower | 0x08; // E = 0; RS,RW = 0

	HAL_I2C_Master_Transmit(&hi2c1, LCD1602_ADDR, data, 4, HAL_MAX_DELAY);
	HAL_Delay(1);
}



/*
 * @fn					- LCD1602_GPIOInit
 * @brief				- Initialize GPIO Pin for I2C expander or LCD1602
 * @param				-
 * @return				-
 */
void I2C1Init()
{
	// I2C Initialization for LCD1602
	// Enable the clock for I2C1
	__HAL_RCC_I2C1_CLK_ENABLE();
	/*
	 * I2C Specs:
	 * - I2C1
	 * - 7 bit addr mode
	 * - 100kHz
	 */
	hi2c1.Instance = I2C1;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.ClockSpeed = 100000;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Mode = HAL_I2C_MODE_MASTER;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK)
	{
		Error_Handler();
	}

}

/*
 * @fn					- LCD1602_Init
 * @brief				- This function complete the Initialization sequence of LCD1602
 * @param				-
 * @return				-
 */
void LCD1602_Init()
{
	// Wait > 15ms
	HAL_Delay(40);
	// Send 0x30
	LCD1602_SendCMD(0x30);
	// Wait > 4.1ms
	HAL_Delay(5);
	// Send 0x30
	LCD1602_SendCMD(0x30);
	// Wait time > 100us
	delay_us(200);
	// Send 0x30
	LCD1602_SendCMD(0x30);
	// Set interface to be 4 bits length
	HAL_Delay(1);
	LCD1602_SendCMD(0x20);

	/*
	 * Display settings
	 */
	// Function Set. DL = 0(4bit mode); N = 1(2 display line); F = 0(5x7 style)
	HAL_Delay(1);
	LCD1602_SendCMD(0x28);
	// Display off
	HAL_Delay(1);
	LCD1602_SendCMD(0x08); // D = 1; C,B = 0;
	// Screen clear
	HAL_Delay(1);
	LCD1602_SendCMD(0x01);
	// Entry mode set => I/D = 1(Increment); S = 0(No shift)
	HAL_Delay(3);
	LCD1602_SendCMD(0x06);
	// Display on
	HAL_Delay(1);
	LCD1602_SendCMD(0x0C); // D = 1; C,B = 0;
}
