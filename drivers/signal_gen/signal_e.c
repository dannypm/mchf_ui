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

#include "signal_e.h"

#ifdef CONTEXT_DRIVER_SIGNAL

#include "stm32f7xx_hal_rcc.h"
#include "stm32f7xx_hal_rng.h"

RNG_HandleTypeDef RngHandle;
__IO uint32_t aRandom32bit[8];

extern 	uchar 		fft_value[1024];

long  skipb = 0;
ulong signal_e = 15;
ulong signal_a = 0;

static void emulate_noise_floor(void)
{
	ulong i,a;
	unsigned char x = 0;

	// Emulate random band noise floor
	for (i = 0; i < sizeof(fft_value); i++)
	{
		HAL_RNG_GenerateRandomNumber(&RngHandle,&a);

		// Limit amplitude
		//x = (a & 0xF);				// looks nice but CPU intensive
		x = (a & 0x3);			// ugly, but fast

		// Expand signal bandwidth
		fft_value[i] = SIGNAL_NOISE_FLOOR + x;

		// Fill signal value as well
		//if(i == 200)
		//	signal_e = fft_value[i];
	}
}

// Emulate strong signal at specific freq
static void insert_strong_signal(void)
{
	uchar i,x = 0;
	uchar range;

	// To local
	x = signal_e;

	// Under noise ?
	if(x <= SIGNAL_NOISE_FLOOR)
		return;

	// Insert centre
	fft_value[200] = x;

	// Calculate range
	range = x - SIGNAL_NOISE_FLOOR;

	// Insert upper sideband
	for(i = 1; i <= range; i++)
	{
		if((SIGNAL_NOISE_FLOOR + range - i*5) <= SIGNAL_NOISE_FLOOR)
			break;

		fft_value[200 + i] = SIGNAL_NOISE_FLOOR + range - i*5;
	}

	// Insert lower sideband
	for(i = 1; i <= range; i++)
	{
		if((SIGNAL_NOISE_FLOOR + range - i*5) <= SIGNAL_NOISE_FLOOR)
			break;

		fft_value[200 - i] = SIGNAL_NOISE_FLOOR + range - i*5;
	}
}

//
// Still doesn't work good, FIX IT!!!
//
static void create_strong_signal(void)
{
	ulong a;

	// How often ?
	skipb++;
	if(skipb < SIGNAL_AVERAGE_RES)
	{
		// Generate new value for the signal
		HAL_RNG_GenerateRandomNumber(&RngHandle,&a);

		// Load accumulator
		signal_a += a;

		return;
	}

	skipb = 0;

	//printf("acc = %08x\r\n",signal_a);

	// Use average value (but shift above noise floor)
	a = (signal_a/SIGNAL_AVERAGE_RES) & 0x6F;
	//printf("a = %d\r\n",a);

	if(a <= SIGNAL_NOISE_FLOOR)
		signal_e  = SIGNAL_NOISE_FLOOR + a;
	else
		signal_e = a;

	//printf("sig = %d\r\n",signal_e);

	// Reset accumulator
	signal_a = 0;
}

void signal_task(void const * argument)
{
	ulong a = 0;

	// Delay start, so UI can paint properly
	OsDelayMs(SIGNAL_DRV_START_DELAY);

	printf("signal driver start\r\n");

	// RNG Peripheral clock enable
	__HAL_RCC_RNG_CLK_ENABLE();

	// RND init
	RngHandle.Instance = RNG;
	HAL_RNG_Init(&RngHandle);
	HAL_RNG_GenerateRandomNumber(&RngHandle,&a);

signal_driver_loop:

	emulate_noise_floor();
	insert_strong_signal();
	create_strong_signal();

	// Every 200 mS
	OsDelayMs(200);

	goto signal_driver_loop;
}

/*void signal_init(void)
{
	// Create task
	osThreadDef(signal_task_m, signal_task, osPriorityNormal, 0, 1024);
	osThreadCreate (osThread(signal_task_m), NULL);
}*/

#endif

