

#include "mchf_pro_board.h"

#ifdef CHIP_F7
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_cortex.h"
#include "stm32f7xx_hal_gpio.h"
#include "stm32f7xx_hal_rcc_ex.h"
//#include "stm32f7xx_ll_fmc.h"
//#include "stm32f7xx_hal_sdram.h"
#endif

#ifdef CHIP_H7
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_cortex.h"
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_rcc_ex.h"
#include "stm32h7xx_hal_rcc.h"
#include "stm32h7xx_hal_mdma.h"

MDMA_HandleTypeDef mdma_handle;
#endif

#include "sdram.h"

static SDRAM_HandleTypeDef 			sdramHandle;
static FMC_SDRAM_TimingTypeDef 		Timing;
static FMC_SDRAM_CommandTypeDef 	Command;

uint8_t BSP_SDRAM_Init(void)
{ 
  static uint8_t sdramstatus = SDRAM_ERROR;

  /* SDRAM device configuration */
  sdramHandle.Instance = FMC_SDRAM_DEVICE;

  // Timing configuration for 100Mhz as SD clock frequency (System clock is up to 200Mhz)
  Timing.LoadToActiveDelay    			= 2;	//2;
  Timing.ExitSelfRefreshDelay 			= 7;	//7;
  Timing.SelfRefreshTime      			= 4;	//4;
  Timing.RowCycleDelay        			= 7;	//7;
  Timing.WriteRecoveryTime    			= 2;	//2;
  Timing.RPDelay              			= 2;	//2;
  Timing.RCDDelay             			= 2;	//2;

#ifdef CHIP_F7
  // Only in 16 bit mode, half bandwidth
  sdramHandle.Init.MemoryDataWidth    	= FMC_SDRAM_MEM_BUS_WIDTH_16;
#endif

#ifdef CHIP_H7
  // We could use the SDRAM in 32 bit on the H7 because of the free FMC_NBL2 signal (pin173)
  sdramHandle.Init.MemoryDataWidth    	= FMC_SDRAM_MEM_BUS_WIDTH_32;
#endif

  // On F7 and H7, bank1 is mapped at 0xC0000000
  sdramHandle.Init.SDBank             	= FMC_SDRAM_BANK1;

  // ToDo: check if matching the pdf
  sdramHandle.Init.ColumnBitsNumber   	= FMC_SDRAM_COLUMN_BITS_NUM_8;
  sdramHandle.Init.RowBitsNumber      	= FMC_SDRAM_ROW_BITS_NUM_12;
  sdramHandle.Init.InternalBankNumber 	= FMC_SDRAM_INTERN_BANKS_NUM_4;

  // Main bandwidth limiting factor
  sdramHandle.Init.CASLatency         	= FMC_SDRAM_CAS_LATENCY_2;

  // F7: 108Mhz or 72MHz clock
  // H7: 200Mhz or 133MHz clock
  sdramHandle.Init.SDClockPeriod      	= FMC_SDRAM_CLOCK_PERIOD_3;

  // --
  sdramHandle.Init.WriteProtection    	= FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  sdramHandle.Init.ReadBurst          	= FMC_SDRAM_RBURST_ENABLE;
  sdramHandle.Init.ReadPipeDelay      	= FMC_SDRAM_RPIPE_DELAY_0;

  // SDRAM controller initialisation
  BSP_SDRAM_MspInit(&sdramHandle, NULL); // __weak function can be rewritten by the application

  if(HAL_SDRAM_Init(&sdramHandle, &Timing) != HAL_OK)
  {
    sdramstatus = SDRAM_ERROR;
  }
  else
  {
    sdramstatus = SDRAM_OK;
  }
  
  /* SDRAM initialisation sequence */
  BSP_SDRAM_Initialization_sequence(REFRESH_COUNT);
  
  return sdramstatus;
}

