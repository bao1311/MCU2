#include "main.h"
#include <string.h>

/*
 * ********* Pulse value for each channel *********
 */
uint32_t ch1_pulse = 500;
uint32_t ch2_pulse = 12500;
uint32_t ch3_pulse = 6250;
uint32_t ch4_pulse = 3125;

/*
 * ********* Clock frequency macros ***********
 */
#define SYSCLK_FREQ_50_MHz		0
#define SYSCLK_FREQ_84_MHz		1
#define SYSCLK_FREQ_120_MHz		2
/*
 * ********* Global handle **********
 */
UART_HandleTypeDef huart2;
TIM_HandleTypeDef htim2;
CAN_HandleTypeDef hcan1;
/*
 * ********* API Prototype **********
 */
void UART2_Init();
void SystemClock_Config(uint8_t SYSCLKFreq);
void delay_us(uint32_t microsec);
void Error_Handler();
void TIM2_Init();
void Btn_Init();
void CAN1_Init();
void CAN1_Tx();
void CAN1_Rx();
void CAN_filter();
/*
 * ********** Function Implementation **********
 */

/*
 * @fn					- CAN_filter
 * @brief				- This function filter CAN message
 * @param				-
 * @return				-
 */
void CAN_filter()
{
	CAN_FilterTypeDef can_filter;
	// We are using accept all mode in this filter
	can_filter.FilterIdHigh = 0x0000;
	can_filter.FilterIdLow = 0x0000;
	can_filter.FilterMaskIdLow = 0x0000;
	can_filter.FilterMaskIdHigh = 0x0000;
	can_filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	can_filter.FilterBank = 0;
	can_filter.FilterMode = CAN_FILTERMODE_IDMASK;
	can_filter.FilterScale = CAN_FILTERSCALE_32BIT;
	can_filter.FilterActivation = CAN_FILTER_ENABLE;

	if (HAL_CAN_ConfigFilter(&hcan1, &can_filter) != HAL_OK)
	{
		Error_Handler();
	}

}
/*
 * @fn					- CAN1_Rx
 * @brief				- Receive the message from the mailbox
 * @param				-
 * @return				-
 */
void CAN1_Rx()
{
	char msg[50];
	uint8_t arr[5];
	CAN_RxHeaderTypeDef can_rx;
	// Initialize the can_rx data structure
	can_rx.StdId = 0x65;
	can_rx.DLC = 5;
	can_rx.RTR = CAN_RTR_DATA;
	can_rx.IDE = CAN_ID_STD;
	// Wait until the mailbox got filled with message
	while (!HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0));
	// Receive the message
	if (HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &can_rx, arr) != HAL_OK)
	{
		Error_Handler();
	}
	sprintf(msg,"Message Received\n");
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}
/*
 * @fn					- CAN1_Tx
 * @brief				- Send Data through CAN1
 * @param				-
 * @return				-
 */
void CAN1_Tx()
{
	CAN_TxHeaderTypeDef can_tx;
	char msg[50];
	uint8_t arr[5] = {'H','E','L','L','O'};
	uint32_t MailBox;
	can_tx.DLC = 5;
	can_tx.StdId = 0x65;
	can_tx.RTR = CAN_RTR_DATA;
	can_tx.IDE = CAN_ID_STD;
	if (HAL_CAN_AddTxMessage(&hcan1, &can_tx, arr, &MailBox) != HAL_OK)
	{
		Error_Handler();
	}
	// Wait until the TxMessage got Transmitted
	while (HAL_CAN_IsTxMessagePending(&hcan1, MailBox));
	sprintf(msg,"Message Transmitted\n");
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}
/*
 * @fn					- CAN1_Init
 * @brief				- Implementation of CAN1 Init code
 * @param				-
 * @return				-
 */
