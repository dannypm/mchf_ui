/**
  ******************************************************************************
  * @file    lcdconf.c
  * @author  MCD Application Team
  * @version V1.1.1
  * @date    18-November-2015
  * @brief   This file implements the configuration for the GUI library
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "mchf_pro_board.h"

#include "mchf_lcd.h"
#include "GUI_Private.h"

#ifdef CHIP_F7
#include "stm32f7xx_hal_gpio.h"
#include "stm32f7xx_hal_rcc.h"
#include "stm32f7xx_hal_cortex.h"
#include "stm32f7xx_hal_ltdc.h"
#include "stm32f7xx_hal_dma2d.h"
#endif

#ifdef CHIP_H7
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_rcc.h"
#include "stm32h7xx_hal_cortex.h"
#include "stm32h7xx_hal_ltdc.h"
#include "stm32h7xx_hal_dma2d.h"
static DMA2D_HandleTypeDef           hdma2d;
#endif

LTDC_HandleTypeDef                   hltdc;  
static LCD_LayerPropTypedef          layer_prop[GUI_NUM_LAYERS];

static const LCD_API_COLOR_CONV * apColorConvAPI[] = 
{
  COLOR_CONVERSION_0,
#if GUI_NUM_LAYERS > 1
  COLOR_CONVERSION_1,
#endif
};

static void     DMA2D_CopyBuffer(U32 LayerIndex, void * pSrc, void * pDst, U32 xSize, U32 ySize, U32 OffLineSrc, U32 OffLineDst);
static void     DMA2D_FillBuffer(U32 LayerIndex, void * pDst, U32 xSize, U32 ySize, U32 OffLine, U32 ColorIndex);

static void     LCD_LL_Init(void); 
static void     LCD_LL_LayerInit(U32 LayerIndex); 
static void     LCD_LL_CopyBuffer(int LayerIndex, int IndexSrc, int IndexDst);
static void     LCD_LL_CopyRect(int LayerIndex, int x0, int y0, int x1, int y1, int xSize, int ySize);
static void     LCD_LL_FillRect(int LayerIndex, int x0, int y0, int x1, int y1, U32 PixelIndex);
static void     LCD_LL_DrawBitmap32bpp(int LayerIndex, int x, int y, U8 const * p,  int xSize, int ySize, int BytesPerLine);

static U32      GetBufferSize(U32 LayerIndex);

// IRQ
void LTDC_IRQHandler(void)
{
  HAL_LTDC_IRQHandler(&hltdc);
}

// IRQ
void DMA2D_IRQHandler(void)
{
//  if(OSReady != 1)
//  {
//    HAL_DMA2D_IRQHandler(&DMA2D_Handle);
//  }
//  else
//  {
//    osSemaphoreRelease(osDma2dSemph);
    DMA2D->CR &= ~(DMA2D_IT_TC);
    DMA2D->IFCR = DMA2D_FLAG_TC;
//  }
}


/**
  * @brief  Return Pixel format for a given layer
  * @param  LayerIndex : Layer Index 
  * @retval Status ( 0 : 0k , 1: error)
  */
static inline U32 LCD_LL_GetPixelformat(U32 LayerIndex)
{
  if (LayerIndex == 0)
  {
    return LTDC_PIXEL_FORMAT_ARGB8888;
  } 
  else
  {
    return LTDC_PIXEL_FORMAT_ARGB1555;
  } 
}

