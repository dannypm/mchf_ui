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

//#include "gui.h"
//#include "dialog.h"
//#include "ST_GUI_Addons.h"

#include "touch_driver.h"

struct TD 			t_d;

// Public radio state
extern struct	TRANSCEIVER_STATE_UI	tsu;
extern struct	UI_DRIVER_STATE			ui_s;

// ToDo: Super stupid solution via public buffer
// - fix!
uchar digitizer_info[100];
uchar di_curr;

static int ft5x06_i2c_read(uchar *writebuf,ushort writelen, uchar *readbuf, ushort readlen);
static void check_touch(void);

#ifdef CHIP_F7
#include "stm32f7xx_hal_i2c.h"
#include "stm32f7xx_hal_gpio.h"
#include "stm32f7xx_hal_cortex.h"
#include "stm32f7xx_hal_rcc.h"
#endif
#ifdef CHIP_H7
#include "stm32h7xx_hal_i2c.h"
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_rcc.h"
#endif

I2C_HandleTypeDef 	I2C1Handle;

#ifdef FT_USE_IRQ
void EXTI9_5_IRQHandler(void)
{
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_8) != RESET)
	{
		// Switch to pressed
		t_d.irq_lock = 1;

		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_8);
	}
}
#endif

static void ft5x06_hw_init(void)
{
#ifdef CHIP_F7
	GPIO_InitTypeDef  			GPIO_InitStruct;
	RCC_PeriphCLKInitTypeDef  	RCC_PeriphCLKInitStruct;

	// Configure the I2C clock source. The clock is derived from the SYSCLK
	RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
	RCC_PeriphCLKInitStruct.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
	HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);

	// Enable I2Cx clock
	__HAL_RCC_I2C1_CLK_ENABLE();

	// I2C pin configuration
	GPIO_InitStruct.Pin       	= GPIO_PIN_6|GPIO_PIN_7;
	GPIO_InitStruct.Mode      	= GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull      	= GPIO_PULLUP;
	GPIO_InitStruct.Speed     	= GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate 	= GPIO_AF4_I2C1;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	// Reset pin init
	GPIO_InitStruct.Pin 		= GPIO_PIN_6;
	GPIO_InitStruct.Mode 		= GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull 		= GPIO_PULLUP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	#ifdef FT_USE_IRQ
	GPIO_InitStruct.Mode 		= GPIO_MODE_IT_FALLING;
	#endif
	#ifdef FT_USE_POLLING
	GPIO_InitStruct.Mode 		= GPIO_MODE_INPUT;
	#endif

	// Interrupt pin init
	GPIO_InitStruct.Pin 		= GPIO_PIN_8;
	GPIO_InitStruct.Pull 		= GPIO_PULLUP;					// ToDo: use pull up or debounce routine!
	GPIO_InitStruct.Speed 		= GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	#ifdef FT_USE_IRQ
	//Init interrupt
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	#endif

#endif
#ifdef CHIP_H7
	GPIO_InitTypeDef  			GPIO_InitStruct;
	RCC_PeriphCLKInitTypeDef  	RCC_PeriphCLKInitStruct;

	/*##-1- Configure the I2C clock source. The clock is derived from the SYSCLK #*/
	RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C123;
	RCC_PeriphCLKInitStruct.I2c123ClockSelection = RCC_I2C123CLKSOURCE_D2PCLK1;
	HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);;

	__HAL_RCC_I2C1_CLK_ENABLE();

	// I2C pin configuration
	GPIO_InitStruct.Pin       	= GPIO_PIN_6|GPIO_PIN_7;
	GPIO_InitStruct.Mode      	= GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull      	= GPIO_PULLUP;
	GPIO_InitStruct.Speed     	= GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate 	= GPIO_AF4_I2C1;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	// Reset pin init
	GPIO_InitStruct.Pin			= GPIO_PIN_6;
	GPIO_InitStruct.Mode 		= GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull 		= GPIO_PULLUP;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	#ifdef FT_USE_IRQ
	GPIO_InitStruct.Mode 		= GPIO_MODE_IT_FALLING;
	#endif
	#ifdef FT_USE_POLLING
	GPIO_InitStruct.Mode 		= GPIO_MODE_INPUT;
	#endif

	// Interrupt pin init
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	//--GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;		// ToDo: use pull up or debounce routine!
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	#ifdef FT_USE_IRQ
	//Init interrupt
	NVIC_SetPriority(EXTI9_5_IRQn, 2);
	NVIC_EnableIRQ(EXTI9_5_IRQn);
	#endif

