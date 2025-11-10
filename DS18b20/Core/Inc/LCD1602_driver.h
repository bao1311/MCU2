/*
 * LCD1602_driver.h
 *
 *  Created on: Nov 9, 2025
 *      Author: gphi1
 */

#ifndef INC_LCD1602_DRIVER_H_
#define INC_LCD1602_DRIVER_H_

/*
 * Macros for LCD1602 Module
 */
#define DEGREE 0xDF


// Instruction Set Macros
#define LCD_SCREEN_CLEAR 0x01
// Other Macros

/*
 * API for LCD1602 Module Driver
 */
void I2C1Init();
void LCD1602_Init();
void LCD1602_SendCMD(uint8_t cmd);
void LCD1602_SendData(uint8_t data);
void LCD1602_SendStr(uint8_t* str);
void LCD1602_ClearScreen();




#endif /* INC_LCD1602_DRIVER_H_ */
