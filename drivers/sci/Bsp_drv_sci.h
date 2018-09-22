/**************************************************************************
  头文件包含                            
**************************************************************************/
#include <mach/DrvInterface.h>
#include "product_config.h"

/**************************************************************************
  宏定义
**************************************************************************/
//GPIO 
#ifdef FEATURE_SIM_NOTIFY


#define SCI_PAD_DIR_INPUT       (0)
#define SCI_PAD_DIR_OUTPUT      (1)

#define SIM_GPIO_GROUP 	   2
#define SIM_GPIO_INT_PIN   9


#define SIM_DETECT_INFO_NO 3

#define INT_GPIO_2                       114

BSP_U32 SCI_Init();

#endif

//





