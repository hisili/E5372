#include <linux/init.h>
#include <asm/io.h>
#include <asm/string.h>
#include <linux/interrupt.h>
#include "BSP.h"
#if (defined (BOARD_FPGA_P500) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
#include "bsp_guvic.h"
#include "bsp_guipc.h"
#endif

#ifndef BSP_VIC_REG_READ
#define BSP_VIC_REG_READ(reg,result) \
	((result) = *(volatile BSP_U32 *)(reg))
#endif /*BSP_VIC_REG_READ*/

#ifndef BSP_VIC_REG_WRITE
#define BSP_VIC_REG_WRITE(reg,data) \
	(*((volatile BSP_U32 *)(reg)) = (data))
#endif /*BSP_VIC_REG_WRITE*/

#define VIC_INT_LOCK(flags) \
    local_irq_save(flags)
#define VIC_INT_UNLOCK(flags) \
    local_irq_restore(flags)

typedef struct {	/* VEC_ENTRY */
    VOIDFUNCPTR	routine;
    BSP_S32	arg;
    } BSP_VEC_ENTRY;

BSP_VEC_ENTRY stSubVecTable [SUBVIC_NUM];

/* base address of subvic registers */
void __iomem *subvic_base = NULL;

/*modify by wangjing 2011-4-7*/
UINT32 g_subVicIntCheck=0;
VOIDFUNCPTR subVICIntHookPtr = (VOIDFUNCPTR) NULL;


/*modify by wangjing 2011-4-7 for 可维可测*/
void subVICIntHook(VOIDFUNCPTR p_Func)
{
    subVICIntHookPtr = p_Func;
}

void subVICIntHookDel(void)
{
    subVICIntHookPtr = (VOIDFUNCPTR) NULL;
}


/*****************************************************************************
* 函 数 名  : SubVic_IntHandler
*
* 功能描述  : 二级中断总挂接函数
*
* 输入参数  : 无  
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2009年3月5日   zhanghailun  creat
*****************************************************************************/
irqreturn_t BSP_DRV_SubVicIntHandler(int irq, void *dev_id)
{
    unsigned int newLevel = 0;
    unsigned int isr = 0;

#if (defined (BOARD_FPGA_P500) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
    BSP_VIC_REG_READ(subvic_base+SUBVIC_IRQ_STATUS_L, isr);
#else
    BSP_REG_READ(FPGA_VIC_STATUS, 0, isr);
#endif

    g_subVicIntCheck = 1;
    /* 找到最高优先级的中断 */
    for(newLevel = 0; newLevel < SUBVIC_NUM; newLevel++)
    {
        if(isr&(1<<newLevel))
        {
            break;
        }
    }
    /* 没有中断 */
    if (newLevel == SUBVIC_NUM)   
    {
        printk(KERN_ERR "BSP_GUFPGA_IntHandler: newLevel==0  Error!\n");
        return;
    }

    /*modify by wangjing 2011-4-7*/
    if (NULL != subVICIntHookPtr)
    {
        (BSP_VOID)subVICIntHookPtr(newLevel);
    }
	if(NULL != stSubVecTable[newLevel].routine)
	{
		stSubVecTable[newLevel].routine(stSubVecTable[newLevel].arg);            
	}	
    /*modify by wangjing 2011-4-7*/
	g_subVicIntCheck = 0;
    if (NULL != subVICIntHookPtr)
    {
        (BSP_VOID)subVICIntHookPtr(newLevel);
    }

	return IRQ_HANDLED;
}