#ifdef CHIP_F7
void HAL_LTDC_MspInit(LTDC_HandleTypeDef *hltdc)
{  
	  GPIO_InitTypeDef gpio_init_structure;
  	  static RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;
  
  	  // Enable the LTDC clocks
  	  __HAL_RCC_LTDC_CLK_ENABLE();
  
#if 0
  	  // LCD clock configuration
  	  // PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz
  	  // PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN = 429 Mhz
  	  // PLLLCDCLK = PLLSAI_VCO Output/PLLSAIR = 429/5 = 85 Mhz
  	  // LTDC clock frequency = PLLLCDCLK / LTDC_PLLSAI_DIVR_2 = 85/4 = 21 Mhz
  	  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  	  PeriphClkInitStruct.PLLSAI.PLLSAIN = 192;
  	  PeriphClkInitStruct.PLLSAI.PLLSAIR = 5;
  	  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;	// 42 Mhz clock, correct ??
  	  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
#endif

  	  // 13.75 Mhz LCD clock
  	  PeriphClkInitStruct.PeriphClockSelection 	= RCC_PERIPHCLK_LTDC;
  	  PeriphClkInitStruct.PLLSAI.PLLSAIN 		= 55;
  	  PeriphClkInitStruct.PLLSAI.PLLSAIR 		= 2;
  	  PeriphClkInitStruct.PLLSAIDivR 			= RCC_PLLSAIDIVR_4;
  	  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  	  {
  		  // ToDo: handle this...
  		  while(1)
  		  {
  			__asm(".word 0x46004600");
  		  }
  	  }

  	  // Enable GPIOs clock
  	  __HAL_RCC_GPIOA_CLK_ENABLE();
  	  __HAL_RCC_GPIOB_CLK_ENABLE();
  	  __HAL_RCC_GPIOC_CLK_ENABLE();
  	  __HAL_RCC_GPIOD_CLK_ENABLE();
  	  __HAL_RCC_GPIOE_CLK_ENABLE();
  	  __HAL_RCC_GPIOF_CLK_ENABLE();
  	  __HAL_RCC_GPIOG_CLK_ENABLE();
  	  __HAL_RCC_GPIOI_CLK_ENABLE();
  	  //__HAL_RCC_GPIOJ_CLK_ENABLE();
  	  //__HAL_RCC_GPIOK_CLK_ENABLE();
  
  	  // -----------------------------------------------------------------------------------------
  	  // Multiplexer allocated pins, important - LTDC could use AF9 or AF14 !!!!
  	  // -----------------------------------------------------------------------------------------

  	  // GPIOA LTDC AF14 (PA5 moved to PA11 !!!)
  	  gpio_init_structure.Pin       = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4  | \
  			  	  	  	  	  	  	  GPIO_PIN_6 | GPIO_PIN_8 | GPIO_PIN_11 | GPIO_PIN_12;
  	  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  	  gpio_init_structure.Pull      = GPIO_NOPULL;
  	  gpio_init_structure.Speed     = GPIO_SPEED_FAST;
  	  gpio_init_structure.Alternate = GPIO_AF14_LTDC;
  	  HAL_GPIO_Init(GPIOA, &gpio_init_structure);

  	  // GPIOB LTDC AF9
  	  gpio_init_structure.Pin       = GPIO_PIN_0;
  	  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  	  gpio_init_structure.Pull      = GPIO_NOPULL;
  	  gpio_init_structure.Speed     = GPIO_SPEED_FAST;
  	  gpio_init_structure.Alternate = GPIO_AF9_LTDC;
  	  HAL_GPIO_Init(GPIOB, &gpio_init_structure);

  	  // GPIOB LTDC AF14
  	  gpio_init_structure.Pin       = GPIO_PIN_8 | GPIO_PIN_9 | \
  			  	  	  	  	  	  	  GPIO_PIN_10 | GPIO_PIN_11;
  	  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  	  gpio_init_structure.Pull      = GPIO_NOPULL;
  	  gpio_init_structure.Speed     = GPIO_SPEED_FAST;
  	  gpio_init_structure.Alternate = GPIO_AF14_LTDC;
  	  HAL_GPIO_Init(GPIOB, &gpio_init_structure);

  	  // GPIOC LTDC AF14
  	  gpio_init_structure.Pin       = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_10;
  	  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  	  gpio_init_structure.Pull      = GPIO_NOPULL;
  	  gpio_init_structure.Speed     = GPIO_SPEED_FAST;
  	  gpio_init_structure.Alternate = GPIO_AF14_LTDC;
  	  HAL_GPIO_Init(GPIOC, &gpio_init_structure);

  	  // GPIOD LTDC AF14
  	  gpio_init_structure.Pin       = GPIO_PIN_3 | GPIO_PIN_6;
  	  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  	  gpio_init_structure.Pull      = GPIO_NOPULL;
  	  gpio_init_structure.Speed     = GPIO_SPEED_FAST;
  	  gpio_init_structure.Alternate = GPIO_AF14_LTDC;
  	  HAL_GPIO_Init(GPIOD, &gpio_init_structure);

  	  // GPIOE LTDC AF14
  	  gpio_init_structure.Pin       = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;
  	  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  	  gpio_init_structure.Pull      = GPIO_NOPULL;
  	  gpio_init_structure.Speed     = GPIO_SPEED_FAST;
  	  gpio_init_structure.Alternate = GPIO_AF14_LTDC;
  	  HAL_GPIO_Init(GPIOE, &gpio_init_structure);

  	  // GPIOF LTDC AF14
  	  gpio_init_structure.Pin       = GPIO_PIN_10;
  	  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  	  gpio_init_structure.Pull      = GPIO_NOPULL;
  	  gpio_init_structure.Speed     = GPIO_SPEED_FAST;
  	  gpio_init_structure.Alternate = GPIO_AF14_LTDC;
  	  HAL_GPIO_Init(GPIOF, &gpio_init_structure);

  	  // GPIOG LTDC AF9
  	  gpio_init_structure.Pin       = GPIO_PIN_10;
  	  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  	  gpio_init_structure.Pull      = GPIO_NOPULL;
  	  gpio_init_structure.Speed     = GPIO_SPEED_FAST;
  	  gpio_init_structure.Alternate = GPIO_AF9_LTDC;
  	  HAL_GPIO_Init(GPIOG, &gpio_init_structure);

  	  // GPIOG LTDC AF14
  	  gpio_init_structure.Pin       = GPIO_PIN_6 | GPIO_PIN_7 |\
  			  	  	  	  	  	  	  GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13;
  	  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  	  gpio_init_structure.Pull      = GPIO_NOPULL;
  	  gpio_init_structure.Speed     = GPIO_SPEED_FAST;
  	  gpio_init_structure.Alternate = GPIO_AF14_LTDC;
  	  HAL_GPIO_Init(GPIOG, &gpio_init_structure);

  	  // GPIOH LTDC alternate configuration
  	  //
  	  // N/A
  	  //

  	  // GPIOI LTDC LTDC AF14
  	  gpio_init_structure.Pin       = GPIO_PIN_4;
  	  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  	  gpio_init_structure.Pull      = GPIO_NOPULL;
  	  gpio_init_structure.Speed     = GPIO_SPEED_FAST;
  	  gpio_init_structure.Alternate = GPIO_AF14_LTDC;
  	  HAL_GPIO_Init(GPIOI, &gpio_init_structure);
  
  	  // -----------------------------------------------------------------------------------------
  	  // GPIO pins
  	  // -----------------------------------------------------------------------------------------

  	  // LCD_DISP GPIO configuration
  	  gpio_init_structure.Pin       = GPIO_PIN_7;
  	  gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
  	  HAL_GPIO_Init(GPIOF, &gpio_init_structure);

  	  // LCD_BL_CTRL GPIO configuration
  	  gpio_init_structure.Pin       = GPIO_PIN_9;
  	  gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
  	  HAL_GPIO_Init(GPIOF, &gpio_init_structure);

#ifdef BOARD_MCHF
  	  // Assert display enable LCD_DISP pin - off
  	  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, GPIO_PIN_RESET);

  	  // Assert backlight LCD_BL_CTRL pin - off
  	  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_RESET);
#endif

  	  // Set LTDC Interrupt to the lowest priority
  	  HAL_NVIC_SetPriority(LTDC_IRQn, 0xE, 0);
  
  	  // Enable LTDC Interrupt
  	  HAL_NVIC_EnableIRQ(LTDC_IRQn);
}
#endif