#endif
}

// export!
// only this ?? But not ft5x06_hw_init() ??
uchar ft5x06_i2c_init(void)
{
	// Hardware init
	ft5x06_hw_init();

	// Configure the I2C peripheral
	I2C1Handle.Instance             = I2C1;
	I2C1Handle.Init.Timing          = I2C_TIMING;
	I2C1Handle.Init.OwnAddress1     = I2C_ADDRESS;
	I2C1Handle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
	I2C1Handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	I2C1Handle.Init.OwnAddress2     = 0xFF;
	I2C1Handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	I2C1Handle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;

	if(HAL_I2C_Init(&I2C1Handle) != HAL_OK)
		return 1;

	#ifdef CHIP_H7
	// Enable the Analog I2C Filter
	HAL_I2CEx_ConfigAnalogFilter(&I2C1Handle,I2C_ANALOGFILTER_ENABLE);
	#endif

	return 0;
}

static void ft5x06_hw_reset(void)
{
	// Reset touch controller, min 5mS
	//HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, GPIO_PIN_RESET);
	GPIOF->BSRRH = GPIO_PIN_6;
	osDelay(50);
	//HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, GPIO_PIN_SET);
	GPIOF->BSRRL = GPIO_PIN_6;

	// Long after reset delay for proper I2C comm (at least 100mS)
	osDelay(200);
}

#if 0
static void scan_i2c_bus(void)
{
	 unsigned char chipId = 0,err,i;

	 /* Try to get chip id */
	 uint8_t data = FT5X0X_REG_FIRMID;

	// Scan bus
	for(i = 0; i < 255; i++)
	{
		osDelay(10);
		data = FT5X0X_REG_FIRMID;

		err = HAL_I2C_Master_Transmit(&I2C1Handle, (uint16_t)i, (uint8_t*)&data, 1,10000);

		if(err == HAL_OK)
		{
			print_text("success");
			print_int("adr",i);
			break;
		}
	}
}
#endif

//*----------------------------------------------------------------------------
//* Function Name       : ft5x06_i2c_read
//* Object              : I2C transfer with the digitizer controller
//* Input Parameters    :
//* Output Parameters   : no printf if called by IRQ!!!
//* Functions called    :
//*----------------------------------------------------------------------------
static int ft5x06_i2c_read(uchar *writebuf,ushort writelen, uchar *readbuf, ushort readlen)
{
	uchar err;

	//printf("ft5x06_i2c_read->in\r\n");

	if(writelen == 0)
		goto read_only;

	if(writebuf == NULL)
		return 10;

	err = HAL_I2C_Master_Transmit(&I2C1Handle, FT5X06_ADDRESS, writebuf, writelen,1000);
	if(err != HAL_OK)
	{
		//printf("ft5x06_i2c_read->err write: %d\r\n",err);
		return 1;
	}
	while (HAL_I2C_GetState(&I2C1Handle) != HAL_I2C_STATE_READY);

read_only:

	if(readbuf == NULL)
		return 10;

	if(readlen == 0)
		return 10;

	err = HAL_I2C_Master_Receive(&I2C1Handle, FT5X06_ADDRESS, readbuf, readlen,1000);
	if(err != HAL_OK)
	{
		//printf("ft5x06_i2c_read->err read: %d\r\n",err);
		return 2;
	}

	//printf("ft5x06_i2c_read->out\r\n");
	return 0;
}

//*----------------------------------------------------------------------------
//* Function Name       : ft5x06_i2c_write
//* Object              : I2C transfer (out only) with the digitizer controller
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
static int ft5x06_i2c_write(uchar *writebuf,ushort writelen)
{
	uchar err;

	//printf("ft5x06_i2c_write->in\r\n");

	if(writelen == 0)
		return 10;

	if(writebuf == NULL)
		return 10;

	err = HAL_I2C_Master_Transmit(&I2C1Handle, FT5X06_ADDRESS, writebuf, writelen,1000);
	if(err != HAL_OK)
	{
		#ifdef DIGIT_ALLOW_PRINT
		printf("ft5x06_i2c_read->err write: %d\r\n",err);
		#endif
		return 1;
	}
	while (HAL_I2C_GetState(&I2C1Handle) != HAL_I2C_STATE_READY);

	//printf("ft5x06_i2c_write->out\r\n");

	return 0;
}

