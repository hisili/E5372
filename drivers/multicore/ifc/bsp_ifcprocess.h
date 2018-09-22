/*************************************************************************
*   版权所有(C) 1987-2004, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_IFCProcess.h
*
*   作    者 :  liumengcun
*
*   描    述 :  底软核间调用处理头文件
*
*   修改记录 :  2011年1月11日  v1.00  liumengcun  创建
*
*************************************************************************/

#ifndef _BSP_IFCP_H_
#define _BSP_IFCP_H_

#ifdef __cplusplus
extern "C" 
{
#endif
#include <mach/hardware.h>
#include "BSP.h"

#define IFCP_INIT_FLAG					0x49464350 /* 初始化完成标志 IFCP AISC码*/
#define IFCP_INIT_DELAY_TIMES			500  /* 5S*/
#define IFCP_MODEM_INIT_FLAG_ADDR		(MEMORY_AXI_IFC_ADDR + 8)
#define IFCP_APP_INIT_FLAG_ADDR			(MEMORY_AXI_IFC_ADDR + 12)

#ifdef PRODUCT_CFG_CORE_TYPE_MODEM
#define IFCP_INIT_FLAG_ADDR				IFCP_MODEM_INIT_FLAG_ADDR
#define IFCP_OTHER_CORE_INIT_FLAG_ADDR	IFCP_APP_INIT_FLAG_ADDR
#else
#define IFCP_INIT_FLAG_ADDR				IFCP_APP_INIT_FLAG_ADDR
#define IFCP_OTHER_CORE_INIT_FLAG_ADDR	IFCP_MODEM_INIT_FLAG_ADDR
#endif



#ifdef __cplusplus
}
#endif


#endif //end of "_BSP_IFC_H_"




