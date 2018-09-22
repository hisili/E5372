/*************************************************************************
*   ��Ȩ����(C) 1987-2010, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_ICC.c
*
*   ��    �� :  zhouluojun
*
*   ��    �� :  ���ļ���Ҫ��ɹ����ڴ�ͨ��������д�ӿڷ�װ.
*
*   �޸ļ�¼ :  2011��1��14��  v1.00  zhouluojun  ����
*************************************************************************/

#include <linux/kernel.h>
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
#include <linux/sched.h>
#include <mach/hardware.h>

#include <mach/spinLockAmp.h>
#include <mach/common/mem/bsp_mem.h>
#include <mach/balong_v100r001.h>
#include "BSP.h"
#include "bsp_icc_drv.h"
#include "bsp_icc_om.h"
#include "bsp_udi_drv.h"
#if(FEATURE_ICC_DEBUG == FEATURE_ON)
#include "icc_mntn.h"
#include "excDrv.h"  
#endif
#include "bsp_drv_ipc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ICC_TASK_PRIORITY     85

#ifndef IO_TYPE
#define IO_TYPE(var) ((ICC_SUB_CHAN_S *)IO_ADDRESS((unsigned int)(var)))
#endif/*IO_TYPE*/

#if(FEATURE_ICC_DEBUG == FEATURE_ON)
extern unsigned int omTimerGet(void);

#endif

/* static BSP_S32			g_s32IccTaskId = 0; */
ICC_CTRL_CHAN_S		*g_pstIccCtrlChan;
static ICC_DEV_S	 g_stIccDev={0};
ICC_CHAN_DEV_S	    *g_pstOpenList = BSP_NULL;

struct semaphore	g_semIccList;
struct semaphore	g_semIccTask;
struct semaphore	g_semIccCtrl;
struct semaphore	g_semIpcSpinLock;

static BSP_BOOL            g_IccStMode = BSP_FALSE;


#if (defined (BOARD_ASIC) || defined (BOARD_SFT))
#define ICC_SPIN_LOCK(a)	\
        down(&g_semIpcSpinLock);\
        BSP_IPC_SpinLock(0)
#define ICC_SPIN_UNLOCK(a)	\        
        BSP_IPC_SpinUnLock(0); \
        up(&g_semIpcSpinLock)

#define SYSCTRL_INT_ADDR    (INTEGRATOR_SC_BASE + 0x4e4)

#define INT_VEC_ICC			        INT_VEC_SY_CONTROL0
#define ICC_SYSCTRL_INT_HOST	    0
#define ICC_SYSCTRL_INT_TARGET		2

#elif  defined (BOARD_FPGA_P500)
#define ICC_SPIN_LOCK(a)	\
        down(&g_semIpcSpinLock);\
        spinLockAmpTake(a)
#define ICC_SPIN_UNLOCK(a)	\        
        spinLockAmpGive(a); \
        up(&g_semIpcSpinLock)

#define INT_VEC_ICC             11
#define INT_VEC_ICC_TARGET      10
#define ICC_CPU_TARGET          0x1

#define ARM_GIC_IPI_COUNT		16
#define GIC_SW_INTERRUPT			(IO_ADDRESS(PBXA9_GIC_DIST_CONTROL) + 0xf00)

static int arm_gic_ipi_gen(unsigned int ipiID, unsigned int cpus)
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
#endif


/* Wakes up icc_thread */
/*lint -e110*/
static DECLARE_WAIT_QUEUE_HEAD(icc_wait);/*lint !e63 !e35 !e43 !e24 !e40*/
/*lint +e110*/
static struct task_struct *icc_task;
static BSP_U32 icc_task_id;
static int thread_flag = 0;

#if(FEATURE_ICC_DEBUG == FEATURE_ON)
BSP_U8 icc_debug_print = FALSE;
#endif

BSP_VOID ICC_SetTestMode(BSP_BOOL bEnable)
{
	g_IccStMode = bEnable;
	return;
}

void ICC_IntGen(void)
{
#if defined (BOARD_FPGA_P500)
	int ret = 0;

	ret = arm_gic_ipi_gen(INT_VEC_ICC_TARGET, ICC_CPU_TARGET);
	if (ret)
	{
		printk(KERN_ERR "BSP_MODU_ICC ipi gen failed.\n");
		return;
	}
#elif (defined (BOARD_ASIC) || defined (BOARD_SFT))
    BSP_REG_SETBITS(IO_ADDRESS(SYSCTRL_INT_ADDR), 0, ICC_SYSCTRL_INT_TARGET, 1, 1);
#else
    #error
#endif
}


#if(FEATURE_ICC_DEBUG == FEATURE_ON)
/*****************************************************************************
* �� �� ��  : ICC_debug_print_send()
*
* ��������  : �����Ƿ��ӡʱ���
*
* �������  : 1����ӡ  0������ӡ
*
* �������  : 
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_VOID ICC_debug_print_send(BSP_U32 value)
{    
    icc_debug_print = value;
    printk( "icc_debug_print=%d\n",value);
    return;
}
#endif

/*****************************************************************************
* �� �� ��  : ICC_NotifyTarget
*
* ��������  : ʹ���жϸ�Ŀ�괦���������ж�
*
* �������  : 
*
* �������  : 
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_VOID ICC_NotifyTarget(ICC_CHAN_DEV_S *pChan, BSP_U32 signal)
{
	unsigned long flag = 0;
    
	local_irq_save(flag);
	
	ICC_SPIN_LOCK(&pChan->ptxSubChanInfo->u32SpinLock);

	pChan->ptxSubChanInfo->u32Signal |= signal;

	ICC_SPIN_UNLOCK(&pChan->ptxSubChanInfo->u32SpinLock);


	/* ��Ҫ�߼�����󣬴�����Ӧ���ж� */
	ICC_IntGen();

    /*����������Ͳ��Ե�����-- linux�ں�*/
    /*lint -e516*/
	local_irq_restore(flag);
	/*lint +e516*/

	return;
}


/*****************************************************************************
* �� �� ��  : ICC_FIFOIdleSize
*
* ��������  : ��ѯͨ���еĿ����ڴ��С
*
* �������  : pSubChan      ��ͨ��
*
* �������  : IdleSize1     ����buffer1
*             IdleSize2     ����buffer2
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_VOID ICC_FIFOIdleSize(ICC_SUB_CHAN_S *pSubChan, BSP_S32 *IdleSize1, BSP_S32 *IdleSize2)
{
	BSP_U32 start = (BSP_U32)pSubChan->pStart;
	BSP_U32 end = (BSP_U32)pSubChan->pEnd;
	BSP_U32 read =  (BSP_U32)pSubChan->pRead;
	BSP_U32 write = (BSP_U32)pSubChan->pWrite;

	if (write < read)
	{
		/* ��ָ�����дָ�룬ֱ�Ӽ��� */
		*IdleSize1 = (BSP_S32)(read - write - 1);
		*IdleSize2 = 0;
	}
	else
	{
		/* дָ����ڶ�ָ�룬��Ҫ���ǻؾ� */
		if (read != start)
		{
			*IdleSize1 = (BSP_S32)(end - write + 1);
			*IdleSize2 = (BSP_S32)(read - start - 1);
		}
		else
		{
			*IdleSize1 = (BSP_S32)(end - write);
			*IdleSize2 = 0;
		}
	}

	return;
}


/*****************************************************************************
* �� �� ��  : ICC_FIFODataSize
*
* ��������  : ��ѯͨ���е���Ч�����ڴ��С
*
* �������  : pSubChan      ��ͨ��
*
* �������  : DataSize1     ��Ч����buffer1
*             DataSize2     ��Ч����buffer2
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_VOID ICC_FIFODataSize(ICC_SUB_CHAN_S *pSubChan, BSP_S32 *s32DataSize1, BSP_S32 *s32DataSize2)
{
	BSP_U32 start = (BSP_U32)PHY_TO_VIRT(pSubChan->pStart);
	BSP_U32 end = (BSP_U32)PHY_TO_VIRT(pSubChan->pEnd);
	BSP_U32 read =  (BSP_U32)PHY_TO_VIRT(pSubChan->pRead);
	BSP_U32 write = (BSP_U32)PHY_TO_VIRT(pSubChan->pWrite);

	if (read <= write)
	{
		/* дָ����ڶ�ָ�룬ֱ�Ӽ��� */
		*s32DataSize1 = (BSP_S32)(write - read);
		*s32DataSize2 = 0;
	}
	else
	{
		/* ��ָ�����дָ�룬��Ҫ���ǻؾ� */
		*s32DataSize1 = (BSP_S32)(end - read + 1);
		*s32DataSize2 = (BSP_S32)(write - start);
	}

	return;
}


/*****************************************************************************
* �� �� ��  : ICC_ListAdd
*
* ��������  : ��ͨ������������һ���ڵ�
*
* �������  : pList         ����ָ��
*             pNode         ���ݽڵ�
*             flag          ���ӷ�ʽ
*
* �������  : 
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_S32 ICC_ListAdd(ICC_CHAN_DEV_S **pList, ICC_CHAN_DEV_S *pNode)
{
	ICC_CHAN_DEV_S *pCur = *pList;
	ICC_CHAN_DEV_S *pPre = BSP_NULL;

	down_timeout(&g_semIccList, msecs_to_jiffies(200));

	while((BSP_NULL != pCur) && (pCur->u32Priority <= pNode->u32Priority))
	{
		pPre = pCur;
		pCur = pCur->next;
	}

	if (pPre)
	{
		pNode->next = pCur;
		pPre->next = pNode;
	}
	else
	{
		*pList = pNode;
	}

	up(&g_semIccList);

	return BSP_OK;
}


/*****************************************************************************
* �� �� ��  : ICC_ListGet
*
* ��������  : ��ͨ�������в�ѯһ���ڵ�
*
* �������  : pList         ����ָ��
*             u32ChanId        ͨ��ID
*
* �������  : pNode         �ڵ�ָ��
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_VOID ICC_ListGet(ICC_CHAN_DEV_S *pList, BSP_U32 u32ChanId, ICC_CHAN_DEV_S **pNode)
{
	ICC_CHAN_DEV_S *pCur = pList;
	*pNode = BSP_NULL;

	down_timeout(&g_semIccList, msecs_to_jiffies(200));

	while(pCur)
	{
		if (pCur->u32ChanID == u32ChanId)
		{
			*pNode = pCur;
			break;
		}
		pCur = pCur->next;
	}

	up(&g_semIccList);

	return;
}


/*****************************************************************************
* �� �� ��  : ICC_ListGet
*
* ��������  : ��ͨ�������в�ѯһ���ڵ�
*
* �������  : pList         ����ָ��
*             u32ChanId        ͨ��ID
*
* �������  : pNode         �ڵ�ָ��
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_VOID ICC_ListGet_Noblock(ICC_CHAN_DEV_S *pList, BSP_U32 u32ChanId, ICC_CHAN_DEV_S **pNode)
{
	ICC_CHAN_DEV_S *pCur = pList;
	*pNode = BSP_NULL;

	while(pCur)
	{
		if (pCur->u32ChanID == u32ChanId)
		{
			*pNode = pCur;
			break;
		}
		pCur = pCur->next;
	}

	return;
}

/*****************************************************************************
* �� �� ��  : ICC_ListGetOut
*
* ��������  : ��ͨ��������ȡ��һ���ڵ�
*
* �������  : pList         ����ָ��
*             u32ChanId        ͨ��ID
*
* �������  : pNode         �ڵ�ָ��
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_VOID ICC_ListGetOut(ICC_CHAN_DEV_S **pList, BSP_U32 u32ChanId, ICC_CHAN_DEV_S **pNode)
{
	ICC_CHAN_DEV_S *pCur = *pList;
	ICC_CHAN_DEV_S *pPre = pCur;
	*pNode = BSP_NULL;

	down_timeout(&g_semIccList, msecs_to_jiffies(200));

	while (pCur)
	{
		if (pCur->u32ChanID == u32ChanId)
		{
			*pNode = pCur;

			/* ɾ���ڵ� */
			if (pCur != pPre)
			{
				pPre->next = pCur->next;
				pCur->next = BSP_NULL;
			}
			else
			{
				*pList = pCur->next;
			}
			break;
		}
		pPre = pCur;
		pCur = pCur->next;
	}

	up(&g_semIccList);

	return;
}


