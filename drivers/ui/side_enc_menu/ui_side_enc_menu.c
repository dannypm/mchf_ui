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

// Side Encoder Options Menu, invoked via side encoder push switch
// it has three sliders, those are
// click-able, aka Windows Mobile 2001, instead slide-able
// and responsive to gentle touch and slide ;(
//
// emWin resources are properly released on exit, so Desktop
// can repaint normally
//

#include "mchf_pro_board.h"

#include "ui_side_enc_menu.h"

// DSP write request proxy
#include "dsp_eep\hw_dsp_eep.h"

#include "gui.h"
#include "dialog.h"
#include "ST_GUI_Addons.h"

// Public radio state
extern struct	TRANSCEIVER_STATE_UI	tsu;

#define ID_CHECKBOX_0				(GUI_ID_USER + 0x01)
#define ID_TEXT_LIST_2             	(GUI_ID_USER + 0x02)
#define ID_LISTBOX_2         		(GUI_ID_USER + 0x03)

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
{
  // -----------------------------------------------------------------------------------------------------------------------------
  //						name						id					x		y		xsize	ysize	?		?		?
  // -----------------------------------------------------------------------------------------------------------------------------
  { FRAMEWIN_CreateIndirect, "Side Encoder Options", 	0,       			80,  	110, 	640, 	260, 	FRAMEWIN_CF_ACTIVE	 		},
  // First slider
  { TEXT_CreateIndirect,     "Audio Gain" ,  			0,                	5,		5,  	100,  	20, 	TEXT_CF_LEFT 				},
  { EDIT_CreateIndirect,     NULL,     					GUI_ID_EDIT0,   	5,  	30,  	40,  	40, 	EDIT_CI_DISABELD,		3 	},
  { SLIDER_CreateIndirect,   NULL,     					GUI_ID_SLIDER0,		60, 	30, 	400,  	40 									},
  // Second slider
  { TEXT_CreateIndirect,     "RF Gain", 				0,              	5,  	80,  	100,  	20, 	TEXT_CF_LEFT 				},
  { EDIT_CreateIndirect,     NULL,     					GUI_ID_EDIT1,   	5,  	105,  	40,  	40, 	EDIT_CI_DISABELD,		3 	},
  { SLIDER_CreateIndirect,   NULL,     					GUI_ID_SLIDER1,  	60, 	105, 	400,  	40 									},
  // Third slider
  { TEXT_CreateIndirect,     "CW Keyer",		  		0,           		5,  	155,  	100,  	20, 	TEXT_CF_LEFT 				},
  { EDIT_CreateIndirect,     NULL,     					GUI_ID_EDIT2,   	5,  	180,  	40,  	40, 	EDIT_CI_DISABELD,		3 	},
  { SLIDER_CreateIndirect,   NULL,     					GUI_ID_SLIDER2,  	60, 	180, 	400,  	40 									},
  // List box
  { TEXT_CreateIndirect, 	"AGC Modes",	 			ID_TEXT_LIST_2,		480,	10,		140, 	20,  	0, 				0x0,	0 	},
  { LISTBOX_CreateIndirect, "Listbox", 					ID_LISTBOX_2, 		480, 	28, 	140, 	192, 	0, 				0x0, 	0 	},
};

WM_HWIN hWin = 0;

struct SLIDER_STATE	ss;