static int ft5x06_get_id(void)
{
	uchar buff[50],err,i;
	uchar reg;

	//printf("ft5x06_get_id\r\n");

	reg = FT_REG_ID;

	if(ft5x06_i2c_read(&reg,1,buff,1) != 0)
		return 1;

	//printf("chip id = 0x%02x\r\n",buff[0]);
	di_curr += sprintf(digitizer_info + di_curr,",0x%02x",buff[0]);

	reg = FT_REG_FW_VENDOR_ID;

	if(ft5x06_i2c_read(&reg,1,buff,1) != 0)
		return 2;

	//printf("vendor id = 0x%02x\r\n",buff[0]);
	di_curr += sprintf(digitizer_info + di_curr,"%02x",buff[0]);

	reg = FT_REG_FW_VER;

	if(ft5x06_i2c_read(&reg,1,(buff + 0),1) != 0)
		return 3;

	reg = FT_REG_FW_MIN_VER;

	if(ft5x06_i2c_read(&reg,1,(buff + 1),1) != 0)
		return 4;

	reg = FT_REG_FW_SUB_MIN_VER;

	if(ft5x06_i2c_read(&reg,1,(buff + 2),1) != 0)
		return 5;

	//printf("Firmware version = %d.%d.%d\r\n",buff[0],buff[1],buff[2]);
	di_curr += sprintf(digitizer_info + di_curr,",fw:%d.%d.%d",buff[0],buff[1],buff[2]);

	return 0;
}

static int post_reset_init(void)
{
	uchar err;
	uchar buff[10];

	//printf("post_reset_init\r\n");

	buff[0] = FT_REG_RESET_FW;

	if(ft5x06_i2c_write(buff,1) != 0)
		return 1;

	osDelay(100);

	buff[0] = FT_REG_POINT_RATE;

	if(ft5x06_i2c_read(buff,1,buff,1) != 0)
		return 2;

	//printf("clock rate = %dHz\r\n",buff[0] * 10);
	di_curr += sprintf(digitizer_info + di_curr,",%dHz",buff[0] * 10);

	return 0;
}

static void print_touch_points(void)
{
	char   buff[100];

	#ifdef DIGIT_ALLOW_PRINT
	printf("---------------------------------------\r\n");
	sprintf(buff,"points %d\r\n",t_d.count);
	printf(buff);

	sprintf(buff,"point1       x: %d       y: %d\r\n",t_d.point_x[0],t_d.point_y[0]);
	printf(buff);

	// point 2
	if(t_d.count > 1)
	{
		sprintf(buff,"point2       x: %d       y: %d\r\n",t_d.point_x[1],t_d.point_y[1]);
		printf(buff);
	}

	// point 3
	if(t_d.count > 2)
	{
		sprintf(buff,"point3       x: %d       y: %d\r\n",t_d.point_x[2],t_d.point_y[2]);
		printf(buff);
	}

	// point 4
	if(t_d.count > 3)
	{
		sprintf(buff,"point4       x: %d       y: %d\r\n",t_d.point_x[3],t_d.point_y[3]);
		printf(buff);
	}

	// point 5
	if(t_d.count > 4)
	{
		sprintf(buff,"point5       x: %d       y: %d\r\n",t_d.point_x[4],t_d.point_y[4]);
		printf(buff);
	}
	#endif
}

static void printf_hex_array(uchar *data,uchar size)
{
	char buf[100];
	uchar i;

	for (i = 0; i < size; i++)
	{
		sprintf(buf,"%02x ", *(data + i));
		printf(buf);
	}
	printf("\r\n");
}

