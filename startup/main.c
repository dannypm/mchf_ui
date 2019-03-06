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


#ifdef CHIP_F7
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_rcc.h"
#include "stm32f7xx_hal_gpio.h"
#endif
#ifdef CHIP_H7
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_rcc.h"
#include "stm32h7xx_hal_gpio.h"
// test only - no gui print here !!!
#include "gui.h"
#include "dialog.h"
#include "ST_GUI_Addons.h"
#endif

// Hardware and libs
#include "hw/sdram/sdram.h"
#include "watchdog.h"

// Drivers
#include "ui_driver.h"
#include "keypad_driver.h"
#include "touch_driver.h"
#include "api_driver.h"
#include "rotary_driver.h"
#include "sd_driver.h"
#include "net_driver.h"
#include "dsp_driver.h"

// rtc
#include "hw\rtc\k_rtc.h"

// Allocate OS heap, based on linker script definition
#ifdef CHIP_F7
uint8_t ucHeap[ configTOTAL_HEAP_SIZE ] __attribute__((section(".RamData2")));
#endif
#ifdef CHIP_H7
uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
#endif

// OS tick export
extern void xPortSysTickHandler(void);

// Public radio state
extern struct	TRANSCEIVER_STATE_UI	tsu;

// Driver communication
#ifdef CONTEXT_DRIVER_API
osMessageQId 	hApiMessage;
#endif
#ifdef CONTEXT_DRIVER_DSP
osMessageQId 	hDspMessage;
#endif

//*----------------------------------------------------------------------------
//* Function Name       : HardFault_Handler
//* Object              : blink LCD backlight once
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_RESET_VECTOR
//*----------------------------------------------------------------------------
void HardFault_Handler(void)
{
	ulong i,j;

	// Long pause
	for(i = 0; i < 0xFFFFFFF; i++)
		__asm(".hword 0x46C0");

	// Blink
	for(j = 0; j < 1; j++)
	{
		// On
		mchf_pro_board_blink_if_alive(1|4);
		for(i = 0; i < 0x1FFFFFF; i++)
			__asm(".hword 0x46C0");

		// Off
		mchf_pro_board_blink_if_alive(1);
		for(i = 0; i < 0x1FFFFFF; i++)
			__asm(".hword 0x46C0");
	}

stall:
	goto stall;
}

//*----------------------------------------------------------------------------
//* Function Name       : MemManage_Handler
//* Object              : blink LCD backlight twice
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_RESET_VECTOR
//*----------------------------------------------------------------------------
void MemManage_Handler(void)
{
	ulong i,j;

	// Long pause
	for(i = 0; i < 0xFFFFFFF; i++)
		__asm(".hword 0x46C0");

	// Blink
	for(j = 0; j < 2; j++)
	{
		// On
		mchf_pro_board_blink_if_alive(1|4);
		for(i = 0; i < 0x1FFFFFF; i++)
			__asm(".hword 0x46C0");

		// Off
		mchf_pro_board_blink_if_alive(1);
		for(i = 0; i < 0x1FFFFFF; i++)
			__asm(".hword 0x46C0");
	}

stall:
	goto stall;
}

//*----------------------------------------------------------------------------
//* Function Name       : BusFault_Handler
//* Object              : blink LCD backlight three times
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_RESET_VECTOR
//*----------------------------------------------------------------------------
void BusFault_Handler(void)
{
	ulong i,j;

	// Long pause
	for(i = 0; i < 0xFFFFFFF; i++)
		__asm(".hword 0x46C0");

	// Blink
	for(j = 0; j < 3; j++)
	{
		// On
		mchf_pro_board_blink_if_alive(1|4);
		for(i = 0; i < 0x1FFFFFF; i++)
			__asm(".hword 0x46C0");

		// Off
		mchf_pro_board_blink_if_alive(1);
		for(i = 0; i < 0x1FFFFFF; i++)
			__asm(".hword 0x46C0");
	}

stall:
	goto stall;
}

