/*************************************************************************
*   版权所有(C) 1987-2010, 深圳华为技术有限公司.
*
*   文 件 名 :  dmaDrv.c
*
*   作    者 :  wuzhen
*
*   描    述 :   本文件主要完成对DMA传输方式的支持，支持LTE项目中的内存和内存
                 以及内存和外设中的DMA数据传输，主要应用场景为内存到内存的数据搬移，根据V400单板器件手册写作，
                 共16通道可用
*
*   修改记录 :  2008年12月26日  v1.00  wuzhen  创建
*
*
*************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __VXWORKS__
#include <vxWorks.h>
#include "config.h"
#include <cacheLib.h>
#include <stdlib.h>
#include <intLib.h>
#include <logLib.h>
#include <stdio.h>
#include <string.h>
#include <taskLib.h>
#include <memLib.h>
#include "arm_pbxa9.h"
#include "BSP_VERSION.h"
#include "BSP_GLOBAL.h"
#include "BSP_DRV_DMAC.h"
#include "BSP.h"
#include "BSP_SYNC_DRV.h"

#define DMAC_MALLOC_OS(size)        memalign(DMA_MEM_ALIGN, size);
#define DMAC_FREE_OS(ptr)           free(ptr)
#define DMAC_INT_LOCK(flag) \
do{\
    flag = intLock();\
}while(0)
#define DMAC_INT_UNLOCK(flag)        intUnlock(flag)
#define DMAC_VIRT_TO_PHYS(a)         (a)
#ifdef BSP_MMU_OPEN
#define DMAC_CACHE_FLUSH(a, s)      (BSP_VOID)cacheFlush(DATA_CACHE, (BSP_VOID *)(a), s)
#else
#define DMAC_CACHE_FLUSH(a, s)      
#endif
#else
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <asm/system.h>
#include <linux/interrupt.h>
#include <linux/string.h>
#include <linux/dma-mapping.h>
#include <asm/cacheflush.h>
#include "BSP.h"

#include "BSP_DRV_DMAC.h"

#define DMAC_MALLOC_OS(size)        kmalloc(size, GFP_KERNEL)
#define DMAC_FREE_OS(ptr)           kfree(ptr)
#define DMAC_INT_LOCK(flag)         local_irq_save(flag)
#define DMAC_INT_UNLOCK(flag)       local_irq_restore(flag)
#define DMAC_VIRT_TO_PHYS(a)        virt_to_phys((void*)(a))
#define DMAC_CACHE_FLUSH(a, s)      __dma_single_cpu_to_dev((const void *)(a), s, DMA_TO_DEVICE)
/*Info 760: (Info -- Redundant macro 'DMA_REGBASE_ADR' defined identically at
  line 75, file D:\personal\sjz\PC-lint\LintCode\android-2.6.35\arch\arm\mach-balong
  \include\mach\balong_v7r1asic.h)*/
#ifndef DMA_REGBASE_ADR  
#define DMA_REGBASE_ADR             IO_ADDRESS(0x900AA000)
#endif/*DMA_REGBASE_ADR*/
#endif
#define BSP_TRACE(enLogLevel, enLogModule, fmt, s32Arg1, s32Arg2, \
               s32Arg3, s32Arg4, s32Arg5, s32Arg6)
			   

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

/* 底层使用的建立传输链表的乒乓地址空间指针 for ps 通道0*/
/* 剩余3个用作普通通道*/
BSP_U32 *g_pu32PhyAddrPointer[DMA_MAX_CHANNELS] = {0};

BSP_U32 g_u32DmaInit = DMA_NOTINITIALIZE;

/* DMA通道相关信息*/
DMA_CHN_INFO_S g_stChannelInfo[DMA_MAX_CHANNELS];

/*****************************************************************************
* 函 数 名  : DMA_CheckChannelBusy
*
* 功能描述  : 判断通道是否空闲
*
* 输入参数  : u32Channel：待判断的通道号
* 输出参数  : 无
* 返 回 值  : DMAC_SUCCESS       通道空闲
*             BSP_ERR_DMA_CHANNEL_BUSY  通道忙
*             BSP_ERR_DMA_INVALID_PARA  参数错误
*
* 修改记录  :2009年06月20日   吴振  创建

*****************************************************************************/
BSP_S32 DMA_CheckChannelBusy(BSP_U32 u32Channel)
{
    BSP_U32 u32Result = 0;

    /* 参数检查*/
    if(DMA_MAX_Index <= u32Channel)
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
        "\r DMA_CheckChannelBusy : Input Para Error u32Channel = %d! \n",u32Channel,2,3,4,5,6);
        return BSP_ERR_DMA_INVALID_PARA;
    }
    /* 读取寄存器判断通道是否忙*/
    BSP_REG_READ(DMA_REGBASE_ADR,DMA_CHENREG, u32Result);
    if(u32Result&(0x01<<u32Channel))
    {
        BSP_TRACE(BSP_LOG_LEVEL_DEBUG, BSP_MODU_DMAC,
        "\r DMA_CheckChannelBusy:The Channel %d is busy! \n", u32Channel,2,3,4,5,6);
        return BSP_ERR_DMA_CHANNEL_BUSY;
    }
    else
    {
        return DMAC_SUCCESS;
    }
}

/*****************************************************************************
* 函 数 名  : DMA_ClearIntStatus
*
* 功能描述  : 清除通道中断状态
*
* 输入参数  : u32Chan：待操作的通道号
*           DMA_INT_TYPE_NUM inttype:待清除的中断类型
*
* 输出参数  : 无
* 返 回 值  : BSP_ERR_DMA_INVALID_PARA：参数错误
*          DMAC_SUCCESS：清除失败
*
* 修改记录  :2009年06月20日   吴振  创建

*****************************************************************************/
BSP_S32 DMA_ClearIntStatus(BSP_U32 u32Chan, DMA_INT_TYPE_NUM_E enIntType)
{
    /* 参数检查*/
    if((DMA_MAX_Index <= u32Chan)||(INT_BUTTOM_BIT <= enIntType))
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
        		"\r DMA_ClearIntStatus : Input Para Error! \n",1,2,3,4,5,6);
        return BSP_ERR_DMA_INVALID_PARA;
    }

    /* 清中断*/
    /* modified for lint e665 */
    BSP_REG_WRITE(DMA_REGBASE_ADR,(DMA_CLEAR_INT_BASE + enIntType * DMA_INT_ADDRESS_SPAN), DMA_HIGH_LEVEL<<u32Chan);
    /* modified for lint e665 */
    
    return DMAC_SUCCESS;
}

/*****************************************************************************
* 函 数 名  : DMA_ClearAllIntStatus
*
* 功能描述  : 清除某通道所有中断状态
*
* 输入参数  : u32Chan：待操作的通道号
* 输出参数  : 无
* 返 回 值  : BSP_ERR_DMA_INVALID_PARA：参数错误
*          DMAC_SUCCESS：清除失败
*
* 修改记录  :2009年06月20日   吴振  创建

*****************************************************************************/
BSP_S32 DMA_ClearAllIntStatus(BSP_U32 u32Chan)
{   
    /* 参数检查*/
    if(DMA_MAX_Index <= u32Chan)
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
        		"\r DMA_ClearAllIntStatus : Input Para Error! \n",1,2,3,4,5,6);
        return BSP_ERR_DMA_INVALID_PARA;
    }

    /* 清5种中断*/
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CLEARTFR, DMA_HIGH_LEVEL<<u32Chan);
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CLEARBLOCK, DMA_HIGH_LEVEL<<u32Chan);
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CLEARSRCTRAN, DMA_HIGH_LEVEL<<u32Chan);
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CLEARDSTTRAN, DMA_HIGH_LEVEL<<u32Chan);
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CLEARERR, DMA_HIGH_LEVEL<<u32Chan);

    return DMAC_SUCCESS;
}

/*****************************************************************************
* 函 数 名  : DMA_SetTransWidth
*
* 功能描述  : 根据传入的传输地址，确定传输位宽
*
* 输入参数  : BSP_U32 u32TransAddr：待传输的地址
              BSP_U32 u32TransLength：待传输的长度
* 输出参数  : BSP_U32 *u32TransWidth：存储传输位宽的值的地址
* 返 回 值  : 无
*
* 修改记录  :2009年06月20日   吴振  创建

*****************************************************************************/
BSP_VOID DMA_SetTransWidth(BSP_U32 u32TransAddr, BSP_U32 u32TransLength, BSP_U32 *u32TransWidth)/*lint -e715*/
{
    /* 内部调用，地址不会超出内存范围，入参不会为空指针 */
    /* 目前IP最大可以支持到64bit位宽*/
    if((0 == u32TransLength%8)&&(u32TransLength >=8))
    {
        *u32TransWidth = DMA_TRANS_WIDTH_64; /* b011代表64位位宽 */
    }
    /* 地址四字节对齐，同时传输长度为4字节整数倍*/
    else if((0 == u32TransLength%4)&&(u32TransLength >=4))
    {
        *u32TransWidth = DMA_TRANS_WIDTH_32; /* b010代表32位位宽 */
    }
    /* 地址或者传输长度为奇数*/
    else if(1 == u32TransLength%2)
    {
        *u32TransWidth = DMA_TRANS_WIDTH_8; /* b000代表8位位宽 */
    }
    /* 剩下的3种情况，均可以2字节位宽传输 */
    else 
    {
        *u32TransWidth = DMA_TRANS_WIDTH_16; /* b001代表16位位宽 */
    }
    return;
}