/*****************************************************************************
* �� �� ��  : ICC_AllocChannel
*
* ��������  : ����һ��ͨ��
*
* �������  : u32ChanId        ͨ��ID
*             FIFOInSize    ����FIFO��С
*             FIFOOutSize   ���FIFO��С
*
* �������  : 
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_S32 ICC_AllocChannel(BSP_U32 u32ChanId, ICC_CHAN_ATTR_S *pChanAttr)
{
	BSP_VOID *pMem1, *pMem2, *pMem3, *pMem4;
	ICC_CHAN_S *pSmChan;
	ICC_SUB_CHAN_S *pTxChan, *pRxChan;

	pSmChan = &g_pstIccCtrlChan->astChanInfo[u32ChanId];

    /* �ֱ����뷢�ͺͽ���ͨ����info��fifo�ֿ����� */
    pMem1 = BSP_SMALLOC(sizeof(ICC_SUB_CHAN_S), MEM_ICC_AXI_POOL);
    if(!pMem1)
    {
        printk(KERN_ERR "BSP_MODU_ICC channel[%d] alloc sm info failed.\n",u32ChanId);
        return BSP_ERR_ICC_NO_MEM;
    }

    pMem2 = BSP_SMALLOC(sizeof(ICC_SUB_CHAN_S), MEM_ICC_AXI_POOL);
    if(!pMem2)
    {
        BSP_SFree(pMem1);
        printk(KERN_ERR "BSP_MODU_ICC channel[%d] alloc sm info failed.\n",u32ChanId);
        return BSP_ERR_ICC_NO_MEM;
    }

    pMem3 = BSP_SMALLOC(pChanAttr->u32FIFOOutSize, MEM_ICC_DDR_POOL);
    if(!pMem3)
    {
        BSP_SFree(pMem1);
        BSP_SFree(pMem2);
        printk(KERN_ERR "BSP_MODU_ICC channel[%d] alloc sm info failed.\n",u32ChanId);
        return BSP_ERR_ICC_NO_MEM;
    }
    
    pMem4 = BSP_SMALLOC(pChanAttr->u32FIFOInSize, MEM_ICC_DDR_POOL);
    if(!pMem4)
    {
        BSP_SFree(pMem1);
        BSP_SFree(pMem2);
        BSP_SFree(pMem3);
        printk(KERN_ERR "BSP_MODU_ICC channel[%d] alloc sm info failed.\n",u32ChanId);
        return BSP_ERR_ICC_NO_MEM;
    }
    
    /* ��ʼ��״̬ */
    pSmChan->enState = ICC_CHAN_STATE_CLOSED;
    pSmChan->enMode = pChanAttr->enChanMode;
    pSmChan->u32Priority = pChanAttr->u32Priority;
    pSmChan->u32SpinLock = 0;
    pSmChan->pSubChanInfo[ICC_HOST] = pMem1;
    pSmChan->pSubChanInfo[ICC_TAGET] = pMem2;
    pSmChan->bAlloc= BSP_TRUE;

    /* ��ʼ������ͨ��������ͨ����FIFO��ǰ����Ӧ��FIFO size��FIFOOutSize */
    pTxChan = pSmChan->pSubChanInfo[ICC_HOST];
    pTxChan->u32SubChanState = ICC_CHAN_STATE_CLOSED;
    pTxChan->u32Signal = 0;
    pTxChan->u32SpinLock = 0;
    pTxChan->u32FIFOsize = pChanAttr->u32FIFOOutSize;
    pTxChan->pStart = (BSP_U8*)DDR_VIRT_TO_PHY(pMem3);
    pTxChan->pEnd = pTxChan->pStart + pChanAttr->u32FIFOOutSize - 1;
    pTxChan->pRead = pTxChan->pStart;
    pTxChan->pWrite = pTxChan->pStart;

    /*fix address for ccpu*/
    pSmChan->pSubChanInfo[ICC_HOST] = (ICC_SUB_CHAN_S *)DRV_AXI_VIRT_TO_PHY((unsigned int)pTxChan);
    
    /* ��ʼ������ͨ��������ͨ����FIFO�ں󣬶�Ӧ��FIFO size��FIFOInSize */
    pRxChan = pSmChan->pSubChanInfo[ICC_TAGET];
    pRxChan->u32SubChanState = ICC_CHAN_STATE_CLOSED;
    pRxChan->u32Signal = 0;
    pRxChan->u32SpinLock = 0;
    pRxChan->u32FIFOsize = pChanAttr->u32FIFOInSize;
    pRxChan->pStart = (BSP_U8*)DDR_VIRT_TO_PHY(pMem4);
    pRxChan->pEnd = pRxChan->pStart + pChanAttr->u32FIFOInSize - 1;
    pRxChan->pRead = pRxChan->pStart;
    pRxChan->pWrite = pRxChan->pStart;

    /*fix address for ccpu*/
    pSmChan->pSubChanInfo[ICC_TAGET] = (ICC_SUB_CHAN_S *)DRV_AXI_VIRT_TO_PHY((unsigned int)pRxChan);
	
	return BSP_OK;
}


/*****************************************************************************
* �� �� ��  : ICC_FreeChannel
*
* ��������  : �ͷ�һ��ͨ��
*
* �������  : u32ChanId        ͨ��ID
*
* �������  : 
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_VOID ICC_FreeChannel(BSP_U32 u32ChanId)
{
	ICC_CHAN_S *pSmChan = &g_pstIccCtrlChan->astChanInfo[u32ChanId];
       ICC_SUB_CHAN_S* temp = BSP_NULL;

	if (pSmChan->bAlloc)
	{
		{
            temp = (IO_TYPE(pSmChan->pSubChanInfo[ICC_HOST]));
			BSP_SFree(PHY_TO_VIRT(temp->pStart));
			temp = (IO_TYPE(pSmChan->pSubChanInfo[ICC_TAGET]));
			BSP_SFree(PHY_TO_VIRT(temp->pStart));
			BSP_SFree(IO_TYPE(pSmChan->pSubChanInfo[ICC_HOST]));
			BSP_SFree(IO_TYPE(pSmChan->pSubChanInfo[ICC_TAGET]));
            pSmChan->pSubChanInfo[ICC_HOST] = BSP_NULL;
            pSmChan->pSubChanInfo[ICC_TAGET] = BSP_NULL;
		}
		pSmChan->enState = ICC_CHAN_STATE_CLOSED;
	}

	pSmChan->bAlloc = BSP_FALSE;

	return;
}


/*****************************************************************************
* �� �� ��  : ICC_ResetChannel
*
* ��������  : ��λͨ��
*
* �������  : u32ChanId        ͨ��ID
*
* �������  : 
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_VOID ICC_ResetChannel(ICC_CHAN_DEV_S *pChan)
{
	/* ��λͨ���ɷ��Ͷ���ִ�У������ںϺ�д�������ֻ��� */
	ICC_NotifyTarget(pChan, ICC_CHAN_SIGNAL_RESET);
	return;
}


/*****************************************************************************
* �� �� ��  : ICC_SetEventCB
*
* ��������  : ���ö����ص�����
*
* �������  : u32ChanId        ͨ��ID
*             cb            �¼��ص�����
*
* �������  : 
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_S32 ICC_SetEventCB(BSP_U32 u32ChanId, icc_event_cb cb)
{
	ICC_CHAN_DEV_S  *pChan = BSP_NULL;

	/* ��ȡͨ�� */
	ICC_ListGet(g_pstOpenList, u32ChanId, &pChan);
	if (BSP_NULL == pChan)
	{
		printk(KERN_ERR "BSP_MODU_ICC channel[%d] is not open!.\n",u32ChanId);
		return BSP_ERR_ICC_NOT_OPEN;
	}

	pChan->event_cb = cb;

	return BSP_OK;
}


/*****************************************************************************
* �� �� ��  : ICC_SetReadCB
*
* ��������  : ���ö����ص�����
*
* �������  : u32ChanId        ͨ��ID
*             cb            �¼��ص�����
*
* �������  : 
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_S32 ICC_SetReadCB(BSP_U32 u32ChanId, icc_read_cb cb)
{
	ICC_CHAN_DEV_S  *pChan = BSP_NULL;

	/* ��ȡͨ�� */
	ICC_ListGet(g_pstOpenList, u32ChanId, &pChan);
	if (BSP_NULL == pChan)
	{
		printk(KERN_ERR "BSP_MODU_ICC channel[%d] is not open!.\n",u32ChanId);
		return BSP_ERR_ICC_NOT_OPEN;
	}

	pChan->read_cb = cb;

	return BSP_OK;
}


/*****************************************************************************
* �� �� ��  : ICC_SetWriteCB
*
* ��������  : ����д���ص�����
*
* �������  : u32ChanId        ͨ��ID
*             cb            �¼��ص�����
*
* �������  : 
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_S32 ICC_SetWriteCB(BSP_U32 u32ChanId, icc_write_cb cb)
{
	ICC_CHAN_DEV_S  *pChan = BSP_NULL;

	/* ��ȡͨ�� */
	ICC_ListGet(g_pstOpenList, u32ChanId, &pChan);
	if (BSP_NULL == pChan)
	{
		printk(KERN_ERR "BSP_MODU_ICC channel[%d] is not open!.\n",u32ChanId);
		return BSP_ERR_ICC_NOT_OPEN;
	}

	pChan->write_cb = cb;

	return BSP_OK;
}


/*****************************************************************************
* �� �� ��  : ICC_GetChanState
*
* ��������  : �ͷ�һ��ͨ��
*
* �������  : u32ChanId        ͨ��ID
*
* �������  : 
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_S32 ICC_GetChanState(BSP_U32 u32ChanId, ICC_CHAN_STATE_E *state)
{
	ICC_CHAN_DEV_S  *pChan = BSP_NULL;

	/* ��ȡͨ�� */
	ICC_ListGet(g_pstOpenList, u32ChanId, &pChan);
	if (BSP_NULL == pChan)
	{
		*state = ICC_CHAN_STATE_CLOSED;
	}

	*state = g_pstIccCtrlChan->astChanInfo[u32ChanId].enState;

	return BSP_OK;
}


