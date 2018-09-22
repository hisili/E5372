/*******************************************************************************
* Copyright (C), 2010-2011, HUAWEI Tech. Co., Ltd
*
* File name:
*                pwcSleepTest.c
* Description:
*                sleep managerment
*
* Author:        ¡ı”¿∏ª
* Date:          2011-09-28
* Version:       1.0
*
*
*
* History:
* Author:		¡ı”¿∏ª
* Date:			2011-09-28
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
#include <mach/pwrctrl/BSP_PWC_SLEEPTST.h>
#include <mach/pwrctrl/BSP_PWC_SLEEP.h>
#include <mach/pwrctrl/BSP_PWC_SLEEPMGR.h>



extern unsigned int g_ulAcpuSleepVoteMap;


unsigned int g_acpu_vote_set(unsigned int x)
{
    g_ulAcpuSleepVoteMap = x;
    return g_ulAcpuSleepVoteMap;
}

#if 0
static irqreturn_t balong_timer0_0_irq_handler(int irq, void *dev_id)
{
#if defined (BOARD_SFT) && defined (VERSION_V3R2)

    unsigned int readValue;
    readValue = readl(__io_address(PV500V1_TIMER0_BASE) + CLK_REGOFF_CLEAR);

    writel('a', VIRT_PV500V1_UART0_BASE);
    writel('\n', VIRT_PV500V1_UART0_BASE);
#endif
	return IRQ_HANDLED;//


    
}

void  PWRCTRL_Timer0Test(int intNumberPerSecond)
{
#if defined (BOARD_SFT) && defined (VERSION_V3R2)


    unsigned int tc;
    unsigned int ret=0;
    #if 0
    *(volatile int*)0xf1001824 = 0x02010101;
    #endif
    writel(CLK_DEF_DISABLE, __io_address(PV500V1_TIMER0_BASE) + CLK_REGOFF_CTRL);
    tc = (32768 * intNumberPerSecond)/1000;
    writel(tc, __io_address(PV500V1_TIMER0_BASE) + CLK_REGOFF_LOAD);
    writel(CLK_DEF_ENABLE, __io_address(PV500V1_TIMER0_BASE) + CLK_REGOFF_CTRL);

    
    writel(CLK_DEF_DISABLE, __io_address(PV500V1_TIMER0_BASE) + CLK_REGOFF_CTRL);
    writel(CLK_DEF_ENABLE, __io_address(PV500V1_TIMER0_BASE) + CLK_REGOFF_CTRL);
    

	ret = request_irq(77, balong_timer0_0_irq_handler, 0, "timer0_0", NULL);
	if (ret)
	{
            //writel('e', VIRT_PV500V1_UART0_BASE);
		return;
	}

#endif
}
#endif
#endif





