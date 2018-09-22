/******************************************************************
 * Copyright (C), 2005-2011, HUAWEI Tech. Co., Ltd.
 *
 * File name: BSP_PWC_SOCREG.h
 *
 * Description:
 *     power ctrl common head file.
 *
 * Date:       2011.11.25
 * Version:    v1.0
 *
 *
 * History:
 * Date:       2011.11.25
 * Discribe:   Initial
 *******************************************************************/
#ifndef PWRCTRL_ACPU_SOC_REG_H
#define PWRCTRL_ACPU_SOC_REG_H

//#include <mach/DrvInterface.h>
#include <mach/balong_v100r001.h>
#include <mach/common/bsp_memory.h>


#define PWR_SYSCTRL_BASE                (IO_ADDRESS(0x90000000))//(IO_ADDRESS(INTEGRATOR_SC_BASE))
#ifdef PRODUCT_CFG_CORE_TYPE_MODEM
#define ISRAM1_VA_SLEEP                (MEMORY_AXI_DRXM_ADDR)
#else
#define ISRAM1_VA_SLEEP                (MEMORY_AXI_DRXA_ADDR)
#endif
#define DSARM0_BASE_ADDR                (ISRAM1_VA_SLEEP+0x2000) 
#define DSRAM0_DEEPSLEEP_ADDR           (DSARM0_BASE_ADDR + 0x200)
#define ISRAM0_DEEPSLEEP_ADDR        (ISRAM1_VA_SLEEP)

#define STORE_ADDR_DDRC                 (DSRAM0_DEEPSLEEP_ADDR)
#define STORE_ADDR_ARM_REG_SLOW         (DSRAM0_DEEPSLEEP_ADDR + 0x100)
#define STORE_ADDR_ARM_REG              (DSRAM0_DEEPSLEEP_ADDR + 0x150)

#define USB_LINK_FLAG                   (DSRAM0_DEEPSLEEP_ADDR + 0x220)//已不用
#define STORE_ADDR_PERSTAT0             (DSRAM0_DEEPSLEEP_ADDR + 0x224)
#define STORE_ADDR_PERSTAT1             (DSRAM0_DEEPSLEEP_ADDR + 0x228)
#define STORE_ADDR_PERSTAT2             (DSRAM0_DEEPSLEEP_ADDR + 0x22c)
#define STORE_ADDR_PERSTAT3             (DSRAM0_DEEPSLEEP_ADDR + 0x230)

#define DRX_SLOW_RESTORE_FLAG           (DSRAM0_DEEPSLEEP_ADDR + 0x238)
#define DRX_SLOW_GBBP_PWR_FLAG          (DSRAM0_DEEPSLEEP_ADDR + 0x23C)
#define DRX_SLOW_WBBP_PWR_FLAG          (DSRAM0_DEEPSLEEP_ADDR + 0x240)

#define DRX_SLOW_LBBP_PWR_FLAG          (DSRAM0_DEEPSLEEP_ADDR + 0x244)

#define STORE_ADDR_GIC                  (DSRAM0_DEEPSLEEP_ADDR + 0x248)   /*偏移从0x248到0x680用来保存GIC寄存器*/
#define STORE_ADDR_GIC_ENABLE           (DSRAM0_DEEPSLEEP_ADDR + 0x600)
#define PWRCTRL_ASM_DEG_FLAG            (DSRAM0_DEEPSLEEP_ADDR + 0x6a0)
#define PWRCTRL_ASM_DEG_FLAG1            (DSRAM0_DEEPSLEEP_ADDR + 0x6a4)
#define STORE_ADDR_ARM_REG_SLEEP        (DSRAM0_DEEPSLEEP_ADDR + 0x6ac)

