#ifndef __ASM_ARCH_BOARD_PV500_H
#define __ASM_ARCH_BOARD_PV500_H
#include <mach/hardware.h> /*add by m00176101*/
#include <asm/sizes.h>
#include <generated/MemoryMap.h>

/*串口*/
#define PV500V1_UART0_BASE      0x20004000  /* UART 0 */
#define PV500V1_UART1_BASE      0x20005000  /* UART 1 */
#define PV500V1_UART2_BASE      0x20006000  /* UART 2 */
#define PV500V1_UART_REGS_SIZE  SZ_4K

#define PV500V1_NANDC_BASE      0x80020000
#define PV500V1_NANDC_REGS_SIZE SZ_64K

/*NAND FLASH*/
#define NANDF_BUFFER_ADDR               0x90000000
#define NANDF_BUFFER_SIZE               SZ_256M

/*timer 4~7*/            
#define PV500V1_TIMER_BASE      0x20003000
#define PV500V1_CLK_REGS_SIZE   SZ_4K

#define PV500V1_TIMER4_BASE     PV500V1_TIMER_BASE
#define PV500V1_TIMER5_BASE     (PV500V1_TIMER4_BASE + 0x14)
#define PV500V1_TIMER6_BASE     (PV500V1_TIMER4_BASE + 0x28)
#define PV500V1_TIMER7_BASE     (PV500V1_TIMER4_BASE + 0x3c)

/*fpga timer*/            
#define FPGA_TIMER6_BASE      0x5F057000
#define FPGA_TIMER6_SIZE      SZ_4K
#define PBXA9_TIMER0_VA_BASE             IO_ADDRESS(FPGA_TIMER6_BASE)

#define PBXA9_PERIPHBASE        (0x10000000)
#define PBXA9_PERIPHBASE_SIZE   SZ_4K

/*
 * Generic Interrupt Controller
 * Note: FIQ is not handled within VxWorks so this is just IRQ
 */
#define PBXA9_GIC_CPU_BASE                  (PBXA9_PERIPHBASE)
#define PBXA9_GIC_CPU_CONTROL               (PBXA9_GIC_CPU_BASE + 0x0100)
#define PBXA9_GIC_CPU_PRIORITY              (PBXA9_GIC_CPU_BASE + 0x0104)
#define PBXA9_GIC_CPU_POINT                 (PBXA9_GIC_CPU_BASE + 0x0108)
#define PBXA9_GIC_CPU_ACK                   (PBXA9_GIC_CPU_BASE + 0x010C)
#define PBXA9_GIC_CPU_END_INTR              (PBXA9_GIC_CPU_BASE + 0x0110)
#define PBXA9_GIC_CPU_RUNNING               (PBXA9_GIC_CPU_BASE + 0x0114)
#define PBXA9_GIC_CPU_PENDING               (PBXA9_GIC_CPU_BASE + 0x0118)


#define VX_PBXA9_DRAM_SHARE_PHYS     (0xc0000000)

/*EXC，同时地址需要和C核一致*/
#define PBXA9_DRAM_EXC_SHARE_PHYS     (GLOBAL_MEM_EXCP_BASE_ADDR) /*syb*/
#define PBXA9_DRAM_EXC_SHARE_VIRT     (ECS_MDDR_RESERVE_BASE_ADDR_VIRT + 0x02000000)
#define PBXA9_DRAM_EXC_SHARE_SIZE     (GLOBAL_MEM_EXCP_SIZE)

/*WDT*/
#define PBXA9_WDT_BASE               0x20001000
#define PBXA9_WDT_SIZE                    SZ_4K
#define PBXA9_WDT_VA_BASE      IO_ADDRESS(PBXA9_WDT_BASE) 

/*系统控制器*/
#define V3R2_SC_BASE            0x80000000
#define V3R2_SC_SIZE			SZ_4K
#define V3R2_SC_VA_BASE      IO_ADDRESS(V3R2_SC_BASE) 

/*FPGA soc reset*/
#define FPGA_SOC_RESET_BASE     0x5f050000
#define FPGA_SOC_RESET_SIZE     SZ_4K
#define FPGA_SOC_RESET_VA_BASE  IO_ADDRESS(0x5f050000)

/*20111231,PS融合调整MemoryMap,c61362 start */
/*IPF Register*/ 
#define IPF_REG_BASE_ADDR               (0x5F0B0000)        
#define IPF_REG_SIZE                    (0x1000) 

/* 96 is the maximum interrupt number. It covers SGI, PPI and SPI */
#define SYS_INT_LEVELS_MAX          (96+32)