#ifdef CHIP_H7
//
static void _DMA2D_ITConfig(U32 DMA2D_IT, int NewState)
{
  if (NewState != DISABLE) {
    DMA2D->CR |= DMA2D_IT;
  } else {
    DMA2D->CR &= (U32)~DMA2D_IT;
  }
}
//
void HAL_LTDC_MspInit(LTDC_HandleTypeDef *hltdc)
{
	GPIO_InitTypeDef gpio_init_structure;

#ifdef LCD_DRIVER_DEBUG
	printf("HAL_LTDC_MspInit\r\n");
#endif

	/* Enable the LTDC and DMA2D clocks */
	__HAL_RCC_LTDC_CLK_ENABLE();
	__HAL_RCC_DMA2D_CLK_ENABLE();

	  // Enable GPIOs clock
	  __HAL_RCC_GPIOA_CLK_ENABLE();
	  __HAL_RCC_GPIOB_CLK_ENABLE();
	  __HAL_RCC_GPIOC_CLK_ENABLE();
	  __HAL_RCC_GPIOD_CLK_ENABLE();
	  __HAL_RCC_GPIOE_CLK_ENABLE();
	  __HAL_RCC_GPIOF_CLK_ENABLE();
	  __HAL_RCC_GPIOG_CLK_ENABLE();
	  __HAL_RCC_GPIOI_CLK_ENABLE();

  	  // -------------------------------------------------------------------------------------------------
  	  // Multiplexer allocated pins, important - LTDC could use AF9,AF12 or AF14 !!!!
  	  // -------------------------------------------------------------------------------------------------
  	  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
	  gpio_init_structure.Pull      = GPIO_NOPULL;
	  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
	  // -------------------------------------------------------------------------------------------------
	  #ifndef PCB_REV_26JULY18
	  // GPIOA LTDC AF12 & AF14
  	  //
  	  //								LCD_R1(AF14)	LCD_B5(AF14)	LCD_VSYNC(AF14)	LCD_G2(AF14)
  	  //								LCD_R6(AF14)	LCD_R4(AF14)	LCD_R5(AF14)
	  gpio_init_structure.Pin       = 	GPIO_PIN_2 | 	GPIO_PIN_3  | 	GPIO_PIN_4  |	GPIO_PIN_6 |\
			  	  	  	  				GPIO_PIN_8 | 	GPIO_PIN_11 | 	GPIO_PIN_12;
	  #endif
	  #ifdef PCB_REV_26JULY18
	  // GPIOA LTDC AF12 & AF14
	  //
	  //								LCD_R2(AF14)	LCD_R1(AF14)	LCD_B5(AF14)	LCD_VSYNC(AF14)
	  //								LCD_G2(AF14)	LCD_R6(AF14)	LCD_R4(AF14)	LCD_R5(AF14)
	  gpio_init_structure.Pin       = 	GPIO_PIN_1 |	GPIO_PIN_2 | 	GPIO_PIN_3  | 	GPIO_PIN_4  |\
			  	  	  	  	  	  	  	GPIO_PIN_6 |	GPIO_PIN_8 | 	GPIO_PIN_11 | 	GPIO_PIN_12;
	  #endif
	  gpio_init_structure.Alternate = 	GPIO_AF14_LTDC;
	  HAL_GPIO_Init(GPIOA, &gpio_init_structure);
	  //								LCD_B4(AF12)
	  gpio_init_structure.Pin       = 	 GPIO_PIN_10;
	  gpio_init_structure.Alternate = 	GPIO_AF12_LTDC;
	  HAL_GPIO_Init(GPIOA, &gpio_init_structure);
	  // -------------------------------------------------------------------------------------------------
	  // GPIOB LTDC AF9 & AF14
	  //								LCD_R3(AF9)
	  gpio_init_structure.Pin       = 	GPIO_PIN_0;
	  gpio_init_structure.Alternate = 	GPIO_AF9_LTDC;
	  HAL_GPIO_Init(GPIOB, &gpio_init_structure);
	  //								LCD_B6(AF14)	LCD_B7(AF14)	LCD_G4(AF14)	LCD_G5(AF14)
	  gpio_init_structure.Pin       = 	GPIO_PIN_8  | 	GPIO_PIN_9  | 	GPIO_PIN_10 | 	GPIO_PIN_11;
	  gpio_init_structure.Alternate = 	GPIO_AF14_LTDC;
	  HAL_GPIO_Init(GPIOB, &gpio_init_structure);
	  // -------------------------------------------------------------------------------------------------
	  #ifndef PCB_REV_26JULY18
	  // GPIOC LTDC AF14
	  //								LCD_HSYNC(AF14)	LCD_G6(AF14)	LCD_R2(AF14)
	  gpio_init_structure.Pin       = 	GPIO_PIN_6  | 	GPIO_PIN_7  | 	GPIO_PIN_10;
	  #endif
	  #ifdef PCB_REV_26JULY18
	  // GPIOC LTDC AF14
	  //								LCD_HSYNC(AF14)	LCD_G6(AF14)
	  gpio_init_structure.Pin       = 	GPIO_PIN_6  | 	GPIO_PIN_7;
	  #endif
	  gpio_init_structure.Alternate =	GPIO_AF14_LTDC;
	  HAL_GPIO_Init(GPIOC, &gpio_init_structure);
	  // -------------------------------------------------------------------------------------------------
	  // GPIOD LTDC AF14
	  //								LCD_G7(AF14)	LCD_B2(AF14)
	  gpio_init_structure.Pin       = 	GPIO_PIN_3  | 	GPIO_PIN_6;
	  gpio_init_structure.Alternate = 	GPIO_AF14_LTDC;
	  HAL_GPIO_Init(GPIOD, &gpio_init_structure);
	  // -------------------------------------------------------------------------------------------------
	  // GPIOE LTDC AF14
	  //								LCD_B0(AF14)	LCD_G0(AF14)	LCD_G1(AF14)
	  gpio_init_structure.Pin       = 	GPIO_PIN_4  | 	GPIO_PIN_5  | 	GPIO_PIN_6;
	  gpio_init_structure.Alternate = 	GPIO_AF14_LTDC;
	  HAL_GPIO_Init(GPIOE, &gpio_init_structure);
	  // -------------------------------------------------------------------------------------------------
	  // GPIOF LTDC AF14
	  //								LCD_DE(AF14)
	  gpio_init_structure.Pin       = 	GPIO_PIN_10;
	  gpio_init_structure.Alternate = 	GPIO_AF14_LTDC;
	  HAL_GPIO_Init(GPIOF, &gpio_init_structure);
	  // -------------------------------------------------------------------------------------------------
	  // GPIOG LTDC AF9 & AF14
	  //								LCD_G3(AF9)
	  gpio_init_structure.Pin       = 	GPIO_PIN_10;
	  gpio_init_structure.Alternate = 	GPIO_AF9_LTDC;
	  HAL_GPIO_Init(GPIOG, &gpio_init_structure);
	  //								LCD_R7(AF14)	LCD_CLK(AF14)	LCD_B3(AF14)	LCD_B1(AF14)
	  //								LCD_R0(AF14)
	  gpio_init_structure.Pin       = 	GPIO_PIN_6  | 	GPIO_PIN_7 |	GPIO_PIN_11 | 	GPIO_PIN_12 |\
			  	  	  	  	  	  	  	GPIO_PIN_13;
	  gpio_init_structure.Alternate = 	GPIO_AF14_LTDC;
	  HAL_GPIO_Init(GPIOG, &gpio_init_structure);
	  // -----------------------------------------------------------------------------------------
	  // GPIO pins
	  // -----------------------------------------------------------------------------------------
	  // LCD_DISP GPIO configuration
	  gpio_init_structure.Pin       = GPIO_PIN_7;
	  gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
	  HAL_GPIO_Init(GPIOF, &gpio_init_structure);
	  // -------------------------------------------------------------------------------------------------
	  // LCD_BL_CTRL GPIO configuration
	  gpio_init_structure.Pin       = GPIO_PIN_9;
	  gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
	  HAL_GPIO_Init(GPIOF, &gpio_init_structure);
	  // -------------------------------------------------------------------------------------------------
	  // -------------------------------------------------------------------------------------------------

	  // Assert display enable LCD_DISP pin - off
	  GPIOF->BSRRH = GPIO_PIN_7;

	  // Assert backlight LCD_BL_CTRL pin - off
	  GPIOF->BSRRH = GPIO_PIN_9;

	  /* Set LTDC Interrupt to the lowest priority */
	  HAL_NVIC_SetPriority(LTDC_IRQn, 0xE, 0);
	  /* Enable LTDC Interrupt */
	  HAL_NVIC_EnableIRQ(LTDC_IRQn);

	  /* Set DMA2D Interrupt to the lowest priority */
	  HAL_NVIC_SetPriority(DMA2D_IRQn, 0xE, 0x0);

	  /* Enable DMA2D Interrupt */
	  HAL_NVIC_EnableIRQ(DMA2D_IRQn);

	  /* Enable DMA2D transfer complete Interrupt */
	  _DMA2D_ITConfig(DMA2D_CR_TCIE, ENABLE);

#ifdef LCD_DRIVER_DEBUG
	  printf("HAL_LTDC_MspInit ok\r\n");
#endif
}
#endif

