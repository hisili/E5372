/*************************************************************************
*   版权所有(C) 1987-2010, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_ICC_OM.c
*
*   作    者 :  zhouluojun
*
*   描    述 :  本文件主要完成ICC模块的可谓可测功能.
*
*   修改记录 :  2012年5月12日  v1.00  zhouluojun  创建
*************************************************************************/

#include <linux/kernel.h>
#include <linux/semaphore.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/freezer.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <asm/io.h>
#include <mach/hardware.h>
#include <mach/common/mem/bsp_mem.h>
#include <mach/balong_v100r001.h>
#include "product_config.h"
#include "BSP.h"
#include "bsp_icc_drv.h"
#include "bsp_icc_om.h"


#ifdef __cplusplus
extern "C" {
#endif

extern ICC_CHAN_DEV_S           *g_pstOpenList;
ICC_OM_CTRL_S                   *g_stIccOmCtrl;

static char* g_IccRecordEvent[]=
{
    "ICC_EVENT_INIT",
    "ICC_EVENT_CONNECTING",
    "ICC_EVENT_CONNECTED",
    "ICC_EVENT_CONNECT_FAIL",
    "ICC_EVENT_OPEN",
    "ICC_EVENT_OPEN_OK",
    "ICC_EVENT_OPEN_FAIL",
    "ICC_EVENT_OPEN_ALLOC",
    "ICC_EVENT_OPEN_WAIT",
    "ICC_EVENT_WRITE_STREAM",
    "ICC_EVENT_WRITE_PACKET",
    "ICC_EVENT_WRITE_FAIL",
    "ICC_EVENT_READ_STREAM",
    "ICC_EVENT_READ_PACKET",
    "ICC_EVENT_READ_FAIL",
};

BSP_VOID ICC_OmInit(BSP_VOID)
{
    g_stIccOmCtrl = (ICC_OM_CTRL_S*)(MEMORY_AXI_ICC_OM_ADDR + MEMORY_AXI_ICC_OM_SIZE/2);
    g_stIccOmCtrl->IccOmAddrBase = (BSP_U32)g_stIccOmCtrl + sizeof(ICC_OM_CTRL_S);
    g_stIccOmCtrl->IccOmAddrSize = MEMORY_AXI_ICC_OM_SIZE/2 - sizeof(ICC_OM_CTRL_S);
    g_stIccOmCtrl->IccOmAddrPos = g_stIccOmCtrl->IccOmAddrBase;

#ifdef ICC_OM_DEBUG
    g_stIccOmCtrl->IccOmEnbale = BSP_TRUE;
#else
    g_stIccOmCtrl->IccOmEnbale = BSP_FALSE;
#endif
}


BSP_VOID ICC_EventRecord(ICC_EVENT_E event, BSP_U32 ChanId, BSP_U32 param1, BSP_U32 param2)
{
    ICC_EVENT_S *pEvent = (ICC_EVENT_S *)g_stIccOmCtrl->IccOmAddrPos;

    if(BSP_TRUE == g_stIccOmCtrl->IccOmEnbale)
    {
        if(g_stIccOmCtrl->IccOmAddrPos + sizeof(ICC_EVENT_S) 
            <= g_stIccOmCtrl->IccOmAddrBase + g_stIccOmCtrl->IccOmAddrSize)
        {
            pEvent->event  = (BSP_U16)event;
            pEvent->chan   = (BSP_U16)ChanId;
            pEvent->param1 = param1;
            pEvent->param2 = param2;
            pEvent->TimeStamp = BSP_GetSliceValue();
            g_stIccOmCtrl->IccOmAddrPos += sizeof(ICC_EVENT_S);
        }
    }

    return;
}

#define  ICC_PrintEvent(OmEvent) \
    printk("%#8xms: CHAN=%d \tPARA1=0x%x \tPARA1=0x%x \tEVENT: %s\n", \
        (0xffffffff - ((ICC_EVENT_S*)OmEvent)->TimeStamp)/32, \
        ((ICC_EVENT_S*)OmEvent)->chan,\
        ((ICC_EVENT_S*)OmEvent)->param1,\
        ((ICC_EVENT_S*)OmEvent)->param2,\
        g_IccRecordEvent[((ICC_EVENT_S*)OmEvent)->event])

BSP_VOID ICC_ShowEvent(BSP_U32 core, BSP_U32 ChanId)
{
    BSP_CHAR *base;
    BSP_CHAR *cur;
    ICC_OM_CTRL_S *pOmCtrl;
    ICC_OM_CTRL_S sOmCtrl;

    if(0 == core)
    {
        pOmCtrl = (ICC_OM_CTRL_S*)(MEMORY_AXI_ICC_OM_ADDR);
        sOmCtrl.IccOmAddrBase = g_stIccOmCtrl->IccOmAddrBase - (MEMORY_AXI_ICC_OM_SIZE)/2;
        sOmCtrl.IccOmAddrPos = sOmCtrl.IccOmAddrBase + (pOmCtrl->IccOmAddrPos - pOmCtrl->IccOmAddrBase);
        sOmCtrl.IccOmAddrSize = pOmCtrl->IccOmAddrSize;
        pOmCtrl = &sOmCtrl;
    }
    else
    {
        pOmCtrl = g_stIccOmCtrl;
    }
    base = (BSP_CHAR*)pOmCtrl->IccOmAddrBase;
    cur = base;

    while(cur < (BSP_CHAR*)pOmCtrl->IccOmAddrPos)
    {
        if((ICC_OM_CHAN_ALL == ChanId) || (ChanId == ((ICC_EVENT_S*)cur)->chan))
        {
            ICC_PrintEvent(cur);
        }
        cur += sizeof(ICC_EVENT_S);
    }

    return;
}


BSP_S32 ICC_ShowChan(BSP_U32 u32ChanId)
{
    ICC_CHAN_DEV_S *pChan;

    ICC_ListGet(g_pstOpenList, u32ChanId, &pChan);
    if(pChan)
    {
        printk("================== CHANNEL %d ==================\n", u32ChanId);
        printk("MODE:\t\t%d\n", pChan->enMode);
        printk("Priority:\t%d\n", pChan->u32Priority);
        printk("bWriteNeed:\t%d\n", pChan->bWriteNeed);
        printk("SpinLock:\t%d\n", *pChan->pSpinLock);
        printk("Packet:\t\t0x%x, 0x%x\n", (BSP_U32)pChan->packet.data, pChan->packet.size);
        printk("TX state:\t0x%x\n", pChan->ptxSubChanInfo->u32SubChanState);
        printk("TX signal:\t0x%x\n", pChan->ptxSubChanInfo->u32Signal);
        printk("TX buffer:\tS=0x%x,E=0x%x,W=0x%x,R=0x%x\n", 
            (BSP_U32)pChan->ptxSubChanInfo->pStart, (BSP_U32)pChan->ptxSubChanInfo->pEnd, 
            (BSP_U32)pChan->ptxSubChanInfo->pWrite, (BSP_U32)pChan->ptxSubChanInfo->pRead);
        printk("TX times:\tWt=0x%x Bytes=0x%x EM=0x%x\n", pChan->sDebug.u32SendTimes, pChan->sDebug.u32SendBytes, pChan->sDebug.u32NoticeEmpty);
        printk("RX state:\t0x%x\n", pChan->prxSubChanInfo->u32SubChanState);
        printk("RX signal:\t0x%x\n", pChan->prxSubChanInfo->u32Signal);
        printk("RX buffer:\tS=0x%x,E=0x%x,W=0x%x,R=0x%x\n", 
            (BSP_U32)pChan->prxSubChanInfo->pStart, (BSP_U32)pChan->prxSubChanInfo->pEnd, 
            (BSP_U32)pChan->prxSubChanInfo->pWrite, (BSP_U32)pChan->prxSubChanInfo->pRead);
        printk("RX times:\tInt=0x%x Read=0x%x Bytes=0x%x EM=0x%x\n", pChan->sDebug.u32RecvInt, 
            pChan->sDebug.u32RecvTimes, pChan->sDebug.u32RecvBytes, pChan->sDebug.u32RecvEmpty);
        printk("RX last:\tRecv=0x%x Read=0x%x\n", pChan->sDebug.u32LastHaveBytes, pChan->sDebug.u32LastReadBytes);
    }

    return 0;
}


BSP_S32 ICC_ShowChanAll()
{
    int i;
    ICC_CHAN_DEV_S *pChan;

    for(i=0; i<32; i++)
    {
        ICC_ListGet(g_pstOpenList, (BSP_U32)i, &pChan);
        if(pChan)
        {
            printk("================== CHANNEL %d ==================\n", i);
            printk("MODE:\t\t%d\n", pChan->enMode);
            printk("Priority:\t%d\n", pChan->u32Priority);
            printk("bWriteNeed:\t%d\n", pChan->bWriteNeed);
            printk("SpinLock:\t%d\n", *pChan->pSpinLock);
            printk("Packet:\t\t0x%x, 0x%x\n", (BSP_U32)pChan->packet.data, pChan->packet.size);
            printk("TX state:\t0x%x\n", pChan->ptxSubChanInfo->u32SubChanState);
            printk("TX signal:\t0x%x\n", pChan->ptxSubChanInfo->u32Signal);
            printk("TX buffer:\tS=0x%x,E=0x%x,W=0x%x,R=0x%x\n", 
                (BSP_U32)pChan->ptxSubChanInfo->pStart, (BSP_U32)pChan->ptxSubChanInfo->pEnd, 
                (BSP_U32)pChan->ptxSubChanInfo->pWrite, (BSP_U32)pChan->ptxSubChanInfo->pRead);
            printk("TX times:\tWt=0x%x Bytes=0x%x EM=0x%x\n", pChan->sDebug.u32SendTimes, pChan->sDebug.u32SendBytes, pChan->sDebug.u32NoticeEmpty);
            printk("RX state:\t0x%x\n", pChan->prxSubChanInfo->u32SubChanState);
            printk("RX signal:\t0x%x\n", pChan->prxSubChanInfo->u32Signal);
            printk("RX buffer:\tS=0x%x,E=0x%x,W=0x%x,R=0x%x\n", 
                (BSP_U32)pChan->prxSubChanInfo->pStart, (BSP_U32)pChan->prxSubChanInfo->pEnd, 
                (BSP_U32)pChan->prxSubChanInfo->pWrite, (BSP_U32)pChan->prxSubChanInfo->pRead);
            printk("RX times:\tInt=0x%x Read=0x%x Bytes=0x%x EM=0x%x\n", pChan->sDebug.u32RecvInt, 
                pChan->sDebug.u32RecvTimes, pChan->sDebug.u32RecvBytes, pChan->sDebug.u32RecvEmpty);
            printk("RX last:\tRecv=0x%x Read=0x%x\n", pChan->sDebug.u32LastHaveBytes, pChan->sDebug.u32LastReadBytes);
        }
    }

    return 0;
}



#ifdef __cplusplus
}
#endif




