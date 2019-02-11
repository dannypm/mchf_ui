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

#include "ui_driver.h"
#include "api_driver.h"

#define	API_DRIVER_USE_POLLING
//#define	API_DRIVER_USE_DMA

#ifdef API_DRIVER_USE_DMA
enum {
	TRANSFER_WAIT,
	TRANSFER_COMPLETE,
	TRANSFER_ERROR
};
__IO uint32_t wTransferState = TRANSFER_WAIT;
static DMA_HandleTypeDef hdma_tx;
static DMA_HandleTypeDef hdma_rx;
#endif

#ifdef API_DRIVER_USE_POLLING
uchar got_data = 0;
#endif

#ifdef CHIP_F7
#include "stm32f7xx_hal_rcc.h"
#include "stm32f7xx_hal_gpio.h"
#include "stm32f7xx_hal_cortex.h"
#include "stm32f7xx_hal_spi.h"
#endif
#ifdef CHIP_H7
#include "stm32h7xx_hal_rcc.h"
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_cortex.h"
#include "stm32h7xx_hal_spi.h"
#endif

// DMA buffers
uchar 				DmaInBuffer[API_TRANSFER_SIZE];
uchar 				DmaOuBuffer[API_TRANSFER_SIZE];
// Local buffers
uchar 				aRxBuffer[API_TRANSFER_SIZE];
uchar 				aTxBuffer[API_TRANSFER_SIZE];
uchar				rx_done = 0;
uchar				rx_active = 0;
ushort				cmd_req_in_progress = 0;

// Spectrum control publics
extern struct 		UI_SW	ui_sw;

// Public radio state
extern struct	TRANSCEIVER_STATE_UI	tsu;

// Driver communication
extern 			osMessageQId 			ApiMessage;

//#ifdef CHIP_F7

SPI_HandleTypeDef 	ApiSpiHandle;

//uchar dsp_irq_test = 0;

// SPI DMA RX
void SPIx_DMA_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(ApiSpiHandle.hdmarx);
}

// SPI DMA TX
void SPIx_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(ApiSpiHandle.hdmatx);
}

#ifndef API_SWAPPED_CS_IRQ
void EXTI9_5_IRQHandler(void)
{
	ulong i,res;

	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_5) != RESET)
	{
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_5);

		rx_active = 1;	// lock out the api_ui_send_fast_cmd
		res = HAL_SPI_Receive(&ApiSpiHandle,(uint8_t*)DmaInBuffer, API_TRANSFER_SIZE, 5000);
		rx_active = 0; 	// release lock

		if(res == HAL_OK)
		{
			// On success, copy to local buffer and set flag for further processing
			// without stalling the IRQ
			if(!rx_done)
			{
				// Copy to another buffer to prevent hanging
      			for(i = 0; i < API_TRANSFER_SIZE; i++)
      				aRxBuffer[i] = DmaInBuffer[i];

      			// Processing flag
      			rx_done = 1;
      		}
		}
	}
}
#endif

#ifdef API_SWAPPED_CS_IRQ
void EXTI4_IRQHandler(void)
{
	ulong i,res;

	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_4) != RESET)
	{
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4);

		rx_active = 1;	// lock out the api_ui_send_fast_cmd
		res = HAL_SPI_Receive(&ApiSpiHandle,(uint8_t*)DmaInBuffer, API_TRANSFER_SIZE, 5000);
		rx_active = 0; 	// release lock

		if(res == HAL_OK)
		{
			// On success, copy to local buffer and set flag for further processing
			// without stalling the IRQ
			if(!rx_done)
			{
				// Copy to another buffer to prevent hanging
      			for(i = 0; i < API_TRANSFER_SIZE; i++)
      				aRxBuffer[i] = DmaInBuffer[i];

      			// Processing flag
      			rx_done = 1;
      		}
		}
	}
}
#endif

//void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
static void api_ui_spi_hw_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	//if(hspi->Instance != SPI2)
	//	return;

	__HAL_RCC_SPI2_CLK_ENABLE();

	#ifndef API_SWAPPED_CS_IRQ
	// IRQ - PD4
	GPIO_InitStruct.Pin		  = GPIO_PIN_4;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	// High by default
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET);

	// CS - PD5 as gpio
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	// CS interrupt
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0x0F, 0x00);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	#endif

	#ifdef API_SWAPPED_CS_IRQ
