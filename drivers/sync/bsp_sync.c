/*************************************************************************
*   ��Ȩ����(C) 1987-2010, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_SYNC.c
*
*   ��    �� :  wangxuesong
*
*   ��    �� :  ���ļ�����IFCģ��ʵ�֣����ڹ���ģ�����Ӻ�ͬ��������
*
*   �޸ļ�¼ :  2010��4��2��  v1.00  wangxuesong  ����
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
#include <linux/time.h>
#include <asm/io.h>
#include <linux/sched.h>
#include <mach/hardware.h>

#include <mach/spinLockAmp.h>
#include <mach/common/mem/bsp_mem.h>
#include <mach/balong_v100r001.h>
#include "BSP.h"

#include "bsp_sync_drv.h"



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

#define SYNC_SPINLOCK_ADDR  (MEMORY_RAM_CORESHARE_SYNC_ADDR)
#define SYNC_LOCK_ADDR      (SYNC_SPINLOCK_ADDR + 32)
#define SYNC_STATE_ADDR     (SYNC_LOCK_ADDR + 32)

BSP_U32 g_semSync = 0;
BSP_CHAR *g_pSyncLock = (BSP_CHAR*)SYNC_LOCK_ADDR;
BSP_CHAR *g_pSyncState = (BSP_CHAR*)SYNC_STATE_ADDR;

//extern BSP_VOID spinLockAmpTake(BSP_U32 *pSpinLock);
//extern BSP_VOID spinLockAmpGive(BSP_U32 *pSpinLock);

BSP_S32 SyncSpinLock(void)
{
    unsigned long flag = 0;

    local_irq_save(flag);
#if defined (VERSION_V3R2) && defined (BOARD_SFT)
    BSP_IPC_SpinLock(IPC_SEM_SYNC);
#else	
    spinLockAmpTake((BSP_U32 *)SYNC_SPINLOCK_ADDR);
#endif

    return (BSP_S32)flag;
}

BSP_VOID SyncSpinUnlock(BSP_S32 flag)
{
#if defined (VERSION_V3R2) && defined (BOARD_SFT)
    BSP_IPC_SpinUnLock(IPC_SEM_SYNC);
#else
    spinLockAmpGive((BSP_U32 *)SYNC_SPINLOCK_ADDR);
#endif
    /*����������Ͳ��Ե�����-- linux�ں�*/
    /*lint -e516*/
    local_irq_restore((unsigned long)flag);
    /*lint +e516*/

    return;
}


/*****************************************************************************
* �� �� �� : BSP_SYNC_Lock
*
* ��������: ��������ѯģ��ĳ�ʼ��״̬
*
* �������: u32Module     ģ��
*               pState        ״ָ̬��
*               u32TimeOut    ��ʱֵ��0Ϊ��Զ�ȴ���
*
* �������: ��
*
* �� �� ֵ : OK&ERROR
*
* ����˵��: ��
*
*****************************************************************************/
BSP_S32 BSP_SYNC_Lock(SYNC_MODULE_E u32Module, BSP_U32 *pState, BSP_U32 u32TimeOut)
{
    BSP_S32 flag;
    struct timeval tmStart;
    struct timeval tmCur;
    struct timeval tmEnd;
    

    if(!pState || (u32Module >= SYNC_MODULE_BUTT))
    {
        printk("BSP_SYNC_Lock, invalid param, module=0x%x, pState=0x%x\n", u32Module, (BSP_U32)pState);
        return BSP_ERROR;
    }

    do_gettimeofday(&tmStart);
    tmEnd.tv_sec = tmStart.tv_sec + (BSP_S32)u32TimeOut/1000;
    tmEnd.tv_usec = tmStart.tv_usec + ((BSP_S32)u32TimeOut%1000)*1000;
    
    /* Ŀǰֻ����usrAppInit��ʹ�ã�����������£�ʹ��ѭ����ѯЧ�ʽϸ�
       �����Ҫ֧�ֶ�������Ҫ�ʵ����ó�CPU */
    for(;;)
    {
        if(!g_pSyncLock[u32Module])
        {
            flag = SyncSpinLock();
            if(!g_pSyncLock[u32Module])
            {
                g_pSyncLock[u32Module] = 1;
                SyncSpinUnlock(flag);
                break;
            }
            else
            {
                SyncSpinUnlock(flag);
            }
        }

        if(u32TimeOut)
        {
            do_gettimeofday(&tmCur);
            if((tmEnd.tv_sec < tmCur.tv_sec) || ((tmEnd.tv_sec == tmCur.tv_sec) && (tmEnd.tv_usec < tmCur.tv_usec)))
            {
                printk("BSP_SYNC_Lock, timeout2, module=0x%x, u32TimeOut=0x%x\n", u32Module, u32TimeOut);                
                return BSP_ERR_SYNC_TIMEOUT;
            }
        }

        msleep(1);
    }

    *pState = (BSP_U32)g_pSyncState[u32Module];/*lint !e571*/

    return BSP_OK;
}


