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
#include "keypad_driver.h"

#include "gui.h"
#include "dialog.h"
//#include "ST_GUI_Addons.h"

#ifdef CHIP_F7
#include "stm32f7xx_hal_gpio.h"
#endif
#ifdef CHIP_H7
#include "stm32h7xx_hal_gpio.h"
#endif

// Tuning steps
const ulong tune_steps[T_STEP_MAX_STEPS] = {
T_STEP_1HZ,
T_STEP_10HZ,
T_STEP_100HZ,
T_STEP_1KHZ,
T_STEP_10KHZ,
T_STEP_100KHZ,
T_STEP_1MHZ,
T_STEP_10MHZ
};

#ifdef CONTEXT_DRIVER_KEYPAD

extern struct	UI_DRIVER_STATE			ui_s;

// Public radio state
extern struct	TRANSCEIVER_STATE_UI	tsu;

struct KEYPAD_STATE						ks;

// -----------------------
// LED driving publics
uchar btn_id = 0;
ushort pwmbuffer[2*24*1];
uchar start_counter = 0;
// -----------------------

static void keypad_driver_leds_init(void)
{
	GPIO_InitTypeDef  gpio_init_structure;

	// SCK - PA5 (ToDo: GPIO for start, finally, use SPI HW)
	gpio_init_structure.Pin 	= KEYLED_SCK_PIN;
	gpio_init_structure.Mode 	= GPIO_MODE_OUTPUT_PP;
	gpio_init_structure.Pull 	= GPIO_PULLUP;
	gpio_init_structure.Speed 	= GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(KEYLED_SCK_PORT, &gpio_init_structure);

	// MOSI - PB5 (ToDo: GPIO for start, finally, use SPI HW)
	gpio_init_structure.Pin 	= KEYLED_MOSI_PIN;
	gpio_init_structure.Mode 	= GPIO_MODE_OUTPUT_PP;
	gpio_init_structure.Pull 	= GPIO_PULLUP;
	gpio_init_structure.Speed 	= GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(KEYLED_MOSI_PORT, &gpio_init_structure);

	// XLAT - PI11
	gpio_init_structure.Pin 	= KEYLED_XLAT_PIN;
	gpio_init_structure.Mode 	= GPIO_MODE_OUTPUT_PP;
	gpio_init_structure.Pull 	= GPIO_PULLUP;
	gpio_init_structure.Speed 	= GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(KEYLED_XLAT_PORT, &gpio_init_structure);

	// BLANK - PI8
	gpio_init_structure.Pin 	= KEYLED_BLANK_PIN;
	gpio_init_structure.Mode 	= GPIO_MODE_OUTPUT_PP;
	gpio_init_structure.Pull 	= GPIO_PULLUP;
	gpio_init_structure.Speed 	= GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(KEYLED_BLANK_PORT, &gpio_init_structure);

	// BLANK high - all off
	KEYLED_BLANK_PORT->BSRRL = KEYLED_BLANK_PIN;

	// XLAT low
	KEYLED_XLAT_PORT->BSRRH = KEYLED_XLAT_PIN;

	memset(pwmbuffer,0,2*24*1);
}

static void keypad_driver_mini_delay(void)
{
	uchar i;

	for(i = 0; i < 200; i++)
		__asm(".word 0x46004600");
}

static void keypad_driver_shift(void)
{
	KEYLED_XLAT_PORT->BSRRH = KEYLED_XLAT_PIN;
	keypad_driver_mini_delay();

	  // 24 channels per TLC5974
	  for (int16_t c=24*1 - 1; c >= 0 ; c--)
	  {
	    // 12 bits per channel, send MSB first
	    for (int8_t b=11; b>=0; b--)
	    {
	    	// Clock low
	    	KEYLED_SCK_PORT->BSRRH = KEYLED_SCK_PIN;
	    	keypad_driver_mini_delay();

	    	// Next data bit
	    	if (pwmbuffer[c] & (1 << b))
	    		KEYLED_MOSI_PORT->BSRRL = KEYLED_MOSI_PIN;
	    	else
	    		KEYLED_MOSI_PORT->BSRRH = KEYLED_MOSI_PIN;

	    	// Clock high
	    	KEYLED_SCK_PORT->BSRRL = KEYLED_SCK_PIN;
	    	keypad_driver_mini_delay();
	    }
	  }

	  // Clock it out on the Latch pin
	  KEYLED_XLAT_PORT->BSRRH = KEYLED_XLAT_PIN;
	  keypad_driver_mini_delay();

	  KEYLED_XLAT_PORT->BSRRL = KEYLED_XLAT_PIN;
	  keypad_driver_mini_delay();

	  KEYLED_XLAT_PORT->BSRRH = KEYLED_XLAT_PIN;
	  keypad_driver_mini_delay();
}