#define STORE_ADDR_MMU_REG              (DSRAM0_DEEPSLEEP_ADDR + 0x700)
#define STORE_RESTORE_TIMER0            (DSRAM0_DEEPSLEEP_ADDR + 0x750)
#define STORE_RESTORE_TIMER1           (DSRAM0_DEEPSLEEP_ADDR + 0x754)
#define STORE_RESTORE_TIMER2           (DSRAM0_DEEPSLEEP_ADDR + 0x758)
#define STORE_RESTORE_TIMER3           (DSRAM0_DEEPSLEEP_ADDR + 0x75c)
#define STORE_RESTORE_TIMER4           (DSRAM0_DEEPSLEEP_ADDR + 0x760)
#define STORE_RESTORE_TIMER5           (DSRAM0_DEEPSLEEP_ADDR + 0x764)
#define STORE_RESTORE_TIMER6           (DSRAM0_DEEPSLEEP_ADDR + 0x768)
#define STORE_RESTORE_TIMER7         (DSRAM0_DEEPSLEEP_ADDR + 0x76c)

#define STORE_BAK_TIMER0            (DSRAM0_DEEPSLEEP_ADDR + 0x770)
#define STORE_BAK_TIMER1           (DSRAM0_DEEPSLEEP_ADDR + 0x774)
#define STORE_BAK_TIMER2           (DSRAM0_DEEPSLEEP_ADDR + 0x778)
#define STORE_BAK_TIMER3           (DSRAM0_DEEPSLEEP_ADDR + 0x77c)
#define STORE_BAK_TIMER4           (DSRAM0_DEEPSLEEP_ADDR + 0x780)
#define STORE_BAK_TIMER5           (DSRAM0_DEEPSLEEP_ADDR + 0x784)
#define STORE_BAK_TIMER6           (DSRAM0_DEEPSLEEP_ADDR + 0x788)
#define STORE_BAK_TIMER7         	(DSRAM0_DEEPSLEEP_ADDR + 0x78c)

#define STORE_PERI_PD_FLAG			(DSRAM0_DEEPSLEEP_ADDR + 0x790)
#define STORE_REMAP_ADDR1           (DSRAM0_DEEPSLEEP_ADDR + 0x794)
#define STORE_REMAP_ADDR2           (DSRAM0_DEEPSLEEP_ADDR + 0x798)
#define STORE_REMAP_ADDR3           (DSRAM0_DEEPSLEEP_ADDR + 0x79C)
#define STORE_REMAP_ADDR4           (DSRAM0_DEEPSLEEP_ADDR + 0x7A0)


#define STORE_CTRL_REG_BASE          (DSRAM0_DEEPSLEEP_ADDR + 0x800)

#define PWRCTRL_ASM_SP_BEGIN            (ISRAM1_VA_SLEEP +0x3000)
#define PWRCTRL_ASM_SP_END              (ISRAM1_VA_SLEEP +0x3000)


/*IPC地址*/
#define PWRCTRL_IPC_INTSEND             (IO_ADDRESS(0x900a5410))

/*GIC基址*/
#ifdef PRODUCT_CFG_CORE_TYPE_MODEM
#define CARM_GIC_CPU_BASE 	             (0x8000100)
#define CARM_GIC_DIST_BASE              (0x8001000)
#else
#define CARM_GIC_CPU_BASE 	            (IO_ADDRESS(0x4000100))
#define CARM_GIC_DIST_BASE              (IO_ADDRESS(0x4001000))
#endif

/* GIC */
#define CARM_GIC_CPUICR                 (CARM_GIC_CPU_BASE + 0x000)
#define CARM_GIC_CPUPMR                 (CARM_GIC_CPU_BASE + 0x004)
#define CARM_GIC_CPUBPR                 (CARM_GIC_CPU_BASE + 0x008)
#define CARM_GIC_CPUIACK                (CARM_GIC_CPU_BASE + 0x00C)
#define CARM_GIC_CPUEOIR                (CARM_GIC_CPU_BASE + 0x010)
#define CARM_GIC_CPURPR                 (CARM_GIC_CPU_BASE + 0x014)
#define CARM_GIC_CPUHPIR                (CARM_GIC_CPU_BASE + 0x018)
#define CARM_GIC_ICABPR					(CARM_GIC_CPU_BASE + 0x01C)

