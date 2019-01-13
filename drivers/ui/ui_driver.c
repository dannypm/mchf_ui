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

#include "ui_driver.h"

#include "signal_e.h"
#include "touch_driver.h"
#include "k_rtc.h"

#include "gui.h"
#include "dialog.h"
#include "ST_GUI_Addons.h"

// -----------------------------------------------------------------------------------------------
// Desktop Mode
#include "desktop\ui_controls_layout.h"
#include "desktop\spectrum\ui_controls_spectrum.h"
#include "desktop\smeter\ui_controls_smeter.h"
#include "desktop\freq\ui_controls_frequency.h"
#include "desktop\volume\ui_controls_volume.h"
#include "desktop\clock\ui_controls_clock.h"
#include "desktop\band\ui_controls_band.h"
#include "desktop\keyer\ui_controls_keyer.h"
#include "desktop\filter\ui_controls_filter.h"
#include "desktop\vfo_step\ui_controls_vfo_step.h"
#include "desktop\rx_tx\ui_controls_rx_tx.h"
#include "desktop\demod\ui_controls_demod.h"
#include "desktop\cpu_stat\ui_controls_cpu_stat.h"
#include "desktop\dsp_stat\ui_controls_dsp_stat.h"
#include "desktop\sd_icon\ui_controls_sd_icon.h"
// -----------------------------------------------------------------------------------------------
// Audio PopUp Mode
#include "audio_popup\ui_audio_popup.h"
// -----------------------------------------------------------------------------------------------
// Menu Mode
#include "k_module.h"
// -----------------------------------------------------------------------------------------------

// Locals only
static void ui_driver_change_mode(void);
static void ui_driver_init_desktop(void);
static void ui_driver_state_machine(void);
static void ui_driver_touch_router(void);
//  -- test --
static void ui_driver_update_dsp_info(void);

// UI driver public state
struct	UI_DRIVER_STATE			ui_s;

// Touch data
GUI_PID_STATE 	TS_State;

// Public radio state
//extern struct	TRANSCEIVER_STATE_UI	tsu;

// Touch driver state - needed here at all ?
extern struct TD 				t_d;

// ---------------------------------------------------
// Menu items
extern K_ModuleItem_Typedef  	settings1_board;
extern K_ModuleItem_Typedef  	settings2_board;
extern K_ModuleItem_Typedef  	settings3_board;
extern K_ModuleItem_Typedef  	file_browser;
extern K_ModuleItem_Typedef  	clock_info;
extern K_ModuleItem_Typedef  	factory_board;
extern K_ModuleItem_Typedef  	wsjt_menu;
extern K_ModuleItem_Typedef  	about_board;
// ---------------------------------------------------

//*----------------------------------------------------------------------------
//* Function Name       : ui_driver_emwin_528_init
//* Object              : working init for the older, 2015 release of emWin
//* Input Parameters    : assumes CRC unit enabled beforehand
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void ui_driver_emwin_528_init(void)
{
	//printf("before\r\n");

	// Initialise GUI
	GUI_Init();

	//printf("after\r\n");

	GUI_X_InitOS();

	#ifdef USE_SDRAM
	WM_MULTIBUF_Enable(1);
	#endif

	// Heavy use of Alpha blending, but this might be
	// enabled already by the emWin config file
	GUI_EnableAlpha(1);

	// Text looks nicer this way
	GUI_SetTextMode(GUI_TM_TRANS);
}

