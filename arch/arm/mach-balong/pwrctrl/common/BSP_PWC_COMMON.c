/*******************************************************************************
* Copyright (C), 2010-2011, HUAWEI Tech. Co., Ltd
*
* File name:
*                pwcCommon.c
* Description:
*                sleep managerment
*
* Author:        ������
* Date:          2011-09-28
* Version:       1.0
*
*
*
* History:
* Author:		������
* Date:			2011-09-28
* Description:	Initial version
*1.
*******************************************************************************/
/*V3R2 CS f00164371*/
#include "generated/FeatureConfigDRV.h"

#if ((defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
#include <linux/linkage.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/interrupt.h>

#include <mach/balong_v100r001.h>
#include "BSP.h"
#if (defined(CHIP_BB_6920ES)||defined (CHIP_BB_6920CS))
#include <mach/pwrctrl/v7r1/BSP_PWC_SOCREG.h>
#else
#include <mach/pwrctrl/v3r2/BSP_PWC_SOCREG.h>
#endif
#include <mach/pwrctrl/BSP_PWC_COMMON.h>
#include <mach/pwrctrl/BSP_PWC_SLEEP.h>
#include <mach/pwrctrl/BSP_PWC_SLEEPMGR.h>
#include <mach/pwrctrl/BSP_PWC_DFS.h>

/*�ñ���ͨ��NV���ȡ���ù��ܿ��Թر�*/
ST_PWC_SWITCH_STRU g_stPwrctrlSwitch = {0};

unsigned int g_ulPwrctrlDebugTrace = DEBUG_OFF;


void PWRCTRL_ChangeGlobalDebugTrace(unsigned int value)
{
    g_ulPwrctrlDebugTrace = value;
    return;

}
static int __init PWRCTRL_CommonInitial(void)
{

    

    #if defined (BOARD_SFT) && defined (VERSION_V3R2)
	unsigned int * pVtmp = NULL;
	
    pVtmp = (unsigned int *)(&g_stPwrctrlSwitch);
    * pVtmp = (unsigned int)(0xffffffff);
#elif ((defined (CHIP_BB_6920ES) || defined (CHIP_BB_6920CS))\
		 && (defined (BOARD_ASIC)||defined (BOARD_ASIC_BIGPACK)))
	
    //pVtmp = (unsigned int *)(&g_stPwrctrlSwitch);
    //* pVtmp = (unsigned int)(0x0);
    #else  
	unsigned int * pVtmp = NULL;
	
    pVtmp = (unsigned int *)(&g_stPwrctrlSwitch);
    * pVtmp = (unsigned int)(0x00000000);
    #endif

#if (defined (BOARD_SFT) && defined (VERSION_V3R2)) \
    ||(defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2))\
    ||((defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)) \
		 && (defined (BOARD_ASIC)||defined (BOARD_ASIC_BIGPACK)))
    PWRCTRL_SleepInitial();
    PWRCTRL_SleepMgrInitial();
    PWRCTRL_DfsAcpuInit();
		
    printk("PWRCTRL: system initial! \n");
    #endif
    return PWRCTRL_OK;
}
/*����flashless,������PWRCTRL_SleepInitialʱ nv��û����ɳ�ʼ����
������Ҫ�Ƴٶ�ȡNV�ļ��еĵ͹������ã����������������rfile_init_cb�е���*/
#if defined (FEATURE_FLASH_LESS)
void PWRCTRL_ReInitialDfs_inRfile(void)
{
#if (defined (BOARD_SFT) && defined (VERSION_V3R2)) \
    ||(defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2))\
    ||((defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)) \
		 && (defined (BOARD_ASIC)||defined (BOARD_ASIC_BIGPACK)))
    PWRCTRL_SleepInitial();
    PWRCTRL_SleepMgrInitial();
    PWRCTRL_DfsAcpuInit();
		
    printk("PWRCTRL_ReInitialDfs_inRfile! \n");
    #endif
}
#endif
/*ends*/