/*****************************************************************************
* �� �� ��  : ICC_GetPriority
*
* ��������  : �ͷ�һ��ͨ��
*
* �������  : u32ChanId        ͨ��ID
*
* �������  : 
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_S32 ICC_GetPriority(BSP_U32 u32ChanId, BSP_U32 *priority)
{
	ICC_CHAN_DEV_S  *pChan = BSP_NULL;

	/* ��ȡͨ�� */
	ICC_ListGet(g_pstOpenList, u32ChanId, &pChan);
	if (BSP_NULL == pChan)
	{
		printk(KERN_ERR "BSP_MODU_ICC channel[%d] is not open!.\n",u32ChanId);
		return BSP_ERR_ICC_NOT_OPEN;
	}

	*priority = pChan->u32Priority;

	return BSP_OK;
}


/*****************************************************************************
* �� �� ��  : ICC_GetPacket
*
* ��������  : 
*
* �������  : 
*
* �������  : 
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_VOID ICC_CopyDataOut(BSP_U8* pData, BSP_S32 size, BSP_U8* start, BSP_U8* read, BSP_S32 size1)
{
	if (size <= size1)
	{
		ICC_INVALID_CACHE(read, (BSP_U32)size);
		/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 3) (int to unsigned int))*/
		memcpy(pData, read, (BSP_U32)size);
		/*end*/
	}
	else
	{
		ICC_INVALID_CACHE(read, (BSP_U32)size1);
		/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 3) (int to unsigned int))*/
		memcpy(pData, read, (BSP_U32)size1);
		/*end*/
        size -= size1;
        ICC_INVALID_CACHE(start, (BSP_U32)size);
		/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 3) (int to unsigned int))*/
        memcpy(pData+size1, start, (BSP_U32)size);
		/*end*/
	}
	return;
}


/*****************************************************************************
* �� �� ��  : ICC_GetPacket
*
* ��������  : 
*
* �������  : 
*
* �������  : 
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_VOID ICC_CopyDataIn(BSP_U8* pData, BSP_S32 size, BSP_U8* start, BSP_U8* write, BSP_S32 size1)
{
	if (size <= size1)
	{
		/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 3) (int to unsigned int))*/
		memcpy(write, pData, (BSP_U32)size);
		/*end*/
		ICC_FLUSH_CACHE(write, (BSP_U32)size);
	}
	else
	{
		/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 3) (int to unsigned int))*/
		memcpy(write, pData, (BSP_U32)size1);
		memcpy(start, pData+size1, (BSP_U32)(size-size1));
		/*end*/
		ICC_FLUSH_CACHE(write, (BSP_U32)size1);
		ICC_FLUSH_CACHE(start, (BSP_U32)size-(BSP_U32)size1);
	}
	return;
}


/*****************************************************************************
* �� �� ��  : ICC_GetPacket
*
* ��������  : 
*
* �������  : 
*
* �������  : 
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_S32 ICC_GetPacket(ICC_CHAN_DEV_S *pChan)
{
	BSP_S32 s32ReadSize, s32DataSize1, s32DataSize2, s32PacketSize, s32PayLoadSize;
	ICC_SUB_CHAN_S *pRxChan = pChan->prxSubChanInfo;
	BSP_U8* pRead = PHY_TO_VIRT(pChan->prxSubChanInfo->pRead);
    BSP_S32 header[(ICC_PACKET_HEADER_LEN/4)];
	BSP_U32 delta;
    #if(FEATURE_ICC_DEBUG == FEATURE_ON)
    BSP_S32 s32sendtime;
    BSP_S32 s32gettime;
    #endif
//    printk("\n============ get packet ===============\n");
//    printk("[%d] S:0x%x, E:0x%x, R:0x%x, W:0x%x\n", __LINE__, pRxChan->pStart, pRxChan->pEnd, pRxChan->pRead, pRxChan->pWrite);

	ICC_FIFODataSize(pRxChan, &s32DataSize1, &s32DataSize2);
	s32ReadSize = s32DataSize1+s32DataSize2;
//    printk("[%d] size:0x%x, size1:0x%x, size2:0x%x\n", __LINE__, s32ReadSize, s32DataSize1, s32DataSize2);

	delta = (BSP_U32)pRead%4;
	if (delta)
	{
		/* ��ָ���4�ֽڶ��룬�����ʼ��ȡ���� */
		delta = 4 - delta;
		pRead += delta;
		s32DataSize1-= (BSP_S32)delta;
		s32ReadSize -= (BSP_S32)delta;
		if (pRead > PHY_TO_VIRT(pRxChan->pEnd))
		{
			pRead = PHY_TO_VIRT(pRxChan->pStart);
			s32DataSize1 = s32DataSize2;
			s32DataSize2 = 0;
		}
	}

	if (s32ReadSize <= ICC_PACKET_HEADER_LEN)
	{
		/* ���ָ�ʽ���󣬸�λͨ�� */
		ICC_ResetChannel(pChan);
		printk(KERN_ERR "BSP_MODU_ICC data size is not correct!.\n");
		return BSP_ERROR;
	}
    
    /* packetģʽ��Ҫ�Ƚ�����������ͷ���� */
    ICC_CopyDataOut((BSP_U8*)header, ICC_PACKET_HEADER_LEN, PHY_TO_VIRT(pRxChan->pStart), 
                     pRead, s32DataSize1);
    s32PayLoadSize = header[1];
    s32PacketSize = s32PayLoadSize+ICC_PACKET_HEADER_LEN;
//    printk("[%d] pRead:0x%x, 0x%x, 0x%x\n", __LINE__, pRead, s32PacketSize, s32PayLoadSize);

    if (ICC_PACKET_HEADER != header[0])
    {
        /* ���ָ�ʽ���󣬰�ͷTAG����ȷ����λͨ�� */
        ICC_ResetChannel(pChan);
        printk(KERN_ERR "BSP_MODU_ICC pakcet header is invalid!. header=0x%x\n", header[0]);
        return BSP_ERROR;
    }

	if (s32PacketSize > s32ReadSize)
	{
		/* ���ָ�ʽ���󣬸�λͨ�� */
		ICC_ResetChannel(pChan);
		printk(KERN_ERR "BSP_MODU_ICC pakcet size is invalid!.\n");
		return BSP_ERROR;
	}
	//printk("[%d] pRead:0x%x, 0x%x, 0x%x\n", __LINE__, pRead, s32PacketSize, s32PayLoadSize);

	/* packet��Ч����¼��ǰpacket��Ϣ */
	pChan->packet.data = (BSP_U8*)DDR_VIRT_TO_PHY(pRead + ICC_PACKET_HEADER_LEN);
	if (PHY_TO_VIRT(pChan->packet.data) > PHY_TO_VIRT(pRxChan->pEnd))
	{
		pChan->packet.data -= pRxChan->u32FIFOsize;
	}
	pChan->packet.size = s32PayLoadSize;
	//printk("[%d] pRead:0x%x, 0x%x, 0x%x\n", __LINE__, pRead, pChan->packet.data, pChan->packet.size);
	
    #if(FEATURE_ICC_DEBUG == FEATURE_ON)
    s32sendtime = header[2];        /*���淢������ʱ��ʱ���*/
    s32gettime  = (BSP_S32)omTimerGet(); /*�����յ�����ʱ��ʱ���*/

    /*��ӡ���ݴ����ͨ����ʱ���*/
    if(TRUE == icc_debug_print)
    {
    	printk("\r\nCHAN:%d,Time: \n%u send the packet,\n%u get the packet  \r\n",pChan->u32ChanID, s32sendtime, s32gettime);
    }
    #endif
	return BSP_OK;
}


/*****************************************************************************
* �� �� ��  : ICC_SendStream
*
* ��������  : �¼�������
*
* �������  : 
*
* �������  : 
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_S32 ICC_SendStream(ICC_CHAN_DEV_S *pChan, BSP_U8 *pData, BSP_S32 s32Size)
{
    ICC_SUB_CHAN_S  *ptxChan = BSP_NULL;
    BSP_S32 s32IdleSize1, s32IdleSize2, s32CopySize;
    unsigned long flags = 0;

    down(&pChan->semSync);

    /* ��ȡ����buffer��С */
    local_irq_save(flags);
    ptxChan = pChan->ptxSubChanInfo;
    ICC_FIFOIdleSize(ptxChan, &s32IdleSize1, &s32IdleSize2);

	/* �������� */
	if (s32Size <= s32IdleSize1)
	{
		memcpy((BSP_VOID*)PHY_TO_VIRT(ptxChan->pWrite), pData, (BSP_U32)s32Size);
		s32CopySize = s32Size;
		ICC_FLUSH_CACHE(PHY_TO_VIRT(ptxChan->pWrite), (BSP_U32)s32Size);
	}
	else
	{
		BSP_S32 len;

		memcpy((BSP_VOID*)PHY_TO_VIRT(ptxChan->pWrite), pData, (BSP_U32)s32IdleSize1);
		len = ((s32Size-s32IdleSize1) > s32IdleSize2) ? s32IdleSize2 : (s32Size-s32IdleSize1);
		memcpy((BSP_VOID*)PHY_TO_VIRT(ptxChan->pStart), pData+s32IdleSize1, (BSP_U32)len);
		s32CopySize = s32IdleSize1+len;
		ICC_FLUSH_CACHE(PHY_TO_VIRT(ptxChan->pWrite), (BSP_U32)s32IdleSize1);
		ICC_FLUSH_CACHE(PHY_TO_VIRT(ptxChan->pStart), (BSP_U32)len);
	}

	/* ����дָ�� */
	ptxChan->pWrite += (BSP_U32)s32CopySize;
	if (PHY_TO_VIRT(ptxChan->pWrite) > PHY_TO_VIRT(ptxChan->pEnd))
	{
		ptxChan->pWrite -= ptxChan->u32FIFOsize;
	}

	if (s32CopySize < s32Size)
	{
		pChan->bWriteNeed = BSP_TRUE;
	}
	else
	{
		pChan->bWriteNeed = BSP_FALSE;
	}

    /*����������Ͳ��Ե�����-- linux�ں�*/
    /*lint -e516*/
	local_irq_restore(flags);
	/*lint +e516*/

	pChan->sDebug.u32SendTimes++;
	pChan->sDebug.u32SendBytes += (BSP_U32)s32CopySize;

	/* ֪ͨ�Է� */
	ICC_NotifyTarget(pChan, ICC_CHAN_SIGNAL_DATA);

	up(&pChan->semSync);

	return s32CopySize;
}