#define CARM_GIC_ICDDCR                 (CARM_GIC_DIST_BASE + 0x000)
#define CARM_GIC_ICDISR					(CARM_GIC_DIST_BASE + 0x080)
#define CARM_GIC_ICDISER                (CARM_GIC_DIST_BASE + 0x100)
#define CARM_GIC_ICDICER                (CARM_GIC_DIST_BASE + 0x180)
#define CARM_GIC_ICDISR1                (CARM_GIC_DIST_BASE + 0x200)
#define CARM_GIC_ICDICPR                (CARM_GIC_DIST_BASE + 0x280)
#define CARM_GIC_ICDISPR                (CARM_GIC_DIST_BASE + 0x300)

#define CARM_GIC_ICDIPR                 (CARM_GIC_DIST_BASE + 0x400)
#define CARM_GIC_ICDTARG                (CARM_GIC_DIST_BASE + 0x800)
#define CARM_GIC_ICDICFR                (CARM_GIC_DIST_BASE + 0xC00)
#define CARM_GIC_ICDSGIR                (CARM_GIC_DIST_BASE + 0xF00)

#define CARM_GIC_ICDISPR_OFFSET(x)      (CARM_GIC_ICDISPR  + ((x)*0x4))
/*系统控制器地址*/
#define PWR_SC_PERIPH_CLKEN0            (PWR_SYSCTRL_BASE + 0x00)
#define PWR_SC_PERIPH_CLKDIS0           (PWR_SYSCTRL_BASE + 0x04)
#define PWR_SC_SCPERSTAT0               (PWR_SYSCTRL_BASE + 0x08)
#define PWR_SC_PERIPH_CLKEN1            (PWR_SYSCTRL_BASE + 0x0C)
#define PWR_SC_PERIPH_CLKDIS1           (PWR_SYSCTRL_BASE + 0x10)
#define PWR_SC_SCPERSTAT1               (PWR_SYSCTRL_BASE + 0x14)
#define PWR_SC_PERIPH_CLKEN2            (PWR_SYSCTRL_BASE + 0x18)
#define PWR_SC_PERIPH_CLKDIS2           (PWR_SYSCTRL_BASE + 0x1C)
#define PWR_SC_SCPERSTAT2               (PWR_SYSCTRL_BASE + 0x20)
#define PWR_SC_PERIPH_CLKEN3            (PWR_SYSCTRL_BASE + 0x24)
#define PWR_SC_PERIPH_CLKDIS3           (PWR_SYSCTRL_BASE + 0x28)
#define PWR_SC_SCPERSTAT3               (PWR_SYSCTRL_BASE + 0x2C)
#define PWR_SC_RST_REQ1                 (PWR_SYSCTRL_BASE + 0x38)
#define PWR_SC_RST_REQ2			        (PWR_SYSCTRL_BASE + 0x3C)

#define PWR_SC_ARM_PLL_CTRL             (PWR_SYSCTRL_BASE + 0x58)
#define PWR_SC_DSP_DFS			        (PWR_SYSCTRL_BASE + 0x64)
#define PWR_SC_BBP1_DFS			        (PWR_SYSCTRL_BASE + 0x70)
#define PWR_SC_PERIPLL_CTRL                (PWR_SYSCTRL_BASE + 0x88)

#define PWR_SC_PERIPH_CLKEN4            (INTEGRATOR_SC_BASE + 0x9C)
#define PWR_SC_PERIPH_CLKDIS4           (INTEGRATOR_SC_BASE + 0xA0)
#define PWR_SC_SCPERSTAT4               (INTEGRATOR_SC_BASE + 0xA4)

#define PWR_SC_AARN_WARM_STAR           (PWR_SYSCTRL_BASE + 0x414)
#define PWR_SC_CARN_WARM_STAR           (PWR_SYSCTRL_BASE + 0x418)
#define PWR_SC_PERI_CLK_GATE_EN1        (PWR_SYSCTRL_BASE + 0x450)
#define PWR_SC_PERI_CLK_GATE_EN2        (PWR_SYSCTRL_BASE + 0x454)
#define PWR_SC_MEM_PWC		        (PWR_SYSCTRL_BASE + 0x4DC)

