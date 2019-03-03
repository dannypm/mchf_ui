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
#ifndef __API_DRIVER_H
#define __API_DRIVER_H

//#define API_UI_ALLOW_DEBUG

#define API_TRANSFER_SIZE				300
//#define API_NO_UPDATE					0xFF

// DMA transfers - still work in progress
//#define	API_DRIVER_USE_DMA

//#define SPI2_TX_DMA_STREAM               DMA2_Stream3
//#define SPI2_TX_DMA_REQUEST              DMA_REQUEST_SPI2_TX
//#define SPI2_DMA_TX_IRQn                 DMA2_Stream3_IRQn

#define SPI2_RX_DMA_STREAM               DMA2_Stream2
#define SPI2_RX_DMA_REQUEST              DMA_REQUEST_SPI2_RX
#define SPI2_DMA_RX_IRQn                 DMA2_Stream2_IRQn

// ---------------------------------------------------------------------------
// Old proto PCBs have PD5 as DSP CS(our IRQ) and PD4 as DSP IRQ(our CS)
// but it makes sense to swap them, so the local IRQ(DSP CS) goes to
// EXTI4_IRQHandler separate handler and not share with digitizer PF8 IRQ
// pin that ends up in EXTI9_5_IRQHandler
//
#define API_SWAPPED_CS_IRQ

// Export and call from main() due to H7 HW bug
void api_driver_hw_init(void);
void api_driver_task(void const * argument);

#endif
