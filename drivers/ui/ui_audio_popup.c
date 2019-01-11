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

#define ID_WINDOW_0               (GUI_ID_USER + 0x00)
#define ID_BUTTON_EXIT            (GUI_ID_USER + 0x01)

static const GUI_WIDGET_CREATE_INFO _aDialog[] =
{
	// -----------------------------------------------------------------------------------------------------------------------------
	//							name						id					x		y		xsize	ysize	?		?		?
	// -----------------------------------------------------------------------------------------------------------------------------
	// Self
	{ WINDOW_CreateIndirect,	"", 						ID_WINDOW_0,		0,    	0,		600,	300, 	0, 		0x64, 	0 },
	// Back Button
	//{ BUTTON_CreateIndirect, 	"Test",			 			ID_BUTTON_EXIT, 	300, 	300, 	120, 	45, 	0, 		0x0, 	0 },
};

static void _cbDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN 			hItem;
	int 				Id, NCode;

	switch (pMsg->MsgId)
	{
		case WM_INIT_DIALOG:
		{
			break;
		}

		case WM_PAINT:
			break;

		case WM_DELETE:
			GUI_EndDialog(pMsg->hWin, 0);
			break;

		/*case WM_NOTIFY_PARENT:
		{
			Id    = WM_GetId(pMsg->hWinSrc);    // Id of widget
			NCode = pMsg->Data.v;               // Notification code

			_cbControl(pMsg,Id,NCode);
			break;
		}*/

		// Process key messages not supported by ICON_VIEW control
		/*case WM_KEY:
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
		}*/

		default:
			WM_DefaultProc(pMsg);
			break;
	}
}

void ui_audio_popup_create(void)
{
	WM_SetCallback(WM_HBKWIN, _cbDialog);
	GUI_CreateDialogBox(_aDialog, GUI_COUNTOF(_aDialog), _cbDialog,WM_HBKWIN, 100, 90);

}
