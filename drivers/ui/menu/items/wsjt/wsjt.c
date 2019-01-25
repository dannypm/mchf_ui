
#include "mchf_pro_board.h"

#include "gui.h"
#include "dialog.h"
#include "ST_GUI_Addons.h"

#include "k_module.h"

#include "menu\items\wsjt\digi\ft8_lib-master\gen_ft8.h"
#include "menu\items\wsjt\digi\ft8_lib-master\decode_ft8.h"

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
	{ BUTTON_CreateIndirect, 	"Encode",		 			ID_BUTTON_ENCODE,	670, 	40, 	120, 	45, 	0, 		0x0, 	0 },
	{ BUTTON_CreateIndirect, 	"Decode",		 			ID_BUTTON_DECODE,	670, 	120, 	120, 	45, 	0, 		0x0, 	0 },

};

static void _cbControl(WM_MESSAGE * pMsg, int Id, int NCode)
{
	WM_HWIN hItem;

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
					//GUI_MessageBox("Encode button clicked.","Information",GUI_MB_OK);

					encode_ft8_message("CQ M0NKA IO92");
					break;
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
					//GUI_MessageBox("Decode button clicked.","Information",GUI_MB_OK);

					decode_ft8_message(NULL);
					break;
			}
			break;
		}

		// -------------------------------------------------------------
		default:
			break;
	}
}

static void _cbDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN 			hItem;
	int 				Id, NCode;

	switch (pMsg->MsgId)
	{
		case WM_INIT_DIALOG:
		{
			//..
			break;
		}

		case WM_PAINT:
			break;

		case WM_DELETE:
			break;

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