//*----------------------------------------------------------------------------
//* Function Name       : UsageFault_Handler
//* Object              : blink LCD backlight four times
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_RESET_VECTOR
//*----------------------------------------------------------------------------
void UsageFault_Handler(void)
{
	ulong i,j;

	// Long pause
	for(i = 0; i < 0xFFFFFFF; i++)
		__asm(".hword 0x46C0");

	// Blink
	for(j = 0; j < 4; j++)
	{
		// On
		mchf_pro_board_blink_if_alive(1|4);
		for(i = 0; i < 0x1FFFFFF; i++)
			__asm(".hword 0x46C0");

		// Off
		mchf_pro_board_blink_if_alive(1);
		for(i = 0; i < 0x1FFFFFF; i++)
			__asm(".hword 0x46C0");
	}

stall:
	goto stall;
}

//*----------------------------------------------------------------------------
//* Function Name       : VectorNotImplementedHandler
//* Object              : blink LCD backlight five times
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_RESET_VECTOR
//*----------------------------------------------------------------------------
void VectorNotImplementedHandler(void)
{
	ulong i,j;

	// Long pause
	for(i = 0; i < 0xFFFFFFF; i++)
		__asm(".hword 0x46C0");

	// Blink
	for(j = 0; j < 5; j++)
	{
		// On
		mchf_pro_board_blink_if_alive(1|4);
		for(i = 0; i < 0x1FFFFFF; i++)
			__asm(".hword 0x46C0");

		// Off
		mchf_pro_board_blink_if_alive(1);
		for(i = 0; i < 0x1FFFFFF; i++)
			__asm(".hword 0x46C0");
	}

stall:
	goto stall;
}

//*----------------------------------------------------------------------------
//* Function Name       : SysTick_Handler
//* Object              : 1mS interrupt for the OS
//* Notes    			:
//* Notes   			: ToDo: Check if interval is really 1mS after clock
//* Notes    			:       setup for various chips
//* Notes    			:
//* Context    			: CONTEXT_IRQ_SYS_TICK
//*----------------------------------------------------------------------------
//uchar tick_flag = 0;
void SysTick_Handler(void)
{
	// HAL functions
	HAL_IncTick();

	// OS scheduler
	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
		xPortSysTickHandler();

	//watchdog_refresh();

#if 0
	// Test tick frequency
	if(tick_flag)
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);

	tick_flag = !tick_flag;
#endif
}

//*----------------------------------------------------------------------------
//* Function Name       : transceive_state_init
//* Object              : publics init
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_RESET_VECTOR
//*----------------------------------------------------------------------------
static void transceiver_state_init(void)
{
	ulong i;

	// Eeprom flags
	tsu.eeprom_init_done	= 0;
	//tsu.eeprom_data_valid 	= 0;

	// Set clocks in use - fix in HW init!!
	//--tsu.main_clk = EXT_16MHZ_XTAL;
	//--tsu.rcc_clk = EXT_32KHZ_XTAL;

	// DSP status
	tsu.dsp_alive 			= 1;	// assume alive
	tsu.dsp_seq_number_old 	= 0;
	tsu.dsp_seq_number	 	= 0;	// track if alive
	tsu.dsp_freq			= 0;	// set as zero ?
	tsu.dsp_volume			= 0;	// set as zero ?

	// Local status
	//tsu.audio_volume		= START_UP_AUDIO_VOL;
	//tsu.vfo_a 				= 0xFFFFFFFF;			// Invalid, do not update DSP
	//tsu.vfo_b 				= 0xFFFFFFFF;			// Invalid, do not update DSP
	//tsu.active_vfo			= VFO_A;
	tsu.step				= 0xFFFFFFFF;			// Invalid, do not update DSP
	//tsu.demod_mode 			= 0xFF;					// Invalid, do not update DSP
	tsu.curr_band			= 0xFF;					// Invalid, do not update DSP
	//tsu.curr_filter			= 0xFF;					// Invalid, do not update DSP
	tsu.step_idx			= 0xFF;					// Invalid, do not update DSP
	//tsu.nco_freq			= 0;					// NCO -20kHz to +20kHz, zero disables translation routines in the the DSP
	//tsu.fixed_mode			= 0;					// vfo in centre mode
	tsu.cw_tx_state			= 0;					// rx mode
	tsu.cw_iamb_type		= 0;					// iambic type, nothing selected

	// DSP API requests
	tsu.update_audio_dsp_req 	= 0;
	tsu.update_freq_dsp_req  	= 0;
	tsu.update_band_dsp_req		= 0;
	tsu.update_demod_dsp_req	= 0;
	tsu.update_filter_dsp_req	= 0;
	tsu.update_nco_dsp_req		= 0;
	tsu.update_dsp_eep_req 		= 0;
	tsu.update_dsp_restart 		= 0;

	// Mute off
	tsu.audio_mute_flag = 0;

	// needed ?
	// Band info structures init with default values
	// need re-load from eeprom
/*	for(i = 0; i < MAX_BANDS; i++)
	{
		// Startup volume
		tsu.band[i].volume 		= START_UP_AUDIO_VOL;
		//
		// VFOs values
		tsu.band[i].vfo_a 		= 0xFFFFFFFF;
		tsu.band[i].vfo_b 		= 0xFFFFFFFF;
		tsu.band[i].step 		= 0xFFFFFFFF;
		tsu.band[i].nco_freq	= 0;
		tsu.band[i].active_vfo	= VFO_A;
		tsu.band[i].fixed_mode	= 0;
		//
		tsu.band[i].filter		= 0xFF;
		//
		tsu.band[i].demod_mode	= 0xFF;
		//
		// Not implemented
		tsu.band[i].dsp_mode 	= 0;
		tsu.band[i].band_start 	= 0;
		tsu.band[i].band_end 	= 0;
	}*/
}

