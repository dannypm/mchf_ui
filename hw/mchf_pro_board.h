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
#ifndef __MCHF_PRO_BOARD_H
#define __MCHF_PRO_BOARD_H
//
// -----------------------------------------------------------------------------
// Version and device id
//
#define	DEVICE_STRING				"mcHF Pro"
#define	AUTHOR_STRING				"Krassi Atanassov/M0NKA 2012-2019"
//
#define	MCHFX_VER_MAJOR				0
#define	MCHFX_VER_MINOR				0
#define	MCHFX_VER_RELEASE			0
#define	MCHFX_VER_BUILD				62
//
// -----------------------------------------------------------------------------
//
// Chip selected based on active build configuration
// If it doesn't work, check: Window->Preferences->Indexer->Build configuration for the indexer->Use active build configuration
#if defined(STM32F746xx)
#define CHIP_F7
#endif
#if defined(STM32H753xx)
#define CHIP_H7
// ----------------------------------------
// Optimisations - Only O0 crashes the code
// ----------------------------------------
#endif
//
// Need to have both files the same time as we support
// the STM32F746IGT6 and STM32H753IIT6 in the same firmware
#ifdef CHIP_F7
// CPU HW definition file
#include "stm32f746xx.h"
// allow HAL modules, but .h files in each driver
// -- Generic --
#define 							F7_HAL_RCC_MODULE_ENABLED
#define 							F7_HAL_GPIO_MODULE_ENABLED
#define 							F7_HAL_PWR_MODULE_ENABLED
#define 							F7_HAL_CORTEX_MODULE_ENABLED
#define 							F7_HAL_FLASH_MODULE_ENABLED
#define 							F7_HAL_DMA_MODULE_ENABLED
// -- UI driver (LCD) --
#define 							F7_HAL_DMA2D_MODULE_ENABLED
#define 							F7_HAL_LTDC_MODULE_ENABLED
#define 							F7_HAL_SDRAM_MODULE_ENABLED
#define 							F7_HAL_RTC_MODULE_ENABLED
// -- Digitizer driver --
#define 							F7_HAL_I2C_MODULE_ENABLED
// -- Signal driver --
#define 							F7_HAL_RNG_MODULE_ENABLED
// -- Keypad LED driver --
#define 							F7_HAL_SPI_MODULE_ENABLED
// LCD brightness
#define 							F7_HAL_TIM_MODULE_ENABLED
//
#endif
#ifdef CHIP_H7
#include "stm32h753xx.h"
// -- Generic --
#define 							H7_HAL_RCC_MODULE_ENABLED
#define 							H7_HAL_GPIO_MODULE_ENABLED
#define 							H7_HAL_PWR_MODULE_ENABLED
#define 							H7_HAL_CORTEX_MODULE_ENABLED
#define 							H7_HAL_FLASH_MODULE_ENABLED
#define 							H7_HAL_DMA_MODULE_ENABLED
// -- UI driver (LCD) --
#define 							H7_HAL_MDMA_MODULE_ENABLED
#define 							H7_HAL_DMA2D_MODULE_ENABLED
#define 							H7_HAL_LTDC_MODULE_ENABLED
#define 							H7_HAL_SDRAM_MODULE_ENABLED
#define 							H7_HAL_RTC_MODULE_ENABLED
// -- Digitizer driver --
#define 							H7_HAL_I2C_MODULE_ENABLED
// -- Signal driver --
#define 							H7_HAL_RNG_MODULE_ENABLED
// -- Keypad LED driver --
#define 							H7_HAL_SPI_MODULE_ENABLED
// LCD brightness
#define 							H7_HAL_TIM_MODULE_ENABLED
// SD card
#define								H7_HAL_SD_MODULE_ENABLED
// watchdog
#define								H7_HAL_WWDG_MODULE_ENABLED
// Network driver
#define								F7_HAL_UART_MODULE_ENABLED
//
//
#define EEP_BASE					0x38800000
#endif
//
#include "mchf_types.h"
#include "mchf_radio_decl.h"
#include "cmsis_os.h"
//
#include "cpu_utils.h"
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//
// What board are we running on ?
//
// - need this in final release at all ?
//
//#define BOARD_DISCO
#define BOARD_MCHF
//
#define PCB_REV_26JULY18
//
// -----------------------------------------------------------------------------
// Are we using internal CPU RAM or external SDRAM
//
// - bad soldering/connection with SDRAM will cause the emWin to hang in GUI_init()
// - might be safe to boot up with a simple UI, run some HW tests, then do full boot
//
//#define USE_INT_RAM
#define USE_SDRAM
//
#define	EMWIN_RAM_SIZE 				(1024*1024)
//
// -----------------------------------------------------------------------------
// Oscillators configuration
//
// - Main
#define INT_16MHZ					0
#define EXT_16MHZ_XTAL				1
#define EXT_16MHZ_TCXO				2
// - Real Time Clock
#define INT_32KHZ					0
#define EXT_32KHZ_XTAL				1
//
#define HSE_VALUE    				((uint32_t)16000000)
#define HSI_VALUE    				((uint32_t)16000000)
#define CSI_VALUE    				((uint32_t)4000000)
//
//#define VECT_TAB_OFFSET  			0x00				// alpha stage, base of flash
#define VECT_TAB_OFFSET  			0x20000				// sector 1, we have bootloader at base
//
// -----------------------------------------------------------------------------
//
#ifdef CHIP_F7
#define		CHIP_UNIQUE_ID			0x1FF0F420
#define		CHIP_FLS_SIZE			0x1FF0F442
#endif
#ifdef CHIP_H7
#define		CHIP_UNIQUE_ID			0x1FF1E800
#define		CHIP_FLS_SIZE			0x1FF1E880
#endif
//
// -----------------------------------------------------------------------------
// Some better looking OS macros
//
#define EnterCriticalSection		taskENTER_CRITICAL
#define ExitCriticalSection			taskEXIT_CRITICAL
#define OsDelayMs					osDelay
//
// -----------------------------------------------------------------------------
// Drivers start delays
// - start the UI driver first, the rest with delay, so UI can be painted first
//
#define BOOT_UP_LCD_DELAY			2000
//
#define SIGNAL_DRV_START_DELAY		500
#define TOUCHC_DRV_START_DELAY		1000
#define KEYPAD_DRV_START_DELAY		1500
#define KEYLED_DRV_START_DELAY		2000
//
// -----------------------------------------------------------------------------
// Firmware execution context
// if this project becomes community effort eventually, it makes sense to mark
// each function with the task context executes into, honestly will save a lot
// of future headache!
//
// Nothing much here except quick initial stack setup, basic hw init, MMU On
// clocks and start the OS
#define CONTEXT_RESET_VECTOR
//
// Don't place code here, don't make it beautiful, just setting of flags
// and super quick copy of data
#define CONTEXT_IRQ_SYS_TICK
#define CONTEXT_IRQ_LTDC
//
// You can go wild in those drivers, just make sure each task has enough stack
// allocated on startup. Also careful of HW access, each task ideally should
// use only own HW core modules. Also need to be super careful of generic
// c lib stuff like printf, memcpy, '/', '%' etc that usually gets compiled
// as shared resource into the CONTEXT_RESET_VECTOR space! Use as much
// critical sections as needed, but on functions that needs fast execution
// like freq_update(), use custom low level implementation, instead of
// execution blocking
//
// -- Following defines could be used to stop specific drivers
//
// - working on both chips
#define CONTEXT_DRIVER_UI
#define CONTEXT_DRIVER_KEYPAD
#define CONTEXT_DRIVER_DIGITIZER
//#define CONTEXT_DRIVER_KEYPAD_LED
#define CONTEXT_DRIVER_API
#define CONTEXT_ROTARY
// - not ported to F7 yet :(
#ifdef CHIP_H7
#define CONTEXT_SD					// ToDo: Make possible full disable of this driver!
//#define CONTEXT_NET
#endif
//#define CONTEXT_DRIVER_SIGNAL
// -----------------------------------------------------------------------------
//
//#define H7_200MHZ
#define H7_400MHZ
//
// -----------------------------------------------------------------------------
// Nice facility for testing the analogue s-meter via side encoder
//
//#define USE_SIDE_ENC_FOR_S_METER
//
// -----------------------------------------------------------------------------
//
#define	VFO_A					0
#define	VFO_B					1
//
// -----------------------------------------------------------------------------
// Public UI driver state
#define	MODE_DESKTOP			0
#define MODE_MENU				1
//
struct UI_DRIVER_STATE {
	// Current LCD state - Desktop or Menu
	// Request flag, updated by keypad driver, read only for the UI task
	uchar	req_state;
	// Accepted state by the UI driver(read/write local)
	uchar	cur_state;
	// Flag to lock out keyboard driver request updates while GUI repaints
	uchar	lock_requests;

