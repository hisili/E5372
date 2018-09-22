#ifndef __ASM_ARCH_IRQS_H
#define __ASM_ARCH_IRQS_H

#include <generated/FeatureConfig.h>   /*syb*/

#define IRQ_PV500_GIC_START     32

#if defined (BOARD_SFT) && defined (VERSION_V3R2)
#define IRQ_PV500_USB_OTG       (IRQ_PV500_GIC_START + 6)
#elif (defined (BOARD_SFT) && defined (VERSION_V7R1))  \
    || (defined (BOARD_ASIC_BIGPACK) && defined (VERSION_V3R2))  \
    ||(defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS))) \
    ||(defined (BOARD_ASIC_BIGPACK) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS))) \
    ||(defined (BOARD_SFT) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
#define IRQ_PV500_USB_OTG       (IRQ_PV500_GIC_START + 6)
#elif defined (BOARD_FPGA) && defined (VERSION_V3R2)\
    ||(defined (BOARD_FPGA_P500) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
#define IRQ_PV500_USB_OTG       (IRQ_PV500_GIC_START + 12)
#endif

#if defined(FEATURE_HSIC_ON) && ((defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))) 
#define IRQ_PV500_USB_HSIC     IRQ_PV500_GIC_START + 90
#endif

/*
 * Only define NR_IRQS if less than NR_IRQS_EB
 */
#if defined (BOARD_FPGA) && defined (VERSION_V3R2)
#define NR_IRQS_PV500V1        (IRQ_PV500_GIC_START + 96)
#elif(defined (BOARD_FPGA_P500) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
#define NR_IRQS_PV500V1        (IRQ_PV500_GIC_START + 96)
#elif defined (BOARD_SFT) && defined (VERSION_V3R2)
#define NR_IRQS_PV500V1        (IRQ_PV500_GIC_START + 96)
#elif defined (BOARD_SFT) && defined (VERSION_V7R1)
#define NR_IRQS_PV500V1        (IRQ_PV500_GIC_START + 128)
#elif defined (BOARD_ASIC_BIGPACK) && defined (VERSION_V3R2)
#define NR_IRQS_PV500V1        (IRQ_PV500_GIC_START + 128)
#elif ((defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)) \
	&&(defined(BOARD_ASIC) || defined (BOARD_ASIC_BIGPACK) || defined (BOARD_SFT)))
#define NR_IRQS_PV500V1        (IRQ_PV500_GIC_START + 128)
#else
#error "Unknown architecture specification"
#endif

#undef NR_IRQS
#define NR_IRQS            NR_IRQS_PV500V1

#endif