#if 1
	// Local IRQ(DSP CS) - PD4
	GPIO_InitStruct.Pin		  = GPIO_PIN_4;
	GPIO_InitStruct.Mode      = GPIO_MODE_IT_FALLING;

	#ifdef CHIP_F7
	GPIO_InitStruct.Speed     = GPIO_SPEED_LOW;
	#endif

	#ifdef CHIP_H7
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
	#endif

	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	// CS interrupt
	HAL_NVIC_SetPriority(EXTI4_IRQn, 0x0F, 0x00);
	HAL_NVIC_EnableIRQ(EXTI4_IRQn);
#endif
#if 1
	// Local CS(DSP IRQ) - PD5
	GPIO_InitStruct.Pin 	= GPIO_PIN_5;
	GPIO_InitStruct.Mode 	= GPIO_MODE_OUTPUT_PP;
	#ifdef CHIP_F7
	GPIO_InitStruct.Speed 	= GPIO_SPEED_HIGH;
	#endif
	#ifdef CHIP_H7
	GPIO_InitStruct.Speed 	= GPIO_SPEED_FREQ_HIGH;
	#endif
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	// High by default
	//HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, GPIO_PIN_SET);
	GPIOD->BSRRL = GPIO_PIN_5;

#endif
	#endif

	// SPI SCK
	GPIO_InitStruct.Pin       = GPIO_PIN_9;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	#ifdef CHIP_F7
	GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
	#endif
	#ifdef CHIP_H7
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
	#endif
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

#if 1
	// SPI MISO
	GPIO_InitStruct.Pin 	  = GPIO_PIN_2;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	#ifdef CHIP_F7
	GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
	#endif
	#ifdef CHIP_H7
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
	#endif
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
#endif
#if 1
	// SPI MOSI
	GPIO_InitStruct.Pin		  = GPIO_PIN_1;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	#ifdef CHIP_F7
	GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
	#endif
	#ifdef CHIP_H7
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
	#endif
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
#endif

#ifdef API_DRIVER_USE_DMA

	__HAL_RCC_DMA1_CLK_ENABLE();

	 // Configure the DMA handler for Transmit
/*	 hdma_tx.Instance                 = DMA1_Stream4;
	 hdma_tx.Init.Channel             = DMA_CHANNEL_0;
	 hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
	 hdma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
	 hdma_tx.Init.MemBurst            = DMA_MBURST_INC4;
	 hdma_tx.Init.PeriphBurst         = DMA_PBURST_INC4;
	 hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
	 hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
	 hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
	 hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	 hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
	 hdma_tx.Init.Mode                = DMA_NORMAL;
	 hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;

	 HAL_DMA_Init(&hdma_tx);

	 // Associate the initialized DMA handle to the the SPI handle
	 __HAL_LINKDMA(&ApiSpiHandle, hdmatx, hdma_tx);*/

	 // Configure the DMA handler for Receive
	 hdma_rx.Instance                 = DMA1_Stream3;
	 hdma_rx.Init.Channel             = DMA_CHANNEL_0;
	 hdma_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
	 hdma_rx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
	 hdma_rx.Init.MemBurst            = DMA_MBURST_INC4;
	 hdma_rx.Init.PeriphBurst         = DMA_PBURST_INC4;
	 hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
	 hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
	 hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
	 hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	 hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
	 hdma_rx.Init.Mode                = DMA_NORMAL;
	 hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;

	 if(HAL_DMA_Init(&hdma_rx) != HAL_OK)
	 {
		 printf("dma err!\r\n");
	 }

	 // Associate the initialised DMA handle to the the SPI handle
	 __HAL_LINKDMA(&ApiSpiHandle, hdmarx, hdma_rx);

	 // NVIC configuration for DMA transfer complete interrupt (SPI2_TX)
	 //HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 1, 1);
	 //HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);

	 // NVIC configuration for DMA transfer complete interrupt (SPI2_RX)
	 HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
	 HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);

	 // NVIC for SPI
	 //HAL_NVIC_SetPriority(SPI2_IRQn, 1, 0);
	 //HAL_NVIC_EnableIRQ(SPI2_IRQn);