/*****************************************************************************
* 函 数 名  : SubVic_Init
*
* 功能描述  : 二级中断初始化
*
* 输入参数  : 无  
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2009年3月5日   zhanghailun  creat
*****************************************************************************/
BSP_VOID BSP_DRV_SubVicInit()
{
    BSP_U32 	i;
    int ret;

	for(i=0; i<SUBVIC_NUM; i++)
	{
		stSubVecTable[i].routine = NULL;
	}

#if (defined (BOARD_FPGA_P500) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
    subvic_base = ioremap(SUBVIC_BASE, SUBVIC_SIZE);
#else
    subvic_base = ioremap(FPGA_VIC_BASE, FPGA_VIC_SIZE);
#endif    
	if (!subvic_base)
	{
		printk(KERN_ERR "subvic ioremap error.\n");
		return;
	}

#if (defined (BOARD_FPGA_P500) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
	BSP_VIC_REG_WRITE(subvic_base+SUBVIC_IRQ_ENABLE_L, 0x00);
	BSP_VIC_REG_WRITE(subvic_base+SUBVIC_IRQ_ENABLE_H, 0x00);	
#else
	BSP_REG_WRITE(FPGA_VIC_DISABLE, 0, 0x00);  
#endif    
#if (defined (BOARD_FPGA_P500) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
    ret = request_irq(INT_LVL_FPGA, BSP_DRV_SubVicIntHandler, 0, "subvic_irq", NULL);
#else
    ret = request_irq(INT_LVL_SUBVIC, BSP_DRV_SubVicIntHandler, 0, "subvic_irq", NULL);
#endif    
	if (ret ) {
		printk(KERN_ERR "BSP_DRV_SubVicInit: Unable to register subvic irq ret=%d.\n", ret);
	}
    else
    {
#if (defined (BOARD_FPGA_P500) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))        
        /*BSP_GUFPGA_IntInit();*/
        /*BSP_GUIPC_Init();*/
#endif
	printk("BSP_DRV_SubVicInit ok\n");
    }

}

/*****************************************************************************
* 函 数 名  : BSP_INT_Enable
*
* 功能描述  : 使能某个中断
*
* 输入参数  : BSP_S32 ulLvl 要使能的中断号，取值范围0～40  
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年3月5日   zhanghailun  creat
*****************************************************************************/

BSP_S32 BSP_INT_Enable ( BSP_S32 s32Lvl)
{
    BSP_U32 ulValue = 0;
    BSP_S32 lockkey = 0;
    unsigned long flag;
    BSP_S32 s32Result = OK;
    BSP_S32 tmp;
    
#if (defined (BOARD_FPGA_P500) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
    if(s32Lvl >= (INT_LVL_MAX + SUBVIC_NUM + GUFPGA_VIC_NUM))
#else
    if(s32Lvl >= (INT_LVL_MAX + SUBVIC_NUM))
#endif
    /* BalongV7R1 Porting DRV: Modify end,c61362,20110322 */
	{
		return 	ERROR;
	}
	
    if(s32Lvl < INT_LVL_MAX)
    {
        ;
    }
    else
	{
#if (defined (BOARD_FPGA_P500) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
        if(s32Lvl >= (INT_LVL_MAX + SUBVIC_NUM))
        {
            BSP_GUFPGA_IntEnable(s32Lvl - (INT_LVL_MAX + SUBVIC_NUM));
        }
        else
        {
        /*设置FPGA VIC*/
        VIC_INT_LOCK(lockkey);        
        BSP_VIC_REG_READ(subvic_base + SUBVIC_IRQ_ENABLE_L,ulValue);
        ulValue |= (1<<(s32Lvl-INT_LVL_MAX));
        BSP_VIC_REG_WRITE(subvic_base + SUBVIC_IRQ_ENABLE_L, ulValue);        
        VIC_INT_UNLOCK(lockkey);
        }
#else
	    local_irq_save(flag);
        BSP_REG_READ(FPGA_VIC_ENABLE, 0, tmp);
        tmp |= (1<<(s32Lvl - INT_LVL_MAX));
        BSP_REG_WRITE(FPGA_VIC_ENABLE, 0, tmp);
        local_irq_restore(flag);
#endif    
    }
    
    return s32Result;
}/*lint !e529*/

/*****************************************************************************
 * 函 数 名  : BSP_INT_Disable
 *
 * 功能描述  : 去使能某个中断
 *
 * 输入参数  : BSP_S32 ulLvl 要使能的中断号，取值范围0～40 
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 修改记录  : 2009年3月5日   zhanghailun  creat
 *****************************************************************************/
