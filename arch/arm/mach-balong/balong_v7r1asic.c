#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/sysdev.h>
#include <linux/amba/bus.h>
#include <linux/io.h>
#include <linux/mm.h>

#include <generated/FeatureConfig.h>   /*syb*/
#include <mach/hardware.h>
#include <asm/irq.h>
#include <asm/leds.h>
#include <asm/mach-types.h>
#include <asm/pmu.h>
#include <asm/pgtable.h>
#include <asm/hardware/gic.h>
#include <asm/localtimer.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/time.h>

#include <mach/irqs.h>
#include <mach/nand.h>

#include "balong_core.h"
#include <mach/balong_v100r001.h>
#include <mach/lm.h>
#include <mach/platform.h>
#include "BSP.h"
#include <mach/common/mem/bsp_mem.h>

static struct amba_device uart3_device =
{
	.dev = 
    {
		.coherent_dma_mask = ~0,
		.init_name = "dev:uart3",
		.platform_data = NULL,
	},
	.res =
	{
		.start	= UART3_BASE,
		.end	= UART3_BASE + SZ_4K - 1,
		.flags	= IORESOURCE_MEM,
	},
	.dma_mask	= ~0,
	.periphid   = 0x000c21c0,
	.irq		= { INT_LVL_UART_3, NO_IRQ },
};

static struct amba_device *amba_devs[] __initdata =
{
	&uart3_device,
};

static struct lm_device pv500_usb_otg={
	.dev={
		.init_name = "synopsys_dwc_otg",
	},
	.resource.start = REG_BASE_USB2DVC,
	.resource.end = REG_BASE_USB2DVC + REG_USB2DVC_IOSIZE -1,
	.resource.flags = IORESOURCE_MEM,
	.irq = IRQ_PV500_USB_OTG,
	.id = DWC_SYNOP_CORE_OTG,
};

#if defined(FEATURE_HSIC_ON) && ((defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))) 
static struct lm_device pv500_usb_hsic={
	.dev={
		.init_name = "synopsys_dwc_hsic",
	},
	.resource.start = REG_BASE_USBHSIC,
	.resource.end = REG_BASE_USBHSIC + REG_USBHSIC_IOSIZE -1,
	.resource.flags = IORESOURCE_MEM,
	.irq = IRQ_PV500_USB_HSIC,
	.id = DWC_SYNOP_CORE_HSIC,
};
#endif

static struct lm_device *pv500_lm_dev[] __initdata = {
#if defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)
#if defined(FEATURE_HSIC_ON)
    &pv500_usb_hsic,
#endif
#if (FEATURE_OTG == FEATURE_ON)
    &pv500_usb_otg,
#endif
#else   /* V3R2 */
	&pv500_usb_otg,
#endif
};

static struct map_desc pv500v1_io_desc[] __initdata = {
	{
		.virtual	= IO_ADDRESS(I2C_BASE),
		.pfn		= __phys_to_pfn(I2C_BASE),
		.length		= I2C_SIZE,
		.type		= MT_DEVICE,
	},

	{
		.virtual	= IO_ADDRESS(BC_CTRL_BASE),
		.pfn		= __phys_to_pfn(BC_CTRL_BASE),
		.length		= BC_CTRL_SIZE,
		.type		= MT_DEVICE,
	},
	
	{
		.virtual	= IO_ADDRESS(TIMER_REGBASE_8TO15),
		.pfn		= __phys_to_pfn(TIMER_REGBASE_8TO15),
		.length		= TIMER8TO15_REGS_SIZE,
		.type		= MT_DEVICE,
	},
	