#endif
}

// Called by main(), instead of local init!!!
// due to H7 bug..
void api_driver_hw_init(void)
{
	//printf("api_ui_hw_init->1\r\n");

	api_ui_spi_hw_init();

	ApiSpiHandle.Instance               = SPI2;
	ApiSpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	ApiSpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
	ApiSpiHandle.Init.CLKPhase          = SPI_PHASE_2EDGE;
	ApiSpiHandle.Init.CLKPolarity       = SPI_POLARITY_HIGH;
	ApiSpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
	ApiSpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
	ApiSpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
	ApiSpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
	ApiSpiHandle.Init.CRCPolynomial     = 7;
	ApiSpiHandle.Init.NSS               = SPI_NSS_SOFT;
	ApiSpiHandle.Init.Mode 			 	= SPI_MODE_SLAVE;

	// H7 only maybe ?
	ApiSpiHandle.Init.NSSPMode          = SPI_NSS_PULSE_DISABLE;
	ApiSpiHandle.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_ENABLE;  // Recommended setting to avoid glitches

	if(HAL_SPI_Init(&ApiSpiHandle) != HAL_OK)
	{
		printf("spi err init\r\n");
	}

	 //printf("api_ui_hw_init->2\r\n");

#ifdef API_DRIVER_USE_DMA
	 //HAL_SPI_Receive_DMA(&ApiSpiHandle,(uint8_t *)aRxBuffer, 300);
#endif
	 //printf("api_ui_hw_init->3\r\n");
}

static void api_ui_send_spi(void)
{
	// Generate DSP IRQ
	#ifndef API_SWAPPED_CS_IRQ
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET);
	#endif
	#ifdef API_SWAPPED_CS_IRQ
	//HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, GPIO_PIN_RESET);
	GPIOD->BSRRH = GPIO_PIN_5;
	#endif
	__asm(".word 0x46C046C0");

	HAL_SPI_Transmit(&ApiSpiHandle,(uint8_t*)aTxBuffer, 16, 1000);

	__asm(".word 0x46C046C0");
	#ifndef API_SWAPPED_CS_IRQ
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET);
	#endif
	#ifdef API_SWAPPED_CS_IRQ
	//HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, GPIO_PIN_SET);
	GPIOD->BSRRL = GPIO_PIN_5;
	#endif
}

// Directly send OS messages to DSP
static void api_ui_send_spi_a(ulong *msg)
{
	struct APIMessage *api_msg = (struct APIMessage *)msg;
	uchar  i;

	printf("api_ui_send_spi_a,msg: 0x%02x\r\n",api_msg->usMessageID);

	aTxBuffer[0x00] = (api_msg->usMessageID >>   8);
	aTxBuffer[0x01] = (api_msg->usMessageID & 0xFF);

	if(api_msg->ucPayload)
	{
		for(i = 0; i < 13; i++)
			aTxBuffer[i] = api_msg->ucData[i];
	}

	// Generate DSP IRQ
	GPIOD->BSRRH = GPIO_PIN_5;
	__asm(".word 0x46C046C0");

	HAL_SPI_Transmit(&ApiSpiHandle,(uint8_t*)aTxBuffer, 16, 1000);

	// Release DSP IRQ
	__asm(".word 0x46C046C0");
	GPIOD->BSRRL = GPIO_PIN_5;
}

