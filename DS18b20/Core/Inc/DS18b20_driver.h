/*
 * DS18b20_driver.h
 *
 *  Created on: Nov 10, 2025
 *      Author: gphi1
 */

#ifndef INC_DS18B20_DRIVER_H_
#define INC_DS18B20_DRIVER_H_

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

// ---------- DS18b20 ROM Commands Macros ---------------
#define Search_ROM		0xF0
#define Read_ROM		0x33
#define Match_ROM		0x55
#define Skip_ROM		0xCC
#define Alarm_Search	0xEC
/*
 * DS18B20 PORT & PIN Specification
 */
#define DS18B20_PORT	GPIOA
#define DS18B20_PIN		GPIO_PIN_1

/*
 * DS18B20 API Prototype
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


#endif /* INC_DS18B20_DRIVER_H_ */