/**
  * @brief LTDC MSP De-Initialization 
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  * @param hltdc: LTDC handle pointer
  * @retval None
  */
void HAL_LTDC_MspDeInit(LTDC_HandleTypeDef *hltdc)
{
  /* Reset peripherals */
  /* Enable LTDC reset state */
  __HAL_RCC_LTDC_FORCE_RESET();
  
  /* Release LTDC from reset state */ 
  __HAL_RCC_LTDC_RELEASE_RESET();
}

/**
  * @brief  Line Event callback.
  * @param  hltdc: pointer to a LTDC_HandleTypeDef structure that contains
  *                the configuration information for the specified LTDC.
  * @retval None
  */
void HAL_LTDC_LineEvenCallback(LTDC_HandleTypeDef *hltdc)
{
  U32 Addr;
  U32 layer;

  for (layer = 0; layer < GUI_NUM_LAYERS; layer++)
  {
    if (layer_prop[layer].pending_buffer >= 0) 
    {
      /* Calculate address of buffer to be used  as visible frame buffer */
      Addr = layer_prop[layer].address + \
             layer_prop[layer].xSize * layer_prop[layer].ySize * layer_prop[layer].pending_buffer * layer_prop[layer].BytesPerPixel;
      
      __HAL_LTDC_LAYER(hltdc, layer)->CFBAR = Addr;
     
      __HAL_LTDC_RELOAD_CONFIG(hltdc);
      
      /* Notify STemWin that buffer is used */
      GUI_MULTIBUF_ConfirmEx(layer, layer_prop[layer].pending_buffer);

      /* Clear pending buffer flag of layer */
      layer_prop[layer].pending_buffer = -1;
    }
  }
  
  // ------------------------------------------------------------------------------------------------------
  // ToDo: Check if relevant on H7 at all!
  //
  // https://community.st.com/s/question/0D50X00009XkfAPSAZ/spi-overrun?t=1551962415721
  //
  HAL_LTDC_ProgramLineEvent(hltdc, 0);
  //
  // Enable the Line interrupt
  //--__HAL_LTDC_ENABLE_IT(hltdc, LTDC_IT_LI);
  //
  // ------------------------------------------------------------------------------------------------------
}

/*******************************************************************************
                          Display configuration
*******************************************************************************/
/**
  * @brief  Called during the initialization process in order to set up the
  *          display driver configuration
  * @param  None
  * @retval None
  */
