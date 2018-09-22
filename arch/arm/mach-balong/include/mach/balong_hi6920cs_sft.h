#ifndef __ASM_ARCH_BOARD_HI6920CS_SFT_H__
#define __ASM_ARCH_BOARD_HI6920CS_SFT_H__

#include <asm/sizes.h>
#include <mach/hardware.h>
#include <generated/FeatureConfig.h>   /*syb*/
#include <mach/memMapGlobal.h>

/*I2C*/
#define I2C_BASE        0x90019000
#define I2C_SIZE        SZ_4K

#define BC_CTRL_BASE    0x9000c000
#define BC_CTRL_SIZE    SZ_4K

/*串口*/
#define UART0_BASE      0x90007000  /* UART 0 */
#define UART1_BASE      0x90016000  /* UART 1 */
#define UART2_BASE      0x90017000  /* UART 2 */
#define UART3_BASE      0x90018000  /* UART 3 */
#define UART_REGS_SIZE  SZ_4K

#if defined(FEATURE_HSIC_ON) && ((defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))) 
/* GPIO */
#define GPIO_0_BASE     0x90006000
#define GPIO_0_REG_SIZE SZ_4K
#define GPIO_1_BASE     0x90011000
#define GPIO_1_REG_SIZE SZ_4K
#define GPIO_2_BASE     0x90012000
#define GPIO_2_REG_SIZE SZ_4K
#define GPIO_3_BASE     0x90013000
#define GPIO_3_REG_SIZE SZ_4K
#define GPIO_4_BASE     0x90014000
#define GPIO_4_REG_SIZE SZ_4K
#define GPIO_5_BASE     0x90015000
#define GPIO_5_REG_SIZE SZ_4K
#endif

#define PV500V1_NANDC_BASE      0xA0100000
#define PV500V1_NANDC_REGS_SIZE SZ_64K

/*系统控制器*/
#define V3R2_SC_BASE            0x90000000
#define V3R2_SC_SIZE			SZ_4K
#define V3R2_SC_VA_BASE      IO_ADDRESS(V3R2_SC_BASE)  
#define V7R1_SC_VA_BASE      IO_ADDRESS(V3R2_SC_BASE)  

/*SRAM1*/
#define V3R2_SRAM1_BASE			0x2FFE0000
#define V3R2_SRAM1_SIZE			SZ_128K
#define ISRAM1_VA_BASE 		IO_ADDRESS(V3R2_SRAM1_BASE) 

#define PBXA9_GIC_DIST_CONTROL_VA	IO_ADDRESS(PBXA9_GIC_DIST_CONTROL)


/*NAND FLASH*/
#define NANDF_BUFFER_ADDR               0xA0000000
#define NANDF_BUFFER_SIZE               SZ_256M

/*timer 8~15*/            
#define TIMER_REGBASE_8TO15             0x90003000
#define TIMER_8                         8
#define TIMER_OFFSET(x)                 0x14*((x)-TIMER_8)  
#define TIMER8TO15_REGS_SIZE            SZ_4K

#define TIMER14_BASE                    (TIMER_REGBASE_8TO15 + TIMER_OFFSET(14))
/*DMAC*/
#define DMA_REGBASE_ADR                 IO_ADDRESS(0x900AA000)

#define PBXA9_PERIPHBASE                (0x4000000)
#define PBXA9_PERIPHBASE_SIZE           SZ_4K

#define INTEGRATOR_SC_BASE              (V3R2_SC_BASE)

/* IPF模块*/
#define IPF_REGBASE_PHY_ADR             0x900AC000
#define IPF_REG_SIZE                    SZ_4K

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
#define AXI_MEM_BASE_ADDR                    0x2ffc0000
#define AXI_MEM_ADDR                    		IO_ADDRESS(AXI_MEM_BASE_ADDR)

#define AXI_MEM_SIZE                    0x40000

//#define PBXA9_DRAM_SHARE_PHYS           (0x38000000)
#define VX_PBXA9_DRAM_SHARE_PHYS     GLOBAL_MEM_BASE_ADDR


/* 96 is the maximum interrupt number. It covers SGI, PPI and SPI */
#define SYS_INT_LEVELS_MAX          (96+32)

/* interrupt distributor */
#define PBXA9_GIC_DIST_CONTROL      (PBXA9_PERIPHBASE + 0x1000)
#define PBXA9_GIC_DIST_SIZE         SZ_4K

/* interrupt levels */
#define A9_GLOBAL_INTS              32
#define INT_LVL_MAX                 128

