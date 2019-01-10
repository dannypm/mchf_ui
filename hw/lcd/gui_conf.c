

#include "mchf_pro_board.h"

#include "gui_conf.h"
#include "GUI.h"
#include "lcd.h"

#ifdef USE_INT_RAM
//-----------------------------------
// Use chip internal RAM
#define GUI_NUMBYTES  (64) *  1024
U32 aMemory[GUI_NUMBYTES / 4];
//-----------------------------------
#endif

#ifdef USE_SDRAM
//
#ifdef CHIP_F7
#define GUI_NUMBYTES  EMWIN_RAM_SIZE
ulong   *aMemory = (ulong *)0xC0000000;
#endif
//
#ifdef CHIP_H7
#define GUI_NUMBYTES  EMWIN_RAM_SIZE
ulong   *aMemory = (ulong *)0xC0000000;
#endif
//
#endif

/*********************************************************************
*
*       GUI_X_Config
*
* Purpose:
*   Called during the initialization process in order to set up the
*   available memory for the GUI.
*/
void GUI_X_Config(void) 
{
	#ifdef LCD_DRIVER_DEBUG
	printf("GUI_X_Config\r\n");
	#endif

	GUI_ALLOC_AssignMemory(aMemory, GUI_NUMBYTES);

	#ifdef LCD_DRIVER_DEBUG
	printf("GUI_X_Config ok\r\n");
	#endif
}

/*************************** End of file ****************************/
