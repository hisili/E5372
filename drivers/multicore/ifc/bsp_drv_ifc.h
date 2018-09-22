/*************************************************************************
*   版权所有(C) 1987-2004, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_DRV_IFC.h
*
*   作    者 :  liumengcun
*
*   描    述 :  IFC内部使用头文件
*
*   修改记录 :  2011年1月11日  v1.00  liumengcun  创建
*
*************************************************************************/
#ifndef _BSP_DRV_IFC_H_
#define _BSP_DRV_IFC_H_

#ifdef __cplusplus
    extern "C" 
    {
#endif

#include "BSP.h"

#define IFC_RCV_TASK_PRIORITY			84			/* 暂订，该任务优先级需讨论*/

#define IFC_ICC_CHAN_NUM				0
#define IFC_ICC_CHAN_PRIORITY			ICC_CHAN_PRIORITY_HIGH
#define IFC_ICC_CHAN_TIMEOUT      2000
#define IFC_SEND_TIMEOUT				200

#define IFC_INIT_FLAG						0x5aa55aa5	/* 初始化完成标志*/
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

/* 全局结构信息*/
typedef struct tagIFC_CTX_S
{
	UDI_HANDLE		IFCSendHandle;			/* 发送UDI通道handle*/
	UDI_HANDLE		IFCReceiveHandle;			/* 接收UDI通道handle*/
	struct semaphore	semIFCRecv;				/* 收包函数互斥信号量*/    
	struct semaphore	semIFCRecvTask;			/* 收包任务同步信号量*/
	struct semaphore	semIFCSend;				/* 发送同步信号量*/
	struct list_head	rcvFuncList;				/* 收包任务函数节点链表*/
	BSP_S32			s32RcvTaskId;			/* 收包任务id*/    
	BSP_S32			s32SendTaskId;			/* 发包任务id*/
	BSP_IFC_REG_FUNC	paFuncCb[IFC_MODULE_BUTT]; /* 各个模块注册的回调函数指针*/
}IFC_CTX_S;

/* 收包函数节点结构*/
typedef struct tagIFC_RCV_FUNC_NODE_S
{
	struct list_head		node;
	BSP_IFC_REG_FUNC	pFunc;				/* 函数指针*/
	BSP_U32				u32Arg0;				/* 第一个参数*/
	BSP_U32				u32Arg1;				/* 函数第二个参数*/
}IFC_RCV_FUNC_NODE_S;

/* 调试信息*/
typedef struct tagIFC_DEBUG_INFO_S
{    
	BSP_U32  u32IFCSendInCnt;					/* IFC发送进入次数*/
	BSP_U32  u32IFCSendSuccessCnt;			/* IFC发送成功次数*/
	BSP_U32  u32IFCSendParamNullFailCnt;		/* IFC发送参数空失败次数*/
	BSP_U32  u32IFCSendInvalidModuleIdCnt;		/* IFC发送模块ID无效失败次数*/    
	BSP_U32  u32IFCSendUdiFailCnt;				/* IFC发送调用UDI接口发送失败次数*/    
	BSP_U32  u32IFCSendIccFIFOFullCnt;			/* IFC发送ICC fifo FULL次数*/    
	BSP_U32  u32IFCSendIccFIFOFullTimeOutCnt;	/* IFC发送ICC fifo FULL等待超时次数*/    
	BSP_U32  u32IFCRcvInCnt;					/* IFC接收进入次数*/
	BSP_U32  u32IFCRcvSuccessCnt;				/* IFC接收成功次数*/
	BSP_U32  u32IFCRcvParamNullFailCnt;		/* IFC接收参数空失败次数*/
	BSP_U32  u32IFCRcvInvalidModuleIdCnt;		/* IFC接收模块ID无效失败次数*/    
	BSP_U32  u32IFCRcvMsgMallocFailCnt;		/* IFC接收申请MSG BODY内存失败次数*/    
	BSP_U32  u32IFCRcvNodeMallocFailCnt;		/* IFC接收申请NODE内存失败次数*/ 
	BSP_U32  u32IFCRcvUdiFailCnt;				/* IFC接收调用UDI接口发送失败次数*/ 
	BSP_U32  u32IFCRcvFuncNodeCnt;			/* IFC接收处理函数链表节点个数*/
	BSP_U32  u32IFCRcvTaskRunCnt;				/* IFC接收处理任务运行次数*/
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


