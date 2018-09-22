/*************************************************************************
*   版权所有(C) 1987-2004, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_IFC.c
*
*   作    者 :  liumengcun
*
*   描    述 :  IFC核间函数调用
*
*   修改记录 :  2011年1月27日  v1.00  liumengcun  创建
*
*************************************************************************/

#include <linux/string.h>
#include <linux/semaphore.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/kthread.h>
#include <linux/sched.h>

#include "BSP.h"
#include "bsp_drv_ifc.h"

#include  <mach/common/bsp_memory.h>
#include  <mach/common/mem/bsp_mem.h>

/*--------------------------------------------------------------*
 * 宏定义                                                       *
 *--------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * 外部函数原型说明                                             *
 *--------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * 内部函数原型说明                                             *
 *--------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * 外部变量引用                                                 *
 *--------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * 全局变量申明                                                 *
 *--------------------------------------------------------------*/

/* 全局信息结构*/
IFC_CTX_S g_stIFCCtx;
/* IFC是否初始化标志*/
BSP_U32 g_u32IsIFCInit = FALSE;
/* debug信息全局结构*/
/*lint -e729 */
IFC_DEBUG_INFO_S g_stIFCDebugInfo;  
/*lint +e729 */

//static struct task_struct *ifc_task;
static BSP_U32 ifc_task_id;

static int IFC_HandleMsg(void *__unused);

/*****************************************************************************
* 函 数 名  : IFC_Init
*
* 功能描述  : IFC模块初始化函数
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_S32 IFC_Init(void)
{
	///BSP_S32 s32Result = 0;
	UDI_OPEN_PARAM stUdiOpenParam;
	ICC_CHAN_ATTR_S stIccChanAttr;
	BSP_U32 u32OtherCoreInitFlag = 0; /* 主从核初始化标志*/
	BSP_U32 u32DelayTimes = IFC_INIT_DELAY_TIMES;

	if (TRUE == g_u32IsIFCInit)
	{
		return OK;
	}

	/* 初始化标志置为未完成*/
	////*(BSP_U32 *)IFC_INIT_FLAG_ADDR = IFC_INIT_FLAG;
	*(BSP_U32 *)IFC_INIT_FLAG_ADDR = 0;

	memset(&g_stIFCCtx, 0x0, sizeof(IFC_CTX_S));
	memset(&g_stIFCDebugInfo, 0x0, sizeof(IFC_DEBUG_INFO_S));
	memset(&stUdiOpenParam, 0x0, sizeof(UDI_OPEN_PARAM));
	memset(&stIccChanAttr, 0x0, sizeof(ICC_CHAN_ATTR_S));

	/* taskId附初值*/
	g_stIFCCtx.s32RcvTaskId = ERROR;    
	g_stIFCCtx.s32SendTaskId = ERROR;

	/* 申请IFC 发送使用的ICC通道*/
	stIccChanAttr.enChanMode = ICC_CHAN_MODE_PACKET;;
	stIccChanAttr.u32Priority = IFC_ICC_CHAN_PRIORITY;    
	stIccChanAttr.u32FIFOInSize = IFC_ICC_CHAN_IN_FIFOSIZE;
	stIccChanAttr.u32FIFOOutSize = IFC_ICC_CHAN_OUT_FIFOSIZE;
	stIccChanAttr.u32TimeOut = IFC_ICC_CHAN_TIMEOUT;
	stIccChanAttr.read_cb = (icc_read_cb)IFC_Receive;
	stIccChanAttr.write_cb = (icc_write_cb)IFC_SendHandle;

	stUdiOpenParam.devid = UDI_BUILD_DEV_ID(UDI_DEV_ICC, IFC_ICC_CHAN_NUM);/*lint !e64*/
	stUdiOpenParam.pPrivate = &stIccChanAttr;
	g_stIFCCtx.IFCSendHandle = udi_open(&stUdiOpenParam);
	if (UDI_INVALID_HANDLE == g_stIFCCtx.IFCSendHandle)
	{
		printk(KERN_ERR "BSP_MODU_IFC udi_open send handle fail\n");
		IFC_UnInit();
		return ERROR;
	}

	/* 创建收包函数互斥信号量*/    
	sema_init(&g_stIFCCtx.semIFCRecv, SEM_FULL);
	/* 创建收包任务同步信号量*/    
	sema_init(&g_stIFCCtx.semIFCRecvTask, SEM_EMPTY);
    /* 创建发包任务同步信号量*/     
	sema_init(&g_stIFCCtx.semIFCSend, SEM_EMPTY);

	/* 创建IFC接收任务链表*/
	INIT_LIST_HEAD(&g_stIFCCtx.rcvFuncList);