//*----------------------------------------------------------------------------
//* Function Name       : transceiver_load_eep_values
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void transceiver_init_eep_defaults(void)
{
	ulong i;
	//uchar *bkp = (uchar *)0x38800000;

	// Generate band info values
	for(i = 0; i < MAX_BANDS; i++)
	{
		// Startup volume
		tsu.band[i].volume 		= 8;
		tsu.band[i].step 		= T_STEP_1KHZ;
		tsu.band[i].filter		= AUDIO_3P6KHZ;
		tsu.band[i].dsp_mode 	= 0;
		tsu.band[i].nco_freq	= 0;
		tsu.band[i].active_vfo	= VFO_A;
		tsu.band[i].fixed_mode	= 0;

		// VFOs values
		switch(i)
		{
			case BAND_MODE_80:
				tsu.band[i].vfo_a 		= BAND_FREQ_80;
				tsu.band[i].vfo_b 		= BAND_FREQ_80;
				tsu.band[i].band_start 	= BAND_FREQ_80;
				tsu.band[i].band_end 	= (BAND_FREQ_80 + BAND_SIZE_80);
				tsu.band[i].demod_mode	= DEMOD_LSB;
				break;
			case BAND_MODE_60:
				tsu.band[i].vfo_a 		= BAND_FREQ_60;
				tsu.band[i].vfo_b 		= BAND_FREQ_60;
				tsu.band[i].band_start 	= BAND_FREQ_60;
				tsu.band[i].band_end 	= (BAND_FREQ_60 + BAND_SIZE_60);
				tsu.band[i].demod_mode	= DEMOD_LSB;
				break;
			case BAND_MODE_40:
				tsu.band[i].vfo_a 		= BAND_FREQ_40;
				tsu.band[i].vfo_b 		= BAND_FREQ_40;
				tsu.band[i].band_start 	= BAND_FREQ_40;
				tsu.band[i].band_end 	= (BAND_FREQ_40 + BAND_SIZE_40);
				tsu.band[i].demod_mode	= DEMOD_LSB;
				break;
			case BAND_MODE_30:
				tsu.band[i].vfo_a 		= BAND_FREQ_30;
				tsu.band[i].vfo_b 		= BAND_FREQ_30;
				tsu.band[i].band_start 	= BAND_FREQ_30;
				tsu.band[i].band_end 	= (BAND_FREQ_30 + BAND_SIZE_30);
				tsu.band[i].demod_mode	= DEMOD_USB;
				break;
			case BAND_MODE_20:
				tsu.band[i].vfo_a 		= BAND_FREQ_20;
				tsu.band[i].vfo_b 		= BAND_FREQ_20;
				tsu.band[i].band_start 	= BAND_FREQ_20;
				tsu.band[i].band_end 	= (BAND_FREQ_20 + BAND_SIZE_20);
				tsu.band[i].demod_mode	= DEMOD_USB;
				break;
			case BAND_MODE_17:
				tsu.band[i].vfo_a 		= BAND_FREQ_17;
				tsu.band[i].vfo_b 		= BAND_FREQ_17;
				tsu.band[i].band_start 	= BAND_FREQ_17;
				tsu.band[i].band_end 	= (BAND_FREQ_17 + BAND_SIZE_17);
				tsu.band[i].demod_mode	= DEMOD_USB;
				break;
			case BAND_MODE_15:
				tsu.band[i].vfo_a 		= BAND_FREQ_15;
				tsu.band[i].vfo_b 		= BAND_FREQ_15;
				tsu.band[i].band_start 	= BAND_FREQ_15;
				tsu.band[i].band_end 	= (BAND_FREQ_15 + BAND_SIZE_15);
				tsu.band[i].demod_mode	= DEMOD_USB;
				break;
			case BAND_MODE_12:
				tsu.band[i].vfo_a 		= BAND_FREQ_12;
				tsu.band[i].vfo_b 		= BAND_FREQ_12;
				tsu.band[i].band_start 	= BAND_FREQ_12;
				tsu.band[i].band_end 	= (BAND_FREQ_12 + BAND_SIZE_12);
				tsu.band[i].demod_mode	= DEMOD_USB;
				break;
			case BAND_MODE_10:
				tsu.band[i].vfo_a 		= BAND_FREQ_10;
				tsu.band[i].vfo_b 		= BAND_FREQ_10;
				tsu.band[i].band_start 	= BAND_FREQ_10;
				tsu.band[i].band_end 	= (BAND_FREQ_10 + BAND_SIZE_10);
				tsu.band[i].demod_mode	= DEMOD_USB;
				break;
			default:
				break;
		}
	}

	//uchar *dst,*src;
	//ulong size;

	//dst = (bkp + EEP_BANDS);
	//src = (uchar *)(&(tsu.band[0].band_start));
	//size = (MAX_BANDS * sizeof(BAND_INFO));

	//printf("dst = %08x\r\n",dst);
	//printf("src = %08x\r\n",src);
	//printf("size = %08x\r\n",size);

	// All at once
	//memcpy((bkp + EEP_BANDS),(uchar *)(&(tsu.band[0].band_start)),(MAX_BANDS * sizeof(BAND_INFO)));
	save_band_info();

	// ------------------------------------
	// Set as initialised
	WRITE_EEPROM(EEP_BASE_ADDR,0x73);
}

