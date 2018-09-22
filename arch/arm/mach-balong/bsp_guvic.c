#include <linux/init.h>
#include <asm/io.h>
#include <asm/string.h>
#include <linux/interrupt.h>
#include "BSP.h"
#include "bsp_guvic.h"

#if ((defined (CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)) && defined (BOARD_FPGA_P500))       /*ֻ��V7R1 FPGAƽ̨���ṩ*/

BSP_GUVIC_ENTRY g_stGUVicVecTable[GUFPGA_VIC_NUM];

/* base address of subvic registers */
void __iomem *g_pGUVicBase = NULL;

/*modify by wangjing 2011-4-7*/
UINT32 g_ulGUVicIntCheck=0;
VOIDFUNCPTR g_pGUVICIntHookPtr = (VOIDFUNCPTR) NULL;


/*****************************************************************************
* �� �� ��  : SubVic_IntHandler
*
* ��������  : �����ж��ܹҽӺ���
*
* �������  : ��  
* �������  : ��
*
* �� �� ֵ  : ��
*
* �޸ļ�¼  : 2009��3��5��   zhanghailun  creat
*****************************************************************************/
irqreturn_t BSP_GUFPGA_IntHandler(int irq, void *dev_id)
{
    unsigned int newLevel = 0;
    unsigned int isr = 0;

    BSP_REG_READ(GUFPGA_VIC_STATUS, 0, isr);


    g_ulGUVicIntCheck = 1;
    /* �ҵ�������ȼ����ж� */
    for(newLevel = 0; newLevel < GUFPGA_VIC_NUM; newLevel++)
    {
        if(isr&(1<<newLevel))
        {
            break;
        }
    }
    /* û���ж� */
    if (newLevel == GUFPGA_VIC_NUM)   
    {
        printk(KERN_ERR "BSP_GUFPGA_IntHandler: newLevel==0  Error!\n");
        return IRQ_HANDLED;
    }

    /*modify by wangjing 2011-4-7*/
    if (NULL != g_pGUVICIntHookPtr)
    {
        (BSP_VOID)g_pGUVICIntHookPtr(newLevel);
    }
	if(NULL != g_stGUVicVecTable[newLevel].routine)
	{
		g_stGUVicVecTable[newLevel].routine(g_stGUVicVecTable[newLevel].arg);            
	}	
    /*modify by wangjing 2011-4-7*/
	g_ulGUVicIntCheck = 0;
    if (NULL != g_pGUVICIntHookPtr)
    {
        (BSP_VOID)g_pGUVICIntHookPtr(newLevel);
    }

	return IRQ_HANDLED;
}

/*****************************************************************************
* �� �� ��  : SubVic_Init
*
* ��������  : �����жϳ�ʼ��
*
* �������  : ��  
* �������  : ��
*
* �� �� ֵ  : ��
*
* �޸ļ�¼  : 2009��3��5��   zhanghailun  creat
*****************************************************************************/
BSP_VOID BSP_GUFPGA_IntInit()
{
    BSP_U32 	i;
    int ret;

	for(i=0; i<GUFPGA_VIC_NUM; i++)
	{
		g_stGUVicVecTable[i].routine = NULL;
	}


    g_pGUVicBase = ioremap(GUFPGA_VIC_BASE, GUFPGA_VIC_SIZE);
    
	if (!g_pGUVicBase)
	{
		printk(KERN_ERR "GU FPGA vic ioremap error.\n");
		return;
	}


	BSP_REG_WRITE(GUFPGA_VIC_DISABLE, 0, 0x00);  

    ret = BSP_INT_Connect((VOIDFUNCPTR *)INT_LVL_GUFPGAVIC, (VOIDFUNCPTR)BSP_GUFPGA_IntHandler, (BSP_S32)0);
    ret |= BSP_INT_Enable(INT_LVL_GUFPGAVIC);
   
	if (ret ) {
		printk(KERN_ERR "BSP_INT_Connect: Unable to register GU FPGA vic, ret=%d.\n", ret);
	}

	printk("BSP_GUFPGAIntInit ok\n");

    return;

}

/*****************************************************************************
* �� �� ��  : BSP_INT_Enable
*
* ��������  : ʹ��ĳ���ж�
*
* �������  : BSP_S32 ulLvl Ҫʹ�ܵ��жϺţ�ȡֵ��Χ0��40  
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* �޸ļ�¼  : 2009��3��5��   zhanghailun  creat
*****************************************************************************/