/******************************************************************************/
    if(BSP_OK != OSA_CreateTask("ifc_thread", 
                                &ifc_task_id, 
                                (osa_task_entry)IFC_HandleMsg, 
                                IFC_RCV_TASK_PRIORITY, 
                                0x1000, 
                                BSP_NULL))
    {
		printk("create kthread icc_thread failed!\n");
		return BSP_ERROR;
    }
    #if 0
	ifc_task = kthread_run(IFC_HandleMsg, NULL, "ifc_thread");
	if (IS_ERR(ifc_task))
	{
		printk("create kthread ifc_thread failed!\n ");
		return -1;
	}
    #endif

/******************************************************************************/

	/* 初始化完成*/
	*(BSP_U32 *)IFC_INIT_FLAG_ADDR = IFC_INIT_FLAG;

	/* 检查等待另外一个核IFC初始化完成*/
	u32OtherCoreInitFlag = *(BSP_U32 *)IFC_OTHER_CORE_INIT_FLAG_ADDR;
	while ((u32DelayTimes) && (IFC_INIT_FLAG != u32OtherCoreInitFlag))
	{
		msleep(10);
		u32DelayTimes--;
		u32OtherCoreInitFlag = *(BSP_U32 *)IFC_OTHER_CORE_INIT_FLAG_ADDR;
	}

	if (IFC_INIT_FLAG != u32OtherCoreInitFlag)
	{
		printk(KERN_ERR "BSP_MODU_IFC wait for another core ifc init timeout,u32DelayTimes = %d, \
			modem core flag = 0x%x,app core flag = 0x%x\n",
			u32DelayTimes, *(BSP_U32*)IFC_MODEM_INIT_FLAG_ADDR, *(BSP_U32*)IFC_APP_INIT_FLAG_ADDR);
		IFC_UnInit();
		return ERROR;
	}

	g_u32IsIFCInit = TRUE;

	return OK;
}
/*****************************************************************************
* 函 数 名  : IFC_UnInit
*
* 功能描述  : IFC去初始化
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_VOID IFC_UnInit(void)
{   
	/* 关闭Call ICC通道*/
	if (UDI_INVALID_HANDLE != g_stIFCCtx.IFCSendHandle)
	{
		udi_close(g_stIFCCtx.IFCSendHandle);
	}

	/* 把所有指针清0*/
	memset(&g_stIFCCtx, 0x0, sizeof(IFC_CTX_S));
	g_stIFCCtx.s32RcvTaskId = ERROR;
	g_stIFCCtx.s32SendTaskId = ERROR;

	g_u32IsIFCInit = FALSE;
	return;
}

