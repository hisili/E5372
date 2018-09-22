/*************************************************************************
*   ��Ȩ����(C) 1987-2004, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_DRV_IFC.h
*
*   ��    �� :  liumengcun
*
*   ��    �� :  IFC�ڲ�ʹ��ͷ�ļ�
*
*   �޸ļ�¼ :  2011��1��11��  v1.00  liumengcun  ����
*
*************************************************************************/
#ifndef _BSP_DRV_IFC_H_
#define _BSP_DRV_IFC_H_

#ifdef __cplusplus
    extern "C" 
    {
#endif

#include "BSP.h"

#define IFC_RCV_TASK_PRIORITY			84			/* �ݶ������������ȼ�������*/

#define IFC_ICC_CHAN_NUM				0
#define IFC_ICC_CHAN_PRIORITY			ICC_CHAN_PRIORITY_HIGH
#define IFC_ICC_CHAN_TIMEOUT      2000
#define IFC_SEND_TIMEOUT				200

#define IFC_INIT_FLAG						0x5aa55aa5	/* ��ʼ����ɱ�־*/
#define IFC_INIT_DELAY_TIMES				500			/* 5S*/
#define IFC_MODEM_INIT_FLAG_ADDR		MEMORY_AXI_IFC_ADDR
#define IFC_APP_INIT_FLAG_ADDR			(MEMORY_AXI_IFC_ADDR + 4)

#ifdef BSP_CORE_MODEM
#define IFC_INIT_FLAG_ADDR				IFC_MODEM_INIT_FLAG_ADDR
#define IFC_OTHER_CORE_INIT_FLAG_ADDR	IFC_APP_INIT_FLAG_ADDR
#else
#define IFC_INIT_FLAG_ADDR				IFC_APP_INIT_FLAG_ADDR
#define IFC_OTHER_CORE_INIT_FLAG_ADDR	IFC_MODEM_INIT_FLAG_ADDR
#endif

/* ȫ�ֽṹ��Ϣ*/
typedef struct tagIFC_CTX_S
{
	UDI_HANDLE		IFCSendHandle;			/* ����UDIͨ��handle*/
	UDI_HANDLE		IFCReceiveHandle;			/* ����UDIͨ��handle*/
	struct semaphore	semIFCRecv;				/* �հ����������ź���*/    
	struct semaphore	semIFCRecvTask;			/* �հ�����ͬ���ź���*/
	struct semaphore	semIFCSend;				/* ����ͬ���ź���*/
	struct list_head	rcvFuncList;				/* �հ��������ڵ�����*/
	BSP_S32			s32RcvTaskId;			/* �հ�����id*/    
	BSP_S32			s32SendTaskId;			/* ��������id*/
	BSP_IFC_REG_FUNC	paFuncCb[IFC_MODULE_BUTT]; /* ����ģ��ע��Ļص�����ָ��*/
}IFC_CTX_S;

/* �հ������ڵ�ṹ*/
typedef struct tagIFC_RCV_FUNC_NODE_S
{
	struct list_head		node;
	BSP_IFC_REG_FUNC	pFunc;				/* ����ָ��*/
	BSP_U32				u32Arg0;				/* ��һ������*/
	BSP_U32				u32Arg1;				/* �����ڶ�������*/
}IFC_RCV_FUNC_NODE_S;

/* ������Ϣ*/
typedef struct tagIFC_DEBUG_INFO_S
{    
	BSP_U32  u32IFCSendInCnt;					/* IFC���ͽ������*/
	BSP_U32  u32IFCSendSuccessCnt;			/* IFC���ͳɹ�����*/
	BSP_U32  u32IFCSendParamNullFailCnt;		/* IFC���Ͳ�����ʧ�ܴ���*/
	BSP_U32  u32IFCSendInvalidModuleIdCnt;		/* IFC����ģ��ID��Чʧ�ܴ���*/    
	BSP_U32  u32IFCSendUdiFailCnt;				/* IFC���͵���UDI�ӿڷ���ʧ�ܴ���*/    
	BSP_U32  u32IFCSendIccFIFOFullCnt;			/* IFC����ICC fifo FULL����*/    
	BSP_U32  u32IFCSendIccFIFOFullTimeOutCnt;	/* IFC����ICC fifo FULL�ȴ���ʱ����*/    
	BSP_U32  u32IFCRcvInCnt;					/* IFC���ս������*/
	BSP_U32  u32IFCRcvSuccessCnt;				/* IFC���ճɹ�����*/
	BSP_U32  u32IFCRcvParamNullFailCnt;		/* IFC���ղ�����ʧ�ܴ���*/
	BSP_U32  u32IFCRcvInvalidModuleIdCnt;		/* IFC����ģ��ID��Чʧ�ܴ���*/    
	BSP_U32  u32IFCRcvMsgMallocFailCnt;		/* IFC��������MSG BODY�ڴ�ʧ�ܴ���*/    
	BSP_U32  u32IFCRcvNodeMallocFailCnt;		/* IFC��������NODE�ڴ�ʧ�ܴ���*/ 
	BSP_U32  u32IFCRcvUdiFailCnt;				/* IFC���յ���UDI�ӿڷ���ʧ�ܴ���*/ 
	BSP_U32  u32IFCRcvFuncNodeCnt;			/* IFC���մ���������ڵ����*/
	BSP_U32  u32IFCRcvTaskRunCnt;				/* IFC���մ����������д���*/
}IFC_DEBUG_INFO_S;

BSP_S32 IFC_Init(void);
BSP_VOID IFC_UnInit(void);
BSP_U32 IFC_Receive(BSP_U32 u32ChanID, BSP_U32 u32Len);
///BSP_VOID IFC_HandleMsg();
BSP_U32 IFC_SendHandle(BSP_U32 u32ChanID);

#ifdef __cplusplus
}
#endif


#endif //end of "_BSP_DRV_IFC_H_"