static void api_ui_process_broadcast(void)
{
	ulong i,temp;

	// Data waiting ?
	if(!rx_done)
		return;

	// Data valid ?
	if((aRxBuffer[0] != 0x12) || (aRxBuffer[1] != 0x34) || (aRxBuffer[298] != 0x55) || (aRxBuffer[299] != 0xAA))
		goto exit_anyway;

	// Check seq no
	if(tsu.dsp_seq_number_old == aRxBuffer[3])
		tsu.dsp_alive = 0;
	else
		tsu.dsp_alive = 1;

	// Save sequence number for next transfer
	tsu.dsp_seq_number_old = aRxBuffer[3];

	// Get blinker
	tsu.dsp_blinker = aRxBuffer[2];

	// Get firmware version
	tsu.dsp_rev1 = aRxBuffer[4];
	tsu.dsp_rev2 = aRxBuffer[5];
	tsu.dsp_rev3 = aRxBuffer[6];
	tsu.dsp_rev4 = aRxBuffer[7];

	// Load DSP frequency
	temp  = aRxBuffer[0x08] << 24;
	temp |= aRxBuffer[0x09] << 16;
	temp |= aRxBuffer[0x0A] <<  8;
	temp |= aRxBuffer[0x0B] <<  0;
	tsu.dsp_freq = temp;	//temp/4;
	//printf("dsp freq = %d\r\n",tsu.dsp_freq);

	// Has local freq being invalidated by band change ?
	//if(tsu.vfo_a == 0xFFFFFFFF)
	//	tsu.vfo_a = tsu.dsp_freq;

	// Temp, load initial VFO B here
	//if(tsu.vfo_b == 0xFFFFFFFF)
	//	tsu.vfo_b = tsu.dsp_freq;

	// Get demod mode
	tsu.dsp_demod  = aRxBuffer[0x0C];

	// Has local demodulator mode being invalidated by band change ?
	//if(tsu.demod_mode == 0xFF)
	//	tsu.demod_mode = tsu.dsp_demod;

	// Get band
	tsu.dsp_band   = aRxBuffer[0x0D];
	//printf("dsp band = %d\r\n",tsu.dsp_band);

	// Volume level in DSP
	tsu.dsp_volume = aRxBuffer[0x0E];

	// Filter selection in DSP
	tsu.dsp_filter = aRxBuffer[0x0F];
	//printf("tsu.dsp_filter = %d\r\n",tsu.dsp_filter);

	// Has local filter being invalidated by band change ?
	//if(tsu.curr_filter == 0xFF)
	//	tsu.curr_filter = tsu.dsp_filter;

	// Tuning step in DSP, but as ptr id, not actual value
	tsu.dsp_step_idx = aRxBuffer[0x10];

	// NCO freq in DSP
	tsu.dsp_nco_freq  = aRxBuffer[0x11] <<  8;
	tsu.dsp_nco_freq |= aRxBuffer[0x12] <<  0;
	//printf("dsp_nco_freq %d\r\n",tsu.dsp_nco_freq);

	tsu.pcb_rev = aRxBuffer[0x13];

	// Copy FFT
	for(i = 0; i < 256; i++)
		ui_sw.fft_dsp[i] = aRxBuffer[i + 0x28];

exit_anyway:

	// Acknowledge next
	rx_done = 0;
}