//*----------------------------------------------------------------------------
//* Function Name       : ui_driver_change_mode
//* Object              : change screen mode
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
static void ui_driver_change_mode(void)
{
	uchar state;

	// Take a snapshot of the state
	state = ui_s.req_state;

	// Do we need update ?
	if(ui_s.cur_state == state)
		return;

	switch(state)
	{
		// Switch to menu mode
		case MODE_MENU:
		{
			printf("Entering Menu mode...\r\n");

			// Destroy desktop controls
			ui_controls_smeter_quit();
			ui_controls_spectrum_quit();

			// Clear screen
			GUI_SetBkColor(GUI_BLACK);
			GUI_Clear();

			// Set General Graphical properties
			k_SetGuiProfile();

			// Show the main menu
			k_InitMenu();

			// Initial paint
			GUI_Exec();

			break;
		}

		// Switch to audio popup mode
		case MODE_AUDIO_POPUP:
		{
			printf("Entering Audio popup mode...\r\n");

			// Destroy desktop controls
			ui_controls_smeter_quit();
			ui_controls_spectrum_quit();

			// Clear screen
			GUI_SetBkColor(GUI_BLACK);
			GUI_Clear();

			// Show popup
			ui_audio_popup_create();

			// Initial paint
			GUI_Exec();

			break;
		}

		// Switch to desktop mode
		case MODE_DESKTOP:
		{
			printf("Entering Desktop mode...\r\n");

			// Destroy any Window Manager items
			destroy_menu();
			ui_audio_popup_destroy();

			// Clear screen
			GUI_SetBkColor(GUI_BLACK);
			GUI_Clear();

			// Init controls
			ui_driver_init_desktop();

			break;
		}

		default:
			break;
	}

	// Update flag
	ui_s.cur_state = state;

	// Release lock
	ui_s.lock_requests = 0;
}

static void ui_driver_init_desktop(void)
{
	ui_controls_clock_init();

	#ifdef SPECTRUM_WATERFALL
	ui_controls_spectrum_init();
	#endif

	ui_controls_smeter_init();

	#ifdef VFO_BOTH
	ui_controls_frequency_init();
	#endif

	ui_controls_volume_init();
	ui_controls_band_init();
	ui_controls_filter_init();
	ui_controls_vfo_step_init();
	ui_controls_rx_tx_init();
	ui_controls_demod_init();
	ui_controls_cpu_stat_init();
	ui_controls_dsp_stat_init();
	ui_controls_sd_icon_init();

	if(*(uchar *)(EEP_BASE + EEP_KEYER_ON))
		ui_controls_keyer_init();
}

//*----------------------------------------------------------------------------
//* Function Name       : ui_driver_task
//* Object              : driver main task
//* Input Parameters    : lower priority, but executes as often as pos
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
//uchar wd_skip = 0;
void ui_driver_task(void const * argument)
{
	printf("ui driver start\r\n");

	// Default driver state
	ui_s.req_state 				= MODE_DESKTOP;
	ui_s.cur_state 				= MODE_DESKTOP;
	ui_s.lock_requests			= 0;

	// Init graphics lib
	ui_driver_emwin_528_init();

	// Show Startup screen(on layer 1)
	k_StartUp();

	// Prepare Desktop screen, while hidden
	GUI_SelectLayer(0);
	GUI_SetBkColor(GUI_BLACK);
	GUI_Clear();

	// Init controls
	ui_driver_init_desktop();

	// Menu items
	k_ModuleInit();
	k_ModuleAdd(&settings1_board);
	k_ModuleAdd(&settings2_board);
	k_ModuleAdd(&settings3_board);
	k_ModuleAdd(&clock_info);
	k_ModuleAdd(&file_browser);
	k_ModuleAdd(&factory_board);
	k_ModuleAdd(&wsjt_menu);
	k_ModuleAdd(&about_board);

	// Show desktop(on layer 0)
	GUI_SetLayerVisEx (1, 0);

ui_driver_loop:

	// Touch events to correct control
	ui_driver_touch_router();

	// Process mode change requests
	ui_driver_change_mode();

	// Refresh screen
	switch(ui_s.cur_state)
	{
		case MODE_DESKTOP:
		{
			// Repaint Desktop controls
			ui_driver_state_machine();

			// 100 Hz refresh
			OsDelayMs(10);
			break;
		}

		case MODE_MENU:
		{
			// Repaint Menu controls
			GUI_Exec();

			// If main menu needs constant refresh
			k_PeriodicProcesses();

			// 25 Hz refresh
			OsDelayMs(40);
			break;
		}

		case MODE_AUDIO_POPUP:
		{
			// Repaint Menu controls
			GUI_Exec();

			// Can't be bothered with fast repaint, unless on screen slider and multitouch
			OsDelayMs(100);
			break;
		}

		default:
			OsDelayMs(500);
			break;
	}

	goto ui_driver_loop;
}

