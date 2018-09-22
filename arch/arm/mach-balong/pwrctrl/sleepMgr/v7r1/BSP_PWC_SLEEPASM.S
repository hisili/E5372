/******************************************************************
 * Copyright (C), 2005-2011, HUAWEI Tech. Co., Ltd.
 *
 * File name: 
 *
 * Description:
 *     power ctrl file.
 *
 * Date:       2012.01.01
 * Version:    v1.0
 *
 *
 * History:
 * Date:       2012.01.01
 * Discribe:   Initial
 *******************************************************************/

#include <linux/linkage.h>
#include <asm/assembler.h>
#include <mach/hardware.h>
#include <mach/balong_v100r001.h>
#include <mach/pwrctrl/v7r1/BSP_PWC_SOCREG.h>
#include <mach/pwrctrl/v7r1/BSP_PWC_SLEEPASM.h>
#include <asm/domain.h>
#include <asm/system.h>
#include <asm/pgtable.h>

_ARM_FUNCTION_CALLED_FROM_C(PWRCTRL_AsmWfi)

	DSB
	ISB
	
	wfi

	ISB
	MCR p15,0,r0,c7,c5,6        // invalidate branch predictor (this core)

	NOP
	NOP
	NOP
	NOP
	NOP

	BX    lr
_ARM_FUNCTION_CALLED_FROM_C(PWRCTRL_IntLock)
    MRS r1,cpsr
    AND r0, r1, #I_BIT
    ORR r1, r1, #I_BIT
    MSR cpsr, r1
    MOV pc, lr
    
    
_ARM_FUNCTION_CALLED_FROM_C(PWRCTRL_IntUnlock)
    MRS r1,cpsr
    BIC r1, r1, #I_BIT
    AND r0, r0, #I_BIT
    ORR r1, r1, r0
    MSR cpsr, r1
    MOV pc, lr

_ARM_FUNCTION_CALLED_FROM_C(PWRCTRL_AsmAcpuCodeBegin)

	LDR r0,=0x2ffe0008
	MOV pc,r0
	
#ifdef PWRCTRL_TIMER_COUNT
    LDR	r2,=SYS_REG_VIRT_TO_PHY(ARM_SOC_TIMER_COUNT)        
    LDR	r1,[r2]
    LDR	r2, =AXI_VIRT_TO_PHY(STORE_RESTORE_TIMER0)
    STR	r1, [r2]
#endif

	MCR 	p15,0,r0,c7,c5,0		// invalidate Icache to PoU (this core)
	MCR 	p15,0,r0,c7,c5,6		// invalidate branch predictor (this core)
	ISB

	/* disable L1 D-caches */	   
	MRC	  	p15, 0, r1, c1, c0, 0 	/* Read control register into r1 */
	BIC	  	r1, r1, #0x4			/* Clear L1 D-cache enable */
	MCR	  	p15, 0, r1, c1, c0, 0 	/* Write control register */

	/* disable and clear I-cache */
	MRC	  	p15 , 0, r2, c1, c0, 0	  /* Read control register */
	BIC	  	r2, r2, #0x1000			  /* Disable I-cache */
	MCR	  	p15 , 0, r2, c1, c0, 0	  /* Write control register */
	ISB 							

	/*恢复大小端*/
	MRS    	r3, spsr
	LDR    	r2,=0xFFFFFDFF
	AND    	r3,r3,r2
	MSR    	spsr_c, r3

#ifdef 	PWRCTRL_DEBUG_FLAG
	/*debug*/
	LDR 	 r2,=AXI_VIRT_TO_PHY(PWRCTRL_ASM_DEG_FLAG1)
	LDR 	 r1,=PWRCTRL_WAKEUP
	STR 	 r1, [r2]		 
#endif

#ifdef PWRCTRL_TIMER_COUNT
    LDR		r2,=SYS_REG_VIRT_TO_PHY(ARM_SOC_TIMER_COUNT)       
    LDR		r1,[r2]
    LDR		r2, =AXI_VIRT_TO_PHY(STORE_RESTORE_TIMER1)
    STR		r1, [r2]
#endif

/************************************************************************
;* FUNCTION
;*				 pwrctrl_asm_restore_arm_co_register
;* DESCRIPTION
;*				 Swich to Normal.
;* INPUTS
;*				 NONE
;* OUTPUTS
;*				 NONE
;*************************************************************************/
_ARM_FUNCTION_CALLED_FROM_C(pwrctrl_asm_restore_mmu_register)

#ifdef PWRCTRL_TIMER_COUNT
    LDR	r2,=SYS_REG_VIRT_TO_PHY(ARM_SOC_TIMER_COUNT)        
    LDR	r1,[r2]
    LDR	r2, =AXI_VIRT_TO_PHY(STORE_RESTORE_TIMER2)
    STR	r1,[r2]
