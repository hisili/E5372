
#include <linux/module.h>
#include <asm/io.h>
#include <asm/string.h>
#include <linux/semaphore.h>
#include <linux/interrupt.h>

#include "BSP.h"
#include "bsp_drv_ipc.h"

#ifdef __cplusplus
extern "C" {
#endif

BSP_IPC_ENTRY	stIpcIntTable[INTSRC_NUM];
static IPC_DEV_S	g_stIpcDev = {0};
BSP_U32			g_CoreNum;

struct semaphore	g_semIpcTask[INTSRC_NUM];
IPC_DEBUG_E		g_stIpc_debug = {0};

/* base address of ipc registers */
void __iomem *ipc_base = NULL;

static irqreturn_t BSP_DRV_IpcIntHandler(int irq, void *dev_id);
static irqreturn_t BSP_IPC_SemIntHandler(int irq, void *dev_id);

#if defined (BOARD_FPGA) && defined (VERSION_V3R2)
BSP_VOID BSP_DRV_IpcIntCombine()
{
    BSP_DRV_IpcIntHandler(0, 0);
    BSP_IPC_SemIntHandler(0, 0);
}
#endif


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
BSP_S32 BSP_DRV_IPCIntInit(void)
{
	BSP_S32 ret = 0;

	if (BSP_TRUE == g_stIpcDev.bInit)
	{
		return BSP_OK;
	}

	g_CoreNum = IPC_CORE_APPARM;

    /*modified for lint e665 */
	memset((void*)stIpcIntTable, 0x0, (sizeof(BSP_IPC_ENTRY) *INTSRC_NUM));

	ipc_base = ioremap(BSP_IPC_BASE_ADDR, IPC_REG_SIZE);
	if (!ipc_base)
	{
		printk(KERN_ERR "ipc ioremap error.\n");
		return -1;
	}

	BSP_REG_WRITE(ipc_base, BSP_IPC_CPU_INT_MASK(g_CoreNum), 0x0);
	BSP_REG_WRITE(ipc_base, BSP_IPC_SEM_INT_MASK(g_CoreNum), 0x0);

	#if defined (BOARD_FPGA) && defined (VERSION_V3R2)
    /* 	�ҽ��ܵ��жϷ�����򣬰����жϻ������жϷ��������ź����ͷ��жϷ������*/
	ret = request_irq(INT_LEV_IPC_COMBINE, BSP_DRV_IpcIntCombine, 0, "ipc_irq", NULL);
	if (ret ) {
		printk(KERN_ERR "BSP_DRV_IPCIntInit: Unable to register ipc irq ret=%d.\n", ret);
		return BSP_ERROR;
	}
	#else

	/* 	�ҽ��ܵ��жϷ�����򣬰����жϻ������жϷ��������ź����ͷ��жϷ������*/
	ret = request_irq(INT_LEV_IPC_CPU, BSP_DRV_IpcIntHandler, 0, "ipc_irq", NULL);
	if (ret ) {
		printk(KERN_ERR "BSP_DRV_IPCIntInit: Unable to register ipc irq ret=%d.\n", ret);
		return BSP_ERROR;
	}

	ret = request_irq(INT_LEV_IPC_SEM, BSP_IPC_SemIntHandler, 0, "ipc_sem", NULL);
	if (ret ) {
		printk(KERN_ERR "BSP_DRV_IPCIntInit: Unable to register sem irq ret=%d.\n", ret);
		return BSP_ERROR;
	}
	#endif

	g_stIpcDev.bInit = BSP_TRUE;

	return BSP_OK;
}

/*****************************************************************************
* �� �� ��  : BSP_IPC_SemCreate
*
* ��������  : ��ʼ���ź���
*
* �������  :   BSP_U32 u32SignalNum Ҫ��ʼ�����ź������
                
* �������  : ��
*
* �� �� ֵ  : ��
*
* �޸ļ�¼  : 2011��4��11�� wangjing creat
*****************************************************************************/
BSP_S32 BSP_IPC_SemCreate(BSP_U32 u32SignalNum)
{
    IPC_CHECK_PARA_NO_RET(u32SignalNum);
	
	sema_init(&g_semIpcTask[u32SignalNum], SEM_EMPTY);

	return BSP_OK;
}

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
* �޸ļ�¼  : 2011��4��11�� wangjing creat
*****************************************************************************/
BSP_S32 BSP_IPC_IntEnable (IPC_INT_LEV_E ulLvl)
{
	BSP_U32 u32IntMask;

	/*�������*/
	IPC_CHECK_PARA(ulLvl);

	/*д�ж����μĴ���*/
	BSP_REG_READ(ipc_base, BSP_IPC_CPU_INT_MASK(g_CoreNum), u32IntMask);
	u32IntMask |= 1 << ulLvl;
	BSP_REG_WRITE(ipc_base,BSP_IPC_CPU_INT_MASK(g_CoreNum), u32IntMask);

	return BSP_OK;
}/*lint !e550*/


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
 * �޸ļ�¼  : 2011��4��11�� wangjing creat
 *****************************************************************************/
BSP_S32 BSP_IPC_IntDisable (IPC_INT_LEV_E ulLvl)
{
	BSP_U32 u32IntMask;

	/*�������*/
	IPC_CHECK_PARA(ulLvl);

	/*д�ж����μĴ���*/
	BSP_REG_READ(ipc_base, BSP_IPC_CPU_INT_MASK(g_CoreNum), u32IntMask);
	u32IntMask = (BSP_U32)(u32IntMask & (~(1 << ulLvl)));/*lint !e502*/
	BSP_REG_WRITE(ipc_base,BSP_IPC_CPU_INT_MASK(g_CoreNum), u32IntMask);

	return BSP_OK;
}

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
 * �޸ļ�¼  : 2011��4��11�� wangjing creat
 *****************************************************************************/
BSP_S32 BSP_IPC_IntConnect  (IPC_INT_LEV_E ulLvl, VOIDFUNCPTR routine, BSP_U32 parameter)
{
	/*�������*/
	IPC_CHECK_PARA(ulLvl);

	stIpcIntTable[ulLvl].routine = routine;
	stIpcIntTable[ulLvl].arg = parameter;

	return BSP_OK;
}

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
 * �޸ļ�¼  : 2011��4��11�� wangjing creat
 *****************************************************************************/
 BSP_S32 BSP_IPC_IntDisonnect  (IPC_INT_LEV_E ulLvl,VOIDFUNCPTR routine, BSP_U32 parameter)
 {
	unsigned long flag = 0;
	
	/*�������*/
	IPC_CHECK_PARA(ulLvl);

    /*����������Ͳ��Ե�����-- linux�ں�*/
    /*lint -e516*/
	local_irq_save(flag);
	stIpcIntTable[ulLvl].routine = NULL;
	stIpcIntTable[ulLvl].arg = 0;
	local_irq_restore(flag);
	/*lint +e516*/

	return BSP_OK;
 }

BSP_S32 ffsLsb(BSP_S32 args)
{
	BSP_S32 num = 0;
	BSP_S32 s32ImpVal = args;

	for(;;)
	{
		num++;

		if (0x1 == (s32ImpVal & 0x1))
		{
			break;
		}
		s32ImpVal = (BSP_S32)((BSP_U32)s32ImpVal >> 1);
	}

	return num;
}

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
 * �޸ļ�¼  : 2011��4��11�� wangjing creat
 *****************************************************************************/
static irqreturn_t BSP_DRV_IpcIntHandler(int irq, void *dev_id)
{
	BSP_S32 retval = IRQ_HANDLED;
	BSP_U32 u32IntStat = 0;
	BSP_U32 i;
	BSP_U32 u32Date = 0x1;
	BSP_U32 u32BitValue = 0;

	BSP_REG_READ(ipc_base, BSP_IPC_CPU_INT_STAT(g_CoreNum), u32IntStat);

	//newLevel = ffsLsb (u32IntStat);
	//--newLevel;		/* ffsLsb returns numbers from 1, not 0 */

	/*���ж�*/
	BSP_REG_WRITE(ipc_base, BSP_IPC_CPU_INT_CLR(g_CoreNum), u32IntStat);

	 /* ����32���ж� */
    
    for (i = 0; i < INTSRC_NUM; i++)
    {   
        if (0 != i)
        {
            u32Date <<= 1;
        }        

        u32BitValue = u32IntStat & u32Date;
       
        /* ������ж� ,����ö�Ӧ�жϴ����� */
        if (0 != u32BitValue)
        {  
            /*����ע����жϴ�����*/
			if (NULL != stIpcIntTable[i].routine)
			{
				stIpcIntTable[i].routine(stIpcIntTable[i].arg);
			}
			else
			{
				printk(KERN_ERR "BSP_DRV_IpcIntHandler:No IntConnect,ERROR!.\n");
			}
			g_stIpc_debug.u32IntHandleTimes[i]++;
        }
    }	
	return IRQ_RETVAL(retval);/*lint !e64*/
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
* �޸ļ�¼  : 2011��4��11�� wangjing creat
*****************************************************************************/
BSP_S32 BSP_IPC_IntSend(IPC_INT_CORE_E enDstCore, IPC_INT_LEV_E ulLvl)
{
	/*�������*/
	IPC_CHECK_PARA(ulLvl);

	/*дԭʼ�жϼĴ���,�����ж�*/
	BSP_REG_WRITE(ipc_base,BSP_IPC_CPU_RAW_INT(enDstCore), 1 << ulLvl);
	g_stIpc_debug.u32RecvIntCore = enDstCore;
	g_stIpc_debug.u32IntSendTimes[ulLvl]++;

	return BSP_OK;
}

/*****************************************************************************
 * �� �� ��  : BSP_MaskInt
 *
 * ��������  : �����ź���������ж��ͷżĴ���
 *
 * �������  : 
                BSP_U32 ulLvl Ҫ���͵��жϺţ�ȡֵ��Χ0��31  
 * �������  : ��
 *
 * �� �� ֵ  : ��
 *
 * �޸ļ�¼  : 2011��4��11�� wangjing creat
 *****************************************************************************/

BSP_VOID BSP_MaskInt(BSP_U32 u32SignalNum)
{
	BSP_U32 u32IntMask;
	IPC_CHECK_PARA_NO_RET(u32SignalNum);

	BSP_REG_READ(ipc_base, BSP_IPC_SEM_INT_MASK(g_CoreNum), u32IntMask);
	u32IntMask = (BSP_U32)(u32IntMask & (~(1 << u32SignalNum))); /*lint !e502*/
	BSP_REG_WRITE(ipc_base, BSP_IPC_SEM_INT_MASK(g_CoreNum), u32IntMask);
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
 * �޸ļ�¼  : 2011��4��11�� wangjing creat
 *****************************************************************************/
 BSP_S32 BSP_IPC_SemTake(BSP_U32 u32SignalNum,BSP_S32 s32timeout)
 {
	BSP_U32 u32HsCtrl,u32IntMask;    

	/*�������*/
	IPC_CHECK_PARA(u32SignalNum);

	/*��������ź�����Ӧ���ͷ��ж�����*/
	BSP_REG_WRITE(ipc_base, BSP_IPC_SEM_INT_CLR(g_CoreNum), 1 << u32SignalNum);
    for(;;)  //b00198513 Modified for pclint e716          
	{
		BSP_REG_READ(ipc_base, BSP_IPC_HS_CTRL(g_CoreNum, u32SignalNum), u32HsCtrl);
		if (0 == u32HsCtrl)
		{
			/*�ź�����ռ�ɹ�,��������˵��ź����ͷ��ж�*/
			BSP_MaskInt(u32SignalNum); 
			g_stIpc_debug.u32SemId = u32SignalNum;
			g_stIpc_debug.u32SemTakeTimes[u32SignalNum]++;/*lint !e661*/   
			break;
		}
		else
		{
			/*ʹ���ź����ͷ��ж�*/
			BSP_REG_READ(ipc_base, BSP_IPC_SEM_INT_MASK(g_CoreNum), u32IntMask);
			u32IntMask = (BSP_U32)(u32IntMask | ((1 << u32SignalNum)));
			BSP_REG_WRITE(ipc_base, BSP_IPC_SEM_INT_MASK(g_CoreNum), u32IntMask);
			//g_SemTakeTimes++;
			if (0 != down_timeout(&g_semIpcTask[u32SignalNum], msecs_to_jiffies(s32timeout * 10)))  /*lint !e732*/
			{
				/*ȥʹ���ź����ͷ��ж�*/
				BSP_MaskInt(u32SignalNum);
				//BSP_REG_WRITE(ipc_base, BSP_IPC_SEM_INT_MASK(g_CoreNum), 0);
				printk(KERN_ERR "semTake timeout!\n");
				return BSP_ERROR;
			}
		}
	}

	return BSP_OK;
 }
 
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
 * �޸ļ�¼  : 2011��4��11�� wangjing creat
 *****************************************************************************/
 BSP_VOID BSP_IPC_SemGive(BSP_U32 u32SignalNum)
 {
	IPC_CHECK_PARA_NO_RET(u32SignalNum);
	
	/*���ź�������Ĵ�����0*/
	BSP_REG_WRITE(ipc_base, BSP_IPC_HS_CTRL(g_CoreNum, u32SignalNum), 0);
	g_stIpc_debug.u32SemGiveTimes[u32SignalNum]++;
 }

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
 * �޸ļ�¼  : 2011��4��11�� wangjing creat
 *****************************************************************************/
static irqreturn_t BSP_IPC_SemIntHandler(int irq, void *dev_id)
{
 	BSP_S32 retval = IRQ_HANDLED;
	BSP_U32 u32SNum, u32IntStat;

	BSP_REG_READ(ipc_base, BSP_IPC_SEM_INT_STAT(g_CoreNum), u32IntStat);

	/*������ź����ͷ��жϣ�������ж�*/
	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
	u32SNum = (BSP_U32)ffsLsb ((BSP_S32)u32IntStat);
	/*end*/
	BSP_REG_WRITE(ipc_base, BSP_IPC_SEM_INT_CLR(g_CoreNum), 1 << --u32SNum);

	up(&g_semIpcTask[u32SNum]);
	g_stIpc_debug.u32SemHandleTimes[u32SNum]++;

	return IRQ_RETVAL(retval);/*lint !e64*/
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
* �޸ļ�¼  : 2011��4��11�� wangjing creat
*****************************************************************************/
BSP_VOID BSP_IPC_SpinLock (BSP_U32 u32SignalNum)
{
	BSP_U32 u32HsCtrl;
	
	IPC_CHECK_PARA_NO_RET(u32SignalNum);
	for(;;)
	{
		BSP_REG_READ(ipc_base, BSP_IPC_HS_CTRL(g_CoreNum, u32SignalNum), u32HsCtrl);
		if (0 == u32HsCtrl)
		{
			break;
		}
	}
}

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
* �޸ļ�¼  : 2011��4��11�� wangjing creat
*****************************************************************************/
BSP_VOID BSP_IPC_SpinUnLock (BSP_U32 u32SignalNum)
{
	IPC_CHECK_PARA_NO_RET(u32SignalNum);
	
	/*���ź�������Ĵ�����0*/
	BSP_REG_WRITE(ipc_base, BSP_IPC_HS_CTRL(g_CoreNum, u32SignalNum), 0);
}



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
* �޸ļ�¼  : 2011��4��11�� wangjing creat
*****************************************************************************/
BSP_VOID BSP_IPC_DebugShow(void)
{
	BSP_U32 i;

	printk("\n��ǰռ�õ��ź���IDΪ       : \t%d\n", g_stIpc_debug.u32SemId);
	for (i = 0; i < INTSRC_NUM; i++)
	{
		printk("�ź���%d��ȡ����             : \t%d\n", i, g_stIpc_debug.u32SemTakeTimes[i]);
		printk("�ź���%d�ͷŴ���             : \t%d\n", i, g_stIpc_debug.u32SemGiveTimes[i]);
		printk("�����ź����жϴ���Ĵ���Ϊ   : \t%d\n", g_stIpc_debug.u32SemHandleTimes[i]);
		printk("�����жϵĴ���Ϊ             : \t%d\n", g_stIpc_debug.u32IntSendTimes[i]);
		printk("�����жϴ���Ĵ���Ϊ         : \t%d\n", g_stIpc_debug.u32IntHandleTimes[i]);
	}

	printk("�����жϵ�Core IDΪ          : \t%d\n", g_stIpc_debug.u32RecvIntCore);
}

EXPORT_SYMBOL(BSP_IPC_IntEnable);
EXPORT_SYMBOL(BSP_IPC_IntDisable);
EXPORT_SYMBOL(BSP_IPC_IntConnect);
EXPORT_SYMBOL(BSP_IPC_IntSend);
EXPORT_SYMBOL(BSP_IPC_SemCreate);
EXPORT_SYMBOL(BSP_IPC_SemTake);
EXPORT_SYMBOL(BSP_IPC_SemGive);
EXPORT_SYMBOL(BSP_IPC_SpinLock);
EXPORT_SYMBOL(BSP_IPC_SpinUnLock);

#ifdef __cplusplus
}
#endif

