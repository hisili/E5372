#ifndef __ASM_ARCH_BALONG_V100_R001_H
#define __ASM_ARCH_BALONG_V100_R001_H
#if defined (BOARD_SFT) && defined (VERSION_V3R2)
#include <mach/balong_v3r2sft_v100r001.h>
#elif defined (BOARD_FPGA) && defined (VERSION_V3R2)
#include <mach/balong_p500bbit_v100r001.h>
#elif (defined (BOARD_FPGA_P500) && defined(CHIP_BB_6920ES))
#include <mach/balong_p500fpga.h>
#elif (defined (BOARD_FPGA_P500) && defined (CHIP_BB_6920CS))
#include <mach/balong_hi6920csp500.h>
#elif defined (BOARD_SFT) && defined (VERSION_V7R1)
#include <mach/balong_v7r1sft.h>
#elif defined (BOARD_ASIC_BIGPACK) && defined (VERSION_V3R2)
	#if defined (CHIP_BB_6756CS)
	#include <mach/balong_v3r2cs.h>
	#else
	#include <mach/balong_v3r2asic.h>
	#endif
#elif (defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
#include <mach/balong_v7r1asic.h>
#elif (defined (BOARD_SFT) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
#include <mach/balong_hi6920cs_sft.h>
#else
#error "there is no file included!"
#endif

#define PBXA9_SCU_BASE                      (PBXA9_PERIPHBASE + 0x0)
#define PBXA9_SCU_CTRL                      (PBXA9_SCU_BASE + 0x00)

/* MMU Control Register bit allocations */

#define MMUCR_M_ENABLE	 (1<<0)  /* MMU enable */
#define MMUCR_A_ENABLE	 (1<<1)  /* Address alignment fault enable */
#define MMUCR_C_ENABLE	 (1<<2)  /* (data) cache enable */
#define MMUCR_W_ENABLE	 (1<<3)  /* write buffer enable */
#define MMUCR_PROG32	 (1<<4)  /* PROG32 */
#define MMUCR_DATA32	 (1<<5)  /* DATA32 */
#define MMUCR_L_ENABLE	 (1<<6)  /* Late abort on earlier CPUs */
#define MMUCR_BIGEND	 (1<<7)  /* Big-endian (=1), little-endian (=0) */
#define MMUCR_SYSTEM	 (1<<8)  /* System bit, modifies MMU protections */
#define MMUCR_ROM	 (1<<9)  /* ROM bit, modifies MMU protections */
#define MMUCR_F		 (1<<10) /* Should Be Zero */
#define MMUCR_Z_ENABLE	 (1<<11) /* Branch prediction enable on 810 */
#define MMUCR_I_ENABLE	 (1<<12) /* Instruction cache enable */
#define MMUCR_V_ENABLE	 (1<<13) /* Exception vectors remap to 0xFFFF0000 */
#define MMUCR_ALTVECT    MMUCR_V_ENABLE /* alternate vector select */
#define MMUCR_RR_ENABLE	 (1<<14) /* Round robin cache replacement enable */
#define MMUCR_ROUND_ROBIN MMUCR_RR_ENABLE  /* round-robin placement */
#define MMUCR_DISABLE_TBIT   (1<<15) /* disable TBIT */
#define MMUCR_ENABLE_DTCM    (1<<16) /* Enable Data TCM */
#define MMUCR_ENABLE_ITCM    (1<<18) /* Enable Instruction TCM */
#define MMUCR_UNALIGNED_ENABLE (1<<22) /* Enable unaligned access */
#define MMUCR_EXTENDED_PAGE (1<<23)  /* Use extended PTE format */
#define MMUCR_VECTORED_INTERRUPT (1<<24) /* Enable VIC Interface */

#define MMUCR_MODE MMUCR_ROM    

#define MMU_MUST_SET_VALUE  (MMUCR_PROG32 | MMUCR_DATA32 | MMUCR_L_ENABLE | \
			     MMUCR_W_ENABLE | MMUCR_ENABLE_DTCM | \
			     MMUCR_ENABLE_ITCM | MMUCR_UNALIGNED_ENABLE | \
			     MMUCR_EXTENDED_PAGE)
#define MMU_INIT_VALUE	(MMU_MUST_SET_VALUE)		

#define ARM_IMM #
#define ARM_LOC_MASK #0x7000000     /* level of coherency mask of CLIDR */

#define _CORTEX_AR_ENTIRE_DATA_CACHE_OP(crm) \
                    \
    MRC    p15, 1, r0, c0, c0, 1    /* r0 = Cache Lvl ID register info */;\
    ANDS   r3, r0, ARM_LOC_MASK     /* get level of coherency (LoC) */;\
    MOV    r3, r3, LSR ARM_IMM 23   /* r3 = LoC << 1 */;\
    BEQ    5f            ;\
                    \
    MOV    r7, ARM_IMM 0            /* r7 = cache level << 1; start at 0 */;\
                    \
1:                    ;\
    AND    r1, r0, ARM_IMM 0x7      /* r1 = cache type(s) for this level */;\
    CMP    r1, ARM_IMM 2        ;\
    BLT    4f                       /* no data cache at this level */;\
                    \
    MCR    p15, 2, r7, c0, c0, 0    /* select the Cache Size ID register */;\
    MCR    p15, 0, r7, c7, c5, 4    /* ISB: sync change to Cache Size ID */;\
    MRC    p15, 1, r1, c0, c0, 0    /* r1 = current Cache Size ID info */;\
    AND    r2, r1, ARM_IMM 0x7      /* r2 = line length */;\
    ADD    r2, r2, ARM_IMM 4        /* add line length offset = log2(16 bytes) */;\
    LDR    r4, =0x3FF        ;\
    ANDS   r4, r4, r1, LSR ARM_IMM 3 /* r4 = (# of ways - 1); way index */;\
    CLZ    r5, r4                    /* r5 = bit position of way size increment */;\
    LDR    r6, =0x00007FFF        ;\
    ANDS    r6, r6, r1, LSR ARM_IMM 13/* r6 = (# of sets - 1); set index */;\
                    \
2:                    ;\
    MOV    r1, r4                 /* r1 = working copy of way number */;\
3:                    ;\
    ORR    r8, r7, r1, LSL r5     /* r8 = set/way operation data word: */;\
    ORR    r8, r8, r6, LSL r2     /* cache level, way and set info */;\
                    \
    MCR    p15, 0, r8, c7, crm, 2 /* dcache operation by set/way */;\
                    \
    SUBS   r1, r1, ARM_IMM 1      /* decrement the way index */;\
    BGE    3b            ;\
    SUBS   r6, r6, ARM_IMM 1      /* decrement the set index */;\
    BGE    2b            ;\
                    \
4:                    ;\
    ADD    r7, r7, ARM_IMM 2      /* increment cache index = level << 1 */;\
    CMP    r3, r7                 /* done when LoC is reached */;\
    MOVGT  r0, r0, LSR ARM_IMM 3  /* rt-align type of next cache level */;\
    BGT    1b            ;\
                    \
5:                    ;\

#endif

