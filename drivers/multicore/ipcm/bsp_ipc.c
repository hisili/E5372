
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
* 函 数 名  : BSP_DRV_IPCIntInit
*
* 功能描述  : IPC模块初始化
*
* 输入参数  : 无  
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2009年3月5日   wangjing  creat
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
    /* 	挂接总的中断服务程序，包括中断互发的中断服务程序和信号量释放中断服务程序*/
	ret = request_irq(INT_LEV_IPC_COMBINE, BSP_DRV_IpcIntCombine, 0, "ipc_irq", NULL);
	if (ret ) {
		printk(KERN_ERR "BSP_DRV_IPCIntInit: Unable to register ipc irq ret=%d.\n", ret);
		return BSP_ERROR;
	}
	#else

	/* 	挂接总的中断服务程序，包括中断互发的中断服务程序和信号量释放中断服务程序*/
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
* 函 数 名  : BSP_IPC_SemCreate
*
* 功能描述  : 初始化信号量
*
* 输入参数  :   BSP_U32 u32SignalNum 要初始化的信号量编号
                
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
BSP_S32 BSP_IPC_SemCreate(BSP_U32 u32SignalNum)
{
    IPC_CHECK_PARA_NO_RET(u32SignalNum);
	
	sema_init(&g_semIpcTask[u32SignalNum], SEM_EMPTY);

	return BSP_OK;
}

/*****************************************************************************
* 函 数 名  : BSP_IPC_IntEnable
*
* 功能描述  : 使能某个中断
*
* 输入参数  :   IPC_INT_CORE_E enCoreNum 要使能中断的core
                BSP_U32 ulLvl 要使能的中断号，取值范围0～31  
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
BSP_S32 BSP_IPC_IntEnable (IPC_INT_LEV_E ulLvl)
{
	BSP_U32 u32IntMask;

	/*参数检查*/
	IPC_CHECK_PARA(ulLvl);

	/*写中断屏蔽寄存器*/
	BSP_REG_READ(ipc_base, BSP_IPC_CPU_INT_MASK(g_CoreNum), u32IntMask);
	u32IntMask |= 1 << ulLvl;
	BSP_REG_WRITE(ipc_base,BSP_IPC_CPU_INT_MASK(g_CoreNum), u32IntMask);

	return BSP_OK;
}/*lint !e550*/


/*****************************************************************************
 * 函 数 名  : BSP_INT_Disable
 *
 * 功能描述  : 去使能某个中断
 *
 * 输入参数  : IPC_INT_CORE_E enCoreNum 要使能中断的core 
                BSP_U32 ulLvl 要使能的中断号，取值范围0～31   
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 修改记录  : 2011年4月11日 wangjing creat
 *****************************************************************************/
BSP_S32 BSP_IPC_IntDisable (IPC_INT_LEV_E ulLvl)
{
	BSP_U32 u32IntMask;

	/*参数检查*/
	IPC_CHECK_PARA(ulLvl);

	/*写中断屏蔽寄存器*/
	BSP_REG_READ(ipc_base, BSP_IPC_CPU_INT_MASK(g_CoreNum), u32IntMask);
	u32IntMask = (BSP_U32)(u32IntMask & (~(1 << ulLvl)));/*lint !e502*/
	BSP_REG_WRITE(ipc_base,BSP_IPC_CPU_INT_MASK(g_CoreNum), u32IntMask);

	return BSP_OK;
}

/*****************************************************************************
 * 函 数 名  : BSP_IPC_IntConnect
 *
 * 功能描述  : 注册某个中断
 *
 * 输入参数  : IPC_INT_CORE_E enCoreNum 要使能中断的core 
               BSP_U32 ulLvl 要使能的中断号，取值范围0～31 
               VOIDFUNCPTR routine 中断服务程序
 *             BSP_U32 parameter      中断服务程序参数
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 修改记录  : 2011年4月11日 wangjing creat
 *****************************************************************************/
BSP_S32 BSP_IPC_IntConnect  (IPC_INT_LEV_E ulLvl, VOIDFUNCPTR routine, BSP_U32 parameter)
{
	/*参数检查*/
	IPC_CHECK_PARA(ulLvl);

	stIpcIntTable[ulLvl].routine = routine;
	stIpcIntTable[ulLvl].arg = parameter;

	return BSP_OK;
}