/*****************************************************************************
* 函 数 名  : DMA_IntHandler
*
* 功能描述  : DMA中断回调函数
*
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : OK
*
* 修改记录  :2008年12月26日   吴振  创建

*****************************************************************************/
#ifdef __KERNEL__
irqreturn_t  DMA_IntHandler (int irq, void* u32Chan)
#else
BSP_VOID  DMA_IntHandler (BSP_U32 u32Chan)
#endif
{
    BSP_U32 u32ChannelStatus, u32ChannelTCStatus, u32ChannelErrStatus;
    BSP_U32 Chan = (BSP_U32)u32Chan;

    PROCESS_ENTER(BSP_DMAC_TRACE);
    /* read the status of current interrupt */
    BSP_REG_READ(DMA_REGBASE_ADR,DMA_STATUSINT, u32ChannelStatus);
    BSP_REG_READ(DMA_REGBASE_ADR,DMA_STATUSTFR, u32ChannelTCStatus);
    BSP_REG_READ(DMA_REGBASE_ADR,DMA_STATUSERR, u32ChannelErrStatus);

    /* 清中断*/
    BSP_REG_WRITE(DMA_REGBASE_ADR,DMA_CLEARTFR, u32ChannelTCStatus&(0x01 << Chan));
    BSP_REG_WRITE(DMA_REGBASE_ADR,DMA_CLEARERR, u32ChannelErrStatus&(0x01 << Chan));

    /* decide which channel has trigger the interrupt*/
    if(u32ChannelStatus & 0x1)/* if there is a tfr int */
    {
        if((u32ChannelTCStatus >> Chan) & 0x01)
        {
            /* 只有普通通道才会释放*/
            if(DMA_COMMON_MODE == g_stChannelInfo[Chan].u32ChnAttribute)
            {
                /* 将通道逻辑状态置为IDLE*/
                g_stChannelInfo[Chan].u32ChnUsrStatus= DMA_CHN_IDLE;
            }
                /* 调用注册的中断处理函数*/
            if(g_stChannelInfo[Chan].pfuncDmaTcIsr != NULL)
            {
                (BSP_VOID)(*g_stChannelInfo[Chan].pfuncDmaTcIsr)(g_stChannelInfo[Chan].s32DmaTcPara);
            }
        }        
    }
    if(u32ChannelStatus & 0x10)/* if there is an err int */
    {
        if((u32ChannelErrStatus >> Chan) & 0x01)
        {
            /* 只有普通通道才会释放*/
            if(DMA_COMMON_MODE == g_stChannelInfo[Chan].u32ChnAttribute)
            {
                /* 将通道逻辑状态置为IDLE*/
                g_stChannelInfo[Chan].u32ChnUsrStatus = DMA_CHN_IDLE;
            }
            /* 调用注册的错误中断处理函数*/
            if(g_stChannelInfo[Chan].pfuncDmaErrIsr!= NULL)
            {
            	(BSP_VOID)(* g_stChannelInfo[Chan].pfuncDmaErrIsr)(g_stChannelInfo[Chan].s32DmaErrPara);
            }
        }
    }

    PROCESS_EXIT(BSP_DMAC_TRACE);
    #ifdef __KERNEL__
    return IRQ_HANDLED;
    #endif
}
/*****************************************************************************
* 函 数 名  : DMA_LliBuildForSingleBlockTrans
*
* 功能描述  : 单块传输超大时拆分为链表的处理函数
*
* 输入参数  : DMA_SINGLE_BLOCK_S stDMASingleBlock              单块传输参数结构
*             DMA_BUILD_LLI_FOR_BLOCK_S stDMABuildLliForBlock  构建链表参数结构
* 输出参数  : 无
*
* 返 回 值  : 头节点
*
* 其它说明  : Modified by liumengcun, 2009-11-13 与PS MSP讨论，刷cache均由使用者完成
*
*****************************************************************************/
BSP_VOID * DMA_LliBuildForSingleBlockTrans(DMA_SINGLE_BLOCK_S * stDMASingleBlock,
    DMA_BUILD_LLI_FOR_BLOCK_S stDMABuildLliForBlock)
{
    DMA_LLI_PHY_S * pstDMALliPhy = NULL;/* 临时结构*/
    BSP_U32 i;
    BSP_U32 u32LengthTmp = 0;
    BSP_U32 u32TransferBytes = 0; /* 每次传输大小*/
    BSP_U32 u32Chan = 0;

    BSP_U32 u32SrcWidth = 0;/* 源传输位宽*/
    BSP_U32 u32DstWidth = 0;/* 目的传输位宽*/
    BSP_U32 u32MemFlushLength = 0;/* 刷新memory长度*/

    u32Chan = stDMASingleBlock->stChnPara.u32Chan;
    u32MemFlushLength = sizeof(DMA_LLI_PHY_S)*stDMABuildLliForBlock.u32NodeNum;

    g_stChannelInfo[u32Chan].pBlockToLLIHead = g_pu32PhyAddrPointer[u32Chan];
    if (NULL == g_stChannelInfo[u32Chan].pBlockToLLIHead)
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC, "\r DMA single block memalign fail!\n",0,0,0,0,0,0);
        return NULL;
    }

    DMA_SetTransWidth(stDMASingleBlock->u32SrcAddr, stDMABuildLliForBlock.u32OneTimeTransLength, &u32SrcWidth);
    DMA_SetTransWidth(stDMASingleBlock->u32DstAddr, stDMABuildLliForBlock.u32OneTimeTransLength, &u32DstWidth);

    pstDMALliPhy = (DMA_LLI_PHY_S *)g_stChannelInfo[u32Chan].pBlockToLLIHead;/*lint !e740 !e826*/
    /* 循环配置节点的数据 */
    for(i = 0; i < stDMABuildLliForBlock.u32NodeNum; i++)
    {
        if (i < stDMABuildLliForBlock.u32NodeNum - 1)
        {
            switch(stDMASingleBlock->enTransType)
            {
                case MEM2MEM_DMA:
                {
                    /* memory到memory情况下需要重新计算源地址、目的地址*/
                    pstDMALliPhy->SrcAddr = stDMASingleBlock->u32SrcAddr + i*stDMABuildLliForBlock.u32OneTimeTransLength;
                    pstDMALliPhy->DstAddr = stDMASingleBlock->u32DstAddr + i*stDMABuildLliForBlock.u32OneTimeTransLength;

                    break;
                }
                case MEM2PRF_DMA:
                case MEM2PRF_PRF:
                {
                    pstDMALliPhy->DstAddr = stDMASingleBlock->u32DstAddr;
                    /* memory到外设情况下需要重新计算源地址*/
                    pstDMALliPhy->SrcAddr = stDMASingleBlock->u32SrcAddr + i*stDMABuildLliForBlock.u32OneTimeTransLength;

                    u32DstWidth = stDMASingleBlock->enDstWidth;
                    break;
                }
                case PRF2MEM_DMA:
                case PRF2MEM_PRF:
                {
                    pstDMALliPhy->SrcAddr = stDMASingleBlock->u32SrcAddr;
                    /* 外设到memory情况下需要重新计算目的地址*/
                    pstDMALliPhy->DstAddr = stDMASingleBlock->u32DstAddr + i*stDMABuildLliForBlock.u32OneTimeTransLength;

                    u32SrcWidth = stDMASingleBlock->enSrcWidth;
                    break;
                }
                case PRF2PRF_SRCPRF:
                case PRF2PRF_DSTPRF:
                {
                    /* 外设到外设情况下不需要重新计算源地址、目的地址*/
                    pstDMALliPhy->SrcAddr = stDMASingleBlock->u32SrcAddr;
                    pstDMALliPhy->DstAddr = stDMASingleBlock->u32DstAddr;

                    u32SrcWidth = stDMASingleBlock->enSrcWidth;
                    u32DstWidth = stDMASingleBlock->enDstWidth;
                    break;
                }
                default:
                    BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
    			        "\r stDMASingleBlock.enTransType ERROR!\n",0,0,0,0,0,0);
                    g_stChannelInfo[u32Chan].pBlockToLLIHead = NULL;
                    break;

            }
            u32TransferBytes = stDMABuildLliForBlock.u32OneTimeTransLength;
        }

        else
        {
            u32LengthTmp = stDMASingleBlock->u32TransLength -
                stDMABuildLliForBlock.u32OneTimeTransLength * (stDMABuildLliForBlock.u32NodeNum - 1);
            switch(stDMASingleBlock->enTransType)
            {
                case MEM2MEM_DMA:
                {

                    /* memory到memory情况下需要重新计算源地址、目的地址*/

                    pstDMALliPhy->SrcAddr = stDMASingleBlock->u32SrcAddr + i*stDMABuildLliForBlock.u32OneTimeTransLength;
                    pstDMALliPhy->DstAddr = stDMASingleBlock->u32DstAddr + i*stDMABuildLliForBlock.u32OneTimeTransLength;

                    DMA_SetTransWidth(pstDMALliPhy->SrcAddr, u32LengthTmp, &u32SrcWidth);
                    DMA_SetTransWidth(pstDMALliPhy->DstAddr, u32LengthTmp, &u32DstWidth);
                    break;
                }
                case MEM2PRF_DMA:
                case MEM2PRF_PRF:
                {
                    pstDMALliPhy->DstAddr = stDMASingleBlock->u32DstAddr;
                    /* memory到外设情况下需要重新计算源地址*/
                    pstDMALliPhy->SrcAddr = stDMASingleBlock->u32SrcAddr + i*stDMABuildLliForBlock.u32OneTimeTransLength;
                    
                    DMA_SetTransWidth(pstDMALliPhy->SrcAddr, u32LengthTmp, &u32SrcWidth);
                    u32DstWidth = stDMASingleBlock->enDstWidth;
                    break;
                }
                case PRF2MEM_DMA:
                case PRF2MEM_PRF:
                {
                    pstDMALliPhy->SrcAddr = stDMASingleBlock->u32SrcAddr;
                    /* 外设到memory情况下需要重新计算目的地址*/                    
                    pstDMALliPhy->DstAddr = stDMASingleBlock->u32DstAddr + i*stDMABuildLliForBlock.u32OneTimeTransLength;

                    u32SrcWidth = stDMASingleBlock->enSrcWidth;
                    DMA_SetTransWidth(pstDMALliPhy->DstAddr, u32LengthTmp, &u32DstWidth);
                    break;
                }
                case PRF2PRF_SRCPRF:
                case PRF2PRF_DSTPRF:
                {
                    /* 外设到外设情况下不需要重新计算源地址、目的地址*/
                    pstDMALliPhy->SrcAddr = stDMASingleBlock->u32SrcAddr;
                    pstDMALliPhy->DstAddr = stDMASingleBlock->u32DstAddr;

                    u32SrcWidth = stDMASingleBlock->enSrcWidth;
                    u32DstWidth = stDMASingleBlock->enDstWidth;
                    break;
                }
                default:
                    BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
    			        "\r stDMASingleBlock.enTransType ERROR!\n",0,0,0,0,0,0);
                    g_stChannelInfo[u32Chan].pBlockToLLIHead = NULL;
                    break;

            }

            u32TransferBytes = u32LengthTmp;
        }

        BSP_TRACE(BSP_LOG_LEVEL_DEBUG, BSP_MODU_DMAC,
            "\r u32TransferBytes = 0x%x,u32SrcWidth = 0x%x! SrcAddr = 0x%x,DstAddr = 0x%x\n",
            u32TransferBytes,u32SrcWidth,pstDMALliPhy->SrcAddr,pstDMALliPhy->DstAddr,0,0);
        pstDMALliPhy->NextLLI = (BSP_U32)DMAC_VIRT_TO_PHYS(pstDMALliPhy + 1);

        pstDMALliPhy->ulTransferCtrl_l = DMA_CTL_L_DEFAULT | DMA_LLI_ENABLE | (u32SrcWidth << 0x4) | (u32DstWidth <<
0x1)
              | (stDMASingleBlock->enTransType << 20) | (stDMABuildLliForBlock.u32CtlDINC << 7)
              | (stDMABuildLliForBlock.u32CtlSINC << 9)
              | (stDMABuildLliForBlock.u32DstBurstLength << 11) | (stDMABuildLliForBlock.u32SrcBurstLength << 14)
              | (stDMABuildLliForBlock.u32CtlSMS << 25) | (stDMABuildLliForBlock.u32CtlDMS << 23);

        pstDMALliPhy->ulTransferCtrl_h = DMA_CTL_H_DEFAULT | (u32TransferBytes / ((BSP_U32)0x1<<u32SrcWidth));
        pstDMALliPhy->s_stat = 0;
        pstDMALliPhy->d_stat = 0;

        pstDMALliPhy++;
    }

    /* 最后一个节点的next赋0*/
    ((DMA_LLI_PHY_S*)(pstDMALliPhy -1))->NextLLI = 0;

    /* 最后一个节点，去使能源和目的链表使能位 */
    ((DMA_LLI_PHY_S*)(pstDMALliPhy -1))->ulTransferCtrl_l = DMA_CTL_L_DEFAULT | 0x0 | (u32SrcWidth<<0x4) | (u32DstWidth<<0x1)
        | (stDMASingleBlock->enTransType << 20) | (stDMABuildLliForBlock.u32CtlDINC << 7)
        | (stDMABuildLliForBlock.u32CtlSINC << 9)
        | (stDMABuildLliForBlock.u32DstBurstLength << 11) | (stDMABuildLliForBlock.u32SrcBurstLength << 14)
        | (stDMABuildLliForBlock.u32CtlSMS << 25) | (stDMABuildLliForBlock.u32CtlDMS << 23);
    /* 刷新cache，ARM是32位cache line，需要以32位为单位刷新 外设不需要刷cache*/

    DMAC_CACHE_FLUSH(g_stChannelInfo[u32Chan].pBlockToLLIHead, u32MemFlushLength);

    return (BSP_VOID*)DMAC_VIRT_TO_PHYS(g_stChannelInfo[u32Chan].pBlockToLLIHead);
}/*lint !e818*/


