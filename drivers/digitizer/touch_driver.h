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

#ifndef __TOUCH_DRIVER_H
#define __TOUCH_DRIVER_H

//#define DIGIT_ALLOW_PRINT

#define I2C_ADDRESS					0x10
#define FT5X06_ADDRESS				0x70

//	register address
#define FT_REG_DEV_MODE				0x00
#define FT_DEV_MODE_REG_CAL			0x02
#define FT_REG_ID					0xA3
#define FT_REG_PMODE				0xA5
#define FT_REG_FW_VER				0xA6
#define FT_REG_FW_VENDOR_ID			0xA8
#define FT_REG_POINT_RATE			0x88
#define FT_REG_THGROUP				0x80
#define FT_REG_ECC					0xCC
#define FT_REG_RESET_FW				0x07
#define FT_REG_FW_MIN_VER			0xB2
#define FT_REG_FW_SUB_MIN_VER		0xB3

#ifdef CHIP_F7
#define I2C_TIMING      			0x10907295		// 100 kHz
#endif
#ifdef CHIP_H7
// I2C TIMING Register define when I2C clock source is APB1 (SYSCLK/4)
// I2C TIMING is calculated in case of the I2C Clock source is the APB1CLK = 100 MHz
// This example use TIMING to 0x00901954 to reach 400 kHz speed (Rise time = 100 ns, Fall time = 10 ns)
//#define I2C_TIMING      			0x00901954
//
// 50 kHz from the mxcube clock tool
#define I2C_TIMING      			0x50404CFD
#endif

#define	FT5206_TOUCH_DATA_SIZE		31

//#define FT_USE_POLLING
#define FT_USE_IRQ

struct TD {
	ushort 	point_x[5];
	ushort 	point_y[5];
	uchar	count;

	ushort 	old_point_x[5];
	ushort 	old_point_y[5];
	uchar	old_count;

	uchar	pending;
	uchar	pending_count;

	uchar	irq_lock;

	uchar	init_done;
} TD;

//int 	ft5x0x_read_data(void);
//void 	ft5x0x_report_value(void);

uchar 	ft5x06_i2c_init(void);

void 	touch_driver(void const * argument);
//void 	check_touch(void);

#endif 

