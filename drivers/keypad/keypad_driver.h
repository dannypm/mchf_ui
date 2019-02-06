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
#ifndef __KEYPAD_DRIVER_H
#define __KEYPAD_DRIVER_H

//#define KEYPAD_ALLOW_DEBUG

#ifdef CONTEXT_DRIVER_KEYPAD

// TEMP pin decl
#define KEYPAD_PIN_Y1	GPIO_PIN_5
#define KEYPAD_PORT_Y1	GPIOC

#define KEYPAD_PIN_Y2	GPIO_PIN_1
#define KEYPAD_PORT_Y2	GPIOB

#define KEYPAD_PIN_Y3	GPIO_PIN_2
#define KEYPAD_PORT_Y3	GPIOB

#define KEYPAD_PIN_Y4	GPIO_PIN_6
#define KEYPAD_PORT_Y4	GPIOH

#define KEYPAD_PIN_X1	GPIO_PIN_2
#define KEYPAD_PORT_X1	GPIOE

#define KEYPAD_PIN_X2	GPIO_PIN_13
#define KEYPAD_PORT_X2	GPIOC

#define KEYPAD_PIN_X3	GPIO_PIN_0
#define KEYPAD_PORT_X3	GPIOC

#define KEYPAD_PIN_X4	GPIO_PIN_2
#define KEYPAD_PORT_X4	GPIOH

#define KEYPAD_PIN_X5	GPIO_PIN_4
#define KEYPAD_PORT_X5	GPIOH

#define KEYPAD_PIN_X6	GPIO_PIN_4
#define KEYPAD_PORT_X6	GPIOC

#define scan_y1() 	{	KEYPAD_PORT_Y1->BSRRH = KEYPAD_PIN_Y1;\
						KEYPAD_PORT_Y2->BSRRL = KEYPAD_PIN_Y2;\
						KEYPAD_PORT_Y3->BSRRL = KEYPAD_PIN_Y3;\
						KEYPAD_PORT_Y4->BSRRL = KEYPAD_PIN_Y4;\
					}

#define scan_y2() 	{	KEYPAD_PORT_Y1->BSRRL = KEYPAD_PIN_Y1;\
						KEYPAD_PORT_Y2->BSRRH = KEYPAD_PIN_Y2;\
						KEYPAD_PORT_Y3->BSRRL = KEYPAD_PIN_Y3;\
						KEYPAD_PORT_Y4->BSRRL = KEYPAD_PIN_Y4;\
					}

#define scan_y3() 	{	KEYPAD_PORT_Y1->BSRRL = KEYPAD_PIN_Y1;\
						KEYPAD_PORT_Y2->BSRRL = KEYPAD_PIN_Y2;\
						KEYPAD_PORT_Y3->BSRRH = KEYPAD_PIN_Y3;\
						KEYPAD_PORT_Y4->BSRRL = KEYPAD_PIN_Y4;\
					}

#define scan_y4() 	{	KEYPAD_PORT_Y1->BSRRL = KEYPAD_PIN_Y1;\
						KEYPAD_PORT_Y2->BSRRL = KEYPAD_PIN_Y2;\
						KEYPAD_PORT_Y3->BSRRL = KEYPAD_PIN_Y3;\
						KEYPAD_PORT_Y4->BSRRH = KEYPAD_PIN_Y4;\
					}

#define scan_off() 	{	KEYPAD_PORT_Y1->BSRRL = KEYPAD_PIN_Y1;\
						KEYPAD_PORT_Y2->BSRRL = KEYPAD_PIN_Y2;\
						KEYPAD_PORT_Y3->BSRRL = KEYPAD_PIN_Y3;\
						KEYPAD_PORT_Y4->BSRRL = KEYPAD_PIN_Y4;\
					}

#define KEYLED_XLAT_PIN		GPIO_PIN_11
#define KEYLED_XLAT_PORT	GPIOI
#define KEYLED_BLANK_PIN	GPIO_PIN_8
#define KEYLED_BLANK_PORT	GPIOI

#define KEYLED_SCK_PIN		GPIO_PIN_5
#define KEYLED_SCK_PORT		GPIOA
#define KEYLED_MOSI_PIN		GPIO_PIN_5
#define KEYLED_MOSI_PORT	GPIOB

#define KEY_LED_SSB			0
#define KEY_LED_ONE			1
#define KEY_LED_TWO			2
#define KEY_LED_THREE		3
#define KEY_LED_M			4
#define KEY_LED_DSP			5
#define KEY_LED_CW			6
#define KEY_LED_FOUR		7
#define KEY_LED_FIVE		8
#define KEY_LED_SIX			9
#define KEY_LED_S			10
#define KEY_LED_STEP		11
#define KEY_LED_AM			12
#define KEY_LED_SEVEN		13
#define KEY_LED_EIGHT		14
#define KEY_LED_NINE		15
#define KEY_LED_ENTER		16
#define KEY_LED_FILTER		17
#define KEY_LED_FIX			18
#define KEY_LED_DOT			19
#define KEY_LED_ZERO		20
#define KEY_LED_C			21
#define KEY_LED_SPLIT		22

#define KEY_LED_OFF_LIGHT	0
#define KEY_LED_LOW_LIGHT	64
#define KEY_LED_MID_LIGHT	96
#define KEY_LED_HIGH_LIGHT	128

struct KEYPAD_STATE {

	// Keypad
	ulong	tap_cnt;
	uchar	tap_id;

	// LEDs
	uchar 	btn_id;
	ushort 	pwmbuffer[2*24*1];
	uchar 	start_counter;

} KEYPAD_STATE;

void keypad_driver_init(void);
void keypad_driver_task(void const * argument);

#endif

#endif