BSP_VOID DMAC_INT_CONNECT(BSP_U32 u32Chan)
{
    switch(u32Chan)
    {
    #ifdef __VXWORKS__
        case 0:
            (BSP_VOID)intConnect ((VOIDFUNCPTR *)INT_VEC_DMA0, DMA_IntHandler, (int)u32Chan);
            (BSP_VOID)intEnable(INT_LVL_DMA0);
            break;
        case 1:
            (BSP_VOID)intConnect ((VOIDFUNCPTR *)INT_VEC_DMA1, DMA_IntHandler, (int)u32Chan);
            (BSP_VOID)intEnable(INT_LVL_DMA1);
            break;
        case 2:
            (BSP_VOID)intConnect ((VOIDFUNCPTR *)INT_VEC_DMA2, DMA_IntHandler, (int)u32Chan);
            (BSP_VOID)intEnable(INT_LVL_DMA2);
            break;
        case 3:
            (BSP_VOID)intConnect ((VOIDFUNCPTR *)INT_VEC_DMA3, DMA_IntHandler, (int)u32Chan);
            (BSP_VOID)intEnable(INT_LVL_DMA3);
            break;
    #else
        case 0:
            if(request_irq(INT_LVL_DMA0, (irq_handler_t)DMA_IntHandler, 0, "DMAC0_APP_IRQ", (void*)u32Chan) != 0)
            {
                DMAC_PRINT("DMAC0 irequest_irq error\n");
            }
            break;
        case 1:
            if(request_irq(INT_LVL_DMA1, (irq_handler_t)DMA_IntHandler, 0, "DMAC1_APP_IRQ", (void*)u32Chan) != 0)
            {
                DMAC_PRINT("DMAC1 irequest_irq error\n");
            }
            break;
        case 2:
            if(request_irq(INT_LVL_DMA2, (irq_handler_t)DMA_IntHandler, 0, "DMAC2_APP_IRQ", (void*)u32Chan) != 0)
            {
                DMAC_PRINT("DMAC2 irequest_irq error\n");
            }
            break;
        case 3:
            if(request_irq(INT_LVL_DMA3, (irq_handler_t)DMA_IntHandler, 0, "DMAC3_APP_IRQ", (void*)u32Chan) != 0)
            {
                DMAC_PRINT("DMAC3 irequest_irq error\n");
            }
            break;
    #endif
        default:
            return ;
    }
        
}
#define DMAC_INIT_TIMEOUT           500
#define DMAC_INIT_FINISH_MAGIC      0x2a2a2a2a

/*****************************************************************************
* 函 数 名  : BSP_DMA_Init
*
* 功能描述  : DMA初始化
*
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : DMAC_SUCCESS           初始化成功
              BSP_ERR_DMA_NULL_PTR  申请资源失败
*
* 修改记录  : 2011年5月13日   吴振  创建
              2011年5月13日   鲁婷  修改

*****************************************************************************/
BSP_S32 BSP_DMA_Init(BSP_VOID)
{
    BSP_U32 i,j;
    BSP_S32 ret;
    BSP_U32 state;

    DMAC_PRINT("============BSP_DMA_Init    in=========\n");
    if(g_u32DmaInit)
    {
    	BSP_TRACE(BSP_LOG_LEVEL_DEBUG, BSP_MODU_DMAC,
    			"\r DMA don't need init again!\n",0,0,0,0,0,0);
    	return DMAC_SUCCESS;
    }

    /* 初始化*/
    memset(g_stChannelInfo, 0x0, sizeof(g_stChannelInfo));
    
    /*lint -e685 -e568 -e681*/
    for(i = DMA_MIN_Index; i < (DMA_MIN_Index+DMA_SPECIAL_MODE_NUM); i++)
    {
        g_stChannelInfo[i].u32ChnAttribute = DMA_SPECIAL_MODE;
        g_stChannelInfo[i].u32ChnUsrStatus = DMA_CHN_ALLOCED;
    }
   
    /* 每个通道的资源节点直接申请好不释放，并为每个通道挂接中断 */
    for(i = DMA_MIN_Index; i < DMA_MAX_Index; i++)
    {
        g_pu32PhyAddrPointer[i] = DMAC_MALLOC_OS(sizeof(DMA_LLI_PHY_S)*DMA_NODE_NUM_NORMAL);
        if(NULL == g_pu32PhyAddrPointer[i])
        {
            for(j = DMA_MIN_Index; j < i; j++)
            {
                DMAC_FREE_OS(g_pu32PhyAddrPointer[j]);
                g_pu32PhyAddrPointer[j] = NULL;
            }
        	BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
        			"\r BSP_DMA_init:The g_pu32PhyAddrPointer[%d] address malloc error!\n",i,0,0,0,0,0);
        	return BSP_ERR_DMA_NULL_PTR;
        }

        g_stChannelInfo[i].pfuncDmaTcIsr = NULL;
        g_stChannelInfo[i].s32DmaTcPara = 0;

        g_stChannelInfo[i].pfuncDmaErrIsr= NULL;
        g_stChannelInfo[i].s32DmaErrPara= 0;

        DMAC_INT_CONNECT(i);
    }