/*****************************************************************************
 * 函 数 名  : BSP_IPC_IntDisonnect
 *
 * 功能描述  : 取消注册某个中断
 *
 * 输入参数  : 
 *              BSP_U32 ulLvl 要使能的中断号，取值范围0～31 
 *              VOIDFUNCPTR routine 中断服务程序
 *             BSP_U32 parameter      中断服务程序参数
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 修改记录  : 2011年4月11日 wangjing creat
 *****************************************************************************/
 BSP_S32 BSP_IPC_IntDisonnect  (IPC_INT_LEV_E ulLvl,VOIDFUNCPTR routine, BSP_U32 parameter)
 {
	unsigned long flag = 0;
	
	/*参数检查*/
	IPC_CHECK_PARA(ulLvl);

    /*解决参数类型不对的问题-- linux内核*/
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
 * 函 数 名  : BSP_DRV_IpcIntHandler
 *
 * 功能描述  : 中断处理函数
 *
 * 输入参数  : 无  
 * 输出参数  : 无
 *
 * 返 回 值  : 无
 *
 * 修改记录  : 2011年4月11日 wangjing creat
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

	/*清中断*/
	BSP_REG_WRITE(ipc_base, BSP_IPC_CPU_INT_CLR(g_CoreNum), u32IntStat);

	 /* 遍历32个中断 */
    
    for (i = 0; i < INTSRC_NUM; i++)
    {   
        if (0 != i)
        {
            u32Date <<= 1;
        }        

        u32BitValue = u32IntStat & u32Date;
       
        /* 如果有中断 ,则调用对应中断处理函数 */
        if (0 != u32BitValue)
        {  
            /*调用注册的中断处理函数*/
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
* 函 数 名  : BSP_IPC_IntSend
*
* 功能描述  : 发送中断
*
* 输入参数  : 
                IPC_INT_CORE_E enDstore 要接收中断的core
                BSP_U32 ulLvl 要发送的中断号，取值范围0～31  
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
BSP_S32 BSP_IPC_IntSend(IPC_INT_CORE_E enDstCore, IPC_INT_LEV_E ulLvl)
{
	/*参数检查*/
	IPC_CHECK_PARA(ulLvl);

	/*写原始中断寄存器,产生中断*/
	BSP_REG_WRITE(ipc_base,BSP_IPC_CPU_RAW_INT(enDstCore), 1 << ulLvl);
	g_stIpc_debug.u32RecvIntCore = enDstCore;
	g_stIpc_debug.u32IntSendTimes[ulLvl]++;

	return BSP_OK;
}

/*****************************************************************************
 * 函 数 名  : BSP_MaskInt
 *
 * 功能描述  : 屏蔽信号量申请的中断释放寄存器
 *
 * 输入参数  : 
                BSP_U32 ulLvl 要发送的中断号，取值范围0～31  
 * 输出参数  : 无
 *
 * 返 回 值  : 无
 *
 * 修改记录  : 2011年4月11日 wangjing creat
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
 * 函 数 名  : BSP_IPC_SemTake
 *
 * 功能描述  : 获取信号量
 *
 * 输入参数  : 无  
 * 输出参数  : 无
 *
 * 返 回 值  : 无
 *
 * 修改记录  : 2011年4月11日 wangjing creat
 *****************************************************************************/
 BSP_S32 BSP_IPC_SemTake(BSP_U32 u32SignalNum,BSP_S32 s32timeout)
 {
	BSP_U32 u32HsCtrl,u32IntMask;    

	/*参数检查*/
	IPC_CHECK_PARA(u32SignalNum);

	/*将申请的信号量对应的释放中断清零*/
	BSP_REG_WRITE(ipc_base, BSP_IPC_SEM_INT_CLR(g_CoreNum), 1 << u32SignalNum);
    for(;;)  //b00198513 Modified for pclint e716          
	{
		BSP_REG_READ(ipc_base, BSP_IPC_HS_CTRL(g_CoreNum, u32SignalNum), u32HsCtrl);
		if (0 == u32HsCtrl)
		{
			/*信号量抢占成功,屏蔽申请核的信号量释放中断*/
			BSP_MaskInt(u32SignalNum); 
			g_stIpc_debug.u32SemId = u32SignalNum;
			g_stIpc_debug.u32SemTakeTimes[u32SignalNum]++;/*lint !e661*/   
			break;
		}
		else
		{
			/*使能信号量释放中断*/
			BSP_REG_READ(ipc_base, BSP_IPC_SEM_INT_MASK(g_CoreNum), u32IntMask);
			u32IntMask = (BSP_U32)(u32IntMask | ((1 << u32SignalNum)));
			BSP_REG_WRITE(ipc_base, BSP_IPC_SEM_INT_MASK(g_CoreNum), u32IntMask);
			//g_SemTakeTimes++;
			if (0 != down_timeout(&g_semIpcTask[u32SignalNum], msecs_to_jiffies(s32timeout * 10)))  /*lint !e732*/
			{
				/*去使能信号量释放中断*/
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
 * 函 数 名  : BSP_IPC_SemGive
 *
 * 功能描述  : 释放信号量
 *
 * 输入参数  : 无  
 * 输出参数  : 无
 *
 * 返 回 值  : 无
 *
 * 修改记录  : 2011年4月11日 wangjing creat
 *****************************************************************************/
 BSP_VOID BSP_IPC_SemGive(BSP_U32 u32SignalNum)
 {
	IPC_CHECK_PARA_NO_RET(u32SignalNum);
	
	/*将信号量请求寄存器清0*/
	BSP_REG_WRITE(ipc_base, BSP_IPC_HS_CTRL(g_CoreNum, u32SignalNum), 0);
	g_stIpc_debug.u32SemGiveTimes[u32SignalNum]++;
 }

 /*****************************************************************************
 * 函 数 名  : BSP_IPC_SemIntHandler
 *
 * 功能描述  : 信号量释放中断处理函数
 *
 * 输入参数  : 无  
 * 输出参数  : 无
 *
 * 返 回 值  : 无
 *
 * 修改记录  : 2011年4月11日 wangjing creat
 *****************************************************************************/
static irqreturn_t BSP_IPC_SemIntHandler(int irq, void *dev_id)
{
 	BSP_S32 retval = IRQ_HANDLED;
	BSP_U32 u32SNum, u32IntStat;

	BSP_REG_READ(ipc_base, BSP_IPC_SEM_INT_STAT(g_CoreNum), u32IntStat);

	/*如果有信号量释放中断，清除该中断*/
	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
	u32SNum = (BSP_U32)ffsLsb ((BSP_S32)u32IntStat);
	/*end*/
	BSP_REG_WRITE(ipc_base, BSP_IPC_SEM_INT_CLR(g_CoreNum), 1 << --u32SNum);

	up(&g_semIpcTask[u32SNum]);
	g_stIpc_debug.u32SemHandleTimes[u32SNum]++;

	return IRQ_RETVAL(retval);/*lint !e64*/
}

/*****************************************************************************
* 函 数 名  : BSP_IPC_SpinLock
*
* 功能描述  : 查询等待获取信号量
*
* 输入参数  : 无  
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2011年4月11日 wangjing creat
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
* 函 数 名  : BSP_IPC_SpinUnLock
*
* 功能描述  : 释放信号量
*
* 输入参数  : 无  
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
BSP_VOID BSP_IPC_SpinUnLock (BSP_U32 u32SignalNum)
{
	IPC_CHECK_PARA_NO_RET(u32SignalNum);
	
	/*将信号量请求寄存器清0*/
	BSP_REG_WRITE(ipc_base, BSP_IPC_HS_CTRL(g_CoreNum, u32SignalNum), 0);
}



/*****************************************************************************
* 函 数 名  : BSP_IPC_DebugShow
*
* 功能描述  : 可维可测接口
*
* 输入参数  : 无  
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
BSP_VOID BSP_IPC_DebugShow(void)
{
	BSP_U32 i;

	printk("\n当前占用的信号量ID为       : \t%d\n", g_stIpc_debug.u32SemId);
	for (i = 0; i < INTSRC_NUM; i++)
	{
		printk("信号量%d获取次数             : \t%d\n", i, g_stIpc_debug.u32SemTakeTimes[i]);
		printk("信号量%d释放次数             : \t%d\n", i, g_stIpc_debug.u32SemGiveTimes[i]);
		printk("进入信号量中断处理的次数为   : \t%d\n", g_stIpc_debug.u32SemHandleTimes[i]);
		printk("发送中断的次数为             : \t%d\n", g_stIpc_debug.u32IntSendTimes[i]);
		printk("进入中断处理的次数为         : \t%d\n", g_stIpc_debug.u32IntHandleTimes[i]);
	}

	printk("接收中断的Core ID为          : \t%d\n", g_stIpc_debug.u32RecvIntCore);
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

