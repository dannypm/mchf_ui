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

#include "k_module.h"

extern GUI_CONST_STORAGE GUI_BITMAP bmicon_gps;

const GUI_BITMAP * logbook_anim[] = {
  &bmicon_gps,   &bmicon_gps,   &bmicon_gps,   &bmicon_gps, &bmicon_gps
};
  
static void Startup(WM_HWIN hWin, uint16_t xpos, uint16_t ypos);

K_ModuleItem_Typedef  logbook =
{
  5,
  "Logbook Viewer",
  logbook_anim,
  &bmicon_gps,
  0,
  Startup,
  NULL,
};

#define ID_WINDOW_0				(GUI_ID_USER + 0x00)
#define ID_BUTTON_EXIT			(GUI_ID_USER + 0x01)

#define ID_BUTTON_UPDATE		(GUI_ID_USER + 0x02)
#define ID_BUTTON_PAGEU			(GUI_ID_USER + 0x03)
#define ID_BUTTON_PAGED			(GUI_ID_USER + 0x04)

#define ID_TEXT_DX_CALL			(GUI_ID_USER + 0x04)
#define ID_EDIT_DX_CALL			(GUI_ID_USER + 0x05)

#define ID_LISTVIEW				(GUI_ID_USER + 0x06)

static const GUI_WIDGET_CREATE_INFO _aDialog[] = 
{
	// -----------------------------------------------------------------------------------------------------------------------------
	//							name						id					x		y		xsize	ysize	?		?		?
	// -----------------------------------------------------------------------------------------------------------------------------
	// Self
	{ WINDOW_CreateIndirect,	"", 						ID_WINDOW_0,		0,    	0,		800,	430, 	0, 		0x64, 	0 },
	// Back Button
	{ BUTTON_CreateIndirect, 	"Back",			 			ID_BUTTON_EXIT, 	670, 	375, 	120, 	45, 	0, 		0x0, 	0 },
	// Other buttons
	{ BUTTON_CreateIndirect, 	"Update",			 		ID_BUTTON_UPDATE, 	460, 	375, 	120, 	45, 	0, 		0x0, 	0 },
	{ BUTTON_CreateIndirect, 	"PageUp",			 		ID_BUTTON_PAGEU, 	310, 	375, 	120, 	45, 	0, 		0x0, 	0 },
	{ BUTTON_CreateIndirect, 	"PageDown",			 		ID_BUTTON_PAGED, 	180, 	375, 	120, 	45, 	0, 		0x0, 	0 },
	// Edit boxes
	{ TEXT_CreateIndirect,     	"Callsign",		  			ID_TEXT_DX_CALL,   	5,		7,  	70,  	25, 	0, 	  	0,		0 },
	{ EDIT_CreateIndirect,     	"dx.call", 					ID_EDIT_DX_CALL,   	80,  	5,  	80,  	25, 	0,		0,		0 },
	// ListView
	{ LISTVIEW_CreateIndirect, 	"Listview", 				ID_LISTVIEW, 		170, 	5, 		620, 	360, 	0, 		0, 		0 },
};

//
// Just a test, need dynamic entry from .ini file
//
static const char * _aTable_1[][11] = {
		{ "2014-10-25", "12:50", "15m", "SSB", "VE3ED", "John", "FN03EA", "21.020.00", "599", "599", 0 },
		{ "2014-10-25", "12:51", "15m", "SSB", "VE3ED", "John", "FN03EB", "21.020.00", "599", "599", 0 },
		{ "2014-10-25", "12:52", "15m", "SSB", "VE3EA", "John", "FN03EC", "21.020.00", "599", "599", 0 },
		{ "2014-10-25", "12:53", "15m", "SSB", "VE3ER", "John", "FN03EE", "21.020.00", "599", "599", 0 },
		{ "2014-10-26", "16:20", "12m", "SSB", "VE3EF", "John", "FN03EF", "24.900.00", "599", "599", 0 },
		{ "2014-10-26", "16:21", "12m", "SSB", "VE3EV", "John", "FN03EQ", "24.900.00", "599", "599", 0 },
		{ "2014-10-26", "16:24", "12m", "SSB", "VE3FE", "John", "FN03EA", "24.900.00", "599", "599", 0 },
		{ "2014-10-26", "16:28", "12m", "SSB", "VE3SW", "John", "FN03EF", "24.900.00", "599", "599", 0 },
		{ "2014-10-28", "10:10", "20m", "SSB", "VE3DS", "John", "FN03ES", "14.200.00", "599", "599", 0 },
		{ "2014-10-28", "10:12", "20m", "SSB", "VE3UY", "John", "FN03EE", "14.200.00", "599", "599", 0 },
		{ "2014-10-28", "10:14", "20m", "SSB", "VE3TE", "John", "FN03EV", "14.200.00", "599", "599", 0 },
		{ "2014-10-28", "10:15", "20m", "SSB", "VE3DC", "John", "FN03ED", "14.200.00", "599", "599", 0 },
		{ "2014-10-28", "10:16", "20m", "SSB", "VE3WS", "John", "FN03EJ", "14.200.00", "599", "599", 0 },
		{ "2014-10-28", "10:17", "20m", "SSB", "VE3HJ", "John", "FN03EH", "14.200.00", "599", "599", 0 },
		{ "2014-10-28", "10:18", "20m", "SSB", "VE3ER", "John", "FN03EG", "14.200.00", "599", "599", 0 },
		{ "2014-10-30", "22:30", "80m", "SSB", "VE3QA", "John", "FN03EF", "03.777.00", "599", "599", 0 },
		{ "2014-10-30", "22:36", "80m", "SSB", "VE3CV", "John", "FN03ET", "03.777.00", "599", "599", 0 },
		{ "2014-10-30", "22:36", "80m", "SSB", "VE3CV", "John", "FN03ET", "03.777.00", "599", "599", 0 },
		{ "2014-10-30", "22:39", "80m", "SSB", "VE3WS", "John", "FN03EY", "03.777.00", "599", "599", 0 }
};