void LCD_X_Config(void) 
{
  U32 i;

#ifdef LCD_DRIVER_DEBUG
  printf("LCD_X_Config\r\n");
#endif

  LCD_LL_Init ();

  //GUI_SetOrientation(GUI_MIRROR_Y);
    
  /* At first initialize use of multiple buffers on demand */
#if (NUM_BUFFERS > 1)
    for (i = 0; i < GUI_NUM_LAYERS; i++) 
    {
      GUI_MULTIBUF_ConfigEx(i, NUM_BUFFERS);
    }
#endif

  /* Set display driver and color conversion for 1st layer */
  GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER_0, COLOR_CONVERSION_0, 0, 0);
  
  /* Set size of 1st layer */
  if (LCD_GetSwapXYEx(0)) {
    LCD_SetSizeEx (0, YSIZE_PHYS, XSIZE_PHYS);
    LCD_SetVSizeEx(0, YSIZE_PHYS * NUM_VSCREENS, XSIZE_PHYS);
  } else {
    LCD_SetSizeEx (0, XSIZE_PHYS, YSIZE_PHYS);
    LCD_SetVSizeEx(0, XSIZE_PHYS, YSIZE_PHYS * NUM_VSCREENS);
  }
  #if (GUI_NUM_LAYERS > 1)
    
    /* Set display driver and color conversion for 2nd layer */
    GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER_1, COLOR_CONVERSION_1, 0, 1);
    
    /* Set size of 2nd layer */
    if (LCD_GetSwapXYEx(1)) {
      LCD_SetSizeEx (1, YSIZE_PHYS, XSIZE_PHYS);
      LCD_SetVSizeEx(1, YSIZE_PHYS * NUM_VSCREENS, XSIZE_PHYS);
    } else {
      LCD_SetSizeEx (1, XSIZE_PHYS, YSIZE_PHYS);
      LCD_SetVSizeEx(1, XSIZE_PHYS, YSIZE_PHYS * NUM_VSCREENS);
    }
  #endif
  
    
    /*Initialize GUI Layer structure */
    layer_prop[0].address = LCD_LAYER0_FRAME_BUFFER;
    
#if (GUI_NUM_LAYERS > 1)    
    layer_prop[1].address = LCD_LAYER1_FRAME_BUFFER;
    //printf("layer one addr = %08x\r\n",LCD_LAYER1_FRAME_BUFFER);
#endif
       
   /* Setting up VRam address and LCD_LL functions for CopyBuffer-, CopyRect- and FillRect operations */
  for (i = 0; i < GUI_NUM_LAYERS; i++) 
  {

    layer_prop[i].pColorConvAPI = (LCD_API_COLOR_CONV *)apColorConvAPI[i];
     
    layer_prop[i].pending_buffer = -1;

    /* Set VRAM address */
    LCD_SetVRAMAddrEx(i, (void *)(layer_prop[i].address));

    /* Remember color depth for further operations */
    layer_prop[i].BytesPerPixel = LCD_GetBitsPerPixelEx(i) >> 3;

    /* Set LCD_LL functions for several operations */
    LCD_SetDevFunc(i, LCD_DEVFUNC_COPYBUFFER, (void(*)(void))LCD_LL_CopyBuffer);
    LCD_SetDevFunc(i, LCD_DEVFUNC_COPYRECT,   (void(*)(void))LCD_LL_CopyRect);
    LCD_SetDevFunc(i, LCD_DEVFUNC_FILLRECT, (void(*)(void))LCD_LL_FillRect);

    /* Set up drawing routine for 32bpp bitmap using DMA2D */
    if (LCD_LL_GetPixelformat(i) == LTDC_PIXEL_FORMAT_ARGB8888) {
     LCD_SetDevFunc(i, LCD_DEVFUNC_DRAWBMP_32BPP, (void(*)(void))LCD_LL_DrawBitmap32bpp);     /* Set up drawing routine for 32bpp bitmap using DMA2D. Makes only sense with ARGB8888 */
    }    
  }

#ifdef LCD_DRIVER_DEBUG
  printf("LCD_X_Config ok\r\n");
#endif
}

/**
  * @brief  This function is called by the display driver for several purposes.
  *         To support the according task the routine needs to be adapted to
  *         the display controller. Please note that the commands marked with
  *         'optional' are not cogently required and should only be adapted if
  *         the display controller supports these features
  * @param  LayerIndex: Index of layer to be configured 
  * @param  Cmd       :Please refer to the details in the switch statement below
  * @param  pData     :Pointer to a LCD_X_DATA structure
  * @retval Status (-1 : Error,  0 : Ok)
  */
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) 
{
  int r = 0;
  U32 addr;
  int xPos, yPos;
  U32 Color;

#ifdef LCD_DRIVER_DEBUG
  printf("LCD_X_DisplayDriver\r\n");
#endif

  switch (Cmd) 
  {
  case LCD_X_INITCONTROLLER: 
    LCD_LL_LayerInit(LayerIndex);
    break;

  case LCD_X_SETORG: 
    addr = layer_prop[LayerIndex].address + ((LCD_X_SETORG_INFO *)pData)->yPos * layer_prop[LayerIndex].xSize * layer_prop[LayerIndex].BytesPerPixel;
    HAL_LTDC_SetAddress(&hltdc, addr, LayerIndex);
    break;

  case LCD_X_SHOWBUFFER: 
    layer_prop[LayerIndex].pending_buffer = ((LCD_X_SHOWBUFFER_INFO *)pData)->Index;
    break;

  case LCD_X_SETLUTENTRY: 
    HAL_LTDC_ConfigCLUT(&hltdc, (uint32_t *)&(((LCD_X_SETLUTENTRY_INFO *)pData)->Color), 1, LayerIndex);
    break;

  case LCD_X_ON: 
    __HAL_LTDC_ENABLE(&hltdc);
    break;

  case LCD_X_OFF: 
    __HAL_LTDC_DISABLE(&hltdc);
    break;
    
  case LCD_X_SETVIS:
    if(((LCD_X_SETVIS_INFO *)pData)->OnOff  == ENABLE )
    {
      __HAL_LTDC_LAYER_ENABLE(&hltdc, LayerIndex); 
    }
    else
    {
      __HAL_LTDC_LAYER_DISABLE(&hltdc, LayerIndex); 
    }
    __HAL_LTDC_RELOAD_CONFIG(&hltdc); 
    break;
    
  case LCD_X_SETPOS: 
    HAL_LTDC_SetWindowPosition(&hltdc, 
                               ((LCD_X_SETPOS_INFO *)pData)->xPos, 
                               ((LCD_X_SETPOS_INFO *)pData)->yPos, 
                               LayerIndex);
    break;

  case LCD_X_SETSIZE:
    GUI_GetLayerPosEx(LayerIndex, &xPos, &yPos);
    layer_prop[LayerIndex].xSize = ((LCD_X_SETSIZE_INFO *)pData)->xSize;
    layer_prop[LayerIndex].ySize = ((LCD_X_SETSIZE_INFO *)pData)->ySize;
    HAL_LTDC_SetWindowPosition(&hltdc, xPos, yPos, LayerIndex);
    break;

  case LCD_X_SETALPHA:
    HAL_LTDC_SetAlpha(&hltdc, ((LCD_X_SETALPHA_INFO *)pData)->Alpha, LayerIndex);
    break;

  case LCD_X_SETCHROMAMODE:
    if(((LCD_X_SETCHROMAMODE_INFO *)pData)->ChromaMode != 0)
    {
      HAL_LTDC_EnableColorKeying(&hltdc, LayerIndex);
    }
    else
    {
      HAL_LTDC_DisableColorKeying(&hltdc, LayerIndex);      
    }
    break;

  case LCD_X_SETCHROMA:

    Color = ((((LCD_X_SETCHROMA_INFO *)pData)->ChromaMin & 0xFF0000) >> 16) |\
             (((LCD_X_SETCHROMA_INFO *)pData)->ChromaMin & 0x00FF00) |\
            ((((LCD_X_SETCHROMA_INFO *)pData)->ChromaMin & 0x0000FF) << 16);
    
    HAL_LTDC_ConfigColorKeying(&hltdc, Color, LayerIndex);
    break;

  default:
    r = -1;
  }

#ifdef LCD_DRIVER_DEBUG
  printf("LCD_X_DisplayDriver ok\r\n");
#endif

  return r;
}