/*****************************************************************************
* �� �� ��  : ICC_SendPacket
*
* ��������  : �¼�������
*
* �������  : 
*
* �������  : 
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_S32 ICC_SendPacket(ICC_CHAN_DEV_S *pChan, BSP_U8 *pData, BSP_S32 s32Size)
{
	ICC_SUB_CHAN_S  *ptxChan = BSP_NULL;
	BSP_S32 s32IdleSize, s32IdleSize1, s32IdleSize2, s32PacketSize;
	BSP_U8* pWrite;
    BSP_S32 header[(ICC_PACKET_HEADER_LEN/4)];
	BSP_U32 delta;
	unsigned long flags = 0;
    
    #if(FEATURE_ICC_DEBUG == FEATURE_ON)
    BSP_S32 s32FifoSize;
    BSP_U8* CopyStart;
    BSP_S32 s32CopySize;
    BSP_S32 s32SaveAdd;
    BSP_S32 s32UsedSize;
    #endif
	down(&pChan->semSync);

	/* ��ȡ����buffer��С */
	local_irq_save(flags);
	ptxChan = pChan->ptxSubChanInfo;
	ICC_FIFOIdleSize(ptxChan, &s32IdleSize1, &s32IdleSize2);
	s32IdleSize = s32IdleSize1 + s32IdleSize2;

	/* ����packet��Ҫ4�ֽڶ��� */
	pWrite = PHY_TO_VIRT(ptxChan->pWrite);
	delta = (BSP_U32)pWrite % 4;
	if (delta)
	{
		/* дָ���4�ֽڶ��룬�����ʼд������ */
		delta = 4 - delta;
		pWrite += delta;
		s32IdleSize1 -= (BSP_S32)delta;
		s32IdleSize -= (BSP_S32)delta;
		if (pWrite > PHY_TO_VIRT(ptxChan->pEnd))
		{
			pWrite = PHY_TO_VIRT(ptxChan->pStart);
			s32IdleSize1 = s32IdleSize2;
			s32IdleSize2 = 0;
		}
	}

	s32PacketSize = s32Size+ICC_PACKET_HEADER_LEN;
	if (s32PacketSize > s32IdleSize)
	{
        /*����������Ͳ��Ե�����-- linux�ں�*/
        /*lint -e516*/
		if ((BSP_U32)s32PacketSize > ptxChan->u32FIFOsize - 4)
		{
			local_irq_restore(flags);
			up(&pChan->semSync);
			return BSP_ERR_ICC_PACKET_SIZE;
		}

		pChan->bWriteNeed = BSP_TRUE;
		local_irq_restore(flags);
		up(&pChan->semSync);
		/*lint +e516*/
		
		//printk(KERN_ERR "BSP_MODU_ICC buffer full read=0x%x! write=0x%x.\n", ptxChan->pRead, pWrite);
    #if(FEATURE_ICC_DEBUG == FEATURE_ON)
        s32FifoSize =ptxChan->u32FIFOsize;

        memcpy((void *) EXCH_A_ICC_SAVE_ADDR, pChan, sizeof (ICC_CHAN_DEV_S));
        memcpy((void *) (EXCH_A_ICC_SAVE_ADDR + sizeof (ICC_CHAN_DEV_S)), ptxChan, sizeof (ICC_SUB_CHAN_S));

        s32UsedSize=sizeof (ICC_SUB_CHAN_S)+sizeof (ICC_CHAN_DEV_S);        
        s32SaveAdd = EXCH_A_ICC_SAVE_ADDR+s32UsedSize;
        s32CopySize= EXCH_A_ICC_SAVE_SIZE-s32UsedSize;
        ICC_DEBUG_printk("((ptxChan->pWrite): %p,(ptxChan->pStart):%p,s32CopySize:%d \n",ptxChan->pWrite, ptxChan->pStart, s32CopySize);//test        
        ICC_DEBUG_printk("s32FifoSize:%d,s32CopySize:%d \n",s32FifoSize, s32CopySize);//test   

        if(s32FifoSize <s32CopySize)
        {
            CopyStart =(BSP_U8*)DRV_PHY_TO_VIRT(ptxChan->pStart);            
            ICC_DEBUG_printk("s32FifoSize <s32CopySize \n");//test
            memcpy((void *)s32SaveAdd, CopyStart, s32FifoSize);

        }

        else if(((ptxChan->pWrite)-(ptxChan->pStart))>s32CopySize)
        {
            CopyStart =(BSP_U8*)DRV_PHY_TO_VIRT((ptxChan->pWrite)-(s32CopySize));            
            ICC_DEBUG_printk("((ptxChan->pWrite)-(ptxChan->pStart))>s32CopySize \n");//test
            memcpy((void *)s32SaveAdd, CopyStart, s32CopySize);

        }

        else
        {
            ICC_DEBUG_printk("((ptxChan->pWrite)-(ptxChan->pStart))<s32CopySize \n");//test
            s32CopySize= (EXCH_A_ICC_SAVE_SIZE-s32UsedSize)-(ptxChan->pWrite- ptxChan->pStart);
            CopyStart = (BSP_U8*)DRV_PHY_TO_VIRT((ptxChan->pEnd)-(s32CopySize));
            memcpy((void *)s32SaveAdd, CopyStart, s32CopySize);
            CopyStart =(BSP_U8*)DRV_PHY_TO_VIRT(ptxChan->pStart);  
            s32CopySize= (ptxChan->pWrite- ptxChan->pStart);
            s32SaveAdd = EXCH_A_ICC_SAVE_ADDR+(s32UsedSize+s32CopySize);
            memcpy((void *)s32SaveAdd, CopyStart, s32CopySize);

        }
        

        *(volatile UINT32 *) EXCH_A_ICC_FLAG_ADDR = EXCH_A_CORE_FLAG;

    #endif
            //systemError(BSP_MODU_MNTN, 0, 0, 0, 0);//test    
		return BSP_ERR_ICC_BUFFER_FULL;
	}
	else
	{
        /*����������Ͳ��Ե�����-- linux�ں�*/
        /*lint -e516*/
		pChan->bWriteNeed = BSP_FALSE;
		local_irq_restore(flags);
		/*lint +e516*/
	}

	/* ������ͷ */
	header[0] = ICC_PACKET_HEADER;
	header[1] = s32Size;
	
    #if(FEATURE_ICC_DEBUG == FEATURE_ON)
    header[2] = (BSP_S32)omTimerGet();
    #endif
	
	ICC_CopyDataIn((BSP_U8*)header, ICC_PACKET_HEADER_LEN, PHY_TO_VIRT(ptxChan->pStart), 
			pWrite, s32IdleSize1);
	pWrite += ICC_PACKET_HEADER_LEN;
	s32IdleSize1 -= ICC_PACKET_HEADER_LEN;
	s32IdleSize -= ICC_PACKET_HEADER_LEN;
	if (pWrite > PHY_TO_VIRT(ptxChan->pEnd))
	{
		pWrite -= ptxChan->u32FIFOsize;
		s32IdleSize2 += s32IdleSize1;
		s32IdleSize1 = s32IdleSize2;
	}

	/* �������� */
	ICC_CopyDataIn(pData, s32Size, PHY_TO_VIRT(ptxChan->pStart), pWrite, s32IdleSize1);
	pWrite += s32Size;
	if (pWrite > PHY_TO_VIRT(ptxChan->pEnd))
	{
		pWrite -= ptxChan->u32FIFOsize;
	}

	/* ����дָ�� */
	ptxChan->pWrite = (BSP_U8*)DDR_VIRT_TO_PHY(pWrite);

	pChan->sDebug.u32SendTimes++;
	pChan->sDebug.u32SendBytes += (BSP_U32)s32Size;

#if(FEATURE_ICC_DEBUG == FEATURE_ON)    
    ICC_FIFOIdleSize(ptxChan, &s32IdleSize1, &s32IdleSize2);
    s32IdleSize = s32IdleSize1 + s32IdleSize2;
    pChan->sDebug.u32IdleSize = s32IdleSize;
#endif

	/* ֪ͨ�Է� */
	ICC_NotifyTarget(pChan, ICC_CHAN_SIGNAL_DATA);

	up(&pChan->semSync);

	return s32Size;
}


/*****************************************************************************
* �� �� ��  : ICC_ReceiveStream
*
* ��������  : �¼�������
*
* �������  : 
*
* �������  : 
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_VOID ICC_ReceiveStream(ICC_CHAN_DEV_S *pChan)
{
	BSP_S32 s32DataSize1, s32DataSize2;
	UDI_HANDLE handle;

	ICC_FIFODataSize(pChan->prxSubChanInfo, &s32DataSize1, &s32DataSize2);
	/*begin lint-Info 732: (Info -- Loss of sign (assignment) (int to unsigned int))*/
	pChan->sDebug.u32LastHaveBytes = (BSP_U32)(s32DataSize1+s32DataSize2);
	/*end*/

	/* streamģʽֱ�ӻص� */
	if (pChan->read_cb)
	{
		/*lint -e732*/
		if (!g_IccStMode)
		{
			handle = BSP_UDI_ICC_ChnToHandle(pChan->u32ChanID);
			(BSP_VOID)pChan->read_cb(handle, s32DataSize1+s32DataSize2);
		}
		/*lint +e732*/
		else
		{
			(BSP_VOID)pChan->read_cb(pChan->u32ChanID, s32DataSize1+s32DataSize2);
		}
	}

	return;
}


/*****************************************************************************
* �� �� ��  : ICC_ReceivePacket
*
* ��������  : �¼�������
*
* �������  : 
*
* �������  : 
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_VOID ICC_ReceivePacket(ICC_CHAN_DEV_S *pChan)
{
	UDI_HANDLE handle;

	/* ���ܻ����˶��packet��ͨ��ѭ�����δ��� */
	while(pChan->prxSubChanInfo->pRead != pChan->prxSubChanInfo->pWrite)
	{
		if(BSP_TRUE == pChan->bFirstRecv)
		{
			pChan->bFirstRecv = BSP_FALSE;
			msleep(100);
		}
		if(BSP_OK == ICC_GetPacket(pChan))
		{
			/* �ص�֪ͨ�û� */
			if(pChan->read_cb)
			{
				if(!g_IccStMode)
				{
					handle = BSP_UDI_ICC_ChnToHandle(pChan->u32ChanID);
					(BSP_VOID)pChan->read_cb(handle, pChan->packet.size);   /*lint !e732*/
				}
				else
				{
					(BSP_VOID)pChan->read_cb(pChan->u32ChanID, pChan->packet.size);
				}
			}
			else
			{
				break;
			}
		}
		else
		{
			printk(KERN_ERR "BSP_MODU_ICC get a packet fail!.\n");
			break;
		}
	}

	return;
}