static void keypad_driver_change_led_state(uchar button,uchar brightness)
{
	// LED state
	btn_id = button;
	pwmbuffer[btn_id] = brightness;

	keypad_driver_shift();
}

static void keyboard_driver_blink_all(void)
{
	uchar i;
	// -------------------------------
		//if(uc_keep_flag)
		//	pwmbuffer[0] = 0;
		//else
		//	pwmbuffer[0] = 64;

		for(i = 0; i < 24; i++)
			pwmbuffer[i] = 0;

		pwmbuffer[btn_id] = 64;

		btn_id++;
		if(btn_id > 23)
			btn_id = 0;

		keypad_driver_shift();
		// --------------------------------
}

// Set up blank early on, to prevent
// LEDs being lit up on start
void keypad_driver_prevent_startup_blink(void)
{
	GPIO_InitTypeDef  gpio_init_structure;

	// BLANK - PI8
	gpio_init_structure.Pin 	= KEYLED_BLANK_PIN;
	gpio_init_structure.Mode 	= GPIO_MODE_OUTPUT_PP;
	gpio_init_structure.Pull 	= GPIO_PULLUP;
	gpio_init_structure.Speed 	= GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(KEYLED_BLANK_PORT, &gpio_init_structure);

	// BLANK high - all off
	KEYLED_BLANK_PORT->BSRRL = KEYLED_BLANK_PIN;
}

//*----------------------------------------------------------------------------
//* Function Name       : keypad_task
//* Object              :
//* Notes    			: call from main(), during init, due to a H7 bug
//* Notes   			: not from this driver!!!
//* Notes    			:
//* Context    			: CONTEXT_RESET_VECTOR
//*----------------------------------------------------------------------------
void keypad_driver_init(void)
{
	GPIO_InitTypeDef  gpio_init_structure;

	#ifdef CHIP_F7
	gpio_init_structure.Speed 	= GPIO_SPEED_LOW;
	#endif
	#ifdef CHIP_H7
	gpio_init_structure.Speed 	= GPIO_SPEED_FREQ_HIGH;
	#endif

	// -------------------------------------------------------------
	// Outputs - horizontal, four lines
	gpio_init_structure.Mode 	= GPIO_MODE_OUTPUT_PP;
	gpio_init_structure.Pull 	= GPIO_PULLUP;
	//
	// KEYPAD_Y1
	gpio_init_structure.Pin 	= KEYPAD_PIN_Y1;
	HAL_GPIO_Init(KEYPAD_PORT_Y1, &gpio_init_structure);
	//
	// KEYPAD_Y2
	gpio_init_structure.Pin 	= KEYPAD_PIN_Y2;
	HAL_GPIO_Init(KEYPAD_PORT_Y2, &gpio_init_structure);
	//
	// KEYPAD_Y3
	gpio_init_structure.Pin 	= KEYPAD_PIN_Y3;
	HAL_GPIO_Init(KEYPAD_PORT_Y3, &gpio_init_structure);
	//
	// KEYPAD_Y4
	gpio_init_structure.Pin 	= KEYPAD_PIN_Y4;
	HAL_GPIO_Init(KEYPAD_PORT_Y4, &gpio_init_structure);

	// -------------------------------------------------------------
	// Inputs - vertical, six lines
	gpio_init_structure.Mode 	= GPIO_MODE_INPUT;
	gpio_init_structure.Pull 	= GPIO_PULLUP;
	//
	// KEYPAD_X1
	gpio_init_structure.Pin 	= KEYPAD_PIN_X1;
	HAL_GPIO_Init(KEYPAD_PORT_X1, &gpio_init_structure);
	//
	// KEYPAD_X2
	gpio_init_structure.Pin 	= KEYPAD_PIN_X2;
	HAL_GPIO_Init(KEYPAD_PORT_X2, &gpio_init_structure);
	//
	// KEYPAD_X3
	gpio_init_structure.Pin 	= KEYPAD_PIN_X3;
	HAL_GPIO_Init(KEYPAD_PORT_X3, &gpio_init_structure);
	//
	// KEYPAD_X4
	gpio_init_structure.Pin 	= KEYPAD_PIN_X4;
	HAL_GPIO_Init(KEYPAD_PORT_X4, &gpio_init_structure);			// not wired on rev 0.7, 12 Apr 2018 !!!
	//
	// KEYPAD_X5
	gpio_init_structure.Pin 	= KEYPAD_PIN_X5;
	HAL_GPIO_Init(KEYPAD_PORT_X5, &gpio_init_structure);
	//
	// KEYPAD_X6
	gpio_init_structure.Pin 	= KEYPAD_PIN_X6;
	HAL_GPIO_Init(KEYPAD_PORT_X6, &gpio_init_structure);

	// All output lines high
	KEYPAD_PORT_Y1->BSRRL = KEYPAD_PIN_Y1;
	KEYPAD_PORT_Y2->BSRRL = KEYPAD_PIN_Y2;
	KEYPAD_PORT_Y3->BSRRL = KEYPAD_PIN_Y3;
	KEYPAD_PORT_Y4->BSRRL = KEYPAD_PIN_Y4;

	// Reset public
	ks.curr_button_id 	= 0;
	ks.hold_time		= 0;

	keypad_driver_leds_init();
}