	{
		.virtual	= IO_ADDRESS(PBXA9_PERIPHBASE),
		.pfn		= __phys_to_pfn(PBXA9_PERIPHBASE),
		.length		= PBXA9_PERIPHBASE_SIZE,
		.type		= MT_DEVICE,
	},
	{
		.virtual	= IO_ADDRESS(PBXA9_GIC_DIST_CONTROL),
		.pfn		= __phys_to_pfn(PBXA9_GIC_DIST_CONTROL),
		.length		= PBXA9_PERIPHBASE_SIZE,
		.type		= MT_DEVICE,
	},
	{
		.virtual	= IO_ADDRESS(PBXA9_GIC_CPU_CONTROL),
		.pfn		= __phys_to_pfn(PBXA9_GIC_CPU_CONTROL),
		.length		= PBXA9_PERIPHBASE_SIZE,
		.type		= MT_DEVICE,
	},
	{
		.virtual	= IO_ADDRESS(UART0_BASE),
		.pfn		= __phys_to_pfn(UART0_BASE),
		.length		= UART_REGS_SIZE,
		.type		= MT_DEVICE,
	},
    {
        .virtual    = IO_ADDRESS(UART3_BASE),
        .pfn        = __phys_to_pfn(UART3_BASE),
        .length     = UART_REGS_SIZE,
        .type       = MT_DEVICE,
    },	
	{
		.virtual	= IO_ADDRESS(PV500V1_GPIO3_BASE),
		.pfn		= __phys_to_pfn(PV500V1_GPIO3_BASE),
		.length	= PV500V1_GPIO_REGS_SIZE,
		.type	= MT_DEVICE,
	},
	{
		.virtual	= IO_ADDRESS(EMI_REG_BASE_ADDR),
		.pfn		= __phys_to_pfn(EMI_REG_BASE_ADDR),
		.length	= EMI_REG_SIZE,
		.type	= MT_DEVICE,
	},
	{
		.virtual	= IO_ADDRESS(EMI_MEM_BASE_ADDR),
		.pfn		= __phys_to_pfn(EMI_MEM_BASE_ADDR),
		.length	= EMI_MEM_SIZE,
		.type	= MT_DEVICE,
	},	
    {
        .virtual    = IO_ADDRESS(IPF_REGBASE_PHY_ADR),
        .pfn        = __phys_to_pfn(IPF_REGBASE_PHY_ADR),
        .length = IPF_REG_SIZE,
        .type   = MT_DEVICE,
    },
	{
		.virtual	= IO_ADDRESS(V3R2_SC_BASE),
		.pfn		= __phys_to_pfn(V3R2_SC_BASE),
		.length		= V3R2_SC_SIZE,
		.type		= MT_DEVICE,
	},
    /* BEGIN: Modified by Mabinjie at 2011-11-8 */
#if defined(FEATURE_HSIC_ON) && ((defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))) 
    /* GPIO0 */
	{
		.virtual	= IO_ADDRESS(GPIO_0_BASE),
		.pfn		= __phys_to_pfn(GPIO_0_BASE),
		.length		= GPIO_0_REG_SIZE,
		.type		= MT_DEVICE,
	},
    /* GPIO1 */
	{
		.virtual	= IO_ADDRESS(GPIO_1_BASE),
		.pfn		= __phys_to_pfn(GPIO_1_BASE),
		.length		= GPIO_1_REG_SIZE,
		.type		= MT_DEVICE,
	},    
    /* GPIO2 */
	{
		.virtual	= IO_ADDRESS(GPIO_2_BASE),
		.pfn		= __phys_to_pfn(GPIO_2_BASE),
		.length		= GPIO_2_REG_SIZE,
		.type		= MT_DEVICE,
	},    
    /* GPIO3 */
	{
		.virtual	= IO_ADDRESS(GPIO_3_BASE),
		.pfn		= __phys_to_pfn(GPIO_3_BASE),
		.length		= GPIO_3_REG_SIZE,
		.type		= MT_DEVICE,
	},    
    /* GPIO4 */
	{
		.virtual	= IO_ADDRESS(GPIO_4_BASE),
		.pfn		= __phys_to_pfn(GPIO_4_BASE),
		.length		= GPIO_4_REG_SIZE,
		.type		= MT_DEVICE,
	},    
    /* GPIO5 */
	{
		.virtual	= IO_ADDRESS(GPIO_5_BASE),
		.pfn		= __phys_to_pfn(GPIO_5_BASE),
		.length		= GPIO_5_REG_SIZE,
		.type		= MT_DEVICE,
	},
#endif
    /* END:   Modified by Mabinjie at 2011-11-8 */
        