BSP_S32 BSP_INT_Disable ( BSP_S32 s32Lvl)
{
    BSP_U32 ulValue = 0;
    BSP_S32 lockkey = 0;
    unsigned long flag;
    BSP_S32 s32Result = OK;
    BSP_U32 tmpBase = 0;
    BSP_S32 tmp;
    
#if (defined (BOARD_FPGA_P500) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
    if(s32Lvl >= (INT_LVL_MAX + SUBVIC_NUM + GUFPGA_VIC_NUM))
#else    
    if(s32Lvl >= (INT_LVL_MAX + SUBVIC_NUM))
#endif
    {
		return 	ERROR;
	}

    if(s32Lvl < INT_LVL_MAX)
    {
        ;
    }
    else
	{
#if (defined (BOARD_FPGA_P500) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
        if(s32Lvl >= (INT_LVL_MAX + SUBVIC_NUM))
        {
            BSP_GUFPGA_IntDisable(s32Lvl - (INT_LVL_MAX + SUBVIC_NUM));
        }
        else
        {
        /*设置FPGA VIC*/
        VIC_INT_LOCK(lockkey);
        BSP_VIC_REG_READ(subvic_base + SUBVIC_IRQ_ENABLE_L,ulValue);
        ulValue &= ~(BSP_U32)(1<<((BSP_U32)(s32Lvl-INT_LVL_MAX)));
        BSP_VIC_REG_WRITE(subvic_base + SUBVIC_IRQ_ENABLE_L, ulValue);
        VIC_INT_UNLOCK(lockkey);
        }
#else
	    local_irq_save(flag);
        BSP_REG_READ(FPGA_VIC_DISABLE, 0, tmp);
        tmp |= (1<<(s32Lvl - INT_LVL_MAX));
        BSP_REG_WRITE(FPGA_VIC_DISABLE, 0, tmp);
        local_irq_restore(flag);
#endif    
    }
    
    return s32Result;	
}

/*****************************************************************************
 * 函 数 名  : BSP_INT_Connect
 *
 * 功能描述  : 注册某个中断
 *
 * 输入参数  : VOIDFUNCPTR * vector 中断向量号，取值范围0～40
 *  		 VOIDFUNCPTR routine  中断服务程序
 *           BSP_S32 parameter      中断服务程序参数
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 修改记录  : 2009年3月5日   zhanghailun  creat
 *****************************************************************************/
BSP_S32 BSP_INT_Connect  (VOIDFUNCPTR * vector,VOIDFUNCPTR routine, BSP_S32 parameter)
{
    BSP_S32 lockkey = 0;
    unsigned long flag;
    BSP_S32 s32Result = OK;
    char acTmp[11] = "subvic_000";
    BSP_S32 ret = OK;
    BSP_CHAR vicname[32] = {0};    
    int iTmp;
    
#if (defined (BOARD_FPGA_P500) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
    if(IVEC_TO_INUM(vector) >= (INT_LVL_MAX + SUBVIC_NUM + GUFPGA_VIC_NUM))
#else      
    if(IVEC_TO_INUM(vector) >= (INT_LVL_MAX + SUBVIC_NUM))
#endif
	{
		return 	ERROR;
	}
	
    if(IVEC_TO_INUM(vector) < INT_LVL_MAX)
    {
        iTmp = IVEC_TO_INUM(vector);        
        acTmp[9] = (iTmp%10) + '0';
        acTmp[8] = ((iTmp/10)%10) + '0';
        acTmp[7] = (iTmp/100) + '0';
        
        s32Result = request_irq(vector, routine, 0, acTmp, NULL);
    }
    else
    {
#if (defined (BOARD_FPGA_P500) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
        if(IVEC_TO_INUM(vector) >= (INT_LVL_MAX + SUBVIC_NUM))
        {
            BSP_GUFPGA_IntConnect(IVEC_TO_INUM(vector) - (INT_LVL_MAX + SUBVIC_NUM), routine, parameter);
        }
        else
        {
        VIC_INT_LOCK(lockkey);
        stSubVecTable[IVEC_TO_INUM(vector)-INT_LVL_MAX].routine = routine;
        stSubVecTable[IVEC_TO_INUM(vector)-INT_LVL_MAX].arg = parameter;	
        VIC_INT_UNLOCK(lockkey);
        }
#else
        local_irq_save(flag); 
        stSubVecTable[IVEC_TO_INUM(vector)-INT_LVL_MAX].routine = routine;
        stSubVecTable[IVEC_TO_INUM(vector)-INT_LVL_MAX].arg = parameter;	
        local_irq_restore(flag);
#endif    
    }
    
	return s32Result;
}

