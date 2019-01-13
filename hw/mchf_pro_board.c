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
#include "stm32f7xx_hal_rcc_ex.h"
#include "stm32f7xx_hal_rcc.h"
#include "stm32f7xx_hal_gpio.h"
#include "stm32f7xx_hal_cortex.h"
#include "stm32f7xx_hal_pwr.h"
#include "stm32f7xx_hal_flash_ex.h"
#endif

#ifdef CHIP_H7
#include "stm32h7xx_hal_rcc.h"
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_cortex.h"
#include "stm32h7xx_hal_pwr.h"
#include "stm32f7xx_hal_flash_ex.h"
#endif

// Sensitive drivers HW init here ;(
#include "touch_driver.h"
#include "api_driver.h"
#include "rotary_driver.h"
#include "keypad_driver.h"
#include "net_driver.h"

extern void ItmInitAsm(void);

// Core unique regs loaded to RAM
struct	CM7_CORE_DETAILS		ccd;
// Public radio state
struct	TRANSCEIVER_STATE_UI	tsu;

//*----------------------------------------------------------------------------
//* Function Name       : mchf_pro_board_debug_led_init
//* Object              : use HW LED to check H7 problems
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: anyone!
//*----------------------------------------------------------------------------
void mchf_pro_board_debug_led_init(void)
{
	GPIO_InitTypeDef  	GPIO_InitStruct;

	__HAL_RCC_GPIOD_CLK_ENABLE();

	GPIO_InitStruct.Pin 		= GPIO_PIN_7;
	GPIO_InitStruct.Mode 		= GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull 		= GPIO_NOPULL;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	// LED off
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
}

//*----------------------------------------------------------------------------
//* Function Name       : mchf_pro_board_blink_if_alive
//* Object              : debug hw problems by blinking the LCD backlight
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: anyone!
//*----------------------------------------------------------------------------
void mchf_pro_board_blink_if_alive(uchar flags)
{
	GPIO_InitTypeDef  	GPIO_InitStruct;
	ulong 				i;

	// Need init ?
	if((flags & 0x01) == 0x01)
	{
		__HAL_RCC_GPIOF_CLK_ENABLE();

		// LCD Backlight PF9
		GPIO_InitStruct.Pin 		= GPIO_PIN_9;
		GPIO_InitStruct.Mode 		= GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull 		= GPIO_NOPULL;
		GPIO_InitStruct.Speed 		= GPIO_SPEED_FREQ_VERY_HIGH;
		HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
		HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_RESET);
	}

	// Need stalling and blink ?
	if((flags & 0x02) == 0x02)
	{
		while(1)
		{
			HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_9);
			//printf("---\r\n");

			for(i = 0; i < 0x5FFFFFF; i++)
				__asm(".hword 0x46C0");
		}
	}

	// Just light on
	if((flags & 0x04) == 0x04)
	{
		HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_SET);
	}
}

//*----------------------------------------------------------------------------
//* Function Name       : mchf_pro_board_read_cpu_details
//* Object              : Read specific CPU regs before MMU remap
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void mchf_pro_board_read_cpu_details(void)
{

	ulong	*p_uniq = (ulong  *)CHIP_UNIQUE_ID;
	ushort 	*p_fls 	= (ushort *)CHIP_FLS_SIZE;

	ccd.wafer_coord 	= *(p_uniq + 0x00);
	ccd.wafer_number	= *(p_uniq + 0x04);
	ccd.lot_number		= *(p_uniq + 0x08);
	ccd.fls_size 		= *(p_fls  + 0x00);
}

//*----------------------------------------------------------------------------
//* Function Name       : mchf_pro_board_start_gpio_clocks
//* Object              : GPIO clocks on
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void mchf_pro_board_start_gpio_clocks(void)
{
	// Enable GPIOs clocks
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOI_CLK_ENABLE();

#ifdef BOARD_DISCO
	//Extra ports on the BGA chip, so enable clocks
	__HAL_RCC_GPIOJ_CLK_ENABLE();
	__HAL_RCC_GPIOK_CLK_ENABLE();
#endif
}

//*----------------------------------------------------------------------------
//* Function Name       : mchf_pro_board_mpu_config
//* Object              : MPU setup
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void mchf_pro_board_mpu_config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;

  // Disable the MPU
  HAL_MPU_Disable();

