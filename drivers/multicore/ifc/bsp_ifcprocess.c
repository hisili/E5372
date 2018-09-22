/*************************************************************************
*   ��Ȩ����(C) 1987-2004, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_IFCProcess.c
*
*   ��    �� :  liumengcun
*
*   ��    �� :  ����˼���÷�װ����
*
*   �޸ļ�¼ :  2011��2��26��  v1.00  liumengcun  ����
*
*************************************************************************/
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/delay.h>

#include "BSP.h"
#include "bsp_ifcprocess.h"

#include <mach/common/bsp_memory.h>
#include <mach/regs_nand.h>

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
BSP_U32 g_u32IFCPFuncCb[IFCP_FUNC_BUTT] = {0};

BSP_U32 BSP_IFCP_RegFunc(IFCP_FUNC_E enFuncId, BSP_IFCP_REG_FUNC pFuncCb)
{
	if (enFuncId >= IFCP_FUNC_BUTT)
	{
		printk(KERN_ERR "BSP_MODU_IFCP BSP_IFCP_RegFunc invalid para enFuncId = %d,  pFuncCb = 0x%x!\n",
			enFuncId, (BSP_U32)pFuncCb);
		return BSP_ERR_INVALID_PARA;
	}

	g_u32IFCPFuncCb[enFuncId] = (BSP_U32)pFuncCb;
	return OK;
}

BSP_S32 IFC_Process(BSP_VOID *pMsgBody,BSP_U32 u32Len)
{
	BSP_U32 u32FuncId = 0;    
	BSP_U32 u32Buffer;

	u32FuncId = *(BSP_U32 *)pMsgBody;

	if (u32FuncId >= IFCP_FUNC_BUTT)
	{
		printk(KERN_ERR "BSP_MODU_IFCP IFC_Process invalid para u32FuncId = %d!\n", u32FuncId);
		return BSP_ERR_INVALID_PARA;
	}

	printk(KERN_DEBUG "IFC_Process in u32FuncId = %d!\n", u32FuncId);

	u32Buffer = (BSP_U32)pMsgBody + sizeof(u32FuncId);
	if (g_u32IFCPFuncCb[u32FuncId])
	{
		((BSP_IFCP_REG_FUNC)g_u32IFCPFuncCb[u32FuncId])((BSP_U8 *)u32Buffer, u32Len - sizeof(BSP_U32));
	}

	return OK;    
}

#if 0
#define NANDF_INIT_FINISH_MAGIC 0x5a5a5a5a

NANDF_ATTR_S g_stNandFlashAttr = {BSP_FALSE, BSP_NULL, 0x80000000, 0x840, 12, 0, {0x800, 0x8, 5, 3, 0x20000, 0x400, 0x40}};


/*****************************************************************************
* name		: nandf_maincore_init_cb
*
* description	: ??o????��?��o?����?a�䨮o?3?��??��3��1|??�̡�
*
* input		:
* output		:
*
* return		:
*
* other		:
*
******************************************************************************/
BSP_S32 nandf_maincore_init_cb(BSP_VOID* pMsgBody, BSP_U32 u32Len)
{
	printk(KERN_ERR "nandf_maincore_init_cb in\n");

	if (NANDF_INIT_FINISH_MAGIC == *(BSP_U32 *)pMsgBody)
	{
		/* we should take the semaphore in nand init, and up it at here */
		/* down(&g_stNandFlashAttr.u32FlashInitSem); */

		printk(KERN_ERR "nandf_maincore_init_cb: msgbody = 0x%x \n", *(BSP_U32 *)pMsgBody);
		return OK;
	}

	printk(KERN_ERR "nandf_maincore_init_cb fail!\n");
	
	return ERROR;
}

/******************************************************************************
* name		: nandf_twocore_sync_init
*
* description	: two core sync init
*
* input		:
* output		:
*
* return		:
*
* other		:
*******************************************************************************/
BSP_U32 nandf_twocore_sync_init(void)
{
	sema_init(&g_stNandFlashAttr.u32FlashInitSem, SEM_EMPTY);

	printk("%s: nandf_twocore_sync_init ****************\n", __FILE__);

	BSP_IFCP_RegFunc(IFCP_FUNC_NANDFINIT, (BSP_IFCP_REG_FUNC)nandf_maincore_init_cb);

	return OK;
}
#endif

BSP_S32 IFCP_Init(void)
{
	BSP_S32 s32Result = 0;    
	BSP_U32 u32OtherCoreInitFlag = 0;/* ���Ӻ˳�ʼ����־*/
	BSP_U32 u32DelayTimes = IFCP_INIT_DELAY_TIMES;


	s32Result = (BSP_S32)BSP_IFC_RegFunc(IFC_MODULE_BSP, (BSP_IFC_REG_FUNC)IFC_Process);
	if (OK != s32Result)
	{
		printk("\r BSP_IFC_RegFunc fail, result = 0x%x\n", s32Result);
	}
	/* ��ʼ����ɱ�־��0*/
	*(BSP_U32 *)IFCP_INIT_FLAG_ADDR = 0;
#if 0
/*****************************************************************/
	s32Result = nandf_twocore_sync_init();
	if (OK != s32Result)
	{
		printk("\r NANDF_TwoCoreSyncInit fail, result = 0x%x\n", s32Result);

		return ERROR;
	}
/*****************************************************************/
#endif
	/* ��ʼ�����*/
	*(BSP_U32 *)IFCP_INIT_FLAG_ADDR = IFCP_INIT_FLAG;

	/* ���ȴ�����һ����IFC��ʼ�����*/
	u32OtherCoreInitFlag = *(BSP_U32 *)IFCP_OTHER_CORE_INIT_FLAG_ADDR;
	while ((u32DelayTimes) && (IFCP_INIT_FLAG != u32OtherCoreInitFlag))
	{
		msleep(10);
		u32DelayTimes--;
		u32OtherCoreInitFlag = *(BSP_U32 *)IFCP_OTHER_CORE_INIT_FLAG_ADDR;
	}

	if (IFCP_INIT_FLAG != u32OtherCoreInitFlag)
	{
		printk(KERN_ERR "BSP_MODU_IFCP IFC Process wait for another core ifc init timeout,u32DelayTimes = %d,\
			modem core flag = 0x%x,app core flag = 0x%x\n",
			u32DelayTimes,*(BSP_U32*)IFCP_MODEM_INIT_FLAG_ADDR,*(BSP_U32*)IFCP_APP_INIT_FLAG_ADDR);
		return ERROR;
	}

	printk(KERN_ERR "BSP_MODU_IFCP \r IFC Process init success!\n");

	return OK;
}