/*****************************************************************************
 * 函 数 名  : BSP_INT_DisConnect
 *
 * 功能描述  : 取消注册某个中断
 *
 * 输入参数  : VOIDFUNCPTR * vector 中断向量号，取值范围0～40
 *  		 VOIDFUNCPTR routine  中断服务程序
 *           BSP_S32 parameter      中断服务程序参数
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 修改记录  : 2009年3月5日   zhanghailun  creat
 *****************************************************************************/
BSP_S32 BSP_INT_DisConnect  (VOIDFUNCPTR * vector,VOIDFUNCPTR routine, BSP_S32 parameter)
{
    BSP_S32 lockkey = 0;
    unsigned long flag;
    BSP_S32 s32Result = OK;

#if (defined (BOARD_FPGA_P500) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
    if(IVEC_TO_INUM(vector) >= (INT_LVL_MAX + SUBVIC_NUM + GUFPGA_VIC_NUM))
#else 
    if(IVEC_TO_INUM(vector) >= (INT_LVL_MAX + SUBVIC_NUM))
#endif
	{
		return 	ERROR;
	}

    if(IVEC_TO_INUM(vector) < INT_LVL_MAX)
    {
        free_irq(vector, NULL);
        s32Result = OK;
    }
    else
    {
#if (defined (BOARD_FPGA_P500) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
        if(IVEC_TO_INUM(vector) >= (INT_LVL_MAX + SUBVIC_NUM))
        {
            BSP_GUFPGA_IntDisConnect(IVEC_TO_INUM(vector) - (INT_LVL_MAX + SUBVIC_NUM), routine, parameter);
        }
        else
        {
        VIC_INT_LOCK(lockkey);
        stSubVecTable[IVEC_TO_INUM(vector)-INT_LVL_MAX].routine = NULL;
        stSubVecTable[IVEC_TO_INUM(vector)-INT_LVL_MAX].arg = NULL;	
        VIC_INT_UNLOCK(lockkey);
        }
#else
        local_irq_save(flag); 
        stSubVecTable[IVEC_TO_INUM(vector)-INT_LVL_MAX].routine = NULL;
        stSubVecTable[IVEC_TO_INUM(vector)-INT_LVL_MAX].arg = 0;	
        local_irq_restore(flag);
#endif    
    }
    
	return s32Result;
}

/*****************************************************************************
 * 函 数 名  : BSP_INT_Lock
 *
 * 功能描述  : 屏蔽所有中断
 *
 * 输入参数  : 无
 * 
 * 输出参数  : 无
 *
 * 返 回 值  : 中断锁号
 *
 * 修改记录  : 2009年3月5日   zhanghailun  creat
 *****************************************************************************/
BSP_S32 BSP_INT_Lock (BSP_VOID)
{
	unsigned long flag;

	local_irq_save(flag); 

	return flag;	
}

/*****************************************************************************
 * 函 数 名  : BSP_INT_UnLock
 *
 * 功能描述  : 打开中断
 *
 * 输入参数  : BSP_S32 ulLocKKey  中断锁号，前一次BSP_INT_Lock的返回值
 * 
 * 输出参数  : 无
 *
 * 返 回 值  : 中断锁号
 *
 * 修改记录  : 2009年3月5日   zhanghailun  creat
 *****************************************************************************/
BSP_VOID BSP_INT_UnLock ( BSP_S32 s32LocKKey)
{
	local_irq_restore(s32LocKKey);
}


#if (defined (BOARD_FPGA_P500) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
#else
/*test Dual Timer0*/
void __iomem *timer_base = NULL;
void testTimeInt()
{
    *(volatile int *)(timer_base + 0x0c) = 0x1;
    printk("testTimeInt\n");
}

void testTimeInit()
{
    if (!timer_base)
    {
        timer_base = ioremap(FPGA_DTIMER0_BASE, FPGA_DTIMER0_SIZE);
    }
	if (!timer_base)
	{
		printk(KERN_ERR "timer ioremap error.\n");
		return;
	}
	
    BSP_INT_Connect(INT_LVL_MAX + 20, testTimeInt, 0);
    BSP_INT_Enable(INT_LVL_MAX + 20);
    *(volatile int *)timer_base = 0x10000;
    *(volatile int *)(timer_base + 8) = 0xe2;
}
#endif

arch_initcall(BSP_DRV_SubVicInit);