#ifdef CHIP_F7
  // Configure the MPU attributes as WT for SRAM
  MPU_InitStruct.Enable 			= MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress 		= 0x20010000;
  MPU_InitStruct.Size 				= MPU_REGION_SIZE_256KB;
  MPU_InitStruct.AccessPermission 	= MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable 		= MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable 		= MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable 		= MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number 			= MPU_REGION_NUMBER0;
  MPU_InitStruct.TypeExtField 		= MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable 	= 0x00;
  MPU_InitStruct.DisableExec 		= MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

#if 1
  // Do we really the SDRAM mapped by the MPU on F7 part ?
   // Configure the MPU attributes as WT for SDRAM
   MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
   MPU_InitStruct.BaseAddress      = 0xC0000000;
   MPU_InitStruct.Size             = MPU_REGION_SIZE_4MB;
   MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
   MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
   MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
   MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
   MPU_InitStruct.Number           = MPU_REGION_NUMBER1;				// table entry
   MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
   MPU_InitStruct.SubRegionDisable = 0x00;
   MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE;

   HAL_MPU_ConfigRegion(&MPU_InitStruct);
#endif

#endif

#ifdef CHIP_H7
  // Layer buffers - 0xC0000000, SDRAM, Bank1
  MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress      = 0xC0000000;
  MPU_InitStruct.Size             = MPU_REGION_SIZE_4MB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number           = MPU_REGION_NUMBER0;					// table entry
  MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);

   // Stack and data vars - 0x24000000 - 0x2407FFFF 	512k	AXI SRAM
   MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
   MPU_InitStruct.BaseAddress      = 0x24000000;
   MPU_InitStruct.Size             = MPU_REGION_SIZE_512KB;
   MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
   MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
   MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
   MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
   MPU_InitStruct.Number           = MPU_REGION_NUMBER1;				// table entry
   MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
   MPU_InitStruct.SubRegionDisable = 0x00;
   MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
   HAL_MPU_ConfigRegion(&MPU_InitStruct);

   // OS heap - 0x38000000 - 0x3800FFFF 	64k		SRAM4
   /* MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress      = 0x38000000;
    MPU_InitStruct.Size             = MPU_REGION_SIZE_64KB;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
    MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.Number           = MPU_REGION_NUMBER2;				// table entry
    MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
    HAL_MPU_ConfigRegion(&MPU_InitStruct); */
#endif

  // Enable the MPU
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

//*----------------------------------------------------------------------------
//* Function Name       : mchf_pro_board_cpu_cache_enable
//* Object              : CPU L1-Cache enable
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void mchf_pro_board_cpu_cache_enable(void)
{
#ifdef CHIP_F7
  // Enable branch prediction
  SCB->CCR |= (1 <<18);
  __DSB();
#endif

  // Enable I-Cache
  SCB_EnableICache();

  // Enable D-Cache
  SCB_EnableDCache();
}

//*----------------------------------------------------------------------------
//* Function Name       : mchf_pro_board_system_clock_config
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
uchar mchf_pro_board_system_clock_config(uchar clk_src)
{
	RCC_ClkInitTypeDef			RCC_ClkInitStruct;
	RCC_OscInitTypeDef 			RCC_OscInitStruct;
	HAL_StatusTypeDef 			ret = HAL_OK;

#ifdef CHIP_F7
	// Enable Power Control clock
	__HAL_RCC_PWR_CLK_ENABLE();

	// The voltage scaling allows optimising the power consumption when the device is
	// clocked below the maximum system frequency, to update the voltage scaling value
	// regarding system frequency refer to product datasheet.
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	switch(clk_src)
	{
		// Internal
		case 0:
			RCC_OscInitStruct.HSIState 			= RCC_HSI_ON;
			RCC_OscInitStruct.OscillatorType	= RCC_OSCILLATORTYPE_HSI;
			RCC_OscInitStruct.PLL.PLLSource 	= RCC_PLLSOURCE_HSI;
			break;

		// XTAL
		case 1:
			RCC_OscInitStruct.HSEState 			= RCC_HSE_ON;
			RCC_OscInitStruct.OscillatorType	= RCC_OSCILLATORTYPE_HSE;
			RCC_OscInitStruct.PLL.PLLSource 	= RCC_PLLSOURCE_HSE;
			break;

		// TCXO
		case 2:
			RCC_OscInitStruct.HSEState 			= RCC_HSE_BYPASS;
			RCC_OscInitStruct.OscillatorType	= RCC_OSCILLATORTYPE_HSE;
			RCC_OscInitStruct.PLL.PLLSource 	= RCC_PLLSOURCE_HSE;
			break;

		default:
			return 20;
	}

	RCC_OscInitStruct.PLL.PLLState 		= RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLM 			= 8;
	RCC_OscInitStruct.PLL.PLLN 			= 216;
	RCC_OscInitStruct.PLL.PLLP 			= RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ 			= 2;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	ret = HAL_PWREx_EnableOverDrive();
	if(ret != HAL_OK)
		return 1;

	// Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
	//	 clocks dividers
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6);
	if(ret != HAL_OK)
		return 2;
