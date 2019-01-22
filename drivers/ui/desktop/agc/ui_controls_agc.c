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

#include "ui_controls_agc.h"
#include "desktop\ui_controls_layout.h"

// ToDo: fully implement control...

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_agc_init(void)
{
	GUI_SetColor(GUI_GRAY);
	GUI_FillRoundedRect((AGC_X + 0),(AGC_Y + 0),(AGC_X + 102),(AGC_Y + 19),2);
	GUI_SetFont(&GUI_Font20_1);
	GUI_SetColor(GUI_WHITE);
	GUI_DispStringAt("AGC",(AGC_X + 2),(AGC_Y + 0));
	GUI_SetColor(GUI_WHITE);
	GUI_FillRoundedRect((AGC_X + 43),(AGC_Y + 2),(AGC_X + 100),(AGC_Y + 17),2);
	GUI_SetColor(GUI_GRAY);

	//GUI_DispStringAt("OFF",(AGC_X + 51),(AGC_Y + 0));
	GUI_DispStringAt("SLOW",(AGC_X + 45),(AGC_Y + 0));
	//GUI_DispStringAt("MID",(AGC_X + 51),(AGC_Y + 0));
	//GUI_DispStringAt("FAST",(AGC_X + 51),(AGC_Y + 0));
	//GUI_DispStringAt("CUST",(AGC_X + 51),(AGC_Y + 0));
}

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_agc_quit(void)
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
void ui_controls_agc_touch(void)
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
void ui_controls_agc_refresh(void)
{

}
