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

// -----------------------------------
// NOT YET SUPPORT FOR F7 !!!!
// -----------------------------------
#include "mchf_pro_board.h"

#include "sd_driver.h"
#include "menu\items\file_b\browser\k_storage.h"

#if 0
// --
#include "C:\Projects\mcHFx\firmware\ui\hw\sdcard\sd_card.h"
#include "C:\Projects\mcHFx\firmware\ui\middleware\FatFs\src\ff_gen_drv.h"

// --
//#include "sd_diskio_dma_rtos.h"

uchar 						statusChanged = 0;
uchar 						isInitialized = 0;
uchar						isFsCreated   = 0;

osMessageQId 				ConnectionEvent;
const Diskio_drvTypeDef  	SD_Driver;

FATFS 						SDFatFs;  	// File system object for SD card logical drive
FIL 						MyFile;     // File object
char 						SDPath[4];	// SD card logical drive path
uchar						workBuffer[2 * _MAX_SS];

/*
 * ensure that the read buffer 'rtext' is 32-Byte aligned in address and size
 * to guarantee that any other data in the cache won't be affected when the 'rtext'
 * is being invalidated.
 */
ALIGN_32BYTES(uchar rtext[64]);
#endif

#if 0
static void FS_FileOperations(void)
{
	FRESULT res;                                          // FatFs function common result code
	uint32_t byteswritten, bytesread;                     // File write/read counts
	uint8_t wtext[] = "This is STM32 working with FatFs uSD + FreeRTOS"; // File write buffer

	printf("do some file operations.\r\n");

	// Register the file system object to the FatFs module
	if(f_mount(&SDFatFs, (TCHAR const*)SDPath, 0) == FR_OK)
	{
		printf("mount ok\r\n");
		//printf("drive name: %s\r\n",SDPath);

		// check whether the FS has been already created
		//if (isFsCreated == 0)
		//{
		//if(f_mkfs(SDPath, FM_ANY, 0, workBuffer, sizeof(workBuffer)) != FR_OK)
		//{
        //BSP_LED_On(LED3);
    	  //printf("sd error format!\r\n");
        //return;
      	  //}
		//isFsCreated = 1;
		//}

    // Create and Open a new text file object with write access
    //if(f_open(&MyFile, "STM32.TXT", FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
    //{
      // Write data to the text file
      //res = f_write(&MyFile, wtext, sizeof(wtext), (void *)&byteswritten);

      //if((byteswritten > 0) && (res == FR_OK))
      //{
        // Close the open text file
        //f_close(&MyFile);

        // Open the text file object with read access
        if(f_open(&MyFile, "STM32.TXT", FA_READ) == FR_OK)
        {

          // Read data from the text file
          res = f_read(&MyFile, rtext, sizeof(rtext), (void *)&bytesread);

          if((bytesread > 0) && (res == FR_OK))
          {
        	  printf("sd success\r\n");
        	  printf("file: %s\r\n",rtext);

            // Close the open text file
            f_close(&MyFile);

            // Compare read data with the expected data
            //if((bytesread == byteswritten))
            //{
              // Success of the demo: no error occurrence
              //BSP_LED_On(LED1);
              //printf("sd success\r\n");
              //return;
            //}
          }
        }
        else
        	printf("-->file open err\r\n");
      //}
    //}

	}
  // Error
  //BSP_LED_On(LED3);
}
#endif
#if 0
static void SD_Init(void)
{
	uchar sd_init_res;

	if(isInitialized == 0)
	{
		printf("sd card init...\r\n");

		sd_init_res = BSP_SD_Init();
		printf("sd card init result: %d\r\n",sd_init_res);

		if(sd_init_res == MSD_OK)
			isInitialized = 1;
	}
}
#endif
#if 0
//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_TEMPLATE
//*----------------------------------------------------------------------------
static void sd_driver_hw_init(void)
{
	printf("sd driver init\r\n");

	// Init the detect pin
	BSP_SD_ITConfig();

	// Link driver and start state machine
	if(FATFS_LinkDriver(&SD_Driver, SDPath) == 0)
	{
		printf("sd driver fatfs link ok\r\n");
		//printf("drive name: %s\r\n",SDPath);

		// Create Storage Message Queue
		osMessageQDef(osqueue, 10, uint16_t);
		ConnectionEvent = osMessageCreate (osMessageQ(osqueue), NULL);

		if(BSP_SD_IsDetected())
			osMessagePut ( ConnectionEvent, CARD_CONNECTED, osWaitForever);
	}
}
#endif
#if 0
//*----------------------------------------------------------------------------
//* Function Name       :
//* Object              :
//* Notes    			:
//* Notes   			:
//* Notes    			:
//* Context    			: CONTEXT_DRIVER_TEMPLATE
//*----------------------------------------------------------------------------
static void sd_driver_worker(void)
{
	osEvent event;

	event = osMessageGet( ConnectionEvent, osWaitForever );

	if(event.status == osEventMessage)
    {
		switch(event.value.v)
		{
			case CARD_STATUS_CHANGED:
			{
				printf("----------------------------------------------------\r\n");
				printf("sd card status changed\r\n");

				if (BSP_SD_IsDetected())
					osMessagePut ( ConnectionEvent, CARD_CONNECTED, osWaitForever);
				else
					osMessagePut ( ConnectionEvent, CARD_DISCONNECTED, osWaitForever);

				break;
			}

			case CARD_CONNECTED:
			{
				printf("sd card connected\r\n");

				SD_Init();
				FS_FileOperations();

				statusChanged = 0;
				break;
			}

			case CARD_DISCONNECTED:
			{
				printf("sd card disconnected\r\n");

				BSP_SD_DeInit();

				if(isInitialized == 1)
				{
					isInitialized = 0;
					f_mount(NULL, (TCHAR const*)"", 0);
				}

				statusChanged = 0;
				break;
			}
	     }
    }
}
#endif

//*----------------------------------------------------------------------------
//* Function Name       :
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
	//sd_driver_hw_init();
	k_StorageInit();

sd_driver_loop:

	//sd_driver_worker();
	StorageThread();

	// Driver sleep period
	OsDelayMs(150);
		
	goto sd_driver_loop;
}