#ifdef FT_USE_POLLING
static void check_touch(void)
{
	uchar 			err,i;
	uchar 			data[FT5206_TOUCH_DATA_SIZE + 1];
	uchar 			*pdata = data;
	GUI_PID_STATE 	TS_State;

	// Should we be here at all ?
	if(t_d.init_done == 0)
		return;

	// Check for old press event
	if(t_d.pending == 1)
	{
		//printf("-- Clear touch event --\r\n");

		// Only in Menu mode ?
		if(ui_s.cur_state == MODE_MENU)
		{
			// Emulate de-press event for the UI
			TS_State.Pressed 	= 0;
			TS_State.Layer 		= 0;
			TS_State.x 			= 0;
			TS_State.y 			= 0;
			GUI_TOUCH_StoreStateEx(&TS_State);

			// Reset flag
			t_d.pending 		= 0;
		}

		// Keyer screen reset
		if((tsu.demod_mode == DEMOD_CW) && (ui_s.cur_state == MODE_DESKTOP))
			t_d.pending = 0;

		// In desktop mode let the screen control reset the event
		// but run counter for 500 mS and clear it eventually,
		// so we don't stall the touch processing
		if(t_d.pending_count < 10)
			(t_d.pending_count)++;
		else
			t_d.pending = 0;
	}

	if(!HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_8))
	{
		data[0] = 0;

		if(ft5x06_i2c_read(data,1,data,FT5206_TOUCH_DATA_SIZE) != 0)
		{
			printf("check_touch->err read: %d\r\n",100);
			return;
		}
		//printf_hex_array(data,FT5206_TOUCH_DATA_SIZE);

		// Do not overwrite pending data
		if(t_d.pending == 1)
			return;

		// how many touch points ?
		t_d.count		= (data[2] & 0x0F);

		// Count '0' is a bug or feature ?
		if(t_d.count == 0)
			return;

		// Load data from array
		// data format:
		// --------------------------------------------
		// header				00 00
		// count				05
		// point1				81 33 01 44
		//						00 00
		// point2				80 29 10 bd
		//						00 00
		// point3				81 cb 21 69
		//						00 00
		// point4				82 a6 31 89
		//						00 00
		// point5				82 b4 40 d3
		// --------------------------------------------
		for(i = 0, pdata +=3; i < 5; i++)
		{
			// Clear point
			t_d.point_x[i]	= 0;
			t_d.point_y[i]	= 0;

			// Do not load unused points
			if(i > t_d.count)
				continue;

			// Load
			t_d.point_x[i]	= ((*(pdata + 0x00) & 0x07) << 8)|(*(pdata + 0x01));
			t_d.point_y[i]	= ((*(pdata + 0x02) & 0x07) << 8)|(*(pdata + 0x03));

			// Swap
			t_d.point_x[i]	= 800 - t_d.point_x[i];
			t_d.point_y[i]	= 480 - t_d.point_y[i];

			// Shift to next
			pdata += 4 + 2;
		}

		// Local debug print
		//print_touch_points();

		// Ready to process touch ?
		// Only in Menu mode ?
		if(ui_s.cur_state == MODE_MENU)
		{
			//printf("-- Submitting touch input to GUI --\r\n");

			// Submit to emWin
			TS_State.Pressed 	= 1;
			TS_State.Layer 		= 0;
			TS_State.x 			= t_d.point_x[0];
			TS_State.y 			= t_d.point_y[0];
			GUI_TOUCH_StoreStateEx(&TS_State);
		}

		// In desktop mode we need some kind of virtual debounce
		if((ui_s.cur_state == MODE_DESKTOP) && (tsu.demod_mode != DEMOD_CW))
			OsDelayMs(100);

		t_d.pending 		= 1; // pending event
		t_d.pending_count 	= 0; // start counter, in case no control clears it
	}
}
#endif