static void api_ui_send_fast_cmd(void)
{
	ulong i,temp;

	if(rx_active)
		return;
#if 1
	// Change band - always first as highest priority call
	if(tsu.update_band_dsp_req)
	{
		#ifdef API_UI_ALLOW_DEBUG
		printf("tsu.curr_band = %d\r\n",tsu.curr_band);
		#endif

		// cmd
		aTxBuffer[0x00] = (API_UPD_BAND >>   8);
		aTxBuffer[0x01] = (API_UPD_BAND & 0xFF);
		// data
		aTxBuffer[0x02] = tsu.curr_band;

		// Send
		api_ui_send_spi();
		// Clear flag
		tsu.update_band_dsp_req = 0;
		//
		return;
	}
#endif
#if 1
	// Update frequency
	if(tsu.update_freq_dsp_req)
	{
		if(tsu.band[tsu.curr_band].active_vfo == 0)
			temp = (tsu.band[tsu.curr_band].vfo_a);
		else
			temp = (tsu.band[tsu.curr_band].vfo_b);

		#ifdef API_UI_ALLOW_DEBUG
		printf("update DSP freq = %d\r\n",temp);
		#endif

		// Do in DSP ??
		temp *= 4;

		// cmd
		aTxBuffer[0x00] = (API_UPD_FREQ >>   8);
		aTxBuffer[0x01] = (API_UPD_FREQ & 0xFF);
		// data
		aTxBuffer[0x02] = temp >> 24;
		aTxBuffer[0x03] = temp >> 16;
		aTxBuffer[0x04] = temp >>  8;
		aTxBuffer[0x05] = temp >>  0;
		// Send
		api_ui_send_spi();
		// Reset flag
		tsu.update_freq_dsp_req  = 0;
		//
		return;
	}
#endif
#if 1
	// NCO frequency
	if(tsu.update_nco_dsp_req)
	{
		#ifdef API_UI_ALLOW_DEBUG
		printf("nco freq = %d\r\n",tsu.band[tsu.curr_band].nco_freq);
		#endif

		aTxBuffer[0x00] = (API_UPD_NCO >>   8);
		aTxBuffer[0x01] = (API_UPD_NCO & 0xFF);
		//
		aTxBuffer[0x02] = tsu.band[tsu.curr_band].nco_freq >>  8;
		aTxBuffer[0x03] = tsu.band[tsu.curr_band].nco_freq >>  0;
		// Send
		api_ui_send_spi();
		// Reset flag
		tsu.update_nco_dsp_req = 0;
		return;
	}
#endif
#if 1
	// Update audio volume
	if(tsu.update_audio_dsp_req)
	{
		#ifdef API_UI_ALLOW_DEBUG
		printf("tsu.audio_volume = %d\r\n",tsu.band[tsu.curr_band].volume);
		#endif

		aTxBuffer[0x00] = (API_UPD_VOL >>   8);
		aTxBuffer[0x01] = (API_UPD_VOL & 0xFF);
		//
		aTxBuffer[0x02] = tsu.band[tsu.curr_band].volume;
		// Send
		api_ui_send_spi();
		// Reset flag
		tsu.update_audio_dsp_req = 0;
		//
		return;
	}
#endif
#if 1
	// Update demod mode
	//if((tsu.dsp_demod != tsu.demod_mode) && (tsu.demod_mode != 0xFF))
	if(tsu.update_demod_dsp_req)
	{
		#ifdef API_UI_ALLOW_DEBUG
		printf("demod_mode = %d\r\n",tsu.band[tsu.curr_band].demod_mode);
		#endif

		aTxBuffer[0x00] = (API_UPD_DEMOD >>   8);
		aTxBuffer[0x01] = (API_UPD_DEMOD & 0xFF);
		//
		aTxBuffer[0x02] = tsu.band[tsu.curr_band].demod_mode;
		// Send
		api_ui_send_spi();
		// Reset flag
		tsu.update_demod_dsp_req = 0;
		return;
	}
#endif
#if 1
	// Change filter
	//if((tsu.dsp_filter != tsu.curr_filter) && (tsu.curr_filter != 0xFF))
	if(tsu.update_filter_dsp_req)
	{
		#ifdef API_UI_ALLOW_DEBUG
		printf("filter = %d\r\n",tsu.band[tsu.curr_band].filter);
		#endif

		aTxBuffer[0x00] = (API_UPD_FILT >>   8);
		aTxBuffer[0x01] = (API_UPD_FILT & 0xFF);
		//
		aTxBuffer[0x02] = tsu.band[tsu.curr_band].filter;
		// Send
		api_ui_send_spi();
		//  Reset flag
		tsu.update_filter_dsp_req = 0;
		return;
	}
#endif
#if 0
	// Tuning step
	//if((tsu.step_idx != tsu.dsp_step_idx) && (tsu.step_idx != 0xFF))
	//{
		//#ifdef API_UI_ALLOW_DEBUG
		//printf("tsu.step_idx = %d\r\n",tsu.step_idx);
		//#endif

		aTxBuffer[0x00] = (API_UPD_STEP >>   8);
		aTxBuffer[0x01] = (API_UPD_STEP & 0xFF);
		//
		aTxBuffer[0x02] = 3;//tsu.step_idx;

		api_ui_send_spi();
		return;
	//}
#endif
#if 1
	// CW TX test
	if(tsu.cw_tx_state)
	{
		#ifdef API_UI_ALLOW_DEBUG
		printf("cw_tx_state = %d\r\n",tsu.cw_tx_state);
		//printf("tsu.cw_iamb_type = %d\r\n",tsu.cw_iamb_type);
		#endif

		aTxBuffer[0x00] = (API_CW_TX >>   8);
		aTxBuffer[0x01] = (API_CW_TX & 0xFF);
		//
		if(tsu.cw_tx_state == 1)
			aTxBuffer[0x02] = 1;					// tx on

		if(tsu.cw_tx_state == 2)
			aTxBuffer[0x02] = 0;					// tx off

		aTxBuffer[0x03] = tsu.cw_iamb_type;			// iambic line

		api_ui_send_spi();

		// Release state no longer needed
		if(tsu.cw_tx_state == 2)
		{
			tsu.cw_tx_state = 0;

			#ifdef API_UI_ALLOW_DEBUG
			printf("reset ok\r\n");
			#endif
		}

		return;
	}
#endif
#if 1
	// Write into DSP eeprom structure
	if(tsu.update_dsp_eep_req)
	{
		ushort offset = 0, size = 0;

		#ifdef API_UI_ALLOW_DEBUG
		printf("update DSP eep request process..\r\n");
		#endif

		aTxBuffer[0x00] = (API_WRITE_EEP >>   8);
		aTxBuffer[0x01] = (API_WRITE_EEP & 0xFF);
		// Offset
		aTxBuffer[0x02] = tsu.update_dsp_eep_offset >> 8;
		aTxBuffer[0x03] = tsu.update_dsp_eep_offset;
		// size
		aTxBuffer[0x04] = tsu.update_dsp_eep_size;
		// value
		aTxBuffer[0x05] = tsu.update_dsp_eep_value >> 24;
		aTxBuffer[0x06] = tsu.update_dsp_eep_value >> 16;
		aTxBuffer[0x07] = tsu.update_dsp_eep_value >>  8;
		aTxBuffer[0x08] = tsu.update_dsp_eep_value;
		// Send
		api_ui_send_spi();
		//  Reset flag
		tsu.update_dsp_eep_req = 0;
		return;
	}
#endif
#if 0
	// Send DSP restart request
	if(tsu.update_dsp_restart)
	{
		#ifdef API_UI_ALLOW_DEBUG
		printf("restart DSP request process..\r\n");
		#endif

		aTxBuffer[0x00] = (API_RESTART >>   8);
		aTxBuffer[0x01] = (API_RESTART & 0xFF);
		// Send
		api_ui_send_spi();
		//  Reset flag
		tsu.update_dsp_restart = 0;
		return;
	}
#endif
}

