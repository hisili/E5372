
#include <linux/linkage.h>
#include <asm/assembler.h>
#include <mach/hardware.h>
#include <mach/balong_v100r001.h>
#include <mach/pwcSleepAsm.h>
#include <asm/domain.h>
#include <asm/system.h>
#include <asm/pgtable.h>


		
_ARM_FUNCTION_CALLED_FROM_C(pwrctrl_asm_acpu_deepsleep)

	STMFD	sp!, {r0 - r12, lr}

	LDR r1, =ISRAM1_VA_SLEEP
	ADRL r2,pwrctrl_asm_code_begin
	ADR  r3,pwrctrl_asm_begin_deepsleep
	SUB r3,r3,r2
	ADD r1,r1,r3
	MOV pc,r1
    .ltorg


_ARM_FUNCTION_CALLED_FROM_C(pwrctrl_asm_code_begin)

	B .
	LDR r0,=0x2ffe040c
	MOV pc,r0
	
	BL pwrctrl_asm_deepsleep_to_normal

    



_ARM_FUNCTION_CALLED_FROM_C(pwrctrl_asm_begin_deepsleep)

mmuHandler:
        @read control register to r1
        MRC    p15, 0, r1, c1, c0, 0
        @R1 = control register value
			
        @read domain access register
        MRC    p15, 0, r2, c3, c0, 0
        @R2 = original domain access register value

        @write domain access to get the domain access right
        @MOV    r3, #DM_ACC_UNCTRL
	    LDR	r3, =0xFFFFFFFF
        MCR    p15, 0, r3, c3, c0, 0   

        @If MMU is not enable,jump to storecode
        TST    r1, #CTRL_MMU_BIT
	    @TST    r1, #0x1
        BEQ    storeCode

creatIdMap:


        STMFD   sp!, {r1-r3}

        @convert disableMmu VA to PA

		LDR r1, =ISRAM1_VA_SLEEP
		ADRL r2,pwrctrl_asm_code_begin
		ADR  r3,disableMmu
		SUB r3,r3,r2
		ADD r1,r1,r3
		SUB r5,r1,#0xc3000000

		LDMFD   sp!, {r1-r3}
	    /*LDR	r5,  =0//(__virt_to_phys(r1))*/
        @R5 = disableMmu entry code physical address

        MOV      r8,  r5
        @R8 = disableMmu entry code physical address

        @creat identity mapping
        LDR      r6, =SEC_ADDR_MASK
        AND      r5, r5, r6
        LDR      r6, =PMD_SECT_ARMA9
        ORR      r5, r5, r6
        @R5 = identity mapping first level descriptor


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
        STMFD   sp!, {r0-r2,r6,r14}
		
    	@@ phys addr save in r9
    	@@MOV	r9, r3
    	MOV	r0, r3
    	MOV	r1, #0x100
    	LDR r6,=hisibalong_ioremap
    	MOV lr,pc
    	MOV pc,r6
    	
    	MOV	r3, r0
    	LDMFD   sp!, {r0-r2,r6,r14}
	

        @R3 = identity mapping first level descriptor virtual address

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


        @clean, flush, invalidate the cache
        MOV   r7, #0

        @invalidate Icache
        MCR   p15, 0, r7, c7, c5, 0
/*
cleanDCache1:
        @test clean and invalidate Dcache
        MRC  p15, 0, pc, c7, c14, 3
        BNE  cleanDCache1
*/
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


/*
cleanDCache2:
        @test clean and invalidate Dcache
        MRC  p15, 0, pc, c7, c14, 3
        BNE  cleanDCache2
*/
    	@drain WB
    	MCR  p15, 0, r7, c7, c10, 4

    	@invalidate I & D TLBs
    	MCR  p15, 0, r7, c8, c7, 0        

        @ write control register to disable the mmu. It's harmless because
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
            
 