/*****************************************************************************
* 函 数 名  : BSP_IFC_RegFunc
*
* 功能描述  : 注册IFC回调函数接口
*
* 输入参数  : IFC_MODULE_E enModuleId    模块ID
*             BSP_IFC_REG_FUNC pFunc     回调函数指针
* 输出参数  : 无
*
* 返 回 值  : BSP_SUCCESS
*             BSP_ERR_MODULE_NOT_INITED
*             BSP_ERR_INVALID_PARA
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_U32 BSP_IFC_RegFunc(IFC_MODULE_E enModuleId, BSP_IFC_REG_FUNC pFunc)
{
	if (FALSE == g_u32IsIFCInit)
	{        
		printk(KERN_ERR "BSP_MODU_IFC BSP_IFC_RegFunc not init!\n");
		return BSP_ERR_MODULE_NOT_INITED;
	}    

	if (enModuleId >= IFC_MODULE_BUTT)
	{
		printk(KERN_ERR "BSP_MODU_IFC BSP_IFC_RegFunc invalid para enModuleId = %d,  pFunc = 0x%x!\n",
			enModuleId, (BSP_U32)pFunc);
		return BSP_ERR_INVALID_PARA;
	}

	g_stIFCCtx.paFuncCb[enModuleId] = pFunc;

	return OK;
}
/*****************************************************************************
* 函 数 名  : BSP_IFC_Send
*
* 功能描述  : IFC发送消息接口
*
* 输入参数  : BSP_VOID * pMspBody           发送消息内容
*             BSP_U32 u32Len               pMspBody的长度 
* 输出参数  : 无
*
* 返 回 值  : BSP_SUCCESS
*             BSP_ERR_MODULE_NOT_INITED
*             BSP_ERR_INVALID_PARA
*             BSP_ERR_IFC_SEND_FAIL
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_U32 BSP_IFC_Send(BSP_VOID * pMspBody, BSP_U32 u32Len)
{  
	BSP_S32 s32Result = 0;
	BSP_U32 u32ModuleId;
	BSP_U8* pMspBodyTmp = NULL;

	g_stIFCDebugInfo.u32IFCRcvInCnt++;
	/* 初始化判断*/
	if (FALSE == g_u32IsIFCInit)
	{        
		printk(KERN_ERR "BSP_MODU_IFC BSP_IFC_Send not init!\n");
		return BSP_ERR_MODULE_NOT_INITED;
	}    

	/* 参数合法性判断*/
	if((NULL == pMspBody) || (0 == u32Len) || (BSP_IFC_SEND_MAX_LEN < u32Len) )
	{
		//printk(KERN_ERR "BSP_MODU_IFC BSP_IFC_Send invalid para,pMspBody = 0x%x, u32Len = 0x%x!\n", pMspBody, u32Len);
		g_stIFCDebugInfo.u32IFCSendParamNullFailCnt++;
		return BSP_ERR_INVALID_PARA;
	}

	pMspBodyTmp = pMspBody;

	u32ModuleId = *(BSP_U32* )pMspBodyTmp;
	if (u32ModuleId >= IFC_MODULE_BUTT)
	{
		printk(KERN_ERR "BSP_MODU_IFC BSP_IFC_Send invalid para u32ModuleId = 0x%x!\n", u32ModuleId);
		g_stIFCDebugInfo.u32IFCSendInvalidModuleIdCnt++;
		return BSP_ERR_INVALID_PARA;
	}

	/* 调用UDI接口向ICC通道发送消息*/
	/*************************************************************************/
#if 0
	s32Result = udi_write(g_stIFCCtx.IFCSendHandle, pMspBodyTmp, u32Len);    
	if (BSP_ERR_ICC_BUFFER_FULL == s32Result)
	{        
		g_stIFCDebugInfo.u32IFCSendIccFIFOFullCnt++;
		if (ERROR == semTake(g_stIFCCtx.semIFCSend,IFC_SEND_TIMEOUT))
		{
			printk(KERN_ERR "BSP_MODU_IFC udi_write FIFO FULL TIMEOUT!\n");
				g_stIFCDebugInfo.u32IFCSendIccFIFOFullTimeOutCnt++;
			return BSP_ERR_IFC_SEND_TIMEOUT;  
		}
		s32Result = udi_write(g_stIFCCtx.IFCSendHandle, pMspBodyTmp, u32Len);
	}
#endif

	s32Result = udi_write(g_stIFCCtx.IFCSendHandle, pMspBodyTmp, u32Len);    
	if (BSP_ERR_ICC_BUFFER_FULL == s32Result)
	{        
		g_stIFCDebugInfo.u32IFCSendIccFIFOFullCnt++;
		if (0 != down_timeout(&g_stIFCCtx.semIFCSend, msecs_to_jiffies(2000)))
		{
			printk(KERN_ERR "BSP_MODU_IFC udi_write FIFO FULL TIMEOUT!\n");
				g_stIFCDebugInfo.u32IFCSendIccFIFOFullTimeOutCnt++;
			return BSP_ERR_IFC_SEND_TIMEOUT;  
		}
		s32Result = udi_write(g_stIFCCtx.IFCSendHandle, pMspBodyTmp, u32Len);
	}
	/**************************************************************************/


	if (s32Result != (BSP_S32)u32Len)
	{                
		printk(KERN_ERR "BSP_MODU_IFC udi_write fail s32Result = 0x%x!\n", s32Result);
		g_stIFCDebugInfo.u32IFCSendUdiFailCnt++;
		return BSP_ERR_IFC_SEND_FAIL;            
	}

	g_stIFCDebugInfo.u32IFCSendSuccessCnt++;

	return OK;
}