#ifdef __VXWORKS__
    ret = BSP_PWRCTRL_TimerOpen(PWRCTRL_MODU_DMAC);
    if(ret != BSP_OK)
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_MEM, 
                  "BSP_PWRCTRL_TimerOpen ret=0x%x, line:%d\n", ret, __LINE__);
    }

    /* 清中断*/
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CLEARTFR, DMA_INT_CLEAR);
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CLEARERR, DMA_INT_CLEAR);
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CLEARBLOCK, DMA_INT_CLEAR);
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CLEARSRCTRAN, DMA_INT_CLEAR);
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CLEARDSTTRAN, DMA_INT_CLEAR);

        /* 打开传输完成和传输错误中断，其余中断暂时不打开 */
        BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_MASKTFR, DMA_INT_MASK_WE|DMA_INT_UNMASK);
        BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_MASKERR, DMA_INT_MASK_WE|DMA_ERRINT_UNMASK);

    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_MASKBLOCK, DMA_INT_MASK_WE|DMA_INT_MASK);
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_MASKSRCTRAN, DMA_INT_MASK_WE|DMA_INT_MASK);
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_MASKDSTTRAN, DMA_INT_MASK_WE|DMA_INT_MASK);
	/* 关闭DMA时钟 */
	BSP_PWRCTRL_SoftClkDis(PWRCTRL_MODU_DMAC);

	BSP_SYNC_Give(SYNC_MODULE_DMA);
#else
	ret = BSP_SYNC_Wait(SYNC_MODULE_DMA, 5000);
	if(ret != BSP_OK)
	{

		return ERROR;
	}

#endif
    g_u32DmaInit = DMA_INITIALIZED;
    DMAC_PRINT("============BSP_DMA_Init    out=========\n");
    return OK;

}


/*****************************************************************************
* 函 数 名  : BSP_DMA_GetIdleChannel
*
* 功能描述  : 获取一个当前处于空闲状态的DMA通道，当前块或者链表传输完数据后会
*             自动释放，下次再使用时需再获取当前空闲通道,目前DMA_SPECIAL_MODE
*             仅供协议栈使用，其他用户申请不到此种通道
*
* 输入参数  : DMA_CHN_TYPE_E enChnType 通道类型
*
* 输出参数  : BSP_U32 *pulChan      存放申请到的通道号
* 返 回 值  : DMAC_SUCCESS       操作成功
*             BSP_ERR_DMA_MODULE_NOT_INITED      DMA未初始化
*             BSP_ERR_DMA_INVALID_PARA  参数错误
*             BSP_ERR_DMA_ALL_CHANNEL_BUSY 所有通道忙
*
* 修改记录  :2009年10月15日   liumengcun  创建

*****************************************************************************/
BSP_S32  BSP_DMA_GetIdleChannel(DMA_CHN_TYPE_E enChnType, BSP_U32 *pulChan)
{
	BSP_U32 channelinfo;
	BSP_U32 lockKey = 0;
    BSP_U32 i;

    /* 是否初始化判断*/
	if(DMA_NOTINITIALIZE == g_u32DmaInit)
	{
		BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
				"\rBSP_DMA_GetIdleChannel Not initialized! \n",0,0,0,0,0,0);
		return BSP_ERR_DMA_MODULE_NOT_INITED;
	}

    /* 参数检查*/
    if ((NULL == pulChan) || (enChnType > DMA_COMMON_MODE))
    {
    	BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
    					"\rBSP_DMA_GetIdleChannel NULL == pulChan! \n",0,0,0,0,0,0);
        return BSP_ERR_DMA_INVALID_PARA;
    }

    *pulChan = 0xffffffff;

	/* 打开DMA时钟 */
	BSP_PWRCTRL_SoftClkEn(PWRCTRL_MODU_DMAC);

    DMAC_INT_LOCK(lockKey);

    /***** get the current channel information*/
    BSP_REG_READ(DMA_REGBASE_ADR,DMA_CHENREG, channelinfo);
    channelinfo= channelinfo&0x00ff;

    /***** according the input parameter,decide the channel number */
    for (i = DMA_MIN_Index; i < DMA_MAX_Index; i++)
    {
         /* 当通道物理状态和逻辑状态均为空闲时才可以使用*/
         if ((DMA_CHN_IDLE == g_stChannelInfo[i].u32ChnUsrStatus)
             && DMA_SPECIAL_MODE != g_stChannelInfo[i].u32ChnAttribute)
         {
              /*clear the interrupt in this channel that has not deal with*/
       	      BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CLEARTFR, (0x01 << i));
       	      BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CLEARERR, (0x01 << i));
              *pulChan = i;

              /* 通道逻辑状态置为busy*/
              g_stChannelInfo[i].u32ChnUsrStatus = DMA_CHN_ALLOCED;
              g_stChannelInfo[i].u32ChnAttribute = enChnType;

              DMAC_INT_UNLOCK(lockKey); /*lint !e718 !e746*/
              return DMAC_SUCCESS;
         }
         channelinfo = channelinfo >> 1;
     }

    DMAC_INT_UNLOCK(lockKey);
    return BSP_ERR_DMA_ALL_CHANNEL_BUSY;
}

/*****************************************************************************
* 函 数 名  : BSP_DMA_ChannelRelease
*
* 功能描述  : DMA传输通道释放,正常情况下不需要调用此接口释放通道，传送完成后
*             底软会自动释放通道，本接口为了处理某些异常情况下，取得了通道但
*             是却没有开始数据传输，无法自动释放通道的时候调用此接口手动释放通道
*             通道正在传输时无法释放通道，通道0给协议栈固定使用无法释放。
*
* 输入参数  : BSP_U32 ulChan  通道号
* 输出参数  : 无
* 返 回 值  : DMAC_SUCCESS       释放成功
*             BSP_ERR_DMA_MODULE_NOT_INITED      DMA未初始化
*             BSP_ERR_DMA_INVALID_PARA  参数错误
*             BSP_ERR_DMA_CHANNEL_BUSY  通道忙
*
* 修改记录  :2008年12月26日   吴振  创建

*****************************************************************************/
BSP_U32  BSP_DMA_ChannelRelease(BSP_U32 u32Chan)
{
    BSP_U32 channelinfo = 0;
    /* 是否初始化判断*/
	if(DMA_NOTINITIALIZE == g_u32DmaInit)
	{
		BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
				"\r Not initialized! \n",0,0,0,0,0,0);
		return BSP_ERR_DMA_MODULE_NOT_INITED;
	}
	/* 参数检查*/
    if(u32Chan >= DMA_MAX_Index)
    {
      	 BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
				"\r BSP_DMA_ChannelRelease u32Chan = 0x%x! \n",
				u32Chan,0,0,0,0,0);
         return BSP_ERR_DMA_INVALID_PARA;
    }

    /* 参数检查*/
    if(DMA_SPECIAL_MODE  ==  g_stChannelInfo[u32Chan].u32ChnAttribute)
    {
      	 BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
				"\r BSP_DMA_ChannelRelease u32Chan = 0x%x,g_stChannelInfo[u32Chan].u32ChnAttribute = 0x%x! \n",
				u32Chan,g_stChannelInfo[u32Chan].u32ChnAttribute,0,0,0,0);
         return BSP_ERR_DMA_INVALID_PARA;
    }

    /* 通道正在传输数据时无法释放通道*/
    BSP_REG_READ(DMA_REGBASE_ADR,DMA_CHENREG, channelinfo);
    if(channelinfo & (0x1<<u32Chan))
    {
    	BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC, 
    					"\r BSP_DMA_ChannelRelease channel is busy u32Chan = 0x%x! \n",u32Chan,0,0,0,0,0);
        return BSP_ERR_DMA_CHANNEL_BUSY;
    }
    /* 释放通道逻辑状态*/
    g_stChannelInfo[u32Chan].u32ChnUsrStatus= DMA_CHN_IDLE;

    return DMAC_SUCCESS;
}