/* interrupt distributor */
#define PBXA9_GIC_DIST_CONTROL      (PBXA9_PERIPHBASE + 0x1000)
#define PBXA9_GIC_DIST_SIZE         SZ_4K

/* interrupt levels */
#define A9_GLOBAL_INTS              32
#define INT_LVL_MAX                 (128 + 16)

#define INT_LVL_WDT                 (A9_GLOBAL_INTS+0)
/* 1保留 */
/* 2-3为ARM11中断,BSP暂不定义 */
#define INT_LVL_TIMER_0             (A9_GLOBAL_INTS+4)
#define INT_LVL_TIMER_1             (A9_GLOBAL_INTS+5)
#define INT_LVL_TIMER_23            (A9_GLOBAL_INTS+6)
#define INT_LVL_TIMER_4567          (A9_GLOBAL_INTS+7)
//TODO:#define INT_LVL_TIMER_2_3     (37) /* 2 or 3 Timers */
//TODO:#define INT_LVL_TIMER_0_1     (36) /* 0 or 1 Timers */
/* 8保留 */
#define INT_LVL_SCI                 (A9_GLOBAL_INTS+9)
#define INT_LVL_DMA                 (A9_GLOBAL_INTS+10)
#define INT_LVL_CIPHER              (A9_GLOBAL_INTS+11)
#define INT_LVL_USB                 (A9_GLOBAL_INTS+12)
#define INT_LVL_UART_0              (A9_GLOBAL_INTS+26) /* UART0 */
#define INT_LVL_UART_1              (A9_GLOBAL_INTS+27) /* UART1 */
#define INT_LVL_UART_2              (A9_GLOBAL_INTS+28) /* UART2 */
#define INT_LVL_RTC                 (A9_GLOBAL_INTS+29)
#define INT_LVL_NANDC               (A9_GLOBAL_INTS+30)
#define INT_LVL_SD_MMC              (A9_GLOBAL_INTS+31)
#define INT_LVL_I2C                 (A9_GLOBAL_INTS+32)
#define INT_LVL_GMAC                (A9_GLOBAL_INTS+33)
#define INT_LVL_GPIO_0              (A9_GLOBAL_INTS+34)
#define INT_LVL_DESRSASHA           (A9_GLOBAL_INTS+35)
#define INT_LVL_IPCM2ARM            (A9_GLOBAL_INTS+36)
/* 37-40为CEVA中断,BSP不定义 */
#define INT_LVL_PMU                 (A9_GLOBAL_INTS+41) /* PMU组合中断*/
#define INT_LVL_AXIMONITOR          (A9_GLOBAL_INTS+42) /* PMU组合中断*/
/* 43-44为CEVA中断,BSP不定义 */
#define INT_LVL_GPIO_1              (A9_GLOBAL_INTS+45)
/* 46-47保留 */
/* 48-63为PAD_INTX,BSP暂不定义 */
/* 64-85保留 */   
/* 88-95保留 */

/* SUB VIC */
#define FPGA_VIC_BASE               0x5F060000
#define FPGA_VIC_SIZE               0x00001000
#define FPGA_VIC_STATUS             (subvic_base + 0x000)
#define FPGA_VIC_ENABLE             (subvic_base + 0x010)
#define FPGA_VIC_DISABLE            (subvic_base + 0x014)
#define SUBVIC_NUM                  32
#define INT_LVL_SUBVIC              (94)
#define INT_LVL_IPF                 (INT_LVL_MAX + 18)  /*IPF*/

#define FPGA_DTIMER0_BASE           0x5F067000
#define FPGA_DTIMER0_SIZE           0x00001000




#define PBXA9_TIMERS_CLK        (19200000)           /* Real time clock */
#define SYS_TIMER_INT_LVL           (INT_LVL_TIMER_4567)
#define AUX_TIMER_INT_LVL           (INT_LVL_TIMER_4567)
#define STAMP_TIMER_INT_LVL         (INT_LVL_TIMER_4567)
#define USR_TIMER_INT_LVL           (INT_LVL_TIMER_4567)

/*address transfer */
#define IO_TYPE(var) ((ICC_SUB_CHAN_S *)IO_ADDRESS((unsigned int)(var)))
#define PHY_TO_VIRT(var)  ((BSP_U8*)IO_ADDRESS((unsigned int)(var)))
#define VIRT_TO_PHY(var)  (((var) - AXI_MEM_ADDR) + AXI_MEM_BASE_ADDR)

#endif