static void _cbControl(WM_MESSAGE * pMsg, int Id, int NCode)
{
	WM_HWIN hItem;

	switch(Id)
	{
		// -------------------------------------------------------------
		// Button
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
		default:
			break;
	}
}

static void _cbDialog(WM_MESSAGE * pMsg)
{
	WM_HWIN 			hItem;
	int 				Id, NCode;
	ulong 				i;

	switch (pMsg->MsgId)
	{
		case WM_INIT_DIALOG:
		{
			//HEADER_Handle hHeader;

			// Callsign edit
			hItem = WM_GetDialogItem(pMsg->hWin, ID_EDIT_DX_CALL);
			EDIT_SetFont(hItem,&GUI_FontAvantGarde16B);
			EDIT_SetBkColor(hItem,EDIT_CI_ENABLED,GUI_STCOLOR_LIGHTBLUE);
			EDIT_SetTextColor(hItem,EDIT_CI_ENABLED,GUI_WHITE);
			EDIT_SetTextAlign(hItem,TEXT_CF_HCENTER|TEXT_CF_VCENTER);
			EDIT_SetText(hItem,"LZ2IIS");

			// ListView
			hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW);
			//hHeader = LISTVIEW_GetHeader(hItem);
			//
			//--HEADER_SetBkColor(hHeader, GUI_RED);		-- already set in ui_set_gui_profile()
			//--HEADER_SetTextColor(hHeader, GUI_BLUE);
			//--HEADER_SetHeight(hHeader, 30);
			//
			// { "2014-10-25", "12:50", "15m", "SSB", "VE3EJ", "John", "FN03ED" },
			//
			LISTVIEW_AddColumn(hItem, 80, "Date", 		GUI_TA_HCENTER | GUI_TA_VCENTER);
			LISTVIEW_AddColumn(hItem, 60, "Time",		GUI_TA_HCENTER | GUI_TA_VCENTER);
			LISTVIEW_AddColumn(hItem, 50, "Band", 		GUI_TA_HCENTER | GUI_TA_VCENTER);
			LISTVIEW_AddColumn(hItem, 50, "Mode",		GUI_TA_HCENTER | GUI_TA_VCENTER);
			LISTVIEW_AddColumn(hItem, 70, "Callsign",	GUI_TA_HCENTER | GUI_TA_VCENTER);
			LISTVIEW_AddColumn(hItem, 70, "Name", 		GUI_TA_HCENTER | GUI_TA_VCENTER);
			LISTVIEW_AddColumn(hItem, 70, "Locator", 	GUI_TA_HCENTER | GUI_TA_VCENTER);
			LISTVIEW_AddColumn(hItem, 80, "Frequency", 	GUI_TA_HCENTER | GUI_TA_VCENTER);
			LISTVIEW_AddColumn(hItem, 40, "RST-R", 		GUI_TA_HCENTER | GUI_TA_VCENTER);
			LISTVIEW_AddColumn(hItem, 40, "RST-S", 		GUI_TA_HCENTER | GUI_TA_VCENTER);
			//
			for(i = 0; i < 19; i++) LISTVIEW_AddRow(hItem,_aTable_1[i]);
			//
			LISTVIEW_SetGridVis(hItem, 1);
			// -- scrollbar looks ugly!
			//SCROLLBAR_SetWidth(SCROLLBAR_CreateAttached(hItem, SCROLLBAR_CF_VERTICAL),20);

			break;
		}

		case WM_PAINT:
			break;

		case WM_DELETE:
			break;

		case WM_NOTIFY_PARENT:
		{
			Id    = WM_GetId(pMsg->hWinSrc);    // Id of widget
			NCode = pMsg->Data.v;               // Notification code

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