uint8_t BSP_SDRAM_DeInit(void)
{ 
  uint8_t sdramstatus = SDRAM_ERROR;

  /* SDRAM device de-initialization */
  sdramHandle.Instance = FMC_SDRAM_DEVICE;

  if(HAL_SDRAM_DeInit(&sdramHandle) != HAL_OK)
  {
    sdramstatus = SDRAM_ERROR;
  }
  else
  {
    sdramstatus = SDRAM_OK;
  }
  
  /* SDRAM controller de-initialization */
  BSP_SDRAM_MspDeInit(&sdramHandle, NULL);
  
  return sdramstatus;
}

/**
  * @brief  Programs the SDRAM device.
  * @param  RefreshCount: SDRAM refresh counter value 
  * @retval None
  */
void BSP_SDRAM_Initialization_sequence(uint32_t RefreshCount)
{
  __IO uint32_t tmpmrd = 0;
  
  /* Step 1: Configure a clock configuration enable command */
  Command.CommandMode            = FMC_SDRAM_CMD_CLK_ENABLE;
  Command.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
  Command.AutoRefreshNumber      = 1;
  Command.ModeRegisterDefinition = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(&sdramHandle, &Command, SDRAM_TIMEOUT);

  /* Step 2: Insert 100 us minimum delay */ 
  /* Inserted delay is equal to 1 ms due to systick time base unit (ms) */
  HAL_Delay(1);
    
  /* Step 3: Configure a PALL (precharge all) command */ 
  Command.CommandMode            = FMC_SDRAM_CMD_PALL;
  Command.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
  Command.AutoRefreshNumber      = 1;
  Command.ModeRegisterDefinition = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(&sdramHandle, &Command, SDRAM_TIMEOUT);  
  
  /* Step 4: Configure an Auto Refresh command */ 
  Command.CommandMode            = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
  Command.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
  Command.AutoRefreshNumber      = 8;
  Command.ModeRegisterDefinition = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(&sdramHandle, &Command, SDRAM_TIMEOUT);
  
  /* Step 5: Program the external memory mode register */
  tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1          |\
                     SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |\
                     SDRAM_MODEREG_CAS_LATENCY_2           |\
                     SDRAM_MODEREG_OPERATING_MODE_STANDARD |\
                     SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
  
  Command.CommandMode            = FMC_SDRAM_CMD_LOAD_MODE;
  Command.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
  Command.AutoRefreshNumber      = 1;
  Command.ModeRegisterDefinition = tmpmrd;

  /* Send the command */
  HAL_SDRAM_SendCommand(&sdramHandle, &Command, SDRAM_TIMEOUT);
  
  /* Step 6: Set the refresh rate counter */
  /* Set the device refresh rate */
  HAL_SDRAM_ProgramRefreshRate(&sdramHandle, RefreshCount); 
}

/**
  * @brief  Reads an amount of data from the SDRAM memory in polling mode.
  * @param  uwStartAddress: Read start address
  * @param  pData: Pointer to data to be read  
  * @param  uwDataSize: Size of read data from the memory
  * @retval SDRAM status
  */
uint8_t BSP_SDRAM_ReadData(uint32_t uwStartAddress, uint32_t *pData, uint32_t uwDataSize)
{
  if(HAL_SDRAM_Read_32b(&sdramHandle, (uint32_t *)uwStartAddress, pData, uwDataSize) != HAL_OK)
  {
    return SDRAM_ERROR;
  }
  else
  {
    return SDRAM_OK;
  }
}

/**
  * @brief  Reads an amount of data from the SDRAM memory in DMA mode.
  * @param  uwStartAddress: Read start address
  * @param  pData: Pointer to data to be read  
  * @param  uwDataSize: Size of read data from the memory
  * @retval SDRAM status
  */
uint8_t BSP_SDRAM_ReadData_DMA(uint32_t uwStartAddress, uint32_t *pData, uint32_t uwDataSize)
{
  if(HAL_SDRAM_Read_DMA(&sdramHandle, (uint32_t *)uwStartAddress, pData, uwDataSize) != HAL_OK)
  {
    return SDRAM_ERROR;
  }
  else
  {
    return SDRAM_OK;
  }
}