//*----------------------------------------------------------------------------
//* Function Name       : _cbControl
//* Object              : Control related events(touch, change value, etc)
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
static void _cbControl(WM_MESSAGE *pMsg, int Id,int NCode)
{
	WM_HWIN hItem;
	WM_HWIN hSlider;
	WM_HWIN hEdit;

	ulong 	v = 0;
	int 	sel;
	char 	buf[50];

	switch(Id)
	{
		// Audio gain slider
		case GUI_ID_SLIDER0:
		{
			if(NCode != WM_NOTIFICATION_VALUE_CHANGED)
				break;

			// Audio mute on, ignore events
			if(tsu.audio_mute_flag)
				break;

			hSlider = WM_GetDialogItem(pMsg->hWin, GUI_ID_SLIDER0);
			hEdit   = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT0);

			// Get slider position
			v = SLIDER_GetValue(hSlider);

			// Update Edit box
			EDIT_SetValue(hEdit, v);

			// Post to DSP
			hw_dsp_eep_update_audio_gain(v);

			// Update public volume
			tsu.band[tsu.curr_band].volume = v;

			// Save band info to eeprom
			save_band_info();

			// Update publics - needed ?
			ss.values[0] = v;

			break;
		}

		// RF gain slider
		case GUI_ID_SLIDER1:
		{
			if(NCode != WM_NOTIFICATION_VALUE_CHANGED)
				break;

			hSlider = WM_GetDialogItem(pMsg->hWin, GUI_ID_SLIDER1);
			hEdit   = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT1);

			// Get slider position
			v = SLIDER_GetValue(hSlider);

			// Update Edit box
			EDIT_SetValue(hEdit, v);

			tsu.rf_gain = v;

			// Post to DSP
			hw_dsp_eep_update_rf_gain(tsu.rf_gain);

			// Update publics - needed ?
			ss.values[1] = v;

			break;
		}

		case ID_LISTBOX_2:
		{
			if(NCode != WM_NOTIFICATION_SEL_CHANGED)
				break;

			hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_2);
			sel = LISTBOX_GetSel(hItem);
			if(sel != LISTBOX_ALL_ITEMS)
			{
				LISTBOX_GetItemText(hItem,sel,buf,sizeof(buf));
				//--printf("list item=%s\r\n",buf);

				switch(buf[0])
				{
					case 'O':
					{
						// Limit RF gain to prevent loud hiss
						tsu.rf_gain = 20;
						hw_dsp_eep_update_rf_gain(tsu.rf_gain);

						hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_SLIDER1);	// rf gain slider handle
						SLIDER_SetValue(hItem,20);								// reflect new RF gain value on screen
						WM_SetFocus(hItem);										// give focus to slider

						// AGC off
						tsu.agc_state = AGC_OFF;
						hw_dsp_eep_set_agc_mode(tsu.agc_state);

						// Save
						WRITE_EEPROM(EEP_AGC_STATE,tsu.agc_state);

						goto finished;
					}
					case 'S':
						tsu.agc_state = AGC_SLOW;
						break;
					case 'F':
						tsu.agc_state = AGC_FAST;
						break;
					case 'C':
						tsu.agc_state = AGC_CUSTOM;
						break;
					default:
						tsu.agc_state = AGC_MED;
						break;
					}
			}
			else
				tsu.agc_state = AGC_MED;

			// Update DSP value
			hw_dsp_eep_set_agc_mode(tsu.agc_state);

			// Save
			WRITE_EEPROM(EEP_AGC_STATE,tsu.agc_state);

			// Audio gain gets focus
			hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_SLIDER0);
			WM_SetFocus(hItem);

			break;
		}

		default:
			break;
	}

finished:
	WM_InvalidateWindow(WM_GetClientWindow(pMsg->hWin));
}