#define PWR_SC_CARM_MOD                 (PWR_SYSCTRL_BASE + 0xC00)
#define PWR_SC_CARM_IMSTAT              (PWR_SYSCTRL_BASE + 0xC04)
#define PWR_SC_TCXO                      (PWR_SYSCTRL_BASE + 0xC08)
#define PWR_SC_PW_AXIDRX                (PWR_SYSCTRL_BASE + 0xC10)
#define PWR_SC_PW_ISOEN                 (PWR_SYSCTRL_BASE + 0xC18)
#define PWR_SC_PW_MTCMOS_CTRL           (PWR_SYSCTRL_BASE + 0xC1C)
#define PWR_SC_PW_STAT0                 (PWR_SYSCTRL_BASE + 0xE04)
#define PWR_SC_PW_ISODIS                (PWR_SYSCTRL_BASE + 0xC18)
//#define PWR_SC_CARM_MOD                 (PWR_SYSCTRL_BASE + 0x418)/*C core*/
#define PWR_ARM_PWCTRL                 (PWR_SYSCTRL_BASE + 0xC20)
#define PWR_PERI_PWCTRL                 (PWR_SYSCTRL_BASE + 0xC24)
#define PWR_SC_USB_WAKEEN               (PWR_SYSCTRL_BASE + 0xC30)/*改寄存器需要修改，需确认是否存在该寄存器 0xc30*/
#define PWR_SYSCLK_DIV                              (PWR_SYSCTRL_BASE + 0xc34)
#define PWR_CARM_WKUP_INT_EN0           (PWR_SYSCTRL_BASE + 0xC3C)
#define PWR_CARM_WKUP_INT_EN1           (PWR_SYSCTRL_BASE + 0xC40)
#define PWR_CARM_WKUP_INT_EN2           (PWR_SYSCTRL_BASE + 0xC44)
#define PWR_CARM_WKUP_INT_EN3           (PWR_SYSCTRL_BASE + 0xC54)
#define PWR_AARM_WKUP_INT_EN0           (PWR_SYSCTRL_BASE + 0xC48)
#define PWR_AARM_WKUP_INT_EN1           (PWR_SYSCTRL_BASE + 0xC4C)
#define PWR_AARM_WKUP_INT_EN2           (PWR_SYSCTRL_BASE + 0xC50)
#define PWR_AARM_WKUP_INT_EN3           (PWR_SYSCTRL_BASE + 0xC58)
#define PWR_WAKEUP_INT_ENABLE           (PWR_SYSCTRL_BASE + 0xC5C)
#define PWR_SC_PWC_STAT                 (PWR_SYSCTRL_BASE + 0xE00)

#define PWR_IOS_CTRL_GPIO0              (PWR_SYSCTRL_BASE + 0x800) 
#define PWR_IOS_CTRL_GPIO1              (PWR_SYSCTRL_BASE + 0x840)
#define PWR_IOS_CTRL_GPIO2              (PWR_SYSCTRL_BASE + 0x880)
#define PWR_IOS_CTRL_GPIO3              (PWR_SYSCTRL_BASE + 0x8C0)
#define PWR_IOS_CTRL_GPIO4              (PWR_SYSCTRL_BASE + 0x9A0) 
#define PWR_IOS_CTRL_GPIO5              (PWR_SYSCTRL_BASE + 0x9e0)

/*DDR寄存器地址*/
#define PWRC_DDRC_BASE                  (IO_ADDRESS(0x90020000))//(IO_ADDRESS(INTEGRATOR_DDRC_BASE))