#ifdef FT_USE_IRQ
static void touch_driver_process(void)
{
	uchar 			err,i;
	uchar 			data[FT5206_TOUCH_DATA_SIZE + 1];
	uchar 			*pdata = data;
	//GUI_PID_STATE 	TS_State;
	uchar			diff,new_data = 0;

	// ---------------------------------------------------------------------------------
	// ---------------------------------------------------------------------------------
	// Read data
	//
	if(t_d.irq_lock)
	{
		#ifdef DIGIT_ALLOW_PRINT
		printf("ts irq\r\n");
		#endif

		// FIX FOR H7 as well !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		#ifdef CHIP_F7
		HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
		#endif
		#ifdef CHIP_H7
		NVIC_DisableIRQ(EXTI9_5_IRQn);
		#endif

		data[0] = 0;
		if(ft5x06_i2c_read(data,1,data,FT5206_TOUCH_DATA_SIZE) == 0)
		{
			new_data = 1;
			//printf_hex_array(data,FT5206_TOUCH_DATA_SIZE);
		}
		else
		{
			#ifdef DIGIT_ALLOW_PRINT
			printf("------------------------\r\n");
			printf("------------------------\r\n");
			printf("------------------------\r\n");
			printf("------------------------\r\n");
			printf("------------------------\r\n");
			printf("--   I2C lock down!   --\r\n");
			printf("------------------------\r\n");
			printf("------------------------\r\n");
			printf("------------------------\r\n");
			printf("------------------------\r\n");
			printf("------------------------\r\n");
			#endif

			//
			// Try to reset local hw - working
			//
			// ToDo: Re-implement I2C !!!
			//
			HAL_I2C_DeInit(&I2C1Handle);
			ft5x06_i2c_init();
		}

		// Switch to depressed
		t_d.irq_lock = 0;

		// FIX FOR H7 as well !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		#ifdef CHIP_F7
		HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
		#endif
		#ifdef CHIP_H7
		NVIC_EnableIRQ(EXTI9_5_IRQn);
		#endif
	}

	// ---------------------------------------------------------------------------------
	// ---------------------------------------------------------------------------------
	// Process data
	//
	if(new_data)
	{
		#ifdef DIGIT_ALLOW_PRINT
		printf("--> new data processing\r\n");
		#endif

		// how many touch points ?
		t_d.count		= (data[2] & 0x0F);

		// Count '0' is a bug or feature ?
		if(t_d.count == 0)
			return;

		// Load data from array
		// data format:
		// --------------------------------------------
		// header				00 00
		// count				05
		// point1				81 33 01 44
		//						00 00
		// point2				80 29 10 bd
		//						00 00
		// point3				81 cb 21 69
		//						00 00
		// point4				82 a6 31 89
		//						00 00
		// point5				82 b4 40 d3
		// --------------------------------------------
		for(i = 0, pdata +=3; i < 5; i++)
		{
			// Clear point
			t_d.point_x[i]	= 0;
			t_d.point_y[i]	= 0;

			// Do not load unused points
			if(i > t_d.count)
				continue;

			// Load
			t_d.point_x[i]	= ((*(pdata + 0x00) & 0x07) << 8)|(*(pdata + 0x01));
			t_d.point_y[i]	= ((*(pdata + 0x02) & 0x07) << 8)|(*(pdata + 0x03));

			// Swap
			t_d.point_x[i]	= 800 - t_d.point_x[i];
			t_d.point_y[i]	= 480 - t_d.point_y[i];

			// Shift to next
			pdata += 4 + 2;
		}

		// Local debug print
		print_touch_points();
	}

	// ---------------------------------------------------------------------------------
	// ---------------------------------------------------------------------------------
	// Check for old press event
	if(t_d.pending == 1)
	{
		#ifdef DIGIT_ALLOW_PRINT
		printf("-- Clear touch event --\r\n");
		#endif

		// Do we have finger still on the same spot ?
		if(new_data)
		{
			#ifdef DIGIT_ALLOW_PRINT
			printf("--> got new data, while pending\r\n");
			#endif

			// Same points count ?
			if(t_d.old_count == t_d.count)
			{
				diff = 0;

				// Compare old and new points
				for(i = 0; i < t_d.count; i++)
				{
					if((t_d.old_point_x[i] != t_d.point_x[i]) || (t_d.old_point_y[i] != t_d.point_y[i]))
					{
						//printf("-->old x = %d\r\n",t_d.old_point_x[i]);
						//printf("-->new x = %d\r\n",t_d.point_x[i]);
						//printf("-->old y = %d\r\n",t_d.old_point_y[i]);
						//printf("-->new y = %d\r\n",t_d.point_y[i]);

						// Check x
						if(t_d.old_point_x[i] > t_d.point_x[i])
						{
							if((t_d.old_point_x[i] - t_d.point_x[i]) > 5)
							{
								diff = 1;
								break;
							}
						}
						else
						{
							if((t_d.point_x[i] - t_d.old_point_x[i]) > 5)
							{
								diff = 1;
								break;
							}
						}

						// Check y
						if(t_d.old_point_y[i] > t_d.point_y[i])
						{
							if((t_d.old_point_y[i] - t_d.point_y[i]) > 5)
							{
								diff = 1;
								break;
							}
						}
						else
						{
							if((t_d.point_y[i] - t_d.old_point_y[i]) > 5)
							{
								diff = 1;
								break;
							}
						}

						continue;
					}
				}

				if(diff)
				{
					#ifdef DIGIT_ALLOW_PRINT
					printf("--> difference found\r\n");
					#endif
				}
				else
				{
					#ifdef DIGIT_ALLOW_PRINT
					printf("--> still same spot, will ignore\r\n");
					#endif
					return;
				}

			}
			else
			{
				#ifdef DIGIT_ALLOW_PRINT
				printf("--> count mismatch\r\n");
				#endif
			}
		}

		#ifdef DIGIT_ALLOW_PRINT
		printf("--> will clear anyway\r\n");
		#endif

		#if 0
		// Only in Menu mode ?
		if(ui_s.cur_state == MODE_MENU)
		{
			// Emulate de-press event for the UI
			TS_State.Pressed 	= 0;
			TS_State.Layer 		= 0;
			TS_State.x 			= 0;
			TS_State.y 			= 0;

			GUI_X_Lock();
			GUI_TOUCH_StoreStateEx(&TS_State);
			GUI_X_Unlock();

			// Reset flag
			t_d.pending 		= 0;
			return;
		}
		#endif

		// Keyer screen reset
		if((tsu.band[tsu.curr_band].demod_mode == DEMOD_CW) && (ui_s.cur_state == MODE_DESKTOP))
		{
			#ifdef DIGIT_ALLOW_PRINT
			printf("--> keyer mode, clear pending and exit\r\n");
			#endif
			t_d.pending = 0;
			return;
		}

		// In desktop mode let the screen control reset the event
		// but run counter for 500 mS and clear it eventually,
		// so we don't stall the touch processing
		if(t_d.pending_count < 10)
		{
			#ifdef DIGIT_ALLOW_PRINT
			printf("--> pending timer working...\r\n");
			#endif

			(t_d.pending_count)++;
		}
		else
		{
			#ifdef DIGIT_ALLOW_PRINT
			printf("--> pending timer expired.\r\n");
			#endif
			t_d.pending = 0;
		}
	}
	else
	{
		if(new_data)
		{
			#ifdef DIGIT_ALLOW_PRINT
			printf("-- Create touch event --\r\n");
			#endif

			#if 0
			// Only in Menu mode ?
			if(ui_s.cur_state == MODE_MENU)
			{
				//printf("-- Submitting touch input to GUI --\r\n");

				// Submit to emWin
				TS_State.Pressed 	= 1;
				TS_State.Layer 		= 0;
				TS_State.x 			= t_d.point_x[0];
				TS_State.y 			= t_d.point_y[0];

				GUI_X_Lock();
				GUI_TOUCH_StoreStateEx(&TS_State);
				GUI_X_Unlock();
			}
			#endif

			// In desktop mode we need some kind of virtual debounce
			//if((ui_s.cur_state == MODE_DESKTOP) && (tsu.demod_mode != DEMOD_CW))
			//	OsDelayMs(100);

			// Copy data
			t_d.old_count = t_d.count;
			for(i = 0; i < 5; i++)
			{
				t_d.old_point_x[i]	= t_d.point_x[i];
				t_d.old_point_y[i]	= t_d.point_y[i];
			}

			t_d.pending 		= 1; // pending event
			t_d.pending_count 	= 0; // start counter, in case no control clears it
		}
	}
}
#endif