#endif

#ifdef CHIP_H7
	// Supply configuration update enable
	MODIFY_REG(PWR->CR3, PWR_CR3_SCUEN, 0);

	//Configure the main internal regulator output voltage
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	while ((PWR->D3CR & (PWR_D3CR_VOSRDY)) != PWR_D3CR_VOSRDY)
	{
		// ToDo: any timeout or handling ?
	}

	switch(clk_src)
	{
		// Internal
		case 0:
			__HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSI);
			RCC_OscInitStruct.HSIState 			= RCC_HSI_ON;
			RCC_OscInitStruct.OscillatorType	= RCC_OSCILLATORTYPE_HSI;
			RCC_OscInitStruct.PLL.PLLSource 	= RCC_PLLSOURCE_HSI;
			break;

		// XTAL
		case 1:
			__HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
			RCC_OscInitStruct.HSEState 			= RCC_HSE_ON;
			RCC_OscInitStruct.OscillatorType	= RCC_OSCILLATORTYPE_HSE;
			RCC_OscInitStruct.PLL.PLLSource 	= RCC_PLLSOURCE_HSE;
			break;

		// TCXO
		case 2:
			__HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
			RCC_OscInitStruct.HSEState 			= RCC_HSE_BYPASS;
			RCC_OscInitStruct.OscillatorType	= RCC_OSCILLATORTYPE_HSE;
			RCC_OscInitStruct.PLL.PLLSource 	= RCC_PLLSOURCE_HSE;
			break;

		default:
			return 20;
	}

	RCC_OscInitStruct.PLL.PLLState 		= RCC_PLL_ON;
	//
	RCC_OscInitStruct.PLL.PLLM 			= 1;
	RCC_OscInitStruct.PLL.PLLN 			= 50;
	RCC_OscInitStruct.PLL.PLLP 			= 2;
	RCC_OscInitStruct.PLL.PLLR 			= 2;
	RCC_OscInitStruct.PLL.PLLQ 			= 2;
	//
	RCC_OscInitStruct.PLL.PLLVCOSEL 	= RCC_PLL1VCOWIDE;
	RCC_OscInitStruct.PLL.PLLRGE 		= RCC_PLL1VCIRANGE_3;	//RCC_PLL1VCIRANGE_2;
	ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
	if(ret != HAL_OK)
		return 1;

	// Select PLL as system clock source and configure  bus clocks dividers
	RCC_ClkInitStruct.ClockType = 	(RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_D1PCLK1 | RCC_CLOCKTYPE_PCLK1 | \
									 RCC_CLOCKTYPE_PCLK2  | RCC_CLOCKTYPE_D3PCLK1);

	#ifdef H7_200MHZ
	RCC_ClkInitStruct.SYSCLKDivider 	= RCC_SYSCLK_DIV2;
	#endif
	#ifdef H7_400MHZ
	RCC_ClkInitStruct.SYSCLKDivider 	= RCC_SYSCLK_DIV1;
	#endif

	RCC_ClkInitStruct.SYSCLKSource 		= RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider 	= RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB3CLKDivider 	= RCC_APB3_DIV2;
	RCC_ClkInitStruct.APB1CLKDivider 	= RCC_APB1_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider 	= RCC_APB2_DIV2;
	RCC_ClkInitStruct.APB4CLKDivider 	= RCC_APB4_DIV2;

	ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
	if(ret != HAL_OK)
		return 2;