/*****************************************************************************
* �� �� ��  : ICC_ReceiveStream
*
* ��������  : �¼�������
*
* �������  : 
*
* �������  : 
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_S32 ICC_ReadStream(ICC_CHAN_DEV_S *pChan, BSP_U8 *pData, BSP_S32 s32Size)
{
	BSP_S32 s32DataSize1, s32DataSize2, s32CopySize=0;
	BSP_U8*  pRead = PHY_TO_VIRT(pChan->prxSubChanInfo->pRead);

	ICC_FIFODataSize(pChan->prxSubChanInfo, &s32DataSize1, &s32DataSize2);

	if (s32DataSize1)
	{
		if (s32Size <= s32DataSize1)
		{
			ICC_INVALID_CACHE(pRead, (BSP_U32)s32Size);
			/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 1) (int to unsigned int))*/
			memcpy(pData, pRead, (BSP_U32)s32Size);
			/*end*/
			pRead += s32Size;
			if (pRead > PHY_TO_VIRT(pChan->prxSubChanInfo->pEnd))
			{
				pRead -= pChan->prxSubChanInfo->u32FIFOsize;
			}
			s32CopySize = s32Size;
		}
		else
		{
			/*begin lint-Info 732: (Info -- Loss of sign (initialization) (int to unsigned int))*/
			BSP_U32 len = (BSP_U32)(s32Size-s32DataSize1);
			/*end*/
			ICC_INVALID_CACHE(pRead, (BSP_U32)s32DataSize1);
			/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 3) (int to unsigned int))*/
			memcpy(pData, pRead, (BSP_U32)s32DataSize1);
			/*end*/
			if (len > (BSP_U32)s32DataSize2)
			{
				/*begin lint-Info 732: (Info -- Loss of sign (assignment) (int to unsigned int))*/
				len = (BSP_U32)s32DataSize2;
				/*end*/
			}
			ICC_INVALID_CACHE(PHY_TO_VIRT(pChan->prxSubChanInfo->pStart), len);
			memcpy(pData+s32DataSize1, PHY_TO_VIRT(pChan->prxSubChanInfo->pStart), len);
			s32CopySize = (BSP_U32)s32DataSize1+len;
			pRead = PHY_TO_VIRT(pChan->prxSubChanInfo->pStart) + len;
		}

		pChan->prxSubChanInfo->pRead = (BSP_U8*)DDR_VIRT_TO_PHY(pRead);

		pChan->sDebug.u32RecvTimes++;
		pChan->sDebug.u32RecvBytes += (BSP_U32)s32CopySize;
		/*begin lint-Info 732: (Info -- Loss of sign (assignment) (int to unsigned int))*/
		pChan->sDebug.u32LastReadBytes = (BSP_U32)s32CopySize;
		/*end*/

		/* ���FIFO�����ˣ�֪ͨ�Է�CPU */
		if (PHY_TO_VIRT(pChan->prxSubChanInfo->pRead) == PHY_TO_VIRT(pChan->prxSubChanInfo->pWrite))
		{
			ICC_NotifyTarget(pChan, ICC_CHAN_SIGNAL_EMPTY);
			pChan->sDebug.u32NoticeEmpty++;
		}
	}

	return s32CopySize;
}


/*****************************************************************************
* �� �� ��  : ICC_ReceivePacket
*
* ��������  : �¼�������
*
* �������  : 
*
* �������  : 
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_S32 ICC_ReadPacket(ICC_CHAN_DEV_S *pChan, BSP_U8 *pData, BSP_S32 s32Size)
{
	ICC_SUB_CHAN_S *pRxChan = pChan->prxSubChanInfo;
	BSP_U8* pRead = PHY_TO_VIRT(pChan->prxSubChanInfo->pRead);

//    printk("\n---- read packet -----\n");
//    printk("[%d] S:0x%x, E:0x%x, R:0x%x, W:0x%x\n", __LINE__, pRxChan->pStart, pRxChan->pEnd, pRxChan->pRead, pRxChan->pWrite);
//    printk("[%d] packet: 0x%x, 0x%x\n", __LINE__, pChan->packet.data, pChan->packet.size);
	if (0 == (pChan->packet.size))
	{
		if (PHY_TO_VIRT(pChan->prxSubChanInfo->pRead) != PHY_TO_VIRT(pChan->prxSubChanInfo->pWrite))
		{
			if (BSP_OK != ICC_GetPacket(pChan))
			{
				return BSP_ERR_ICC_INVALID_PACKET;
			}
		}
		else
		{
			return 0;
		}
	}

	if (pChan->packet.size)
	{
		if (pChan->packet.size > s32Size)
		{
			return BSP_ERR_ICC_USER_BUFFER_SIZE;
		}

		if (PHY_TO_VIRT(pChan->packet.data) + pChan->packet.size <= PHY_TO_VIRT(pRxChan->pEnd) + 1)
		{
			ICC_INVALID_CACHE(PHY_TO_VIRT(pChan->packet.data), (BSP_U32)pChan->packet.size);
			/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 3) (int to unsigned int))*/
			memcpy(pData, PHY_TO_VIRT(pChan->packet.data), (BSP_U32)(pChan->packet.size));
			/*end*/
		}
		else
		{
			BSP_S32 size1, size2;

			size1 = PHY_TO_VIRT(pRxChan->pEnd) + 1 - PHY_TO_VIRT(pChan->packet.data);
			size2 = pChan->packet.size - size1;
			ICC_INVALID_CACHE(PHY_TO_VIRT(pChan->packet.data), (BSP_U32)size1);
			/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 3) (int to unsigned int))*/
			memcpy(pData, PHY_TO_VIRT(pChan->packet.data), (BSP_U32)size1);
			/*end*/
			ICC_INVALID_CACHE(PHY_TO_VIRT(pRxChan->pStart), (BSP_U32)size2);
			/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 3) (int to unsigned int))*/
			memcpy(pData+size1, PHY_TO_VIRT(pRxChan->pStart), (BSP_U32)size2);
			/*end*/
		}

		pRead = PHY_TO_VIRT(pChan->packet.data) + pChan->packet.size;
		if (pRead > PHY_TO_VIRT(pRxChan->pEnd))
		{
			pRead -= pRxChan->u32FIFOsize;
		}

		/* ����״̬ */
		pChan->packet.data = BSP_NULL;
		pChan->packet.size = 0;
		pRxChan->pRead = (BSP_U8*)DDR_VIRT_TO_PHY(pRead);

		pChan->sDebug.u32RecvTimes++;
		pChan->sDebug.u32RecvBytes += (BSP_U32)s32Size;

		/* ���FIFO�����ˣ�֪ͨ�Է�CPU */
		if (PHY_TO_VIRT(pChan->prxSubChanInfo->pRead) == PHY_TO_VIRT(pChan->prxSubChanInfo->pWrite))
		{
			ICC_NotifyTarget(pChan, ICC_CHAN_SIGNAL_EMPTY);
			pChan->sDebug.u32NoticeEmpty++;
		}
	}

    return s32Size;
}


/*****************************************************************************
* �� �� ��  : ICC_HandleEvent
*
* ��������  : �¼�������
*
* �������  : 
*
* �������  : 
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_VOID ICC_HandleEvent(ICC_CHAN_DEV_S *pChan)
{
	///ICC_CHAN_S *pSmChan = &g_pstIccCtrlChan->astChanInfo[pChan->u32ChanID];
	BSP_U32 u32Signal;
	///BSP_U32 flag = 0;
	UDI_HANDLE handle;
	unsigned long flag = 0;
    
	local_irq_save(flag);

	ICC_SPIN_LOCK(&pChan->prxSubChanInfo->u32SpinLock);
	u32Signal = pChan->prxSubChanInfo->u32Signal;
	pChan->prxSubChanInfo->u32Signal &= (~u32Signal);
	ICC_SPIN_UNLOCK(&pChan->prxSubChanInfo->u32SpinLock);
    
	local_irq_restore(flag);

	if (ICC_CHAN_SIGNAL_OPEN & u32Signal)
	{
		up(&pChan->semOpen);
	}

	/* Ŀ�괦�����ر���ͨ�� */
	if(ICC_CHAN_SIGNAL_CLOSE & u32Signal)
	{
		if(ICC_CHAN_STATE_CLOSED == pChan->prxSubChanInfo->u32SubChanState)
		{
			if(pChan->event_cb)
			{
				if(!g_IccStMode)
				{
					handle = BSP_UDI_ICC_ChnToHandle(pChan->u32ChanID);
					(BSP_VOID)pChan->event_cb(handle, ICC_EVENT_CLOSE, 0);   /*lint !e732*/
				}
				else
				{
					(BSP_VOID)pChan->event_cb(pChan->u32ChanID, ICC_EVENT_CLOSE, 0);
				}
			}
		}

		/* close֮�󣬺����������ź� */
		u32Signal = 0;
		pChan->bWriteNeed = BSP_FALSE;
	}

	if (ICC_CHAN_SIGNAL_RESET & u32Signal)
	{
		/* ��λͨ���Ķ�дָ�룬ֻ��λ����ͨ�� */
		pChan->ptxSubChanInfo->pWrite = pChan->ptxSubChanInfo->pStart;
		pChan->ptxSubChanInfo->pRead = pChan->ptxSubChanInfo->pStart;

		/* ֪ͨ�ϲ� */
		if (pChan->event_cb)
		{
			if (!g_IccStMode)
			{
				handle = BSP_UDI_ICC_ChnToHandle(pChan->u32ChanID);
				(BSP_VOID)pChan->event_cb(handle, ICC_EVENT_RESET, 0);        /*lint !e732*/
			}
			else
			{
				(BSP_VOID)pChan->event_cb(pChan->u32ChanID, ICC_EVENT_RESET, 0);
			}
		}
	}

	if (ICC_CHAN_SIGNAL_DATA & u32Signal)
	{
		pChan->sDebug.u32RecvInt++;
		if (ICC_CHAN_MODE_STREAM == pChan->enMode)
		{
			/* streamģʽ */
			ICC_ReceiveStream(pChan);
		}
		else
		{
			/* packetģʽ��Ҫ�Ƚ��� */
			ICC_ReceivePacket(pChan);
		}
	}

	if (ICC_CHAN_SIGNAL_EMPTY & u32Signal)
	{
		pChan->sDebug.u32RecvEmpty++;
		if ((pChan->bWriteNeed) && (pChan->write_cb))
		{
			if (!g_IccStMode)
			{
				handle = BSP_UDI_ICC_ChnToHandle(pChan->u32ChanID);
				(BSP_VOID)pChan->write_cb(handle);    /*lint !e732*/
			}
			else
			{
				(BSP_VOID)pChan->write_cb(pChan->u32ChanID);
			}
			//            pChan->bWriteNeed = BSP_FALSE;
		}
	}

	return;
}


/*****************************************************************************
* �� �� ��  : ICC_HandleEvent
*
* ��������  : ICC��������
*
* �������  : 
*
* �������  : 
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_S32 ICC_task(void)
{
	ICC_CHAN_DEV_S *pChan = BSP_NULL;
	ICC_CHAN_DEV_S *pChanNext = BSP_NULL;
//	unsigned long flags;

	do {
		down(&g_semIccTask);

		for (pChan = g_pstOpenList; BSP_NULL != pChan ; pChan = pChanNext)
		{
			/* ����event�У�������Ϊ���ȼ�������������˳��ı� */
			pChanNext = pChan->next;

			if (pChan->prxSubChanInfo->u32Signal)
			{
				ICC_HandleEvent(pChan);
			}
		}

	} while(1);
}


