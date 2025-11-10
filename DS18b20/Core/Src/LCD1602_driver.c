/*
 * LCD1602_driver.c
 *
 *  Created on: Nov 9, 2025
 *      Author: gphi1
 */

#include "main_app.h"

extern I2C_HandleTypeDef hi2c1;


void I2C1Init();
void LCD1602_Init();
void LCD1602_SendCMD();
void LCD1602_SendData();


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
