/*************************************************************************
*   ��Ȩ����(C) 1987-2004, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_IFC.c
*
*   ��    �� :  liumengcun
*
*   ��    �� :  IFC�˼亯������
*
*   �޸ļ�¼ :  2011��1��27��  v1.00  liumengcun  ����
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
 * �궨��                                                       *
 *--------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * �ⲿ����ԭ��˵��                                             *
 *--------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * �ڲ�����ԭ��˵��                                             *
 *--------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * �ⲿ��������                                                 *
 *--------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * ȫ�ֱ�������                                                 *
 *--------------------------------------------------------------*/

/* ȫ����Ϣ�ṹ*/
IFC_CTX_S g_stIFCCtx;
/* IFC�Ƿ��ʼ����־*/
BSP_U32 g_u32IsIFCInit = FALSE;
/* debug��Ϣȫ�ֽṹ*/
/*lint -e729 */
IFC_DEBUG_INFO_S g_stIFCDebugInfo;  
/*lint +e729 */

//static struct task_struct *ifc_task;
static BSP_U32 ifc_task_id;

static int IFC_HandleMsg(void *__unused);

/*****************************************************************************
* �� �� ��  : IFC_Init
*
* ��������  : IFCģ���ʼ������
*
* �������  : ��
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* ����˵��  : ��
*
*****************************************************************************/
BSP_S32 IFC_Init(void)
{
	///BSP_S32 s32Result = 0;
	UDI_OPEN_PARAM stUdiOpenParam;
	ICC_CHAN_ATTR_S stIccChanAttr;
	BSP_U32 u32OtherCoreInitFlag = 0; /* ���Ӻ˳�ʼ����־*/
	BSP_U32 u32DelayTimes = IFC_INIT_DELAY_TIMES;

	if (TRUE == g_u32IsIFCInit)
	{
		return OK;
	}

	/* ��ʼ����־��Ϊδ���*/
	////*(BSP_U32 *)IFC_INIT_FLAG_ADDR = IFC_INIT_FLAG;
	*(BSP_U32 *)IFC_INIT_FLAG_ADDR = 0;

	memset(&g_stIFCCtx, 0x0, sizeof(IFC_CTX_S));
	memset(&g_stIFCDebugInfo, 0x0, sizeof(IFC_DEBUG_INFO_S));
	memset(&stUdiOpenParam, 0x0, sizeof(UDI_OPEN_PARAM));
	memset(&stIccChanAttr, 0x0, sizeof(ICC_CHAN_ATTR_S));

	/* taskId����ֵ*/
	g_stIFCCtx.s32RcvTaskId = ERROR;    
	g_stIFCCtx.s32SendTaskId = ERROR;

	/* ����IFC ����ʹ�õ�ICCͨ��*/
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

	/* �����հ����������ź���*/    
	sema_init(&g_stIFCCtx.semIFCRecv, SEM_FULL);
	/* �����հ�����ͬ���ź���*/    
	sema_init(&g_stIFCCtx.semIFCRecvTask, SEM_EMPTY);
    /* ������������ͬ���ź���*/     
	sema_init(&g_stIFCCtx.semIFCSend, SEM_EMPTY);

	/* ����IFC������������*/
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

	/* ��ʼ�����*/
	*(BSP_U32 *)IFC_INIT_FLAG_ADDR = IFC_INIT_FLAG;

	/* ���ȴ�����һ����IFC��ʼ�����*/
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
* �� �� ��  : IFC_UnInit
*
* ��������  : IFCȥ��ʼ��
*
* �������  : ��
* �������  : ��
*
* �� �� ֵ  : ��
*
* ����˵��  : ��
*
*****************************************************************************/
BSP_VOID IFC_UnInit(void)
{   
	/* �ر�Call ICCͨ��*/
	if (UDI_INVALID_HANDLE != g_stIFCCtx.IFCSendHandle)
	{
		udi_close(g_stIFCCtx.IFCSendHandle);
	}

	/* ������ָ����0*/
	memset(&g_stIFCCtx, 0x0, sizeof(IFC_CTX_S));
	g_stIFCCtx.s32RcvTaskId = ERROR;
	g_stIFCCtx.s32SendTaskId = ERROR;

	g_u32IsIFCInit = FALSE;
	return;
}

/*****************************************************************************
* �� �� ��  : BSP_IFC_RegFunc
*
* ��������  : ע��IFC�ص������ӿ�
*
* �������  : IFC_MODULE_E enModuleId    ģ��ID
*             BSP_IFC_REG_FUNC pFunc     �ص�����ָ��
* �������  : ��
*
* �� �� ֵ  : BSP_SUCCESS
*             BSP_ERR_MODULE_NOT_INITED
*             BSP_ERR_INVALID_PARA
*
* ����˵��  : ��
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
* �� �� ��  : BSP_IFC_Send
*
* ��������  : IFC������Ϣ�ӿ�
*
* �������  : BSP_VOID * pMspBody           ������Ϣ����
*             BSP_U32 u32Len               pMspBody�ĳ��� 
* �������  : ��
*
* �� �� ֵ  : BSP_SUCCESS
*             BSP_ERR_MODULE_NOT_INITED
*             BSP_ERR_INVALID_PARA
*             BSP_ERR_IFC_SEND_FAIL
*
* ����˵��  : ��
*
*****************************************************************************/
BSP_U32 BSP_IFC_Send(BSP_VOID * pMspBody, BSP_U32 u32Len)
{  
	BSP_S32 s32Result = 0;
	BSP_U32 u32ModuleId;
	BSP_U8* pMspBodyTmp = NULL;

	g_stIFCDebugInfo.u32IFCRcvInCnt++;
	/* ��ʼ���ж�*/
	if (FALSE == g_u32IsIFCInit)
	{        
		printk(KERN_ERR "BSP_MODU_IFC BSP_IFC_Send not init!\n");
		return BSP_ERR_MODULE_NOT_INITED;
	}    

	/* �����Ϸ����ж�*/
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

	/* ����UDI�ӿ���ICCͨ��������Ϣ*/
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
* �� �� ��  : IFC_Receive
*
* ��������  : IFC���մ�������ע�ᵽICC�ص�����
*
* �������  : BSP_VOID * pBuffer  
*             BSP_U32 u32Len      
* �������  : ��
*
* �� �� ֵ  : OK
*             BSP_ERR_MODULE_NOT_INITED
*             BSP_ERR_INVALID_PARA
*             BSP_ERR_BUF_ALLOC_FAILED
*
* ����˵��  : ��
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
	/* ��ʼ���ж�*/
	if (FALSE == g_u32IsIFCInit)
	{        
		printk(KERN_ERR "BSP_MODU_IFC IFC not init!\n");
		return BSP_ERR_MODULE_NOT_INITED;
	}    

	/* �����Ϸ����ж�*/
	if (0 == u32Len)
	{
		printk(KERN_ERR "BSP_MODU_IFC IFC_Receive invalid para ,u32Len = 0x%x!\n", u32Len);
			g_stIFCDebugInfo.u32IFCRcvParamNullFailCnt++;
		return BSP_ERR_INVALID_PARA;
	}

	/* ����msgbody�ڴ�*/
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

	/* �����Ϸ����ж�*/
	if (u32ModuleId >= IFC_MODULE_BUTT)
	{
		printk(KERN_ERR "BSP_MODU_IFC IFC_Receive invalid para u32ModuleId = 0x%x!\n",u32ModuleId);
			g_stIFCDebugInfo.u32IFCRcvInvalidModuleIdCnt++;
		return BSP_ERR_INVALID_PARA;
	}
	/* ��Ϣ����ȥ��moduleId����*/
	u32MsgLen = u32Len - sizeof(BSP_U32);

	/* ���뺯������ڵ��ڴ�*/
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

	/* ����ģ��ID ��¼�����ڵ��еĺ���ָ�룬�ڽ��������д���*/
	pNode->pFunc = g_stIFCCtx.paFuncCb[u32ModuleId];
	pNode->u32Arg0 = (BSP_U32)pMsgBuffer;
	pNode->u32Arg1 = u32MsgLen;

	down(&g_stIFCCtx.semIFCRecv);
	/* ����������������*/
	list_add_tail(&pNode->node, &g_stIFCCtx.rcvFuncList);  

	g_stIFCDebugInfo.u32IFCRcvFuncNodeCnt++;
	up(&g_stIFCCtx.semIFCRecv);

	/* �ͷ��հ������ź�����ʹ�հ�����õ�����*/
	up(&g_stIFCCtx.semIFCRecvTask);

	g_stIFCDebugInfo.u32IFCRcvSuccessCnt++;

	return OK;
}
/*lint +e429*/
/*****************************************************************************
* �� �� ��  : IFC_HandleMsg
*
* ��������  : IFCʵ�ʴ��������Ϣ������������IFC_Receive�����ͷ�ͬ���ź���
*
* �������  : ��
* �������  : ��
*
* �� �� ֵ  : ��
*
* ����˵��  : ��
*
*****************************************************************************/
static int IFC_HandleMsg(void *__unused)
{
	IFC_RCV_FUNC_NODE_S * pNode = NULL;

	for (;;)
	{
		/* �ȴ��հ������ź������������Ҫ����Ľ��պ����ʹ���*/
		down(&g_stIFCCtx.semIFCRecvTask);
		g_stIFCDebugInfo.u32IFCRcvTaskRunCnt++;

		/* �Ӻ���������ȡһ�������ڵ㴦��*/
		down(&g_stIFCCtx.semIFCRecv);

		pNode = list_first_entry(&g_stIFCCtx.rcvFuncList, IFC_RCV_FUNC_NODE_S, node);

		g_stIFCDebugInfo.u32IFCRcvFuncNodeCnt--;
		up(&g_stIFCCtx.semIFCRecv);

		/* ����������Ϊ��ʱ��ѭ������ֱ���������*/
		while ( (pNode != NULL) && ((unsigned int)pNode->pFunc != 0xFFFFFFFF) ) /*judge pNode if NULL,clean lint warning e413*/
		{
            /*���pNode ��ָ������*/
			/*lint -e613*/
			if (pNode->pFunc)
			{
				/* MsgBody ��Ҫȥ��moduleid 4���ֽ�*/
				(BSP_VOID)pNode->pFunc((BSP_U8*)(pNode->u32Arg0 + sizeof(BSP_U32)),pNode->u32Arg1);
			}

			list_del(&pNode->node);

			/* �ͷ�msgbody��node�ڴ�*/
			kfree((BSP_U8*)pNode->u32Arg0);
			kfree(pNode);
            /*lint +e613*/
			
			/* �Ӻ���������ȡ��һ�������ڵ㴦��*/
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
/* BEGIN: Modified by liumengcun, 2011-1-27 debug����*/
BSP_VOID IFC_Help(void)
{
#if 0
	logMsg("\r |*************************************|\n",0,0,0,0,0,0);    
	logMsg("\r IFC_debugCntShow : �鿴����ͳ����Ϣ\n",0,0,0,0,0,0);    
	logMsg("\r IFC_debugCtxShow : �鿴ȫ����Ϣ\n",0,0,0,0,0,0);
	logMsg("\r IFC_debugCbShow  : �鿴�ص�������Ϣ\n",0,0,0,0,0,0);
	logMsg("\r |*************************************|\n",0,0,0,0,0,0);
#endif
}

BSP_VOID IFC_debugCtxShow(void)
{
#if 0
	logMsg("\r The IFC Context info:\n",0,0,0,0,0,0);        
	logMsg("\r IFC�Ƿ��ʼ��             : 0x%x\n",(BSP_S32)g_u32IsIFCInit,0,0,0,0,0);    
	logMsg("\r ����UDIͨ��handle         : 0x%x\n",(BSP_S32)g_stIFCCtx.IFCSendHandle,0,0,0,0,0);    
	logMsg("\r ����UDIͨ��handle         : 0x%x\n",(BSP_S32)g_stIFCCtx.IFCReceiveHandle,0,0,0,0,0);    
	logMsg("\r �հ����������ź���        : 0x%x\n",(BSP_S32)g_stIFCCtx.semIFCRecv,0,0,0,0,0);  
	logMsg("\r �հ�����ͬ���ź���        : 0x%x\n",(BSP_S32)g_stIFCCtx.semIFCRecvTask,0,0,0,0,0);
	logMsg("\r �հ��������ڵ�����      : 0x%x\n",(BSP_S32)g_stIFCCtx.rcvFuncList,0,0,0,0,0); 
	logMsg("\r �հ�����id                : 0x%x\n",(BSP_S32)g_stIFCCtx.s32RcvTaskId,0,0,0,0,0);
#endif
}

BSP_VOID IFC_debugCbShow(void)
{
#if 0
	logMsg("\r The IFC callback func info:\n",0,0,0,0,0,0);
	logMsg("\r BSP�ص�����               :0x%x\n",(BSP_S32)g_stIFCCtx.paFuncCb[0] ,0,0,0,0,0);
	logMsg("\r LTE PS�ص�����            :0x%x\n",(BSP_S32)g_stIFCCtx.paFuncCb[1],0,0,0,0,0);
	logMsg("\r GU PS�ص�����             :0x%x\n",(BSP_S32)g_stIFCCtx.paFuncCb[2],0,0,0,0,0);
	logMsg("\r LTE MSP�ص�����           :0x%x\n",(BSP_S32)g_stIFCCtx.paFuncCb[3],0,0,0,0,0);
	logMsg("\r GU OM�ص�����             :0x%x\n",(BSP_S32)g_stIFCCtx.paFuncCb[4],0,0,0,0,0);
#endif
}

BSP_VOID IFC_debugCntShow(void)
{
#if 0
	logMsg("\r The IFC Debug Count Info:\n",0,0,0,0,0,0);        
	logMsg("\r IFC���ͽ������                  : 0x%x\n",(BSP_S32)g_stIFCDebugInfo.u32IFCSendInCnt,0,0,0,0,0);    
	logMsg("\r IFC���ͳɹ�����                  : 0x%x\n",(BSP_S32)g_stIFCDebugInfo.u32IFCSendSuccessCnt,0,0,0,0,0);    
	logMsg("\r IFC���Ͳ�����ʧ�ܴ���            : 0x%x\n",(BSP_S32)g_stIFCDebugInfo.u32IFCSendParamNullFailCnt,0,0,0,0,0);    
	logMsg("\r IFC����ģ��ID��Чʧ�ܴ���        : 0x%x\n",(BSP_S32)g_stIFCDebugInfo.u32IFCSendInvalidModuleIdCnt,0,0,0,0,0);    
	logMsg("\r IFC���͵���UDI�ӿڷ���ʧ�ܴ���   : 0x%x\n",(BSP_S32)g_stIFCDebugInfo.u32IFCSendUdiFailCnt,0,0,0,0,0);  
	logMsg("\r IFC����ICC fifo FULL����         : 0x%x\n",(BSP_S32)g_stIFCDebugInfo.u32IFCSendIccFIFOFullCnt,0,0,0,0,0);  
	logMsg("\r IFC����ICC fifo FULL�ȴ���ʱ���� : 0x%x\n",(BSP_S32)g_stIFCDebugInfo.u32IFCSendIccFIFOFullTimeOutCnt,0,0,0,0,0);  
	logMsg("\r IFC���ս������                  : 0x%x\n",(BSP_S32)g_stIFCDebugInfo.u32IFCRcvInCnt,0,0,0,0,0);
	logMsg("\r IFC���ճɹ�����                  : 0x%x\n",(BSP_S32)g_stIFCDebugInfo.u32IFCRcvSuccessCnt,0,0,0,0,0); 
	logMsg("\r IFC���ղ�����ʧ�ܴ���            : 0x%x\n",(BSP_S32)g_stIFCDebugInfo.u32IFCRcvParamNullFailCnt,0,0,0,0,0);
	logMsg("\r IFC����ģ��ID��Чʧ�ܴ���        : 0x%x\n",(BSP_S32)g_stIFCDebugInfo.u32IFCRcvInvalidModuleIdCnt,0,0,0,0,0); 
	logMsg("\r IFC��������MSG BODY�ڴ�ʧ�ܴ���  : 0x%x\n",(BSP_S32)g_stIFCDebugInfo.u32IFCRcvMsgMallocFailCnt,0,0,0,0,0);
	logMsg("\r IFC��������NODE�ڴ�ʧ�ܴ���      : 0x%x\n",(BSP_S32)g_stIFCDebugInfo.u32IFCRcvNodeMallocFailCnt,0,0,0,0,0); 
	logMsg("\r IFC���յ���UDI�ӿڷ���ʧ�ܴ���   : 0x%x\n",(BSP_S32)g_stIFCDebugInfo.u32IFCRcvUdiFailCnt,0,0,0,0,0); 
	logMsg("\r IFC���մ���������ڵ����      : 0x%x\n",(BSP_S32)g_stIFCDebugInfo.u32IFCRcvFuncNodeCnt,0,0,0,0,0);
	logMsg("\r IFC���մ����������д���          : 0x%x\n",(BSP_S32)g_stIFCDebugInfo.u32IFCRcvTaskRunCnt,0,0,0,0,0);
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

