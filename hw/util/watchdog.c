#include "mchf_pro_board.h"

#ifdef CHIP_H7
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_rcc.h"
#include "stm32h7xx_hal_wwdg.h"
#endif

#include "watchdog.h"

/* WWDG handler declaration */
WWDG_HandleTypeDef   WwdgHandle;

uchar	watchdog_enabled = 0;

// Public radio state
extern struct	TRANSCEIVER_STATE_UI	tsu;

/**
  * @brief  Timeout calculation function.
  *         This function calculates any timeout related to
  *         WWDG with given prescaler and system clock.
  * @param  timevalue: period in term of WWDG counter cycle.
  * @retval None
  */
static uint32_t TimeoutCalculation(uint32_t timevalue)
{
  uint32_t timeoutvalue = 0;
  uint32_t pclk1 = 0;
  uint32_t wdgtb = 0;

  /* Get PCLK1 value */
  pclk1 = HAL_RCC_GetPCLK1Freq();

  /* get prescaler */
  switch(WwdgHandle.Init.Prescaler)
  {
    case WWDG_PRESCALER_1:   wdgtb = 1;   break;
    case WWDG_PRESCALER_2:   wdgtb = 2;   break;
    case WWDG_PRESCALER_4:   wdgtb = 4;   break;
    case WWDG_PRESCALER_8:   wdgtb = 8;   break;
    case WWDG_PRESCALER_16:  wdgtb = 16;  break;
    case WWDG_PRESCALER_32:  wdgtb = 32;  break;
    case WWDG_PRESCALER_64:  wdgtb = 64;  break;
    case WWDG_PRESCALER_128: wdgtb = 128; break;

    default:
    	//Error_Handler();
    	break;
  }

  /* calculate timeout */
  timeoutvalue = ((4096 * wdgtb * timevalue) / (pclk1 / 1000));

  return timeoutvalue;
}

void watchdog_check_reset(void)
{
	if(__HAL_RCC_GET_FLAG(RCC_FLAG_D1RST) != RESET)
		tsu.reset_reason = RCC_FLAG_D1RST;

	if(__HAL_RCC_GET_FLAG(RCC_FLAG_D2RST) != RESET)
		tsu.reset_reason = RCC_FLAG_D2RST;

	if(__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST) != RESET)
		tsu.reset_reason = RCC_FLAG_BORRST;

	if(__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET)
		tsu.reset_reason = RCC_FLAG_PINRST;

	if(__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET)
		tsu.reset_reason = RCC_FLAG_PORRST;

	if(__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST) != RESET)
		tsu.reset_reason = RCC_FLAG_SFTRST;

	if(__HAL_RCC_GET_FLAG(RCC_FLAG_IWDG1RST) != RESET)
		tsu.reset_reason = RCC_FLAG_IWDG1RST;

	if(__HAL_RCC_GET_FLAG(RCC_FLAG_WWDG1RST) != RESET)
		tsu.reset_reason = RCC_FLAG_WWDG1RST;

	if(__HAL_RCC_GET_FLAG(RCC_FLAG_LPWR1RST) != RESET)
		tsu.reset_reason = RCC_FLAG_LPWR1RST;

	if(__HAL_RCC_GET_FLAG(RCC_FLAG_LPWR2RST) != RESET)
		tsu.reset_reason = RCC_FLAG_LPWR2RST;

	// Clear reset flags in any case
	__HAL_RCC_CLEAR_RESET_FLAGS();

	// Enable system wide reset
	HAL_RCCEx_WWDGxSysResetConfig(RCC_WWDG1);
}

void watchdog_init(void)
{
	uint32_t delay;

	printf("watchdog init\r\n");

	__HAL_RCC_WWDG1_CLK_ENABLE();

	/*##-2- Init & Start WWDG peripheral ######################################*/
	  /*  Configuration:
	      1] Set WWDG counter to maximum 0x7F (127 cycles)  and window to 0x50 (80 cycles)
	      2] Set Prescaler to 8 (2^3)

	      Timing calculation:
	      a) WWDG clock counter period (in ms) = (4096 * 8) / (PCLK1 / 1000)
	                                           = 0,4096 ms
	      b) WWDG timeout (in ms) = (127 + 1) * 0,4096
	                              ~= 52,42 ms
	      => After refresh, WWDG will expires after 52,42 ms and generate reset if
	      counter is not reloaded.
	      c) Time to enter inside window
	      Window timeout (in ms) = (127 - 80 + 1) * 0,4096
	                             = 19,66 ms */
	  WwdgHandle.Instance 		= WWDG1;
	  WwdgHandle.Init.Prescaler = WWDG_PRESCALER_128;
	  WwdgHandle.Init.Window    = 0x50;
	  WwdgHandle.Init.Counter   = 0x7F;
	  WwdgHandle.Init.EWIMode   = WWDG_EWI_DISABLE;

	  if (HAL_WWDG_Init(&WwdgHandle) != HAL_OK)
	  {
		//  printf("watchdog init error\r\n");
	  }

	  /* Calculate delay to enter window. Add 1ms to secure round number to upper number  */
	  delay = TimeoutCalculation((WwdgHandle.Init.Counter-WwdgHandle.Init.Window) + 1) + 1;
	  //printf("watchdog window: %dmS\r\n",delay);

	  watchdog_enabled = 1;
}

void watchdog_refresh(void)
{
	if(!watchdog_enabled)
		return;

	//printf("watchdog refresh\r\n");

	HAL_WWDG_Refresh(&WwdgHandle);
}