//----------------------------------------------------------------
//----------------------------------------------------------------
// All ui call that take too long should call here to release
// control to functions that need real time reaction
//
void ui_callback_one(void)
{
	//--printf("callback1\r\n");
	#ifdef VFO_BOTH
	ui_controls_frequency_refresh(0);
	#endif
	ui_controls_clock_refresh();

	ui_controls_cpu_stat_refresh();

	//ui_driver_change_screen_demod_mode();
	//ui_controls_band_refresh();
}
//
void ui_callback_two(void)
{
	//--printf("callback2\r\n");
	#ifdef VFO_BOTH
	ui_controls_frequency_refresh(0);
	#endif
	ui_controls_clock_refresh();

	//ui_driver_change_screen_demod_mode();
	//ui_controls_band_refresh();
}
//----------------------------------------------------------------
//----------------------------------------------------------------

//*----------------------------------------------------------------------------
//* Function Name       : ui_driver_state_machine
//* Object              : ui processing switch
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
static void ui_driver_state_machine(void)
{
	// To prevent lag on frequency update, always call here
	#ifdef VFO_BOTH
	ui_controls_frequency_refresh(0);
	#endif

	if(*(uchar *)(EEP_BASE + EEP_KEYER_ON))
		ui_controls_keyer_refresh();

	ui_controls_smeter_refresh  (&ui_callback_one);
	//
	#ifdef SPECTRUM_WATERFALL
	ui_controls_spectrum_refresh(&ui_callback_two);
	#endif

	ui_controls_dsp_stat_refresh();
	ui_controls_cpu_stat_refresh();
	ui_controls_volume_refresh();
	ui_controls_demod_refresh();
	ui_controls_band_refresh();
	ui_controls_filter_refresh();
	ui_controls_vfo_step_refresh();
	ui_controls_clock_refresh();
	ui_controls_sd_icon_refresh();

	// test
	ui_driver_update_dsp_info();
}