/*****************************************************************************
 Function   : PWRCTRL_WriteReg32
 Description: write 32bit data
 Input      : ulRegAddr -- address
            : ulRegVal -- 32bit data
 Return     : void
 Other      :
*****************************************************************************/
VOID PWRCTRL_WriteReg32( UINT32 ulRegAddr, UINT32 ulRegVal)
{
    if (0 != (ulRegAddr & 0x03))/* not 4byte aligned */
    {
        printk("PWRCTRL_WriteReg32 Address: 0x%x not aligned.\r\n", (int)ulRegAddr);
        return;
    }

    *(volatile UINT32 *)ulRegAddr = ulRegVal;
}
/*****************************************************************************
 Function   : PWRCTRL_ReadReg32
 Description: read 32bit data 
 Input      : ulRegAddr -- address
            : ulRegVal -- 32bit data
 Return     : void
 Other      :
*****************************************************************************/
UINT32 PWRCTRL_ReadReg32( UINT32 ulRegAddr )
{
    if (0 != (ulRegAddr & 0x03))/* not 4byte aligned */
    {
        printk("HPA_Read32Reg Address: 0x%x not aligned.\r\n", (int)ulRegAddr);
        return 0;
    }

    return *(volatile UINT32 *)ulRegAddr;
}
/*****************************************************************************
 Function   : PWRCTRL_WriteReg32Mask
 Description: write mask 
 Input      : ulRegAddr -- address 
            : ulMask -- mask
 Return     : void
 Other      : caution: BIT_N must be used befor call the function
*****************************************************************************/
VOID PWRCTRL_WriteReg32Mask(UINT32 ulRegAddr, UINT32 ulMask)
{
    if (0 != (ulRegAddr & 0x03))/* not 4byte aligned */
    {
        printk("PWRCTRL_WriteReg32Mask Address: 0x%x not aligned.\r\n", (int)ulRegAddr);
        return;
    }

    *(volatile UINT32 *)ulRegAddr |= ulMask;
}

/*****************************************************************************
 Function   : PWRCTRL_ClearReg32Mask
 Description: clear mask 
 Input      : ulRegAddr -- address 
            : ulMask -- mask
 Return     : void
 Other      : caution: BIT_N must be used befor call the function
*****************************************************************************/
VOID PWRCTRL_ClearReg32Mask(UINT32 ulRegAddr, UINT32 ulMask)
{
    if (0 != (ulRegAddr & 0x03))/* not 4byte aligned */
    {
        printk("PWRCTRL_ClearReg32Mask Address: 0x%x not aligned.\r\n", (int)ulRegAddr);
        return;
    }

    *(volatile UINT32 *)ulRegAddr &= ~ulMask;
}

#ifdef CHIP_BB_6920CS

/*****************************************************************************
 Function   : BSP_PWC_SetTimer4WakeSrc
 Description: ����timer4��Ϊ����Դ
 Input      : 
 Return     : void
 Other      : 
*****************************************************************************/
VOID BSP_PWC_SetTimer4WakeSrc(VOID)
{
	BSP_REG_WRITEBITS(0,PWR_WAKEUP_INT_ENABLE,PWC_WAKEUP_TIMER4);
}

/*****************************************************************************
 Function   : BSP_PWC_DelTimer4WakeSrc
 Description: ����timer4����Ϊ����Դ 
 Input      :  
            : 
 Return     : void
 Other      : 
*****************************************************************************/
VOID BSP_PWC_DelTimer4WakeSrc(VOID)
{
	BSP_REG_CLRBIT(0,PWR_WAKEUP_INT_ENABLE,PWC_WAKEUP_TIMER4);
}
#else
/*****************************************************************************
 Function   : BSP_PWC_SetTimer4WakeSrc
 Description: clear mask 
 Input      : ulRegAddr -- address 
            : ulMask -- mask
 Return     : void
 Other      : caution: BIT_N must be used befor call the function
*****************************************************************************/
VOID BSP_PWC_SetTimer4WakeSrc(VOID)
{
}

/*****************************************************************************
 Function   : BSP_PWC_DelTimer4WakeSrc
 Description: clear mask 
 Input      :  
            : 
 Return     : void
 Other      : caution: BIT_N must be used befor call the function
*****************************************************************************/
VOID BSP_PWC_DelTimer4WakeSrc(VOID)
{
}

#endif

EXPORT_SYMBOL(BSP_PWC_SetTimer4WakeSrc);
EXPORT_SYMBOL(BSP_PWC_DelTimer4WakeSrc);

#if  ((defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)) && !defined (PRODUCT_CFG_BUILD_SEPARATE) \
		 && (defined (BOARD_ASIC)||defined (BOARD_ASIC_BIGPACK)))
	late_initcall(PWRCTRL_CommonInitial);
#elif (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS))

#else
late_initcall(PWRCTRL_CommonInitial);
#endif
#endif

