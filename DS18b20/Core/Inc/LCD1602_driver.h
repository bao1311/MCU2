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
// Instruction Set Macros
// Other Macros
#define LCD1602_ADDR			0x20

/*
 * API for LCD1602 Module Driver
 */
void I2C1Init();
void LCD1602_Init();
void LCD1602_SendCMD();
void LCD1602_SendData();



#endif /* INC_LCD1602_DRIVER_H_ */
