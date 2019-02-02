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

#include "gui.h"
#include "dialog.h"
#include "ST_GUI_Addons.h"

#include "C:\Projects\mcHFx\firmware\mchf_ui\hw\watchdog\watchdog.h"

// Externally declared s-meter bmp
//extern GUI_CONST_STORAGE GUI_BITMAP bmscale;

// Speaker icon in C file as binary resource
//extern GUI_CONST_STORAGE GUI_BITMAP bmtechrubio;

// Core unique regs loaded to RAM
extern struct	CM7_CORE_DETAILS	ccd;
// Radio state
extern struct	TRANSCEIVER_STATE_UI	tsu;

#ifdef CHIP_F7
#include "stm32f7xx_hal_rcc.h"
#endif
#ifdef CHIP_H7
#include "stm32h7xx_hal_rcc.h"
#endif

//*----------------------------------------------------------------------------
//* Function Name       : ui_startup
//* Object              : show bootup screen
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_startup(void)
{
	uint32_t XPos = 80, XOffset = 3,curr_y = 0;

	char start_logo[200],buf[80];
	char *p = start_logo;

	// -----------------------------------------------------------------------------------------------
	EnterCriticalSection();
	//
	memset(start_logo,0,sizeof(start_logo));
	memset(buf,0,sizeof(buf));
	strcpy(p,DEVICE_STRING);
	p += strlen(DEVICE_STRING);
	sprintf(buf," v %d.%d.%d.%d",MCHFX_VER_MAJOR,MCHFX_VER_MINOR,MCHFX_VER_RELEASE,MCHFX_VER_BUILD);
	strcpy(p,buf);
	p += strlen(buf);
	strcpy(p,", ");
	p += 2;
	strcpy(p,AUTHOR_STRING);
	p += strlen(AUTHOR_STRING);
	memset(buf,0,sizeof(buf));
	//
	ExitCriticalSection();
	// -----------------------------------------------------------------------------------------------

	p = start_logo;

	GUI_SelectLayer(1);
	GUI_SetBkColor(GUI_WHITE);
	GUI_Clear();

#if 0
	GUI_SetAlpha(128);
	GUI_DrawBitmapMag(&bmtechrubio,500,50,4,4);
	GUI_DrawBitmap(&bmscale,400,200);
	GUI_SetAlpha(0);
#endif

	GUI_SetFont(&GUI_FontAvantGarde24);
	GUI_SetColor(GUI_STCOLOR_DARKBLUE);

	// CPU type - ToDo: fix this, just a wild guess
	if(ccd.wafer_coord == 0x400026)
		GUI_DispStringAt(" - CPU is STM32F7xxx", 5,curr_y);
	else
		GUI_DispStringAt(" - CPU is STM32H7xxx", 5,curr_y);

	curr_y += 24;

	// Flash size
	EnterCriticalSection();
	sprintf(buf," - Flash size is %dkB",ccd.fls_size);
	ExitCriticalSection();
	GUI_DispStringAt(buf, 5,curr_y);

	curr_y += 24;

	// Main clock - could be improved
	if(__HAL_RCC_GET_PLL_OSCSOURCE() == RCC_PLLSOURCE_HSI)
		GUI_DispStringAt(" - Main clock using internal RC ocs", 5,curr_y);
	if(__HAL_RCC_GET_PLL_OSCSOURCE() == RCC_PLLSOURCE_HSE)
		GUI_DispStringAt(" - Main clock using external XTAL or TCXO", 5,curr_y);

	curr_y += 24;

	// RTC clock
	if(tsu.rcc_clk == EXT_32KHZ_XTAL)
		GUI_DispStringAt(" - RTC using external 32 kHz XTAL", 5,curr_y);
	else
		GUI_DispStringAt(" - RTC using internal osc, clock will be off without power!", 5,curr_y);
  
	curr_y += 24;
	GUI_DispStringAt("Drivers in use:", 5,curr_y);

	#ifdef CONTEXT_DRIVER_UI
	curr_y += 24;
	GUI_DispStringAt(" - User Interface(UI)", 5,curr_y);
	#endif
	#ifdef CONTEXT_DRIVER_KEYPAD
	curr_y += 24;
	GUI_DispStringAt(" - Keypad Input/LEDs", 5,curr_y);
	#endif
	#ifdef CONTEXT_DRIVER_DIGITIZER
	curr_y += 24;
	GUI_DispStringAt(" - Touch Panel Input", 5,curr_y);
	#endif
	#ifdef CONTEXT_DRIVER_KEYPAD_LED
	curr_y += 24;
	GUI_DispStringAt(" - Keypad LEDs(old)", 5,curr_y);
	#endif
	#ifdef CONTEXT_DRIVER_API
	curr_y += 24;
	GUI_DispStringAt(" - DSP API Interface", 5,curr_y);
	#endif
	#ifdef CONTEXT_ROTARY
	curr_y += 24;
	GUI_DispStringAt(" - Rotary Encoders Input", 5,curr_y);
	#endif
	#ifdef CONTEXT_SD
	curr_y += 24;
	GUI_DispStringAt(" - SD Card and File System", 5,curr_y);
	#endif
	#ifdef CONTEXT_NET
	curr_y += 24;
	GUI_DispStringAt(" - Internet interface", 5,curr_y);
	#endif

	// Firmware ID print
	do
	{
		//watchdog_refresh();

		GUI_DispCharAt(*p, XPos , 450);
		XPos += GUI_GetCharDistX(*p++) + XOffset + 1;
		if(*p == ',')
		{
			GUI_SetColor(GUI_STCOLOR_LIGHTBLUE);
			XOffset = 0;
		}
		OsDelayMs(30);
	}while (*p != 0);

	// - delay here causes crash on H7
	//
	// LCD boot-up delay
	OsDelayMs(BOOT_UP_LCD_DELAY);
}