/**
  * @brief  Writes an amount of data to the SDRAM memory in polling mode.
  * @param  uwStartAddress: Write start address
  * @param  pData: Pointer to data to be written  
  * @param  uwDataSize: Size of written data from the memory
  * @retval SDRAM status
  */
uint8_t BSP_SDRAM_WriteData(uint32_t uwStartAddress, uint32_t *pData, uint32_t uwDataSize) 
{
  if(HAL_SDRAM_Write_32b(&sdramHandle, (uint32_t *)uwStartAddress, pData, uwDataSize) != HAL_OK)
  {
    return SDRAM_ERROR;
  }
  else
  {
    return SDRAM_OK;
  }
}

/**
  * @brief  Writes an amount of data to the SDRAM memory in DMA mode.
  * @param  uwStartAddress: Write start address
  * @param  pData: Pointer to data to be written  
  * @param  uwDataSize: Size of written data from the memory
  * @retval SDRAM status
  */
uint8_t BSP_SDRAM_WriteData_DMA(uint32_t uwStartAddress, uint32_t *pData, uint32_t uwDataSize) 
{
  if(HAL_SDRAM_Write_DMA(&sdramHandle, (uint32_t *)uwStartAddress, pData, uwDataSize) != HAL_OK)
  {
    return SDRAM_ERROR;
  }
  else
  {
    return SDRAM_OK;
  } 
}

/**
  * @brief  Sends command to the SDRAM bank.
  * @param  SdramCmd: Pointer to SDRAM command structure 
  * @retval SDRAM status
  */  
uint8_t BSP_SDRAM_Sendcmd(FMC_SDRAM_CommandTypeDef *SdramCmd)
{
  if(HAL_SDRAM_SendCommand(&sdramHandle, SdramCmd, SDRAM_TIMEOUT) != HAL_OK)
  {
    return SDRAM_ERROR;
  }
  else
  {
    return SDRAM_OK;
  }
}

/**
  * @brief  Handles SDRAM DMA transfer interrupt request.
  * @retval None
  */
void BSP_SDRAM_DMA_IRQHandler(void)
{
#ifdef CHIP_F7
  HAL_DMA_IRQHandler(sdramHandle.hdma);
#endif
}

/**
  * @brief  Initializes SDRAM MSP.
  * @param  hsdram: SDRAM handle
  * @param  Params
  * @retval None
  */