/*****************************************************************************
* 函 数 名  : IFC_Receive
*
* 功能描述  : IFC接收处理函数，注册到ICC回调处理
*
* 输入参数  : BSP_VOID * pBuffer  
*             BSP_U32 u32Len      
* 输出参数  : 无
*
* 返 回 值  : OK
*             BSP_ERR_MODULE_NOT_INITED
*             BSP_ERR_INVALID_PARA
*             BSP_ERR_BUF_ALLOC_FAILED
*
* 其它说明  : 无
*
*****************************************************************************/
/*lint -e429*/
BSP_U32 IFC_Receive(BSP_U32 u32ChanID, BSP_U32 u32Len)
{
	BSP_U8 * pMsgBuffer = NULL;
	BSP_U32 u32MsgLen = 0;
	BSP_U32 u32ModuleId;
	IFC_RCV_FUNC_NODE_S * pNode = NULL;
	BSP_S32 s32Result = 0;

	g_stIFCDebugInfo.u32IFCRcvInCnt++;
	/* 初始化判断*/
	if (FALSE == g_u32IsIFCInit)
	{        
		printk(KERN_ERR "BSP_MODU_IFC IFC not init!\n");
		return BSP_ERR_MODULE_NOT_INITED;
	}    

	/* 参数合法性判断*/
	if (0 == u32Len)
	{
		printk(KERN_ERR "BSP_MODU_IFC IFC_Receive invalid para ,u32Len = 0x%x!\n", u32Len);
			g_stIFCDebugInfo.u32IFCRcvParamNullFailCnt++;
		return BSP_ERR_INVALID_PARA;
	}

	/* 申请msgbody内存*/
#if 0
	pMsgBuffer = BSP_MALLOC(u32Len, 0);
#else
	pMsgBuffer = kmalloc(u32Len, GFP_KERNEL);
#endif
	if(NULL == pMsgBuffer)
	{
		printk(KERN_ERR "BSP_MODU_IFC IFC_Receive BSP_MALLOC pMsgBuffer fail!\n");
			g_stIFCDebugInfo.u32IFCRcvMsgMallocFailCnt++;
		kfree(pMsgBuffer);
		return BSP_ERR_BUF_ALLOC_FAILED;
	}

	s32Result = udi_read(g_stIFCCtx.IFCSendHandle, pMsgBuffer, u32Len);
	if (u32Len != (BSP_U32)s32Result)
	{
		printk(KERN_ERR "BSP_MODU_IFC IFC_Receive udi_read fail s32Result = %d!\n",s32Result);
			g_stIFCDebugInfo.u32IFCRcvUdiFailCnt++;
		return BSP_ERR_IFC_RCV_FAIL;
	}

	u32ModuleId = *(BSP_U32 *)pMsgBuffer;

	/* 参数合法性判断*/
	if (u32ModuleId >= IFC_MODULE_BUTT)
	{
		printk(KERN_ERR "BSP_MODU_IFC IFC_Receive invalid para u32ModuleId = 0x%x!\n",u32ModuleId);
			g_stIFCDebugInfo.u32IFCRcvInvalidModuleIdCnt++;
		return BSP_ERR_INVALID_PARA;
	}
	/* 消息长度去除moduleId长度*/
	u32MsgLen = u32Len - sizeof(BSP_U32);

	/* 申请函数链表节点内存*/
#if 0
	pNode = BSP_MALLOC(sizeof(IFC_RCV_FUNC_NODE_S), 0);
#else
	pNode = kmalloc(sizeof(IFC_RCV_FUNC_NODE_S), GFP_KERNEL);
#endif
	if (NULL == pNode)
	{
		printk(KERN_ERR "BSP_MODU_IFC IFC_Receive BSP_MALLOC pNode fail!\n");
		kfree(pMsgBuffer);
		pMsgBuffer = NULL;
		g_stIFCDebugInfo.u32IFCRcvNodeMallocFailCnt++;
		return BSP_ERR_BUF_ALLOC_FAILED;
	}

	/* 根据模块ID 记录函数节点中的函数指针，在接收任务中处理*/
	pNode->pFunc = g_stIFCCtx.paFuncCb[u32ModuleId];
	pNode->u32Arg0 = (BSP_U32)pMsgBuffer;
	pNode->u32Arg1 = u32MsgLen;

	down(&g_stIFCCtx.semIFCRecv);
	/* 存入任务处理链表中*/
	list_add_tail(&pNode->node, &g_stIFCCtx.rcvFuncList);  

	g_stIFCDebugInfo.u32IFCRcvFuncNodeCnt++;
	up(&g_stIFCCtx.semIFCRecv);

	/* 释放收包任务信号量，使收包任务得到调度*/
	up(&g_stIFCCtx.semIFCRecvTask);

	g_stIFCDebugInfo.u32IFCRcvSuccessCnt++;

	return OK;
}
/*lint +e429*/
/*****************************************************************************
* 函 数 名  : IFC_HandleMsg
*
* 功能描述  : IFC实际处理接收消息函数，依赖与IFC_Receive函数释放同步信号量
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 其它说明  : 无
*
*****************************************************************************/
static int IFC_HandleMsg(void *__unused)
{
	IFC_RCV_FUNC_NODE_S * pNode = NULL;

	for (;;)
	{
		/* 等待收包任务信号量，如果有需要处理的接收函数就处理*/
		down(&g_stIFCCtx.semIFCRecvTask);
		g_stIFCDebugInfo.u32IFCRcvTaskRunCnt++;

		/* 从函数链表中取一个函数节点处理*/
		down(&g_stIFCCtx.semIFCRecv);

		pNode = list_first_entry(&g_stIFCCtx.rcvFuncList, IFC_RCV_FUNC_NODE_S, node);

		g_stIFCDebugInfo.u32IFCRcvFuncNodeCnt--;
		up(&g_stIFCCtx.semIFCRecv);

		/* 当函数链表不为空时，循环处理，直到处理完毕*/
		while ( (pNode != NULL) && ((unsigned int)pNode->pFunc != 0xFFFFFFFF) ) /*judge pNode if NULL,clean lint warning e413*/
		{
            /*解决pNode 空指针问题*/
			/*lint -e613*/
			if (pNode->pFunc)
			{
				/* MsgBody 需要去除moduleid 4个字节*/
				(BSP_VOID)pNode->pFunc((BSP_U8*)(pNode->u32Arg0 + sizeof(BSP_U32)),pNode->u32Arg1);
			}

			list_del(&pNode->node);

			/* 释放msgbody和node内存*/
			kfree((BSP_U8*)pNode->u32Arg0);
			kfree(pNode);
            /*lint +e613*/
			
			/* 从函数链表中取下一个函数节点处理*/
			down(&g_stIFCCtx.semIFCRecv);

			pNode = list_first_entry(&g_stIFCCtx.rcvFuncList, IFC_RCV_FUNC_NODE_S, node);

			g_stIFCDebugInfo.u32IFCRcvFuncNodeCnt--;
			up(&g_stIFCCtx.semIFCRecv);
		}

	}

	return 0;
}