static int icc_thread(void)
{
	ICC_CHAN_DEV_S *pChan = BSP_NULL;
	ICC_CHAN_DEV_S *pChanNext = BSP_NULL;
	unsigned long flags = 0;

	set_freezable();

	do {
		for (pChan = g_pstOpenList; BSP_NULL != pChan ; pChan = pChanNext)
		{
			/* ����event�У�������Ϊ���ȼ�������������˳��ı� */
			pChanNext = pChan->next;

			if (pChan->prxSubChanInfo->u32Signal)
			{
				ICC_HandleEvent(pChan);
			}
		}

        /*����������Ͳ��Ե�����-- linux�ں�*/
        /*lint -e516*/
 		wait_event(icc_wait, thread_flag);/*lint !e614 !e615*/
		local_irq_save(flags);
		thread_flag = 0;
		local_irq_restore(flags);
		/*lint +e516*/

	} while (1);

	return 0;
}






/*****************************************************************************
* �� �� ��  : ICC_HandleEvent
*
* ��������  : ICC���жϴ�����
*
* �������  : 
*
* �������  : 
*
* �� �� ֵ  : 
*****************************************************************************/
static irqreturn_t ICC_IntHandle(int data,void *ptr)
{
	/* ���ж�״̬ */

	/* ���ж� */
#if (defined (BOARD_SFT) && defined (VERSION_V7R1))||(defined (BOARD_ASIC_BIGPACK) && defined (VERSION_V3R2))
    /*V3R2 CS f00164371*/
    #if defined(CHIP_BB_6756CS)
	#else
        BSP_REG_CLRBITS(IO_ADDRESS(SYSCTRL_INT_ADDR), 0, ICC_SYSCTRL_INT_HOST, 1);   
	#endif
#elif ((defined (BOARD_ASIC) || defined (BOARD_SFT)) && (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)))
    BSP_REG_CLRBITS(IO_ADDRESS(SYSCTRL_INT_ADDR), 0, ICC_SYSCTRL_INT_HOST, 1);   
#endif

	/* ����ICC TASK */
	thread_flag = 1;
	wake_up(&icc_wait);
	return IRQ_HANDLED;
}


/*****************************************************************************
* �� �� ��  : BSP_ICC_Init
*
* ��������  : ICC��ʼ��
*
* �������  : ��
*
* �������  : ��
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_S32 BSP_ICC_Init(void)
{
	BSP_U32 cnt = 2000;
	int ret = 0;
	unsigned long flag = 0;

	if (BSP_TRUE == g_stIccDev.bInit)
	{
		return BSP_OK;
	}

    ICC_OmInit();

	/* ��ʼ������ͨ����ַ */
	g_pstIccCtrlChan = (ICC_CTRL_CHAN_S*)MEMORY_AXI_ICC_ADDR;
    printk("g_pstIccCtrlChan = 0x%x\n",g_pstIccCtrlChan);
    ICC_EventRecord(ICC_OM_EVENT_INIT, ICC_OM_CHAN_CTRL, (BSP_U32)g_pstIccCtrlChan, 0);

	/* �����������ź��� */
	sema_init(&g_semIccCtrl, SEM_FULL);
	/* �����������ź��� */
	sema_init(&g_semIccTask, SEM_EMPTY);
	/* �����������ź��� */
	sema_init(&g_semIccList, SEM_FULL);

	/* ������Դ�������ź��� */
	sema_init(&g_semIpcSpinLock, SEM_FULL);
    
    #if(FEATURE_ICC_DEBUG == FEATURE_ON)
    BSP_MNTN_IccErrlogInit();
    #endif

    if(BSP_OK != OSA_CreateTask("icc_thread", 
                                &icc_task_id, 
                                (osa_task_entry)icc_thread, 
                                ICC_TASK_PRIORITY, 
                                0x1000, 
                                BSP_NULL))
    {
		printk("create kthread icc_thread failed!\n");
		return BSP_ERROR;
    }

#if 0
	icc_task = kthread_run(icc_thread, NULL, "icc_thread");
	if (IS_ERR(icc_task))
	{
		printk("create kthread icc_thread failed!\n");
		return -1;
	}
#endif

    /* register multicore irq function and enable irq */
//		BSP_IPC_IntConnect(INT_VEC_ICC, (VOIDFUNCPTR)ICC_IntHandle, 0);
//		BSP_IPC_IntEnable(INT_VEC_ICC);
	ret = request_irq(INT_VEC_ICC, ICC_IntHandle, 0, "icc_irq", NULL);
	if (ret ) {
		printk(KERN_ERR "bsp_icc_init: Unable to register irq ret=%d.\n", ret);
		return BSP_ERROR;
	}

    
	local_irq_save(flag);
	ICC_SPIN_LOCK(&g_pstIccCtrlChan->u32SpinLock);

	if (!g_pstIccCtrlChan->bInit[ICC_TAGET])
	{
	    /*modified for lint e665 */
		memset(g_pstIccCtrlChan->astChanInfo, 0, (ICC_CHAN_NUM_MAX*sizeof(ICC_CHAN_S)));
	}
	else
	{
		g_pstIccCtrlChan->bConnect = BSP_TRUE;
	}

	g_pstIccCtrlChan->bInit[ICC_HOST] = BSP_TRUE;
	ICC_SPIN_UNLOCK(&g_pstIccCtrlChan->u32SpinLock);
	local_irq_restore(flag);

    ICC_EventRecord(ICC_OM_EVENT_CONNECTING, ICC_OM_CHAN_CTRL, 0, 0);

	while ((cnt--) && (!g_pstIccCtrlChan->bInit[ICC_TAGET]))
	{
		msleep(10);
	}

	if (g_pstIccCtrlChan->bInit[ICC_TAGET])
	{
		g_stIccDev.bInit = BSP_TRUE;
		printk("##### icc init ok!, cnt=%d, connet=%d\n", cnt, g_pstIccCtrlChan->bConnect);
        ICC_EventRecord(ICC_OM_EVENT_CONNECTED, ICC_OM_CHAN_CTRL, cnt, 0);
		return BSP_OK;
	}
	else
	{
    	g_pstIccCtrlChan->bInit[ICC_HOST] = BSP_FALSE;
        ICC_EventRecord(ICC_OM_EVENT_CONNECT_FAIL, ICC_OM_CHAN_CTRL, cnt, 0);
		return BSP_ERROR;
	}
 }


