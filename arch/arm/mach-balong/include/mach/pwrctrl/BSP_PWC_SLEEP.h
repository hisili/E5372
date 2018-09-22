/*******************************************************************************
* Copyright (C), 2010-2011, HUAWEI Tech. Co., Ltd
*
* File name:
*                pwcSlee.h
* Description:
*                sleep managerment
*
* Author:        张世楚
* Date:          2011-09-20
* Version:       1.0
*
*
*
* History:
* Author:		张世楚
* Date:			2011-09-20
* Description:	Initial version
* Author:		刘永富
* Date:			2011-09-28
* Description:	修改文件结构
* Author:		王静
* Date:			2011-12-19
* Description:	增加V7R1接口
*******************************************************************************/
#ifndef PWRCTRL_ACPU_SLEEP
#define PWRCTRL_ACPU_SLEEP
    
#include "BSP.h"

#if (defined(CHIP_BB_6920ES)||defined (CHIP_BB_6920CS))
#include <mach/pwrctrl/v7r1/BSP_PWC_SOCREG.h>
#else
#include <mach/pwrctrl/v3r2/BSP_PWC_SOCREG.h>
#endif
#include <mach/pwrctrl/BSP_PWC_SYSFUNC.h>
#define DEEPSLEEP_TYPE_CCPU             (0)
#define DEEPSLEEP_TYPE_TOGETHER         (1)

#define DEEPSLEEP_TYPE_NORMAL           (0)
#define DEEPSLEEP_TYPE_USB  	        (1)
#define PWR_SC_PERIPH_STAT0_DEBUG       (0x3<<18)
#define PWR_SC_AARM_WKUP_INT_MASK       (0x3FFFF)
#define PWC_ACPU_DEEPSLEEP_SZ 	        (0x2000)

#define PWC_SC_ACPU_ZDMAC_CLCK	        (0x1<<30)
#define PWC_SC_ACPU_ZSP_AHB_CLCK        (0x1<<29)
#define PWC_SC_ACPU_ADMAC_CLCK          (0x1<<28)
#define PWC_SC_ACPU_EDMAC_ACLCK1        (0x1<<27)
#define PWC_SC_ACPU_EDMAC_ACLCK0        (0x1<<26)
#define PWC_SC_ACPU_MDDRC_ACLCK         (0x1<<25)
#define PWC_SC_ACPU_AXILB_CLCK          (0x1<<24)
#define PWC_SC_ACPU_AXIPB_CLCK          (0x1<<23)
#define PWC_SC_ACPU_MDDRC_PCLCK         (0x1<<22)
#define PWC_SC_ACPU_DDR2X_PCLCK         (0x1<<21)

#define CACHE_DC_ENABLE             (0x1<<2) 
#define CACHE_IC_ENABLE             (0x1<<12) 


#define PWR_SC_ACPU_SET_REG_MASK    (0X7FE00000)


#define DSRAM1_VA_BASE 				(ISRAM1_VA_BASE + 0x2000)
#define DSARM1_ACPU_SET_REG_SAVE    (DSRAM1_VA_BASE + 0x04)
#define AARM_WKUP_SOURCE            (0xFFFFFFFF)

#if (defined(CHIP_BB_6920ES)||defined (CHIP_BB_6920CS))
#define PWR_CLK_GATE_MASK1               0x15087a01
#define PWR_CLK_GATE_MASK2               0x7FFE3B
#define PWC_CIPHER_REGBASE_ADDR		IO_ADDRESS(0x900AB000)
#define PWC_IPF_REGBASE_ADR			IO_ADDRESS(0x900AC000)
#define PWC_SOCP_REG_BASEADDR		IO_ADDRESS(0x900A9000)

#define PWR_ACPU_POWER_DOWN_MASK        (0X1<<15)
#define PWR_ACPU_RST_MASK               (0X1<<16)
#define PWR_ACPU_ISO_MASK               (0X1<<20)
#define PWR_ACPU_MTCMOS_MASK            (0X1<<20)
#define ACPU_POWER_STAT_MASK            (0x1 << 20)
#define PWR_ACPU_WFI_EN                 (0x1 << 19)
#define PWR_ACPU_WFI_STAT               (0x3 << 12)
#define PWR_CARM_POWER_DOWN_MASK        (0x01 << 10)
#define PWR_PERI_POWER_DOWN_MASK        (0x01 << 29)
#define PWR_PERI_CLK_MASK        		(0x01 << 30)

/*GIC*/
#define CARM_GIC_ICDABR_NUM                 5

#endif
extern void PWRCTRL_SleepInitial(void);
extern void PWRCTRL_AsmWfi(void);
extern void PWRCTRL_AsmDeepSleep(void);

extern void PWRCTRL_AsmLightSleep(void);

extern void PWRCTRL_AsmCodeBegin(void);

extern BSP_S32  PWRCTRL_DeepSleep(BSP_U32 u32sleepmode);

extern unsigned long PWRCTRL_HisiBalongIoRemap(unsigned long phys_addr, size_t size);

extern void PWRCTRL_HisiBalongIoUnmap(unsigned long virt_addr);
extern void PWRCTRL_LightSleep(void);
extern void PWRCTRL_ArmSleep(void);    



#endif