/**
  * @brief  Initialise the LCD Controller.
  * @param  LayerIndex : layer Index.
  * @retval None
  */
static void LCD_LL_LayerInit(U32 LayerIndex) 
{
  LTDC_LayerCfgTypeDef   layer_cfg;

#ifdef LCD_DRIVER_DEBUG
  printf("LCD_LL_LayerInit\r\n");
#endif

  if (LayerIndex < GUI_NUM_LAYERS) 
  { 
    /* Layer configuration */
    layer_cfg.WindowX0 = 0;
    layer_cfg.WindowX1 = XSIZE_PHYS;
    layer_cfg.WindowY0 = 0;
    layer_cfg.WindowY1 = YSIZE_PHYS; 
    layer_cfg.PixelFormat = LCD_LL_GetPixelformat(LayerIndex);
    layer_cfg.FBStartAdress = layer_prop[LayerIndex].address;
    layer_cfg.Alpha = 255;
    layer_cfg.Alpha0 = 0;
    layer_cfg.Backcolor.Blue = 0;
    layer_cfg.Backcolor.Green = 0;
    layer_cfg.Backcolor.Red = 0;
    layer_cfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
    layer_cfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
    layer_cfg.ImageWidth = XSIZE_PHYS;
    layer_cfg.ImageHeight = YSIZE_PHYS;
    HAL_LTDC_ConfigLayer(&hltdc, &layer_cfg, LayerIndex);  
    
    /* Enable LUT on demand */
    if (LCD_GetBitsPerPixelEx(LayerIndex) <= 8) 
    {
      /* Enable usage of LUT for all modes with <= 8bpp*/
      HAL_LTDC_EnableCLUT(&hltdc, LayerIndex);
    } 
  } 

#ifdef LCD_DRIVER_DEBUG
  printf("LCD_LL_LayerInit ok\r\n");
#endif
}

/**
  * @brief  Initialize the LCD Controller.
  * @param  LayerIndex : layer Index.
  * @retval None
  */
#ifdef CHIP_F7
static void LCD_LL_Init(void) 
{ 
  /* DeInit */
  HAL_LTDC_DeInit(&hltdc);
  
  /* Set LCD Timings */
  hltdc.Init.HorizontalSync = (HSYNC - 1);
  hltdc.Init.VerticalSync = (VSYNC - 1);
  hltdc.Init.AccumulatedHBP = (HSYNC + HBP - 1);
  hltdc.Init.AccumulatedVBP = (VSYNC + VBP - 1);
  hltdc.Init.AccumulatedActiveH = (YSIZE_PHYS + VSYNC + VBP - 1);
  hltdc.Init.AccumulatedActiveW = (XSIZE_PHYS + HSYNC + HBP - 1);
  hltdc.Init.TotalHeigh = (YSIZE_PHYS + VSYNC + VBP + VFP - 1);
  hltdc.Init.TotalWidth = (XSIZE_PHYS + HSYNC + HBP + HFP - 1);
  
  /* background value */
  hltdc.Init.Backcolor.Blue = 0;
  hltdc.Init.Backcolor.Green = 0;
  hltdc.Init.Backcolor.Red = 0;  
  
  /* Polarity */
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL; 
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;  
  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hltdc.Instance = LTDC;
  
  HAL_LTDC_Init(&hltdc);
  HAL_LTDC_ProgramLineEvent(&hltdc, 0);
  
  /* Enable dithering */
  HAL_LTDC_EnableDither(&hltdc);
  
  /* Enable DMA2D */
  __HAL_RCC_DMA2D_CLK_ENABLE(); 

#ifdef BOARD_DISCO
  // Assert display enable LCD_DISP pin
  HAL_GPIO_WritePin(GPIOI, GPIO_PIN_12, GPIO_PIN_SET);

  // Assert backlight LCD_BL_CTRL pin
  HAL_GPIO_WritePin(GPIOK, GPIO_PIN_3, GPIO_PIN_SET);
#endif

#ifdef BOARD_MCHF
  // Assert display enable LCD_DISP pin
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7, GPIO_PIN_SET);

  // Assert backlight LCD_BL_CTRL pin
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_SET);
#endif

}
#endif