#endif

	return 0;
}

//*----------------------------------------------------------------------------
//* Function Name       : mchf_pro_board_rtc_clock_config
//* Object              : start rtc osc and pll separate from the main clock
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
uchar mchf_pro_board_rtc_clock_config(uchar clk_src)
{
#ifdef CHIP_F7
	RCC_OscInitTypeDef 			RCC_OscInitStruct;
	RCC_PeriphCLKInitTypeDef  	PeriphClkInitStruct;

	__HAL_RCC_PWR_CLK_ENABLE();

	HAL_PWR_EnableBkUpAccess();

	if(clk_src == EXT_32KHZ_XTAL)
	{
		RCC_OscInitStruct.OscillatorType		= RCC_OSCILLATORTYPE_LSE;
		RCC_OscInitStruct.LSEState 				= RCC_LSE_ON;
		PeriphClkInitStruct.RTCClockSelection 	= RCC_RTCCLKSOURCE_LSE;
	}
	else
	{
		RCC_OscInitStruct.OscillatorType		= RCC_OSCILLATORTYPE_LSI;
		RCC_OscInitStruct.LSIState 				= RCC_LSI_ON;
		PeriphClkInitStruct.RTCClockSelection 	= RCC_RTCCLKSOURCE_LSI;
	}

	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;

	if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
		return 1;

	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
		return 2;

	__HAL_RCC_RTC_ENABLE();
#endif

#ifdef CHIP_H7
	RCC_OscInitTypeDef        RCC_OscInitStruct;
	RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

	/*##-1- Enables access to the backup domain ######*/
	/* To enable access on RTC registers */
	HAL_PWR_EnableBkUpAccess();
	/*##-2- Configure LSE/LSI as RTC clock source ###############################*/
	if(clk_src == EXT_32KHZ_XTAL)
	{
		RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
		RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
		RCC_OscInitStruct.LSEState = RCC_LSE_ON;
		RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
		if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
			return 1;

		PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
		PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
		if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
			return 2;
	}
	else
	{
		RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
		RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
		RCC_OscInitStruct.LSIState = RCC_LSI_ON;
		RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
		if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
			return 1;

		PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
		PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
		if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
			return 2;
	}
	//##-3- Enable RTC peripheral Clocks #######################################
	// Enable RTC Clock
	__HAL_RCC_RTC_ENABLE();

    // Enable write access to Backup domain
    PWR->CR1 |= PWR_CR1_DBP;
    while((PWR->CR1 & PWR_CR1_DBP) == RESET)
    {
    }

    // Enable BKPRAM clock
    __HAL_RCC_BKPRAM_CLK_ENABLE();

    // Enable the Backup SRAM low power Regulator
    HAL_PWREx_EnableBkUpReg();

    // Enable virtual eeprom
    tsu.eeprom_init_done = 1;

#endif

	return 0;
}

//*----------------------------------------------------------------------------
//* Function Name       : mchf_pro_board_rtc_clock_disable
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void mchf_pro_board_rtc_clock_disable(void)
{
#ifdef CHIP_F7
	__HAL_RCC_RTC_DISABLE();
	HAL_PWR_DisableBkUpAccess();
	__HAL_RCC_PWR_CLK_DISABLE();
#endif
}

//*----------------------------------------------------------------------------
//* Function Name       : mchf_pro_board_swo_init
//* Object              : Debug print via ITM(SWO pin)
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void mchf_pro_board_swo_init(void)
{
#ifdef CHIP_F7
	volatile uint32_t *ITM_LAR = (volatile uint32_t *)0xE0000FB0; // ITM->LAR

	// Enable Access
	*ITM_LAR = 0xC5ACCE55;
#endif

#ifdef CHIP_H7
	ItmInitAsm();
#endif

	printf("--- Debug Print Session on ---\r\n");
}

