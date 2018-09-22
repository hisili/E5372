/******************************************************************
 * Copyright (C), 2005-2011, HUAWEI Tech. Co., Ltd.
 *
 * File name:  pwcSleepAsm.h
 *
 * Description:
 *     power ctrl  head file.
 *
 * Author:     l56193
 * Date:       2011.05.01
 * Version:    v1.0
 *
 *
 * History:
 * Author:     l56193
 * Date:       2011.05.01
 * Discribe:   Initial
 *******************************************************************/
#ifndef PWRCTRL_ACPU_SLEEP_ASM
#define PWRCTRL_ACPU_SLEEP_ASM

/*****************************************************************************/
/*include headfile */
/*****************************************************************************/
//#include <mach/DrvInterface.h>

#if (defined(CHIP_BB_6920ES)||defined (CHIP_BB_6920CS))
#include <mach/pwrctrl/v7r1/BSP_PWC_SOCREG.h>
#else
#include <mach/pwrctrl/v3r2/BSP_PWC_SOCREG.h>
#endif
/*****************************************************************************/
/*macro define*/
/*****************************************************************************/
#define _ARM_FUNCTION_CALLED_FROM_C(a) \
    .globl a    ;\
    .code   32  ;\
    .balign 4   ;\
a:


/*****************************************************************************/
/*define*/
/*****************************************************************************/


/* CPSR config bit . */;
#ifndef I_BIT
#define I_BIT       (1 << 7)
#endif
#ifndef F_BIT
#define F_BIT       (1 << 6)
#endif

#define PWRCTRL_TIMER_COUNT
#define PWRCTRL_DEBUG_FLAG

//#define PWRCTRL_DEEPSLEEP_USB
/* mode bits */
#define MODE_SYSTEM32   0x1F
#define MODE_UNDEF32    0x1B
#define MODE_ABORT32    0x17
#define MODE_SVC32      0x13
#define MODE_IRQ32      0x12
#define MODE_FIQ32      0x11
#define MODE_USER32     0x10
//#define MODE_MASK       0x1F //clean warning

/* masks for getting bits from PSR */
#ifndef MASK_MODE
#define MASK_MODE       0x0000003F
#endif

#define PWR_ASM_CODE_COPY_OFFSET 0x200

#define PWRCTRL_DRX_LOG_WAKE_TIME_REC   (0xDEB8)
#define PWRCTRL_DRX_TCM_ON              (0xDEB9)
#define PWRCTRL_DEEPSLEEP_DEAD_LOOP     (0xDEB)

#define PWR_BOOT_NORMAL_ADDR    (PRODUCT_CFG_MCORE_RAM_LOW_ADDR)        

/*define end flag for code-stable*/
#define PWRCTRL_ASM_LINK_FLAG1 (0xAABBCCDD)
#define PWRCTRL_ASM_LINK_FLAG2 (0xDDCCBBAA)

/*双核唤醒源，暂做成互斥，后续修改*/
#define CARM_AWAKE_INT_ENABLE0  (0x80000050)          
#define CARM_AWAKE_INT_ENABLE1  (0x1020000)  
#define CARM_AWAKE_INT_ENABLE2  (0x3f0000)  
#define CARM_AWAKE_INT_ENABLE3  (0x1001008) 
#define AARM_AWAKE_INT_ENABLE0  (0x50)          
#define AARM_AWAKE_INT_ENABLE2  (0x3f0000)  
#define AARM_AWAKE_INT_ENABLE3  (0x8) 

/*DEBUG标志位*/
#define PWRCTRL_BAK_GIC_REG        0x1
#define PWRCTRL_BAK_CO_REG         0x2
#define PWRCTRL_BAK_CO_REG_END     0x3
#define PWRCTRL_ENTER_WFI           0x4
#define PWRCTRL_WAKEUP              0x5
#define PWRCTRL_RESTORE_CO_REG      0X6
#define PWRCTRL_RESTORE_GIC_REG     0X7
#define PWRCTRL_RESTORE_GIC_REG_END 0X8
#define PWRCTRL_PWRDOWNP              0x9
#define PWRCTRL_CLOSEGIC                0xa
#define PWRCTRL_STORE_IPREG         0xb
#define PWRCTRL_SET_WAKEUP_INT         0xc
#define PWRCTRL_WAKEUP_DS         0xd
#define PWRCTRL_DS2SLOW         0xe
#define PWRCTRL_SLOW2NORMAL 0xf
#define PWRCTRL_ENTER_NORMAL    0x10
#define PWRCTRL_DDR_INIT    0x11
#define PWRCTRL_OUT_SRAM 0x12

/* cpsr control bits */
#define CTRL_MMU_BIT            (0x01)
#define CTRL_ICACHE_BIT         (1<<12)
#define CTRL_DCACHE_BIT         (1<<2)
#define CTRL_CACHE_BIT          CTRL_DCACHE_BIT | CTRL_ICACHE_BIT
#define CTRL_MMU_CACHE          CTRL_MMU_BIT | CTRL_CACHE_BIT

/*first level section address mask*/
#define SEC_ADDR_MASK           (0xFFF00000)     /*bit 31 ~ 20 mask*/

/*modefied by zsc */
#define PMD_SECT_ARMA9        	PMD_TYPE_SECT | PMD_SECT_BUFFERABLE \
								| PMD_SECT_CACHEABLE|PMD_SECT_AP_WRITE| PMD_SECT_AP_READ

/*TTB register transfer bit0~13 MASK*/
#define TRANS_BASE_MASK         (0x3FFF)

#define DM_ACC_UNCTRL               (0xFFFFFFFF)

/*****************************************************************************/
/*variable prototype*/
/*****************************************************************************/
#define AXI_VIRT_TO_PHY(var)  (((var) - AXI_MEM_ADDR) + AXI_MEM_BASE_ADDR)
#define SYS_REG_VIRT_TO_PHY(var)  (((var) - V7R1_SC_VA_BASE) + INTEGRATOR_SC_BASE)

/*****************************************************************************/
/*function prototype*/
/*****************************************************************************/

#endif
