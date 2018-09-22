/*************************************************************************
*   ��Ȩ����(C) 1987-2004, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_DRV_IPC.h
*
*   ��    �� :  wangjing
*
*   ��    �� :  IPCģ���û��ӿ��ļ�
*
*   �޸ļ�¼ :  2011��4��11��  v1.00  wangjing  ����
*************************************************************************/

#ifndef _BSP_DRV_IPC_H_
#define _BSP_DRV_IPC_H_

#include <asm/io.h>
#include "BSP.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined (BOARD_FPGA) && defined (VERSION_V3R2)
#define BSP_IPC_BASE_ADDR                   0x5f054000
#else
#define BSP_IPC_BASE_ADDR                   0x900A5000
#endif
#define BSP_IPC_CPU_RAW_INT(i)		(0x400 + (i * 0x10))
#define BSP_IPC_CPU_INT_MASK(i)		(0x404 + (i * 0x10))
#define BSP_IPC_CPU_INT_STAT(i)		(0x408 + (i * 0x10))
#define BSP_IPC_CPU_INT_CLR(i)		(0x40C + (i * 0x10))

#define BSP_IPC_SEM_RAW_INT(i)		(0x600 + (i * 0x10))
#define BSP_IPC_SEM_INT_MASK(i)		(0x604 + (i * 0x10))
#define BSP_IPC_SEM_INT_STAT(i)		(0x608 + (i * 0x10))
#define BSP_IPC_SEM_INT_CLR(i)		(0x60C + (i * 0x10))

#define BSP_IPC_HS_CTRL(i,j)			(0x800 + (i * 0x100) + (j * 0x8 ))
#define BSP_IPC_HS_STAT(i,j)			(0x804 + (i * 0x100) + (j * 0x8 ))

#define IPC_REG_SIZE		0x1000

#define IPC_MASK						0xFFFFFF0F       /*��������[4:7]bitλ*/

/*������Ҫд��ARM_paxb.h��*/
#if defined (BOARD_FPGA) && defined (VERSION_V3R2)
#define A9_GLOBAL_INTS              32
#define INT_LVL_HW_MAX              128
#define INT_LVL_SW_MAX              16
#define INT_LVL_MAX                 ((INT_LVL_HW_MAX) + (INT_LVL_SW_MAX))
#define INT_LEV_IPC_COMBINE          (INT_LVL_MAX + 15)
#else
#define INT_LEV_IPC_CPU				32
#define INT_LEV_IPC_SEM				33
#endif

#define INT_VEC_IPC_SEM				IVEC_TO_INUM(INT_LEV_IPC_SEM)
#define INTSRC_NUM					32

#define INT_VEC_IPC_CPU				IVEC_TO_INUM(INT_LEV_IPC_CPU)
#define IPC_CHECK_PARA(ulLvl) \
	do{\
		if(ulLvl >= INTSRC_NUM)\
		{\
			printk("Wrong para , line:%d\n", __LINE__);\
			return BSP_ERROR;\
		}\
	}while(0)
	
#define IPC_CHECK_PARA_NO_RET(ulLvl) \
	do{\
		if(ulLvl >= INTSRC_NUM)\
		{\
			printk("Wrong para , line:%d\n", __LINE__);\
			return;\
		}\
	}while(0)		

#define BSP_REG_READ(base, reg, resule) \
	(resule = readl(base + reg))
		
#define BSP_REG_WRITE(base, reg, data) \
	(writel(data, (base + reg)))
 
typedef struct tagIPC_DEV_S
{
	BSP_BOOL            bInit;
}IPC_DEV_S;    

typedef struct {	
	VOIDFUNCPTR	routine;
	BSP_U32	arg;
} BSP_IPC_ENTRY;

typedef struct tagIPC_DEBUG_E
{
	BSP_U32 u32RecvIntCore;
	BSP_U32 u32IntHandleTimes[INTSRC_NUM];
	BSP_U32 u32IntSendTimes[INTSRC_NUM];
	BSP_U32 u32SemId;
	BSP_U32 u32SemTakeTimes[INTSRC_NUM];
	BSP_U32 u32SemGiveTimes[INTSRC_NUM];
	BSP_U32 u32SemHandleTimes[INTSRC_NUM];
}IPC_DEBUG_E;


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
BSP_S32 BSP_DRV_IPCIntInit(void);

BSP_S32 BSP_IPC_IntEnable (IPC_INT_LEV_E ulLvl);

BSP_S32 BSP_IPC_IntConnect  (IPC_INT_LEV_E ulLvl, VOIDFUNCPTR routine, BSP_U32 parameter);

BSP_S32 BSP_IPC_IntSend(IPC_INT_CORE_E enDstCore, IPC_INT_LEV_E ulLvl);

BSP_VOID BSP_IPC_SpinLock (BSP_U32 u32SignalNum);

BSP_VOID BSP_IPC_SpinUnLock (BSP_U32 u32SignalNum);



#ifdef __cplusplus
}
#endif

#endif /* end #define _BSP_IPC_H_*/