/*****************************************************************************
* 函 数 名  : BSP_DMA_AllChannelCheck
*
* 功能描述  : DMA全部通道是否空闲检测函数
*
* 输入参数  : 无
* 输出参数  : pChannelInfo   所有通道使用状态，用bit掩码表示，正在传输数据的通道bit位置1，
*               bit0对应0通道，bit1对应1通道，以此类推
* 返 回 值  : DMAC_SUCCESS       操作成功
*             BSP_ERR_DMA_MODULE_NOT_INITED      DMA未初始化
*             BSP_ERR_DMA_INVALID_PARA  参数错误
* 修改记录  :2008年12月26日   吴振  创建

*****************************************************************************/
BSP_S32  BSP_DMA_AllChannelCheck(BSP_U32 *pChannelInfo)
{
    BSP_U32 u32ChnInfo = 0;

    /* 是否初始化判断*/
    if(DMA_NOTINITIALIZE == g_u32DmaInit)
    {
    	BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
    			"\rNot initialized! \n",0,0,0,0,0,0);
    	return BSP_ERR_DMA_MODULE_NOT_INITED;
    }

    /* 参数检查*/
    if(NULL == pChannelInfo)
    {
     	 BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
				"\r BSP_DMA_AllChannelCheck NULL == pChannelInfo \n",0,0,0,0,0,0);
        return BSP_ERR_DMA_INVALID_PARA;
    }

    /* 读取所有通道的状态*/
    BSP_REG_READ(DMA_REGBASE_ADR,DMA_CHENREG, u32ChnInfo);
    
    /* 目前DMA有4个通道，所以取低4bit*/
    *pChannelInfo = u32ChnInfo&DMA_CHN_EN;
    BSP_TRACE(BSP_LOG_LEVEL_DEBUG, BSP_MODU_DMAC,
        			"\r BSP_DMA_AllChannelCheck *pChannelInfo = 0x%x! \n",*pChannelInfo,0,0,0,0,0);

    return DMAC_SUCCESS;
}


/*****************************************************************************
* 函 数 名  : BSP_DMA_OneChannelCheck
*
* 功能描述  : DMA单个通道具体物理状态检查
*
* 输入参数  : BSP_U32 ulChan   待检查的通道号
* 输出参数  : BSP_U32 *pChannelStatus  该通道具体状态，0表示空闲，1表示忙
* 返 回 值  : DMAC_SUCCESS       操作成功
*             BSP_ERR_DMA_MODULE_NOT_INITED      DMA未初始化
*             BSP_ERR_DMA_INVALID_PARA  参数错误
*
* 修改记录  :2008年12月26日   吴振  创建

*****************************************************************************/
BSP_S32 BSP_DMA_OneChannelCheck(BSP_U32 u32Chan, BSP_U32 *pChannelStatus)
{
    BSP_U32 channelinfo = 0;
    /* 是否初始化判断*/
	if(DMA_NOTINITIALIZE == g_u32DmaInit)
	{
		BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
				"\rNot initialized! \n",0,0,0,0,0,0);
		return BSP_ERR_DMA_MODULE_NOT_INITED;
	}

    /* 参数检查*/
	if((DMA_MAX_Index <= u32Chan) || (NULL == pChannelStatus))
	{
     	 BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
				"\r BSP_DMA_OneChannelcheck u32Chan = 0x%x,or NULL == pChannelStatus \n",
				u32Chan,0,0,0,0,0);
	    return BSP_ERR_DMA_INVALID_PARA;
	}

    /* 返回当前通道的物理状态*/
    BSP_REG_READ(DMA_REGBASE_ADR,DMA_CHENREG, channelinfo);
    if (channelinfo&(0x1<<u32Chan))
    {
        *pChannelStatus = DMA_CHN_BUSY;
    }
    else
    {
        *pChannelStatus = DMA_CHN_FREE;
    }
    BSP_TRACE(BSP_LOG_LEVEL_DEBUG, BSP_MODU_DMAC,
            "\r*p the channel[%d]'s real status is = 0x%x! \n",
            u32Chan,channelinfo,0,0,0,0);
    return DMAC_SUCCESS;
}




/*****************************************************************************
* 函 数 名  : BSP_DMA_SingleBlockStart
*
* 功能描述  : 单块数据传输启动
*
* 输入参数  : DMA_SINGLE_BLOCK_S stDMASingleBlock   单块传输使用的参数结构体
*
* 输出参数  : 无
* 返 回 值  : DMAC_SUCCESS       操作成功
*             BSP_ERR_DMA_MODULE_NOT_INITED      DMA未初始化
*             BSP_ERR_DMA_INVALID_PARA  参数错误
*             BSP_ERR_DMA_CHANNEL_BUSY  通道忙
*
* 修改记录  :2008年12月26日   吴振  创建
             2010年12月1日    鲁婷  修改 1.与外设有关的位宽都由调用者赋值
                                         2.由调用者保证传输长度是位宽的整数倍
                                         3.修改传输类型判断条件，增加外设作流控的情况

*****************************************************************************/
BSP_S32 BSP_DMA_SingleBlockStart(DMA_SINGLE_BLOCK_S * stDMASingleBlock)
{
    /* 传输通道配置*/
    BSP_U32 u32TransferCtrl_h, u32TransferCtrl_l, u32TransferCfg_h, u32TransferCfg_l;
    BSP_U32 u32SrcWidth = 0;/* 源传输位宽*/
    BSP_U32 u32DstWidth = 0;/* 目的传输位宽*/
    BSP_U32 u32CtlSINC = 0; /* 源地址操作模式，即递增递减还是不变*/
    BSP_U32 u32CtlDINC = 0; /* 目的地址操作模式，即递增递减还是不变*/
    BSP_U32 u32BlockTS = 0; /* 单块传输大小*/
    BSP_U32 u32CtlSMS = DMA_MASTER1; /* 源设备Master 选择位*/
    BSP_U32 u32CtlDMS = DMA_MASTER1; /* 目的设备Master 选择位*/
    BSP_U32 u32Chan;/* 通道号*/
    BSP_U32 u32SrcBurstLength = DMA_MSIZE_16;
    BSP_U32 u32DstBurstLength = DMA_MSIZE_16;
    DMA_BUILD_LLI_FOR_BLOCK_S  stDMABuildLliForBlock;
    BSP_U32 *pHeadLLI = NULL;
    BSP_U32 u32LockKey = 0;
    BSP_U32 u32HS_SEL_SRC = DMA_SW_HANDSHAKE;
    BSP_U32 u32HS_SEL_DST = DMA_SW_HANDSHAKE;
    BSP_U32 u32MAX_ABRST = DMA_BURST_LEN16;
    
    /* 是否初始化判断*/
	if(DMA_NOTINITIALIZE == g_u32DmaInit)
	{
		BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
				"\rNot initialized! \n",0,0,0,0,0,0);
		return BSP_ERR_DMA_MODULE_NOT_INITED;
	}

    u32Chan = stDMASingleBlock->stChnPara.u32Chan;

    BSP_TRACE(BSP_LOG_LEVEL_DEBUG, BSP_MODU_DMAC,
    		"\r stDMASingleBlock=0x%x\n",stDMASingleBlock,0,0,0,0,0);

    /* 入参判断 */
    /* 目前DMA最大只支持到64位位宽*/
    /* 目前burst最大只支持到16*/
    /* 目前DMA不支持外设流控*/
    if((NULL == stDMASingleBlock) || (DMA_MAX_Index <= u32Chan) || (0 == stDMASingleBlock->u32SrcAddr)
        || (0 == stDMASingleBlock->u32DstAddr) || (0 == stDMASingleBlock->u32TransLength)
        || (CTL_TT_FC_BUTTOM <= stDMASingleBlock->enTransType) || (stDMASingleBlock->enSrcWidth > DMA_TRANS_WIDTH_64)
        || (stDMASingleBlock->enDstWidth > DMA_TRANS_WIDTH_64) || (stDMASingleBlock->enSrcBurstLength > DMA_MSIZE_16)
        || (stDMASingleBlock->enDstBurstLength > DMA_MSIZE_16) || (stDMASingleBlock->enSrcHSHKInterface> DMA_HK_BUTT)
        || (stDMASingleBlock->enDstHSHKInterface > DMA_HK_BUTT))
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
        		"\r BSP_DMA_SingleBlockStart:Input Para Error!",0,0,0,0,0,0);
        return BSP_ERR_DMA_INVALID_PARA;
    }

    DMAC_INT_LOCK(u32LockKey);
    /* 读通道使能寄存器ChEnReg[CH_EN]，判断通道是否空闲。为0空闲 */
    if(DMAC_SUCCESS != DMA_CheckChannelBusy(u32Chan))
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
            "\r BSP_DMA_SingleBlockStart:The dmac Channel is busy! \n",0,0,0,0,0,0);

        DMAC_INT_UNLOCK(u32LockKey);
        return BSP_ERR_DMA_CHANNEL_BUSY;
    }

    DMAC_INT_UNLOCK(u32LockKey);
    
    /* 注册完成中断处理函数*/    
    g_stChannelInfo[u32Chan].pfuncDmaTcIsr = stDMASingleBlock->stChnPara.pfuncDmaTcIsr;
    g_stChannelInfo[u32Chan].s32DmaTcPara = stDMASingleBlock->stChnPara.s32DmaTcPara;
    /* 注册错误中断处理函数*/
    g_stChannelInfo[u32Chan].pfuncDmaErrIsr = stDMASingleBlock->stChnPara.pfuncDmaErrIsr;
    g_stChannelInfo[u32Chan].s32DmaErrPara = stDMASingleBlock->stChnPara.s32DmaErrPara;

    /* 向INT_TC_CLR和INT_ERR_CLR的对应为1，清除选定通道的中断状态 */
    (BSP_VOID)DMA_ClearAllIntStatus(u32Chan);

    switch(stDMASingleBlock->enTransType)
    {
        case MEM2MEM_DMA:
        {
            /* 根据入参，计算最佳配置参数并进行配置
            * 起始地址和待传输长度需要配合，才能使用最好的配置*/
            DMA_SetTransWidth(stDMASingleBlock->u32SrcAddr, stDMASingleBlock->u32TransLength, &u32SrcWidth);
            DMA_SetTransWidth(stDMASingleBlock->u32DstAddr, stDMASingleBlock->u32TransLength, &u32DstWidth);

            u32CtlSINC = DMA_ADDR_INCREMENT;/* 源地址操作模式递增*/
            u32CtlDINC = DMA_ADDR_INCREMENT;/* 目的地址操作模式递增*/
            u32SrcBurstLength = DMA_MSIZE_16;
            u32DstBurstLength = DMA_MSIZE_16;

            break;
        }
        case MEM2PRF_DMA:
        case MEM2PRF_PRF:
        {
            u32SrcWidth = stDMASingleBlock->enSrcWidth;
            u32DstWidth = stDMASingleBlock->enDstWidth;
            /* 每一次目的传输中表明是否递增或者递减源地址，如果设备从源外设FIFO 的一个固定地址取数据，
              则将此域设置为“固定”*/
            u32CtlSINC = DMA_ADDR_INCREMENT;/* 源地址操作模式递增*/
            u32CtlDINC = DMA_ADDR_NOCHANGE;/* 目的地址操作模式固定*/

            u32SrcBurstLength = DMA_MSIZE_16;
            u32DstBurstLength = stDMASingleBlock->enDstBurstLength;
            u32HS_SEL_DST = DMA_HW_HANDSHAKE;
            break;
        }
        case PRF2MEM_DMA:
        case PRF2MEM_PRF:
        {
            u32SrcWidth = stDMASingleBlock->enSrcWidth;
            u32DstWidth = stDMASingleBlock->enDstWidth;
            /* 每一次源传输中表明是否递增或者递减源地址，如果设备用一个固定地址向目的外设FIFO 写数据，
              则将此域设置为“固定”*/
            u32CtlSINC = DMA_ADDR_NOCHANGE;/* 源地址操作模式固定*/
            u32CtlDINC = DMA_ADDR_INCREMENT; /* 目的地址操作模式递增*/

            u32SrcBurstLength = stDMASingleBlock->enSrcBurstLength;
            u32DstBurstLength = DMA_MSIZE_16;
            u32HS_SEL_SRC = DMA_HW_HANDSHAKE;
            break;
        }
        case PRF2PRF_SRCPRF:
        case PRF2PRF_DSTPRF:
        {
            u32SrcWidth = stDMASingleBlock->enSrcWidth;
            u32DstWidth = stDMASingleBlock->enDstWidth;
            u32CtlSINC = DMA_ADDR_NOCHANGE;/* 目的地址操作模式固定*/
            u32CtlDINC = DMA_ADDR_NOCHANGE;/* 目的地址操作模式固定*/

            u32SrcBurstLength = stDMASingleBlock->enSrcBurstLength;
            u32DstBurstLength = stDMASingleBlock->enDstBurstLength;
            u32HS_SEL_SRC = DMA_HW_HANDSHAKE;
            u32HS_SEL_DST = DMA_HW_HANDSHAKE;
            break;
        }
        default:
            BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
		        "\r stDMASingleBlock.enTransType ERROR!\n",0,0,0,0,0,0);
            break;

    }

        /* 主核用master1 从核用master2 */