//*----------------------------------------------------------------------------
//* Function Name       : mchf_pro_board_mco2_on
//* Object              : Output 24.576 Mhz on PC9 for the Quadrature upconverter
//* Input Parameters    : use PLLSAI clock instead of PLLI2S - works somehow ?
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void mchf_pro_board_mco2_on(void)
{
#ifdef CHIP_F7
	RCC_PeriphCLKInitTypeDef RCC_ExCLKInitStruct;

	HAL_RCCEx_GetPeriphCLKConfig(&RCC_ExCLKInitStruct);

    // Configure PLLSAI prescalers */
    // PLLI2S_VCO: VCO_429M
    // SAI_CLK(first level) = PLLI2S_VCO/PLLSAIQ = 429/2 = 214.5 Mhz
    // SAI_CLK_x = SAI_CLK(first level)/PLLI2SDivQ = 214.5/19 = 11.289 Mhz
	//-RCC_ExCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI2;
	//-RCC_ExCLKInitStruct.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLI2S;
	//-RCC_ExCLKInitStruct.PLLI2S.PLLI2SP = 8;
	//-RCC_ExCLKInitStruct.PLLI2S.PLLI2SN = 429;
	//-RCC_ExCLKInitStruct.PLLI2S.PLLI2SQ = 2;
	//-RCC_ExCLKInitStruct.PLLI2SDivQ = 19;
	//-HAL_RCCEx_PeriphCLKConfig(&RCC_ExCLKInitStruct);

	// SAI clock config
	// PLLI2S_VCO: VCO_344M
	// SAI_CLK(first level) = PLLI2S_VCO/PLLSAIQ = 344/7 = 49.142 Mhz
	// SAI_CLK_x = SAI_CLK(first level)/PLLI2SDivQ = 49.142/1 = 49.142 Mhz
	RCC_ExCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SAI2;
	RCC_ExCLKInitStruct.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLI2S;
	RCC_ExCLKInitStruct.PLLI2S.PLLI2SP = 8;
	RCC_ExCLKInitStruct.PLLI2S.PLLI2SN = 344;
	RCC_ExCLKInitStruct.PLLI2S.PLLI2SQ = 7;
	RCC_ExCLKInitStruct.PLLI2SDivQ = 1;
	HAL_RCCEx_PeriphCLKConfig(&RCC_ExCLKInitStruct);

	// Output SYSCLK /2 on MCO2 pin(PC.09)
	// ToDo: 28.5714286 Mhz, but need 24.576 Mhz
	HAL_RCC_MCOConfig(RCC_MCO2, RCC_MCO2SOURCE_PLLI2SCLK, RCC_MCODIV_4);
#endif
}

//*----------------------------------------------------------------------------
//* Function Name       : mchf_pro_board_sensitive_hw_init
//* Object              :
//* Notes    			: On H7 hardware will crash if init is done in a task
//* Notes   			: during runtime, so init here
//* Notes    			:
//* Context    			: CONTEXT_RESET_VECTOR
//*----------------------------------------------------------------------------
void mchf_pro_board_sensitive_hw_init(void)
{
	// API driver SPI and GPIOs
	api_driver_hw_init();
	//
	// Digitizer driver I2C
	ft5x06_i2c_init();
	//
	// Rotary driver timers
	rotary_driver_hw_init();
	//
	// Keypad driver GPIO
	#ifdef CONTEXT_DRIVER_KEYPAD
	keypad_driver_init();
	#endif
	//
	// Net driver
	#ifdef CONTEXT_NET
	net_driver_hw_init();
	#endif
}

//*----------------------------------------------------------------------------
//* Function Name       : WRITE_EEPROM
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: anyone!
//*----------------------------------------------------------------------------
void WRITE_EEPROM(ushort addr,uchar value)
{
	uchar *bkp = (uchar *)EEP_BASE;

	if(!tsu.eeprom_init_done)
		return;

	if(addr > 0xFFF)
		return;

	// Write to BackUp SRAM
	*(bkp + addr) = value;
}

//*----------------------------------------------------------------------------
//* Function Name       : READ_EEPROM
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: anyone!
//*----------------------------------------------------------------------------
uchar READ_EEPROM(ushort addr)
{
	uchar ret;
	uchar *bkp = (uchar *)EEP_BASE;

	if(!tsu.eeprom_init_done)
		return 0xFF;

	if(addr > 0xFFF)
		return 0xFF;

	// Read BackUp SRAM
	return *(bkp + addr);
}