//*----------------------------------------------------------------------------
//* Function Name       : _cbBkWindow
//* Object              : Foreground window handler
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
static void _cbBkWindow(WM_MESSAGE* pMsg)
{
	switch (pMsg->MsgId)
	{
		case WM_PAINT:
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

//*----------------------------------------------------------------------------
//* Function Name       : _cbCallback
//* Object              : Dialog handler
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
static void _cbCallback(WM_MESSAGE * pMsg)
{
	WM_HWIN 			hDlg;
	WM_HWIN 			hItem;
	SCROLLBAR_Handle 	hScrollV;

	int     Sel;
	int     NCode;
	int     Id;
	int     i;

	GUI_PID_STATE * pState;
	hDlg = pMsg->hWin;

	switch (pMsg->MsgId)
	{
    	case WM_INIT_DIALOG:
    	{
    		for (i = 0; i < 3; i++)
    		{
    			// Fix edit box
    			hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT0 + i);
    			EDIT_SetDecMode(hItem, ss.values[i],   0, ss.ranges[i], 0, 0);
    			EDIT_SetFont(hItem,&GUI_FontAvantGarde16B);
    			EDIT_SetBkColor(hItem,EDIT_CI_ENABLED,GUI_STCOLOR_LIGHTBLUE);
    			EDIT_SetTextColor(hItem,EDIT_CI_ENABLED,GUI_WHITE);
    			EDIT_SetTextAlign(hItem,TEXT_CF_HCENTER|TEXT_CF_VCENTER);

    			// Fix scroll - can we make those not ugly ??
    			hItem = WM_GetDialogItem(hDlg, GUI_ID_SLIDER0 + i);
    			SLIDER_SetRange(hItem, 0, ss.ranges[i]);
    			SLIDER_SetValue(hItem,ss.values[i]);
    		}

    		// Init Listbox
    		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_LIST_2);
    		TEXT_SetFont(hItem,&GUI_FontAvantGarde16);
    		TEXT_SetBkColor(hItem,GUI_STCOLOR_LIGHTBLUE);
    		TEXT_SetTextColor(hItem,GUI_WHITE);
    		TEXT_SetTextAlign(hItem,TEXT_CF_HCENTER|TEXT_CF_VCENTER);
    		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_2);
    		LISTBOX_SetFont(hItem, &GUI_FontAvantGarde32);
    		LISTBOX_SetTextColor(hItem,LISTBOX_CI_UNSEL,GUI_STCOLOR_LIGHTBLUE);
    		LISTBOX_AddString(hItem, "SLOW");
    		LISTBOX_AddString(hItem, "MEDIUM");
    		LISTBOX_AddString(hItem, "FAST");
    		LISTBOX_AddString(hItem, "CUSTOM");
    		LISTBOX_AddString(hItem, "OFF");
    		LISTBOX_SetSel(hItem,tsu.agc_state);

    		// Set focus on audio slider at first paint
    		hItem = WM_GetDialogItem(hDlg, GUI_ID_SLIDER0);
    		WM_SetFocus(hItem);

    		break;
    	}

    	// ToDo: Implement proper keypad handling to allow to control
    	//       only from the keyboard, in case we have a faulty touch screen
    	case WM_KEY:
    	{
    		switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key)
    		{
    			case GUI_KEY_ESCAPE:
    				GUI_EndDialog(hDlg, 1);
    				break;
    			case GUI_KEY_ENTER:
    				GUI_EndDialog(hDlg, 0);
    				break;
    		}
    		break;
    	}

    	case WM_NOTIFY_PARENT:
    	{
    		Id    = WM_GetId(pMsg->hWinSrc);      // Id of widget
    		NCode = pMsg->Data.v;                 // Notification code

    		_cbControl(pMsg, Id,NCode);
    		break;
    	}

    	default:
    		WM_DefaultProc(pMsg);
	}
}