// This version using messaging, instead of public flags
//
// - work in progress
//
static void api_ui_send_fast_cmd_a(void)
{
	osEvent event;

	// Wait for a short time for pending messages
	event = osMessageGet(ApiMessage, 20);
	if(event.status != osEventMessage)
		return;												// ideally this should fire every 20mS

	// Check status type
	if(event.status != osEventMessage)
		return;												// is that even possible ?

	// Send
	api_ui_send_spi_a(event.value.p);
}

// CS pin is also used to enter bootloader mode,
// that is why delayed start of broadcast
//
static void api_ui_allow_broadcast(void)
{
	// Broadcast on
	aTxBuffer[0x00] = (API_ENABLE_POST >>   8);
	aTxBuffer[0x01] = (API_ENABLE_POST & 0xFF);
	api_ui_send_spi();
}

static void api_driver_worker(void)
{
	api_ui_send_fast_cmd_a();
	api_ui_send_fast_cmd();
	api_ui_process_broadcast();
}

void api_driver_task(void const * argument)
{
	// HW init is done, we can allow the DSP to start broadcast
	//--api_ui_allow_broadcast();

	// Delay start, so UI can paint properly
	OsDelayMs(3000);

	printf("api driver start\r\n");

	// -----------------------------------------------
	// HW init done by main(), due to bug on the H7
	//--api_ui_hw_init();
	// -----------------------------------------------

	//OsDelayMs(200);

api_driver_loop:

	api_driver_worker();

	//-- OsDelayMs(20);		instead of idle-ing here, wait messages. ToDo: test if this stalls other drivers!!!
	goto api_driver_loop;
}

//#endif


