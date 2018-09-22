/*
 * asm-generic/msa.h
 * Provide a generic time-of-day clock for
 * microstate accounting.
 */

#ifndef _ASM_GENERIC_MSA_H
#define _ASM_GENERIC_MSA_H

/*#include <mach/DrvInterface.h>*/
extern unsigned int BSP_GetSliceValue(BSP_VOID);
#define DRV_GET_SLICE()   BSP_GetSliceValue()

# ifdef __KERNEL__
/*
 * Every architecture is supposed to provide sched_clock, a free-running,
 * non-wrapping, per-cpu clock in nanoseconds.
 */
#  define MSA_NOW(now)  do { (now) = DRV_GET_SLICE(); } while (0)
#  define MSA_TO_NSEC(clk) (clk)
# endif

#endif /* _ASM_GENERIC_MSA_H */