/*******************************************************************************
* Copyright (C), 2010-2011, HUAWEI Tech. Co., Ltd
*
* File name:
*                pwcSleepMntn.h
* Description:
*                
*
* Author:        ¡ı”¿∏ª
* Date:          2011-09-20
* Version:       1.0
*
*
*
* History:
* Author:		¡ı”¿∏ª
* Date:			2011-09-20
* Description:	Initial version
*
*******************************************************************************/

#ifndef PWRCTRL_ACPU_SLEEP_MNTN
#define PWRCTRL_ACPU_SLEEP_MNTN
#include <mach/balong_v100r001.h>
#include "BSP.h"

#if (defined(CHIP_BB_6920ES)||defined (CHIP_BB_6920CS))
#include <mach/pwrctrl/v7r1/BSP_PWC_SOCREG.h>
#else
#include <mach/pwrctrl/v3r2/BSP_PWC_SOCREG.h>
#endif
typedef struct
{
    BSP_U32 u32DsInTimes;
    BSP_U32 u32DsOutTimes;
    BSP_U32 u32LsInTimes;
    BSP_U32 u32WfiInTimes;
    BSP_U32 u32AcpuPWDTimes;
    BSP_U32 u32AcpuPWUpTimes;
    BSP_U32 u32AcpuPWUpIntTimes;
} PWC_SLEEP_COUNT;

#endif