	/*HDLC*/
    {   
            .virtual        = IO_ADDRESS(0x90120000),
            .pfn            = __phys_to_pfn(0x90120000),
            .length = 0x10000,
            .type   = MT_DEVICE,
    },
	/*Timer*/
    {   
            .virtual        = IO_ADDRESS(0x90000000),
            .pfn            = __phys_to_pfn(0x90000000),
            .length = 0x10000,
            .type   = MT_DEVICE,
    },
	/*Timer*/
   {   
            .virtual        = IO_ADDRESS(0x90004000),
            .pfn            = __phys_to_pfn(0x90004000),
            .length = 0x10000,
            .type   = MT_DEVICE,
    },
	/*SOCP*/
    {   
            .virtual        = IO_ADDRESS(0x900a9000),
            .pfn            = __phys_to_pfn(0x900a9000),
            .length = 0x10000,
            .type   = MT_DEVICE,
    },
    /*SOC RTC*/
    {   
            .virtual        = IO_ADDRESS(RTC_PHY_BASE_ADDR),
            .pfn            = __phys_to_pfn(RTC_PHY_BASE_ADDR),
            .length = 0x10000,
            .type   = MT_DEVICE,
    },
	/*NV backup,192K*/
    {		
    	.virtual	= ECS_NV_BASE_ADDR_VIRT, /*(0xF3400000 + 0x01FB0000)*/		
    	.pfn		= __phys_to_pfn(ECS_NV_BASE_ADDR),		
    	.length		= ECS_NV_SIZE,		
    	.type		= MT_MEMORY_NONCACHED_READ_ONLY,	
    },
		
    /*TTF memory,20MB*/
    {
            .virtual        = ECS_TTF_BASE_ADDR_VIRT,
            .pfn            = __phys_to_pfn(ECS_TTF_BASE_ADDR),
            .length = ECS_TTF_SIZE,
    #if (FEATURE_TTFMEM_CACHE == FEATURE_ON)
            .type   = MT_MEMORY,
	#else
            .type   = MT_DEVICE,
    #endif
    },
    /*DICC,128KB*/
    {
            .virtual        = ECS_TTF_DICC_ADDR_VIRT,
            .pfn            = __phys_to_pfn(ECS_TTF_DICC_ADDR),
            .length = ECS_TTF_DICC_SIZE,
            .type   = MT_DEVICE,
    },
    /*exc,640k*/
	{
		.virtual        = PBXA9_DRAM_EXC_SHARE_VIRT, /*(0xF3400000 + 0x02000000)*/
		.pfn            = __phys_to_pfn(PBXA9_DRAM_EXC_SHARE_PHYS),
		.length = PBXA9_DRAM_EXC_SHARE_SIZE,
		.type   = MT_DEVICE,
	},
    
#ifdef CONFIG_BALONG_MULTICORE
		