__weak void BSP_SDRAM_MspInit(SDRAM_HandleTypeDef  *hsdram, void *Params)
{  
#ifdef BOARD_DISCO
	  static DMA_HandleTypeDef dma_handle;
	  GPIO_InitTypeDef gpio_init_structure;

	  /* Enable FMC clock */
	  __HAL_RCC_FMC_CLK_ENABLE();

	  /* Enable chosen DMAx clock */
	  __DMAx_CLK_ENABLE();

	  /* Enable GPIOs clock */
	  __HAL_RCC_GPIOC_CLK_ENABLE();
	  __HAL_RCC_GPIOD_CLK_ENABLE();
	  __HAL_RCC_GPIOE_CLK_ENABLE();
	  __HAL_RCC_GPIOF_CLK_ENABLE();
	  __HAL_RCC_GPIOG_CLK_ENABLE();
	  __HAL_RCC_GPIOH_CLK_ENABLE();

	  /* Common GPIO configuration */
	  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
	  gpio_init_structure.Pull      = GPIO_PULLUP;
	  gpio_init_structure.Speed     = GPIO_SPEED_FAST;
	  gpio_init_structure.Alternate = GPIO_AF12_FMC;

	  /* GPIOC configuration */
	  gpio_init_structure.Pin   = GPIO_PIN_3;
	  HAL_GPIO_Init(GPIOC, &gpio_init_structure);

	  /* GPIOD configuration */
	  gpio_init_structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_8 | GPIO_PIN_9 |
	                              GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15;
	  HAL_GPIO_Init(GPIOD, &gpio_init_structure);

	  /* GPIOE configuration */
	  gpio_init_structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_7| GPIO_PIN_8 | GPIO_PIN_9 |\
	                              GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 |\
	                              GPIO_PIN_15;
	  HAL_GPIO_Init(GPIOE, &gpio_init_structure);

	  /* GPIOF configuration */
	  gpio_init_structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3 | GPIO_PIN_4 |\
	                              GPIO_PIN_5 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 |\
	                              GPIO_PIN_15;
	  HAL_GPIO_Init(GPIOF, &gpio_init_structure);

	  /* GPIOG configuration */
	  gpio_init_structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4| GPIO_PIN_5 | GPIO_PIN_8 |\
	                              GPIO_PIN_15;
	  HAL_GPIO_Init(GPIOG, &gpio_init_structure);

	  /* GPIOH configuration */
	  gpio_init_structure.Pin   = GPIO_PIN_3 | GPIO_PIN_5;
	  HAL_GPIO_Init(GPIOH, &gpio_init_structure);

	  /* Configure common DMA parameters */
	  dma_handle.Init.Channel             = SDRAM_DMAx_CHANNEL;
	  dma_handle.Init.Direction           = DMA_MEMORY_TO_MEMORY;
	  dma_handle.Init.PeriphInc           = DMA_PINC_ENABLE;
	  dma_handle.Init.MemInc              = DMA_MINC_ENABLE;
	  dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	  dma_handle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
	  dma_handle.Init.Mode                = DMA_NORMAL;
	  dma_handle.Init.Priority            = DMA_PRIORITY_HIGH;
	  dma_handle.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
	  dma_handle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
	  dma_handle.Init.MemBurst            = DMA_MBURST_SINGLE;
	  dma_handle.Init.PeriphBurst         = DMA_PBURST_SINGLE;

	  dma_handle.Instance = SDRAM_DMAx_STREAM;

	   /* Associate the DMA handle */
	  __HAL_LINKDMA(hsdram, hdma, dma_handle);

	  /* Deinitialize the stream for new transfer */
	  HAL_DMA_DeInit(&dma_handle);

	  /* Configure the DMA stream */
	  HAL_DMA_Init(&dma_handle);

	  /* NVIC configuration for DMA transfer complete interrupt */
	  HAL_NVIC_SetPriority(SDRAM_DMAx_IRQn, 5, 0);
	  HAL_NVIC_EnableIRQ(SDRAM_DMAx_IRQn);
#endif

#ifdef CHIP_F7
  static DMA_HandleTypeDef dma_handle;
  GPIO_InitTypeDef gpio_init_structure;
  
  // Enable FMC clock
  __HAL_RCC_FMC_CLK_ENABLE();
  
  // Enable chosen DMAx clock
  __DMAx_CLK_ENABLE();

  // Enable GPIOs clock
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();

  // Common GPIO configuration
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_PULLUP;
  gpio_init_structure.Speed     = GPIO_SPEED_FAST;
  gpio_init_structure.Alternate = GPIO_AF12_FMC;
  
  // ------------------------------------------------------------------------------------------------------
  // GPIOC configuration
  //						  FMC_SDCKE0
  //
  gpio_init_structure.Pin   = GPIO_PIN_3;
  HAL_GPIO_Init(GPIOC, &gpio_init_structure);

  // ------------------------------------------------------------------------------------------------------
  // GPIOD configuration
  //							FMC_D2		FMC_D3			FMC_D13		FMC_D14
  //							FMC_D15		FMC_D0			FMC_D1
  //
  gpio_init_structure.Pin   = GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_8 | GPIO_PIN_9 |
                              GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOD, &gpio_init_structure);

  // ------------------------------------------------------------------------------------------------------
  // GPIOE configuration
  //							FMC_NBL0	FMC_NBL1		FMC_D4		FMC_D5			FMC_D6
  //							FMC_D7		FMC_D8			FMC_D9		FMC_D10			FMC_D11
  //							FMC_D12
  //
  gpio_init_structure.Pin   = GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_7  | GPIO_PIN_8  | GPIO_PIN_9 |\
                              GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 |\
                              GPIO_PIN_15;
  HAL_GPIO_Init(GPIOE, &gpio_init_structure);
  
  // ------------------------------------------------------------------------------------------------------
  // GPIOF configuration
  //							FMC_A0		FMC_A1			FMC_A2		FMC_A3			FMC_A4
  //							FMC_A5		FMC_SDNRAS		FMC_A6		FMC_A7			FMC_A8
  //							FMC_A9
  //
  gpio_init_structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1  | GPIO_PIN_2  | GPIO_PIN_3  | GPIO_PIN_4 |\
                              GPIO_PIN_5 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 |\
                              GPIO_PIN_15;
  HAL_GPIO_Init(GPIOF, &gpio_init_structure);
  
  // ------------------------------------------------------------------------------------------------------
  // GPIOG configuration
  //							FMC_A10		FMC_A11			FMC_BA0		FMC_BA11		FMC_SDCLK
  //							FMC_SDNCAS
  //
  gpio_init_structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 |  GPIO_PIN_4 |  GPIO_PIN_5 |  GPIO_PIN_8 |\
                              GPIO_PIN_15;
  HAL_GPIO_Init(GPIOG, &gpio_init_structure);

  // ------------------------------------------------------------------------------------------------------
  // GPIOH configuration
  //							FMC_SDCNE0	FMC_SDNWE		FMC_D16		FMC_D17			FMC_D18
  //							FMC_D19		FMC_D20			FMC_D21		FMC_D22			FMC_D23
  //
  gpio_init_structure.Pin   = GPIO_PIN_3  | GPIO_PIN_5;

