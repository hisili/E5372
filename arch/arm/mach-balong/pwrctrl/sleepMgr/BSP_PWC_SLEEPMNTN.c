/*******************************************************************************
* Copyright (C), 2010-2011, HUAWEI Tech. Co., Ltd
*
* File name:
*                pwcSleepMntn.c
* Description:
*                sleep managerment
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
/*V3R2 CS f00164371*/
#include "generated/FeatureConfigDRV.h"

#if (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS))
#include <linux/linkage.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/interrupt.h>

#include <mach/balong_v100r001.h>
#if (defined(CHIP_BB_6920ES)||defined (CHIP_BB_6920CS))
#include <mach/pwrctrl/v7r1/BSP_PWC_SOCREG.h>
#else
#include <mach/pwrctrl/v3r2/BSP_PWC_SOCREG.h>
#endif
#include <mach/pwrctrl/BSP_PWC_SLEEPMNTN.h>



void PWRCTRL_MntnFunc(void)
{

}

#endif