//*----------------------------------------------------------------------------
//* Function Name       : transceiver_load_eep_values
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
static void transceiver_load_eep_values(void)
{
	ulong i;
	uchar *bkp = (uchar *)EEP_BASE;
	uchar r0;

	// Test - force write
	//--WRITE_EEPROM(EEP_BASE_ADDR,0x00);

	// Check if data is valid
	if(READ_EEPROM(EEP_BASE_ADDR) != 0x73)
		transceiver_init_eep_defaults();

	// All at once
	memcpy((uchar *)(&(tsu.band[0].band_start)),(bkp + EEP_BANDS),(MAX_BANDS * sizeof(BAND_INFO)));

#if 0
	// Debug print band values from eeprom
	for(i = 0; i < MAX_BANDS; i++)
	{
		printf("------------------------------\r\n");
		printf("vfo_a = %d\r\n",tsu.band[i].vfo_a);
		printf("vfo_b = %d\r\n",tsu.band[i].vfo_b);
		printf("start = %d\r\n",tsu.band[i].band_start);
		printf("__end = %d\r\n",tsu.band[i].band_end);
		printf("ncofr = %d\r\n",tsu.band[i].nco_freq);

		printf("acvfo = %d\r\n",tsu.band[i].active_vfo);
		printf("c/fix = %d\r\n",tsu.band[i].fixed_mode);
		printf("tstep = %d\r\n",tsu.band[i].step);
		printf("filte = %d\r\n",tsu.band[i].filter);
		printf("demod = %d\r\n",tsu.band[i].demod_mode);
		printf("dspmo = %d\r\n",tsu.band[i].dsp_mode);
		printf("volum = %d\r\n",tsu.band[i].volume);
	}
#endif

	// ---------------------------------------
	// Load audio level
	//r0 = READ_EEPROM(EEP_AUDIO_VOL);
	//printf("r0 = %d\r\n",r0);
	//if(r0 != 0xFF) tsu.audio_volume = r0;
	// ---------------------------------------
	// Load band
	r0 = READ_EEPROM(EEP_CURR_BAND);
	//printf("r0 = %d\r\n",r0);
	if(r0 != 0xFF) tsu.curr_band = r0;
	// ---------------------------------------
	// Load demodulator mode
	//r0 = READ_EEPROM(EEP_DEMOD_MOD);
	//printf("r0 = %d\r\n",r0);
	//if(r0 != 0xFF) tsu.demod_mode = r0;
	// ---------------------------------------
	// Load filter
	//r0 = READ_EEPROM(EEP_CURFILTER);
	//printf("r0 = %d\r\n",r0);
	//if(r0 != 0xFF) tsu.curr_filter = r0;
	// ---------------------------------------
}