/*****************************************************************************
* �� �� ��  : BSP_ICC_Open
*
* ��������  : ��һ��ICCͨ��
*
* �������  : u32ChanId        ͨ��ID
*             FIFOInSize    ͨ��������FIFO��С
*             FIFOOutSize   ͨ�������FIFO��С
*             EventCb       �¼��ص�����
*
* �������  : ��
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_S32 BSP_ICC_Open(BSP_U32 u32ChanId, ICC_CHAN_ATTR_S *pChanAttr)
{
	BSP_S32 ret;
	ICC_CHAN_DEV_S *pChan = BSP_NULL;
	ICC_CHAN_S *pSmChan = BSP_NULL;
	unsigned long flag = 0;

    ICC_EventRecord(ICC_OM_EVENT_OPEN, u32ChanId, 0, 0);

	if (BSP_FALSE == g_stIccDev.bInit)
	{
		printk(KERN_ERR "BSP_MODU_ICC ICC is not inited!.\n");
        ICC_EventRecord(ICC_OM_EVENT_OPEN_FAIL, u32ChanId, g_stIccDev.bInit, __LINE__);
		return BSP_ERR_ICC_NOT_INIT;
	}

	if (BSP_FALSE == g_pstIccCtrlChan->bConnect)
	{
		printk(KERN_ERR "BSP_MODU_ICC ICC is not connect!.\n");
        ICC_EventRecord(ICC_OM_EVENT_OPEN_FAIL, u32ChanId, g_pstIccCtrlChan->bConnect, __LINE__);
		return BSP_ERR_ICC_NOT_CONNECT;
	}

	if (u32ChanId >= ICC_CHAN_NUM_MAX)
	{
		printk(KERN_ERR "BSP_MODU_ICC invalid channel[%d]!.\n",u32ChanId);
        ICC_EventRecord(ICC_OM_EVENT_OPEN_FAIL, u32ChanId, ICC_CHAN_NUM_MAX, __LINE__);
		return BSP_ERR_ICC_INVALID_CHAN;
	}

	if (BSP_NULL == pChanAttr)
	{
		printk(KERN_ERR "BSP_MODU_ICC invalid param[%d]!.\n",u32ChanId);
        ICC_EventRecord(ICC_OM_EVENT_OPEN_FAIL, u32ChanId, (BSP_U32)pChanAttr, __LINE__);
		return BSP_ERR_ICC_NULL;
	}

	if ( (pChanAttr->enChanMode >= ICC_CHAN_MODE_BUTT)
	||(pChanAttr->u32Priority > ICC_CHAN_PRIORITY_LOW)
	||(pChanAttr->u32FIFOInSize < ICC_CHAN_FIFO_MIN)
	||(pChanAttr->u32FIFOInSize > ICC_CHAN_FIFO_MAX)
	||(pChanAttr->u32FIFOInSize % 4)
	||(pChanAttr->u32FIFOOutSize < ICC_CHAN_FIFO_MIN)
	||(pChanAttr->u32FIFOOutSize > ICC_CHAN_FIFO_MAX)
	||(pChanAttr->u32FIFOOutSize % 4))
	{
		printk(KERN_ERR "BSP_MODU_ICC invalid para:[chan%d], mode=%d, priority=%d, iSize=%d, oSize=%d.\n",
		                    u32ChanId,
		                    pChanAttr->enChanMode,
		                    pChanAttr->u32Priority,
		                    pChanAttr->u32FIFOInSize,
		                    pChanAttr->u32FIFOOutSize);
        ICC_EventRecord(ICC_OM_EVENT_OPEN_FAIL, u32ChanId, pChanAttr->enChanMode, __LINE__);
        ICC_EventRecord(ICC_OM_EVENT_OPEN_FAIL, 
            pChanAttr->u32Priority, pChanAttr->u32FIFOInSize, pChanAttr->u32FIFOOutSize);
		return BSP_ERR_ICC_INVALID_PARAM;
	}
    
	down(&g_semIccCtrl);

	/* �ȼ���Ƿ��Ѿ��� */
	ICC_ListGet(g_pstOpenList, u32ChanId, &pChan);
	if (pChan)
	{
		up(&g_semIccCtrl);
		printk(KERN_ERR "BSP_MODU_ICC channel[%d] is openned!.\n",u32ChanId);
        ICC_EventRecord(ICC_OM_EVENT_OPEN_FAIL, u32ChanId, 0, __LINE__);
		return BSP_ERR_ICC_OPENED;
	}

	/* ����ͨ���ڵ� */
	pChan = (ICC_CHAN_DEV_S*)kmalloc(sizeof(ICC_CHAN_DEV_S), GFP_KERNEL);
	if (!pChan)
	{
		up(&g_semIccCtrl);
		printk(KERN_ERR "BSP_MODU_ICC channel[%d] alloc channel info failed.\n",u32ChanId);
        ICC_EventRecord(ICC_OM_EVENT_OPEN_FAIL, u32ChanId, 0, __LINE__);
		return BSP_ERR_ICC_NO_MEM;
	}

	sema_init(&pChan->semSync, SEM_FULL);
	sema_init(&pChan->semOpen, SEM_EMPTY);
	sema_init(&pChan->semRead, SEM_EMPTY);

	/* �Կ���ͨ������ */
	local_irq_save(flag);
	ICC_SPIN_LOCK(&g_pstIccCtrlChan->u32SpinLock);

	/* ���ͨ���Ƿ��Ѿ����� */
	pSmChan = &g_pstIccCtrlChan->astChanInfo[u32ChanId];
	//    printk("chan[%d], alloc? %d!\n", u32ChanId, pSmChan->bAlloc);
	if (BSP_FALSE == pSmChan->bAlloc)
	{
		/* ����Է���û������ͨ������������ͨ�� */
        ICC_EventRecord(ICC_OM_EVENT_OPEN_ALLOC, u32ChanId, 0, __LINE__);
		ret = ICC_AllocChannel(u32ChanId, pChanAttr);
		if (BSP_OK != ret)
		{
			kfree(pChan);
			pChan = BSP_NULL;
			ICC_SPIN_UNLOCK(&g_pstIccCtrlChan->u32SpinLock);
			local_irq_restore(flag);

			up(&g_semIccCtrl);
			printk(KERN_ERR "BSP_MODU_ICC channel[%d] alloc channel failed.\n",u32ChanId);
            ICC_EventRecord(ICC_OM_EVENT_OPEN_FAIL, u32ChanId, ret, __LINE__);
            ICC_EventRecord(ICC_OM_EVENT_OPEN_FAIL, 
                pChanAttr->u32FIFOInSize, pChanAttr->u32FIFOOutSize, 0);
			return ret;
		}
	}
	else
	{
		/* ����Է�ͨ���Ѿ����룬�Ƚ�ͨ�����ԣ�
		���In/Out FIFO��size����ȣ�����ͨ��ģ��/���ȼ�����ȣ����ش���*/
		if ( (pChanAttr->enChanMode != pSmChan->enMode)
		||(pChanAttr->u32Priority != pSmChan->u32Priority)
		||(pChanAttr->u32FIFOInSize != IO_TYPE(pSmChan->pSubChanInfo[ICC_TAGET])->u32FIFOsize)
		||(pChanAttr->u32FIFOOutSize !=IO_TYPE( pSmChan->pSubChanInfo[ICC_HOST])->u32FIFOsize))
		{
			kfree(pChan);
			pChan = BSP_NULL;
			ICC_SPIN_UNLOCK(&g_pstIccCtrlChan->u32SpinLock);
			local_irq_restore(flag);

			up(&g_semIccCtrl);
			printk(KERN_ERR "BSP_MODU_ICC channel attr is defferent! chan[%d] mode=%d priority=%d iSize=%d oSize=%d.\n",
			u32ChanId,
			pChanAttr->enChanMode,
			pChanAttr->u32Priority,
			pChanAttr->u32FIFOInSize,
			pChanAttr->u32FIFOOutSize);
            ICC_EventRecord(ICC_OM_EVENT_OPEN_FAIL, u32ChanId, pChanAttr->enChanMode, __LINE__);
            ICC_EventRecord(ICC_OM_EVENT_OPEN_FAIL, pChanAttr->u32Priority, 
                pChanAttr->u32FIFOInSize, pChanAttr->u32FIFOOutSize);
            ICC_EventRecord(ICC_OM_EVENT_OPEN_FAIL, pSmChan->u32Priority, 
                pSmChan->pSubChanInfo[ICC_TAGET]->u32FIFOsize, pSmChan->pSubChanInfo[ICC_HOST]->u32FIFOsize);
			return BSP_ERR_ICC_INVALID_PARAM;
		}

		if (ICC_CHAN_STATE_CLOSEING == pSmChan->enState)
		{
			kfree(pChan);
			pChan = BSP_NULL;
			ICC_SPIN_UNLOCK(&g_pstIccCtrlChan->u32SpinLock);
			local_irq_restore(flag);

			up(&g_semIccCtrl);
			printk(KERN_ERR "BSP_MODU_ICC channel[%d] is closing.\n", u32ChanId);
            ICC_EventRecord(ICC_OM_EVENT_OPEN_FAIL, u32ChanId, 0, __LINE__);
			return BSP_ERR_ICC_OPENED;
		}
	}

	/* ��ʼ��ͨ���ڵ� */
	pChan->u32ChanID = u32ChanId;
	pChan->u32Priority = pSmChan->u32Priority;
	pChan->enMode = pSmChan->enMode;
	pChan->pSpinLock = &pSmChan->u32SpinLock;
	pChan->bWriteNeed = BSP_FALSE;
	pChan->packet.data = BSP_NULL;
	pChan->packet.size = 0;
        
        {
            pChan->ptxSubChanInfo = IO_TYPE(pSmChan->pSubChanInfo[ICC_HOST]);
	    pChan->prxSubChanInfo = IO_TYPE(pSmChan->pSubChanInfo[ICC_TAGET]);
        }
	pChan->prxSubChanInfo->u32Signal = 0;
	pChan->ptxSubChanInfo->u32SubChanState = ICC_CHAN_STATE_OPENED;
    pChan->bFirstRecv = BSP_TRUE;
	pChan->event_cb = pChanAttr->event_cb;
	pChan->read_cb = pChanAttr->read_cb;
	pChan->write_cb = pChanAttr->write_cb;
	pChan->next = BSP_NULL;
	pChan->sDebug.u32SendTimes = 0;
	pChan->sDebug.u32SendBytes= 0;
	pChan->sDebug.u32RecvTimes= 0;
	pChan->sDebug.u32RecvBytes= 0;
	pChan->sDebug.u32RecvInt= 0;
	pChan->sDebug.u32LastHaveBytes = 0;
	pChan->sDebug.u32LastReadBytes = 0;
	pChan->sDebug.u32NoticeEmpty = 0;
	pChan->sDebug.u32RecvEmpty = 0;

	/* �ж�Ŀ�괦����ͨ����״̬ */
	if(ICC_CHAN_STATE_OPENED == pChan->prxSubChanInfo->u32SubChanState)
	{
		pSmChan->enState = ICC_CHAN_STATE_OPENED;

		/* �������� */
		(BSP_VOID)ICC_ListAdd(&g_pstOpenList, pChan);

		/* �ͷſ���ͨ�������� */
		ICC_SPIN_UNLOCK(&g_pstIccCtrlChan->u32SpinLock);
		local_irq_restore(flag);

		/* ֪ͨ�Է� */
		ICC_NotifyTarget(pChan, ICC_CHAN_SIGNAL_OPEN);

		up(&g_semIccCtrl);

        ICC_EventRecord(ICC_OM_EVENT_OPEN_OK, u32ChanId, 0, __LINE__);

		return BSP_OK;
	}
	else
	{
        ICC_EventRecord(ICC_OM_EVENT_OPEN_WAIT, u32ChanId, 0, __LINE__);
		/* �������� */
		(BSP_VOID)ICC_ListAdd(&g_pstOpenList, pChan);

		/* �ͷſ���ͨ�������� */
		ICC_SPIN_UNLOCK(&g_pstIccCtrlChan->u32SpinLock);
		local_irq_restore(flag);

		/* ֪ͨ�Է� */
		ICC_NotifyTarget(pChan, ICC_CHAN_SIGNAL_OPEN);

		/* �����ȴ� */
		down_timeout(&pChan->semOpen, msecs_to_jiffies(100000 * 10));

		if (ICC_CHAN_STATE_OPENED == pChan->prxSubChanInfo->u32SubChanState)
		{
			up(&g_semIccCtrl);
            ICC_EventRecord(ICC_OM_EVENT_OPEN_OK, u32ChanId, 0, __LINE__);
			return BSP_OK;
		}
		else
		{
			/* �������� */
			ICC_ListGetOut(&g_pstOpenList, pChan->u32ChanID, &pChan);
			ICC_FreeChannel(pChan->u32ChanID);

			up(&g_semIccCtrl);
		    printk(KERN_ERR "BSP_MODU_ICC ICC NEGOTIATE_FAIL. %d\n", u32ChanId);
            ICC_EventRecord(ICC_OM_EVENT_OPEN_FAIL, u32ChanId, 0, __LINE__);
			return BSP_ERR_ICC_NEGOTIATE_FAIL;
		}
	}
}


/*****************************************************************************
* �� �� ��  : BSP_ICC_Close
*
* ��������  : �ر�һ��ICCͨ��
*
* �������  : u32ChanId    ͨ��ID
*
* �������  : ��
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_S32 BSP_ICC_Close(BSP_U32 u32ChanId)
{
	ICC_CHAN_DEV_S  *pChan = BSP_NULL;
	ICC_CHAN_S *pSmChan = BSP_NULL;
	unsigned int flag = 0;

	printk(KERN_INFO "BSP_MODU_ICC BSP_ICC_Close enter chan[%d]!.\n",u32ChanId);

	if (BSP_FALSE == g_stIccDev.bInit)
	{
		printk(KERN_ERR "BSP_MODU_ICC ICC is not inited!.\n");
		return BSP_ERR_ICC_NOT_INIT;
	}

	if (BSP_FALSE == g_pstIccCtrlChan->bConnect)
	{
		printk(KERN_ERR "BSP_MODU_ICC ICC is not inited!.\n");
		return BSP_ERR_ICC_NOT_CONNECT;
	}

	if (u32ChanId >= ICC_CHAN_NUM_MAX)
	{
		printk(KERN_ERR "BSP_MODU_ICC invalid channel[%d]!.\n",u32ChanId);
		return BSP_ERR_ICC_INVALID_CHAN;
	}

	/* ��ȡͨ�� */
	ICC_ListGetOut(&g_pstOpenList, u32ChanId, &pChan);
	if (BSP_NULL == pChan)
	{
		printk(KERN_ERR "BSP_MODU_ICC channel[%d] is not open!.\n",u32ChanId);
		return BSP_ERR_ICC_NOT_OPEN;
	}

	down(&pChan->semSync);

	local_irq_save(flag);
	ICC_SPIN_LOCK(pChan->pSpinLock);
	pSmChan = &g_pstIccCtrlChan->astChanInfo[u32ChanId];
	if (ICC_CHAN_STATE_CLOSEING == pSmChan->enState)
	{
		/* ����Է��ѹرգ����ͷŹ����ڴ��е�ͨ�� */
		ICC_FreeChannel(u32ChanId);

		ICC_SPIN_UNLOCK(pChan->pSpinLock);
		local_irq_restore(flag);
	}
	else
	{
		/* ����״̬ */
		pChan->ptxSubChanInfo->u32SubChanState = ICC_CHAN_STATE_CLOSED;
		pSmChan->enState = ICC_CHAN_STATE_CLOSEING;
		ICC_SPIN_UNLOCK(pChan->pSpinLock);
		local_irq_restore(flag);

		/* ֪ͨ�Է� */
		ICC_NotifyTarget(pChan, ICC_CHAN_SIGNAL_CLOSE);
	}

	up(&pChan->semSync);

	kfree(pChan);
	pChan = BSP_NULL;


	return BSP_OK;
}