BSP_U32 IFC_SendHandle(BSP_U32 u32ChanID)
{
	up(&g_stIFCCtx.semIFCSend);
	return OK;
}
/* BEGIN: Modified by liumengcun, 2011-1-27 debug函数*/
BSP_VOID IFC_Help(void)
{
#if 0
	logMsg("\r |*************************************|\n",0,0,0,0,0,0);    
	logMsg("\r IFC_debugCntShow : 查看各种统计信息\n",0,0,0,0,0,0);    
	logMsg("\r IFC_debugCtxShow : 查看全局信息\n",0,0,0,0,0,0);
	logMsg("\r IFC_debugCbShow  : 查看回调函数信息\n",0,0,0,0,0,0);
	logMsg("\r |*************************************|\n",0,0,0,0,0,0);
#endif
}

BSP_VOID IFC_debugCtxShow(void)
{
#if 0
	logMsg("\r The IFC Context info:\n",0,0,0,0,0,0);        
	logMsg("\r IFC是否初始化             : 0x%x\n",(BSP_S32)g_u32IsIFCInit,0,0,0,0,0);    
	logMsg("\r 发送UDI通道handle         : 0x%x\n",(BSP_S32)g_stIFCCtx.IFCSendHandle,0,0,0,0,0);    
	logMsg("\r 接收UDI通道handle         : 0x%x\n",(BSP_S32)g_stIFCCtx.IFCReceiveHandle,0,0,0,0,0);    
	logMsg("\r 收包函数互斥信号量        : 0x%x\n",(BSP_S32)g_stIFCCtx.semIFCRecv,0,0,0,0,0);  
	logMsg("\r 收包任务同步信号量        : 0x%x\n",(BSP_S32)g_stIFCCtx.semIFCRecvTask,0,0,0,0,0);
	logMsg("\r 收包任务函数节点链表      : 0x%x\n",(BSP_S32)g_stIFCCtx.rcvFuncList,0,0,0,0,0); 
	logMsg("\r 收包任务id                : 0x%x\n",(BSP_S32)g_stIFCCtx.s32RcvTaskId,0,0,0,0,0);
#endif
}