#ifdef CHIP_H7
#define  HSYNC            ((uint16_t)48)   	/* Horizontal synchronization */
#define  HBP              ((uint16_t)88)   	/* Horizontal back porch      */
#define  HFP              ((uint16_t)40)   	/* Horizontal front porch     */
#define  VSYNC            ((uint16_t)3)  	/* Vertical synchronization   */
#define  VBP              ((uint16_t)32)    /* Vertical back porch        */
#define  VFP              ((uint16_t)13)    /* Vertical front porch       */

static void LCD_LL_Init(void)
{
  static RCC_PeriphCLKInitTypeDef  periph_clk_init_struct;

#ifdef LCD_DRIVER_DEBUG
  printf("LCD_LL_Init\r\n");
#endif

  /* DeInit */
  HAL_LTDC_DeInit(&hltdc);

  /* Set LCD Timings */
  hltdc.Init.HorizontalSync 	= (HSYNC - 1);							//29;
  hltdc.Init.VerticalSync 		= (VSYNC - 1);							//2;
  hltdc.Init.AccumulatedHBP 	= (HSYNC + HBP - 1);					//143;
  hltdc.Init.AccumulatedVBP 	= (VSYNC + VBP - 1);					//34;
  hltdc.Init.AccumulatedActiveH = (YSIZE_PHYS + VSYNC + VBP - 1);		//514;
  hltdc.Init.AccumulatedActiveW = (XSIZE_PHYS + HSYNC + HBP - 1);		//783;
  hltdc.Init.TotalHeigh 		= (YSIZE_PHYS + VSYNC + VBP + VFP - 1);	//524;
  hltdc.Init.TotalWidth 		= (XSIZE_PHYS + HSYNC + HBP + HFP - 1);	//799;

  /* background value */
  hltdc.Init.Backcolor.Blue = 0;
  hltdc.Init.Backcolor.Green = 0;
  hltdc.Init.Backcolor.Red = 0;

#if 0
  //  LTDC Clock = 42.4 Mhz (unstable)
  periph_clk_init_struct.PLL3.PLL3M = 32;
  periph_clk_init_struct.PLL3.PLL3N = 85;
  periph_clk_init_struct.PLL3.PLL3R = 1;
#endif

#if 1
  //  LTDC Clock = 35 Mhz (seems stable)
  periph_clk_init_struct.PLL3.PLL3M = 32;
  periph_clk_init_struct.PLL3.PLL3N = 140;
  periph_clk_init_struct.PLL3.PLL3R = 2;
#endif

#if 0
  //  LTDC Clock = 21.25 Mhz (ok)
  periph_clk_init_struct.PLL3.PLL3M = 32;
  periph_clk_init_struct.PLL3.PLL3N = 85;
  periph_clk_init_struct.PLL3.PLL3R = 2;
#endif

#if 0
  //  LTDC Clock = 10.625 Mhz (ok)
  periph_clk_init_struct.PLL3.PLL3M = 32;
  periph_clk_init_struct.PLL3.PLL3N = 85;
  periph_clk_init_struct.PLL3.PLL3R = 4;
#endif

  // LTDC clock
  periph_clk_init_struct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  periph_clk_init_struct.PLL3.PLL3P = 2;
  periph_clk_init_struct.PLL3.PLL3Q = 2;
  HAL_RCCEx_PeriphCLKConfig(&periph_clk_init_struct);

  /* Polarity */
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hltdc.Instance = LTDC;

  HAL_LTDC_Init(&hltdc);
  HAL_LTDC_ProgramLineEvent(&hltdc, 0);

  /* Enable dithering */
  HAL_LTDC_EnableDither(&hltdc);

   /* Configure the DMA2D default mode */
  hdma2d.Init.Mode         = DMA2D_R2M;
  hdma2d.Init.ColorMode    = DMA2D_INPUT_ARGB8888;
  hdma2d.Init.OutputOffset = 0x0;

  hdma2d.Instance          = DMA2D;

  if(HAL_DMA2D_Init(&hdma2d) != HAL_OK)
  {
    //while (1);
  }

  // LCD on
  GPIOF->BSRRL = GPIO_PIN_7;
  GPIOF->BSRRL = GPIO_PIN_9;

#ifdef LCD_DRIVER_DEBUG
  printf("LCD_LL_Init ok\r\n");
#endif
}

#endif


/**
  * @brief  Return Pixel format for a given layer
  * @param  LayerIndex : Layer Index 
  * @retval Status ( 0 : 0k , 1: error)
  */
static void DMA2D_CopyBuffer(U32 LayerIndex, void * pSrc, void * pDst, U32 xSize, U32 ySize, U32 OffLineSrc, U32 OffLineDst)
{
  U32 PixelFormat;

	PixelFormat = LCD_LL_GetPixelformat(LayerIndex);
	DMA2D->CR      = 0x00000000UL | (1 << 9);

	// Set up pointers
	DMA2D->FGMAR   = (U32)pSrc;
	DMA2D->OMAR    = (U32)pDst;
	DMA2D->FGOR    = OffLineSrc;
	DMA2D->OOR     = OffLineDst;
  
	// Set up pixel format
	DMA2D->FGPFCCR = PixelFormat;
  
	//  Set up size
	DMA2D->NLR     = (U32)(xSize << 16) | (U16)ySize;
  
	DMA2D->CR     |= DMA2D_CR_START;
 
	// Wait until transfer is done
	while (DMA2D->CR & DMA2D_CR_START)
	{
	}
}

/**
  * @brief  Fill Buffer
  * @param  LayerIndex : Layer Index
  * @param  pDst:        pointer to destination
  * @param  xSize:       X size
  * @param  ySize:       Y size
  * @param  OffLine:     offset after each line
  * @param  ColorIndex:  color to be used.           
  * @retval None.
  */
