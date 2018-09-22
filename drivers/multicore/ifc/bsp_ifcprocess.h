/*************************************************************************
*   ��Ȩ����(C) 1987-2004, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_IFCProcess.h
*
*   ��    �� :  liumengcun
*
*   ��    �� :  ����˼���ô���ͷ�ļ�
*
*   �޸ļ�¼ :  2011��1��11��  v1.00  liumengcun  ����
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

#define IFCP_INIT_FLAG					0x49464350 /* ��ʼ����ɱ�־ IFCP AISC��*/
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




