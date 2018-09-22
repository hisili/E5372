#define  ERROR    (-1)
#define DEEPSLEEP_TYPE_NORMAL   (0)
#define DEEPSLEEP_TYPE_USB  	(1)
#define PWR_SC_PERIPH_STAT0_DEBUG (0X3<<18)
#define PWR_SC_AARM_WKUP_INT_MASK (0X3FFFF)
#define PWC_ACPU_DEEPSLEEP_SZ 	  (0X2000)

#define PWC_SC_ACPU_ZDMAC_CLCK	  (0X1<<30)
#define PWC_SC_ACPU_ZSP_AHB_CLCK  (0X1<<29)
#define PWC_SC_ACPU_ADMAC_CLCK    (0X1<<28)
#define PWC_SC_ACPU_EDMAC_ACLCK1  (0X1<<27)
#define PWC_SC_ACPU_EDMAC_ACLCK0  (0X1<<26)
#define PWC_SC_ACPU_MDDRC_ACLCK   (0X1<<25)
#define PWC_SC_ACPU_AXILB_CLCK   (0X1<<24)
#define PWC_SC_ACPU_AXIPB_CLCK   (0X1<<23)
#define PWC_SC_ACPU_MDDRC_PCLCK  (0X1<<22)
#define PWC_SC_ACPU_DDR2X_PCLCK  (0X1<<21)

#define CACHE_DC_ENABLE   (0x1<<2) 
#define CACHE_IC_ENABLE   (0x1<<12) 

#define SRAM1_BASE			(0x2FFE0000)
#define SRAM1_SIZE			(0x4000)

#define PWR_SC_ACPU_SET_REG_MASK (0X7FE00000)

#define ISRAM1_VA_SLEEP (ISRAM1_VA_BASE + 0x400) 

#define DSRAM1_VA_BASE 				(ISRAM1_VA_BASE + 0x2000)
#define DSARM1_ACPU_SET_REG_SAVE (DSRAM1_VA_BASE + 0x04)


#define PWR_SC_PHY_BASE 	(0x90000000)

//#define PBXA9_GIC_CPU_CONTROL_VA IO_ADDRESS(PBXA9_GIC_CPU_CONTROL)
//#define PBXA9_GIC_CPU_CLEAR_INT_VA  (PBXA9_GIC_CPU_CONTROL_VA +0x80)

#define PWR_SC_PERIPH_STAT0   	( V3R2_SC_VA_BASE+ 0X108)
#define PWR_SC_AARM_WKUP_INT_EN ( V3R2_SC_VA_BASE+ 0X0B4)
#define PWR_SC_PERIPH_CLKSTAT3  (  V3R2_SC_VA_BASE+0X080)
#define PWR_SC_PERIPH_CLKDIS3	( V3R2_SC_VA_BASE+0X07C)

typedef unsigned int  UINT32;
#define ClearBitReg32(reg,value) ((*((volatile UINT32 *)(reg))) = (*(volatile UINT32 *)(reg))& ~(UINT32)(value))
#define SetBitReg32(reg,value)  ((*((volatile UINT32 *)(reg))) = (*(volatile UINT32 *)(reg))|(UINT32)(value))
#define REG_READ_32(reg,result)   ((result) = (*(volatile UINT32 *)(reg)))
#define REG_WRITE_32(reg,value)		((*(volatile UINT32 *)(reg)) =(value))


extern void pwrctrl_asm_acpu_deepsleep(void);

extern void pwrctrl_asm_code_begin(void);

extern void pwrctrl_acpu_deepsleep(int sleepmode);

extern unsigned long hisibalong_ioremap(unsigned long phys_addr, size_t size);

extern void hisibalong_iounmap(unsigned long virt_addr);