//*----------------------------------------------------------------------------
//* Function Name       : threads_launcher
//* Object              : tasks setup
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_RESET_VECTOR
//*----------------------------------------------------------------------------
static void threads_launcher(void)
{
	// ---------------------------------------------------------
	// Drivers communication
	#ifdef CONTEXT_DRIVER_API
	osMessageQDef(api_queue, 5, sizeof(struct APIMessage *));
	hApiMessage = osMessageCreate (osMessageQ(api_queue), NULL);
	#endif
	#ifdef CONTEXT_DRIVER_DSP
	osMessageQDef(dsp_queue, 5, sizeof(struct DSPMessage *));
	hDspMessage = osMessageCreate (osMessageQ(dsp_queue), NULL);
	#endif

	// --------------------------------------------------------------------------------------------------
	// Create API driver
	#ifdef CONTEXT_DRIVER_API
	osThreadDef(api_driver_task_m, 	api_driver_task, 					osPriorityNormal, 		0, 	256);
	osThreadCreate (osThread(api_driver_task_m), NULL);
	#endif
	// --------------------------------------------------------------------------------------------------
	// Create UI driver
	// ToDo: Check stack usage and priority level !!!
	#ifdef CONTEXT_DRIVER_UI
	osThreadDef(ui_driver_task_m, 	ui_driver_task, 					osPriorityNormal, 		0, 	8192);
	osThreadCreate (osThread(ui_driver_task_m), NULL);
	#endif
	// --------------------------------------------------------------------------------------------------
	// Create digitizer driver
	#ifdef CONTEXT_DRIVER_DIGITIZER
	osThreadDef(touch_driver_m, 	touch_driver, 						osPriorityNormal, 		0, 	256);
	osThreadCreate (osThread(touch_driver_m), NULL);
	#endif
	// --------------------------------------------------------------------------------------------------
	// Create keypad driver
	#ifdef CONTEXT_DRIVER_KEYPAD
	osThreadDef(keypad_task_driver_m, 		keypad_driver_task, 		osPriorityNormal, 		0, 	256);
	osThreadCreate (osThread(keypad_task_driver_m), NULL);
	#endif
	// --------------------------------------------------------------------------------------------------
	// Create rotary encoders driver
	#ifdef CONTEXT_ROTARY
	osThreadDef(rotary_driver_m, 	rotary_driver, 						osPriorityNormal, 		0, 	256);
	osThreadCreate (osThread(rotary_driver_m), NULL);
	#endif
	// --------------------------------------------------------------------------------------------------
	// Create SD card driver
	#ifdef CONTEXT_SD
	osThreadDef(sd_driver_task_m, 	sd_driver_task, 					osPriorityNormal, 		0, 	256);
	osThreadCreate (osThread(sd_driver_task_m), NULL);
	#endif
	// --------------------------------------------------------------------------------------------------
	// Create Network driver
	#ifdef CONTEXT_NET
	osThreadDef(net_driver_task_m, 	net_driver_task, 					osPriorityNormal, 		0, 	256);
	osThreadCreate (osThread(net_driver_task_m), NULL);
	#endif
	// --------------------------------------------------------------------------------------------------
	// Create DSP driver
	#ifdef CONTEXT_DRIVER_DSP
	osThreadDef(dsp_driver_task_m, 	dsp_driver_task, 					osPriorityNormal, 		0, 	2048);
	osThreadCreate (osThread(dsp_driver_task_m), NULL);
	#endif
	// --------------------------------------------------------------------------------------------------
}

