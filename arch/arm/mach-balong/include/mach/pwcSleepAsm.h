
#define _ARM_FUNCTION_CALLED_FROM_C(a) \
    .globl a ;\
    .code  32 ;\
    .balign 4 ;\
a:




#define SARM1_BASE          (0x2FFE0000)
#define PWR_SC_PHY_BASE 	(0x90000000)
#define DSRAM1_STORE_BASE	 (SARM1_BASE + 0x2000)
#define DSRAM1_STORE_ARM_REG_BASE	 (SARM1_BASE + 0x2150)
#define DSRAM1_STORE_GIC_BASE  (SARM1_BASE + 0x2200)
#define DSRAM1_STORE_CTRL_REG_BASE (SARM1_BASE + 0x2400)
 
#define ACPU_GIC_PHY_BASE 		(0X04001000)
#define ACPU_IPC_INT_PHY_BASE    (0x900A5410)

#define DM_ACC_UNCTRL       0xFFFFFFFF




//#define PBXA9_GIC_CPU_CONTROL_VA IO_ADDRESS(PBXA9_GIC_CPU_CONTROL)





#define ACPU_GIC_ICCICR (PBXA9_GIC_CPU_CONTROL + 0x000)
#define ACPU_GIC_ICCPMR (PBXA9_GIC_CPU_CONTROL + 0x004)
#define ACPU_GIC_ICCBPR (PBXA9_GIC_CPU_CONTROL + 0x008)
#define ACPU_GIC_ICCEOIR (PBXA9_GIC_CPU_CONTROL + 0x010)
#define ACPU_GIC_ICABPR (PBXA9_GIC_CPU_CONTROL + 0x01C)

#define ACPU_GIC_ICDDCR (PBXA9_GIC_DIST_CONTROL + 0x000)
#define ACPU_GIC_ICDISR (PBXA9_GIC_DIST_CONTROL + 0x080)
#define ACPU_GIC_ICDISER (PBXA9_GIC_DIST_CONTROL + 0x100)
#define ACPU_GIC_ICDICER (PBXA9_GIC_DIST_CONTROL + 0x180)
#define ACPU_GIC_ICDISPR (PBXA9_GIC_DIST_CONTROL + 0x200)
#define ACPU_GIC_ICDICPR (PBXA9_GIC_DIST_CONTROL + 0x280)
#define ACPU_GIC_ICDIPR (PBXA9_GIC_DIST_CONTROL + 0x400)
#define ACPU_GIC_ICDICFR (PBXA9_GIC_DIST_CONTROL + 0xC00)
#define ACPU_GIC_ICDSGIR (PBXA9_GIC_DIST_CONTROL + 0xF00)









#define ISRAM1_VA_SLEEP (ISRAM1_VA_BASE + 0x400) 


#define PBXA9_GIC_MASK	(0xFFFFFFFF)
#define IPC_INT_APP_MODEM		  (2)
#define PWC_SC_ACPU_MDDRC_ACLCK   (0X1<<25)
#define PWC_SC_ACPU_AXILB_CLCK   (0X1<<24)
#define PWC_SC_ACPU_DDR2X_PCLCK  (0X1<<21)
#define PWC_SC_ACPU_MDDRC_PCLCK  (0X1<<22)
#define ACPU_IMSAT_MASK     	(0x1)
//#define MODE_MASK 				 (0x1F)
#define MODE_SVC32 (0X13)
#define MODE_ABORT32 (0X17)
#define MODE_UNDEF32 (0X1B)
#define MODE_IRQ32   (0X12)
#define MODE_FIQ32  (0X11)
#define MODE_USER32 (0X10)
#define MODE_SYSTEM32 (0x1F)
#define I_BIT  (0x1<<7)
#define F_BIT  (0x1<<6)

#define MMUCR_W_ENABLE	(0x1<<3)
#define MMUCR_PROG32	(0x1<<4)
#define MMUCR_DATA32	(0x1<<5)
#define MMUCR_L_ENABLE	(0x1<<6)

#define MMUCR_M_ENABLE	(0x1<<0)
#define MMUCR_C_ENABLE	(0x1<<2)



#define PMD_SECT_ARM926_1       PMD_TYPE_SECT | PMD_SECT_BUFFERABLE
#define PMD_SECT_ARM926_2       PMD_SECT_CACHEABLE | PMD_BIT4
#define PMD_SECT_ARM926_3       PMD_SECT_AP_WRITE | PMD_SECT_AP_READ

#define PMD_SECT_ARM926_4       PMD_SECT_ARM926_1 | PMD_SECT_ARM926_2
#define PMD_SECT_ARM926         PMD_SECT_ARM926_3 | PMD_SECT_ARM926_4

/*modefied by zsc */
#define PMD_SECT_ARMA9        	PMD_TYPE_SECT | PMD_SECT_BUFFERABLE \
								| PMD_SECT_CACHEABLE|PMD_SECT_AP_WRITE| PMD_SECT_AP_READ


/*first level section address mask*/
#define SEC_ADDR_MASK       0xFFF00000     /*bit 31 ~ 20 mask*/

/*TTB register transfer bit0~13 MASK*/
#define TRANS_BASE_MASK     0x3FFF

/* cpsr control bits */
#define CTRL_MMU_BIT            0x01
#define CTRL_ICACHE_BIT         1<<12
#define CTRL_DCACHE_BIT         1<<2
#define CTRL_CACHE_BIT          CTRL_DCACHE_BIT | CTRL_ICACHE_BIT
#define CTRL_MMU_CACHE          CTRL_MMU_BIT | CTRL_CACHE_BIT