	uchar	dummy;
} UI_DRIVER_STATE;
//
struct KEYPAD_STATE {

	uchar	curr_button_id;
	uchar	hold_time;

} KEYPAD_STATE;
//
// -----------------------------------------------------------------------------
// Hardware regs, read before MMU init
struct CM7_CORE_DETAILS {
	// X and Y coordinates on the wafer
	ulong 	wafer_coord;
	// Bits 31:8 UID[63:40]: LOT_NUM[23:0] 	- Lot number (ASCII encoded)
	// Bits 7:0 UID[39:32]: WAF_NUM[7:0] 	- Wafer number (8-bit unsigned number)
	ulong	wafer_number;
	// Bits 31:0 UID[95:64]: LOT_NUM[55:24]	- Lot number (ASCII encoded)
	ulong	lot_number;
	// Flash size
	ushort 	fls_size;
	ushort	dummy;
} CM7_CORE_DETAILS;
//
// Per band settings
struct BAND_INFO {
	// Frequency boundary
	ulong 	band_start;
	//
	ulong	band_end;
	// Common controls
	uchar 	volume;
	uchar 	demod_mode;
	uchar 	filter;
	uchar 	dsp_mode;
	//
	ulong	step;
	// Frequency
	ulong	vfo_a;
	//
	ulong	vfo_b;
	//
	short 	nco_freq;
	uchar	fixed_mode;
	uchar   active_vfo;			// A, B
	//
	// Align four!
} BAND_INFO;
//
// Public structure to hold current radio state
struct TRANSCEIVER_STATE_UI {

