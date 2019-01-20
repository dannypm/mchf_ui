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

// Audio Popup Menu, invoked via side encoder push switch
// it has three sliders. Super ugly for now, sliders are
// click-able, aka Windows Mobile 2001, instead slide-able
// and responsive to gentle touch and slide ;(
//
// Also not glued to DSP functionality yet
//
// emWin resources are properly released on exit, so Desktop
// can repaint normally
//
// ToDo: Set controls default values(skin) as the Menu mode
//       will mess around with formatting of this pop up

#include "mchf_pro_board.h"

#include "ui_audio_popup.h"

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
  { FRAMEWIN_CreateIndirect, "Side Encoder Options", 	0,       			50,  	90, 	680, 	260, 	FRAMEWIN_CF_ACTIVE	 		},
  //
  { TEXT_CreateIndirect,     "Audio Gain" ,  			0,                	5,		5,  	100,  	20, 	TEXT_CF_LEFT 				},
  { EDIT_CreateIndirect,     NULL,     					GUI_ID_EDIT0,   	5,  	30,  	40,  	40, 	EDIT_CI_DISABELD,		3 	},
  { SLIDER_CreateIndirect,   NULL,     					GUI_ID_SLIDER0,		60, 	30, 	400,  	40 									},
  //
  { TEXT_CreateIndirect,     "RF Gain", 				0,              	5,  	80,  	100,  	20, 	TEXT_CF_LEFT 				},
  { EDIT_CreateIndirect,     NULL,     					GUI_ID_EDIT1,   	5,  	105,  	40,  	40, 	EDIT_CI_DISABELD,		3 	},
  { SLIDER_CreateIndirect,   NULL,     					GUI_ID_SLIDER1,  	60, 	105, 	400,  	40 									},
  //
  { TEXT_CreateIndirect,     "CW Keyer",		  		0,           		5,  	155,  	100,  	20, 	TEXT_CF_LEFT 				},
  { EDIT_CreateIndirect,     NULL,     					GUI_ID_EDIT2,   	5,  	180,  	40,  	40, 	EDIT_CI_DISABELD,		3 	},
  { SLIDER_CreateIndirect,   NULL,     					GUI_ID_SLIDER2,  	60, 	180, 	400,  	40 									},
  // Check boxes
  { CHECKBOX_CreateIndirect,"Checkbox", 				ID_CHECKBOX_0, 		480,  	190,	200, 	30, 	0, 				0x0, 	0 	},
  // List box
  { TEXT_CreateIndirect, 	"AGC Modes",	 			ID_TEXT_LIST_2,		480,	10,		180, 	20,  	0, 				0x0,	0 	},
  { LISTBOX_CreateIndirect, "Listbox", 					ID_LISTBOX_2, 		480, 	28, 	180, 	150, 	0, 				0x0, 	0 	},
};

WM_HWIN hWin = 0;

struct SLIDER_STATE	ss;

static void _OnValueChangedA(WM_MESSAGE *pMsg, int Id,int NCode)
{
	WM_HWIN hItem;
	WM_HWIN hSlider;
	WM_HWIN hEdit;

	ulong 	v = 0;
	int 	sel;
	char 	buf[50];

	switch(Id)
	{
		// AGC On/Off checkbox
		case ID_CHECKBOX_0:
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0);

			// Set DSP mode
			if(!CHECKBOX_GetState(hItem))
			{
				hw_dsp_eep_set_agc_mode(AGC_OFF);

				// RF gain gets focus
				hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_SLIDER1);
				WM_SetFocus(hItem);
			}
			else
			{
				hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_2);
				sel = LISTBOX_GetSel(hItem);
				if(sel != LISTBOX_ALL_ITEMS)
				{
					LISTBOX_GetItemText(hItem,sel,buf,sizeof(buf));
					//printf("lb0 text=%s\r\n",buf);

					switch(buf[0])
					{
						case 'S':
							hw_dsp_eep_set_agc_mode(AGC_SLOW);
							break;
						case 'F':
							hw_dsp_eep_set_agc_mode(AGC_FAST);
							break;
						case 'C':
							hw_dsp_eep_set_agc_mode(AGC_CUSTOM);
							break;
						default:
							hw_dsp_eep_set_agc_mode(AGC_MED);
							break;
					}
				}
				else
					hw_dsp_eep_set_agc_mode(AGC_MED);

				// Audio gain gets focus
				hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_SLIDER0);
				WM_SetFocus(hItem);
			}

			break;
		}

		// Audio gain slider
		case GUI_ID_SLIDER0:
		{
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
			hSlider = WM_GetDialogItem(pMsg->hWin, GUI_ID_SLIDER1);
			hEdit   = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT1);

			// Get slider position
			v = SLIDER_GetValue(hSlider);

			// Update Edit box
			EDIT_SetValue(hEdit, v);

			// Post to DSP
			hw_dsp_eep_update_rf_gain(v);

			// Update publics - needed ?
			ss.values[1] = v;

			break;
		}

		case ID_LISTBOX_2:
		{

			hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_2);
			sel = LISTBOX_GetSel(hItem);
			if(sel != LISTBOX_ALL_ITEMS)
			{
				LISTBOX_GetItemText(hItem,sel,buf,sizeof(buf));
				//printf("lb0 text=%s\r\n",buf);

				switch(buf[0])
				{
					case 'S':
						hw_dsp_eep_set_agc_mode(AGC_SLOW);
						break;
					case 'F':
						hw_dsp_eep_set_agc_mode(AGC_FAST);
						break;
					case 'C':
						hw_dsp_eep_set_agc_mode(AGC_CUSTOM);
						break;
					default:
						hw_dsp_eep_set_agc_mode(AGC_MED);
						break;
					}
				}
				else
					hw_dsp_eep_set_agc_mode(AGC_MED);

				// Audio gain gets focus
				hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_SLIDER0);
				WM_SetFocus(hItem);

				break;
			}

		default:
			break;
	}
	WM_InvalidateWindow(WM_GetClientWindow(pMsg->hWin));
}