//  gpio_init_structure.Pin   = GPIO_PIN_2  | GPIO_PIN_4  |  GPIO_PIN_7 |  GPIO_PIN_8 |  GPIO_PIN_9 |\
//  		  	  	  	  	  	  GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14;

  HAL_GPIO_Init(GPIOH, &gpio_init_structure); 
  
  // ------------------------------------------------------------------------------------------------------
  // GPIOI configuration
  //							FMC_D24		FMC_D25			FMC_D26		FMC_D27		FMC_D28
  //							FMC_D29		FMC_D30			FMC_D31
  //
//  gpio_init_structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_6 |\
  		  	  	  	  	  	  GPIO_PIN_7 | GPIO_PIN_9 | GPIO_PIN_10;
//  HAL_GPIO_Init(GPIOI, &gpio_init_structure);

  // -------------------------------------------------
  // Temporary all higher data bits as outputs and low
  gpio_init_structure.Pin =  GPIO_PIN_7 |  GPIO_PIN_8 |  GPIO_PIN_9 |\
		    		  	  	 GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14;
  gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
  gpio_init_structure.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOH, &gpio_init_structure);

  gpio_init_structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_6 |\
    		  	  	  	  	  GPIO_PIN_7 | GPIO_PIN_9 | GPIO_PIN_10;
  gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
  gpio_init_structure.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOI, &gpio_init_structure);

  // Already have pull downs, need to put low ??
  //HAL_GPIO_WritePin(GPIOH,GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14, GPIO_PIN_RESET);
  //HAL_GPIO_WritePin(GPIOI,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_9|GPIO_PIN_10,     GPIO_PIN_RESET);

  // ------------------------------------------------------------------------------------------------------
  // Configure common DMA parameters
  dma_handle.Init.Channel             = SDRAM_DMAx_CHANNEL;
  dma_handle.Init.Direction           = DMA_MEMORY_TO_MEMORY;
  dma_handle.Init.PeriphInc           = DMA_PINC_ENABLE;
  dma_handle.Init.MemInc              = DMA_MINC_ENABLE;
  dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  dma_handle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
  dma_handle.Init.Mode                = DMA_NORMAL;
  dma_handle.Init.Priority            = DMA_PRIORITY_HIGH;
  dma_handle.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;         
  dma_handle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  dma_handle.Init.MemBurst            = DMA_MBURST_SINGLE;
  dma_handle.Init.PeriphBurst         = DMA_PBURST_SINGLE; 
  
  dma_handle.Instance = SDRAM_DMAx_STREAM;
  
   // Associate the DMA handle
  __HAL_LINKDMA(hsdram, hdma, dma_handle);
  
  // Deinitialize the stream for new transfer
  HAL_DMA_DeInit(&dma_handle);
  
  // Configure the DMA stream
  HAL_DMA_Init(&dma_handle); 
  
  // NVIC configuration for DMA transfer complete interrupt
  HAL_NVIC_SetPriority(SDRAM_DMAx_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(SDRAM_DMAx_IRQn);
#endif

#ifdef CHIP_H7
  GPIO_InitTypeDef gpio_init_structure;

  /* Enable FMC clock */
  __HAL_RCC_FMC_CLK_ENABLE();

  /* Enable chosen MDMAx clock */
  __MDMAx_CLK_ENABLE();

  // Enable GPIOs clock
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();

  /* Common GPIO configuration */
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_PULLUP;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init_structure.Alternate = GPIO_AF12_FMC;

   // ------------------------------------------------------------------------------------------------------
   // GPIOC configuration
   //						  FMC_SDCKE0
   //
   gpio_init_structure.Pin   = GPIO_PIN_3;
   HAL_GPIO_Init(GPIOC, &gpio_init_structure);

   // ------------------------------------------------------------------------------------------------------
   // GPIOD configuration
   //							FMC_D2		FMC_D3			FMC_D13		FMC_D14
   //							FMC_D15		FMC_D0			FMC_D1
   //
   gpio_init_structure.Pin   = GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_8 | GPIO_PIN_9 |\
                               GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15;
   HAL_GPIO_Init(GPIOD, &gpio_init_structure);

   // ------------------------------------------------------------------------------------------------------
   // GPIOE configuration
   //							FMC_NBL0	FMC_NBL1		FMC_D4		FMC_D5			FMC_D6
   //							FMC_D7		FMC_D8			FMC_D9		FMC_D10			FMC_D11
   //							FMC_D12
   //
   gpio_init_structure.Pin   = GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_7  | GPIO_PIN_8  | GPIO_PIN_9 |\
                               GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 |\
                               GPIO_PIN_15;
   HAL_GPIO_Init(GPIOE, &gpio_init_structure);

   // ------------------------------------------------------------------------------------------------------
   // GPIOF configuration
   //							FMC_A0		FMC_A1			FMC_A2		FMC_A3			FMC_A4
   //							FMC_A5		FMC_SDNRAS		FMC_A6		FMC_A7			FMC_A8
   //							FMC_A9
   //
   gpio_init_structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1  | GPIO_PIN_2  | GPIO_PIN_3  | GPIO_PIN_4 |\
                               GPIO_PIN_5 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 |\
                               GPIO_PIN_15;
   HAL_GPIO_Init(GPIOF, &gpio_init_structure);

   // ------------------------------------------------------------------------------------------------------
   // GPIOG configuration
   //							FMC_A10		FMC_A11			FMC_BA0		FMC_BA11		FMC_SDCLK
   //							FMC_SDNCAS
   //
   gpio_init_structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 |  GPIO_PIN_4 |  GPIO_PIN_5 |  GPIO_PIN_8 |\
                               GPIO_PIN_15;
   HAL_GPIO_Init(GPIOG, &gpio_init_structure);

   // ------------------------------------------------------------------------------------------------------
   // GPIOH configuration
   //							FMC_SDCNE0	FMC_SDNWE		FMC_D16			FMC_D17			FMC_D18
   //							FMC_D19		FMC_D20			FMC_D21			FMC_D22			FMC_D23
   //
   gpio_init_structure.Pin   =  GPIO_PIN_3  | GPIO_PIN_5  |  GPIO_PIN_8  |  GPIO_PIN_9 |  	GPIO_PIN_10 |\
   		  	  	  	  	  	 	GPIO_PIN_11 | GPIO_PIN_12 |	 GPIO_PIN_13 | GPIO_PIN_14 | 	 GPIO_PIN_15;

   HAL_GPIO_Init(GPIOH, &gpio_init_structure);

   // ------------------------------------------------------------------------------------------------------
   // GPIOI configuration
   //							FMC_D24		FMC_D25			FMC_D26		FMC_D27		FMC_NBL2
   //							FMC_NBL3	FMC_D28			FMC_D29		FMC_D30		FMC_D31
   //
   gpio_init_structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 |\
		   	   	   	   	   	   GPIO_PIN_5 |	GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_9 | GPIO_PIN_10;
   HAL_GPIO_Init(GPIOI, &gpio_init_structure);

   // Configure common MDMA parameters
   mdma_handle.Init.Request 					= MDMA_REQUEST_SW;
   mdma_handle.Init.TransferTriggerMode 		= MDMA_BLOCK_TRANSFER;
   mdma_handle.Init.Priority 					= MDMA_PRIORITY_HIGH;
   mdma_handle.Init.Endianness 					= MDMA_LITTLE_ENDIANNESS_PRESERVE;
   mdma_handle.Init.SourceInc 					= MDMA_SRC_INC_WORD;
   mdma_handle.Init.DestinationInc 				= MDMA_DEST_INC_WORD;
   mdma_handle.Init.SourceDataSize 				= MDMA_SRC_DATASIZE_WORD;
   mdma_handle.Init.DestDataSize 				= MDMA_DEST_DATASIZE_WORD;
   mdma_handle.Init.DataAlignment 				= MDMA_DATAALIGN_PACKENABLE;
   mdma_handle.Init.SourceBurst 				= MDMA_SOURCE_BURST_SINGLE;
   mdma_handle.Init.DestBurst 					= MDMA_DEST_BURST_SINGLE;
   mdma_handle.Init.BufferTransferLength 		= 128;
   mdma_handle.Init.SourceBlockAddressOffset 	= 0;
   mdma_handle.Init.DestBlockAddressOffset 		= 0;

   mdma_handle.Instance = SDRAM_MDMAx_CHANNEL;

   /* Associate the DMA handle */
  __HAL_LINKDMA(hsdram, hmdma, mdma_handle);

  /* Deinitialize the stream for new transfer */
  HAL_MDMA_DeInit(&mdma_handle);

  /* Configure the DMA stream */
  HAL_MDMA_Init(&mdma_handle);

  /* NVIC configuration for DMA transfer complete interrupt */
  HAL_NVIC_SetPriority(SDRAM_MDMAx_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ(SDRAM_MDMAx_IRQn);
#endif
}

/**
  * @brief  DeInitializes SDRAM MSP.
  * @param  hsdram: SDRAM handle
  * @param  Params
  * @retval None
  */
__weak void BSP_SDRAM_MspDeInit(SDRAM_HandleTypeDef  *hsdram, void *Params)
{
#ifdef CHIP_F7
    static DMA_HandleTypeDef dma_handle;
  
    /* Disable NVIC configuration for DMA interrupt */
    HAL_NVIC_DisableIRQ(SDRAM_DMAx_IRQn);

    /* Deinitialize the stream for new transfer */
    dma_handle.Instance = SDRAM_DMAx_STREAM;
    HAL_DMA_DeInit(&dma_handle);

    /* GPIO pins clock, FMC clock and DMA clock can be shut down in the applications
       by surcharging this __weak function */
#endif

#ifdef CHIP_H7
    static MDMA_HandleTypeDef mdma_handle;

    /* Disable NVIC configuration for DMA interrupt */
    HAL_NVIC_DisableIRQ(SDRAM_MDMAx_IRQn);

    /* Deinitialize the stream for new transfer */
    mdma_handle.Instance = SDRAM_MDMAx_CHANNEL;
    HAL_MDMA_DeInit(&mdma_handle);

    /* GPIO pins clock, FMC clock and MDMA clock can be shut down in the applications
       by surcharging this __weak function */
#endif
}



