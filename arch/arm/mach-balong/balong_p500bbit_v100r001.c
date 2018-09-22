#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/sysdev.h>
#include <linux/amba/bus.h>
#include <linux/io.h>
#include <linux/mm.h>

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

static struct amba_device uart0_device =
{
	.dev = 
    {
		.coherent_dma_mask = ~0,
		.init_name = "dev:uart0",
		.platform_data = NULL,
	},
	.res =
	{
		.start	= PV500V1_UART0_BASE,
		.end	= PV500V1_UART0_BASE + SZ_4K - 1,
		.flags	= IORESOURCE_MEM,
	},
	.dma_mask	= ~0,
	.periphid   = 0x000c21c0,
	.irq		= { INT_LVL_UART_0, NO_IRQ },
};

static struct amba_device *amba_devs[] __initdata =
{
	&uart0_device,
};

static struct lm_device pv500_usb_otg={
	.dev={
		.init_name = "synopsys_dwc_otg",
	},
	.resource.start = REG_BASE_USB2DVC,
	.resource.end = REG_BASE_USB2DVC + REG_USB2DVC_IOSIZE -1,
	.resource.flags = IORESOURCE_MEM,
	.irq = IRQ_PV500_USB_OTG,
	.id = -1,
};

static struct lm_device *pv500_lm_dev[] __initdata = {
	&pv500_usb_otg,
};

static struct map_desc pv500v1_io_desc[] __initdata = {
	{
		.virtual	= IO_ADDRESS(PV500V1_TIMER_BASE),
		.pfn		= __phys_to_pfn(PV500V1_TIMER_BASE),
		.length		= PV500V1_CLK_REGS_SIZE,
		.type		= MT_DEVICE,
	},
	{
		.virtual	= IO_ADDRESS(FPGA_TIMER6_BASE),
		.pfn		= __phys_to_pfn(FPGA_TIMER6_BASE),
		.length		= FPGA_TIMER6_SIZE,
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
	/*wdt*/
	{
		.virtual	= IO_ADDRESS(PBXA9_WDT_BASE),
		.pfn		= __phys_to_pfn(PBXA9_WDT_BASE),
		.length		= PBXA9_WDT_SIZE,
		.type		= MT_DEVICE,
	},
	/*sys control reg*/
	{
		.virtual	= IO_ADDRESS(V3R2_SC_BASE),
		.pfn		= __phys_to_pfn(V3R2_SC_BASE),
		.length		= V3R2_SC_SIZE,
		.type		= MT_DEVICE,
	},
	
#ifdef CONFIG_DEBUG_LL
	{
		.virtual	= IO_ADDRESS(PV500V1_UART0_BASE),
		.pfn		= __phys_to_pfn(PV500V1_UART0_BASE),
		.length		= PV500V1_UART_REGS_SIZE,
		.type		= MT_DEVICE,
	},
#endif
#if 0
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
		.virtual	= IO_ADDRESS(V3R2_SC_BASE),
		.pfn		= __phys_to_pfn(V3R2_SC_BASE),
		.length		= V3R2_SC_SIZE,
		.type		= MT_DEVICE,
	},
#endif
	{   
            .virtual        = IO_ADDRESS(0x5f0c0000),
            .pfn            = __phys_to_pfn(0x5f0c0000),
            .length = 0x10000,
            .type   = MT_DEVICE,
    },
    {   
            .virtual        = IO_ADDRESS(0x90002000),
            .pfn            = __phys_to_pfn(0x90002000),
            .length = 0x10000,
            .type   = MT_DEVICE,
    },
    {   
            .virtual        = IO_ADDRESS(0x5f057000),
            .pfn            = __phys_to_pfn(0x5f057000),
            .length = 0x10000,
            .type   = MT_DEVICE,
    },
    {   
            .virtual        = IO_ADDRESS(FPGA_SOC_RESET_BASE),
            .pfn            = __phys_to_pfn(FPGA_SOC_RESET_BASE),
            .length = FPGA_SOC_RESET_SIZE,
            .type   = MT_DEVICE,
    },
    
    {   
            .virtual        = IO_ADDRESS(0x900a9000),
            .pfn            = __phys_to_pfn(0x900a9000),
            .length = 0x10000,
            .type   = MT_DEVICE,
    },

    /*20111231,PS¨¨¨²o?¦Ì¡Â??MemoryMap,c61362 start */
    {   
            .virtual        = IO_ADDRESS(IPF_REG_BASE_ADDR),
            .pfn            = __phys_to_pfn(IPF_REG_BASE_ADDR),
            .length = IPF_REG_SIZE,
            .type   = MT_DEVICE,
    },
    
    {
            .virtual        = ECS_TTF_BASE_ADDR - ECS_MDDR_RESERVE_BASE_ADDR + ECS_MDDR_RESERVE_BASE_ADDR_VIRT,
            .pfn            = __phys_to_pfn(ECS_TTF_BASE_ADDR),
            .length = ECS_TTF_SIZE,
            .type   = MT_DEVICE,
    },

    {		
    	.virtual	= ECS_NV_BASE_ADDR_VIRT,		
    	.pfn		= __phys_to_pfn(ECS_NV_BASE_ADDR),		
    	.length		= ECS_NV_SIZE,		
    	.type		= MT_MEMORY_NONCACHED_READ_ONLY,	
    },

    {		
    	.virtual	= ECS_TTF_DICC_ADDR_VIRT,		
    	.pfn		= __phys_to_pfn(ECS_TTF_DICC_ADDR),		
    	.length		= ECS_TTF_DICC_SIZE,		
    	.type		= MT_DEVICE,	
    },
	/*20111231,PS¨¨¨²o?¦Ì¡Â??MemoryMap,c61362 end*/
    
#ifdef CONFIG_BALONG_MULTICORE
        {
		.virtual	= IO_ADDRESS(AXI_MEM_BASE_ADDR),
		.pfn		= __phys_to_pfn(AXI_MEM_BASE_ADDR),
		.length		= AXI_MEM_SIZE,
                     .type           = MT_DEVICE,
	},
	{
		.virtual	= IO_ADDRESS(PBXA9_DRAM_SHARE_PHYS),
		.pfn		= __phys_to_pfn(PBXA9_DRAM_SHARE_PHYS),
		.length		= PBXA9_DRAM_SHARE_SIZE,
		.type		= MT_DEVICE,
	},
	
	{
	.virtual	= PBXA9_DRAM_EXC_SHARE_VIRT,
	.pfn		= __phys_to_pfn(PBXA9_DRAM_EXC_SHARE_PHYS),
	.length		= PBXA9_DRAM_EXC_SHARE_SIZE,
	.type		= MT_DEVICE,
    },
	
	
 #endif
};

static void __init pv500v1_map_io(void)
{
	iotable_init(pv500v1_io_desc, ARRAY_SIZE(pv500v1_io_desc));
}

static void __init pv500v1_gic_init(void)
{
    gic_init(0, 29, __io_address(PBXA9_GIC_DIST_CONTROL),
    	 __io_address(PBXA9_GIC_CPU_CONTROL));
}

void pv500v1_timer_init(void)
{
    unsigned int timer_irq = INT_LVL_TIMER_4567;
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
	uart0_device.irq[0]	= INT_LVL_UART_0;
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
	.boot_params	= PHYS_OFFSET + 0x00000100,
	.fixup		= pv500v1_fixup,
	.map_io		= pv500v1_map_io,
	.init_irq	= pv500v1_gic_init,
	.timer		= &pv500v1_timer,
	.init_machine	= pv500v1_init,
MACHINE_END