//*----------------------------------------------------------------------------
//* Function Name       : keypad_cmd_processor_desktop
//* Object              :
//* Notes    			: keypad process while in Desktop
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_KEYPAD
//*----------------------------------------------------------------------------
static void keypad_cmd_processor_desktop(uchar x,uchar y, uchar hold)
{
	#ifdef KEYPAD_ALLOW_DEBUG
	printf("x=%d,y=%d,hold=%d\r\n",x,y,hold);
	#endif

	// SSB - USB/LSB
	if((x == 1) && (y == 1))
	{
		if(!hold)
		{
			if(tsu.band[tsu.curr_band].demod_mode > DEMOD_LSB)
				tsu.band[tsu.curr_band].demod_mode = DEMOD_LSB;
			else
			{
				if(tsu.band[tsu.curr_band].demod_mode == DEMOD_LSB)
					tsu.band[tsu.curr_band].demod_mode = DEMOD_USB;
				else
					tsu.band[tsu.curr_band].demod_mode = DEMOD_LSB;
			}
		}
		else
		{
			// ..
		}

		return;
	}
	// 160m
	if((x == 2) && (y == 1))
	{
		if(!hold)
		{
			//tsu.curr_band = BAND_MODE_160;
		}
		else
		{
			// ..
		}

		return;
	}
	// 80m
	if((x == 3) && (y == 1))
	{
		if(!hold)
		{
			tsu.curr_band = BAND_MODE_80;
		}
		else
		{
			// ..
		}

		return;
	}
	// 60m
	if((x == 4) && (y == 1))
	{
		if(!hold)
		{
			tsu.curr_band = BAND_MODE_60;
		}
		else
		{
			// ..
		}

		return;
	}
	// 40m
	if((x == 5) && (y == 1))
	{
		if(!hold)
		{
			tsu.curr_band = BAND_MODE_40;
		}
		else
		{
			// ..
		}

		return;
	}
	// DSP
	if((x == 6) && (y == 1))
	{
		if(!hold)
		{
			//--tsu.cw_tx_on = !tsu.cw_tx_on;
		}
		else
		{
			// ..
		}

		return;
	}
	// CW
	if((x == 1) && (y == 2))
	{
		if(!hold)
		{
			tsu.band[tsu.curr_band].demod_mode = DEMOD_CW;
		}
		else
		{
			// ..
		}

		return;
	}
	// 30m
	if((x == 2) && (y == 2))
	{
		if(!hold)
		{
			tsu.curr_band = BAND_MODE_30;
		}
		else
		{
			// ..
		}

		return;
	}
	// 20m
	if((x == 3) && (y == 2))
	{

		if(!hold)
		{
			tsu.curr_band = BAND_MODE_20;

			// ToDo: to make this work, need to put all other band leds to off
			//       in every band selection case!
			//
			// Toggle key LED
			//if(tsu.curr_band != BAND_MODE_20)
			//	keypad_driver_change_led_state(KEY_LED_FIVE,KEY_LED_OFF_LIGHT);
			//else
			//	keypad_driver_change_led_state(KEY_LED_FIVE,KEY_LED_LOW_LIGHT);
		}
		else
		{
			// ..
		}

		return;
	}
	// 17m
	if((x == 4) && (y == 2))
	{
		if(!hold)
		{
			tsu.curr_band = BAND_MODE_17;
		}
		else
		{
			// ..
		}

		return;
	}
	// 15m
	if((x == 5) && (y == 2))
	{
		if(!hold)
		{
			tsu.curr_band = BAND_MODE_15;
		}
		else
		{
			// ..
		}

		return;
	}
	// Step
	if((x == 6) && (y == 2))
	{
		if(!hold)
		{
			// Toggle 1KHz and 10Khz only
			if(tsu.band[tsu.curr_band].step != T_STEP_1KHZ)
				tsu.band[tsu.curr_band].step = T_STEP_1KHZ;
			else
				tsu.band[tsu.curr_band].step = T_STEP_10KHZ;
		}
		else
		{
			uchar id;
			// Jump through all possible steps
			// Scan
			for(id = 0; id < T_STEP_MAX_STEPS;id++)
			{
				if(tune_steps[id] == tsu.band[tsu.curr_band].step)
					break;
			}

			if(id < (T_STEP_MAX_STEPS - 1))
				id++;
			else
				id = 0;

			tsu.band[tsu.curr_band].step = tune_steps[id];
		}

		return;
	}
	// AM
	if((x == 1) && (y == 3))
	{
		if(!hold)
		{
			tsu.band[tsu.curr_band].demod_mode = DEMOD_AM;
		}
		else
		{
			// Maybe toggle different digi modes here ?
			// Default - FT8
			//
			// Stay in USB ? Force DSP to digi mode ??
			//
			//
			//tsu.band[tsu.curr_band].demod_mode = DEMOD_DIGI;

			// Pass request to UI driver to change mode
			if(ui_s.req_state == MODE_DESKTOP)
				ui_s.req_state = MODE_DESKTOP_FT8;
			else
			{
				if(ui_s.req_state == MODE_DESKTOP_FT8)
					ui_s.req_state = MODE_DESKTOP;
			}
		}

		return;
	}
	// 12m
	if((x == 2) && (y == 3))
	{
		if(!hold)
		{
			tsu.curr_band = BAND_MODE_12;
		}
		else
		{
			// ..
		}

		return;
	}
	// 10m
	if((x == 3) && (y == 3))
	{
		if(!hold)
		{
			tsu.curr_band = BAND_MODE_10;
		}
		else
		{
			// ..
		}

		return;
	}
	// 6m
	if((x == 4) && (y == 3))
	{
		if(!hold)
		{
			//tsu.curr_band = BAND_MODE_6;
		}
		else
		{
			// ..
		}

		return;
	}
	// Enter(MENU)
	if((x == 5) && (y == 3))
	{
		if(!hold)
		{
			// ...
		}
		else
		{
			if(!ui_s.lock_requests)
			{
				// Lock yourself out, then only the UI driver can release the lock
				// after GUI repaint
				ui_s.lock_requests = 1;

				// Pass request to UI driver to change mode
				if(ui_s.req_state == MODE_DESKTOP)
					ui_s.req_state = MODE_MENU;
				else
				{
					if(ui_s.req_state == MODE_MENU)
						ui_s.req_state = MODE_DESKTOP;
				}

				// Large debounce
				OsDelayMs(500);
			}
			else
				printf("locked\r\n");
		}

		return;
	}
	// Filter
	if((x == 6) && (y == 3))
	{
		if(!hold)
		{
			tsu.band[tsu.curr_band].filter++;
			if(tsu.band[tsu.curr_band].filter > AUDIO_WIDE)
				tsu.band[tsu.curr_band].filter = AUDIO_300HZ;

			//printf("keypad filter: %d\r\n",tsu.curr_filter);
		}
		else
		{
			// ..
		}

		return;
	}
	// Toggle Fix/Centre VFO mode
	if((x == 1) && (y == 4))
	{
		if(!hold)
		{
			uchar loc_osc_mode = tsu.band[tsu.curr_band].fixed_mode;

			loc_osc_mode = !loc_osc_mode;

			// If back to centre mode, reset NCO freq
			if(!loc_osc_mode)
			{
				tsu.band[tsu.curr_band].nco_freq = 0;

				// Set request to DSP as well
				tsu.update_nco_dsp_req = 1;
			}

			// Toggle key LED
			if(!loc_osc_mode)
				keypad_driver_change_led_state(KEY_LED_FIX,KEY_LED_OFF_LIGHT);
			else
				keypad_driver_change_led_state(KEY_LED_FIX,KEY_LED_MID_LIGHT);

			tsu.band[tsu.curr_band].fixed_mode = loc_osc_mode;
		}
		else
		{
			// ..
		}

		return;
	}
	// 4m
	if((x == 2) && (y == 4))
	{
		if(!hold)
		{
			//tsu.curr_band = BAND_MODE_4;
		}
		else
		{
			// ..
		}

		return;
	}
	// LF
	if((x == 3) && (y == 4))
	{
		if(!hold)
		{
			//tsu.curr_band = BAND_MODE_LF;
		}
		else
		{
			// Pass request to UI driver to change mode
			if(ui_s.req_state == MODE_DESKTOP)
				ui_s.req_state = MODE_QUICK_LOG;
			else
			{
				if(ui_s.req_state == MODE_QUICK_LOG)
					ui_s.req_state = MODE_DESKTOP;
			}
		}

		return;
	}
	// MF
	if((x == 4) && (y == 4))
	{
		if(!hold)
		{
			//tsu.curr_band = BAND_MODE_MF;
		}
		else
		{
			// ..
		}

		return;
	}
	// No button connected
	if((x == 5) && (y == 4))
	{
		// NA
	}
	// VFO A/B, SPLIT
	if((x == 6) && (y == 4))
	{
		if(!hold)
		{
			// Toggle active
			tsu.band[tsu.curr_band].active_vfo = !tsu.band[tsu.curr_band].active_vfo;

			// Toggle key LED
			if(!tsu.band[tsu.curr_band].active_vfo)
				keypad_driver_change_led_state(KEY_LED_SPLIT,KEY_LED_OFF_LIGHT);
			else
				keypad_driver_change_led_state(KEY_LED_SPLIT,KEY_LED_MID_LIGHT);
		}
		else
		{
			// ..
		}
	}
}

