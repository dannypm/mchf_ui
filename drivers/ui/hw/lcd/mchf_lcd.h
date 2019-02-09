
#ifndef __MCHF_LCD_H
#define __MCHF_LCD_H
  
#include "mchf_pro_board.h"

#include "GUI.h"
#include "GUIDRV_Lin.h"

// Allow print
//#define LCD_DRIVER_DEBUG

//#undef  LCD_SWAP_XY
//#undef  LCD_MIRROR_Y
//#undef  LCD_SWAP_RB

//#define LCD_SWAP_XY  1
//#define LCD_MIRROR_Y 1
//#define LCD_SWAP_RB  0

#ifdef USE_INT_RAM
// ------------------------------------------------------------------------------------
#define XSIZE_PHYS 		 	64
#define YSIZE_PHYS 		 	64

#define NUM_BUFFERS  		1	 			// Number of multiple buffers to be used
#define NUM_VSCREENS 		1 				// Number of virtual screens to be used

#undef  GUI_NUM_LAYERS
#define GUI_NUM_LAYERS 		1
// ------------------------------------------------------------------------------------
#endif

#ifdef USE_SDRAM
// ------------------------------------------------------------------------------------
#define XSIZE_PHYS 		 	800
#define YSIZE_PHYS 		 	480

#define NUM_BUFFERS  		2 				// Number of multiple buffers to be used ToDo: 3 or 2 ?
#define NUM_VSCREENS 		1 				// Number of virtual screens to be used

#undef  GUI_NUM_LAYERS
#define GUI_NUM_LAYERS 		2
// ------------------------------------------------------------------------------------
#endif

// LCD Timings
#ifdef BOARD_DISCO
// -----------------------------------------------------------------------------
#define  HSYNC            ((uint16_t)41)   	/* Horizontal synchronization */
#define  HBP              ((uint16_t)13)   	/* Horizontal back porch      */
#define  HFP              ((uint16_t)32)   	/* Horizontal front porch     */
#define  VSYNC            ((uint16_t)10)   	/* Vertical synchronization   */
#define  VBP              ((uint16_t)2)    	/* Vertical back porch        */
#define  VFP              ((uint16_t)2)    	/* Vertical front porch       */
// -----------------------------------------------------------------------------
#endif

#ifdef BOARD_MCHF
// -----------------------------------------------------------------------------
#define  HSYNC            ((uint16_t)48)   	/* Horizontal synchronization */
#define  HBP              ((uint16_t)88)   	/* Horizontal back porch      */
#define  HFP              ((uint16_t)40)   	/* Horizontal front porch     */
#define  VSYNC            ((uint16_t)3)  	/* Vertical synchronization   */
#define  VBP              ((uint16_t)32)    /* Vertical back porch        */
#define  VFP              ((uint16_t)13)    /* Vertical front porch       */
// -----------------------------------------------------------------------------
#endif

#define BK_COLOR GUI_DARKBLUE

#ifdef BOARD_MCHF
#define COLOR_CONVERSION_0 GUICC_M8888I
#define DISPLAY_DRIVER_0   GUIDRV_LIN_OXY_32

// - Need to properly re-write all controls!
//#define COLOR_CONVERSION_0 GUICC_M1555I
//#define DISPLAY_DRIVER_0   GUIDRV_LIN_OXY_16

#if (GUI_NUM_LAYERS > 1)
  #define COLOR_CONVERSION_1 GUICC_M1555I
  #define DISPLAY_DRIVER_1   GUIDRV_LIN_OXY_16
#endif
#endif

#ifndef   XSIZE_PHYS
  #error Physical X size of display is not defined!
#endif
#ifndef   YSIZE_PHYS
  #error Physical Y size of display is not defined!
#endif
#ifndef   NUM_VSCREENS
  #define NUM_VSCREENS 1
#else
  #if (NUM_VSCREENS <= 0)
    #error At least one screeen needs to be defined!
  #endif
#endif
#if (NUM_VSCREENS > 1) && (NUM_BUFFERS > 1)
  #error Virtual screens and multiple buffers are not allowed!
#endif

#ifdef USE_INT_RAM
// ---------------------------------------------------
// Use chip internal ram
#ifdef CHIP_F7
#define LCD_LAYER0_FRAME_BUFFER  ((int)0x20000140)
#endif
#ifdef CHIP_H7
#define LCD_LAYER0_FRAME_BUFFER  ((int)0x30000000)
#endif
// ----------------------------------------------------
#endif

#define LAYER_MEM_REQUIRED			(XSIZE_PHYS * YSIZE_PHYS * 4)

#ifdef USE_SDRAM
#ifdef CHIP_F7
#define LCD_LAYER0_FRAME_BUFFER  	(0xC0000000 				+ EMWIN_RAM_SIZE		)
#define LCD_LAYER1_FRAME_BUFFER  	(LCD_LAYER0_FRAME_BUFFER 	+ LAYER_MEM_REQUIRED	)
#endif
#ifdef CHIP_H7
//#define LCD_LAYER0_FRAME_BUFFER  ((int)0xC0200000)
//#define LCD_LAYER1_FRAME_BUFFER  ((int)0xC0400000)
//
#define LCD_LAYER0_FRAME_BUFFER  	(0xC0000000 				+ EMWIN_RAM_SIZE		)
#define LCD_LAYER1_FRAME_BUFFER  	(LCD_LAYER0_FRAME_BUFFER 	+ LAYER_MEM_REQUIRED	)
#endif
#endif

typedef struct
{
	volatile int32_t      address;
	volatile int32_t      pending_buffer;
	volatile int32_t      buffer_index;

	int32_t      xSize;
	int32_t      ySize;
	int32_t      BytesPerPixel;

	LCD_API_COLOR_CONV   *pColorConvAPI;
} LCD_LayerPropTypedef;

#endif /* LCDCONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