//*----------------------------------------------------------------------------
//* Function Name       : ui_driver_touch_router
//* Object              : -- route touch events to user controls --
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_UI
//*----------------------------------------------------------------------------
static void ui_driver_touch_router(void)
{
	// ---------------------------------------------------------
	// --------------		MENU MODE        -------------------
	// ---------------------------------------------------------
	//
	// Submit to emWin
	//
	// - the reason this is here, instead of the touch driver -
	//   emWin doesn't like being accessed from separate threads
	//   and we don't want critical sections on touch events!
	//
	if((ui_s.cur_state == MODE_MENU)||(ui_s.cur_state == MODE_AUDIO_POPUP))
	{
		// Process pending from digitizer driver
		if((t_d.pending) && (!TS_State.Pressed))
		{
			//printf("pass touch event to emWin\r\n");

			TS_State.Pressed 	= 1;
			TS_State.Layer 		= 0;
			TS_State.x 			= t_d.point_x[0];
			TS_State.y 			= t_d.point_y[0];

			GUI_TOUCH_StoreStateEx(&TS_State);

			// Reset flag
			t_d.pending 		= 0;

			return;
		}

		// emWin needs depress event, so generate on next call
		if((!t_d.pending) && (TS_State.Pressed))
		{
			//printf("clear touch event in emWin\r\n");

			TS_State.Pressed 	= 0;
			TS_State.Layer 		= 0;
			TS_State.x 			= 0;
			TS_State.y 			= 0;

			GUI_TOUCH_StoreStateEx(&TS_State);

			return;
		}

		// Just in case
		return;
	}

	// ---------------------------------------------------------
	// --------------		DESKTOP MODE     -------------------
	// ---------------------------------------------------------
	//
	// Anything waiting ?
	if(t_d.pending == 0)
		return;
#if 1
	// ---------------------------------------------------------
	// Process on screen keyer
	if(		(t_d.point_x[0] >= IAMB_KEYER_X) &&\
			(t_d.point_x[0] <= (IAMB_KEYER_X + IAMB_KEYER_SIZE_X)) &&\
			(t_d.point_y[0] >= IAMB_KEYER_Y) &&\
			(t_d.point_y[0] <= (IAMB_KEYER_Y + IAMB_KEYER_SIZE_Y))
	  )
	{
		ui_controls_keyer_touch();
		return;
	}
#endif
	// ---------------------------------------------------------
	// Speaker control
	if(		(t_d.point_x[0] >= SPEAKER_X) &&\
			(t_d.point_x[0] <= (SPEAKER_X + SPEAKER_SIZE_X)) &&\
			(t_d.point_y[0] >= SPEAKER_Y) &&\
			(t_d.point_y[0] <= (SPEAKER_Y + SPEAKER_SIZE_Y))
	  )
	{
		ui_controls_volume_touch();
		return;
	}

	// ---------------------------------------------------------
	// S-meter control
	if(		(t_d.point_x[0] >= S_METER_X) &&\
			(t_d.point_x[0] <= (S_METER_X + S_METER_SIZE_X)) &&\
			(t_d.point_y[0] >= S_METER_Y) &&\
			(t_d.point_y[0] <= (S_METER_Y + S_METER_SIZE_Y))
	  )
	{
		ui_controls_smeter_touch();
		return;
	}

	// ---------------------------------------------------------
	// Spectrum control
	if(		(t_d.point_x[0] >= SW_FRAME_X) &&\
			(t_d.point_x[0] <= (SW_FRAME_X + SW_CONTROL_X_SIZE)) &&\
			(t_d.point_y[0] >= SW_FRAME_Y) &&\
			(t_d.point_y[0] <= (SW_FRAME_Y + SW_CONTROL_Y_SIZE))
	  )
	{
		ui_controls_spectrum_touch();
		return;
	}

	// ---------------------------------------------------------
	// Filter control
	if(		(t_d.point_x[0] >= FILTER_X) &&\
			(t_d.point_x[0] <= (FILTER_X + FILTER_SIZE_X)) &&\
			(t_d.point_y[0] >= FILTER_Y) &&\
			(t_d.point_y[0] <= (FILTER_Y + FILTER_SIZE_Y))
	  )
	{
		ui_controls_filter_touch();
		return;
	}
}

// -------------------------------------------
// -------------------------------------------
// Debug only!!!!
uchar sw_d_skip = 0;
static void ui_driver_update_dsp_info(void)
{
#if 0
	char buf[100];

	sw_d_skip++;
	if(sw_d_skip < 5)
		return;

	sw_d_skip = 0;

	GUI_SetColor(GUI_BLACK);
	GUI_FillRect(SW_FRAME_X + 3,145,(SW_FRAME_Y + 170),(145 + 19));

	GUI_SetFont(&GUI_Font20B_ASCII);
	GUI_SetColor(GUI_GREEN);
	sprintf(buf,"b:%d d:%d f:%d s:%d xo:%d nco:%d\r\n",tsu.dsp_band,tsu.dsp_demod,tsu.dsp_filter,tsu.dsp_step_idx,tsu.dsp_freq,tsu.dsp_nco_freq);
	GUI_DispStringAt(buf,((SW_FRAME_X) + 0),(145 + 1));
#endif
}