void CAN1_Init()
{
	/*
	 * CAN1 Specs:
	 * - Bit Rate: 500bps
	 * - Prescaler: 5
	 * - No of time quanta: 10
	 * - Seg 1: 8
	 * - Seg 2: 1
	 * -
	 */
	hcan1.Instance = CAN1;

	hcan1.Init.AutoBusOff = DISABLE;
	hcan1.Init.AutoRetransmission = ENABLE;
	hcan1.Init.AutoWakeUp = DISABLE;
	hcan1.Init.Mode = CAN_MODE_LOOPBACK;
	hcan1.Init.Prescaler = 5;
	hcan1.Init.ReceiveFifoLocked = DISABLE;
	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
	hcan1.Init.TimeSeg1 = CAN_BS1_8TQ;
	hcan1.Init.TimeSeg2 = CAN_BS2_1TQ;
	hcan1.Init.TimeTriggeredMode = DISABLE;
	hcan1.Init.TransmitFifoPriority = DISABLE;

	if (HAL_CAN_Init(&hcan1) != HAL_OK)
	{
		Error_Handler();
	}
}

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	// On every call back we have to update the CCR1
	// CCR1: Capture/Compare 1 Register
	// Channel 1 Callback
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
	{
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
		uint32_t ccr = __HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_1);
		__HAL_TIM_SET_COMPARE(htim,TIM_CHANNEL_1,ccr+ch1_pulse);
	}
	// Channel 2 Callback
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
	{
		uint32_t ccr = __HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_2);
		__HAL_TIM_SET_COMPARE(htim,TIM_CHANNEL_2,ccr+ch2_pulse);

	}
	// Channel 3 Callback
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
	{

		uint32_t ccr = __HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_3);
		__HAL_TIM_SET_COMPARE(htim,TIM_CHANNEL_3,ccr+ch3_pulse);

	}
	// Channel 4 Callback
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
	{

		uint32_t ccr = __HAL_TIM_GET_COMPARE(htim, TIM_CHANNEL_4);
		__HAL_TIM_SET_COMPARE(htim,TIM_CHANNEL_4,ccr+ch4_pulse);

	}
}

/*
 * @fn					- TIM2_Init
 * @brief				- Set up Timer 2 peripheral
 * @param				-
 * @return				-
 */
void TIM2_Init()
{
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 1;
	htim2.Init.Period = 0xFFFFFFFF;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;

	if (HAL_TIM_OC_Init(&htim2) != HAL_OK)
	{
		Error_Handler();
	}

	TIM_OC_InitTypeDef tim2OC_Init;
	memset(&tim2OC_Init, 0,sizeof(tim2OC_Init));
	tim2OC_Init.OCMode = TIM_OCMODE_TOGGLE;
	tim2OC_Init.OCPolarity = TIM_OCPOLARITY_HIGH;
	tim2OC_Init.Pulse = ch1_pulse;
	// Config Channel 1 of TIM2
	if (HAL_TIM_OC_ConfigChannel(&htim2, &tim2OC_Init, TIM_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}


	tim2OC_Init.Pulse = ch2_pulse;
	if (HAL_TIM_OC_ConfigChannel(&htim2, &tim2OC_Init, TIM_CHANNEL_2) != HAL_OK)
	{
		Error_Handler();
	}

	tim2OC_Init.Pulse = ch3_pulse;
	if (HAL_TIM_OC_ConfigChannel(&htim2, &tim2OC_Init, TIM_CHANNEL_3) != HAL_OK)
	{
		Error_Handler();
	}

	tim2OC_Init.Pulse = ch4_pulse;
	if (HAL_TIM_OC_ConfigChannel(&htim2, &tim2OC_Init, TIM_CHANNEL_4) != HAL_OK)
	{
		Error_Handler();
	}

}
/*
 * @fn					- SystemClock_Config
 * @brief				- This function modify the System Clock Frequency
 * @param				- SYSCLKFreq
 * @return				- void
 */
void SystemClock_Config(uint8_t SYSCLKFreq)
{
	RCC_OscInitTypeDef osc_init = {0};
	RCC_ClkInitTypeDef clk_init = {0};
	// Initialize osc_init
	/*
	 * PLL
	 * Src_Clock: HSI
	 */
	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSE;
	osc_init.HSIState = RCC_HSI_ON;
	osc_init.HSEState = RCC_HSE_ON;

	switch (SYSCLKFreq) {
		case SYSCLK_FREQ_50_MHz:
			osc_init.PLL.PLLM = 8;
			osc_init.PLL.PLLN = 100;
			osc_init.PLL.PLLP = 2;
			osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSE;
			osc_init.PLL.PLLState = RCC_PLL_ON;

			// Initialize osc_init
			if (HAL_RCC_OscConfig(&osc_init) != HAL_OK)
			{
				Error_Handler();
			}

			// Configure that Clock Init
			clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
			clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK  | \
									RCC_CLOCKTYPE_PCLK1  | RCC_CLOCKTYPE_PCLK2;
			clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
			clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
			clk_init.APB2CLKDivider = RCC_HCLK_DIV2;
			if (HAL_RCC_ClockConfig(&clk_init, 2) != HAL_OK)
			{
				Error_Handler();
			}
			break;

		case SYSCLK_FREQ_84_MHz:
			osc_init.PLL.PLLM = 8;
			osc_init.PLL.PLLN = 168;
			osc_init.PLL.PLLP = 2;
			osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSE;
			osc_init.PLL.PLLState = RCC_PLL_ON;
			// Initialize osc_init
			if (HAL_RCC_OscConfig(&osc_init) != HAL_OK)
			{
				Error_Handler();
			}

			// Configure that Clock Init
			clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
			clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK  | \
									RCC_CLOCKTYPE_PCLK1  | RCC_CLOCKTYPE_PCLK2;
			clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
			clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
			clk_init.APB2CLKDivider = RCC_HCLK_DIV2;
			if (HAL_RCC_ClockConfig(&clk_init, FLASH_ACR_LATENCY_2WS) != HAL_OK)
			{
				Error_Handler();
			}
			break;
		case SYSCLK_FREQ_120_MHz:
			osc_init.PLL.PLLM = 8;
			osc_init.PLL.PLLN = 240;
			osc_init.PLL.PLLP = 2;
			osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSE;
			osc_init.PLL.PLLState = RCC_PLL_ON;
			// Initialize osc_init
			if (HAL_RCC_OscConfig(&osc_init) != HAL_OK)
			{
				Error_Handler();
			}

			// Configure that Clock Init
			clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
			clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK  | \
									RCC_CLOCKTYPE_PCLK1  | RCC_CLOCKTYPE_PCLK2;
			clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
			clk_init.APB1CLKDivider = RCC_HCLK_DIV4;
			clk_init.APB2CLKDivider = RCC_HCLK_DIV2;
			if (HAL_RCC_ClockConfig(&clk_init, 3) != HAL_OK)
			{
				Error_Handler();
			}
			break;

			break;
		default:
			break;
		}


}
/*
 * @name: 		- UART2_Init
 */
void UART2_Init()
{
	//Info:
	/*
	 * 3V3 -> Green
	 * TX -> Yellow
	 * RX -> Orange
	 * GND -> Red
	 *
	 */
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.OverSampling = UART_OVERSAMPLING_8;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		Error_Handler();
	}


}

