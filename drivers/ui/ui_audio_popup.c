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

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
{
  { FRAMEWIN_CreateIndirect, "Side Encoder Options", 0,       100,  100, 500, 300, FRAMEWIN_CF_MOVEABLE },
  { RADIO_CreateIndirect,     NULL,           GUI_ID_RADIO0,   5,  10,   0,   0,   0,  3 },
  { TEXT_CreateIndirect,     "Suspend",       GUI_ID_TEXT0,   25,  10,  70,  20, TEXT_CF_LEFT },
  { TEXT_CreateIndirect,     "Shut down",     GUI_ID_TEXT1,   25,  30,  70,  20, TEXT_CF_LEFT },
  { TEXT_CreateIndirect,     "Restart after", GUI_ID_TEXT2,   25,  50,  70,  20, TEXT_CF_LEFT },
  { TEXT_CreateIndirect,     "seconds",       GUI_ID_TEXT3,  130,  50,  70,  20, TEXT_CF_LEFT },
  { EDIT_CreateIndirect,     "200",           GUI_ID_EDIT0,   95,  47,  30,  19, 0, 3},
  { BUTTON_CreateIndirect,   "OK",            GUI_ID_OK,     180,  10,  60,  20 },
  { BUTTON_CreateIndirect,   "Cancel",        GUI_ID_CANCEL, 180,  40,  60,  20 }
};

WM_HWIN hWin = 0;

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

static void _cbCallback(WM_MESSAGE * pMsg)
{
	WM_HWIN hDlg;
	WM_HWIN hItem;
	int     Sel;
	int     NCode;
	int     Id;

	GUI_PID_STATE * pState;
	hDlg = pMsg->hWin;

	switch (pMsg->MsgId)
	{
    	case WM_INIT_DIALOG:
    	{
    		hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT0);
    		EDIT_SetDecMode(hItem, 30,   0, 999, 0, 0);    // Select decimal mode
    		WM_DisableWindow(hItem);
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

    	case WM_TOUCH_CHILD:
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
    	}

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
    				hItem = WM_GetDialogItem(hDlg, GUI_ID_RADIO0);
    				Sel   = RADIO_GetValue(hItem);
    				hItem = WM_GetDialogItem(hDlg, GUI_ID_EDIT0);
    				WM_SetEnableState(hItem, Sel == 2);
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
	WM_SetCallback(WM_HBKWIN, &_cbBkWindow);
	hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbCallback, 0, 0, 0);
}

void ui_audio_popup_destroy(void)
{
	//WM_SetCallback		(WM_HBKWIN, 0);
	//WM_InvalidateWindow	(WM_HBKWIN);
	if(hWin) WM_DeleteWindow(hWin);
}
