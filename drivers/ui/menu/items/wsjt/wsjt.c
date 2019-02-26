
#include "mchf_pro_board.h"

#include "gui.h"
#include "dialog.h"
#include "ST_GUI_Addons.h"

#include "k_module.h"

//#include "menu\items\wsjt\digi\ft8_lib-master\gen_ft8.h"
//#include "menu\items\wsjt\digi\ft8_lib-master\decode_ft8.h"

extern GUI_CONST_STORAGE GUI_BITMAP bmwsjtx_globe_1024x1024;

const GUI_BITMAP * wsjt_anims[] = {
  &bmwsjtx_globe_1024x1024,   &bmwsjtx_globe_1024x1024,   &bmwsjtx_globe_1024x1024,   &bmwsjtx_globe_1024x1024, &bmwsjtx_globe_1024x1024
};
  
static void Startup(WM_HWIN hWin, uint16_t xpos, uint16_t ypos);

K_ModuleItem_Typedef  wsjt =
{
  1,
  "WSJT-X Tools",
  wsjt_anims,
  &bmwsjtx_globe_1024x1024,
  0,
  Startup,
  NULL,
};

#define ID_WINDOW_0               	(GUI_ID_USER + 0x00)
#define ID_BUTTON_EXIT            	(GUI_ID_USER + 0x01)

#define ID_BUTTON_ENCODE	  		(GUI_ID_USER + 0x02)
#define ID_BUTTON_DECODE	  		(GUI_ID_USER + 0x04)

static const GUI_WIDGET_CREATE_INFO _aDialog[] = 
{
	// -----------------------------------------------------------------------------------------------------------------------------
	//							name						id					x		y		xsize	ysize	?		?		?
	// -----------------------------------------------------------------------------------------------------------------------------
	// Self
	{ WINDOW_CreateIndirect,	"", 						ID_WINDOW_0,		0,    	0,		800,	430, 	0, 		0x64, 	0 },
	// Back Button
	{ BUTTON_CreateIndirect, 	"Back",			 			ID_BUTTON_EXIT, 	670, 	375, 	120, 	45, 	0, 		0x0, 	0 },
	//
	{ BUTTON_CreateIndirect, 	"Encode",		 			ID_BUTTON_ENCODE,	670, 	20, 	120, 	45, 	0, 		0x0, 	0 },
	{ EDIT_CreateIndirect,     	NULL,     					GUI_ID_EDIT0,   	20,  	20,  	300,  	40, 	EDIT_CI_ENABELD,0 },
	{ BUTTON_CreateIndirect, 	"Decode",		 			ID_BUTTON_DECODE,	670, 	80, 	120, 	45, 	0, 		0x0, 	0 },
	{ EDIT_CreateIndirect,     	NULL,     					GUI_ID_EDIT1,   	340,  	20,  	300,  	40, 	EDIT_CI_ENABELD,0 },

};

// For delayed DSP processing
WM_HTIMER 		hTimerDsp;

// DSP Driver messaging
extern osMessageQId 				hDspMessage;
struct DSPMessage					wsjt_dsp_call;

static void _cbControl(WM_MESSAGE * pMsg, int Id, int NCode)
{
	WM_HWIN hItem;
	char 	buf[40];
	uchar 	i;

	switch(Id)
	{
		// -------------------------------------------------------------
		// Button - exit
		case ID_BUTTON_EXIT:
		{
			switch(NCode)
			{
				case WM_NOTIFICATION_RELEASED:
					GUI_EndDialog(pMsg->hWin, 0);
					break;
			}
			break;
		}

		// -------------------------------------------------------------
		// Button click
		case ID_BUTTON_ENCODE:
		{
			switch(NCode)
			{
				case WM_NOTIFICATION_RELEASED:
				{
					// Encode and save
					//
					// This stalls the UI repaint, so if this menu
					// stays, need to have non blocking implementation of
					// this call!
					//for(i = 0; i < 40; i++)
					//	buf[i] = 0;
					//
					//hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT0);
					//EDIT_GetText(hItem,buf,40);
					//
					//encode_ft8_message(buf,1);
					//GUI_MessageBox("Message encoded and saved to card!","Information",GUI_MB_OK);

					// ------------------------------------------------------------
					// new, via message to DSP driver
					//
					for(i = 0; i < DSP_MAX_PAYLOAD; i++)
						wsjt_dsp_call.cData[i] = 0;

					hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT0);
					EDIT_GetText(hItem,wsjt_dsp_call.cData,DSP_MAX_PAYLOAD);

					wsjt_dsp_call.ucMessageID 	= 1;	// menu in driver to call
					wsjt_dsp_call.ucProcessDone = 0;	// flag to signal encode done
					osMessagePut(hDspMessage, (ulong)&wsjt_dsp_call, osWaitForever);

					// Disable button
					hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_ENCODE);
					WM_DisableWindow(hItem);

					break;
				}
			}
			break;
		}

		// -------------------------------------------------------------
		// Button click
		case ID_BUTTON_DECODE:
		{
			switch(NCode)
			{
				case WM_NOTIFICATION_RELEASED:
				{
					// ------------------------------------------------------------
					//for(i = 0; i < 40; i++)
					//	buf[i] = 0;
					// Decode - old, direct call to library
					//
					// This stalls the UI repaint, so if this menu
					// stays, need to have non blocking implementation of
					// this call!
					//
					//decode_ft8_message(buf);
					//hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT1);
					//EDIT_SetText(hItem,buf);

					// ------------------------------------------------------------
					// new, via message to DSP driver
					//
					wsjt_dsp_call.ucMessageID 	= 2;	// menu in driver to call
					wsjt_dsp_call.ucDataReady	= 0;	// clear data ready flag
					osMessagePut(hDspMessage, (ulong)&wsjt_dsp_call, osWaitForever);

					// Disable button
					hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_DECODE);
					WM_DisableWindow(hItem);

					break;
				}
			}
			break;
		}

		// -------------------------------------------------------------
		default:
			break;
	}

	WM_InvalidateWindow(WM_GetClientWindow(pMsg->hWin));
}

