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

#include "ui_controls_clock.h"
#include "ui_controls_layout.h"

#ifdef CHIP_F7
#include "stm32f7xx_hal_rtc.h"
#endif
#ifdef CHIP_H7
#include "stm32h7xx_hal_rtc.h"
#endif

RTC_DateTypeDef sdatestructureget;
RTC_TimeTypeDef stimestructureget;

extern RTC_HandleTypeDef RtcHandle;

uchar time_skip = 0;

uchar loc_seconds = 0;
uchar loc_minutes = 0;

static void ui_controls_clock_reload_time(void)
{
	// -------------------------------------------------------------------
	// -- Read both, keep order!!!! Some ST bug relating to read of RTC --
	HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, RTC_FORMAT_BIN);
	// -------------------------------------------------------------------

	//printf("%02d:%02d:%2d\r\n", stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
}

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_clock_init(void)
{

	time_skip = 0;
	loc_seconds = 0;
	loc_minutes = 0;

	GUI_SetColor(GUI_WHITE);
	GUI_SetFont(&GUI_Font8x16_1);
	GUI_DispStringAt("UTC",(CLOCK_X - 28),(CLOCK_Y + 0));
	GUI_DispStringAt("00:00",(CLOCK_X + 0),(CLOCK_Y + 0));
	GUI_SetFont(&GUI_Font8x8_1);
	GUI_DispStringAt("00",(CLOCK_X + 45),(CLOCK_Y + 5));
}

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_clock_quit(void)
{

}

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_clock_touch(void)
{

}

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_clock_refresh(void)
{
	char buf[20];

	time_skip++;

	// Adjust skip interval, so we don't miss seconds
	if(time_skip < 4)
		return;

	time_skip = 0;

	// Dump state from RTC
	ui_controls_clock_reload_time();

	// Update seconds
	if(loc_seconds != stimestructureget.Seconds)
	{
		// Clear seconds area
		GUI_SetColor(GUI_BLACK);
		GUI_FillRect((CLOCK_X + 45),(CLOCK_Y + 4),(CLOCK_X + 60),(CLOCK_Y + 11));

		EnterCriticalSection();
		sprintf(buf,"%02d",stimestructureget.Seconds);
		ExitCriticalSection();

		// Update seconds area
		GUI_SetColor(GUI_WHITE);
		GUI_SetFont(&GUI_Font8x8_1);
		GUI_DispStringAt(buf,(CLOCK_X + 45),(CLOCK_Y + 5));

		// Save to local
		loc_seconds = stimestructureget.Seconds;
	}

	// Update seconds
	if(loc_minutes != stimestructureget.Minutes)
	{
		// Clear hour/min area
		GUI_SetColor(GUI_BLACK);
		GUI_FillRect((CLOCK_X + 0),(CLOCK_Y + 0),(CLOCK_X + 39),(CLOCK_Y + 12));

		EnterCriticalSection();
		sprintf(buf,"%02d:%02d",stimestructureget.Hours,stimestructureget.Minutes);
		ExitCriticalSection();

		GUI_SetColor(GUI_WHITE);
		GUI_SetFont(&GUI_Font8x16_1);
		GUI_DispStringAt(buf,(CLOCK_X + 0),(CLOCK_Y + 0));

		// Save to local
		loc_minutes = stimestructureget.Minutes;
	}
}



