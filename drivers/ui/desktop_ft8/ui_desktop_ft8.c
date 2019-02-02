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

#include "ui_desktop_ft8.h"

// DSP write request proxy
//#include "dsp_eep\hw_dsp_eep.h"

#include "gui.h"
#include "dialog.h"
#include "ST_GUI_Addons.h"

#include "k_rtc.h"

// Public radio state
//extern struct	TRANSCEIVER_STATE_UI	tsu;

#define ID_WINDOW_0               	(GUI_ID_USER + 0x00)

#define ID_TEXT_LIST1             	(GUI_ID_USER + 0x01)
#define ID_LISTBOX1	        		(GUI_ID_USER + 0x02)
#define ID_TEXT_LIST2             	(GUI_ID_USER + 0x03)
#define ID_LISTBOX2	        		(GUI_ID_USER + 0x04)

#define ID_EDIT1	        		(GUI_ID_USER + 0x05)

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
{
  // -----------------------------------------------------------------------------------------------------------------------------
  //						name						id					x		y		xsize	ysize	?		?		?
  // -----------------------------------------------------------------------------------------------------------------------------
  // Self
  { WINDOW_CreateIndirect,	"", 						ID_WINDOW_0,		0,    	0,		800,	480, 	0, 		0x64, 	0 },
  // List box
  { TEXT_CreateIndirect, 	"BandActivity.Text",		ID_TEXT_LIST1,		5,		5,		390, 	20,  	0, 		0x0,	0 },
  { LISTBOX_CreateIndirect, "BandActivity.Listbox",		ID_LISTBOX1, 		5, 		23, 	390, 	200, 	0, 		0x0, 	0 },
  { TEXT_CreateIndirect, 	"RxFreq.Text",				ID_TEXT_LIST2,		405,	5,		390, 	20,  	0, 		0x0,	0 },
  { LISTBOX_CreateIndirect, "RxFreq.Listbox",			ID_LISTBOX2, 		405, 	23, 	390, 	200, 	0, 		0x0, 	0 },
  // Edit boxes
  { EDIT_CreateIndirect,     NULL,     					ID_EDIT1,   		5,  	230,  	100,  	30, 	0,		0x0,	0 },
};

WM_HWIN 		hDesktopFT8 = 0;
WM_HTIMER 		hTimerTime;