	// Per band info
	struct BAND_INFO	band[MAX_BANDS];

	// -----------------------------
	// Clock to use
	uchar 	main_clk;
	// RCC clock to use
	uchar 	rcc_clk;
	ushort 	dummy1;
	// --
	// -----------------------------
	// DSP status
	uchar 	dsp_alive;
	uchar 	dsp_seq_number_old;
	uchar 	dsp_seq_number;
	uchar 	dsp_blinker;
	// --
	uchar 	dsp_rev1;
	uchar 	dsp_rev2;
	uchar 	dsp_rev3;
	uchar 	dsp_rev4;
	// --
	ulong 	dsp_freq;
	short 	dsp_nco_freq;
	uchar 	dsp_step_idx;
	uchar 	step_idx;
	// --
	uchar 	dsp_volume;
	uchar 	dsp_filter;
	uchar   dsp_band;
	uchar 	dsp_demod;
	// --
	// -----------------------------
	// Local status
	//uchar 	audio_volume;
	//uchar 	demod_mode;
	uchar 	curr_band;
	//uchar 	curr_filter;
	// --
	ulong	step;
	// --
	//ulong	vfo_a;
	// --
	//ulong	vfo_b;
	// --
	//short 	nco_freq;
	//uchar	fixed_mode;
	//uchar   active_vfo;			// A, B
	// --
	uchar	cw_tx_state;		// 0 - idle, 1 - on, 2 - release
	uchar	cw_iamb_type;

