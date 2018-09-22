#ifndef __ASM_ARCH_HARDWARE_H
#define __ASM_ARCH_HARDWARE_H

#include <asm/sizes.h>

/* macro to get at IO space when running virtually */
#ifdef CONFIG_MMU
#define IO_ADDRESS(x)	((((x) & 0xF0000000) >> 4) | ((x) & 0x00FFFFFF) | 0xF0000000)
#else
#define IO_ADDRESS(x)        (x)
#endif
#define __io_address(n)        __io(IO_ADDRESS(n))

#endif