// Foreground window handler
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

// Dialog handler
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

    			// Fix scroll
    			hItem = WM_GetDialogItem(hDlg, GUI_ID_SLIDER0 + i);
    			SLIDER_SetRange(hItem, 0, ss.ranges[i]);
    			SLIDER_SetValue(hItem,ss.values[i]);
    		}

    		// Init Checkbox
    		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0);
    		CHECKBOX_SetFont(hItem,&GUI_FontAvantGarde16);
    		CHECKBOX_SetText(hItem, "AGC ON");
    		CHECKBOX_SetState(hItem, 1);

    		// Init Listbox
    		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_LIST_2);
    		TEXT_SetFont(hItem,&GUI_FontAvantGarde16);
    		TEXT_SetBkColor(hItem,GUI_STCOLOR_LIGHTBLUE);
    		TEXT_SetTextColor(hItem,GUI_WHITE);
    		TEXT_SetTextAlign(hItem,TEXT_CF_HCENTER|TEXT_CF_VCENTER);
    		hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX_2);
    		LISTBOX_SetFont(hItem, &GUI_FontAvantGarde32);
    		LISTBOX_SetTextColor(hItem,LISTBOX_CI_UNSEL,GUI_STCOLOR_LIGHTBLUE);
    		LISTBOX_AddString(hItem, "MEDIUM");
    		LISTBOX_AddString(hItem, "SLOW");
    		LISTBOX_AddString(hItem, "FAST");
    		LISTBOX_AddString(hItem, "CUSTOM");
    		hScrollV = SCROLLBAR_CreateAttached(hItem, SCROLLBAR_CF_VERTICAL);
    		//--SCROLLBAR_SetValue(hItem,2);

    		// Set selected
    		hItem = WM_GetDialogItem(hDlg, GUI_ID_SLIDER0);
    		WM_SetFocus(hItem);

    		break;
    	}

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

    	/*case WM_TOUCH_CHILD:
    	{
    		Id = WM_GetId(pMsg->hWinSrc);      // Id of widget
    		switch (Id)
    		{
    			case GUI_ID_TEXT0:
    			case GUI_ID_TEXT1:
    			case GUI_ID_TEXT2:
    				pState = (GUI_PID_STATE *)((WM_MESSAGE *)pMsg->Data.p)->Data.p;
    				if (pState)
    				{
    					if (pState->Pressed)
    					{
    						WM_HWIN hRadio = WM_GetDialogItem(hDlg, GUI_ID_RADIO0);
    						RADIO_SetValue(hRadio, Id - GUI_ID_TEXT0);    	// Use the text beside the radio button to
    																		// set the value of the radio button
    					}
    				}
    				break;
    		}
    		break;
    	}*/

    	case WM_NOTIFY_PARENT:
    	{
    		Id    = WM_GetId(pMsg->hWinSrc);      // Id of widget
    		NCode = pMsg->Data.v;                 // Notification code
    		switch (NCode)
    		{
    			case WM_NOTIFICATION_RELEASED:      // React only if released
    			{
    				switch (Id)
    				{
    					case GUI_ID_OK:
    						GUI_EndDialog(hDlg, 0);
    						break;
    					case GUI_ID_CANCEL:
    						GUI_EndDialog(hDlg, 1);
    						break;
    				}
    				break;
    			}

    			case WM_NOTIFICATION_VALUE_CHANGED:
    				_OnValueChangedA(pMsg, Id,NCode);
    				break;
    			default:
    				break;
    		}
    		break;
    	}

    	default:
    		WM_DefaultProc(pMsg);
	}
}

static void ui_audio_popup_set_profile(void)
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

void ui_audio_popup_create(void)
{
	ss.active_id = 0;
	// Audio gain
	ss.values[0] = tsu.band[tsu.curr_band].volume;
	ss.ranges[0] = 17;
	// RF gain
	ss.values[1] = 50;
	ss.ranges[1] = 50;
	// Keyer speed
	ss.values[2] = 18;
	ss.ranges[2] = 60;

	ui_audio_popup_set_profile();

	WM_SetCallback(WM_HBKWIN, &_cbBkWindow);
	hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbCallback, 0, 0, 0);
}

void ui_audio_popup_destroy(void)
{
	//WM_SetCallback		(WM_HBKWIN, 0);
	//WM_InvalidateWindow	(WM_HBKWIN);
	if(hWin) WM_DeleteWindow(hWin);
}