	{
		.virtual	= IO_ADDRESS(AXI_MEM_BASE_ADDR),
		.pfn		= __phys_to_pfn(AXI_MEM_BASE_ADDR),
		.length		= 0x10000,
        .type       = MT_DEVICE,
	},
	{
		.virtual	= IO_ADDRESS(AXI_MEM_BASE_ADDR+0x10000),
		.pfn		= __phys_to_pfn(AXI_MEM_BASE_ADDR+0x10000),
		.length		= 0x7800,
        .type       = MT_DEEPSLEEP,
	},
	{
		.virtual	= IO_ADDRESS(AXI_MEM_BASE_ADDR+0x17800),
		.pfn		= __phys_to_pfn(AXI_MEM_BASE_ADDR+0x17800),
		.length		= 0x8800,
        .type       = MT_DEVICE,
	},
	{
		.virtual	= IO_ADDRESS(AXI_MEM_BASE_ADDR+0x20000),
		.pfn		= __phys_to_pfn(AXI_MEM_BASE_ADDR+0x20000),
		.length		= 0x7800,
        .type       = MT_DEEPSLEEP,
	},
	{
		.virtual	= IO_ADDRESS(AXI_MEM_BASE_ADDR+0x27800),
		.pfn		= __phys_to_pfn(AXI_MEM_BASE_ADDR+0x27800),
		.length		= 0x8800,
        .type       = MT_DEVICE,
	},
	{
		.virtual	= IO_ADDRESS(AXI_MEM_BASE_ADDR+0x30000),
		.pfn		= __phys_to_pfn(AXI_MEM_BASE_ADDR+0x30000),
		.length 	= 0x10000,
		.type		= MT_DEVICE,
	},
	{
		.virtual	= IO_ADDRESS(PBXA9_DRAM_SHARE_PHYS),
		.pfn		= __phys_to_pfn(PBXA9_DRAM_SHARE_PHYS),
		.length		= PBXA9_DRAM_SHARE_SIZE,
		.type		= MT_DEVICE,
	},
	
 #endif
};
int io_address_flag_print = 0;
EXPORT_SYMBOL(io_address_flag_print);

static void __init pv500v1_map_io(void)
{
	iotable_init(pv500v1_io_desc, ARRAY_SIZE(pv500v1_io_desc));
    io_address_flag_print = 1;
}

static void __init pv500v1_gic_init(void)
{
    gic_init(0, 29, __io_address(PBXA9_GIC_DIST_CONTROL),
    	 __io_address(PBXA9_GIC_CPU_CONTROL));
}

void pv500v1_timer_init(void)
{
    unsigned int timer_irq = INT_LVL_TIMER_14;
    balong_timer_init(timer_irq);
}

static struct sys_timer pv500v1_timer = {
	.init		= pv500v1_timer_init,
};
#if 0
extern struct platform_device pv500v1_device_nand;
static struct platform_device *pv500v1_devices[] __initdata = {
	&pv500v1_device_nand,
};
#endif
extern void balong_nand_set_platdata(struct balongv7r1_platform_nand *nand);
extern struct balongv7r1_platform_nand smdk_nand_info;

static void __init pv500v1_init(void)
{
    int i;
#if 0
	platform_add_devices(pv500v1_devices, ARRAY_SIZE(pv500v1_devices));
	balong_nand_set_platdata(&smdk_nand_info);
#endif
 	if(PRT_FLAG_EN_MAGIC_A == *(BSP_U32*)MEMORY_AXI_PRT_FLAG_ADDR)
    {
        uart3_device.dev.init_name = "dev:uart0";
		uart3_device.res.start = UART0_BASE;
		uart3_device.res.end = UART0_BASE + SZ_4K - 1;
		uart3_device.irq[0]	= INT_LVL_UART_0;
    }
	else
	{
		uart3_device.irq[0]	= INT_LVL_UART_3;
	}
	for (i = 0; i < ARRAY_SIZE(amba_devs); i++)
    {
		struct amba_device *d = amba_devs[i];
		amba_device_register(d, &iomem_resource);
	}
	for (i = 0; i < ARRAY_SIZE(pv500_lm_dev); i++)
	{
		struct lm_device *d = pv500_lm_dev[i];
		lm_device_register(d);
	}
}

void pv500v1_fixup(struct machine_desc *mdesc, struct tag *tags, char **from,
		    struct meminfo *meminfo)
{
}

MACHINE_START(BALONG_V100R001, "Hisilicon Balong")
        .phys_io		= UART3_BASE,
        .io_pg_offst	= ((IO_ADDRESS(UART3_BASE)) >> 18) & 0xfffc,
	.boot_params	= GLOBAL_MEM_ACORE_BASE_ADDR + 0x00000100,
	.fixup		= pv500v1_fixup,
	.map_io		= pv500v1_map_io,
	.init_irq		= pv500v1_gic_init,
	.timer		= &pv500v1_timer,
	.init_machine	= pv500v1_init,
MACHINE_END