BSP_S32 BSP_GUFPGA_IntEnable ( BSP_S32 s32Lvl)
{
    unsigned long flag;
    BSP_S32 tmp;
    
    if(s32Lvl >= GUFPGA_VIC_NUM)
	{
		return 	ERROR;
	}

    local_irq_save(flag);
    BSP_REG_READ(GUFPGA_VIC_ENABLE, 0, tmp);
    tmp |= (1<<s32Lvl);
    BSP_REG_WRITE(GUFPGA_VIC_ENABLE, 0, tmp);
    local_irq_restore(flag);
   
    return OK;
}/*lint !e529*/

/*****************************************************************************
 * �� �� ��  : BSP_INT_Disable
 *
 * ��������  : ȥʹ��ĳ���ж�
 *
 * �������  : BSP_S32 ulLvl Ҫʹ�ܵ��жϺţ�ȡֵ��Χ0��40 
 * �������  : ��
 *
 * �� �� ֵ  : OK&ERROR
 *
 * �޸ļ�¼  : 2009��3��5��   zhanghailun  creat
 *****************************************************************************/
BSP_S32 BSP_GUFPGA_IntDisable ( BSP_S32 s32Lvl)
{
    unsigned long flag;
    BSP_S32 tmp;
    
    if(s32Lvl >= GUFPGA_VIC_NUM)
    {
		return 	ERROR;
	}

    
    local_irq_save(flag);
    BSP_REG_READ(GUFPGA_VIC_DISABLE, 0, tmp);
    tmp |= (1<<s32Lvl);
    BSP_REG_WRITE(GUFPGA_VIC_DISABLE, 0, tmp);
    local_irq_restore(flag);
    
    return OK;	
}

/*****************************************************************************
 * �� �� ��  : BSP_INT_Connect
 *
 * ��������  : ע��ĳ���ж�
 *
 * �������  : VOIDFUNCPTR * vector �ж������ţ�ȡֵ��Χ0��40
 *  		 VOIDFUNCPTR routine  �жϷ������
 *           BSP_S32 parameter      �жϷ���������
 * �������  : ��
 *
 * �� �� ֵ  : OK&ERROR
 *
 * �޸ļ�¼  : 2009��3��5��   zhanghailun  creat
 *****************************************************************************/
BSP_S32 BSP_GUFPGA_IntConnect  (VOIDFUNCPTR * vector,VOIDFUNCPTR routine, BSP_S32 parameter)
{
    unsigned long flag;
    
    if(IVEC_TO_INUM(vector) >= GUFPGA_VIC_NUM)
	{
		return 	ERROR;
	}
	
    local_irq_save(flag); 
    g_stGUVicVecTable[IVEC_TO_INUM(vector)-INT_LVL_MAX].routine = routine;
    g_stGUVicVecTable[IVEC_TO_INUM(vector)-INT_LVL_MAX].arg = parameter;	
    local_irq_restore(flag);   
    
	return OK;
}

/*****************************************************************************
 * �� �� ��  : BSP_INT_DisConnect
 *
 * ��������  : ȡ��ע��ĳ���ж�
 *
 * �������  : VOIDFUNCPTR * vector �ж������ţ�ȡֵ��Χ0��40
 *  		 VOIDFUNCPTR routine  �жϷ������
 *           BSP_S32 parameter      �жϷ���������
 * �������  : ��
 *
 * �� �� ֵ  : OK&ERROR
 *
 * �޸ļ�¼  : 2009��3��5��   zhanghailun  creat
 *****************************************************************************/
BSP_S32 BSP_GUFPGA_IntDisConnect  (VOIDFUNCPTR * vector,VOIDFUNCPTR routine, BSP_S32 parameter)
{
    unsigned long flag;

    if(IVEC_TO_INUM(vector) >= GUFPGA_VIC_NUM)
	{
		return 	ERROR;
	}

    local_irq_save(flag); 
    g_stGUVicVecTable[IVEC_TO_INUM(vector)-INT_LVL_MAX].routine = NULL;
    g_stGUVicVecTable[IVEC_TO_INUM(vector)-INT_LVL_MAX].arg = 0;	
    local_irq_restore(flag);
    
	return OK;
}

#endif