	uchar	eeprom_init_done;
	//uchar	eeprom_data_valid;
	//
	// Request flags for CPU->DSP comm
	uchar	update_audio_dsp_req;
	uchar	update_freq_dsp_req;
	uchar	update_band_dsp_req;
	uchar	update_demod_dsp_req;
	uchar	update_filter_dsp_req;

	uchar	update_nco_dsp_req;

	// Always align last member!

} TRANSCEIVER_STATE_UI;
//
#define	SW_CONTROL_BIG				0
#define	SW_CONTROL_MID				1
#define	SW_CONTROL_SMALL			2
//
// Spectrum/Waterfall control publics
struct UI_SW {
	//
	// screen pixel data
	uchar 	fft_value[1024];
	// FFT array from DSP
	uchar 	fft_dsp[256];
	//
	uchar 	ctrl_type;
	//
	uchar	dummy1;
	// --
	ushort 	bandpass_start;
	ushort 	bandpass_end;

} UI_SW;
// -----------------------------------------------------------------------------
// Virtual Eeprom in BackUp SRAM access macros
//
// assumes: 1. Battery connected to VBAT pin
//			2. External 32 kHz LSE, all clocks enabled(RCC_OSCILLATORTYPE_LSE,RCC_LSE_ON)
//			3. Write access to Backup domain enabled (PWR->CR1 |= PWR_CR1_DBP)
//			4. Enabled BKPRAM clock (__HAL_RCC_BKPRAM_CLK_ENABLE())
//			5. Enabled Backup SRAM low power Regulator (HAL_PWREx_EnableBkUpReg())
//
void  WRITE_EEPROM(ushort addr,uchar value);
uchar READ_EEPROM (ushort addr);
#define save_band_info()	{memcpy((uchar *)(EEP_BASE + EEP_BANDS),(uchar *)(&(tsu.band[0].band_start)),(MAX_BANDS * sizeof(BAND_INFO)));}
//
// -----------------------------------------------------------------------------
// HAL compatibility
#define assert_param(expr) ((void)0)
//
typedef enum
{
  RESET = 0,
  SET = !RESET
} FlagStatus, ITStatus;
//
typedef enum
{
  DISABLE = 0,
  ENABLE = !DISABLE
} FunctionalState;
#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))
//
typedef enum
{
  ERROR = 0,
  SUCCESS = !ERROR
} ErrorStatus;
//
#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)    ((REG) & (BIT))
#define CLEAR_REG(REG)        ((REG) = (0x0))
#define WRITE_REG(REG, VAL)   ((REG) = (VAL))
#define READ_REG(REG)         ((REG))
#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))
#define POSITION_VAL(VAL)     (__CLZ(__RBIT(VAL)))

// missing from f7_hal, but present on h7_hal
#ifdef CHIP_F7
#define HSE_STARTUP_TIMEOUT		((uint32_t)500)
#define TICK_INT_PRIORITY     	((uint32_t)0x0F)
#define EXTERNAL_CLOCK_VALUE    ((uint32_t)12288000)
#endif
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void 	mchf_pro_board_debug_led_init(void);
void 	mchf_pro_board_blink_if_alive(uchar flags);

void 	mchf_pro_board_read_cpu_details(void);
void 	mchf_pro_board_start_gpio_clocks(void);

void 	mchf_pro_board_mpu_config(void);
void 	mchf_pro_board_cpu_cache_enable(void);

uchar 	mchf_pro_board_system_clock_config(uchar clk_src);
uchar 	mchf_pro_board_rtc_clock_config(uchar clk_src);
void 	mchf_pro_board_rtc_clock_disable(void);

void 	mchf_pro_board_swo_init(void);
void 	mchf_pro_board_mco2_on(void);

void 	mchf_pro_board_sensitive_hw_init(void);

#endif