#define PWR_DDRC_STATUS                 (PWRC_DDRC_BASE + 0x0)
#define PWR_DDRC_SREFCTRL               (PWRC_DDRC_BASE + 0x4)
#define PWR_DDRC_CLK_SWITCH             (PWRC_DDRC_BASE + 0x20)
#define PWR_DDRC_EMRS                   (PWRC_DDRC_BASE + 0x14)
#define PWR_DDRC_RNKCFG                	(PWRC_DDRC_BASE + 0x2c)
#define PWR_DDRC_BASEADDR             	(PWRC_DDRC_BASE + 0x40)  
#define PWR_DDRC_TIMING0              	(PWRC_DDRC_BASE + 0x50)
#define PWR_DDRC_DTRCTRL               	(PWRC_DDRC_BASE + 0xac)
#define PWR_DDRC_PHYSEL          		(PWRC_DDRC_BASE + 0x430)
#define PWR_DDRC_QDLLCFG          		(PWRC_DDRC_BASE + 0x440)
#define PWR_DDRC_WRDQS_SKEW             (PWRC_DDRC_BASE + 0x500)
#define PWR_DDRC_ZQCTRL0              	(PWRC_DDRC_BASE + 0x800)
#define PWR_DDRC_ZQCTRL1              	(PWRC_DDRC_BASE + 0x804)
#define PWR_DDRC_STATUS0              	(PWRC_DDRC_BASE + 0x808)
#define PWR_DDRC_CKSEL           		(PWRC_DDRC_BASE + 0x810)
#define PWR_DDRC_HISI_IOCFG      		(PWRC_DDRC_BASE + 0x814)

#define ARM_SOC_TIMER0_BASE_ADDR         (IO_ADDRESS(0x90002000))
#define ARM_SOC_TIMER1_BASE_ADDR         (ARM_SOC_TIMER0_BASE_ADDR + 0x14)

#define ARM_SOC_TIMER2_BASE_ADDR         (ARM_SOC_TIMER1_BASE_ADDR + 0x14)
#define ARM_SOC_TIMER3_BASE_ADDR         (ARM_SOC_TIMER2_BASE_ADDR + 0x14)

#define ARM_SOC_TIMER4_BASE_ADDR         (ARM_SOC_TIMER3_BASE_ADDR + 0x14)
#define ARM_SOC_TIMER5_BASE_ADDR         (ARM_SOC_TIMER4_BASE_ADDR + 0x14)

#define ARM_SOC_TIMER6_BASE_ADDR         (ARM_SOC_TIMER5_BASE_ADDR + 0x14)
#define ARM_SOC_TIMER7_BASE_ADDR         (ARM_SOC_TIMER6_BASE_ADDR + 0x14)

#define ARM_SOC_TIMER8_BASE_ADDR         (IO_ADDRESS(0x90003000))
#define ARM_SOC_TIMER9_BASE_ADDR         (ARM_SOC_TIMER8_BASE_ADDR + 0x14)

#define ARM_SOC_TIMER10_BASE_ADDR        (ARM_SOC_TIMER9_BASE_ADDR + 0x14)
#define ARM_SOC_TIMER11_BASE_ADDR        (ARM_SOC_TIMER10_BASE_ADDR + 0x14)

#define ARM_SOC_TIMER12_BASE_ADDR        (ARM_SOC_TIMER11_BASE_ADDR + 0x14)
#define ARM_SOC_TIMER13_BASE_ADDR        (ARM_SOC_TIMER12_BASE_ADDR + 0x14)

#define ARM_SOC_TIMER14_BASE_ADDR        (ARM_SOC_TIMER13_BASE_ADDR + 0x14)
#define ARM_SOC_TIMER15_BASE_ADDR        (ARM_SOC_TIMER14_BASE_ADDR + 0x14)

#define ARM_SOC_TIMER16_BASE_ADDR        (IO_ADDRESS(0x90004000))
#define ARM_SOC_TIMER17_BASE_ADDR        (ARM_SOC_TIMER16_BASE_ADDR + 0x14)

#define ARM_SOC_TIMER18_BASE_ADDR        (ARM_SOC_TIMER17_BASE_ADDR + 0x14)
#define ARM_SOC_TIMER19_BASE_ADDR        (ARM_SOC_TIMER18_BASE_ADDR + 0x14)

#define ARM_SOC_TIMER20_BASE_ADDR        (ARM_SOC_TIMER19_BASE_ADDR + 0x14)
#define ARM_SOC_TIMER21_BASE_ADDR        (ARM_SOC_TIMER20_BASE_ADDR + 0x14)

