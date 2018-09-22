#include "balong_v100r001.h"
#if defined (BOARD_FPGA) && defined (VERSION_V3R2)\
   || (defined (BOARD_FPGA_P500) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
#ifndef DEBUG_LL_UART_OFFSET
#define DEBUG_LL_UART_OFFSET    0x00020000
#elif DEBUG_LL_UART_OFFSET != 0x00004000
#warning "DEBUG_LL_UART_OFFSET already defined to a different value"
#endif
#endif

#ifndef DEBUG_LL_UART_OFFSET
#error "Unknown RealView platform"
#endif

        .macro	addruart, rx, tmp
	mrc	p15, 0, \rx, c1, c0
	tst	\rx, #1			@ MMU enabled?
	ldreq	\rx,      =PV500V1_UART0_BASE
        ldrne	\rx,      =VIRT_PV500V1_UART0_BASE	@ virtual base
	.endm

#include <linux/amba/serial.h>

        .macro    senduart,rd,rx
        strb    \rd, [\rx, #0x00]
        .endm

        .macro    waituart,rd,rx
1001:        ldr    \rd, [\rx, #0x7c]
        tst    \rd, #0x01
        bne    1001b
        .endm

        .macro    busyuart,rd,rx
1001:        ldr    \rd, [\rx, #0x7c]
        tst    \rd, #0x01
        bne    1001b
        .endm


