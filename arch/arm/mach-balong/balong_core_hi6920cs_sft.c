#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/irq.h>
#include <linux/clocksource.h>
#include <linux/interrupt.h>
#include <linux/amba/bus.h>
#include <linux/clockchips.h>
#include <linux/platform_device.h>
#include <linux/smsc911x.h>

#include <mach/balong_v100r001.h>
#include <mach/irqs.h>
#include <mach/hardware.h>
#include <mach/clkdev.h>

#include <asm/hardware/vic.h>
#include <asm/setup.h>
#include <asm/pgtable.h>
#include <asm/clkdev.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/time.h>
#include <asm/mach/map.h>


#include "balong_core_hi6920cs_sft.h"


static irqreturn_t balong_timer_irq_handler(int irq, void *dev_id)
{
    unsigned int readValue;
    readValue = readl(__io_address(TIMER14_BASE) + CLK_REGOFF_CLEAR);
	timer_tick();
	return IRQ_HANDLED;
}

static struct irqaction balong_timer_irq = {
	.name		= "Balong systimer Tick",
	.flags		= IRQF_DISABLED | IRQF_TIMER,
	.handler	= balong_timer_irq_handler,
};

#define sysClkTicksPerSecond    100
void __init balong_timer_init(unsigned int timer_irq)
{
    unsigned int tc;
    #if 0
    *(volatile int*)0xf1001824 = 0x02010101;
    #endif
    writel(CLK_DEF_DISABLE, __io_address(TIMER14_BASE) + CLK_REGOFF_CTRL);
    tc = (19200000 / sysClkTicksPerSecond) - AMBA_RELOAD_TICKS;
    writel(tc, __io_address(TIMER14_BASE) + CLK_REGOFF_LOAD);
    writel(CLK_DEF_ENABLE, __io_address(TIMER14_BASE) + CLK_REGOFF_CTRL);
    
    
    setup_irq (INT_LVL_TIMER_14, &balong_timer_irq);
}

int clk_enable (struct clk *clk)
{
    //writel(CLK_DEF_ENABLE, __io_address(PV500V1_TIMER0_BASE) + CLK_REGOFF_CTRL);
	return 0;
}
EXPORT_SYMBOL(clk_enable);

void clk_disable (struct clk *clk)
{
    //writel(CLK_DEF_DISABLE, __io_address(CLK_DEF_DISABLE) + CLK_REGOFF_CTRL);
}
EXPORT_SYMBOL(clk_disable);

unsigned long clk_get_rate (struct clk *clk)
{
	return SYS_TIMER_CLK;
}

struct clk *clk_get(struct device *dev, const char *id)
{
	return NULL;
}

void clk_put(struct clk *clk)
{
}


