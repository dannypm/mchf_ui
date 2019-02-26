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

// Can disable the driver for testing
#ifdef CONTEXT_DRIVER_DSP

#include "dsp_driver.h"

#include "digi\ft8_lib-master\gen_ft8.h"
#include "digi\ft8_lib-master\decode_ft8.h"

// Driver communication
extern 			osMessageQId 			hDspMessage;

//*----------------------------------------------------------------------------
//* Function Name       : dsp_driver_hw_init
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_DSP
//*----------------------------------------------------------------------------
static void dsp_driver_hw_init(void)
{
	// nothing ?
}

//*----------------------------------------------------------------------------
//* Function Name       : dsp_driver_worker
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_DSP
//*----------------------------------------------------------------------------
static void dsp_driver_worker(void)
{
	osEvent 			event;
	struct DSPMessage 	*dsp_msg;

	// Wait for a short time for pending messages
	event = osMessageGet(hDspMessage, 150);		// second arg is driver idle time in mS
	if(event.status != osEventMessage)
		return;

	// Check status type
	if(event.status != osEventMessage)
		return;

	dsp_msg = (struct DSPMessage *)event.value.p;

	// Process menu calls
	switch(dsp_msg->ucMessageID)
	{
		// Encode FT8
		case 1:
		{
			//char 	bufa[40];
			//strcpy(bufa,"CQ M0NKA IO92");

			// ToDo: strxxx checks on passed buffer ??

			encode_ft8_message(dsp_msg->cData,1);

			// Signal UI driver
			dsp_msg->ucProcessDone = 1;

			break;
		}

		// Decode FT8
		case 2:
		{
			char 	buf[40];
			uchar 	i;

			//printf("menu 1 called\r\n");

			decode_ft8_message(buf);
			//printf("decoded: %s\r\n",buf);

			// unsafe strcpy...
			for(i = 0; i < 50; i++)
			{
				if(buf[i] == 0) break;
				dsp_msg->cData[i] = buf[i];
			}
			if(i) dsp_msg->ucDataReady = 1;

			break;
		}

		default:
			break;
	}
}

//*----------------------------------------------------------------------------
//* Function Name       : dsp_driver_task
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_DSP
//*----------------------------------------------------------------------------
void dsp_driver_task(void const * argument)
{
	// Delay start, so UI can paint properly
	OsDelayMs(5000);

	printf("dsp driver start\r\n");

	dsp_driver_hw_init();

dsp_driver_loop:
	//--printf("dsp loop\r\n");
	// process requests
	dsp_driver_worker();
	//--OsDelayMs(150);					osMessageGet will stall the driver anyway, so no need to idle here!
	goto dsp_driver_loop;
}

#endif