static void DMA2D_FillBuffer(U32 LayerIndex, void * pDst, U32 xSize, U32 ySize, U32 OffLine, U32 ColorIndex) 
{
  U32 PixelFormat;

  PixelFormat = LCD_LL_GetPixelformat(LayerIndex);
	
  /* Set up mode */
  DMA2D->CR      = 0x00030000UL | (1 << 9);        
  DMA2D->OCOLR   = ColorIndex;                     

  /* Set up pointers */
  DMA2D->OMAR    = (U32)pDst;                      

  /* Set up offsets */
  DMA2D->OOR     = OffLine;                        

  /* Set up pixel format */
  DMA2D->OPFCCR  = PixelFormat;                    

  /*  Set up size */
  DMA2D->NLR     = (U32)(xSize << 16) | (U16)ySize;
    
  DMA2D->CR     |= DMA2D_CR_START; 
  
  /* Wait until transfer is done */
  while (DMA2D->CR & DMA2D_CR_START)
  {
  }
}


/**
  * @brief  Get buffer size
  * @param  LayerIndex : Layer Index           
  * @retval None.
  */
static U32 GetBufferSize(U32 LayerIndex) 
{
  U32 BufferSize;

  BufferSize = layer_prop[LayerIndex].xSize * layer_prop[LayerIndex].ySize * layer_prop[LayerIndex].BytesPerPixel;
  return BufferSize;
}

/**
  * @brief  LCD_LLized copy buffer
  * @param  LayerIndex : Layer Index
  * @param  IndexSrc:    index source
  * @param  IndexDst:    index destination           
  * @retval None.
  */
static void LCD_LL_CopyBuffer(int LayerIndex, int IndexSrc, int IndexDst)
{
  U32 BufferSize, AddrSrc, AddrDst;

  BufferSize = GetBufferSize(LayerIndex);
  AddrSrc    = layer_prop[LayerIndex].address + BufferSize * IndexSrc;
  AddrDst    = layer_prop[LayerIndex].address + BufferSize * IndexDst;
  DMA2D_CopyBuffer(LayerIndex, (void *)AddrSrc, (void *)AddrDst, layer_prop[LayerIndex].xSize, layer_prop[LayerIndex].ySize, 0, 0);
  layer_prop[LayerIndex].buffer_index = IndexDst;
}

//*----------------------------------------------------------------------------
//* Function Name       : LCD_LL_CopyRect
//* Object              : Copy rectangle
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
static void LCD_LL_CopyRect(int LayerIndex, int x0, int y0, int x1, int y1, int xSize, int ySize)
{
	U32 AddrSrc, AddrDst;
	int l_y0, l_y1;

	// Swapped LCD
	l_y0 = 480 - y0;
	l_y1 = 480 - y1;

	// Calculate source
	AddrSrc = layer_prop[LayerIndex].address + (l_y0 * layer_prop[LayerIndex].xSize + x0) * layer_prop[LayerIndex].BytesPerPixel;

	// Calculate destination
	AddrDst = layer_prop[LayerIndex].address + (l_y1 * layer_prop[LayerIndex].xSize + x1) * layer_prop[LayerIndex].BytesPerPixel;

	// Copy via DMA
	DMA2D_CopyBuffer(	LayerIndex,
						(void *)AddrSrc,
						(void *)AddrDst,
						xSize,
						ySize,
						(layer_prop[LayerIndex].xSize - xSize),
						(layer_prop[LayerIndex].xSize - xSize)
					);
}

/**
  * @brief  Fill rectangle
  * @param  LayerIndex : Layer Index
  * @param  x0:          X0 position
  * @param  y0:          Y0 position
  * @param  x1:          X1 position
  * @param  y1:          Y1 position
  * @param  PixelIndex:  Pixel index.             
  * @retval None.
  */
static void LCD_LL_FillRect(int LayerIndex, int x0, int y0, int x1, int y1, U32 PixelIndex) 
{
  U32 BufferSize, AddrDst;
  int xSize, ySize;
  
  if (GUI_GetDrawMode() == GUI_DM_XOR) 
  {		
    LCD_SetDevFunc(LayerIndex, LCD_DEVFUNC_FILLRECT, NULL);
    LCD_FillRect(x0, y0, x1, y1);
    LCD_SetDevFunc(LayerIndex, LCD_DEVFUNC_FILLRECT, (void(*)(void))LCD_LL_FillRect);
  } 
  else 
  {
    xSize = x1 - x0 + 1;
    ySize = y1 - y0 + 1;
    BufferSize = GetBufferSize(LayerIndex);
    AddrDst = layer_prop[LayerIndex].address + BufferSize * layer_prop[LayerIndex].buffer_index + (y0 * layer_prop[LayerIndex].xSize + x0) * layer_prop[LayerIndex].BytesPerPixel;
    DMA2D_FillBuffer(LayerIndex, (void *)AddrDst, xSize, ySize, layer_prop[LayerIndex].xSize - xSize, PixelIndex);
  }
}

/**
  * @brief  Draw indirect color bitmap
  * @param  pSrc: pointer to the source
  * @param  pDst: pointer to the destination
  * @param  OffSrc: offset source
  * @param  OffDst: offset destination
  * @param  PixelFormatDst: pixel format for destination
  * @param  xSize: X size
  * @param  ySize: Y size
  * @retval None
  */
static void LCD_LL_DrawBitmap32bpp(int LayerIndex, int x, int y, U8 const * p, int xSize, int ySize, int BytesPerLine)
{
	U32 BufferSize, AddrDst;
  int OffLineSrc, OffLineDst;

  BufferSize = GetBufferSize(LayerIndex);
  AddrDst = layer_prop[LayerIndex].address + BufferSize * layer_prop[LayerIndex].buffer_index + (y * layer_prop[LayerIndex].xSize + x) * layer_prop[LayerIndex].BytesPerPixel;
  OffLineSrc = (BytesPerLine / 4) - xSize;
  OffLineDst = layer_prop[LayerIndex].xSize - xSize;
  DMA2D_CopyBuffer(LayerIndex, (void *)p, (void *)AddrDst, xSize, ySize, OffLineSrc, OffLineDst);
}