#ifdef PRODUCT_CFG_CORE_TYPE_MODEM 
        u32CtlSMS = DMA_MASTER1;
        u32CtlDMS = DMA_MASTER1;
#else         
        u32CtlSMS = DMA_MASTER2;
        u32CtlDMS = DMA_MASTER2;
#endif
    
    BSP_TRACE(BSP_LOG_LEVEL_DEBUG, BSP_MODU_DMAC,"\r BSP_DMA_SingleBlockStart:ulChan=%d,u32SrcWidth = 0x%x,u32DstWidth = 0x%x,u32HS_SEL_SRC = 0x%x,u32HS_SEL_DST = 0x%x\n",u32Chan,u32SrcWidth,u32SrcWidth,u32HS_SEL_SRC,u32HS_SEL_DST,0);

    /* 配置CFG寄存器*/
    u32TransferCfg_h = DMA_CFG_H_DEFAULT|(stDMASingleBlock->enDstHSHKInterface<<11)
        |(stDMASingleBlock->enSrcHSHKInterface<<7);
    u32TransferCfg_l = DMA_CFG_L_DEFAULT|(u32HS_SEL_SRC<<11)|(u32HS_SEL_DST<<10)|(u32MAX_ABRST<<20);

    /* 如果是超大块的情况，需要拆分为多次传输*/
    if((DMA_MAX_SINGLE_BLOCK_TRANS_LENGTH*((BSP_U32)0x1<<u32SrcWidth)) < (stDMASingleBlock->u32TransLength))
    {
        memset((void*)&stDMABuildLliForBlock, 0x0, sizeof(DMA_BUILD_LLI_FOR_BLOCK_S));
        /* 计算当前配置下每块可以支持的最大传输大小*/
        stDMABuildLliForBlock.u32OneTimeTransLength = (1<<u32SrcWidth) * DMA_SINGLE_BLOCK_TRANS_LENGTH ;

        BSP_TRACE(BSP_LOG_LEVEL_DEBUG, BSP_MODU_DMAC, "\r u32OneTimeTransLength =  0x%x\n",
                stDMABuildLliForBlock.u32OneTimeTransLength,0,0,0,0,0);

        /* 计算需要拆分为几次传输*/
        if (0 == stDMASingleBlock->u32TransLength%stDMABuildLliForBlock.u32OneTimeTransLength)
        {
            stDMABuildLliForBlock.u32NodeNum = stDMASingleBlock->u32TransLength/stDMABuildLliForBlock.
u32OneTimeTransLength;
        }
        else
        {
            stDMABuildLliForBlock.u32NodeNum = stDMASingleBlock->u32TransLength/stDMABuildLliForBlock.
u32OneTimeTransLength + 1;
        }

        BSP_TRACE(BSP_LOG_LEVEL_DEBUG, BSP_MODU_DMAC,
                "\r BSP_DMA_SingleBlockStart:u32NodeNum= 0x%x\n",
                stDMABuildLliForBlock.u32NodeNum,0,0,0,0,0);
        stDMABuildLliForBlock.u32CtlDINC = u32CtlDINC;
        stDMABuildLliForBlock.u32CtlSINC = u32CtlSINC;
        stDMABuildLliForBlock.u32CtlDMS = u32CtlDMS;
        stDMABuildLliForBlock.u32CtlSMS = u32CtlSMS;
        stDMABuildLliForBlock.u32SrcBurstLength = u32SrcBurstLength;
        stDMABuildLliForBlock.u32DstBurstLength = u32DstBurstLength;

        pHeadLLI = DMA_LliBuildForSingleBlockTrans(stDMASingleBlock, stDMABuildLliForBlock);       

        BSP_TRACE(BSP_LOG_LEVEL_DEBUG, BSP_MODU_DMAC,
                "\r pHeadLLI= 0x%x\n",pHeadLLI,0,0,0,0,0);

        BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_LLP(u32Chan), ((BSP_U32)(pHeadLLI)>>2)<<2);

        /* 需要把CTL寄存器写为链端使能 */
        BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CTL(u32Chan), (DMA_LLI_ENABLE|DMA_ENABLE));
        
    }
    else
    {
        /* 计算块传输大小配置值*/
        u32BlockTS = stDMASingleBlock->u32TransLength/((BSP_U32)1<<u32SrcWidth);    
        /* 配置crl寄存器*/
        u32TransferCtrl_h = DMA_CTL_H_DEFAULT | u32BlockTS;
        u32TransferCtrl_l = DMA_CTL_L_DEFAULT | (u32SrcWidth << 0x4) | (u32DstWidth << 0x1)
            | (stDMASingleBlock->enTransType << 20) | (u32CtlDINC << 7) | (u32CtlSINC << 9)
            | (u32DstBurstLength << 11) | (u32SrcBurstLength << 14)
            | (u32CtlSMS << 25) | (u32CtlDMS << 23);

        BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_LLP(u32Chan), 0);

        /* 设置通道参数，直接写寄存器，就不调用函数了 */
        BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_SAR(u32Chan), stDMASingleBlock->u32SrcAddr);
        BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_DAR(u32Chan), stDMASingleBlock->u32DstAddr);
        BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CTL(u32Chan), u32TransferCtrl_l);
        /* modified for lint e665*/
        BSP_REG_WRITE(DMA_REGBASE_ADR, (DMA_CTL(u32Chan) + 4), u32TransferCtrl_h);
        /* modified for lint e665*/
    }
    /* 写CFG的值 */
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CFG(u32Chan), u32TransferCfg_l);
    /* modified for lint e665*/
    BSP_REG_WRITE(DMA_REGBASE_ADR, (DMA_CFG(u32Chan)+4), u32TransferCfg_h);
    /* modified for lint e665*/
    /* 使能DMA */
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_DMACFGREG, DMA_ENABLE);

    /* 使能通道 */
    BSP_REG_WRITE(DMA_REGBASE_ADR,DMA_CHENREG, (0x100<<(u32Chan))|(0x1<<(u32Chan)));

    return DMAC_SUCCESS;
}/*lint !e550*/


