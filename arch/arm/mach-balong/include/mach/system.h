#ifndef __ASM_ARCH_SYSTEM_H
#define __ASM_ARCH_SYSTEM_H

#include <mach/hardware.h>
#include <mach/platform.h>

void (*realview_reset)(char mode);

void arch_idle(void);

static inline void arch_reset(char mode, const char *cmd)
{
}

#endif