void LED_Init()
{
	GPIO_InitTypeDef led;
	__HAL_RCC_GPIOD_CLK_ENABLE();
	led.Mode = GPIO_MODE_OUTPUT_PP;
	led.Pin = GPIO_PIN_13;
	HAL_GPIO_Init(GPIOD, &led);
}


int main(void)
{
	char debug_msg[80] = "";
	HAL_Init();
	SystemClock_Config(SYSCLK_FREQ_50_MHz);
	char msg[50];
	// System clock check
	 //printf("System clock: %lu Hz\r\n", HAL_RCC_GetSysClockFreq());
	// LED Init
	//LED_Init();
	// Timer 2 Initialization
	//TIM2_Init();
	// GPIO Init for the DS18b20 on STM32 discovery board
	UART2_Init();
	// Test
	sprintf(msg,"Inside main function\n");
	HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

	// CAN Init
	CAN1_Init();

	// CAN filtering
	CAN_filter();

	if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_TX_MAILBOX_EMPTY | CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_BUSOFF) != HAL_OK)
	{
		Error_Handler();
	}
	// Start the CAN so it switch from Initialization -> Normal mode
	if (HAL_CAN_Start(&hcan1) != HAL_OK)
	{
		Error_Handler();
	}
	// CAN TX message
	CAN1_Tx();
	// CAN RX message (Loop back so everything we sent, we received it back
//	CAN1_Rx();
	while (1)
	{

	}
}

// Delay with microsecond version
void delay_us(uint32_t microsec)
{
	uint32_t tick1 = __HAL_TIM_GET_COUNTER(&htim2);
	while ((( __HAL_TIM_GET_COUNTER(&htim2) - tick1 ) & 0xFFFFFFFFU) < microsec)
	{
		;
	}
}


void Error_Handler()
{
	while (1)
	{
		;
	}
}

