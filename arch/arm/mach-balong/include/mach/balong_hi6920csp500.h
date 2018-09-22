#ifndef __ASM_ARCH_BOARD_HI6920CS_P500_H
#define __ASM_ARCH_BOARD_HI6920CS_P500_H

#include <asm/sizes.h>
#include <mach/memMapGlobal.h>

/*串口*/
#define PV500V1_UART0_BASE                  0x20004000  /* UART 0 */
#define PV500V1_UART1_BASE                  0x20005000  /* UART 1 */
#define PV500V1_UART2_BASE                  0x20006000  /* UART 2 */
#define PV500V1_UART_REGS_SIZE              SZ_4K

#define PV500V1_NANDC_BASE                  0x80020000
#define PV500V1_NANDC_REGS_SIZE             SZ_64K

/*NAND FLASH*/
#define NANDF_BUFFER_ADDR                   0x90000000
#define NANDF_BUFFER_SIZE                   SZ_256M

/*timer 4~7*/            
#define PV500V1_TIMER_BASE                  0x20003000
#define PV500V1_CLK_REGS_SIZE               SZ_4K

#define PV500V1_TIMER4_BASE                 PV500V1_TIMER_BASE
#define PV500V1_TIMER5_BASE                 (PV500V1_TIMER4_BASE + 0x14)
#define PV500V1_TIMER6_BASE                 (PV500V1_TIMER4_BASE + 0x28)
#define PV500V1_TIMER7_BASE                 (PV500V1_TIMER4_BASE + 0x3c)

/*fpga timer*/
#if 1 //comment by w54542 in 2011.12.06:not used
#define FPGA_TIMER6_BASE                    PV500V1_TIMER6_BASE
#define FPGA_TIMER6_SIZE                    SZ_4K
#define PBXA9_TIMER0_VA_BASE                IO_ADDRESS(FPGA_TIMER6_BASE)
#endif

#define PBXA9_PERIPHBASE                    (0x10000000)
#define PBXA9_PERIPHBASE_SIZE               SZ_4K

#define INTEGRATOR_SC_BASE                  (0x80000000)
#define INTEGRATOR_SC_SIZE                  (SZ_4K)
#define INTEGRATOR_SC_VA_SIZE               IO_ADDRESS(INTEGRATOR_SC_BASE)

/* SOCP */
#define SOCP_REG_BASEADDR                   0x500A9000
#define SOCP_REG_SIZE                       SZ_4K

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

/*AXI*/
#define AXI_MEM_BASE_ADDR                   0x30000000
#define AXI_MEM_ADDR                    	IO_ADDRESS(0x30000000)
#define AXI_MEM_SIZE                        (0x20000)

//#define PBXA9_DRAM_SHARE_PHYS               (0xc8000000)
#define VX_PBXA9_DRAM_SHARE_PHYS     GLOBAL_MEM_BASE_ADDR


/*WDT*/
#define PBXA9_WDT_BASE                      0x20001000
#define PBXA9_WDT_SIZE                      SZ_4K
#define PBXA9_WDT_VA_BASE                   IO_ADDRESS(PBXA9_WDT_BASE) 

/*系统控制器*/
#define V3R2_SC_BASE                        0x80000000
#define V3R2_SC_SIZE			            SZ_4K
#define V3R2_SC_VA_BASE                     IO_ADDRESS(V3R2_SC_BASE) 

/*FPGA soc reset*/
#define FPGA_SOC_RESET_BASE                 0x5f050000
#define FPGA_SOC_RESET_SIZE                 SZ_4K
#define FPGA_SOC_RESET_VA_BASE              IO_ADDRESS(0x5f050000)

/* IPF*/
#define IPF_REGBASE_PHY_ADR                 0x500AC000
#define IPF_REG_SIZE                        SZ_4K

/* interrupt distributor */
#define PBXA9_GIC_DIST_CONTROL      (PBXA9_PERIPHBASE + 0x1000)
#define PBXA9_GIC_DIST_SIZE         SZ_4K

/* interrupt levels */
#define A9_GLOBAL_INTS              32
#define INT_LVL_HW_MAX              128
#define INT_LVL_SW_MAX              16
#define INT_LVL_MAX                 ((INT_LVL_HW_MAX) + (INT_LVL_SW_MAX))
#define SYS_INT_LEVELS_MAX          INT_LVL_MAX
#define SUBVIC_NUM                  29

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
#ifdef PRODUCT_CFG_CORE_TYPE_MODEM
#define INT_LVL_SUBVIC              (A9_GLOBAL_INTS+62)
#else
#define INT_LVL_SUBVIC              (A9_GLOBAL_INTS+63)
#endif
#define INT_LVL_FPGA                INT_LVL_SUBVIC      /* FPGA_AXI中断*/