static void _cbDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN 			hItem;
	int 				Id, NCode;

	switch (pMsg->MsgId)
	{
		case WM_INIT_DIALOG:
		{
			// Timer for delayed DSP processing
			wsjt_dsp_call.ucDataReady = 0;
			wsjt_dsp_call.ucProcessDone = 0;
			hTimerDsp = WM_CreateTimer(pMsg->hWin, 0, 500, 0);

			// Encode input edit
			hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT0);
			EDIT_SetFont(hItem,&GUI_FontAvantGarde20B);
			EDIT_SetBkColor(hItem,EDIT_CI_ENABLED,GUI_WHITE);
			EDIT_SetTextColor(hItem,EDIT_CI_ENABLED,GUI_STCOLOR_LIGHTBLUE);
			EDIT_SetTextAlign(hItem,TEXT_CF_LEFT|TEXT_CF_VCENTER);
			EDIT_SetMaxLen(hItem,40);
			EDIT_SetText(hItem,"CQ M0NKA IO92");

			// Decode result edit
			hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT1);
			EDIT_SetFont(hItem,&GUI_FontAvantGarde20B);
			EDIT_SetBkColor(hItem,EDIT_CI_ENABLED,GUI_WHITE);
			EDIT_SetTextColor(hItem,EDIT_CI_ENABLED,GUI_STCOLOR_LIGHTBLUE);
			EDIT_SetTextAlign(hItem,TEXT_CF_LEFT|TEXT_CF_VCENTER);
			EDIT_SetMaxLen(hItem,40);

			break;
		}

		case WM_PAINT:
			break;

		case WM_DELETE:
		{
			WM_DeleteTimer(hTimerDsp);
			wsjt_dsp_call.ucDataReady = 0;
			wsjt_dsp_call.ucProcessDone = 0;
			break;
		}

		case WM_TIMER:
		{
			// Decode finished
			if(wsjt_dsp_call.ucDataReady)
			{
				//--printf("wsjt menu, DSP driver signalled data ready\r\n");

				// Copy data to Edit
				hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT1);
				EDIT_SetText(hItem,wsjt_dsp_call.cData);

				// Clear flag to prevent excessive repaint here
				wsjt_dsp_call.ucDataReady = 0;

				// Enable button
				hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_DECODE);
				WM_EnableWindow(hItem);
			}

			// Encode finished
			if(wsjt_dsp_call.ucProcessDone)
			{
				wsjt_dsp_call.ucProcessDone = 0;

				// Enable button
				hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_ENCODE);
				WM_EnableWindow(hItem);
			}

			WM_InvalidateWindow(pMsg->hWin);
			WM_RestartTimer(pMsg->Data.v, 1000);
			break;
		}


		case WM_NOTIFY_PARENT:
		{
			Id    = WM_GetId(pMsg->hWinSrc);    /* Id of widget */
			NCode = pMsg->Data.v;               /* Notification code */

			_cbControl(pMsg,Id,NCode);
			break;
		}

		// Process key messages not supported by ICON_VIEW control
		case WM_KEY:
		{
			switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key)
			{
		        // Return from menu
		        case GUI_KEY_HOME:
		        {
		        	//printf("GUI_KEY_HOME\r\n");
		        	GUI_EndDialog(pMsg->hWin, 0);
		        	break;
		        }
			}
			break;
		}

		default:
			WM_DefaultProc(pMsg);
			break;
	}
}

static void Startup(WM_HWIN hWin, uint16_t xpos, uint16_t ypos)
{
	GUI_CreateDialogBox(_aDialog, GUI_COUNTOF(_aDialog), _cbDialog, hWin, xpos, ypos);
}