//*----------------------------------------------------------------------------
//* Function Name       : keypad_cmd_processor_menu
//* Object              :
//* Notes    			: keypad process while in Menu
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_KEYPAD
//*----------------------------------------------------------------------------
static void keypad_cmd_processor_menu(uchar x,uchar y, uchar hold)
{
	#ifdef KEYPAD_ALLOW_DEBUG
	printf("x=%d,y=%d,hold=%d\r\n",x,y,hold);
	#endif

	// SSB - USB/LSB
	if((x == 1) && (y == 1))
	{
		if(!hold)
		{

		}
		else
		{
			// ..
		}

		return;
	}
	// 160m
	if((x == 2) && (y == 1))
	{
		if(!hold)
		{

		}
		else
		{
			// ..
		}

		return;
	}
	// 80m
	if((x == 3) && (y == 1))
	{
		if(!hold)
		{

		}
		else
		{
			// ..
		}

		return;
	}
	// 60m
	if((x == 4) && (y == 1))
	{
		if(!hold)
		{

		}
		else
		{
			// ..
		}

		return;
	}
	// 40m
	if((x == 5) && (y == 1))
	{
		if(!hold)
		{

		}
		else
		{
			// ..
		}

		return;
	}
	// DSP
	if((x == 6) && (y == 1))
	{
		if(!hold)
		{
			GUI_StoreKeyMsg(GUI_KEY_RIGHT,1);
			GUI_StoreKeyMsg(GUI_KEY_RIGHT,0);
		}
		else
		{
			// ..
		}

		return;
	}
	// CW
	if((x == 1) && (y == 2))
	{
		if(!hold)
		{

		}
		else
		{
			// ..
		}

		return;
	}
	// 30m
	if((x == 2) && (y == 2))
	{
		if(!hold)
		{

		}
		else
		{
			// ..
		}

		return;
	}
	// 20m
	if((x == 3) && (y == 2))
	{

		if(!hold)
		{

		}
		else
		{
			// ..
		}

		return;
	}
	// 17m
	if((x == 4) && (y == 2))
	{
		if(!hold)
		{

		}
		else
		{
			// ..
		}

		return;
	}
	// 15m
	if((x == 5) && (y == 2))
	{
		if(!hold)
		{

		}
		else
		{
			// ..
		}

		return;
	}
	// Step
	if((x == 6) && (y == 2))
	{
		if(!hold)
		{
			GUI_StoreKeyMsg(GUI_KEY_LEFT,1);
			GUI_StoreKeyMsg(GUI_KEY_LEFT,0);
		}
		else
		{

		}

		return;
	}
	// AM
	if((x == 1) && (y == 3))
	{
		if(!hold)
		{

		}
		else
		{
			// ..
		}

		return;
	}
	// 12m
	if((x == 2) && (y == 3))
	{
		if(!hold)
		{

		}
		else
		{
			// ..
		}

		return;
	}
	// 10m
	if((x == 3) && (y == 3))
	{
		if(!hold)
		{

		}
		else
		{
			// ..
		}

		return;
	}
	// 6m
	if((x == 4) && (y == 3))
	{
		if(!hold)
		{

		}
		else
		{
			// ..
		}

		return;
	}
	// Enter(MENU)
	if((x == 5) && (y == 3))
	{
		if(!hold)
		{
			GUI_StoreKeyMsg(GUI_KEY_ENTER,1);
			GUI_StoreKeyMsg(GUI_KEY_ENTER,0);
		}
		else
		{
			if(!ui_s.lock_requests)
			{
				// Lock yourself out, then only the UI driver can release the lock
				// after GUI repaint
				ui_s.lock_requests = 1;

				// Pass request to UI driver to change mode
				if(ui_s.req_state == MODE_DESKTOP)
					ui_s.req_state = MODE_MENU;
				else
				{
					if(ui_s.req_state == MODE_MENU)
						ui_s.req_state = MODE_DESKTOP;
				}

				// Large debounce
				OsDelayMs(500);
			}
			else
				printf("locked\r\n");
		}

		return;
	}
	// Filter
	if((x == 6) && (y == 3))
	{
		if(!hold)
		{
			GUI_StoreKeyMsg(GUI_KEY_HOME,1);
			GUI_StoreKeyMsg(GUI_KEY_HOME,0);
		}
		else
		{
			// ..
		}

		return;
	}
	// Toggle Fix/Centre VFO mode
	if((x == 1) && (y == 4))
	{
		if(!hold)
		{

		}
		else
		{
			// ..
		}

		return;
	}
	// 4m
	if((x == 2) && (y == 4))
	{
		if(!hold)
		{

		}
		else
		{
			// ..
		}

		return;
	}
	// LF(Space)
	if((x == 3) && (y == 4))
	{
		if(!hold)
		{
			GUI_StoreKeyMsg(GUI_KEY_SPACE,1);
			GUI_StoreKeyMsg(GUI_KEY_SPACE,0);
		}
		else
		{
			// ..
		}

		return;
	}
	// MF(Backspace)
	if((x == 4) && (y == 4))
	{
		if(!hold)
		{
			GUI_StoreKeyMsg(GUI_KEY_BACKSPACE,1);
			GUI_StoreKeyMsg(GUI_KEY_BACKSPACE,0);
		}
		else
		{
			// ..
		}

		return;
	}
	// No button connected
	if((x == 5) && (y == 4))
	{
		// NA
	}
	// VFO A/B, SPLIT(TAB)
	if((x == 6) && (y == 4))
	{
		if(!hold)
		{
			GUI_StoreKeyMsg(GUI_KEY_TAB,1);
			GUI_StoreKeyMsg(GUI_KEY_TAB,0);
		}
		else
		{
			// ..
		}
	}
}