#define INT_LVL_WDT                 (A9_GLOBAL_INTS+0)
/* 1保留 */
/* 2-3为ARM11中断,BSP暂不定义 */
#define INT_LVL_TIMER_0             (A9_GLOBAL_INTS+4)
#define INT_LVL_TIMER_1             (A9_GLOBAL_INTS+5)
#define INT_LVL_TIMER_23            (A9_GLOBAL_INTS+6)
#define INT_LVL_TIMER_4567          (A9_GLOBAL_INTS+7)
#define INT_LVL_TIMER_14            91

//TODO:#define INT_LVL_TIMER_2_3     (37) /* 2 or 3 Timers */
//TODO:#define INT_LVL_TIMER_0_1     (36) /* 0 or 1 Timers */
/* 8保留 */
#define INT_LVL_SCI                 (A9_GLOBAL_INTS+9)
#define INT_LVL_DMA0                42
#define INT_LVL_DMA1                128
#define INT_LVL_DMA2                129
#define INT_LVL_DMA3                130

#define INT_LVL_CIPHER              (A9_GLOBAL_INTS+11)
#define INT_LVL_USB                 (A9_GLOBAL_INTS+12)
#define INT_LVL_UART_0              102 /* UART0 */
#define INT_LVL_UART_1              103 /* UART1 */
#define INT_LVL_UART_2              104 /* UART2 */
#define INT_LVL_UART_3              105 /* UART3 */
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
#ifdef PRODUCT_CFG_CORE_TYPE_MODEM
#define INT_LVL_SUBVIC              (A9_GLOBAL_INTS+54)
#else
#define INT_LVL_SUBVIC              (A9_GLOBAL_INTS+55)
#endif
#define INT_LVL_FPGA                INT_LVL_SUBVIC      /* FPGA_AXI中断*/
/* 88-95保留 */

/* IPF */
#define INT_LVL_IPF                 45  /*IPF*/

#define INT_VEC_SY_CONTROL0         132

#define SYS_TIMER_INT_LVL           (INT_LVL_TIMER_14)
#define AUX_TIMER_INT_LVL           (INT_LVL_TIMER_14)
#define STAMP_TIMER_INT_LVL         (INT_LVL_TIMER_14)
#define USR_TIMER_INT_LVL          84

/*****************************************************
 * 时钟区 
 *****************************************************/
 /* signals generated from various clock generators */
#define PBXA9_TIMERS_CLK        (19200000)           /* Real time clock */
#define PBXA9_UART_CLK          (48000000)           /* UART clock */
/* frequency of counter/timers */
#define SYS_TIMER_CLK           (PBXA9_TIMERS_CLK)
//#define SYS_CLK_RATE_MIN        (10)
//#define SYS_CLK_RATE_MAX        (8000)
#define AUX_TIMER_CLK           (PBXA9_TIMERS_CLK)
#define STAMP_TIMER_CLK         (PBXA9_TIMERS_CLK)

#define STAMP_CLK_RATE_MIN      10
#define STAMP_CLK_RATE_MAX      8000

/*timer0~2*/
#define PBXA9_TIMER0_BASE            (0x90002000)
#define PBXA9_TIMER0_SIZE			SZ_4K
#define PBXA9_TIMER0_VA_BASE      IO_ADDRESS(PBXA9_TIMER0_BASE+0x28)  

#define PBXA9_TIMER1_BASE            (0x90003000)
#define PBXA9_TIMER1_SIZE			SZ_4K
#define PBXA9_TIMER1_VA_BASE      IO_ADDRESS(PBXA9_TIMER1_BASE)  

#define PBXA9_TIMER2_BASE            (0x90004000)
#define PBXA9_TIMER2_SIZE			SZ_4K
#define PBXA9_TIMER2_VA_BASE      IO_ADDRESS(PBXA9_TIMER2_BASE) 

/*WDT*/
#define PBXA9_WDT_BASE               0x90001000
#define PBXA9_WDT_SIZE                    SZ_4K
#define PBXA9_WDT_VA_BASE      IO_ADDRESS(PBXA9_WDT_BASE) 



/*address transfer */
#define IO_TYPE(var) ((ICC_SUB_CHAN_S *)IO_ADDRESS((unsigned int)(var)))
#define PHY_TO_VIRT(var)  ((BSP_U8*)IO_ADDRESS((unsigned int)(var)))
#define VIRT_TO_PHY(var)  (((var) - AXI_MEM_ADDR) + AXI_MEM_BASE_ADDR)

/* gpio3 */
#define PV500V1_GPIO3_BASE	0x90014000
#define PV500V1_GPIO_REGS_SIZE	SZ_4K

/* emi */
#define EMI_REG_BASE_ADDR	0x900A2000
#define EMI_REG_SIZE		0x1000
#define EMI_MEM_BASE_ADDR	0x900A3000
#define EMI_MEM_SIZE		0x1000

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