#endif 

	/*IF L1 Dcache has no retention,invalidate L1 by set/way*/
	MOV  r1, #0
	MCR  p15,2,r1,c0,c0,0  		// clear the CSSELR (select L1 Dcache)
	ISB
	MRC  p15,1,r7,c0,c0,0	    //read CCSIDR for L1 cache parameters

	MOV  r3,r7					   // extract the CCSIDR.LineSize bit field
	AND r3,r3,#0x7
	ADD  r3, r3, #4
	/*extract the CCSIDR.NSETS bit field*/
	MOV    r4,r7
	LDR    r1, =0x7FFF
	AND  r4, r1, r4, LSR #13 
	/*extract the CCSIDR.ASSOC bit field*/
	MOV  r5,r7
	LDR  r1, =0x3ff
	AND   r5, r1, r5, LSR #3

	CMP 	r5,#3		   		// Cortex-A9 optimisation (known size)
	CMPEQ	r3,#5
	BNE 	label_generic	   

	LDR    r1,=0x40000000
	ADD   r2, r1, r4, LSL #5
	LDR    r1,=0x80000000
	ADD   r3, r1, r4, LSL #5
	LDR    r1,=0xC0000000
	ADD   r4, r1, r4, LSL #5
	MOV   r1,r4,LSL #5

label_optloop:
	MCR 	p15,0,r1,c7,c6,2
	MCR 	p15,0,r2,c7,c6,2
	MCR 	p15,0,r3,c7,c6,2
	MCR 	p15,0,r4,c7,c6,2
	SUBS	r1,r1,#0x20
	SUB 	r2,r2,#0x20
	SUB 	r3,r3,#0x20
	SUB 	r4,r4,#0x20
	BPL 	label_optloop

label_generic:
	CLZ r6, r5		   			// r6 is the bit position of the way size
				   				// increment
label_loopl:
	MOV r2,r5	   				//r2 working copy of the max way size
		   						// (right aligned)
label_loop2:
	MOV   r7,r2,LSL r6
	MOV   r1,r4,LSL r3	
	ORR r7,r7,r1	   			// factor in the index number
	MCR p15,0,r7,c7,c14,2		// DCxxSW cache operation
	SUBS	r2,r2,#1	   		// decrement the way number
	BGE label_loop2
	SUBS	r4,r4,#1	   		// decrement the index
	BGE label_loopl
label_end:
	DSB

	LDR    r0, =AXI_VIRT_TO_PHY(STORE_ADDR_MMU_REG);/* Restore MMU Regist */
	LDR    r0,[r0]	

	LDMDB	r0!, {r3 - r7}	 
	MCR p15, 0, r3, c12, c0, 0	   // VBAR
	MCR p15, 0, r4, c13, c0, 1	   // CONTEXTIDR
	MCR p15, 0, r5, c13, c0, 2	   // TPIDRURW
	MCR p15, 0, r6, c13, c0, 3	   // TPIDRURO
	MCR p15, 0, r7, c13, c0, 4	  // TPIDRPRW

	LDMDB	r0!, {r4 - r7}			
	MCR p15, 0, r4, c3, c0, 0	   // DACR
	MCR p15, 0, r5, c7, c4, 0	   // PAR
	MCR p15, 0, r6, c10, c2, 0	   // PRRR
	MCR p15, 0, r7, c10, c2, 1	   // NMRR

	//resume TTB register
	LDMDB	r0!, {r5 - r7}	   
	MCR p15, 0, r5, c2, c0, 0	   // TTBR0 
	MCR p15, 0, r6, c2, c0, 1	   // TTBR1
	MCR p15, 0, r7, c2, c0, 2	  //TTBCR
    
	LDR r0,=AXI_VIRT_TO_PHY(STORE_CTRL_REG_BASE+0x4)
	@current mode registers reload
	LDMIA	r0!, {r1-r9, r11}

	@R0  = save memory address
	@R1  = control register value
	@R2  = domain access register value
	@R3  = identity first level descriptor address
	@R4  = direct mapping first level descriptor
	@R5  = identity mapping first level descriptor
	@R6  = TTB register value
	@R7  = PID register value
	@R8  = disabmmu physical address
	@R9  = boundary address

	@write domain access for getting the domain access right
	MOV    r10, #DM_ACC_UNCTRL
	MCR    p15, 0, r10, c3, c0, 0	

	@resume TLB register
	MCR    p15, 0, r6, c2, c0, 0	

	@resume PID register
	MCR    p15, 0, r7, c13, c0, 0	

	@clean, flush, invalidate the cache
	MOV    r7, #0

	@invalidate Icache
	MCR    p15, 0, r7, c7, c5, 0

	@drain WB
	MCR  p15, 0, r7, c7, c10, 4

	@invalidate I & D TLBs
	MCR  p15, 0, r7, c8, c7, 0

	DSB
	ISB

	@resume control register,
	@if mmu configed enable then it will be enabled
	MCR  p15, 0, r1, c1, c0, 0
	DSB
	ISB
	
	LDR r0,=STORE_CTRL_REG_BASE
	@current mode registers reload
	LDMIA	r0!, {sp}
	
	@load virt boundary address to pc
	MOV 	pc, r9

boundary:

    @resume the direct mapping first level descriptor
    STR     r4, [r3] 

	@resume domain access register
	MCR 	p15, 0, r2, c3, c0, 0


#ifdef PWRCTRL_TIMER_COUNT
    LDR	r2,=ARM_SOC_TIMER_COUNT       
    LDR	r1,[r2]
    LDR	r2, =STORE_RESTORE_TIMER3
    STR	r1, [r2]
#endif