#define ARM_SOC_TIMER22_BASE_ADDR        (ARM_SOC_TIMER21_BASE_ADDR + 0x14)
#define ARM_SOC_TIMER23_BASE_ADDR        (ARM_SOC_TIMER22_BASE_ADDR + 0x14)

#define ARM_SOC_TIMER_COUNT                     (IO_ADDRESS(0x90003040))

/**********************************************************/
/*physical addr */
/**********************************************************/
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2))  \
    ||(defined(BOARD_SFT) && defined(VERSION_V7R1)) 

#define PWRCTRL_SOC_PHY_ADDR_TIMER0			(0x90002000)
#define PWRCTRL_SOC_PHY_ADDR_TIMER1			(0x90003000)
#define PWRCTRL_SOC_PHY_ADDR_TIMER2			(0x90004000)

#elif (defined (BOARD_SFT) && defined (VERSION_V3R2))

#define PWRCTRL_SOC_PHY_ADDR_TIMER0			(0x90026000)
#define PWRCTRL_SOC_PHY_ADDR_TIMER1			(0x90002000)
#define PWRCTRL_SOC_PHY_ADDR_TIMER2			(0x90003000)

#elif ((defined (BOARD_ASIC)||defined (BOARD_ASIC_BIGPACK)) \
  && (defined(CHIP_BB_6920ES)||defined (CHIP_BB_6920CS)))

#define PWRCTRL_SOC_PHY_ADDR_TIMER0			(0x90002000)
#define PWRCTRL_SOC_PHY_ADDR_TIMER1			(0x90003000)
#define PWRCTRL_SOC_PHY_ADDR_TIMER2			(0x90004000)

#endif

/*********************************************************/
/*virtual addr*/
/*********************************************************/
#define PWRCTRL_SOC_ADDR_TIMER0_0         	(IO_ADDRESS(PWRCTRL_SOC_PHY_ADDR_TIMER0 + 5*4*0))
#define PWRCTRL_SOC_ADDR_TIMER0_1         	(IO_ADDRESS(PWRCTRL_SOC_PHY_ADDR_TIMER0 + 5*4*1))
#define PWRCTRL_SOC_ADDR_TIMER0_2         	(IO_ADDRESS(PWRCTRL_SOC_PHY_ADDR_TIMER0 + 5*4*2))
#define PWRCTRL_SOC_ADDR_TIMER0_3         	(IO_ADDRESS(PWRCTRL_SOC_PHY_ADDR_TIMER0 + 5*4*3))
#define PWRCTRL_SOC_ADDR_TIMER0_4         	(IO_ADDRESS(PWRCTRL_SOC_PHY_ADDR_TIMER0 + 5*4*4))
#define PWRCTRL_SOC_ADDR_TIMER0_5         	(IO_ADDRESS(PWRCTRL_SOC_PHY_ADDR_TIMER0 + 5*4*5))
#define PWRCTRL_SOC_ADDR_TIMER0_6         	(IO_ADDRESS(PWRCTRL_SOC_PHY_ADDR_TIMER0 + 5*4*6))
#define PWRCTRL_SOC_ADDR_TIMER0_7         	(IO_ADDRESS(PWRCTRL_SOC_PHY_ADDR_TIMER0 + 5*4*7))

#define PWRCTRL_SOC_ADDR_TIMER1_0         	(IO_ADDRESS(PWRCTRL_SOC_PHY_ADDR_TIMER1 + 5*4*0))
#define PWRCTRL_SOC_ADDR_TIMER1_1         	(IO_ADDRESS(PWRCTRL_SOC_PHY_ADDR_TIMER1 + 5*4*1))
#define PWRCTRL_SOC_ADDR_TIMER1_2         	(IO_ADDRESS(PWRCTRL_SOC_PHY_ADDR_TIMER1 + 5*4*2))
#define PWRCTRL_SOC_ADDR_TIMER1_3         	(IO_ADDRESS(PWRCTRL_SOC_PHY_ADDR_TIMER1 + 5*4*3))
#define PWRCTRL_SOC_ADDR_TIMER1_4         	(IO_ADDRESS(PWRCTRL_SOC_PHY_ADDR_TIMER1 + 5*4*4))
#define PWRCTRL_SOC_ADDR_TIMER1_5         	(IO_ADDRESS(PWRCTRL_SOC_PHY_ADDR_TIMER1 + 5*4*5))
#define PWRCTRL_SOC_ADDR_TIMER1_6         	(IO_ADDRESS(PWRCTRL_SOC_PHY_ADDR_TIMER1 + 5*4*6))
#define PWRCTRL_SOC_ADDR_TIMER1_7         	(IO_ADDRESS(PWRCTRL_SOC_PHY_ADDR_TIMER1 + 5*4*7))

