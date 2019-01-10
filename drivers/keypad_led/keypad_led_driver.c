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

//#include "main.h"
#include "mchf_pro_board.h"

#include "keypad_led_driver.h"

// Supposed to be old implementation, where we have separate
// driver for Keypad LEDs control
// - better solution, do this in actual keypad driver

#ifdef CONTEXT_DRIVER_KEYPAD_LED

uchar btn_id = 0;
ushort pwmbuffer[2*24*1];

#ifdef CHIP_F7
#include "stm32f7xx_hal_gpio.h"
#endif
#ifdef CHIP_H7
#include "stm32h7xx_hal_gpio.h"
#endif

static void keypad_led_mini_delay(void)
{
	uchar i;

	for(i = 0; i < 200; i++)
		__asm(".word 0x46004600");
}

static void keypad_led_shift(void)
{
	KEYLED_XLAT_PORT->BSRRH = KEYLED_XLAT_PIN;
	keypad_led_mini_delay();

	  // 24 channels per TLC5974
	  for (int16_t c=24*1 - 1; c >= 0 ; c--)
	  {
	    // 12 bits per channel, send MSB first
	    for (int8_t b=11; b>=0; b--)
	    {
	    	// Clock low
	    	KEYLED_SCK_PORT->BSRRH = KEYLED_SCK_PIN;
	    	keypad_led_mini_delay();

	    	// Next data bit
	    	if (pwmbuffer[c] & (1 << b))
	    		KEYLED_MOSI_PORT->BSRRL = KEYLED_MOSI_PIN;
	    	else
	    		KEYLED_MOSI_PORT->BSRRH = KEYLED_MOSI_PIN;

	    	// Clock high
	    	KEYLED_SCK_PORT->BSRRL = KEYLED_SCK_PIN;
	    	keypad_led_mini_delay();
	    }
	  }

	  // Clock it out on the Latch pin
	  KEYLED_XLAT_PORT->BSRRH = KEYLED_XLAT_PIN;
	  keypad_led_mini_delay();

	  KEYLED_XLAT_PORT->BSRRL = KEYLED_XLAT_PIN;
	  keypad_led_mini_delay();

	  KEYLED_XLAT_PORT->BSRRH = KEYLED_XLAT_PIN;
	  keypad_led_mini_delay();
}

static void blink_all(void)
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

		keypad_led_shift();
		// --------------------------------
}

// Set up blank early on, to prevent
// LEDs being lit up on start
void keypad_led_prevent_startup_blink(void)
{
	GPIO_InitTypeDef  gpio_init_structure;

	// BLANK - PI8
	gpio_init_structure.Pin 	= KEYLED_BLANK_PIN;
	gpio_init_structure.Mode 	= GPIO_MODE_OUTPUT_PP;
	gpio_init_structure.Pull 	= GPIO_PULLUP;
	gpio_init_structure.Speed 	= GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(KEYLED_BLANK_PORT, &gpio_init_structure);

	// BLANK high - all off
	//HAL_GPIO_WritePin(KEYLED_BLANK_PORT, KEYLED_BLANK_PIN, GPIO_PIN_SET);
	KEYLED_BLANK_PORT->BSRRL = KEYLED_BLANK_PIN;
}

uchar start_counter = 0;

void keypad_led_driver(void const * argument)
{
	// Delay start, so UI can paint properly
	OsDelayMs(KEYLED_DRV_START_DELAY);

	printf("keypad led driver start\r\n");

	// --------------------------------------
	// Moved to main()
	// Init HW
	//--keypad_led_init();
	// --------------------------------------

	// BLANK low - all LEDs on
	//HAL_GPIO_WritePin(KEYLED_BLANK_PORT, KEYLED_BLANK_PIN, GPIO_PIN_RESET);
	KEYLED_BLANK_PORT->BSRRH = KEYLED_BLANK_PIN;

keypadled_driver_loop:

	if(start_counter < 24)
	{
		blink_all();
		start_counter++;
	}

	// Every 200 mS
	OsDelayMs(50);

	goto keypadled_driver_loop;
}

#endif

void keypad_led_driver_init(void)
{
#ifdef CONTEXT_DRIVER_KEYPAD_LED
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
#endif
}