/************************************************************************
;* FUNCTION
;*				 pwrctrl_asm_restore_gic_register
;* DESCRIPTION
;*				 Swich to Normal.
;* INPUTS
;*				 NONE
;* OUTPUTS
;*				 NONE
;*************************************************************************/
_ARM_FUNCTION_CALLED_FROM_C(pwrctrl_asm_restore_gic_register)


	LDR r0, =STORE_ADDR_GIC

	LDR r1, =CARM_GIC_CPUPMR
	LDR    r2,[r0,#0x04]
	STR    r2,[r1]

	LDR r1, =CARM_GIC_CPUBPR
	LDR    r2,[r0,#0x08]
	STR    r2,[r1]

	LDR r1, =CARM_GIC_ICABPR
	LDR    r2,[r0,#0x14]
	STR    r2,[r1]
        

	LDR r1, =CARM_GIC_ICDISR
	LDR    r2,[r0,#0x1C]
	STR    r2,[r1]
	                
	LDR r1, =CARM_GIC_ICDISR1
	LDR    r2,[r0,#0x48]
	STR    r2,[r1]

	LDR	 r2,[r0,#0x4c]
	STR	 r2,[r1,#0x04]

	LDR	 r2,[r0,#0x50]
	STR	 r2,[r1,#0x08]

	LDR	 r2,[r0,#0x54]
	STR	 r2,[r1,#0x0c]

	LDR	 r2,[r0,#0x58]	
	STR	 r2,[r1,#0x10]

	LDR r1, =CARM_GIC_ICDIPR
	LDR	 r2,[r0,#0x5c]
	STR	 r2,[r1]

	LDR	 r2,[r0,#0x60]
	STR	 r2,[r1,#0x04]

	LDR	 r2,[r0,#0x64]
	STR	 r2,[r1,#0x08]

	LDR	 r2,[r0,#0x68]
	STR	 r2,[r1,#0x0c]

	LDR	 r2,[r0,#0x6c]
	STR	 r2,[r1,#0x10]

	LDR	 r2,[r0,#0x70]
	STR	 r2,[r1,#0x14]

	LDR	 r2,[r0,#0x74]
	STR	 r2,[r1,#0x18]

	LDR	 r2,[r0,#0x78]
	STR	 r2,[r1,#0x1c]

	LDR	 r2,[r0,#0x7c]
	STR	 r2,[r1,#0x20]

	LDR	 r2,[r0,#0x80]
	STR	 r2,[r1,#0x24]

	LDR	 r2,[r0,#0x84]
	STR	 r2,[r1,#0x28]

	LDR	 r2,[r0,#0x88]
	STR	 r2,[r1,#0x2c]

	LDR	 r2,[r0,#0x90]
	STR	 r2,[r1,#0x30]

	LDR	 r2,[r0,#0x94]
	STR	 r2,[r1,#0x34]

	LDR	 r2,[r0,#0x98]
	STR	 r2,[r1,#0x38]

	LDR	 r2,[r0,#0x9c]
	STR	 r2,[r1,#0x3c]

	LDR	 r2,[r0,#0xa0]
	STR	 r2,[r1,#0x40]

	LDR	 r2,[r0,#0xa4]
	STR	 r2,[r1,#0x44]

	LDR	 r2,[r0,#0xa8]
	STR	 r2,[r1,#0x48]

	LDR	 r2,[r0,#0xac]
	STR	 r2,[r1,#0x4c]

	LDR	 r2,[r0,#0xb0]
	STR	 r2,[r1,#0x50]

	LDR	 r2,[r0,#0xb4]
	STR	 r2,[r1,#0x54]

	LDR	 r2,[r0,#0xb8]
	STR	 r2,[r1,#0x58]

	LDR	 r2,[r0,#0xbc]
	STR	 r2,[r1,#0x5c]

	LDR	 r2,[r0,#0xc0]
	STR	 r2,[r1,#0x60]

	LDR	 r2,[r0,#0xc4]
	STR	 r2,[r1,#0x64]

	LDR	 r2,[r0,#0xc8]
	STR	 r2,[r1,#0x68]

	LDR	 r2,[r0,#0xcc]
	STR	 r2,[r1,#0x6c]

	LDR	 r2,[r0,#0xd0]
	STR	 r2,[r1,#0x70]

	LDR	 r2,[r0,#0xd4]
	STR	 r2,[r1,#0x74]

	LDR	 r2,[r0,#0xd8]
	STR	 r2,[r1,#0x78]

	LDR	 r2,[r0,#0xdc]
	STR	 r2,[r1,#0x7c]

	LDR	 r2,[r0,#0xe0]
	STR	 r2,[r1,#0x80]

	LDR	 r2,[r0,#0xe4]
	STR	 r2,[r1,#0x84]

	LDR	 r2,[r0,#0xe8]
	STR	 r2,[r1,#0x88]

	LDR	 r2,[r0,#0xec]
	STR	 r2,[r1,#0x8c]

	LDR	 r2,[r0,#0xf0]
	STR	 r2,[r1,#0x90]

	LDR	 r2,[r0,#0xf4]
	STR	 r2,[r1,#0x94]

	LDR	 r2,[r0,#0xf8]
	STR	 r2,[r1,#0x98]

	LDR	 r2,[r0,#0xfc]
	STR	 r2,[r1,#0x9c]

	LDR	 r2,[r0,#0x100]
	STR	 r2,[r1,#0xa0]

	LDR r1, =CARM_GIC_ICDICFR
	LDR	 r2,[r0,#0x104]
	STR	 r2,[r1,#0x10]

	LDR r1, =CARM_GIC_ICDSGIR
	LDR	 r2,[r0,#0x108]
	STR	 r2,[r1]
    
	LDR    r1, =CARM_GIC_CPUICR
	LDR    r2,[r0]
	STR    r2,[r1]

	LDR    r1, =CARM_GIC_ICDDCR
	LDR    r2,[r0,#0x18]
	STR    r2,[r1] 
	/*debug*/
#ifdef 	PWRCTRL_DEBUG_FLAG
	LDR	  r2,=PWRCTRL_ASM_DEG_FLAG1
	LDR	  r1,=PWRCTRL_RESTORE_GIC_REG_END
	STR	  r1, [r2]
#endif

/************************************************************************
;* FUNCTION
;*				 pwrctrl_asm_restore_arm_co_register
;* DESCRIPTION
;*				 Swich to Normal.
;* INPUTS
;*				 NONE
;* OUTPUTS
;*				 NONE
;*************************************************************************/
_ARM_FUNCTION_CALLED_FROM_C(pwrctrl_asm_restore_arm_co_register)

	LDR    r0, =STORE_ADDR_ARM_REG	  ;/* Restore Arm Regist */
	LDR    r0,[r0]	

	/*debug*/
#ifdef 	PWRCTRL_DEBUG_FLAG
	LDR 	 r2,=PWRCTRL_ASM_DEG_FLAG1
	LDR 	 r1,=PWRCTRL_RESTORE_CO_REG
	STR 	 r1, [r2]
#endif

	ISB
	/* Resume co-cpu register */
	LDMDB	r0!, {r1-r5}
	MCR    p15, 2, r1, c0, c0, 0
	ISB
	MCR    p15, 0, r2, c1, c0, 1
	ISB
	MCR    p15, 0, r3, c1, c0, 0
	ISB
	MCR    p15, 0, r4, c1, c0, 2 
	ISB
	MCR    p15, 0, r5, c2, c0, 0

	/*resume fiq mode registers*/
	/*enter fiq mode, no interrupts*/
	MRS 	r2, cpsr
	BIC 	r2, r2, #MODE_MASK		
	ORR 	r2, r2, #MODE_FIQ32 | I_BIT | F_BIT
	MSR 	cpsr_c, r2
	LDMDB	r0!, {r1, r8-r14}
	MSR 	spsr_cxsf, r1

	/*resume irq mode registers*/
	/*enter irq mode, no interrupts*/
	MRS 	r2, cpsr
	BIC 	r2, r2, #MODE_MASK		
	ORR 	r2, r2, #MODE_IRQ32 | I_BIT | F_BIT
	MSR 	cpsr_c, r2
	LDMDB	r0!, {r1, r13, r14}
	MSR 	spsr_cxsf, r1

	/*resume undefine mode registers*/
	/*enter undefine mode, no interrupts*/
	MRS 	r2, cpsr
	BIC 	r2, r2, #MODE_MASK		
	ORR 	r2, r2, #MODE_UNDEF32 | I_BIT | F_BIT
	MSR 	cpsr_c, r2
	LDMDB	r0!, {r1, r13, r14}
	MSR 	spsr_cxsf, r1

	/*resume abort mode registers*/
	/*enter abort mode, no interrupts*/
	MRS 	r2, cpsr
	BIC 	r2, r2, #MODE_MASK		
	ORR 	r2, r2, #MODE_ABORT32 | I_BIT | F_BIT
	MSR 	cpsr_c, r2
	LDMDB	r0!, {r1, r13, r14}
	MSR 	spsr_cxsf ,r1

	/*resume system mode registers*/
	/*enter system mode, no interrupts*/
	MRS 	r2, cpsr
	BIC 	r2, r2, #MODE_MASK		
	ORR 	r2, r2, #MODE_SYSTEM32 | I_BIT | F_BIT
	MSR 	cpsr_c, r2
	LDMDB	r0!, {r13, r14}

	/*resume svc mode registers*/
	/*enter svc mode, no interrupts*/
	/*B  .*/
	MRS 	r2, cpsr
	BIC 	r2, r2, #MODE_MASK		
	ORR 	r2, r2, #MODE_SVC32 | I_BIT | F_BIT
	MSR 	cpsr_c, r2
	LDMDB	r0!, {r1-r2}
	NOP
	MSR 	cpsr_cxsf, r2
	MSR 	spsr_cxsf, r1

	/*debug*/
#ifdef 	PWRCTRL_DEBUG_FLAG
	LDR 	 r2,=PWRCTRL_ASM_DEG_FLAG1
	LDR 	 r1,=PWRCTRL_RESTORE_GIC_REG
	STR 	 r1, [r2]
#endif

#ifdef PWRCTRL_TIMER_COUNT
	LDR r2,=ARM_SOC_TIMER_COUNT 	   
	LDR r1,[r2]
	LDR r2, =STORE_RESTORE_TIMER5
	STR r1, [r2]
#endif

	LDR    r0, =STORE_ADDR_ARM_REG_SLEEP    ;/* Restore Arm Regist */
	LDR    r0,[r0]  

	LDMDB   r0!, {r1-r13,pc}

/**********************END **********************************************/

/************************************************************************
;* FUNCTION
;*               PWRCTRL_AsmAcpuMemoryCopy
;* DESCRIPTION
;*               Copy deepsleep code to sram0.
;* INPUTS
;*               NONE
;* OUTPUTS
;*               NONE
;*************************************************************************/
_ARM_FUNCTION_CALLED_FROM_C(PWRCTRL_AsmAcpuMemoryCopy)

    STMFD  sp!, {r0-r10, r14}
    ADRL   r0, PWRCTRL_AsmAcpuCodeBegin
    LDR    r1, =ISRAM0_DEEPSLEEP_ADDR
    LDR    r2, =pwrctrl_asm_sleep_end
    ADD    r2,r2,#PWR_ASM_CODE_COPY_OFFSET
CopyLoop1:
    LDMIA  r0!, {r3-r10}
    STMIA  r1!, {r3-r10}
    CMP    r0,      r2
    BCC    CopyLoop1
    
    LDMFD  sp!, {r0-r10, pc}

_ARM_FUNCTION_CALLED_FROM_C(PWRCTRL_AsmAcpuSleep)     

    LDR    r0, =STORE_ADDR_ARM_REG_SLEEP
    ADD    r0,r0,#0x04
    STMIA  r0!, {r1-r13,r14}
    LDR    r2, =STORE_ADDR_ARM_REG_SLEEP
    STR    r0,[r2]

	LDR   r1,=ISRAM0_DEEPSLEEP_ADDR
    LDR   r2,=PWRCTRL_AsmAcpuCodeBegin
    LDR   r3,=pwrctrl_asm_bak_arm_co_register
    SUB   r3,r3,r2
    ADD   r1,r1,r3   
    MOV   PC,r1 

/************************************************************************/
/*deepsleep entry code in SARM0*/

/************************************************************************
;* FUNCTION
;*               pwrctrl_asm_bak_arm_co_register
;* DESCRIPTION
;*               bak arm and co cpu register to sram0.
;* INPUTS
;*               NONE
;* OUTPUTS
;*               NONE
;*************************************************************************/
_ARM_FUNCTION_CALLED_FROM_C(pwrctrl_asm_bak_arm_co_register)

    /* save arm register in DSRAM0 */
    
    LDR    r0, =STORE_ADDR_ARM_REG
    ADD    r0,r0,#0x04   
    
    MRS     r2, cpsr
    BIC     r2, r2, #MODE_MASK      
    ORR     r2, r2, #MODE_SVC32 | I_BIT | F_BIT
    MSR     cpsr_c, r2
    MRS     r1, spsr
    MRS     r2, cpsr
    STMIA   r0!, {r1-r2}

	/*save system mode registers*/
    /*enter system mode, no interrupts*/
    MRS     r2, cpsr
    BIC     r2, r2, #MODE_MASK      
    ORR     r2, r2, #MODE_SYSTEM32 | I_BIT | F_BIT
    MSR     cpsr_c, r2
    STMIA   r0!, {r13, r14}

    /*save abort mode registers*/
    /*enter abort mode, no interrupts*/
    MRS     r2, cpsr
    BIC     r2, r2, #MODE_MASK      
    ORR     r2, r2, #MODE_ABORT32 | I_BIT | F_BIT
    MSR     cpsr_c, r2
    MRS     r1,spsr
    STMIA   r0!, {r1, r13, r14}

	/*save undefine mode registers*/
    /*enter undefine mode, no interrupts*/
    MRS     r2, cpsr
    BIC     r2, r2, #MODE_MASK      
    ORR     r2, r2, #MODE_UNDEF32 | I_BIT | F_BIT
    MSR     cpsr_c, r2
    MRS     r1,spsr
    STMIA   r0!, {r1, r13, r14}

	/*save irq mode registers*/
    /*enter irq mode, no interrupts*/
    MRS     r2, cpsr
    BIC     r2, r2, #MODE_MASK      
    ORR     r2, r2, #MODE_IRQ32 | I_BIT | F_BIT
    MSR     cpsr_c, r2
    MRS     r1,spsr
    STMIA   r0!, {r1, r13, r14}

    /*save fiq mode registers*/
    /*enter fiq mode, no interrupts*/
    MRS     r2, cpsr
    BIC     r2, r2, #MODE_MASK      
    ORR     r2, r2, #MODE_FIQ32 | I_BIT | F_BIT
    MSR     cpsr_c, r2
    MRS     r1,spsr
    STMIA   r0!, {r1, r8-r14}

	/*back to SVC mode, no interrupts*/
    MRS     r2, cpsr
    BIC     r2, r2, #MODE_MASK      
    ORR     r2, r2, #MODE_SVC32 | I_BIT | F_BIT
    MSR     cpsr_c, r2

    /* save co-cpu register */
	MRC    p15, 2, r1, c0, c0, 0 
	MRC    p15, 0, r2, c1, c0, 1	
    MRC    p15, 0, r3, c1, c0, 0
    MRC    p15, 0, r4, c1, c0, 2
    MRC    p15, 0, r5, c2, c0, 0               
    STMIA   r0!, {r1-r5}

    /*save current addr in */
    LDR    r2, =STORE_ADDR_ARM_REG
    STR    r0,[r2]

#ifdef PWRCTRL_DEBUG_FLAG
    /*debug*/
    LDR      r2,=PWRCTRL_ASM_DEG_FLAG
    LDR      r1,=PWRCTRL_BAK_CO_REG_END
    STR      r1, [r2]
#endif    
/************************************************************************
;* FUNCTION
;*               pwrctrl_asm_bak_gic_register
;* DESCRIPTION
;*               bak gic register.
;* INPUTS
;*               NONE
;* OUTPUTS
;*               NONE
;*************************************************************************/

_ARM_FUNCTION_CALLED_FROM_C(pwrctrl_asm_bak_gic_register)

#ifdef PWRCTRL_DEBUG_FLAG

    /*debug*/
    LDR      r2,=PWRCTRL_ASM_DEG_FLAG
    LDR      r1,=PWRCTRL_BAK_GIC_REG
    STR      r1, [r2]
#endif
    LDR r0, =STORE_ADDR_GIC
    LDR r1, =CARM_GIC_CPUICR
    LDR    r2,[r1]
    STR    r2,[r0]

    LDR r1, =CARM_GIC_CPUPMR
    LDR    r2,[r1]
    STR    r2,[r0,#0x04]

    LDR r1, =CARM_GIC_CPUBPR
    LDR    r2,[r1]
    STR    r2,[r0,#0x08]

	LDR r1, =CARM_GIC_ICABPR
	LDR    r2,[r0,#0x14]
	STR    r2,[r1]

	LDR r1, =CARM_GIC_ICDDCR
	LDR    r2,[r1]
	STR    r2,[r0,#0x18]

	LDR r1, =CARM_GIC_ICDISR
	LDR    r2,[r1]
	STR    r2,[r0,#0x1C]
	
    LDR r1, =CARM_GIC_ICDISR1
    LDR    r2,[r1]
    STR    r2,[r0,#0x48]

    LDR    r2,[r1,#0x04]
    STR    r2,[r0,#0x4c]

    LDR    r2,[r1,#0x08]
    STR    r2,[r0,#0x50]

    LDR    r2,[r1,#0x0c]
    STR    r2,[r0,#0x54]

    LDR    r2,[r1,#0x10]
    STR    r2,[r0,#0x58]  
 
    LDR r1, =CARM_GIC_ICDIPR
    LDR    r2,[r1]
    STR    r2,[r0,#0x5c]

    LDR    r2,[r1,#0x04]
    STR    r2,[r0,#0x60]

    LDR    r2,[r1,#0x08]
    STR    r2,[r0,#0x64]

    LDR    r2,[r1,#0x0c]
    STR    r2,[r0,#0x68]

    LDR    r2,[r1,#0x10]
    STR    r2,[r0,#0x6c]

    LDR    r2,[r1,#0x14]
    STR    r2,[r0,#0x70]

    LDR    r2,[r1,#0x18]
    STR    r2,[r0,#0x74]

    LDR    r2,[r1,#0x1c]
    STR    r2,[r0,#0x78]

    LDR    r2,[r1,#0x20]
    STR    r2,[r0,#0x7c]

    LDR    r2,[r1,#0x24]
    STR    r2,[r0,#0x80]

    LDR    r2,[r1,#0x28]
    STR    r2,[r0,#0x84]

    LDR    r2,[r1,#0x2c]
    STR    r2,[r0,#0x88]

    LDR    r2,[r1,#0x30]
    STR    r2,[r0,#0x90]

    LDR    r2,[r1,#0x34]
    STR    r2,[r0,#0x94]

    LDR    r2,[r1,#0x38]
    STR    r2,[r0,#0x98]

    LDR    r2,[r1,#0x3c]
    STR    r2,[r0,#0x9c]

    LDR    r2,[r1,#0x40]
    STR    r2,[r0,#0xa0]

    LDR    r2,[r1,#0x44]
    STR    r2,[r0,#0xa4]

    LDR    r2,[r1,#0x48]
    STR    r2,[r0,#0xa8]

    LDR    r2,[r1,#0x4c]
    STR    r2,[r0,#0xac]

    LDR    r2,[r1,#0x50]
    STR    r2,[r0,#0xb0]

    LDR    r2,[r1,#0x54]
    STR    r2,[r0,#0xb4]

    LDR    r2,[r1,#0x58]
    STR    r2,[r0,#0xb8]

    LDR    r2,[r1,#0x5c]
    STR    r2,[r0,#0xbc]

    LDR    r2,[r1,#0x60]
    STR    r2,[r0,#0xc0]

    LDR    r2,[r1,#0x64]
    STR    r2,[r0,#0xc4]

    LDR    r2,[r1,#0x68]
    STR    r2,[r0,#0xc8]

    LDR    r2,[r1,#0x6c]
    STR    r2,[r0,#0xcc]

    LDR    r2,[r1,#0x70]
    STR    r2,[r0,#0xd0]

    LDR    r2,[r1,#0x74]
    STR    r2,[r0,#0xd4]

    LDR    r2,[r1,#0x78]
    STR    r2,[r0,#0xd8]

    LDR    r2,[r1,#0x7c]
    STR    r2,[r0,#0xdc]

    LDR    r2,[r1,#0x80]
    STR    r2,[r0,#0xe0]

    LDR    r2,[r1,#0x84]
    STR    r2,[r0,#0xe4]

    LDR    r2,[r1,#0x88]
    STR    r2,[r0,#0xe8]

    LDR    r2,[r1,#0x8c]
    STR    r2,[r0,#0xec]

    LDR    r2,[r1,#0x90]
    STR    r2,[r0,#0xf0]

    LDR    r2,[r1,#0x94]
    STR    r2,[r0,#0xf4]

    LDR    r2,[r1,#0x98]
    STR    r2,[r0,#0xf8]

    LDR    r2,[r1,#0x9c]
    STR    r2,[r0,#0xfc]

    LDR    r2,[r1,#0xa0]
    STR    r2,[r0,#0x100]
   
    LDR r1, =CARM_GIC_ICDICFR
    LDR    r2,[r1,#0x10]
    STR    r2,[r0,#0x104]
   
    LDR r1, =CARM_GIC_ICDSGIR
    LDR    r2,[r1]
    STR    r2,[r0,#0x108]

#ifdef PWRCTRL_DEBUG_FLAG
    /*debug*/
    LDR      r2,=PWRCTRL_ASM_DEG_FLAG
    LDR      r1,=PWRCTRL_BAK_CO_REG
    STR      r1, [r2]
#endif
/************************************************************************
;* FUNCTION
;*               pwrctrl_asm_bak_mmu_register 
;* DESCRIPTION
;*               bak gic register.
;* INPUTS
;*               NONE
;* OUTPUTS
;*               NONE
;*************************************************************************/

_ARM_FUNCTION_CALLED_FROM_C(pwrctrl_asm_bak_mmu_register)

mmuHandler:
			  
	@read control register to r1
	MRC    p15, 0, r1, c1, c0, 0
	@R1 = control register value
		
	@read domain access register
	MRC    p15, 0, r2, c3, c0, 0
	@R2 = original domain access register value

	@write domain access to get the domain access right
	@MOV	r3, #DM_ACC_UNCTRL
	LDR r3, =0xFFFFFFFF
	MCR    p15, 0, r3, c3, c0, 0 

	@If MMU is not enable,jump to storecode
	TST    r1, #CTRL_MMU_BIT
	@TST	r1, #0x1
	BEQ    pwrctrl_asm_bak_aarm_register_finished

	STMFD   sp!, {r1-r2}
	@R3 = identity mapping first level descriptor virtual address

	LDR    r2, =STORE_REMAP_ADDR1
	LDR    r3,[r2]

	LDR    r2, =STORE_REMAP_ADDR3
	LDR    r5,[r2]

	LDR    r2, =STORE_REMAP_ADDR4
	LDR    r8,[r2]
	
	LDMFD	sp!, {r1-r2}

	LDR    r4, [r3]
	@R4 = direct mapping first level descriptor

	@change the descriptor value to build the identity mapping
	STR    r5, [r3]

	@save a virtual address for resume
	ADRL   r9, boundary
	@resume process virtual to physical boundary address

	MOV    r6, r1
	LDR    r7, =CTRL_MMU_CACHE
	BIC    r6, r6, r7
	@r6 = control register config value(disable cache&mmu)

	STMFD	sp!, {r1-r8}

	//cache clear
	
	MOV   r1, #0

	MCR   p15,2,r1,c0,c0,0	// clear the CSSELR (select L1 Dcache)
	ISB
	MRC   p15,1,r7,c0,c0,0		//read CCSIDR for L1 cache parameters

	MOV   r3,r7 					// extract the CCSIDR.LineSize bit field
	AND   r3,r3,#0x7
	ADD   r3, r3, #4
	/* extract the CCSIDR.NSETS bit field*/
	MOV 	r4,r7
	LDR 	r1,=0x7FFF
	AND 	r4,r1,r4, LSR #13	 
	/*extract the CCSIDR.ASSOC bit field*/
	MOV 	r5,r7
	LDR 	r1,=0x3ff
	AND 	r5,r1 ,r5, LSR	#3
	
	CMP 	 r5,#3			// Cortex-A9 optimisation (known size)
	CMPEQ	 r3,#5
	BNE 	 label_generic1
	LDR 	r1,=0x40000000
	ADD    r2, r1, r4, LSL #5
	LDR 	r1,=0x80000000
	ADD    r3, r1, r4, LSL #5
	LDR 	r1,=0xC0000000
	ADD    r4, r1, r4, LSL #5
	MOV    r1,r4,LSL #5
				
label_optloop1:
	MCR 	 p15,0,r1,c7,c14,2
	MCR 	 p15,0,r2,c7,c14,2
	MCR 	 p15,0,r3,c7,c14,2
	MCR 	 p15,0,r4,c7,c14,2
	SUBS	 r1,r1,#0x20
	SUB 	 r2,r2,#0x20
	SUB 	 r3,r3,#0x20
	SUB 	 r4,r4,#0x20
	BPL 	 label_optloop1

label_generic1:
	CLZ r6, r5				// r6 is the bit position of the way size
							// increment
label_loopl3:
	MOV r2,r5				//r2 working copy of the max way size
							// (right aligned)
label_loop4:
	MOV   r7,r2,LSL r6
	MOV   r1,r4,LSL r3	

	ORR r7,r7,r1			// factor in the index number
	MCR p15,0,r7,c7,c14,2	// DCxxSW cache operation
	SUBS	r2,r2,#1		// decrement the way number
	BGE label_loop4
	SUBS	r4,r4,#1		// decrement the index
	BGE label_loopl3
label_end1:
	DSB

	LDMFD	sp!, {r1-r8}
	
	@clean, flush, invalidate the cache
	MOV   r7, #0

	@invalidate Icache
	MCR   p15, 0, r7, c7, c5, 0

	@drain WB
	MCR  p15, 0, r7, c7, c10, 4

	@invalidate I & D TLBs
	MCR  p15, 0, r7, c8, c7, 0

	@jump to PA
	MOV    pc, r8


disableMmu:
	MOV   r7, #0
	@invalidate Icache
	MCR   p15, 0, r7, c7, c5, 0


	@drain WB
	MCR  p15, 0, r7, c7, c10, 4

	@invalidate I & D TLBs
	MCR  p15, 0, r7, c8, c7, 0		  

	@ write control register to disable the mmu. It is harmless because
	@ we have build the direct mapping
	MCR    p15, 0, r6, c1, c0, 0
	NOP
	NOP
	NOP
	NOP

	@current store data
	@R0  = save memory address pha
	@R1  = control register value
	@R2  = domain access register value
	@R3  = identity first level descriptor address
	@R4  = direct mapping first level descriptor
	@R5  = identity mapping first level descriptor
	@R8  = disabmmu physical address
	@R9  = boundary address
	@R11 = current mode sp register value
	@R12 = cpsr register value
				
	@read TTB register
    MRC    p15, 0, r6, c2, c0, 0
    @R6 = TTB register value

    @ read PID register
    MRC    p15, 0, r7, c13, c0, 0
    @R7 = PID register value

  	LDR r0,=AXI_VIRT_TO_PHY(STORE_CTRL_REG_BASE+0x4)
    @save arm registers and mapping informations
    STMIA    r0!, {r1-r9, r11}

    LDR    r0, =AXI_VIRT_TO_PHY(STORE_ADDR_MMU_REG)
    ADD    r0,r0,#0x04
        
    /* read TTB register*/
    MRC	p15, 0, r5, c2, c0, 0     // TTBR0
    MRC	p15, 0, r6, c2, c0, 1     //TTBR1	
    MRC	p15, 0, r7, c2, c0, 2     // TTBCR
    STMIA	r0!, {r5 - r7}	
    
    MRC	p15, 0, r4, c3, c0, 0    // DACR
    MRC	p15, 0, r5, c7, c4, 0     // PAR	
    MRC	p15, 0, r6, c10, c2, 0    // PRRR
    MRC	p15, 0, r7, c10, c2, 1    // NMRR	
    STMIA	r0!, {r4 - r7}
    
    MRC	p15, 0, r3, c12, c0, 0    // VBAR
    MRC	p15, 0, r4, c13, c0, 1    // CONTEXTIDR
    MRC	p15, 0, r5, c13, c0, 2    // TPIDRURW
    MRC	p15, 0, r6, c13, c0, 3    // TPIDRURO
    MRC	p15, 0, r7, c13, c0, 4    // TPIDRPRW	
    STMIA	r0!, {r3 - r7}
	
    ;/*save current addr in */
    LDR    r2, =AXI_VIRT_TO_PHY(STORE_ADDR_MMU_REG)
    STR    r0,[r2]
    
pwrctrl_asm_bak_aarm_register_finished:
#ifdef PWRCTRL_DEBUG_FLAG   
     /*debug*/
    LDR      r2,=AXI_VIRT_TO_PHY(PWRCTRL_ASM_DEG_FLAG)
    LDR      r1,=PWRCTRL_ENTER_WFI
    STR      r1, [r2]
#endif

	/*发送IPC中断*/
    LDR      r2,=SYS_REG_VIRT_TO_PHY(PWRCTRL_IPC_INTSEND)
    LDR      r1,=0x8
    STR      r1, [r2]
    
	/*? App Cpu最后执行STANDBY指令，发出standbywfi/standbywfe；（PC指针 Hold）*/
    DSB
    ISB
    wfi
    
    NOP 
    NOP
    NOP

    BL      PWRCTRL_AsmAcpuCodeBegin

_ARM_FUNCTION_CALLED_FROM_C(PWRCTRL_CreatIdMap)

	STMFD   sp!, {r1-r8}
	
	@convert disableMmu VA to PA

	LDR r1, =ISRAM1_VA_SLEEP
	ADRL r2,PWRCTRL_AsmAcpuCodeBegin
	ADR  r3,disableMmu
	SUB r3,r3,r2
	ADD r1,r1,r3
	SUB r5,r1,#0xc3000000

	@R5 = disableMmu entry code physical address

	MOV 	 r8,  r5
	@R8 = disableMmu entry code physical address

	@creat identity mapping
	LDR 	 r6, =SEC_ADDR_MASK
	AND 	 r5, r5, r6
	LDR 	 r6, =PMD_SECT_ARMA9
	ORR 	 r5, r5, r6
	@R5 = identity mapping first level descriptor

	LDR	r1,=STORE_REMAP_ADDR3
	STR	r5,[r1]

	LDR    r1, =STORE_REMAP_ADDR4
	STR    r8,[r1]
	
	@read TTB registers
	MRC    p15, 0, r3, c2, c0, 0

	@get the first level descriptor
	LDR    r4, =TRANS_BASE_MASK
	BIC    r3, r3, r4
	MOV    r4, r8, LSR #0x14
	MOV    r4, r4, LSL #0x02
	ORR    r3, r3, r4
	@R3 = identity mapping first level descriptor physical address

	@get the first level descriptor VA	
	MOV r0, r3
	
	LDMFD	sp!, {r1-r8}
	MOV pc, lr        
pwrctrl_asm_lightsleep_end:

    LDR    r0,=PWRCTRL_ASM_LINK_FLAG1
    LDR    r1,=PWRCTRL_ASM_LINK_FLAG2

pwrctrl_asm_sleep_end:

   /* END*/


#ifdef  __cplusplus
}
#endif