/*****************************************************************************
* 函 数 名  : BSP_DMA_LliBuild
*
* 功能描述  : 链表构建函数
*
* 输入参数  : BSP_U32 ulChan        通道号
*             DMA_LLI_S *pFirstLLI  资源链表的第一个节点指针
* 输出参数  : 无
* 返 回 值  : BSP_VOID *  指向底层建立好的传输链表的头节点的指针
*
* 修改记录  :2009年7月24日   吴振  创建
             Modified by liumengcun, 2010-1-12 与PS MSP讨论，刷cache均由使用者完成

*****************************************************************************/
BSP_VOID * BSP_DMA_LliBuild(BSP_U32 ulChan, DMA_LLI_S *pFirstLLI)
{
    BSP_U32 *pHeadLLI;/* 头节点*/
    DMA_LLI_PHY_S * pstDMALliPhy = NULL;/* 临时结构*/
    DMA_LLI_S *pLocalLLI;/* 循环变量*/
    BSP_U32 u32TempSrcWidth, u32TempDstWidth;/* 位宽临时变量*/
    BSP_U32 u32MemFlushLength = 0;/* 刷新memory长度*/
    BSP_U32 u32CtlSMS = DMA_MASTER1;/* 源DMA master选择*/
    BSP_U32 u32CtlDMS = DMA_MASTER1;/* 目的DMA master选择*/
    BSP_U32 i = 0;
    
    /* 是否初始化判断*/
    if(DMA_NOTINITIALIZE == g_u32DmaInit)
    {
    	BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
    			"\rNot initialized! \n",0,0,0,0,0,0);
    	return NULL;
    }

    /* 参数检查*/
	if((DMA_MAX_Index <= ulChan) || (NULL == pFirstLLI))
	{
		BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
			"\r BSP_DMA_LliBuild Input Para Error! ulChan = 0x%x,pFirstLLI = 0x%x\n",
			ulChan,pFirstLLI,0,0,0,0);
	    return NULL;
	}

    pHeadLLI = g_pu32PhyAddrPointer[ulChan];
    //u32MemFlushLength = sizeof(DMA_LLI_PHY_S)*DMA_NODE_NUM_NORMAL;

    /* 主核用master1 从核用master2 */
#ifdef PRODUCT_CFG_CORE_TYPE_MODEM 
    u32CtlSMS = DMA_MASTER1;
    u32CtlDMS = DMA_MASTER1;
#else         
    u32CtlSMS = DMA_MASTER2;
    u32CtlDMS = DMA_MASTER2;
#endif                

    pstDMALliPhy = (DMA_LLI_PHY_S *)pHeadLLI;/*lint !e740 !e826*/
    /* 循环配置节点的数据 */
    for(pLocalLLI = pFirstLLI; NULL != pLocalLLI; pLocalLLI = pLocalLLI->pNextLLI)
    {
        if(i >= DMA_NODE_NUM_NORMAL)
        {
            BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC, "\r  Node Number is bigger than DMA_NODE_NUM_NORMAL\n",0,0,0,0,0,0);
    		return NULL;
        }
        /* 链表构造*/
    	pstDMALliPhy->SrcAddr = pLocalLLI->SrcAddr;

        /* 计算源位宽*/
    	DMA_SetTransWidth(pLocalLLI->SrcAddr, pLocalLLI->TransLength, &u32TempSrcWidth);

        DMA_SetTransWidth(pLocalLLI->DstAddr, pLocalLLI->TransLength, &u32TempDstWidth);
	    pstDMALliPhy->DstAddr = pLocalLLI->DstAddr;

    	/* 链表传输中不支持大于DMA单块传输最大长度的情况*/
    	if(((DMA_MAX_SINGLE_BLOCK_TRANS_LENGTH*((BSP_U32)0x1<<u32TempSrcWidth)) < pLocalLLI->TransLength)
    	    ||(0 ==pLocalLLI->TransLength))
    	{
    		BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC, "\r The transferlength of the node is not valid \n",0,0,0,0,0,0);
    		return NULL;
    	}
    	pstDMALliPhy->NextLLI = (BSP_U32)DMAC_VIRT_TO_PHYS(pstDMALliPhy + 1);
    	pstDMALliPhy->ulTransferCtrl_l = DMA_CTL_L_DEFAULT | DMA_LLI_ENABLE | (u32TempSrcWidth<<0x4) 
                                        | (u32TempDstWidth<<0x1)| (u32CtlSMS << 25) | (u32CtlDMS << 23)
                                        | (DMA_MSIZE_16 << 11) | (DMA_MSIZE_16 << 14);
    	pstDMALliPhy->ulTransferCtrl_h = DMA_CTL_H_DEFAULT | (pLocalLLI->TransLength / ((BSP_U32)0x1<<u32TempSrcWidth));
    	pstDMALliPhy->s_stat = 0;
    	pstDMALliPhy->d_stat = 0;

        pstDMALliPhy++;
        i++;
    }
    /* 最后一个节点的next赋0*/
    ((DMA_LLI_PHY_S*)(pstDMALliPhy -1))->NextLLI = 0;

    /* 最后一个节点，去使能源和目的链表使能位 */
    ((DMA_LLI_PHY_S*)(pstDMALliPhy -1))->ulTransferCtrl_l = DMA_CTL_L_DEFAULT | 0x0 | (u32TempSrcWidth<<0x4)
                                                            |(u32TempDstWidth<<0x1)| (u32CtlSMS << 25) | (u32CtlDMS << 23)
                                                            |(DMA_MSIZE_16 << 11) | (DMA_MSIZE_16 << 14);
    u32MemFlushLength = sizeof(DMA_LLI_PHY_S)*i;

    DMAC_CACHE_FLUSH(pHeadLLI, u32MemFlushLength);

    return (BSP_VOID *)DMAC_VIRT_TO_PHYS(pHeadLLI);

}

/*****************************************************************************
* 函 数 名  : BSP_DMA_LliStart
*
* 功能描述  : 链表数据传输启动专用函数
*
* 输入参数  : DMA_CHN_PARA_S stChnPara    传输通道参数结构
*             BSP_VOID *pu32Head：   调用BSP_DMA_LliBuild获得的待传输链表头指针
* 输出参数  : 无
* 返 回 值  : DMAC_SUCCESS       操作成功
*             BSP_ERR_DMA_MODULE_NOT_INITED      DMA未初始化
*             BSP_ERR_DMA_INVALID_PARA  参数错误
*
* 修改记录  :2008年12月26日   吴振  创建

*****************************************************************************/
BSP_U32 BSP_DMA_LliStart(DMA_CHN_PARA_S * stChnPara, BSP_VOID *pu32Head)
{
    BSP_U32 *pHeadLLI = NULL;
    BSP_U32 u32CtlTemp = 0;
    BSP_U32 u32TransferCfg_h = DMA_CFG_H_DEFAULT;
    BSP_U32 u32TransferCfg_l = DMA_CFG_L_DEFAULT;
    BSP_U32 u32LockKey = 0;
    BSP_U32 u32MAX_ABRST = DMA_BURST_LEN16;

    pHeadLLI = pu32Head;

    /* 是否初始化判断*/
    if(DMA_NOTINITIALIZE == g_u32DmaInit)
    {
    	BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
    	"\rNot initialized! \n",0,0,0,0,0,0);
    	return BSP_ERR_DMA_MODULE_NOT_INITED;
    }

    /* 参数检查*/
	if((DMA_MAX_Index <= stChnPara->u32Chan) || (NULL == pu32Head))
	{
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
            "\r BSP_DMA_LliStart Input Para Error! ulChan = 0x%x,pu32Head = 0x%x\n",
            stChnPara->u32Chan,pu32Head,0,0,0,0);
	    return BSP_ERR_DMA_INVALID_PARA;
	}

    DMAC_INT_LOCK(u32LockKey);
    /* 读通道使能寄存器ChEnReg[CH_EN]，判断通道是否空闲。为0空闲 */
    if(DMAC_SUCCESS != DMA_CheckChannelBusy(stChnPara->u32Chan))
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
            "\r BSP_DMA_LliStart:The Channel is busy! \n",0,0,0,0,0,0);

        DMAC_INT_UNLOCK(u32LockKey);
        return BSP_ERR_DMA_CHANNEL_BUSY;
    }

    DMAC_INT_LOCK(u32LockKey);

    u32TransferCfg_l = DMA_CFG_L_DEFAULT|(DMA_SW_HANDSHAKE<<11)|(DMA_SW_HANDSHAKE<<10)|(u32MAX_ABRST<<20); 

    /* 注册完成中断处理函数*/
    g_stChannelInfo[stChnPara->u32Chan].pfuncDmaTcIsr = stChnPara->pfuncDmaTcIsr;
    g_stChannelInfo[stChnPara->u32Chan].s32DmaTcPara = stChnPara->s32DmaTcPara;

    /* 注册错误中断处理函数*/
    g_stChannelInfo[stChnPara->u32Chan].pfuncDmaErrIsr = stChnPara->pfuncDmaErrIsr;
    g_stChannelInfo[stChnPara->u32Chan].s32DmaErrPara = stChnPara->s32DmaErrPara;

    /* 清除通道在前一次DMA 传送中的所有中断。 */
    (BSP_VOID)DMA_ClearAllIntStatus(stChnPara->u32Chan);

    /* 设置寄存器LLP 为LLP0 的值，指向第一个LLI 的指针
    * 为链表的第一个节点的sar 的地址
    */
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_LLP(stChnPara->u32Chan), ((BSP_U32)(pHeadLLI)>>2)<<2);

    /* 需要把CTL寄存器写为链端使能
    BSP_REG_READ(DMA_REGBASE_ADR, DMA_CTL(u32Chan), u32CtlTemp);*/
    u32CtlTemp |= 0x18000001;
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CTL(stChnPara->u32Chan), u32CtlTemp);

    /* 写CFG的值 */
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CFG(stChnPara->u32Chan), u32TransferCfg_l);
    /* modified for lint e665*/
    BSP_REG_WRITE(DMA_REGBASE_ADR, (DMA_CFG(stChnPara->u32Chan)+4), u32TransferCfg_h);
    /* modified for lint e665*/

    /* 使能DMA */
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_DMACFGREG, DMA_ENABLE);

    /* 使能通道 */
    BSP_REG_WRITE(DMA_REGBASE_ADR,DMA_CHENREG, (0x100<<(stChnPara->u32Chan))|(0x1<<(stChnPara->u32Chan)));

    return DMAC_SUCCESS;
}/*lint !e818 !e550 !e529*/

