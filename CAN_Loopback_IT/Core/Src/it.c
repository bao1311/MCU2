/*
 * it.c
 *
 *  Created on: Nov 19, 2025
 *      Author: gphi1
 */

#include "main.h"
#include <stdio.h>
extern TIM_HandleTypeDef htim2;
extern CAN_HandleTypeDef hcan1;

int _write(int file, char *ptr, int len)
{
    for (int i = 0; i < len; i++)
        ITM_SendChar(*ptr++);
    return len;
}
void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

void TIM2_IRQHandler()
{
	HAL_TIM_IRQHandler(&htim2);
}

void CAN1_TX_IRQHandler()
{
	HAL_CAN_IRQHandler(&hcan1);

}

void CAN1_RX0_IRQHandler()
{

	HAL_CAN_IRQHandler(&hcan1);
}

void CAN1_RX1_IRQHandler()
{

	HAL_CAN_IRQHandler(&hcan1);
}

void CAN1_SCE_IRQHandler()
{

	HAL_CAN_IRQHandler(&hcan1);
}
