/*
 * msp.c
 *
 *  Created on: Oct 2, 2025
 *      Author: gphi1
 */

#include "stm32f4xx_hal.h"
void HAL_MspInit(void)
{
	// This file will do the low level processor specific inits

	// 1. Set up the priority grouping of arm cortex mx processor
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	// 2. Enable the required system exceptions of the arm cortex mx processor

	// 3. Configure the pri:ority for the system exceptions
}
