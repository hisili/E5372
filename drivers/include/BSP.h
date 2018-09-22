/*************************************************************************
*   ��Ȩ����(C) 1987-2011, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  Drvinterface.h
*
*   ��    �� :  yangzhi
*
*   ��    �� :  ���ļ�����Ϊ"Drvinterface.h", ����V7R1�����Э��ջ֮���API�ӿ�ͳ��
*
*   �޸ļ�¼ :  2011��1��18��  v1.00  yangzhi����
*************************************************************************/

#ifndef __BSP_H__
#define __BSP_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#define VOS_WIN32   (1)
#define VOS_OS_VER  (3)

#ifdef __KERNEL__
//#include <asm/io.h>
#include <mach/hardware.h>
#include <generated/product_config.h>
#include <generated/FeatureConfigDRV.h>
#include <generated/FeatureConfig.h>
#else
#include "product_config.h"
#include "FeatureConfigDRV.h"
#include "FeatureConfig.h"
#endif

#include "DrvInterface.h"

#include "memMapGlobal.h"

#include "BSP_LOG.h"
#include "BSP_REG.h"
#include "BSP_TRACE.h"
#include "BSP_OSA.h"
#include "BSP_MEM_API.h"

#include "BSP_GPIO.h"
#include "BSP_INT.h"
#include "BSP_BBPINT.h"
#include "BSP_USRCLK.h"
#include "BSP_SYSCLK.h"

#include "BSP_SCI.h"
#include "BSP_SPI_API.h"
#include "BSP_PMU_API.h"
#include "BSP_WDT.h"

#include "BSP_SD.h"
#include "BSP_CIPHER.h"
#include "BSP_SOCP.h"

#include "BSP_DMAC.h"
#include "BSP_NAND.h"
#include "BSP_OM_API.h"

#include "BSP_GMAC.h"
#include "BSP_IPCONFIG_API.h"
#include "BSP_FTPLOAD.h"

#include "BSP_ICC.h"
#include "BSP_IFC_API.h"

#include "BSP_USB.h"
#include "BSP_USB_NCM.h"
#include "BSP_ACM.h"

#include "BSP_SEC.h"
#include "BSP_SSI.h"
#include "BSP_HKADC.h"
#include "BSP_HUAWEI_INFO.h"

#include "BSP_AUDIO.h"
#include "BSP_SCREEN.h"
#include "BSP_CHG_API.h"
#include "BSP_ONOFF.h"
#include "BSP_WIFI.h"

#include "BSP_DEVICE_EVENT.h"
#include "BSP_PWRCTRL_API.h"
#include "BSP_DPM.h"
#include "BSP_OM_API.h"
/*Added by l00212112, 20120621, starts, ����Զ���ļ�ϵͳ�ӿ�,������Ҫ�������ͷ�ļ�*/
#if defined (FEATURE_FLASH_LESS)
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM) || defined(__VXWORKS__)
//#include "FeatureConfig.h"
#include <stdio.h>
#include "FileSysInterface.h"
#endif
#endif
/*ends*/


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif


#endif /* end of __BSP_H__ */


