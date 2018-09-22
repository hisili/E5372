/*
 *  arch/arm/mach-realview/include/mach/uncompress.h
 *
 *  Copyright (C) 2003 ARM Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <mach/hardware.h>
#include <asm/mach-types.h>

#include <mach/balong_v100r001.h>

#include <generated/product_config.h>   /*syb*/

static inline unsigned long get_uart_base(void)
{
#if defined (BOARD_SFT) && defined (VERSION_V7R1)
    return UART3_BASE;
#elif defined (BOARD_ASIC_BIGPACK) && defined (VERSION_V3R2)
/*++pandong cs*/
	#if defined(CHIP_BB_6756CS)
	return PV500V1_UART0_BASE;
	#else
    return UART0_BASE;
	#endif
/*--pandong cs*/
#elif ((defined (BOARD_ASIC) || defined (BOARD_SFT)) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
#if defined(FEATURE_BOARD_STUB_BJ_UDP)
    return UART3_BASE;
#else
    return UART0_BASE;
#endif
#elif (defined (BOARD_FPGA_P500) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
    return PV500V1_UART2_BASE;    
#else
    return PV500V1_UART0_BASE;
#endif
}

#define AMBA_UART_FR(base)    (*(volatile unsigned int *)((base) + 0x7c))
#define AMBA_UART_DR(base)    (*(volatile unsigned int *)((base) + 0x00))

static inline void putc(int c)
{
    /* 只有UDP才使能串口打印 */
#if defined(FEATURE_BOARD_STUB_BJ_UDP)
    unsigned long base = get_uart_base();
    unsigned int   temp = 0;
    while (AMBA_UART_FR(base) & 0x9)
    {
        temp = AMBA_UART_DR(base);
	temp = temp -1; /*For pclint*/
    }

    while (AMBA_UART_FR(base) & 1)
        barrier();

    AMBA_UART_DR(base) = (unsigned int)c;/*For pclint*/
#endif
}

static inline void flush(void)
{
}

/*
 * nothing to do
 */
#define arch_decomp_setup()
#define arch_decomp_wdog()