//*----------------------------------------------------------------------------
//* Function Name       : keypad_cmd_processor
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_KEYPAD
//*----------------------------------------------------------------------------
static void keypad_cmd_processor(uchar x,uchar y, uchar hold)
{
	//if(ui_s.cur_state == MODE_DESKTOP)
	//	keypad_cmd_processor_desktop(x,y,hold);
	//else
	//	keypad_cmd_processor_menu(x,y,hold);

	// Manage different UI driver modes
	switch(ui_s.cur_state)
	{
		// Main radio desktop
		case MODE_DESKTOP:
		case MODE_DESKTOP_FT8:
		case MODE_QUICK_LOG:
			keypad_cmd_processor_desktop(x,y,hold);
			break;

		// Menu mode via Window Manager
		case MODE_MENU:
			keypad_cmd_processor_menu(x,y,hold);
			break;

		// Do we need keyboard processing in other modes ?
		// If yes, declare and handle...
		default:
			break;
	}
}

//*----------------------------------------------------------------------------
//* Function Name       : keypad_check_input_lines
//* Object              :
//* Notes    			: Return ID of clicked button
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_KEYPAD
//*----------------------------------------------------------------------------
static uchar keypad_check_input_lines(void)
{
	if((KEYPAD_PORT_X1->IDR & KEYPAD_PIN_X1) != KEYPAD_PIN_X1)
		return 1;
	if((KEYPAD_PORT_X2->IDR & KEYPAD_PIN_X2) != KEYPAD_PIN_X2)
		return 2;
	if((KEYPAD_PORT_X3->IDR & KEYPAD_PIN_X3) != KEYPAD_PIN_X3)
		return 3;
	if((KEYPAD_PORT_X4->IDR & KEYPAD_PIN_X4) != KEYPAD_PIN_X4)
		return 4;
	if((KEYPAD_PORT_X5->IDR & KEYPAD_PIN_X5) != KEYPAD_PIN_X5)
		return 5;
	if((KEYPAD_PORT_X6->IDR & KEYPAD_PIN_X6) != KEYPAD_PIN_X6)
		return 6;
	return 0;
}