//*----------------------------------------------------------------------------
//* Function Name       : ui_side_enc_menu_set_profile
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
static void ui_side_enc_menu_set_profile(void)
{
	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
	DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);
	MENU_SetDefaultSkin(MENU_SKIN_FLEX);

	CHECKBOX_SetDefaultSkin(CHECKBOX_SKIN_FLEX);
	SPINBOX_SetDefaultSkin(SPINBOX_SKIN_FLEX);

    FRAMEWIN_SetDefaultTextColor(0, GUI_WHITE);
    FRAMEWIN_SetDefaultTextColor(1, GUI_WHITE);

    FRAMEWIN_SetDefaultBarColor(0, GUI_STCOLOR_LIGHTBLUE);
    FRAMEWIN_SetDefaultBarColor(1, GUI_STCOLOR_LIGHTBLUE);

    FRAMEWIN_SetDefaultFont(&GUI_FontAvantGarde20B);

    FRAMEWIN_SetDefaultClientColor(GUI_WHITE);
    FRAMEWIN_SetDefaultTitleHeight(25);

    WINDOW_SetDefaultBkColor(GUI_WHITE);

    LISTVIEW_SetDefaultGridColor(GUI_WHITE);
    LISTVIEW_SetDefaultBkColor(LISTVIEW_CI_SEL, GUI_STCOLOR_LIGHTBLUE);
    DROPDOWN_SetDefaultColor(DROPDOWN_CI_SEL, GUI_STCOLOR_LIGHTBLUE);
    LISTVIEW_SetDefaultBkColor(LISTVIEW_CI_SELFOCUS, GUI_STCOLOR_LIGHTBLUE);
    DROPDOWN_SetDefaultColor(DROPDOWN_CI_SELFOCUS, GUI_STCOLOR_LIGHTBLUE);

    HEADER_SetDefaultBkColor(GUI_STCOLOR_LIGHTBLUE);
    HEADER_SetDefaultTextColor(GUI_WHITE);
    HEADER_SetDefaultFont(GUI_FONT_16_1);
    HEADER_SetDefaultSTSkin();

    TREEVIEW_SetDefaultFont(GUI_FONT_32B_ASCII);

    ST_CHOOSEFILE_SetDelim('/');
    GUI_SetDefaultFont(GUI_FONT_20_ASCII);

    ST_CHOOSEFILE_SetButtonSize(40, 20);

  	TEXT_SetDefaultTextColor(GUI_STCOLOR_LIGHTBLUE);
  	TEXT_SetDefaultFont(&GUI_FontLubalGraph20);

  	// List box defaults
  	LISTBOX_SetDefaultScrollMode(LISTBOX_CF_AUTOSCROLLBAR_V);

  	// Need extra large buttons for Clock menu
  	SPINBOX_SetDefaultButtonSize(64);

  	// Allow scroll bar on a list box
  	SCROLLBAR_SetDefaultColor(GUI_STCOLOR_LIGHTBLUE, SCROLLBAR_CI_THUMB|SCROLLBAR_CI_SHAFT|SCROLLBAR_CI_ARROW);
  	SCROLLBAR_SetDefaultWidth(50);
  	//--SCROLLBAR_SetDefaultSTSkin();

  	//RADIO_SetDefaultFont(&GUI_FontAvantGarde16);
  	//RADIO_SetDefaultFocusColor(GUI_STCOLOR_LIGHTBLUE);
  	//RADIO_SetDefaultTextColor(GUI_STCOLOR_LIGHTBLUE);

  	SLIDER_SetDefaultBkColor   (GUI_STCOLOR_LIGHTBLUE);
  	SLIDER_SetDefaultBarColor  (GUI_STCOLOR_LIGHTBLUE);
  	SLIDER_SetDefaultFocusColor(GUI_STCOLOR_LIGHTBLUE);
  	SLIDER_SetDefaultTickColor (GUI_STCOLOR_LIGHTBLUE);
}

//*----------------------------------------------------------------------------
//* Function Name       : ui_side_enc_menu_create
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void ui_side_enc_menu_create(void)
{
	ss.active_id = 0;
	// Audio gain
	ss.values[0] = tsu.band[tsu.curr_band].volume;
	ss.ranges[0] = 17;
	// RF gain
	ss.values[1] = tsu.rf_gain;
	ss.ranges[1] = 50;
	// Keyer speed
	ss.values[2] = 18;
	ss.ranges[2] = 60;

	ui_side_enc_menu_set_profile();

	WM_SetCallback(WM_HBKWIN, &_cbBkWindow);
	hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbCallback, 0, 0, 0);
}

//*----------------------------------------------------------------------------
//* Function Name       : ui_side_enc_menu_destroy
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void ui_side_enc_menu_destroy(void)
{
	//WM_SetCallback		(WM_HBKWIN, 0);
	//WM_InvalidateWindow	(WM_HBKWIN);
	if(hWin) WM_DeleteWindow(hWin);
}
