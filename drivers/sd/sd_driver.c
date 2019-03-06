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

#include "sd_driver.h"
#include "C:\Projects\mcHFx\firmware\mchf_ui\drivers\sd\hw\k_storage.h"

//*----------------------------------------------------------------------------
//* Function Name       : sd_driver_hw_init
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_TEMPLATE
//*----------------------------------------------------------------------------
static void sd_driver_hw_init(void)
{
	#ifdef CONTEXT_SD
	k_StorageInit();
	#endif
}

//*----------------------------------------------------------------------------
//* Function Name       : sd_driver_worker
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_TEMPLATE
//*----------------------------------------------------------------------------
static void sd_driver_worker(void)
{
	#ifdef CONTEXT_SD
	StorageThread();
	#endif
}

//*----------------------------------------------------------------------------
//* Function Name       : sd_driver_task
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_TEMPLATE
//*----------------------------------------------------------------------------
void sd_driver_task(void const * argument)
{
	// Delay start, so UI can paint properly
	OsDelayMs(1000);

	printf("sd driver start\r\n");

	// Local HW init
	sd_driver_hw_init();

sd_driver_loop:

	sd_driver_worker();

	// Driver sleep period
	OsDelayMs(150);
		
	goto sd_driver_loop;
}
