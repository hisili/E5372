#ifndef __ARCH_ARM_MACH_PV500_CLOCK_H
#define __ARCH_ARM_MACH_PV500_CLOCK_H

#ifdef LUOCHUANZAO
struct clk {
	unsigned long               rate;
    char                        *name;
	const struct clk_ops        *ops;
	//void __iomem                *vcoreg;
};

#define __clk_get(clk) ({ 1; })
#define __clk_put(clk) do { } while (0)
#endif
#endif