/* 64-85保留 */   
/* 88-95保留 */

/* SUB VIC */
#ifdef PRODUCT_CFG_CORE_TYPE_MODEM
#define SUBVIC_BASE 0x50140000
#else
#define SUBVIC_BASE 0x50150000
#endif
#define SUBVIC_SIZE 0x00001000

/*SUBVIC*/
#define SUBVIC_IRQ_ENABLE_L                 (0x00)
#define SUBVIC_IRQ_ENABLE_H                 (0x04)
#define SUBVIC_IRQ_STATUS_L                 (0x30)

//#define FPGA_VIC_BASE               0x5F060000
//#define FPGA_VIC_SIZE               0x00001000
//#define FPGA_VIC_STATUS             (subvic_base + 0x000)
//#define FPGA_VIC_ENABLE             (subvic_base + 0x010)
//#define FPGA_VIC_DISABLE            (subvic_base + 0x014)
//#define INT_LVL_SUBVIC              (94)
#define INT_LVL_IPF                 (INT_LVL_MAX + 18) /*IPF*/

#define FPGA_DTIMER0_BASE           0x5F067000
#define FPGA_DTIMER0_SIZE           0x00001000


#define PBXA9_TIMERS_CLK            (19200000)           /* Real time clock */
#define SYS_TIMER_INT_LVL           (INT_LVL_TIMER_4567)
#define AUX_TIMER_INT_LVL           (INT_LVL_TIMER_4567)
#define STAMP_TIMER_INT_LVL         (INT_LVL_TIMER_4567)
#define USR_TIMER_INT_LVL           (INT_LVL_TIMER_4567)

/*address transfer */
#define IO_TYPE(var) ((ICC_SUB_CHAN_S *)IO_ADDRESS((unsigned int)(var)))
#define PHY_TO_VIRT(var)  ((BSP_U8*)IO_ADDRESS((unsigned int)(var)))
#define VIRT_TO_PHY(var)  (((var) - AXI_MEM_ADDR) + AXI_MEM_BASE_ADDR)

/* MEM Rserved For GU */
#define ECS_MDDR_RESERVE_ADDR       GLOBAL_MEM_GU_RESERVED_ADDR
#define ECS_TTF_MEM_BASE            GLOBAL_MEM_GU_RESERVED_ADDR
#define ECS_TTF_SIZE                        (0x00E00000)
#define ECS_TTF_MEM_SIZE            ECS_TTF_SIZE

#define ECS_NV_BASE                 (ECS_MDDR_RESERVE_ADDR + 0x014A0000)
#define ECS_NV_SIZE                 0x00030000

#define ECS_TTF_DICC_BASE           (ECS_MDDR_RESERVE_ADDR + 0x014D0000)
#define ECS_TTF_DICC_SIZE           0x00020000

/* added by w54542 in 2011-12-06 start */
#define PBXA9_VX_RAM_HIGH_ADRS              GLOBAL_MEM_MCORE_USER_RESERVED_ADDR    /* 和C核保持一致 */
#define PBXA9_VX_RAM_BOOTLOAD_VER_LEN       MEMORY_RAM_BOOTLOAD_VER_LEN
#define PBXA9_VX_RAM_TRACE_SIZE             MEMORY_RAM_TRACE_SIZE
#define PBXA9_VX_RAM_EXC_ALIGN_SIZE         (4*1024-PBXA9_VX_RAM_BOOTLOAD_VER_LEN-PBXA9_VX_RAM_TRACE_SIZE)
/* added by w54542 in 2011-12-06 end */

/*EXC，同时地址需要和C核一致*/
#define PBXA9_DRAM_EXC_SHARE_PHYS           MEMORY_RAM_DEBUG_ADDR
#define PBXA9_DRAM_EXC_SHARE_VIRT           (GLOBAL_MEM_GU_RESERVED_ADDR_VIRT + 0x02000000)
#define PBXA9_DRAM_EXC_SHARE_SIZE           MEMORY_RAM_DEBUG_SIZE

#define PBXA9_DRAM_GU_NV_VIRT           GLOBAL_MEM_GUNV_RESV_VIRT
#define PBXA9_DRAM_GU_NV_PHYS           GLOBAL_MEM_GUNV_RESV_PHYS
#define PBXA9_DRAM_GU_NV_SIZE           GLOBAL_MEM_GUNV_RESV_SIZE
#endif