//*----------------------------------------------------------------------------
//* Function Name       : keypad_scan_a
//* Object              :
//* Notes    			: support for press and hold
//* Notes   			: - non blocking implementation
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_KEYPAD
//*----------------------------------------------------------------------------
static void keypad_scan_a(void)
{
	uchar i,j,id;

	// Full 4x6 matrix scan
	for(i = 0; i < 5; i++)
	{
		// Rotate output state
		switch(i)
		{
			case 0:
				scan_y1();
				break;
			case 1:
				scan_y2();
				break;
			case 2:
				scan_y3();
				break;
			case 3:
				scan_y4();
				break;
			default:
				scan_off();
				return;
		}

		// Check lines, and get an id
		id = keypad_check_input_lines();

		// Nothing clicked on this line, check next
		if(!id)
			continue;

		//printf("click id: %d\r\n",id);

		// Is button still pressed, maybe hold instead of click ?
		// 400mS wait for press, enough ?
		for(j = 0; j < 4; j++)
		{
			// Hold delay(also act as debounce delay on click only)
			OsDelayMs(100);

			// Keep on checking
			if(keypad_check_input_lines() != id)
			{
				//printf("is click\r\n");

				// End scan
				scan_off();

				// Process 'click'
				keypad_cmd_processor(id,(i + 1),0);

				OsDelayMs(500);
				return;
			}
		}

		//printf("is hold\r\n");

		// End scan
		scan_off();

		// Process 'hold'
		keypad_cmd_processor(id,(i + 1),1);

		OsDelayMs(500);
		return;
	}
}

//*----------------------------------------------------------------------------
//* Function Name       : keypad_task
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_KEYPAD
//*----------------------------------------------------------------------------
void keypad_driver_task(void const * argument)
{
	// Delay start, so UI can paint properly
	OsDelayMs(KEYPAD_DRV_START_DELAY);

	printf("keypad driver start\r\n");

	// BLANK low - all LEDs on
	KEYLED_BLANK_PORT->BSRRH = KEYLED_BLANK_PIN;

	// Moved to main()
	//--keypad_init();

keypad_driver_loop:

	if(start_counter < 24)
	{
		keyboard_driver_blink_all();
		start_counter++;

		OsDelayMs(50);
		goto keypad_driver_loop;
	}

	keypad_scan_a();
	OsDelayMs(60);

	goto keypad_driver_loop;
}

#endif




