/************************************************************************************
**                                                                                 **
**                             mcHF Pro QRP Transceiver                            **
**                         Krassi Atanassov - M0NKA 2012-2019                      **
**                            mail: djchrismarc@gmail.com                          **
**                                 twitter: @M0NKA_                                **
**---------------------------------------------------------------------------------**
**                                                                                 **
**  File name:                                                                     **
**  Description:                                                                   **
**  Last Modified:                                                                 **
**  Licence:                                                                       **
**          The mcHF project is released for radio amateurs experimentation,       **
**          non-commercial use only. All source files under GPL-3.0, unless        **
**          third party drivers specifies otherwise. Thank you!                    **
************************************************************************************/

#include "mchf_pro_board.h"

#include "net_driver.h"

// We can completely bypass and remove
// each driver with this
#ifdef CONTEXT_NET

#ifdef CHIP_H7
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_rcc.h"
#include "stm32h7xx_hal_uart.h"
#endif

UART_HandleTypeDef 	UartHandle;
DMA_HandleTypeDef 	hdma_tx;
DMA_HandleTypeDef 	hdma_rx;

void USARTx_DMA_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(UartHandle.hdmarx);
}

void USARTx_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(UartHandle.hdmatx);
}

void USARTx_IRQHandler(void)
{
  HAL_UART_IRQHandler(&UartHandle);
}

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_TEMPLATE
//*----------------------------------------------------------------------------
void net_driver_hw_init(void)
{
#ifdef CHIP_H7
	GPIO_InitTypeDef  			GPIO_InitStruct;
	RCC_PeriphCLKInitTypeDef 	RCC_PeriphClkInit;

	// Select SysClk as source of USART1 clocks
	RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART16;
	RCC_PeriphClkInit.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
	HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);

	__HAL_RCC_USART6_CLK_ENABLE();
	__HAL_RCC_DMA1_CLK_ENABLE();	// correct ?

	// UART TX GPIO pin configuration
	GPIO_InitStruct.Pin       	= GPIO_PIN_14;
	GPIO_InitStruct.Mode      	= GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      	= GPIO_PULLUP;
	GPIO_InitStruct.Speed     	= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate 	= GPIO_AF7_USART6;				// on F7 is AF8!!!
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	// UART RX GPIO pin configuration
	GPIO_InitStruct.Pin 		= GPIO_PIN_9;
	GPIO_InitStruct.Alternate 	= GPIO_AF7_USART6;				// on F7 is AF8!!!
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	// Configure the DMA handler for Transmission process
	hdma_tx.Instance                 = DMA1_Stream7;			// ToDo: 	check if correct stream ?
	hdma_tx.Init.Request             = DMA_REQUEST_USART1_TX;	// 			problem with LCD or other DMA ??
	hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
	hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
	hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
	hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
	hdma_tx.Init.Mode                = DMA_NORMAL;
	hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;
	hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
	hdma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
	hdma_tx.Init.MemBurst            = DMA_MBURST_INC4;
	hdma_tx.Init.PeriphBurst         = DMA_PBURST_INC4;

	HAL_DMA_Init(&hdma_tx);

	// Associate the initialised DMA handle to the UART handle
	__HAL_LINKDMA(&UartHandle, hdmatx, hdma_tx);

	// Configure the DMA handler for reception process
	hdma_rx.Instance                 = DMA1_Stream5;			// ToDo: 	check if correct stream ?
	hdma_rx.Init.Request             = DMA_REQUEST_USART1_RX;	// 			problem with LCD or other DMA ??
	hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
	hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
	hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
	hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
	hdma_rx.Init.Mode                = DMA_NORMAL;
	hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;
	hdma_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
	hdma_rx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
	hdma_rx.Init.MemBurst            = DMA_MBURST_INC4;
	hdma_rx.Init.PeriphBurst         = DMA_PBURST_INC4;

	HAL_DMA_Init(&hdma_rx);

	// Associate the initialised DMA handle to the the UART handle
	__HAL_LINKDMA(&UartHandle, hdmarx, hdma_rx);

	// NVIC configuration for DMA transfer complete interrupt (USARTx_TX)
	NVIC_SetPriority(DMA1_Stream7_IRQn, 1);
	NVIC_EnableIRQ(DMA1_Stream7_IRQn);

	// NVIC configuration for DMA transfer complete interrupt (USARTx_RX)
	NVIC_SetPriority(DMA1_Stream5_IRQn, 0);
	NVIC_EnableIRQ(DMA1_Stream5_IRQn);

	// NVIC configuration for USART, to catch the TX complete
	NVIC_SetPriority(USART1_IRQn, 1);
	NVIC_EnableIRQ(USART1_IRQn);

	// Uart init
	UartHandle.Instance        = USART6;
	UartHandle.Init.BaudRate   = 9600;
	UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	UartHandle.Init.StopBits   = UART_STOPBITS_1;
	UartHandle.Init.Parity     = UART_PARITY_NONE;
	UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	UartHandle.Init.Mode       = UART_MODE_TX_RX;
	UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;		// what is this ??

	if(HAL_UART_Init(&UartHandle) != HAL_OK)
	{
		printf("net_driver uart init error!\r\n");
	}
#endif
}

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_TEMPLATE
//*----------------------------------------------------------------------------
static void net_driver_worker(void)
{

}

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_TEMPLATE
//*----------------------------------------------------------------------------
void net_driver_task(void const * argument)
{
	char msg1[] = "AT+CWMODE=3\r\n";
	char msg2[] = "AT+CWLAP\r\n";
	char buff[200];

	// Delay start, so UI can paint properly
	OsDelayMs(6000);

	printf("net_driver start\r\n");

	// from main()
	//--net_driver_hw_init();

	// ----------------------------------------------------------------------------------
	// test
	// --
	if(HAL_UART_Transmit_DMA(&UartHandle, (uint8_t*)msg1, strlen(msg1))!= HAL_OK)
	{
		printf("net_driver err tx1\r\n");
	}

	memset(buff,0,sizeof(buff));
	if (HAL_UART_Receive_DMA(&UartHandle, (uint8_t *)buff, sizeof(buff)) != HAL_OK)
	{
		printf("net_driver err rx1\r\n");
	}

	while (HAL_UART_GetState(&UartHandle) == HAL_UART_STATE_BUSY_RX)
	{
	}

	if(buff[0]) printf(buff);
	printf("\r\n");

	while (HAL_UART_GetState(&UartHandle) != HAL_UART_STATE_READY)
	{
	}

	// --
	if(HAL_UART_Transmit_DMA(&UartHandle, (uint8_t*)msg2, strlen(msg2))!= HAL_OK)
	{
		printf("net_driver err tx2\r\n");
	}

	memset(buff,0,sizeof(buff));
	if (HAL_UART_Receive_DMA(&UartHandle, (uint8_t *)buff, sizeof(buff)) != HAL_OK)
	{
		printf("net_driver err rx2\r\n");
	}

	while (HAL_UART_GetState(&UartHandle) == HAL_UART_STATE_BUSY_RX)
	{
	}

	if(buff[0]) printf(buff);
	printf("\r\n");
	//
	// end test
	// ----------------------------------------------------------------------------------

net_driver_loop:

	//net_driver_worker();
	
	// Driver sleep period
	OsDelayMs(150);
		
	goto net_driver_loop;
}

#endif


