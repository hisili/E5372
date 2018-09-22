#ifndef __MACH_IRQS_EB_H
#define __MACH_IRQS_EB_H

#define IRQ_PV500_GIC_START    32
#define IRQ_PV500_USB_OTG      IRQ_PV500_GIC_START + 12
/*
 * Only define NR_IRQS if less than NR_IRQS_EB
 */
#define NR_IRQS_PV500V1        (IRQ_PV500_GIC_START + 96)

#if defined(CONFIG_ARCH_SOC_VERSION_V3R2_C00) && (!defined(NR_IRQS))
#undef NR_IRQS
#define NR_IRQS            NR_IRQS_PV500V1
#endif

#endif    /* __MACH_IRQS_EB_H */