//*----------------------------------------------------------------------------
//* Function Name       : _cbControl
//* Object              : Control related events(touch, change value, etc)
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
static void _cbControl(WM_MESSAGE *pMsg, int Id,int NCode)
{
	WM_HWIN 	hList1,hList2;
	int 		sel;
	char 		buf[50];

	switch(Id)
	{
		// Band Activity List
		case ID_LISTBOX1:
		{
			// Only processing Item selection via touch
			if(NCode != WM_NOTIFICATION_SEL_CHANGED)
				break;

			hList1 = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX1);
			sel = LISTBOX_GetSel(hList1);
			if(sel != LISTBOX_ALL_ITEMS)
			{
				// Get text
				LISTBOX_GetItemText(hList1,sel,buf,sizeof(buf));
				//printf("text=%s\r\n",buf);

				// Add to second list
				hList2 = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX2);
				LISTBOX_AddString(hList2, buf);
			}

			break;
		}

		// RX Frequency List
		case ID_LISTBOX2:
		{
			// Only processing Item selection via touch
			if(NCode != WM_NOTIFICATION_SEL_CHANGED)
				break;

			hList1 = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX2);
			sel = LISTBOX_GetSel(hList1);
			if(sel != LISTBOX_ALL_ITEMS)
			{
				// Get text
				LISTBOX_GetItemText(hList1,sel,buf,sizeof(buf));
				//printf("text=%s\r\n",buf);

				// Do something..
			}

			break;
		}

		default:
			break;
	}

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
	WM_HWIN 			hDlg,hText,hList,hEdit;
	SCROLLBAR_Handle 	hScrollV;
	int     			NCode,Id;

	RTC_DateTypeDef     Date;
	RTC_TimeTypeDef     Time;

	GUI_PID_STATE * pState;
	hDlg = pMsg->hWin;

	switch (pMsg->MsgId)
	{
    	case WM_INIT_DIALOG:
    	{
    		hTimerTime = WM_CreateTimer(pMsg->hWin, 0, 1000, 0);

			// Init Listbox
    		hText = WM_GetDialogItem(pMsg->hWin, ID_TEXT_LIST1);
			TEXT_SetFont(hText,&GUI_Font8x16_1);
			TEXT_SetBkColor(hText,GUI_STCOLOR_LIGHTBLUE);
			TEXT_SetTextColor(hText,GUI_WHITE);
			TEXT_SetTextAlign(hText,TEXT_CF_LEFT|TEXT_CF_VCENTER);
			//
			hList = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX1);
			LISTBOX_SetFont(hList, &GUI_Font8x16_1);
			LISTBOX_SetTextColor(hList,LISTBOX_CI_UNSEL,GUI_STCOLOR_DARKBLUE);
			hScrollV = SCROLLBAR_CreateAttached(hList, SCROLLBAR_CF_VERTICAL);
			//
			TEXT_SetText(	  hText, "  UTC   dB   DT Freq       Message       ");
			//
			#if 1
			LISTBOX_AddString(hList, "133900 -19  0.1 1199 ~  K2NRS R5DU -25"   );
			LISTBOX_AddString(hList, "133900 -17  1.9  853 ~  CQ UR4LBG KN89"   );
			LISTBOX_AddString(hList, "133900  -6  2.1  905 ~  CQ OK1HEH JN79"   );
			LISTBOX_AddString(hList, "133900   0  2.3 1831 ~  UA6HI E74BYZ R-24");
			LISTBOX_AddString(hList, "133900 -18  2.0 2064 ~  GI3SG RX9ATX -25" );
			LISTBOX_AddString(hList, "133900  -9  1.3 2181 ~  CQ ER1OO KN46"    );
			LISTBOX_AddString(hList, "133900  -9  2.1 2260 ~  SE3X UN7LZ R-08"  );
			LISTBOX_AddString(hList, "133900  -6  2.2  918 ~  CQ OH6HPS KP10"   );
			LISTBOX_AddString(hList, "133900 -18  2.1 2134 ~  CQ E73DN JN93"    );
			#endif

			// Init Listbox
    		hText = WM_GetDialogItem(pMsg->hWin, ID_TEXT_LIST2);
			TEXT_SetFont(hText,&GUI_Font8x16_1);
			TEXT_SetBkColor(hText,GUI_STCOLOR_LIGHTBLUE);
			TEXT_SetTextColor(hText,GUI_WHITE);
			TEXT_SetTextAlign(hText,TEXT_CF_LEFT|TEXT_CF_VCENTER);
			//
			hList = WM_GetDialogItem(pMsg->hWin, ID_LISTBOX2);
			LISTBOX_SetFont(hList, &GUI_Font8x16_1);
			LISTBOX_SetTextColor(hList,LISTBOX_CI_UNSEL,GUI_STCOLOR_DARKBLUE);
			hScrollV = SCROLLBAR_CreateAttached(hList, SCROLLBAR_CF_VERTICAL);
			//
			TEXT_SetText(	  hText, "  UTC   dB   DT Freq       Message       ");
			//
			#if 0
			LISTBOX_AddString(hList, "133900 -19  0.1 1199 ~  K2NRS R5DU -25"   );
			#endif

			hEdit = WM_GetDialogItem(hDlg, ID_EDIT1);
			EDIT_SetFont(hEdit,&GUI_FontAvantGarde16B);
			EDIT_SetBkColor(hEdit,EDIT_CI_ENABLED,GUI_STCOLOR_LIGHTBLUE);
			EDIT_SetTextColor(hEdit,EDIT_CI_ENABLED,GUI_WHITE);
			EDIT_SetTextAlign(hEdit,TEXT_CF_HCENTER|TEXT_CF_VCENTER);

    		break;
    	}

    	case WM_PAINT:
    	{
    		break;
    	}

    	case WM_DELETE:
    		WM_DeleteTimer(hTimerTime);
    		break;

    	case WM_TIMER:
    	{
    		char buf[50];

    		WM_InvalidateWindow(pMsg->hWin);

    		k_GetTime(&Time);
    		k_GetDate(&Date);

    		hEdit = WM_GetDialogItem(hDlg, ID_EDIT1);

    		sprintf(buf,"%02d:%02d:%02d",Time.Hours,Time.Minutes,Time.Seconds);
    		EDIT_SetText(hEdit, buf);

    		WM_RestartTimer(pMsg->Data.v, 1000);
    		break;
    	}

		// Process key messages
		case WM_KEY:
		{
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key)
			{
		        case GUI_KEY_HOME:
		        	break;
		        case GUI_KEY_ENTER:
		        	break;
		        default:
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
//* Function Name       : ui_desktop_ft8_set_profile
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
static void ui_desktop_ft8_set_profile(void)
{
	WINDOW_SetDefaultBkColor(GUI_LIGHTGRAY);	// looks better than white background

	LISTVIEW_SetDefaultGridColor(GUI_WHITE);
	LISTVIEW_SetDefaultBkColor(LISTVIEW_CI_SEL, GUI_STCOLOR_LIGHTBLUE);
	LISTVIEW_SetDefaultBkColor(LISTVIEW_CI_SELFOCUS, GUI_STCOLOR_LIGHTBLUE);
	LISTBOX_SetDefaultScrollMode(LISTBOX_CF_AUTOSCROLLBAR_V);

	TEXT_SetDefaultTextColor(GUI_STCOLOR_LIGHTBLUE);
	TEXT_SetDefaultFont(&GUI_FontLubalGraph20);

	SCROLLBAR_SetDefaultColor(GUI_STCOLOR_LIGHTBLUE, SCROLLBAR_CI_THUMB|SCROLLBAR_CI_SHAFT|SCROLLBAR_CI_ARROW);
	SCROLLBAR_SetDefaultWidth(30);
}

//*----------------------------------------------------------------------------
//* Function Name       : ui_desktop_ft8_create
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void ui_desktop_ft8_create(void)
{
	ui_desktop_ft8_set_profile();

	WM_SetCallback(WM_HBKWIN, &_cbBkWindow);
	hDesktopFT8 = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbCallback, 0, 0, 0);
}

//*----------------------------------------------------------------------------
//* Function Name       : ui_desktop_ft8_destroy
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void ui_desktop_ft8_destroy(void)
{
	if(hDesktopFT8)
	{
		WM_SetCallback		(WM_HBKWIN, 0);
		WM_InvalidateWindow	(WM_HBKWIN);
		WM_DeleteWindow(hDesktopFT8);
	}
}
