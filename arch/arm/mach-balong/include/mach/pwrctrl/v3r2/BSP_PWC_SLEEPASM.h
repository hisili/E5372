/*******************************************************************************
* Copyright (C), 2010-2011, HUAWEI Tech. Co., Ltd
*
* File name:
*                pwcSleepAsm.s
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
*******************************************************************************/
#ifndef PWRCTRL_ACPU_SLEEP_ASM
#define PWRCTRL_ACPU_SLEEP_ASM


#define _ARM_FUNCTION_CALLED_FROM_C(a) \
    .globl a ;\
    .code  32 ;\
    .balign 4 ;\
a:







#define DM_ACC_UNCTRL               (0xFFFFFFFF)

#define PBXA9_GIC_MASK	            (0xFFFFFFFF)

#define IPC_INT_ARM_SLEEP		    (8)

#define PWC_SC_ACPU_MDDRC_ACLCK     (0x1<<25)
#define PWC_SC_ACPU_AXILB_CLCK      (0x1<<24)
#define PWC_SC_ACPU_DDR2X_PCLCK     (0x1<<21)
#define PWC_SC_ACPU_MDDRC_PCLCK     (0x1<<22)
#define ACPU_IMSAT_MASK     	    (0x1)

#ifndef I_BIT
#define I_BIT           (1 << 7)
#endif

#ifndef F_BIT
#define F_BIT           (0x1<<6)
#endif

/*#define MODE_MASK 				 (0x1F)*/
#define MODE_SVC32              (0X13)
#define MODE_ABORT32            (0X17)
#define MODE_UNDEF32            (0X1B)
#define MODE_IRQ32              (0X12)
#define MODE_FIQ32              (0X11)
#define MODE_USER32             (0X10)
#define MODE_SYSTEM32           (0x1F)

#define MMUCR_W_ENABLE	        (0x1<<3)
#define MMUCR_PROG32	        (0x1<<4)
#define MMUCR_DATA32	        (0x1<<5)
#define MMUCR_L_ENABLE	        (0x1<<6)

#define MMUCR_M_ENABLE	        (0x1<<0)
#define MMUCR_C_ENABLE	        (0x1<<2)



#define PMD_SECT_ARM926_1       PMD_TYPE_SECT | PMD_SECT_BUFFERABLE
#define PMD_SECT_ARM926_2       PMD_SECT_CACHEABLE | PMD_BIT4
#define PMD_SECT_ARM926_3       PMD_SECT_AP_WRITE | PMD_SECT_AP_READ

#define PMD_SECT_ARM926_4       PMD_SECT_ARM926_1 | PMD_SECT_ARM926_2
#define PMD_SECT_ARM926         PMD_SECT_ARM926_3 | PMD_SECT_ARM926_4

/*modefied by zsc */
#define PMD_SECT_ARMA9        	PMD_TYPE_SECT | PMD_SECT_BUFFERABLE \
								| PMD_SECT_CACHEABLE|PMD_SECT_AP_WRITE| PMD_SECT_AP_READ


/*first level section address mask*/
#define SEC_ADDR_MASK           (0xFFF00000)     /*bit 31 ~ 20 mask*/

/*TTB register transfer bit0~13 MASK*/
#define TRANS_BASE_MASK         (0x3FFF)

/* cpsr control bits */
#define CTRL_MMU_BIT            (0x01)
#define CTRL_ICACHE_BIT         (1<<12)
#define CTRL_DCACHE_BIT         (1<<2)
#define CTRL_CACHE_BIT          CTRL_DCACHE_BIT | CTRL_ICACHE_BIT
#define CTRL_MMU_CACHE          CTRL_MMU_BIT | CTRL_CACHE_BIT


#endif