#define PWRCTRL_SOC_ADDR_TIMER2_0         	(IO_ADDRESS(PWRCTRL_SOC_PHY_ADDR_TIMER2 + 5*4*0))
#define PWRCTRL_SOC_ADDR_TIMER2_1         	(IO_ADDRESS(PWRCTRL_SOC_PHY_ADDR_TIMER2 + 5*4*1))
#define PWRCTRL_SOC_ADDR_TIMER2_2         	(IO_ADDRESS(PWRCTRL_SOC_PHY_ADDR_TIMER2 + 5*4*2))
#define PWRCTRL_SOC_ADDR_TIMER2_3         	(IO_ADDRESS(PWRCTRL_SOC_PHY_ADDR_TIMER2 + 5*4*3))
#define PWRCTRL_SOC_ADDR_TIMER2_4         	(IO_ADDRESS(PWRCTRL_SOC_PHY_ADDR_TIMER2 + 5*4*4))
#define PWRCTRL_SOC_ADDR_TIMER2_5         	(IO_ADDRESS(PWRCTRL_SOC_PHY_ADDR_TIMER2 + 5*4*5))
#define PWRCTRL_SOC_ADDR_TIMER2_6         	(IO_ADDRESS(PWRCTRL_SOC_PHY_ADDR_TIMER2 + 5*4*6))
#define PWRCTRL_SOC_ADDR_TIMER2_7         	(IO_ADDRESS(PWRCTRL_SOC_PHY_ADDR_TIMER2 + 5*4*7))

/*********************************************************/
/*WIFI/HSIC SLAVE PERI*/
/*********************************************************/
#define PWRCTRL_WIFI_LIGHT_SLEEP_BIT          (0)
#define PWRCTRL_WIFI_DEEP_SLEEP_BIT           (1)
#define PWRCTRL_HSIC_SLAVE_L2_BIT             (2)
#define PWRCTRL_HSIC_SLAVE_L3_BIT             (3)

#define PWRCTRL_WIFI_SET_LIGHT_SLEEP_BIT(x)   (*(volatile unsigned int*)x |= (1<<PWRCTRL_WIFI_LIGHT_SLEEP_BIT))
#define PWRCTRL_WIFI_SET_DEEP_SLEEP_BIT(x)    (*(volatile unsigned int*)x |= (1<<PWRCTRL_WIFI_DEEP_SLEEP_BIT))
#define PWRCTRL_WIFI_CLEAR_SLEEP_BIT(x)       (*(volatile unsigned int*)x &= (~(unsigned int)(0x3)))
#define PWRCTRL_WIFI_GET_DEEP_SLEEP_BIT(x)    (*(volatile unsigned int*)x & (1<<PWRCTRL_WIFI_DEEP_SLEEP_BIT))
#define PWRCTRL_HSIC_SET_L2_BIT(x)            (*(volatile unsigned int*)x |= (1<<PWRCTRL_HSIC_SLAVE_L2_BIT))
#define PWRCTRL_HSIC_SET_L3_BIT(x)            (*(volatile unsigned int*)x |= (1<<PWRCTRL_HSIC_SLAVE_L3_BIT))
#define PWRCTRL_HSIC_CLEAR_SLEEP_BIT(x)       (*(volatile unsigned int*)x &= (~(unsigned int)(0xc)))
#define PWRCTRL_HSIC_GET_L3_BIT(x)            (*(volatile unsigned int*)x & (1<<PWRCTRL_HSIC_SLAVE_L3_BIT))


#endif



