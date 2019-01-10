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
#ifndef __KEYPAD_LED_DRIVER_H
#define __KEYPAD_LED_DRIVER_H

#ifdef CONTEXT_DRIVER_KEYPAD_LED

// TEMP pin decl
#define KEYLED_XLAT_PIN		GPIO_PIN_11
#define KEYLED_XLAT_PORT	GPIOI
#define KEYLED_BLANK_PIN	GPIO_PIN_8
#define KEYLED_BLANK_PORT	GPIOI

#define KEYLED_SCK_PIN		GPIO_PIN_5
#define KEYLED_SCK_PORT		GPIOA
#define KEYLED_MOSI_PIN		GPIO_PIN_5
#define KEYLED_MOSI_PORT	GPIOB

void keypad_led_prevent_startup_blink(void);
void keypad_led_driver(void const * argument);

#endif

void keypad_led_driver_init(void);

#endif