//*----------------------------------------------------------------------------
//* Function Name       : basic_hw_init
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_RESET_VECTOR
//*----------------------------------------------------------------------------
static void basic_hw_init(void)
{
	// Read core unique regs before any init
	mchf_pro_board_read_cpu_details();

	// Start GPIO clocks
	mchf_pro_board_start_gpio_clocks();

	#ifdef CHIP_H7
	// Extra SRAM clocks on the H7 part
	__HAL_RCC_D2SRAM1_CLK_ENABLE();
	__HAL_RCC_D2SRAM2_CLK_ENABLE();
	__HAL_RCC_D2SRAM3_CLK_ENABLE();
	#endif

	// Configure the MPU
	mchf_pro_board_mpu_config();

	// Enable the CPU Cache
	mchf_pro_board_cpu_cache_enable();

	// STM32F7xx HAL library initialisation:
	HAL_Init();

	// ------------------------------------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------------------------
	// Configure the system clock
	//
	// ToDo: we need proper fall back mechanism, not the default startup clock, otherwise the LCD doesn't work
	//
	//mchf_pro_board_system_clock_config(INT_16MHZ);
	//
	#ifdef CHIP_F7
	mchf_pro_board_system_clock_config(EXT_16MHZ_XTAL);
	#endif
	#ifdef CHIP_H7
	//mchf_pro_board_system_clock_config(EXT_16MHZ_TCXO);
	mchf_pro_board_system_clock_config(EXT_16MHZ_XTAL);
	#endif
	//
	#if 0
	// seems to work
	tsu.main_clk = EXT_16MHZ_TCXO;
	if(mchf_pro_board_system_clock_config(EXT_16MHZ_TCXO))
	{
		tsu.main_clk = INT_16MHZ;
		mchf_pro_board_system_clock_config(INT_16MHZ);
	}
	#endif
	#if 0
	// doesnt work :(
	tsu.main_clk = EXT_16MHZ_XTAL;
	if(mchf_pro_board_system_clock_config(EXT_16MHZ_XTAL))
	{
		tsu.main_clk = INT_16MHZ;
		mchf_pro_board_system_clock_config(INT_16MHZ);
	}
	#endif
	// ------------------------------------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------------------------
	// ------------------------------------------------------------------------------------------------------------

	// Debug print on (after system clock setup)
	mchf_pro_board_swo_init();

	// Initialise the SDRAM - it is mapped at wrong address on H7, ToDo: check it!!!
	#ifdef USE_SDRAM
	BSP_SDRAM_Init();
	#endif

	// Enable CRC to Unlock GUI
	__HAL_RCC_CRC_CLK_ENABLE();

	// Initialise RTC
	tsu.rcc_clk = EXT_32KHZ_XTAL;
	if(mchf_pro_board_rtc_clock_config(EXT_32KHZ_XTAL))
	{
		printf("Error RTC clock! Fallback to internal.\r\n");

		tsu.rcc_clk = INT_32KHZ;
		mchf_pro_board_rtc_clock_config(INT_32KHZ);
	}

	//#ifdef CHIP_F7
	k_CalendarBkupInit();
	//#endif

	// To fix H7 bug, all drivers init here
	mchf_pro_board_sensitive_hw_init();

	// Test GUI init here - seems to cause some trouble booting up
	// ---ui_driver_emwin_528_init();
}

//*----------------------------------------------------------------------------
//* Function Name       : main
//* Object              : entry from asm startup code
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_RESET_VECTOR
//*----------------------------------------------------------------------------
int main(void)
{
	//------mchf_pro_board_blink_if_alive(3);

	// ---------------------------------------
	// Essential system init
	// Debug led
	//--mchf_pro_board_debug_led_init();
	// Check reset reason
	watchdog_check_reset();
	// ---------------------------------------

	// Reset publics
	transceiver_state_init();

	// Init HW
	basic_hw_init();

	// Create drivers
	threads_launcher();

	// Load saved values from EEPROM
	transceiver_load_eep_values();

	// Start watchdog/refresh in vApplicationTickHook() (cpu_utils.c)
	watchdog_init();

	// Start scheduler
	osKernelStart ();

stall_main:
	goto stall_main;

	return 0;
}
