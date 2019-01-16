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

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
{
  { FRAMEWIN_CreateIndirect, "Side Encoder Options", 	0,       			150,  	90, 	500, 	300, 	FRAMEWIN_CF_ACTIVE	 		},
  //
  { TEXT_CreateIndirect,     "Audio Gain" ,  			0,                	5,		30,  	70,  	20, 	TEXT_CF_LEFT 				},
  { EDIT_CreateIndirect,     NULL,     					GUI_ID_EDIT0,   	5,  	50,  	40,  	20, 	EDIT_CI_DISABELD,		3 	},
  { SLIDER_CreateIndirect,   NULL,     					GUI_ID_SLIDER0,		80, 	30, 	400,  	40 									},
  //
  { TEXT_CreateIndirect,     "RF Gain", 				0,              	5,  	100,  	70,  	20, 	TEXT_CF_LEFT 				},
  { EDIT_CreateIndirect,     NULL,     					GUI_ID_EDIT1,   	5,  	120,  	40,  	20, 	EDIT_CI_DISABELD,		3 	},
  { SLIDER_CreateIndirect,   NULL,     					GUI_ID_SLIDER1,  	80, 	100, 	400,  	40 									},
  //
  { TEXT_CreateIndirect,     "CW Keyer",		  		0,           		5,  	170,  	70,  	20, 	TEXT_CF_LEFT 				},
  { EDIT_CreateIndirect,     NULL,     					GUI_ID_EDIT2,   	5,  	190,  	40,  	20, 	EDIT_CI_DISABELD,		3 	},
  { SLIDER_CreateIndirect,   NULL,     					GUI_ID_SLIDER2,  	80, 	170, 	400,  	40 									},
};

WM_HWIN hWin = 0;

//static U8 _aColorSep[3] = {0, 127, 255};  // Red, green and blue components

struct SLIDER_STATE	ss;

// Sliders/Edits handler
static void _OnValueChanged(WM_HWIN hDlg, int Id)
{
  unsigned Index;
  unsigned v;
  WM_HWIN  hSlider;
  WM_HWIN  hEdit;

  Index = 0;
  v     = 0;

  	  // Update DSP with screen slider value (in turn controlled by the rotary driver via keypad message(left/right))
    //
    // ToDo: sync with Desktop mode (band publics in local eeprom, etc),on load and on exit values..
    //
  	if(Id == GUI_ID_SLIDER0)
	{
  		hSlider = WM_GetDialogItem(hDlg, GUI_ID_SLIDER0 + Index);
  		v = SLIDER_GetValue(hSlider);

  		// Post to DSP
  		hw_dsp_eep_update_audio_gain(v);
	}

  	// Example stuff, remove...
  if ((Id >= GUI_ID_SLIDER0) && (Id <= GUI_ID_SLIDER2))
  {
    Index = Id - GUI_ID_SLIDER0;
    //
    // SLIDER-widget has changed, update EDIT-widget
    //
    hSlider = WM_GetDialogItem(hDlg, GUI_ID_SLIDER0 + Index);
    hEdit   = WM_GetDialogItem(hDlg, GUI_ID_EDIT0 + Index);
    v = SLIDER_GetValue(hSlider);
    EDIT_SetValue(hEdit, v);
  } else if ((Id >= GUI_ID_EDIT0) && (Id <= GUI_ID_EDIT2)) {
    Index = Id - GUI_ID_EDIT0;
    //
    // If EDIT-widget has changed, update SLIDER-widget
    //
    hSlider = WM_GetDialogItem(hDlg, GUI_ID_SLIDER0 + Index);
    hEdit   = WM_GetDialogItem(hDlg, GUI_ID_EDIT0 + Index);
    v = EDIT_GetValue(hEdit);
    SLIDER_SetValue(hSlider, v);
  }
  //_aColorSep[Index] = v;
  ss.values[Index] = v;
  //
  // At last invalidate dialog client window
  //
  WM_InvalidateWindow(WM_GetClientWindow(hDlg));
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
	WM_HWIN hDlg;
	WM_HWIN hItem;
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
    			hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT0 + i);
    			EDIT_SetDecMode(hItem, ss.values[i],   0, ss.ranges[i], 0, 0);

    			hItem = WM_GetDialogItem(hDlg, GUI_ID_SLIDER0 + i);
    			SLIDER_SetRange(hItem, 0, ss.ranges[i]);
    			SLIDER_SetValue(hItem,ss.values[i]);

    			// ToDo: Make them all beautiful..
    		}

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
    			{
    				_OnValueChanged(hDlg, Id);
    				break;
    			}
    		}
    		break;
    	}

    	default:
    		WM_DefaultProc(pMsg);
	}
}

void ui_audio_popup_create(void)
{
	ss.active_id = 0;
	ss.values[0] = 0;
	ss.values[1] = 0;
	ss.values[2] = 0;
	ss.ranges[0] = 17;
	ss.ranges[1] = 50;
	ss.ranges[2] = 60;

	WM_SetCallback(WM_HBKWIN, &_cbBkWindow);
	hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbCallback, 0, 0, 0);
}

void ui_audio_popup_destroy(void)
{
	//WM_SetCallback		(WM_HBKWIN, 0);
	//WM_InvalidateWindow	(WM_HBKWIN);
	if(hWin) WM_DeleteWindow(hWin);
}
