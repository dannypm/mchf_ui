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

#include "ui_controls_band.h"
#include "desktop\ui_controls_layout.h"

#include "desktop\freq\ui_controls_frequency.h"

uchar loc_band = 0;

// Public radio state
extern struct	TRANSCEIVER_STATE_UI	tsu;

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_band_init(void)
{
	GUI_SetColor(GUI_GRAY);
	GUI_FillRoundedRect((BAND_X + 0),(BAND_Y + 0),(BAND_X + 102),(BAND_Y + 19),2);
	GUI_SetFont(&GUI_Font20_1);
	GUI_SetColor(GUI_WHITE);
	GUI_DispStringAt("Band",(BAND_X + 4),(BAND_Y + 0));
	GUI_SetColor(GUI_WHITE);
	GUI_FillRoundedRect((BAND_X + 50),(BAND_Y + 2),(BAND_X + 100),(BAND_Y + 17),2);
	GUI_SetColor(GUI_GRAY);
	GUI_DispStringAt("40m",(BAND_X + 58),(BAND_Y + 0));
}

//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
void ui_controls_band_quit(void)
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
void ui_controls_band_touch(void)
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
void ui_controls_band_refresh(void)
{
	// Does keypad driver detected band change
	// and change public flag ?
	if(loc_band == tsu.curr_band)
		return;

	// -----------------------------------------------------------
	// This will cause API driver to send request for band change to DSP
	tsu.update_band_dsp_req = 1;

	// -----------------------------------------------------------
	// Update main oscillator frequency on band change
	tsu.update_freq_dsp_req = 1;

	// -----------------------------------------------------------
	// NCO value in the DSP needs update(always)
	tsu.update_nco_dsp_req = 1;

	// -----------------------------------------------------------
	// Update audio volume(band specific)
	tsu.update_audio_dsp_req = 1;

	// -----------------------------------------------------------
	// Controls that need complete repaint - call them directly
	// the rest - auto update via public flags (actually the tsu.curr_band index)
	ui_controls_frequency_change_band();
	// ...

	GUI_SetColor(GUI_WHITE);
	GUI_FillRoundedRect((BAND_X + 50),(BAND_Y + 2),(BAND_X + 100),(BAND_Y + 17),2);

	GUI_SetColor(GUI_GRAY);
	GUI_SetFont(&GUI_Font20_1);

	switch(tsu.curr_band)
	{
		case BAND_MODE_80:
			GUI_DispStringAt("80m",(BAND_X + 58),(BAND_Y + 0));
			break;
		case BAND_MODE_60:
			GUI_DispStringAt("60m",(BAND_X + 58),(BAND_Y + 0));
			break;
		case BAND_MODE_40:
			GUI_DispStringAt("40m",(BAND_X + 58),(BAND_Y + 0));
			break;
		case BAND_MODE_30:
			GUI_DispStringAt("30m",(BAND_X + 58),(BAND_Y + 0));
			break;
		case BAND_MODE_20:
			GUI_DispStringAt("20m",(BAND_X + 58),(BAND_Y + 0));
			break;
		case BAND_MODE_17:
			GUI_DispStringAt("17m",(BAND_X + 58),(BAND_Y + 0));
			break;
		case BAND_MODE_15:
			GUI_DispStringAt("15m",(BAND_X + 58),(BAND_Y + 0));
			break;
		case BAND_MODE_12:
			GUI_DispStringAt("12m",(BAND_X + 58),(BAND_Y + 0));
			break;
		case BAND_MODE_10:
			GUI_DispStringAt("10m",(BAND_X + 58),(BAND_Y + 0));
			break;
		default:
			break;
	}
	loc_band = tsu.curr_band;

	WRITE_EEPROM(EEP_CURR_BAND,tsu.curr_band);
}