void touch_driver(void const * argument)
{
	// Delay start, so UI can paint properly
	OsDelayMs(TOUCHC_DRV_START_DELAY);

	printf("digitizer driver start\r\n");

	// Reset public structure
	t_d.count 		= 0;			// no points
	t_d.pending 	= 0;			// nothing to process
	t_d.init_done	= 0;
	t_d.irq_lock	= 0;

	memset(digitizer_info,0, sizeof(digitizer_info));
	di_curr = 0;

	// --------------------------------------------
	// I2C init
	//if(ft5x06_i2c_init() != 0)
	//{
	//	printf("unable to init I2C1\r\n");
	//	goto stall_task;
	//}
	// --------------------------------------------

	// Reset controller
	ft5x06_hw_reset();

	// Set operating mode - totally kills the firmware
	post_reset_init();

	// Get ID  - totally kills the firmware
	ft5x06_get_id();

	// Ready
	t_d.init_done = 1;

touch_driver_loop:

	#ifdef FT_USE_POLLING
	check_touch();
	#endif
	#ifdef FT_USE_IRQ
	touch_driver_process();
	#endif

	// Every 50 mS or 100 mS ? - smaller sleep seems to work better with wmWin touch screen handler
	OsDelayMs(20);
	goto touch_driver_loop;

//stall_task:
//	goto stall_task;
}