/*****************************************************************************
* �� �� ��  : BSP_ICC_Write
*
* ��������  : ��ICCͨ����д������
*
* �������  : u32ChanId    ͨ��ID
*             data      ����ָ��
*             size      ���ݴ�С
*
* �������  : ��
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_S32 BSP_ICC_Write(BSP_U32 u32ChanId, BSP_U8* pData, BSP_S32 s32Size)
{
	ICC_CHAN_DEV_S  *pChan = BSP_NULL;
	BSP_S32 s32CopySize;
    BSP_S32 s32ErrorNum=0;
    #if(FEATURE_ICC_DEBUG == FEATURE_ON)
    icc_mntn_info_t drv_mntn_icc_ncm_cmd = {0};
    #endif
	//printk(KERN_ERR "BSP_MODU_ICC BSP_ICC_Write enter chan[%d]!.\n",u32ChanId);

	/* δ��ʼ��������д������ */
	if (BSP_FALSE == g_stIccDev.bInit)
	{
        s32ErrorNum=BSP_ERR_ICC_NOT_INIT;
        goto errorExit;
	}

	if (BSP_FALSE == g_pstIccCtrlChan->bConnect)
	{
        s32ErrorNum=BSP_ERR_ICC_NOT_CONNECT;
        goto errorExit;
	}

	if (!pData)
	{
        s32ErrorNum=BSP_ERR_ICC_NULL;
        goto errorExit;
	}

	if (s32Size <= 0)
	{
        s32ErrorNum= BSP_ERR_ICC_INVALID_PARAM;
        goto errorExit;
	}

	if (u32ChanId >= ICC_CHAN_NUM_MAX)
	{
        s32ErrorNum= BSP_ERR_ICC_INVALID_CHAN;
        goto errorExit;
    }

	/* ��ȡͨ�� */
	ICC_ListGet(g_pstOpenList, u32ChanId, &pChan);
	if (BSP_NULL == pChan)
	{
        s32ErrorNum= BSP_ERR_ICC_NOT_OPEN;
        goto errorExit;
	}

	if (ICC_CHAN_MODE_PACKET == pChan->enMode)
	{
		s32CopySize = ICC_SendPacket(pChan, pData, s32Size);
        ICC_EventRecord(ICC_OM_EVENT_WRITE_PACKET, u32ChanId, s32Size, s32CopySize);
	}
	else
	{
		s32CopySize = ICC_SendStream(pChan, pData, s32Size);
        ICC_EventRecord(ICC_OM_EVENT_WRITE_STREAM, u32ChanId, s32Size, s32CopySize);
	}

    if(s32Size == s32CopySize)
    {
    	return s32CopySize;
    }
    else
    {
        s32ErrorNum=s32CopySize;
        goto errorExit;
    }
    errorExit:

    ICC_EventRecord(ICC_OM_EVENT_WRITE_FAIL, u32ChanId, s32Size, s32ErrorNum);
    ICC_EventRecord(ICC_OM_EVENT_WRITE_FAIL, g_stIccDev.bInit, g_pstIccCtrlChan->bConnect, s32CopySize);
#if(FEATURE_ICC_DEBUG == FEATURE_ON)
    drv_mntn_icc_ncm_cmd.u32ChanID=u32ChanId;
    drv_mntn_icc_ncm_cmd.s32Errlog=s32ErrorNum;
    drv_mntn_icc_ncm_cmd.s32Datesize=s32Size;
    ICC_DEBUG_printk("Write  errorExit::u32ChanId:%d,s32ErrorNum:%d,s32Size:%d \n",u32ChanId,s32ErrorNum,s32Size);
    BSP_ICC_Errlog_Save( DRV_MODULE_ID_ICC,DRV_PRIM_ICC_ERRLOG,&drv_mntn_icc_ncm_cmd);
#endif    

    return s32ErrorNum;
}


/*****************************************************************************
* �� �� ��  : BSP_ICC_Read
*
* ��������  : ��������
*
* �������  : u32ChanId    ͨ��ID
*             cmd       ��������
*             param     ��������Ĳ���
*
* �������  : ��
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_S32 BSP_ICC_Read(BSP_U32 u32ChanId, BSP_U8* pData, BSP_S32 s32Size)
{
	ICC_CHAN_DEV_S  *pChan = BSP_NULL;
	BSP_S32 s32CopySize;
    BSP_S32 s32ErrorNum=0;
    #if(FEATURE_ICC_DEBUG == FEATURE_ON)
    icc_mntn_info_t drv_mntn_icc_ncm_cmd = {0};
    #endif
	//printk(KERN_ERR "BSP_MODU_ICC BSP_ICC_Read enter chan[%d]!.\n",u32ChanId);

	/* δ��ʼ��������д������ */
	if (BSP_FALSE == g_stIccDev.bInit)
	{
        s32ErrorNum=BSP_ERR_ICC_NOT_INIT;
        goto errorExit;	
    }

	if (BSP_FALSE == g_pstIccCtrlChan->bConnect)
	{
        s32ErrorNum=BSP_ERR_ICC_NOT_CONNECT;
        goto errorExit;	
	}

	if (!pData)
	{
        s32ErrorNum=BSP_ERR_ICC_NULL;
        goto errorExit;	
	}

	if (u32ChanId >= ICC_CHAN_NUM_MAX)
	{
        s32ErrorNum=BSP_ERR_ICC_INVALID_CHAN;
        goto errorExit;	
    }

	/* ��ȡͨ�� */
	ICC_ListGet(g_pstOpenList, u32ChanId, &pChan);
	if (BSP_NULL == pChan)
	{
        s32ErrorNum=BSP_ERR_ICC_NOT_OPEN;
        goto errorExit;	
	}

	if (ICC_CHAN_MODE_PACKET == pChan->enMode)
	{
		s32CopySize = ICC_ReadPacket(pChan, pData, s32Size);
        ICC_EventRecord(ICC_OM_EVENT_READ_PACKET, u32ChanId, s32Size, s32CopySize);
	}
	else
	{
		s32CopySize = ICC_ReadStream(pChan, pData, s32Size);
        ICC_EventRecord(ICC_OM_EVENT_READ_STREAM, u32ChanId, s32Size, s32CopySize);
	}
    
    if(s32Size == s32CopySize)
    {
    	return s32CopySize;
    }
    else
    {
        s32ErrorNum=s32CopySize;
        goto errorExit;
    }
    errorExit:

    ICC_EventRecord(ICC_OM_EVENT_READ_FAIL, u32ChanId, s32Size, s32ErrorNum);
    ICC_EventRecord(ICC_OM_EVENT_READ_FAIL, g_stIccDev.bInit, g_pstIccCtrlChan->bConnect, s32CopySize);

#if(FEATURE_ICC_DEBUG == FEATURE_ON)
    drv_mntn_icc_ncm_cmd.u32ChanID=u32ChanId;
    drv_mntn_icc_ncm_cmd.s32Errlog=s32ErrorNum;
    drv_mntn_icc_ncm_cmd.s32Datesize=s32Size;
    BSP_ICC_Errlog_Save( DRV_MODULE_ID_ICC,DRV_PRIM_ICC_ERRLOG,&drv_mntn_icc_ncm_cmd);
    ICC_DEBUG_printk("Read  errorExit::u32ChanId:%d,s32ErrorNum:%d,s32Size:%d \n",u32ChanId,s32ErrorNum,s32Size);
#endif

    return s32ErrorNum;
}


/*****************************************************************************
* �� �� ��  : BSP_ICC_Ioctl
*
* ��������  : ��������
*
* �������  : u32ChanId    ͨ��ID
*             cmd       ��������
*             param     ��������Ĳ���
*
* �������  : ��
*
* �� �� ֵ  : 
*****************************************************************************/
BSP_S32 BSP_ICC_Ioctl(BSP_U32 u32ChanId, BSP_U32 cmd, BSP_VOID *param)
{
	BSP_S32 ret;

	printk(KERN_ERR "BSP_MODU_ICC BSP_ICC_Ioctl enter chan[%d]!.\n",u32ChanId);

	if (BSP_FALSE == g_stIccDev.bInit)
	{
		printk(KERN_ERR "BSP_MODU_ICC ICC is not inited!.\n");
		return BSP_ERR_ICC_NOT_INIT;
	}

	if (BSP_FALSE == g_pstIccCtrlChan->bConnect)
	{
		printk(KERN_ERR "BSP_MODU_ICC ICC is not inited!.\n");
		return BSP_ERR_ICC_NOT_CONNECT;
	}

	if (u32ChanId >= ICC_CHAN_NUM_MAX)
	{
		printk(KERN_ERR "BSP_MODU_ICC invalid channel[%d]!.\n",u32ChanId);
		return BSP_ERR_ICC_INVALID_CHAN;
	}

	switch(cmd)
	{
		case ICC_IOCTL_SET_EVENT_CB:
			{
				ret = ICC_SetEventCB(u32ChanId, (icc_event_cb)param);
				break;
			}
		case ICC_IOCTL_SET_READ_CB:
			{
				ret = ICC_SetReadCB(u32ChanId, (icc_read_cb)param);
				break;
			}
		case ICC_IOCTL_SET_WRITE_CB:
			{
				ret = ICC_SetWriteCB(u32ChanId, (icc_write_cb)param);
				break;
			}
		case ICC_IOCTL_GET_STATE:
			{
				ret = ICC_GetChanState(u32ChanId, (ICC_CHAN_STATE_E*)param);
				break;
			}
		default:
			{
				printk(KERN_ERR "BSP_MODU_ICC channel[%d] invalid cmd[%d].\n",u32ChanId,cmd);
				ret = BSP_ERR_ICC_INVALID_PARAM;
				break;
			}
	}

	return ret;
}


BSP_U32 BSP_ICC_CanSleep(BSP_U32 u32Flag)
{
    int i;
    ICC_CHAN_DEV_S *pChan;
	
    for(i=0; i<32; i++)
    {
        ICC_ListGet_Noblock(g_pstOpenList, (BSP_U32)i, &pChan);
        if(pChan)
        {
        	if((pChan->ptxSubChanInfo->pWrite!=pChan->ptxSubChanInfo->pRead) || (pChan->prxSubChanInfo->pWrite!=pChan->prxSubChanInfo->pRead))
        	{
        		if(u32Flag != 0)
        		{
        			printk("chan ID is %d ,pWrite = 0x%x, pRead = 0x%x\n",pChan->u32ChanID,*(pChan->ptxSubChanInfo->pWrite),*(pChan->ptxSubChanInfo->pRead));
        		}
        		return BSP_ERR_ICC_INVALID_CHAN;
        	}
        }

    }
    
    return BSP_OK;
}
#ifdef __cplusplus
}
#endif