/*****************************************************************************
* 函 数 名  : BSP_DMA_GetDar
*
* 功能描述  : 外设使用该函数获取DMA的DAR寄存器的值
*
* 输入参数  : BSP_U32 u32Chan   通道号
*
* 输出参数  : BSP_U32 *u32Dar  该通道对应的目的地址寄存器的值
* 返 回 值  : DMAC_SUCCESS       获取成功
*             BSP_ERR_DMA_MODULE_NOT_INITED      DMA未初始化
*             BSP_ERR_DMA_INVALID_PARA  参数错误
*
* 修改记录  :2010年12月1日   鲁婷  创建

*****************************************************************************/
BSP_S32 BSP_DMA_GetDar(BSP_U32 u32Chan, BSP_U32 *u32Dar)
{
    BSP_U32 u32ADR = 0;
    
    /* 是否初始化判断*/
	if(DMA_NOTINITIALIZE == g_u32DmaInit)
	{
		BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
				"\rBSP_DMA_GetIdleChannel Not initialized! \n",0,0,0,0,0,0);
		return BSP_ERR_DMA_MODULE_NOT_INITED;
	}

    /* 参数检查*/
    if ((u32Chan >= DMA_MAX_Index) || (NULL == u32Dar))
    {
    	BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
    					"\rBSP_DMA_GetIdleChannel NULL == pulChan! \n",0,0,0,0,0,0);
        return BSP_ERR_DMA_INVALID_PARA;
    }

    BSP_REG_READ(DMA_REGBASE_ADR, DMA_DAR(u32Chan), u32ADR);
    
    *u32Dar = u32ADR;

    return DMAC_SUCCESS;
}

/*****************************************************************************
* 函 数 名  : BSP_DMA_DisableChannel
*
* 功能描述  : 外设使用该函数去使能DMAC通道
*
* 输入参数  : BSP_U32 u32Chan   通道号
*
* 输出参数  : 无
* 返 回 值  : DMAC_SUCCESS       获取成功
*             BSP_ERR_DMA_MODULE_NOT_INITED      DMA未初始化
*             BSP_ERR_DMA_INVALID_PARA  参数错误
*
* 修改记录  :2010年12月1日   鲁婷  创建

*****************************************************************************/
BSP_S32 BSP_DMA_DisableChannel(BSP_U32 u32Chan)
{
    /* 是否初始化判断*/
	if(DMA_NOTINITIALIZE == g_u32DmaInit)
	{
		BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
				"\rBSP_DMA_GetIdleChannel Not initialized! \n",0,0,0,0,0,0);
		return BSP_ERR_DMA_MODULE_NOT_INITED;
	}

    /* 参数检查*/
    if (u32Chan >= DMA_MAX_Index)
    {
    	BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
    					"\rBSP_DMA_GetIdleChannel NULL == pulChan! \n",0,0,0,0,0,0);
        return BSP_ERR_DMA_INVALID_PARA;
    }    
    
    BSP_REG_WRITE(DMA_REGBASE_ADR,DMA_CHENREG, (0x100<<(u32Chan))|(0x0<<(u32Chan)));
    g_stChannelInfo[u32Chan].u32ChnUsrStatus = DMA_CHN_IDLE;

    return DMAC_SUCCESS;

}

/* DMAC可维可测 */
/*****************************************************************************
* 函 数 名  : DMAC_GetRegistValue
*
* 功能描述  : 获取DMAC模块寄存器的值
*
* 输入参数  : BSP_VOID
* 输出参数  : NA
*
* 返 回 值  : NA
*
* 其它说明  : 可维可测接口函数
*
*****************************************************************************/
BSP_VOID DMAC_GetRegistValue()
{
    BSP_U32 channelinfo;
    BSP_U32 u32ChannelStatus;
    BSP_U32 u32ChannelTCStatus;
    BSP_U32 u32ChannelErrStatus;

    BSP_REG_READ(DMA_REGBASE_ADR,DMA_CHENREG, channelinfo);
    BSP_REG_READ(DMA_REGBASE_ADR,DMA_STATUSINT, u32ChannelStatus);
    BSP_REG_READ(DMA_REGBASE_ADR,DMA_STATUSTFR, u32ChannelTCStatus);
    BSP_REG_READ(DMA_REGBASE_ADR,DMA_STATUSERR, u32ChannelErrStatus);
    channelinfo= channelinfo&0x00ff;

    DMAC_PRINT("=============== DMAC registers value: =============\n");
    DMAC_PRINT("通道使能寄存器         DMA_CHENREG:   0x%08x\n", channelinfo);
    DMAC_PRINT("组合中断状态寄存器     DMA_STATUSINT: 0x%08x\n", u32ChannelStatus);
    DMAC_PRINT("IntTfr 中断状态        DMA_STATUSTFR: 0x%08x\n", u32ChannelTCStatus);
    DMAC_PRINT("IntErr 中断状态        DMA_STATUSERR: 0x%08x\n", u32ChannelErrStatus);
    DMAC_PRINT("===================================================\n\n");
}

/*****************************************************************************
* 函 数 名  : DMAC_GetChannelInfo
*
* 功能描述  : 获取DMAC模块通道信息
*
* 输入参数  : BSP_U32
* 输出参数  : NA
*
* 返 回 值  : NA
*
* 其它说明  : 可维可测接口函数
*
*****************************************************************************/
BSP_VOID DMAC_GetChannelInfo(BSP_U32 u32Chan)
{
    if((u32Chan>=DMA_MAX_CHANNELS)||(u32Chan<0))
    {
        DMAC_PRINT("输入通道号非法！\n");
        return;
    }

    DMAC_PRINT("============= DMAC_GetChannelInfo ============\n");
    DMAC_PRINT("channel逻辑状态      u32ChnUsrStatus = %u\n", g_stChannelInfo[u32Chan].u32ChnUsrStatus);
    DMAC_PRINT("通道属性             u32ChnAttribute = %u\n", g_stChannelInfo[u32Chan].u32ChnAttribute);
    DMAC_PRINT("==============================================\n\n");
}

#ifdef __KERNEL__
module_init(BSP_DMA_Init);
EXPORT_SYMBOL(BSP_DMA_GetIdleChannel);
EXPORT_SYMBOL(BSP_DMA_ChannelRelease);
EXPORT_SYMBOL(BSP_DMA_AllChannelCheck);
EXPORT_SYMBOL(BSP_DMA_OneChannelCheck);
EXPORT_SYMBOL(BSP_DMA_SingleBlockStart);
EXPORT_SYMBOL(BSP_DMA_LliBuild);
EXPORT_SYMBOL(BSP_DMA_LliStart);
EXPORT_SYMBOL(BSP_DMA_GetDar);
EXPORT_SYMBOL(BSP_DMA_DisableChannel);
EXPORT_SYMBOL(DMAC_GetRegistValue);
EXPORT_SYMBOL(DMAC_GetChannelInfo);
#endif

#ifdef __cplusplus
}
#endif