storeCode:
        @read TTB register
        MRC    p15, 0, r6, c2, c0, 0
        @R6 = TTB register value

        @ read PID register
        MRC    p15, 0, r7, c13, c0, 0
        @R7 = PID register value

      	LDR r0,=DSRAM1_STORE_CTRL_REG_BASE
        @save arm registers and mapping informations
        STMIA    r0!, {r1-r9, r11}

	/*
	LDR r2, =DDRC_VA_BASE
	LDR r1, [r2,#0x04]
	ORR r1, r1, #0x01
	STR r1, [r2,#0x04]
	;@whether the DDR entered self refresh
DdrEnterSfrsh:
	LDR r1,[r2,#0x00]
	AND r1,r1, #0x00000004
	CMP r1,#0x04
	BNE DdrEnterSfrsh
	*/

	/*DDR进入自刷新*/
    LDR r2, =V3R2_SC_BASE
    LDR r1,[r2,#0x08]
    ORR r1,r1,#0x800
    STR r1,[r2,#0x08]
	

    ;/*关AXI时钟*/
	LDR r2, = V3R2_SC_BASE
	LDR r1, [r2,#0x07C]
	ORR r1, r1, #PWC_SC_ACPU_AXILB_CLCK
	STR r1,[r2,#0x07C]

    ;/*关DDR时钟*/
	LDR r2, = V3R2_SC_BASE
	LDR r1, [r2,#0x07C]
	ORR r1, r1,#PWC_SC_ACPU_MDDRC_ACLCK |PWC_SC_ACPU_MDDRC_PCLCK|PWC_SC_ACPU_DDR2X_PCLCK
	STR r1,[r2,#0x07C]

	/*配置系统控制器清除ACPU中断响应状态*/
	LDR r2, = V3R2_SC_BASE
	LDR r1, [r2,#0x024]
	BIC r1, r1, #ACPU_IMSAT_MASK
	STR r1, [r2,#0x024]



_ARM_FUNCTION_CALLED_FROM_C(pwrctrl_asm_bak_aarm_co_register)

	/*保存AARM寄存器到DSRAM1*/
	
	LDR r0, =DSRAM1_STORE_ARM_REG_BASE
	ADD r0,r0,#0x04
	STMIA r0!,{r1-r12}
	

   /*save svc mode registers*/ 
   /*enter svc mode no interrupts*/ 
	MRS r2,cpsr
	BIC r2,r2,#MODE_MASK
	ORR r2,r2, #MODE_SVC32 |I_BIT |F_BIT
	MSR cpsr_c, r2
	MRS r1,spsr
	STMIA r0!,{r1,r13,r14}

   /*save firq mode registers*/ 
   /*enter firq mode no interrupts*/ 
	MRS r2,cpsr
	BIC r2,r2,#MODE_MASK
	ORR r2,r2, #MODE_FIQ32 |I_BIT |F_BIT
	MSR cpsr_c, r2
	MRS r1,spsr
	STMIA r0!,{r1,r8-r14}



   /*save irq mode registers*/ 
   /*enter irq mode no interrupts*/ 
	MRS r2,cpsr
	BIC r2,r2,#MODE_MASK
	ORR r2,r2, #MODE_IRQ32 |I_BIT |F_BIT
	MSR cpsr_c, r2
	MRS r1,spsr
	STMIA r0!,{r1,r13,r14}
   
   /*save undefine mode registers*/ 
   /*enter undefine mode no interrupts*/ 
	MRS r2,cpsr
	BIC r2,r2,#MODE_MASK
	ORR r2,r2, #MODE_UNDEF32 |I_BIT |F_BIT
	MSR cpsr_c, r2
	MRS r1,spsr
	STMIA r0!,{r1,r13,r14}

   /*save abort mode registers*/ 
   /*enter abort mode no interrupts*/ 
	MRS r2,cpsr
	BIC r2,r2,#MODE_MASK
	ORR r2,r2, #MODE_ABORT32 |I_BIT |F_BIT
	MSR cpsr_c, r2
	MRS r1,spsr
	STMIA r0!,{r1,r13,r14}

   /*save system mode registers*/ 
   /*enter system mode no interrupts*/ 
	MRS r2,cpsr
	BIC r2,r2,#MODE_MASK
	ORR r2,r2, #MODE_SYSTEM32 |I_BIT |F_BIT
	MSR cpsr_c, r2
	MRS r1,spsr
	STMIA r0!,{r13,r14}

	;/*save co-cpu register*/
	MRC  p15,0,r1,c1,c0,0
	MRC  p15,0,r2,c2,c0,0
	MRC  p15,0,r3,c9,c0,0
	MRC  p15,0,r4,c9,c0,1
	MRC  p15,0,r5,c9,c1,0
	MRC  p15,0,r6,c9,c1,1
	MRC  p15,0,r7,c10,c2,0  
	STMIA r0!,{r1-r7}

  	;/*back to SVC mode , no interupts*/
	MRS r2,cpsr
	BIC r2,r2,#MODE_MASK
	ORR r2,r2, #MODE_SVC32 |I_BIT |F_BIT
	MSR cpsr_c, r2

	;/*save current addr in*/
   LDR r2,=DSRAM1_STORE_ARM_REG_BASE
   STR r0,[r2]



_ARM_FUNCTION_CALLED_FROM_C(pwrctrl_asm_bak_gic_register)

	/*保存gic寄存器*/

	LDR r0,=DSRAM1_STORE_GIC_BASE
	LDR r1,=PBXA9_GIC_CPU_CONTROL
	LDR r2,[r1]
	STR r2,[r0]

	LDR r1,=ACPU_GIC_ICCPMR
	LDR r2,[r1]
	STR r2,[r0,#0x04]

	LDR r1,=ACPU_GIC_ICCBPR
	LDR r2,[r1]
	STR r2,[r0,#0x08]

	LDR r1,=ACPU_GIC_ICCEOIR
	LDR r2,[r1]
	STR r2,[r0,#0x0C]

	LDR r1,=ACPU_GIC_ICABPR
	LDR r2,[r1]
	STR r2,[r0,#0x10]
	
	LDR r1,=ACPU_GIC_ICDDCR
	LDR r2,[r1]
	STR r2,[r0,#0x14]
	
	LDR r1,=ACPU_GIC_ICDISR
	LDR r2,[r1]
	STR r2,[r0,#0x18]

	LDR r1,=ACPU_GIC_ICDISER
	LDR r2,[r1]
	STR r2,[r0,#0x1C]

	LDR r2,[r1,#0x04]
	STR r2,[r0,#0x20]

	LDR r2,[r1,#0x08]
	STR r2,[r0,#0x24]

	LDR r2,[r1,#0x0C]
	STR r2,[r0,#0x28]

	LDR r2,[r1,#0x10]
	STR r2,[r0,#0x2C]


	LDR r1,=ACPU_GIC_ICDISPR
	LDR r2,[r1]
	STR r2,[r0,#0x30]

	LDR r2,[r1,#0x04]
	STR r2,[r0,#0x34]

	LDR r1,=ACPU_GIC_ICDICPR
	LDR r2,[r1]
	STR r2,[r0,#0x38]

	LDR r2,[r1,#0x04]
	STR r2,[r0,#0x3C]
	
	LDR r1,=ACPU_GIC_ICDIPR
	LDR r2,[r1]
	STR r2,[r0,#0x40]

	LDR r2,[r1,#0x04]
	STR r2,[r0,#0x44]

	LDR r2,[r1,#0x08]
	STR r2,[r0,#0x48]

	LDR r2,[r1,#0x0c]
	STR r2,[r0,#0x4c]
	
	LDR r2,[r1,#0x1c]
	STR r2,[r0,#0x50]



	LDR r1,=ACPU_GIC_ICDICFR
	LDR r2,[r1]
	STR r2,[r0,#0x54]

	LDR r1,=ACPU_GIC_ICDSGIR
	LDR r2,[r1]
	STR r2,[r0,#0x58]

	
    /*back to SVC mode ,no interupts*/
	MRS r2,cpsr
	BIC r2,r2,#MODE_MASK
	ORR r2,r2, #MODE_SVC32 |I_BIT |F_BIT
	MSR cpsr_c, r2

	/*配置ACPU寄存器屏蔽所有中断*/
	
	LDR r2,=PBXA9_GIC_DIST_CONTROL
	LDR r1,[r2]
	BIC r1,r1,#0x1
	STR r1,[r2]
	

	
	/*写IPC发中断给CCPU,只将相应的中断位置1*/
	/*
	LDR r2, = ACPU_IPC_INT_PHY_BASE
	LDR r1, [r2]
	ORR r1, r1, #0x1<<IPC_INT_APP_MODEM
	STR r1,[r2]
	*/


	/*使能ACPU唤醒中断*/
	
	LDR r2, =V3R2_SC_BASE
	LDR r1, =0x3FFF
	STR r1,[r2,#0x0B4]
	

     /*配置ACPU系统进入sleep 模式*/

	
	LDR r2, =V3R2_SC_BASE
	LDR r1, [r2,#0x08]
	BIC r1, #0x07
	ORR r1,r1,#0x00
	STR r1,[r2,#0x08]
	

	;/*进入wfi*/
	wfi
	
	NOP
	NOP
	NOP
	NOP
	NOP
	


_ARM_FUNCTION_CALLED_FROM_C(pwrctrl_asm_deepsleep_to_normal)



_ARM_FUNCTION_CALLED_FROM_C(pwrctrl_asm_restore_arm_co_register)

	/*恢复ARM寄存器*/
	LDR r0, =DSRAM1_STORE_ARM_REG_BASE
	LDR r0,[r0]

	LDMDB r0!,{r1-r7}
	MCR p15,0,r1,c1,c0,0
	MCR p15,0,r2,c2,c0,0
	MCR p15,0,r3,c9,c0,0
	MCR p15,0,r4,c9,c0,1
	MCR p15,0,r5,c9,c1,0
	MCR p15,0,r6,c9,c1,1
	MCR p15,0,r7,c10,c2,0  
   /*resume system mode registers*/ 
   /*enter system mode no interrupts*/ 
	MRS r2,cpsr
	BIC r2,r2,#MODE_MASK
	ORR r2,r2, #MODE_SYSTEM32 |I_BIT |F_BIT
	MSR cpsr_c, r2
	LDMDB r0!,{r13,r14}

	

   /*resume abort mode registers*/ 
   /*enter abort mode no interrupts*/ 
	MRS r2,cpsr
	BIC r2,r2,#MODE_MASK
	ORR r2,r2, #MODE_ABORT32 |I_BIT |F_BIT
	MSR cpsr_c, r2
	LDMDB r0!,{r1,r13,r14}
    MSR spsr_c,r1

   /*resume undefine  mode registers*/ 
   /*enter undefine mode no interrupts*/ 
	MRS r2,cpsr
	BIC r2,r2,#MODE_MASK
	ORR r2,r2, #MODE_UNDEF32 |I_BIT |F_BIT
	MSR cpsr_c, r2
	LDMDB r0!,{r1,r13,r14}
    MSR spsr_c,r1

   /*resume irq  mode registers*/ 
   /*enter irq mode no interrupts*/ 
	MRS r2,cpsr
	BIC r2,r2,#MODE_MASK
	ORR r2,r2, #MODE_IRQ32 |I_BIT |F_BIT
	MSR cpsr_c, r2
	LDMDB r0!,{r1,r13,r14}
    MSR spsr_c,r1

   /*resume fiq  mode registers*/ 
   /*enter fiq mode no interrupts*/ 
	MRS r2,cpsr
	BIC r2,r2,#MODE_MASK
	ORR r2,r2, #MODE_FIQ32 |I_BIT |F_BIT
	MSR cpsr_c, r2
	LDMDB r0!,{r1,r8-r14}
    MSR spsr_c,r1

   /*resume svc  mode registers*/ 
   /*enter svc mode no interrupts*/ 
	MRS r2,cpsr
	BIC r2,r2,#MODE_MASK
	ORR r2,r2, #MODE_SVC32 |I_BIT |F_BIT
	MSR cpsr_c, r2
	LDMDB r0!,{r1,r13,r14}
    MSR spsr_c,r1
	LDMDB r0!,{r1-r12}



_ARM_FUNCTION_CALLED_FROM_C(pwrctrl_asm_restore_gic_register)

	    /*恢复GIC寄存器*/
	
		LDR r0,=DSRAM1_STORE_GIC_BASE
		LDR r1,=PBXA9_GIC_CPU_CONTROL
		LDR r2,[r0]
		STR r2,[r1]

		LDR r1,=ACPU_GIC_ICCPMR
		LDR r2,[r0,#0x04]
		STR r2,[r1]

		LDR r1,=ACPU_GIC_ICCBPR
		LDR r2,[r0,#0x08]
		STR r2,[r1]

		LDR r1,=ACPU_GIC_ICCEOIR
		LDR r2,[r0,#0x0C]
		STR r2,[r1]

		LDR r1,=ACPU_GIC_ICABPR
		LDR r2,[r0,#0x10]
		STR r2,[r1]
		
		LDR r1,=ACPU_GIC_ICDDCR
		LDR r2,[r0,#0x14]
		STR r2,[r1]
		
		LDR r1,=ACPU_GIC_ICDISR
		LDR r2,[r0,#0x18]
		STR r2,[r1]

		LDR r1,=ACPU_GIC_ICDISER
		LDR r2,[r0,#0x1C]
		STR r2,[r1]

		LDR r2,[r0,#0x20]
		STR r2,[r1,#0x04]

		LDR r2,[r0,#0x24]
		STR r2,[r1,#0x08]

		LDR r2,[r0,#0x28]
		STR r2,[r1,#0x0C]

		LDR r2,[r0,#0x2C]
		STR r2,[r1,#0x10]


		LDR r1,=ACPU_GIC_ICDISPR
		LDR r2,[r0,#0x30]
		STR r2,[r1]

		LDR r2,[r0,#0x34]
		STR r2,[r1,#0x04]

		LDR r1,=ACPU_GIC_ICDICPR
		LDR r2,[r0,#0x38]
		STR r2,[r1]

		LDR r2,[r0,#0x3C]
		STR r2,[r1,#0x04]

		
		LDR r1,=ACPU_GIC_ICDIPR
		LDR r2,[r0,#0x40]
		STR r2,[r1]

		LDR r2,[r0,#0x44]
		STR r2,[r1,#0x04]

		LDR r2,[r0,#0x48]
		STR r2,[r1,#0x08]

		LDR r2,[r0,#0x4c]
		STR r2,[r1,#0x0c]
		
		
		LDR r2,[r0,#0x50]
		STR r2,[r1,#0x1c]


		LDR r1,=ACPU_GIC_ICDICFR
		LDR r2,[r0,#0x54]
		STR r2,[r1]

		LDR r1,=ACPU_GIC_ICDSGIR
		LDR r2,[r0,#0x58]
		STR r2,[r1]




		/*DDR退出自刷新*/
	    LDR r2, =V3R2_SC_BASE
	    LDR r1,[r2,#0x08]
	    BIC r1,r1,#0x800
	    STR r1,[r2,#0x08]



		LDR r0,=DSRAM1_STORE_CTRL_REG_BASE
		@current mode registers reload
        LDMIA   r0!, {r1-r9, r11}

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

        @resume TTB register
        MCR    p15, 0, r6, c2, c0, 0

        @resume PID register
        MCR    p15, 0, r7, c13, c0, 0

        @clean, flush, invalidate the cache
        MOV    r7, #0

        @invalidate Icache
        MCR    p15, 0, r7, c7, c5, 0
#if 0
cleanDCache3:
        @test clean and invalidate Dcache
        MRC p15, 0, pc, c7, c14, 3
        BNE cleanDCache3
#endif
        @drain WB
        MCR  p15, 0, r7, c7, c10, 4

        @invalidate I & D TLBs
        MCR  p15, 0, r7, c8, c7, 0

        @resume control register,
        @if mmu configed enable then it will be enabled
        MCR  p15, 0, r1, c1, c0, 0

        @load virt boundary address to pc
        MOV     pc, r9

boundary:

        @resume the direct mapping first level descriptor
        STR     r4, [r3] 

        @get the first level descriptor VA
        STMFD   sp!, {r0-r3, r14}
		MOV	r0,  r3
		LDR r6,=hisibalong_iounmap
		MOV lr,pc
		MOV pc,r6
		MOV	r3,  #0x0 
        LDMFD   sp!, {r0-r3, r14}
        @R3 = identity mapping first level descriptor virtual address free

acrossBound:
        @resume domain access register
        MCR     p15, 0, r2, c3, c0, 0

    	NOP	
    	LDMFD   sp!, {r0 -r12, pc}
	
#if 0
	/*使能ACPU唤醒中断*/
	LDR r2, =PWR_SC_VA_BASE
	LDR r1, =0x3FFF
	STR r1,[r2,#0x0B4]
	
AcpuWakeupCheck:
	/*读唤醒中断状态*/
	LDR r2, =PWR_SC_VA_BASE
	LDR r1, [r2,#0xB8]
	CMP r1,#0x000
	be  AcpuWakeupCheck

	/*写IPC发中断给CCPU,只将相应的中断位置1*/
	LDR r2, = ACPU_IPC_INT_PHY_BASE
	LDR r1, [r2]
	ORR r1, r1, #0x1<<IPC_INT_APP_MODEM
	STR r1,[r2]
#endif