/*****************************************************************************
* �� �� ��  : BSP_SYNC_UnLock
*
* ��������  : ����������ģ���ʼ��״̬
*
* �������: u32Module     ģ��
*               u32State       ����ģ��״̬
*
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* ����˵��  : ��
*
*****************************************************************************/
BSP_S32 BSP_SYNC_UnLock(SYNC_MODULE_E u32Module, BSP_U32 u32State)
{
    if(u32Module >= SYNC_MODULE_BUTT)
    {
        printk("BSP_SYNC_UnLock, invalid param, module=0x%x\n", u32Module);
#if 0   /* BEGIN: Modified by z67193, 2011-11-14 */
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_SYNC, 
            "BSP_SYNC_UnLock, invalid param, module=0x%x\n", u32Module);
#endif  /* END:   Modified by z67193, 2011-11-14 */
        return BSP_ERROR;
    }

    g_pSyncState[u32Module] = (BSP_CHAR)u32State;
    g_pSyncLock[u32Module] = 0;

    return BSP_OK;
}


/*****************************************************************************
* �� �� ��  : BSP_SYNC_Wait
*
* ��������  : �Ӻ˵ȴ����˲�����ɺ���
*
* �������: u32Module     ģ��
*               u32TimeOut    ��ʱֵ��0Ϊ��Զ�ȴ���
*
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* ����˵��  : ��
*
*****************************************************************************/
BSP_S32 BSP_SYNC_Wait(SYNC_MODULE_E u32Module, BSP_U32 u32TimeOut)
{
    struct timeval tmStart;
    struct timeval tmCur;
    struct timeval tmEnd;

    if(u32Module >= SYNC_MODULE_BUTT)
    {
        printk("BSP_SYNC_UnLock, invalid param, module=0x%x\n", u32Module);
#if 0   /* BEGIN: Modified by z67193, 2011-11-14 */
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_SYNC, 
            "BSP_SYNC_UnLock, invalid param, module=0x%x\n", u32Module);
#endif  /* END:   Modified by z67193, 2011-11-14 */
        return BSP_ERROR;
    }

    do_gettimeofday(&tmStart);
    tmEnd.tv_sec = tmStart.tv_sec + (BSP_S32)u32TimeOut/1000;
    tmEnd.tv_usec = tmStart.tv_usec + ((BSP_S32)u32TimeOut%1000)*1000;

    for(;;)
    {
        if(g_pSyncState[u32Module])
        {
            return BSP_OK;
        }

        if(u32TimeOut)
        {
            do_gettimeofday(&tmCur);
            if((tmEnd.tv_sec < tmCur.tv_sec) || ((tmEnd.tv_sec == tmCur.tv_sec) && (tmEnd.tv_usec < tmCur.tv_usec)))
            {
                printk("BSP_SYNC_Wait, timeout, module=0x%x, u32TimeOut=0x%x\n", u32Module, u32TimeOut);
#if 0   /* BEGIN: Modified by z67193, 2011-11-14 */
                BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_SYNC, 
                    "BSP_SYNC_Wait, timeout, module=0x%x, u32TimeOut=0x%x\n", u32Module, u32TimeOut);
#endif  /* END:   Modified by z67193, 2011-11-14 */
                
                return BSP_ERR_SYNC_TIMEOUT;
            }
        }

        msleep(1);
    }
}


/*****************************************************************************
* �� �� ��  : BSP_SYNC_Give
*
* ��������  : ����֪ͨ�Ӻ����˲����Ѿ����
*
* �������  : 
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* ����˵��  : ��
*
*****************************************************************************/
BSP_S32 BSP_SYNC_Give(SYNC_MODULE_E u32Module)
{
    if(u32Module >= SYNC_MODULE_BUTT)
    {
        printk("BSP_SYNC_Give, invalid param, module=0x%x\n", u32Module);
#if 0   /* BEGIN: Modified by z67193, 2011-11-14 */
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_SYNC, 
            "BSP_SYNC_Give, invalid param, module=0x%x\n", u32Module);
#endif  /* END:   Modified by z67193, 2011-11-14 */
        return BSP_ERROR;
    }

    g_pSyncState[u32Module] = 1;
    
    return BSP_OK;
}
EXPORT_SYMBOL(BSP_SYNC_Lock);
EXPORT_SYMBOL(BSP_SYNC_UnLock);
EXPORT_SYMBOL(BSP_SYNC_Wait);
EXPORT_SYMBOL(BSP_SYNC_Give);
