/*************************************************************************
*   ��Ȩ����(C) 1987-2004, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  GUIpc.h
*
*   ��    �� :  c61362
*
*   ��    �� :  IPCģ���û��ӿ��ļ�
*
*   �޸ļ�¼ :  2012��2��4��  v1.00  c61362  ����
*************************************************************************/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/semaphore.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/kthread.h>
#include <linux/freezer.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <asm/mach/irq.h>
#include <linux/sched.h>
#include <mach/hardware.h>
#include <mach/spinLockAmp.h>
#include <mach/common/mem/bsp_mem.h>
#include <mach/balong_v100r001.h>
#include "BSP.h"
#include "bsp_guvic.h"
#include "bsp_guipc.h"

#ifdef __cplusplus
extern "C" {
#endif

#if ((defined (CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)) && defined (BOARD_FPGA_P500))       /*ֻ��V7R1 FPGAƽ̨���ṩ*/

BSP_GUIPC_ENTRY   g_stGUIpcIntTable[INTSRC_NUM];
BSP_U32           g_ulGUIpcInitFlag = BSP_FALSE;
BSP_U32           g_ulGUIpcCoreNum;
struct semaphore  g_semGUIpcTask[INTSRC_NUM];
IPC_DEBUG_E       g_stGUIpcDebug = {0}; 
void __iomem      *g_pGuIpcBase = NULL;

/*****************************************************************************
* �� �� ��  : BSP_DRV_IPCIntInit
*
* ��������  : IPCģ���ʼ��
*
* �������  : ��  
* �������  : ��
*
* �� �� ֵ  : ��
*
* �޸ļ�¼  : 2009��3��5��   wangjing  creat
*****************************************************************************/
BSP_S32 BSP_GUIPC_Init()
{    
    if(BSP_TRUE == g_ulGUIpcInitFlag)
    {
        return BSP_OK;
    }

    g_ulGUIpcCoreNum = GUIPC_CORE_APPARM;
    
    memset((void*)g_stGUIpcIntTable,0x0,sizeof(BSP_GUIPC_ENTRY) *INTSRC_NUM);

    g_pGuIpcBase = ioremap(BSP_IPC_BASE_ADDR, BSP_IPC_SIZE);
	if (!g_pGuIpcBase)
	{
		printk(KERN_ERR "gusipc ioremap error.\n");
		return BSP_ERROR;
	}

    BSP_REG_WRITE(g_pGuIpcBase, BSP_IPC_CPU_INT_MASK(g_ulGUIpcCoreNum), 0x0);
    BSP_REG_WRITE(g_pGuIpcBase, BSP_IPC_SEM_INT_MASK(g_ulGUIpcCoreNum), 0x0);

    /* 	�ҽ��ܵ��жϷ�����򣬰����жϻ������жϷ��������ź����ͷ��жϷ������*/
    (BSP_VOID)BSP_INT_Connect((VOIDFUNCPTR *)INT_LEV_IPC_COMBINE, (VOIDFUNCPTR)BSP_GUIPC_IntCombine, (BSP_S32)NULL);
    
    /* ʹ���жϻ�����Ӧ�Ŀ����ж�*/
    (BSP_VOID)BSP_INT_Enable(INT_LEV_IPC_COMBINE);

    g_ulGUIpcInitFlag = BSP_TRUE;
    
    return BSP_OK;   
}
EXPORT_SYMBOL(BSP_GUIPC_Init);


BSP_VOID BSP_GUIPC_IntCombine()
{
    BSP_GUIPC_IpcIntHandler();
    BSP_GUIPC_SemIntHandler();
}


/*****************************************************************************
* �� �� ��  : BSP_IPC_SemCreate
*
* ��������  : �����ź���
*
* �������  :   BSP_U32 u32SignalNum Ҫ�������ź������
                
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* �޸ļ�¼  : 2012��2��4�� c61362 creat
*****************************************************************************/
BSP_S32 BSP_GUIPC_SemCreate(BSP_U32 u32SignalNum)
{
    IPC_CHECK_PARA(u32SignalNum);
    sema_init(&g_semGUIpcTask[u32SignalNum], SEM_EMPTY);
   
    return BSP_OK;
}
EXPORT_SYMBOL(BSP_GUIPC_SemCreate);

/*****************************************************************************
* �� �� ��  : BSP_IPC_SemDelete
*
* ��������  : ɾ���ź���
*
* �������  :   BSP_U32 u32SignalNum Ҫɾ�����ź������
                
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* �޸ļ�¼  : 2012��2��4�� c61362 creat
*****************************************************************************/
BSP_S32 BSP_GUIPC_SemDelete(BSP_U32 u32SignalNum)
{
    IPC_CHECK_PARA(u32SignalNum);
    
    return BSP_OK;
}
EXPORT_SYMBOL(BSP_GUIPC_SemDelete);


/*****************************************************************************
* �� �� ��  : BSP_IPC_IntEnable
*
* ��������  : ʹ��ĳ���ж�
*
* �������  :   IPC_INT_CORE_E enCoreNum Ҫʹ���жϵ�core
                BSP_U32 ulLvl Ҫʹ�ܵ��жϺţ�ȡֵ��Χ0��31  
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* �޸ļ�¼  : 2012��2��4�� c61362 creat
*****************************************************************************/
BSP_S32 BSP_GUIPC_IntEnable (IPC_INT_LEV_E ulLvl)
{
    BSP_U32 u32IntMask;

    /*�������*/
    IPC_CHECK_PARA(ulLvl);
    
    /*д�ж����μĴ���*/
    BSP_REG_READ(g_pGuIpcBase, BSP_IPC_CPU_INT_MASK(g_ulGUIpcCoreNum), u32IntMask);
    u32IntMask |= 1 << ulLvl;
    BSP_REG_WRITE(g_pGuIpcBase,BSP_IPC_CPU_INT_MASK(g_ulGUIpcCoreNum), u32IntMask);

    return BSP_OK;
}
EXPORT_SYMBOL(BSP_GUIPC_IntEnable);


/*****************************************************************************
 * �� �� ��  : BSP_INT_Disable
 *
 * ��������  : ȥʹ��ĳ���ж�
 *
 * �������  : IPC_INT_CORE_E enCoreNum Ҫʹ���жϵ�core 
                BSP_U32 ulLvl Ҫʹ�ܵ��жϺţ�ȡֵ��Χ0��31   
 * �������  : ��
 *
 * �� �� ֵ  : OK&ERROR
 *
 * �޸ļ�¼  : 2012��2��4�� c61362 creat
 *****************************************************************************/
BSP_S32 BSP_GUIPC_IntDisable (IPC_INT_LEV_E ulLvl)
{
    BSP_U32 u32IntMask;

    /*�������*/
    IPC_CHECK_PARA(ulLvl);

    /*д�ж����μĴ���*/
    BSP_REG_READ(g_pGuIpcBase, BSP_IPC_CPU_INT_MASK(g_ulGUIpcCoreNum), u32IntMask);
    u32IntMask = (BSP_U32)(u32IntMask & (~(1 << ulLvl)));/*lint !e502*/
    BSP_REG_WRITE(g_pGuIpcBase, BSP_IPC_CPU_INT_MASK(g_ulGUIpcCoreNum), u32IntMask);

    return BSP_OK;
}
EXPORT_SYMBOL(BSP_GUIPC_IntDisable);

/*****************************************************************************
 * �� �� ��  : BSP_IPC_IntConnect
 *
 * ��������  : ע��ĳ���ж�
 *
 * �������  : IPC_INT_CORE_E enCoreNum Ҫʹ���жϵ�core 
               BSP_U32 ulLvl Ҫʹ�ܵ��жϺţ�ȡֵ��Χ0��31 
               VOIDFUNCPTR routine �жϷ������
 *             BSP_U32 parameter      �жϷ���������
 * �������  : ��
 *
 * �� �� ֵ  : OK&ERROR
 *
 * �޸ļ�¼  : 2012��2��4�� c61362 creat
 *****************************************************************************/
BSP_S32 BSP_GUIPC_IntConnect  (IPC_INT_LEV_E ulLvl,VOIDFUNCPTR routine, BSP_U32 parameter)
{
    BSP_S32 s32lockkey;
    
    /*�������*/
    IPC_CHECK_PARA(ulLvl);

    VIC_INT_LOCK(s32lockkey);
    g_stGUIpcIntTable[ulLvl].routine = routine;
    g_stGUIpcIntTable[ulLvl].arg = parameter;
    VIC_INT_UNLOCK(s32lockkey);

    return BSP_OK;
    
}
EXPORT_SYMBOL(BSP_GUIPC_IntConnect);

/*****************************************************************************
 * �� �� ��  : BSP_IPC_IntDisonnect
 *
 * ��������  : ȡ��ע��ĳ���ж�
 *
 * �������  : 
 *              BSP_U32 ulLvl Ҫʹ�ܵ��жϺţ�ȡֵ��Χ0��31 
 *              VOIDFUNCPTR routine �жϷ������
 *             BSP_U32 parameter      �жϷ���������
 * �������  : ��
 *
 * �� �� ֵ  : OK&ERROR
 *
 * �޸ļ�¼  : 2012��2��4�� c61362 creat
 *****************************************************************************/
BSP_S32 BSP_GUIPC_IntDisonnect  (IPC_INT_LEV_E ulLvl,VOIDFUNCPTR routine, BSP_U32 parameter)
{
    BSP_S32 s32lockkey;
    /*�������*/
    IPC_CHECK_PARA(ulLvl);

    VIC_INT_LOCK(s32lockkey);
    g_stGUIpcIntTable[ulLvl].routine = NULL;
    g_stGUIpcIntTable[ulLvl].arg = NULL;
    VIC_INT_UNLOCK(s32lockkey);

    return BSP_OK;
}
EXPORT_SYMBOL(BSP_GUIPC_IntDisonnect);

/*****************************************************************************
 * �� �� ��  : BSP_DRV_IpcIntHandler
 *
 * ��������  : �жϴ�����
 *
 * �������  : ��  
 * �������  : ��
 *
 * �� �� ֵ  : ��
 *
 * �޸ļ�¼  : 2012��2��4�� c61362 creat
 *****************************************************************************/
BSP_VOID BSP_GUIPC_IpcIntHandler()
{
    BSP_U32 newLevel = 0;
    BSP_U32 u32IntStat = 0;

    BSP_REG_READ(g_pGuIpcBase, BSP_IPC_CPU_INT_STAT(g_ulGUIpcCoreNum),u32IntStat);

    for(newLevel = 0; newLevel < 32; newLevel++)
    {
        if(u32IntStat&(1 << newLevel))
        {
            break;
        }
    }

    if(newLevel == 32)
    {
        return;
    }

    /*���ж�*/
    BSP_REG_WRITE(g_pGuIpcBase, BSP_IPC_CPU_INT_CLR(g_ulGUIpcCoreNum),1 << newLevel);
    g_stGUIpcDebug.u32IntHandleTimes[newLevel]++;

    /*����ע����жϴ�����*/
    if(NULL != g_stGUIpcIntTable[newLevel].routine)
    {
        g_stGUIpcIntTable[newLevel].routine(g_stGUIpcIntTable[newLevel].arg);
    }	
}

 /*****************************************************************************
* �� �� ��  : BSP_IPC_IntSend
*
* ��������  : �����ж�
*
* �������  : 
                IPC_INT_CORE_E enDstore Ҫ�����жϵ�core
                BSP_U32 ulLvl Ҫ���͵��жϺţ�ȡֵ��Χ0��31  
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* �޸ļ�¼  : 2012��2��4�� c61362 creat
*****************************************************************************/

BSP_S32 BSP_GUIPC_IntSend(IPC_INT_CORE_E enDstCore, IPC_INT_LEV_E ulLvl)
{
    /*�������*/
    IPC_CHECK_PARA(ulLvl);
       
    /*дԭʼ�жϼĴ���,�����ж�*/
    BSP_REG_WRITE(g_pGuIpcBase, BSP_IPC_CPU_RAW_INT(enDstCore), 1 << ulLvl);
    g_stGUIpcDebug.u32RecvIntCore = enDstCore;
    g_stGUIpcDebug.u32IntSendTimes[ulLvl]++;

    return BSP_OK;
    
}
EXPORT_SYMBOL(BSP_GUIPC_IntSend);

/*****************************************************************************
 * �� �� ��  : BSP_GUIPC_MaskInt
 *
 * ��������  : �����ź���������ж��ͷżĴ���
 *
 * �������  : 
                BSP_U32 ulLvl Ҫ���͵��жϺţ�ȡֵ��Χ0��31  
 * �������  : ��
 *
 * �� �� ֵ  : ��
 *
 * �޸ļ�¼  : 2012��2��4�� c61362 creat
 *****************************************************************************/

BSP_VOID BSP_GUIPC_MaskInt(BSP_U32 u32SignalNum)
{
    BSP_U32 u32IntMask;
    
    if(u32SignalNum >= INTSRC_NUM)
    {
        printk("BSP_GUIPC_MaskInt  Parameter error.\n");
        return;
    }
    BSP_REG_READ(g_pGuIpcBase, BSP_IPC_SEM_INT_MASK(g_ulGUIpcCoreNum), u32IntMask);
    u32IntMask = (BSP_U32)(u32IntMask & (~(1 << u32SignalNum))); /*lint !e502*/
    BSP_REG_WRITE(g_pGuIpcBase, BSP_IPC_SEM_INT_MASK(g_ulGUIpcCoreNum), u32IntMask);
}

/*****************************************************************************
 * �� �� ��  : BSP_IPC_SemTake
 *
 * ��������  : ��ȡ�ź���
 *
 * �������  : ��  
 * �������  : ��
 *
 * �� �� ֵ  : ��
 *
 * �޸ļ�¼  : 2012��2��4�� c61362 creat
 *****************************************************************************/
BSP_S32 BSP_GUIPC_SemTake(BSP_U32 u32SignalNum,BSP_S32 s32timeout)
{
    BSP_U32 u32HsCtrl,u32IntMask;    

    /*�������*/
    IPC_CHECK_PARA(u32SignalNum);

    /*��������ź�����Ӧ���ͷ��ж�����*/
    BSP_REG_WRITE(g_pGuIpcBase, BSP_IPC_SEM_INT_CLR(g_ulGUIpcCoreNum), 1 << u32SignalNum);
    while(1)
    {
        BSP_REG_READ(g_pGuIpcBase, BSP_IPC_HS_CTRL(g_ulGUIpcCoreNum, u32SignalNum), u32HsCtrl);
        if(0 == u32HsCtrl)
        {
            /*�ź�����ռ�ɹ�,��������˵��ź����ͷ��ж�*/
            BSP_GUIPC_MaskInt(u32SignalNum); 
            g_stGUIpcDebug.u32SemId = u32SignalNum;
            g_stGUIpcDebug.u32SemTakeTimes[u32SignalNum]++;/*lint !e661*/   
            break;
        }
        else
        {
            if(0 == s32timeout)
            {
            	return BSP_ERROR;
            }
        
            /*ʹ���ź����ͷ��ж�*/
            BSP_REG_READ(g_pGuIpcBase, BSP_IPC_SEM_INT_MASK(g_ulGUIpcCoreNum), u32IntMask);
            u32IntMask = (BSP_U32)(u32IntMask | ((1 << u32SignalNum)));
            BSP_REG_WRITE(g_pGuIpcBase, BSP_IPC_SEM_INT_MASK(g_ulGUIpcCoreNum), u32IntMask);

            if(0 != down_timeout(&g_semGUIpcTask[u32SignalNum], msecs_to_jiffies(s32timeout*10)))
            {
                /*ȥʹ���ź����ͷ��ж�*/
                BSP_GUIPC_MaskInt(u32SignalNum);
                //BSP_REG_WRITE(BSP_IPC_BASE_ADDR, BSP_IPC_SEM_INT_MASK(g_ulGUIpcCoreNum), 0);
                printk("semTake timeout!\n");
                return BSP_ERROR;
            }
      }
     
   }

    return BSP_OK;
}
EXPORT_SYMBOL(BSP_GUIPC_SemTake);

/*****************************************************************************
 * �� �� ��  : BSP_IPC_SemGive
 *
 * ��������  : �ͷ��ź���
 *
 * �������  : ��  
 * �������  : ��
 *
 * �� �� ֵ  : ��
 *
 * �޸ļ�¼  : 2012��2��4�� c61362 creat
 *****************************************************************************/
BSP_VOID BSP_GUIPC_SemGive(BSP_U32 u32SignalNum)
{
    if(u32SignalNum >= INTSRC_NUM)
    {
         printk("BSP_IPC_SpinLock  Parameter error.\n");
         return;
    }
    
    /*���ź�������Ĵ�����0*/
    BSP_REG_WRITE(g_pGuIpcBase, BSP_IPC_HS_CTRL(g_ulGUIpcCoreNum, u32SignalNum), 0);
    g_stGUIpcDebug.u32SemGiveTimes[u32SignalNum]++;

}
EXPORT_SYMBOL(BSP_GUIPC_SemGive);

/*****************************************************************************
* �� �� ��  : BSP_IPC_SemIntHandler
*
* ��������  : �ź����ͷ��жϴ�����
*
* �������  : ��  
* �������  : ��
*
* �� �� ֵ  : ��
*
* �޸ļ�¼  : 2012��2��4�� c61362 creat
*****************************************************************************/
BSP_VOID BSP_GUIPC_SemIntHandler(BSP_U32 u32SemNum)
{
    BSP_U32 u32SNum, u32IntStat;
    
    BSP_REG_READ(g_pGuIpcBase, BSP_IPC_SEM_INT_STAT(g_ulGUIpcCoreNum), u32IntStat);

    for(u32SNum = 0; u32SNum < 32; u32SNum++)
    {
        if(u32IntStat&(1 << u32SNum))
        {
            break;
        }
    }

    if(u32SNum == 32)
    {
        return;
    }
    BSP_REG_WRITE(g_pGuIpcBase, BSP_IPC_SEM_INT_CLR(g_ulGUIpcCoreNum), 1 << u32SNum);
   
    up(&g_semGUIpcTask[u32SNum]);
}

/*****************************************************************************
* �� �� ��  : BSP_IPC_SpinLock
*
* ��������  : ��ѯ�ȴ���ȡ�ź���
*
* �������  : ��  
* �������  : ��
*
* �� �� ֵ  : ��
*
* �޸ļ�¼  : 2012��2��4�� c61362 creat
*****************************************************************************/
BSP_VOID BSP_GUIPC_SpinLock (BSP_U32 u32SignalNum)
{
    BSP_U32 u32HsCtrl;
    
    if(u32SignalNum >= INTSRC_NUM)
    {
         printk("BSP_IPC_SpinLock  Parameter error.\n");
         return;
    }
    for(;;)
    {
        BSP_REG_READ(g_pGuIpcBase, BSP_IPC_HS_CTRL(g_ulGUIpcCoreNum, u32SignalNum), u32HsCtrl);
        if(0 == u32HsCtrl)
        {
            break;
        }
    }

}
EXPORT_SYMBOL(BSP_GUIPC_SpinLock);

/*****************************************************************************
* �� �� ��  : BSP_IPC_SpinUnLock
*
* ��������  : �ͷ��ź���
*
* �������  : ��  
* �������  : ��
*
* �� �� ֵ  : ��
*
* �޸ļ�¼  : 2012��2��4�� c61362 creat
*****************************************************************************/
BSP_VOID BSP_GUIPC_SpinUnLock (BSP_U32 u32SignalNum)
{
    if(u32SignalNum >= INTSRC_NUM)
    {
     printk("BSP_IPC_SpinUnLock  Parameter error.\n");
      return;
    }

    /*���ź�������Ĵ�����0*/
    BSP_REG_WRITE(g_pGuIpcBase, BSP_IPC_HS_CTRL(g_ulGUIpcCoreNum, u32SignalNum), 0);

}
EXPORT_SYMBOL(BSP_GUIPC_SpinUnLock);

/*****************************************************************************
* �� �� ��  : BSP_IPC_DebugShow
*
* ��������  : ��ά�ɲ�ӿ�
*
* �������  : ��  
* �������  : ��
*
* �� �� ֵ  : ��
*
* �޸ļ�¼  : 2012��2��4�� c61362 creat
*****************************************************************************/
BSP_VOID BSP_GUIPC_DebugShow()
{
    BSP_U32 i;
    
    printk("\n��ǰռ�õ��ź���IDΪ       : \t%d\n", g_stGUIpcDebug.u32SemId);
    for(i = 0; i < INTSRC_NUM; i++)
    {
        printk("�ź���%d��ȡ����             : \t%d\n", i,g_stGUIpcDebug.u32SemTakeTimes[i]);
        printk("�ź���%d�ͷŴ���             : \t%d\n", i,g_stGUIpcDebug.u32SemGiveTimes[i]);
        printk("�����жϵĴ���Ϊ             : \t%d\n", g_stGUIpcDebug.u32IntSendTimes[i]);
        printk("�����жϴ���Ĵ���Ϊ         : \t%d\n", g_stGUIpcDebug.u32IntHandleTimes[i]);
    }
    
    printk("�����жϵ�Core IDΪ          : \t%d\n", g_stGUIpcDebug.u32RecvIntCore);
}
EXPORT_SYMBOL(BSP_GUIPC_DebugShow);

BSP_S32 BSP_SGI_Connect(BSP_U32 ulLvl,VOIDFUNCPTR routine, BSP_U32 parameter)
{
    BSP_S32 ret;
    BSP_CHAR irq_name[10];

    sprintf(irq_name, "SGI_IRQ%d", ulLvl);
    ret = request_irq(ulLvl, routine, 0, irq_name, 0);
    if (ret ) {
    	printk(KERN_ERR "BSP_SGI_Connect: Unable to register irq ret=%d.\n", ret);
    	return ERROR;    
    }
    
	return OK;
}
EXPORT_SYMBOL(BSP_SGI_Connect);
#define ARM_GIC_IPI_COUNT		16
#define GIC_SW_INTERRUPT			(IO_ADDRESS(PBXA9_GIC_DIST_CONTROL) + 0xf00)
static int arm_gic_sgi_gen(unsigned int ipiID, unsigned int cpus)
{
	unsigned int val = 0;

	if (ipiID < 0 || ipiID >= ARM_GIC_IPI_COUNT || cpus == 0)
		return -1;

	val = readl(GIC_SW_INTERRUPT);
	val &= 0xff00fff0;
	val |= (cpus << 16 | ipiID);
	writel(val, GIC_SW_INTERRUPT);


	return 0;
}	
BSP_S32 BSP_SGI_IntSend(BSP_U32 ulLvl)
{
	int ret = 0;

	ret = arm_gic_sgi_gen(ulLvl, 0x1);
	if (ret)
	{
		printk(KERN_ERR "BSP_SGI_IntSend sgi gen failed.\n");
		return;
	}
    return OK;
}
EXPORT_SYMBOL(BSP_SGI_IntSend);
#endif

#ifdef __cplusplus
}
#endif

