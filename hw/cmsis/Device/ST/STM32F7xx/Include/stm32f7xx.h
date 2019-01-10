    
#ifndef __STM32F7xx_H
#define __STM32F7xx_H
  

#if !defined  (STM32F7)
#define STM32F7
#endif /* STM32F7 */

#if !defined (STM32F756xx) && !defined (STM32F746xx) && !defined (STM32F745xx)
  /* #define STM32F756xx */   /*!< STM32F756VG, STM32F756ZG, STM32F756ZG, STM32F756IG, STM32F756BG,
                                   STM32F756NG Devices */
  /* #define STM32F746xx */   /*!< STM32F746VE, STM32F746VG, STM32F746ZE, STM32F746ZG, STM32F746IE, STM32F746IG,
                                   STM32F746BE, STM32F746BG, STM32F746NE, STM32F746NG Devices */
  /* #define STM32F745xx */   /*!< STM32F745VE, STM32F745VG, STM32F745ZG, STM32F745ZE, STM32F745IE, STM32F745IG Devices */
#endif


#if !defined  (USE_HAL_DRIVER)
/**
 * @brief Comment the line below if you will not use the peripherals drivers.
   In this case, these drivers will not be included and the application code will 
   be based on direct access to peripherals registers 
   */
  //#define USE_HAL_DRIVER
#endif /* USE_HAL_DRIVER */

/**
  * @brief CMSIS Device version number V1.0.3
  */
#define __STM32F7xx_CMSIS_DEVICE_VERSION_MAIN   (0x01) /*!< [31:24] main version */
#define __STM32F7xx_CMSIS_DEVICE_VERSION_SUB1   (0x00) /*!< [23:16] sub1 version */
#define __STM32F7xx_CMSIS_DEVICE_VERSION_SUB2   (0x03) /*!< [15:8]  sub2 version */
#define __STM32F7xx_CMSIS_DEVICE_VERSION_RC     (0x00) /*!< [7:0]  release candidate */ 
#define __STM32F7xx_CMSIS_DEVICE_VERSION        ((__STM32F7xx_CMSIS_DEVICE_VERSION_MAIN << 24)\
                                                |(__STM32F7xx_CMSIS_DEVICE_VERSION_SUB1 << 16)\
                                                |(__STM32F7xx_CMSIS_DEVICE_VERSION_SUB2 << 8 )\
                                                |(__STM32F7xx_CMSIS_DEVICE_VERSION))
#if defined(STM32F756xx)
  #include "stm32f756xx.h"
#elif defined(STM32F746xx)
  #include "stm32f746xx.h"
#elif defined(STM32F745xx)
  #include "stm32f745xx.h"
#else
 #error "Please select first the target STM32F7xx device used in your application (in stm32f7xx.h file)"
#endif

#if 0
typedef enum 
{
  RESET = 0, 
  SET = !RESET
} FlagStatus, ITStatus;

typedef enum 
{
  DISABLE = 0, 
  ENABLE = !DISABLE
} FunctionalState;
#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

typedef enum 
{
  ERROR = 0, 
  SUCCESS = !ERROR
} ErrorStatus;

#define SET_BIT(REG, BIT)     ((REG) |= (BIT))

#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))

#define READ_BIT(REG, BIT)    ((REG) & (BIT))

#define CLEAR_REG(REG)        ((REG) = (0x0))

#define WRITE_REG(REG, VAL)   ((REG) = (VAL))

#define READ_REG(REG)         ((REG))

#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

#define POSITION_VAL(VAL)     (__CLZ(__RBIT(VAL))) 
#endif

#ifdef USE_HAL_DRIVER
//  #include "stm32f7xx_hal_conf.h"
#endif /* USE_HAL_DRIVER */



#endif /* __STM32F7xx_H */