BSP_VOID IFC_debugCbShow(void)
{
#if 0
	logMsg("\r The IFC callback func info:\n",0,0,0,0,0,0);
	logMsg("\r BSP回调函数               :0x%x\n",(BSP_S32)g_stIFCCtx.paFuncCb[0] ,0,0,0,0,0);
	logMsg("\r LTE PS回调函数            :0x%x\n",(BSP_S32)g_stIFCCtx.paFuncCb[1],0,0,0,0,0);
	logMsg("\r GU PS回调函数             :0x%x\n",(BSP_S32)g_stIFCCtx.paFuncCb[2],0,0,0,0,0);
	logMsg("\r LTE MSP回调函数           :0x%x\n",(BSP_S32)g_stIFCCtx.paFuncCb[3],0,0,0,0,0);
	logMsg("\r GU OM回调函数             :0x%x\n",(BSP_S32)g_stIFCCtx.paFuncCb[4],0,0,0,0,0);
#endif
}

BSP_VOID IFC_debugCntShow(void)
{
#if 0
	logMsg("\r The IFC Debug Count Info:\n",0,0,0,0,0,0);        
	logMsg("\r IFC发送进入次数                  : 0x%x\n",(BSP_S32)g_stIFCDebugInfo.u32IFCSendInCnt,0,0,0,0,0);    
	logMsg("\r IFC发送成功次数                  : 0x%x\n",(BSP_S32)g_stIFCDebugInfo.u32IFCSendSuccessCnt,0,0,0,0,0);    
	logMsg("\r IFC发送参数空失败次数            : 0x%x\n",(BSP_S32)g_stIFCDebugInfo.u32IFCSendParamNullFailCnt,0,0,0,0,0);    
	logMsg("\r IFC发送模块ID无效失败次数        : 0x%x\n",(BSP_S32)g_stIFCDebugInfo.u32IFCSendInvalidModuleIdCnt,0,0,0,0,0);    
	logMsg("\r IFC发送调用UDI接口发送失败次数   : 0x%x\n",(BSP_S32)g_stIFCDebugInfo.u32IFCSendUdiFailCnt,0,0,0,0,0);  
	logMsg("\r IFC发送ICC fifo FULL次数         : 0x%x\n",(BSP_S32)g_stIFCDebugInfo.u32IFCSendIccFIFOFullCnt,0,0,0,0,0);  
	logMsg("\r IFC发送ICC fifo FULL等待超时次数 : 0x%x\n",(BSP_S32)g_stIFCDebugInfo.u32IFCSendIccFIFOFullTimeOutCnt,0,0,0,0,0);  
	logMsg("\r IFC接收进入次数                  : 0x%x\n",(BSP_S32)g_stIFCDebugInfo.u32IFCRcvInCnt,0,0,0,0,0);
	logMsg("\r IFC接收成功次数                  : 0x%x\n",(BSP_S32)g_stIFCDebugInfo.u32IFCRcvSuccessCnt,0,0,0,0,0); 
	logMsg("\r IFC接收参数空失败次数            : 0x%x\n",(BSP_S32)g_stIFCDebugInfo.u32IFCRcvParamNullFailCnt,0,0,0,0,0);
	logMsg("\r IFC接收模块ID无效失败次数        : 0x%x\n",(BSP_S32)g_stIFCDebugInfo.u32IFCRcvInvalidModuleIdCnt,0,0,0,0,0); 
	logMsg("\r IFC接收申请MSG BODY内存失败次数  : 0x%x\n",(BSP_S32)g_stIFCDebugInfo.u32IFCRcvMsgMallocFailCnt,0,0,0,0,0);
	logMsg("\r IFC接收申请NODE内存失败次数      : 0x%x\n",(BSP_S32)g_stIFCDebugInfo.u32IFCRcvNodeMallocFailCnt,0,0,0,0,0); 
	logMsg("\r IFC接收调用UDI接口发送失败次数   : 0x%x\n",(BSP_S32)g_stIFCDebugInfo.u32IFCRcvUdiFailCnt,0,0,0,0,0); 
	logMsg("\r IFC接收处理函数链表节点个数      : 0x%x\n",(BSP_S32)g_stIFCDebugInfo.u32IFCRcvFuncNodeCnt,0,0,0,0,0);
	logMsg("\r IFC接收处理任务运行次数          : 0x%x\n",(BSP_S32)g_stIFCDebugInfo.u32IFCRcvTaskRunCnt,0,0,0,0,0);
#endif
}
BSP_S32 BSP_IFC_CanSleep(void)
{
	if(list_empty(&g_stIFCCtx.rcvFuncList))
	{
		return OK;
	}
	else
	{
		return ERROR;
	}
	
}
/* END:   Modified by liumengcun, 2011-1-27 */

