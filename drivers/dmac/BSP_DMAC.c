/*************************************************************************
*   ��Ȩ����(C) 1987-2010, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  dmaDrv.c
*
*   ��    �� :  wuzhen
*
*   ��    �� :   ���ļ���Ҫ��ɶ�DMA���䷽ʽ��֧�֣�֧��LTE��Ŀ�е��ڴ���ڴ�
                 �Լ��ڴ�������е�DMA���ݴ��䣬��ҪӦ�ó���Ϊ�ڴ浽�ڴ�����ݰ��ƣ�����V400���������ֲ�д����
                 ��16ͨ������
*
*   �޸ļ�¼ :  2008��12��26��  v1.00  wuzhen  ����
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

/* �ײ�ʹ�õĽ������������ƹ�ҵ�ַ�ռ�ָ�� for ps ͨ��0*/
/* ʣ��3��������ͨͨ��*/
BSP_U32 *g_pu32PhyAddrPointer[DMA_MAX_CHANNELS] = {0};

BSP_U32 g_u32DmaInit = DMA_NOTINITIALIZE;

/* DMAͨ�������Ϣ*/
DMA_CHN_INFO_S g_stChannelInfo[DMA_MAX_CHANNELS];

/*****************************************************************************
* �� �� ��  : DMA_CheckChannelBusy
*
* ��������  : �ж�ͨ���Ƿ����
*
* �������  : u32Channel�����жϵ�ͨ����
* �������  : ��
* �� �� ֵ  : DMAC_SUCCESS       ͨ������
*             BSP_ERR_DMA_CHANNEL_BUSY  ͨ��æ
*             BSP_ERR_DMA_INVALID_PARA  ��������
*
* �޸ļ�¼  :2009��06��20��   ����  ����

*****************************************************************************/
BSP_S32 DMA_CheckChannelBusy(BSP_U32 u32Channel)
{
    BSP_U32 u32Result = 0;

    /* �������*/
    if(DMA_MAX_Index <= u32Channel)
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
        "\r DMA_CheckChannelBusy : Input Para Error u32Channel = %d! \n",u32Channel,2,3,4,5,6);
        return BSP_ERR_DMA_INVALID_PARA;
    }
    /* ��ȡ�Ĵ����ж�ͨ���Ƿ�æ*/
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
* �� �� ��  : DMA_ClearIntStatus
*
* ��������  : ���ͨ���ж�״̬
*
* �������  : u32Chan����������ͨ����
*           DMA_INT_TYPE_NUM inttype:��������ж�����
*
* �������  : ��
* �� �� ֵ  : BSP_ERR_DMA_INVALID_PARA����������
*          DMAC_SUCCESS�����ʧ��
*
* �޸ļ�¼  :2009��06��20��   ����  ����

*****************************************************************************/
BSP_S32 DMA_ClearIntStatus(BSP_U32 u32Chan, DMA_INT_TYPE_NUM_E enIntType)
{
    /* �������*/
    if((DMA_MAX_Index <= u32Chan)||(INT_BUTTOM_BIT <= enIntType))
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
        		"\r DMA_ClearIntStatus : Input Para Error! \n",1,2,3,4,5,6);
        return BSP_ERR_DMA_INVALID_PARA;
    }

    /* ���ж�*/
    /* modified for lint e665 */
    BSP_REG_WRITE(DMA_REGBASE_ADR,(DMA_CLEAR_INT_BASE + enIntType * DMA_INT_ADDRESS_SPAN), DMA_HIGH_LEVEL<<u32Chan);
    /* modified for lint e665 */
    
    return DMAC_SUCCESS;
}

/*****************************************************************************
* �� �� ��  : DMA_ClearAllIntStatus
*
* ��������  : ���ĳͨ�������ж�״̬
*
* �������  : u32Chan����������ͨ����
* �������  : ��
* �� �� ֵ  : BSP_ERR_DMA_INVALID_PARA����������
*          DMAC_SUCCESS�����ʧ��
*
* �޸ļ�¼  :2009��06��20��   ����  ����

*****************************************************************************/
BSP_S32 DMA_ClearAllIntStatus(BSP_U32 u32Chan)
{   
    /* �������*/
    if(DMA_MAX_Index <= u32Chan)
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
        		"\r DMA_ClearAllIntStatus : Input Para Error! \n",1,2,3,4,5,6);
        return BSP_ERR_DMA_INVALID_PARA;
    }

    /* ��5���ж�*/
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CLEARTFR, DMA_HIGH_LEVEL<<u32Chan);
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CLEARBLOCK, DMA_HIGH_LEVEL<<u32Chan);
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CLEARSRCTRAN, DMA_HIGH_LEVEL<<u32Chan);
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CLEARDSTTRAN, DMA_HIGH_LEVEL<<u32Chan);
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CLEARERR, DMA_HIGH_LEVEL<<u32Chan);

    return DMAC_SUCCESS;
}

/*****************************************************************************
* �� �� ��  : DMA_SetTransWidth
*
* ��������  : ���ݴ���Ĵ����ַ��ȷ������λ��
*
* �������  : BSP_U32 u32TransAddr��������ĵ�ַ
              BSP_U32 u32TransLength��������ĳ���
* �������  : BSP_U32 *u32TransWidth���洢����λ���ֵ�ĵ�ַ
* �� �� ֵ  : ��
*
* �޸ļ�¼  :2009��06��20��   ����  ����

*****************************************************************************/
BSP_VOID DMA_SetTransWidth(BSP_U32 u32TransAddr, BSP_U32 u32TransLength, BSP_U32 *u32TransWidth)/*lint -e715*/
{
    /* �ڲ����ã���ַ���ᳬ���ڴ淶Χ����β���Ϊ��ָ�� */
    /* ĿǰIP������֧�ֵ�64bitλ��*/
    if((0 == u32TransLength%8)&&(u32TransLength >=8))
    {
        *u32TransWidth = DMA_TRANS_WIDTH_64; /* b011����64λλ�� */
    }
    /* ��ַ���ֽڶ��룬ͬʱ���䳤��Ϊ4�ֽ�������*/
    else if((0 == u32TransLength%4)&&(u32TransLength >=4))
    {
        *u32TransWidth = DMA_TRANS_WIDTH_32; /* b010����32λλ�� */
    }
    /* ��ַ���ߴ��䳤��Ϊ����*/
    else if(1 == u32TransLength%2)
    {
        *u32TransWidth = DMA_TRANS_WIDTH_8; /* b000����8λλ�� */
    }
    /* ʣ�µ�3�������������2�ֽ�λ���� */
    else 
    {
        *u32TransWidth = DMA_TRANS_WIDTH_16; /* b001����16λλ�� */
    }
    return;
}

/*****************************************************************************
* �� �� ��  : DMA_IntHandler
*
* ��������  : DMA�жϻص�����
*
* �������  : ��
* �������  : ��
* �� �� ֵ  : OK
*
* �޸ļ�¼  :2008��12��26��   ����  ����

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

    /* ���ж�*/
    BSP_REG_WRITE(DMA_REGBASE_ADR,DMA_CLEARTFR, u32ChannelTCStatus&(0x01 << Chan));
    BSP_REG_WRITE(DMA_REGBASE_ADR,DMA_CLEARERR, u32ChannelErrStatus&(0x01 << Chan));

    /* decide which channel has trigger the interrupt*/
    if(u32ChannelStatus & 0x1)/* if there is a tfr int */
    {
        if((u32ChannelTCStatus >> Chan) & 0x01)
        {
            /* ֻ����ͨͨ���Ż��ͷ�*/
            if(DMA_COMMON_MODE == g_stChannelInfo[Chan].u32ChnAttribute)
            {
                /* ��ͨ���߼�״̬��ΪIDLE*/
                g_stChannelInfo[Chan].u32ChnUsrStatus= DMA_CHN_IDLE;
            }
                /* ����ע����жϴ�����*/
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
            /* ֻ����ͨͨ���Ż��ͷ�*/
            if(DMA_COMMON_MODE == g_stChannelInfo[Chan].u32ChnAttribute)
            {
                /* ��ͨ���߼�״̬��ΪIDLE*/
                g_stChannelInfo[Chan].u32ChnUsrStatus = DMA_CHN_IDLE;
            }
            /* ����ע��Ĵ����жϴ�����*/
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
* �� �� ��  : DMA_LliBuildForSingleBlockTrans
*
* ��������  : ���鴫�䳬��ʱ���Ϊ����Ĵ�����
*
* �������  : DMA_SINGLE_BLOCK_S stDMASingleBlock              ���鴫������ṹ
*             DMA_BUILD_LLI_FOR_BLOCK_S stDMABuildLliForBlock  ������������ṹ
* �������  : ��
*
* �� �� ֵ  : ͷ�ڵ�
*
* ����˵��  : Modified by liumengcun, 2009-11-13 ��PS MSP���ۣ�ˢcache����ʹ�������
*
*****************************************************************************/
BSP_VOID * DMA_LliBuildForSingleBlockTrans(DMA_SINGLE_BLOCK_S * stDMASingleBlock,
    DMA_BUILD_LLI_FOR_BLOCK_S stDMABuildLliForBlock)
{
    DMA_LLI_PHY_S * pstDMALliPhy = NULL;/* ��ʱ�ṹ*/
    BSP_U32 i;
    BSP_U32 u32LengthTmp = 0;
    BSP_U32 u32TransferBytes = 0; /* ÿ�δ����С*/
    BSP_U32 u32Chan = 0;

    BSP_U32 u32SrcWidth = 0;/* Դ����λ��*/
    BSP_U32 u32DstWidth = 0;/* Ŀ�Ĵ���λ��*/
    BSP_U32 u32MemFlushLength = 0;/* ˢ��memory����*/

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
    /* ѭ�����ýڵ������ */
    for(i = 0; i < stDMABuildLliForBlock.u32NodeNum; i++)
    {
        if (i < stDMABuildLliForBlock.u32NodeNum - 1)
        {
            switch(stDMASingleBlock->enTransType)
            {
                case MEM2MEM_DMA:
                {
                    /* memory��memory�������Ҫ���¼���Դ��ַ��Ŀ�ĵ�ַ*/
                    pstDMALliPhy->SrcAddr = stDMASingleBlock->u32SrcAddr + i*stDMABuildLliForBlock.u32OneTimeTransLength;
                    pstDMALliPhy->DstAddr = stDMASingleBlock->u32DstAddr + i*stDMABuildLliForBlock.u32OneTimeTransLength;

                    break;
                }
                case MEM2PRF_DMA:
                case MEM2PRF_PRF:
                {
                    pstDMALliPhy->DstAddr = stDMASingleBlock->u32DstAddr;
                    /* memory�������������Ҫ���¼���Դ��ַ*/
                    pstDMALliPhy->SrcAddr = stDMASingleBlock->u32SrcAddr + i*stDMABuildLliForBlock.u32OneTimeTransLength;

                    u32DstWidth = stDMASingleBlock->enDstWidth;
                    break;
                }
                case PRF2MEM_DMA:
                case PRF2MEM_PRF:
                {
                    pstDMALliPhy->SrcAddr = stDMASingleBlock->u32SrcAddr;
                    /* ���赽memory�������Ҫ���¼���Ŀ�ĵ�ַ*/
                    pstDMALliPhy->DstAddr = stDMASingleBlock->u32DstAddr + i*stDMABuildLliForBlock.u32OneTimeTransLength;

                    u32SrcWidth = stDMASingleBlock->enSrcWidth;
                    break;
                }
                case PRF2PRF_SRCPRF:
                case PRF2PRF_DSTPRF:
                {
                    /* ���赽��������²���Ҫ���¼���Դ��ַ��Ŀ�ĵ�ַ*/
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

                    /* memory��memory�������Ҫ���¼���Դ��ַ��Ŀ�ĵ�ַ*/

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
                    /* memory�������������Ҫ���¼���Դ��ַ*/
                    pstDMALliPhy->SrcAddr = stDMASingleBlock->u32SrcAddr + i*stDMABuildLliForBlock.u32OneTimeTransLength;
                    
                    DMA_SetTransWidth(pstDMALliPhy->SrcAddr, u32LengthTmp, &u32SrcWidth);
                    u32DstWidth = stDMASingleBlock->enDstWidth;
                    break;
                }
                case PRF2MEM_DMA:
                case PRF2MEM_PRF:
                {
                    pstDMALliPhy->SrcAddr = stDMASingleBlock->u32SrcAddr;
                    /* ���赽memory�������Ҫ���¼���Ŀ�ĵ�ַ*/                    
                    pstDMALliPhy->DstAddr = stDMASingleBlock->u32DstAddr + i*stDMABuildLliForBlock.u32OneTimeTransLength;

                    u32SrcWidth = stDMASingleBlock->enSrcWidth;
                    DMA_SetTransWidth(pstDMALliPhy->DstAddr, u32LengthTmp, &u32DstWidth);
                    break;
                }
                case PRF2PRF_SRCPRF:
                case PRF2PRF_DSTPRF:
                {
                    /* ���赽��������²���Ҫ���¼���Դ��ַ��Ŀ�ĵ�ַ*/
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

    /* ���һ���ڵ��next��0*/
    ((DMA_LLI_PHY_S*)(pstDMALliPhy -1))->NextLLI = 0;

    /* ���һ���ڵ㣬ȥʹ��Դ��Ŀ������ʹ��λ */
    ((DMA_LLI_PHY_S*)(pstDMALliPhy -1))->ulTransferCtrl_l = DMA_CTL_L_DEFAULT | 0x0 | (u32SrcWidth<<0x4) | (u32DstWidth<<0x1)
        | (stDMASingleBlock->enTransType << 20) | (stDMABuildLliForBlock.u32CtlDINC << 7)
        | (stDMABuildLliForBlock.u32CtlSINC << 9)
        | (stDMABuildLliForBlock.u32DstBurstLength << 11) | (stDMABuildLliForBlock.u32SrcBurstLength << 14)
        | (stDMABuildLliForBlock.u32CtlSMS << 25) | (stDMABuildLliForBlock.u32CtlDMS << 23);
    /* ˢ��cache��ARM��32λcache line����Ҫ��32λΪ��λˢ�� ���費��Ҫˢcache*/

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
* �� �� ��  : BSP_DMA_Init
*
* ��������  : DMA��ʼ��
*
* �������  : ��
* �������  : ��
* �� �� ֵ  : DMAC_SUCCESS           ��ʼ���ɹ�
              BSP_ERR_DMA_NULL_PTR  ������Դʧ��
*
* �޸ļ�¼  : 2011��5��13��   ����  ����
              2011��5��13��   ³��  �޸�

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

    /* ��ʼ��*/
    memset(g_stChannelInfo, 0x0, sizeof(g_stChannelInfo));
    
    /*lint -e685 -e568 -e681*/
    for(i = DMA_MIN_Index; i < (DMA_MIN_Index+DMA_SPECIAL_MODE_NUM); i++)
    {
        g_stChannelInfo[i].u32ChnAttribute = DMA_SPECIAL_MODE;
        g_stChannelInfo[i].u32ChnUsrStatus = DMA_CHN_ALLOCED;
    }
   
    /* ÿ��ͨ������Դ�ڵ�ֱ������ò��ͷţ���Ϊÿ��ͨ���ҽ��ж� */
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

    /* ���ж�*/
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CLEARTFR, DMA_INT_CLEAR);
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CLEARERR, DMA_INT_CLEAR);
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CLEARBLOCK, DMA_INT_CLEAR);
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CLEARSRCTRAN, DMA_INT_CLEAR);
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CLEARDSTTRAN, DMA_INT_CLEAR);

        /* �򿪴�����ɺʹ�������жϣ������ж���ʱ���� */
        BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_MASKTFR, DMA_INT_MASK_WE|DMA_INT_UNMASK);
        BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_MASKERR, DMA_INT_MASK_WE|DMA_ERRINT_UNMASK);

    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_MASKBLOCK, DMA_INT_MASK_WE|DMA_INT_MASK);
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_MASKSRCTRAN, DMA_INT_MASK_WE|DMA_INT_MASK);
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_MASKDSTTRAN, DMA_INT_MASK_WE|DMA_INT_MASK);
	/* �ر�DMAʱ�� */
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
* �� �� ��  : BSP_DMA_GetIdleChannel
*
* ��������  : ��ȡһ����ǰ���ڿ���״̬��DMAͨ������ǰ����������������ݺ��
*             �Զ��ͷţ��´���ʹ��ʱ���ٻ�ȡ��ǰ����ͨ��,ĿǰDMA_SPECIAL_MODE
*             ����Э��ջʹ�ã������û����벻������ͨ��
*
* �������  : DMA_CHN_TYPE_E enChnType ͨ������
*
* �������  : BSP_U32 *pulChan      ������뵽��ͨ����
* �� �� ֵ  : DMAC_SUCCESS       �����ɹ�
*             BSP_ERR_DMA_MODULE_NOT_INITED      DMAδ��ʼ��
*             BSP_ERR_DMA_INVALID_PARA  ��������
*             BSP_ERR_DMA_ALL_CHANNEL_BUSY ����ͨ��æ
*
* �޸ļ�¼  :2009��10��15��   liumengcun  ����

*****************************************************************************/
BSP_S32  BSP_DMA_GetIdleChannel(DMA_CHN_TYPE_E enChnType, BSP_U32 *pulChan)
{
	BSP_U32 channelinfo;
	BSP_U32 lockKey = 0;
    BSP_U32 i;

    /* �Ƿ��ʼ���ж�*/
	if(DMA_NOTINITIALIZE == g_u32DmaInit)
	{
		BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
				"\rBSP_DMA_GetIdleChannel Not initialized! \n",0,0,0,0,0,0);
		return BSP_ERR_DMA_MODULE_NOT_INITED;
	}

    /* �������*/
    if ((NULL == pulChan) || (enChnType > DMA_COMMON_MODE))
    {
    	BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
    					"\rBSP_DMA_GetIdleChannel NULL == pulChan! \n",0,0,0,0,0,0);
        return BSP_ERR_DMA_INVALID_PARA;
    }

    *pulChan = 0xffffffff;

	/* ��DMAʱ�� */
	BSP_PWRCTRL_SoftClkEn(PWRCTRL_MODU_DMAC);

    DMAC_INT_LOCK(lockKey);

    /***** get the current channel information*/
    BSP_REG_READ(DMA_REGBASE_ADR,DMA_CHENREG, channelinfo);
    channelinfo= channelinfo&0x00ff;

    /***** according the input parameter,decide the channel number */
    for (i = DMA_MIN_Index; i < DMA_MAX_Index; i++)
    {
         /* ��ͨ������״̬���߼�״̬��Ϊ����ʱ�ſ���ʹ��*/
         if ((DMA_CHN_IDLE == g_stChannelInfo[i].u32ChnUsrStatus)
             && DMA_SPECIAL_MODE != g_stChannelInfo[i].u32ChnAttribute)
         {
              /*clear the interrupt in this channel that has not deal with*/
       	      BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CLEARTFR, (0x01 << i));
       	      BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CLEARERR, (0x01 << i));
              *pulChan = i;

              /* ͨ���߼�״̬��Ϊbusy*/
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
* �� �� ��  : BSP_DMA_ChannelRelease
*
* ��������  : DMA����ͨ���ͷ�,��������²���Ҫ���ô˽ӿ��ͷ�ͨ����������ɺ�
*             ������Զ��ͷ�ͨ�������ӿ�Ϊ�˴���ĳЩ�쳣����£�ȡ����ͨ����
*             ��ȴû�п�ʼ���ݴ��䣬�޷��Զ��ͷ�ͨ����ʱ����ô˽ӿ��ֶ��ͷ�ͨ��
*             ͨ�����ڴ���ʱ�޷��ͷ�ͨ����ͨ��0��Э��ջ�̶�ʹ���޷��ͷš�
*
* �������  : BSP_U32 ulChan  ͨ����
* �������  : ��
* �� �� ֵ  : DMAC_SUCCESS       �ͷųɹ�
*             BSP_ERR_DMA_MODULE_NOT_INITED      DMAδ��ʼ��
*             BSP_ERR_DMA_INVALID_PARA  ��������
*             BSP_ERR_DMA_CHANNEL_BUSY  ͨ��æ
*
* �޸ļ�¼  :2008��12��26��   ����  ����

*****************************************************************************/
BSP_U32  BSP_DMA_ChannelRelease(BSP_U32 u32Chan)
{
    BSP_U32 channelinfo = 0;
    /* �Ƿ��ʼ���ж�*/
	if(DMA_NOTINITIALIZE == g_u32DmaInit)
	{
		BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
				"\r Not initialized! \n",0,0,0,0,0,0);
		return BSP_ERR_DMA_MODULE_NOT_INITED;
	}
	/* �������*/
    if(u32Chan >= DMA_MAX_Index)
    {
      	 BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
				"\r BSP_DMA_ChannelRelease u32Chan = 0x%x! \n",
				u32Chan,0,0,0,0,0);
         return BSP_ERR_DMA_INVALID_PARA;
    }

    /* �������*/
    if(DMA_SPECIAL_MODE  ==  g_stChannelInfo[u32Chan].u32ChnAttribute)
    {
      	 BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
				"\r BSP_DMA_ChannelRelease u32Chan = 0x%x,g_stChannelInfo[u32Chan].u32ChnAttribute = 0x%x! \n",
				u32Chan,g_stChannelInfo[u32Chan].u32ChnAttribute,0,0,0,0);
         return BSP_ERR_DMA_INVALID_PARA;
    }

    /* ͨ�����ڴ�������ʱ�޷��ͷ�ͨ��*/
    BSP_REG_READ(DMA_REGBASE_ADR,DMA_CHENREG, channelinfo);
    if(channelinfo & (0x1<<u32Chan))
    {
    	BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC, 
    					"\r BSP_DMA_ChannelRelease channel is busy u32Chan = 0x%x! \n",u32Chan,0,0,0,0,0);
        return BSP_ERR_DMA_CHANNEL_BUSY;
    }
    /* �ͷ�ͨ���߼�״̬*/
    g_stChannelInfo[u32Chan].u32ChnUsrStatus= DMA_CHN_IDLE;

    return DMAC_SUCCESS;
}

/*****************************************************************************
* �� �� ��  : BSP_DMA_AllChannelCheck
*
* ��������  : DMAȫ��ͨ���Ƿ���м�⺯��
*
* �������  : ��
* �������  : pChannelInfo   ����ͨ��ʹ��״̬����bit�����ʾ�����ڴ������ݵ�ͨ��bitλ��1��
*               bit0��Ӧ0ͨ����bit1��Ӧ1ͨ�����Դ�����
* �� �� ֵ  : DMAC_SUCCESS       �����ɹ�
*             BSP_ERR_DMA_MODULE_NOT_INITED      DMAδ��ʼ��
*             BSP_ERR_DMA_INVALID_PARA  ��������
* �޸ļ�¼  :2008��12��26��   ����  ����

*****************************************************************************/
BSP_S32  BSP_DMA_AllChannelCheck(BSP_U32 *pChannelInfo)
{
    BSP_U32 u32ChnInfo = 0;

    /* �Ƿ��ʼ���ж�*/
    if(DMA_NOTINITIALIZE == g_u32DmaInit)
    {
    	BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
    			"\rNot initialized! \n",0,0,0,0,0,0);
    	return BSP_ERR_DMA_MODULE_NOT_INITED;
    }

    /* �������*/
    if(NULL == pChannelInfo)
    {
     	 BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
				"\r BSP_DMA_AllChannelCheck NULL == pChannelInfo \n",0,0,0,0,0,0);
        return BSP_ERR_DMA_INVALID_PARA;
    }

    /* ��ȡ����ͨ����״̬*/
    BSP_REG_READ(DMA_REGBASE_ADR,DMA_CHENREG, u32ChnInfo);
    
    /* ĿǰDMA��4��ͨ��������ȡ��4bit*/
    *pChannelInfo = u32ChnInfo&DMA_CHN_EN;
    BSP_TRACE(BSP_LOG_LEVEL_DEBUG, BSP_MODU_DMAC,
        			"\r BSP_DMA_AllChannelCheck *pChannelInfo = 0x%x! \n",*pChannelInfo,0,0,0,0,0);

    return DMAC_SUCCESS;
}


/*****************************************************************************
* �� �� ��  : BSP_DMA_OneChannelCheck
*
* ��������  : DMA����ͨ����������״̬���
*
* �������  : BSP_U32 ulChan   ������ͨ����
* �������  : BSP_U32 *pChannelStatus  ��ͨ������״̬��0��ʾ���У�1��ʾæ
* �� �� ֵ  : DMAC_SUCCESS       �����ɹ�
*             BSP_ERR_DMA_MODULE_NOT_INITED      DMAδ��ʼ��
*             BSP_ERR_DMA_INVALID_PARA  ��������
*
* �޸ļ�¼  :2008��12��26��   ����  ����

*****************************************************************************/
BSP_S32 BSP_DMA_OneChannelCheck(BSP_U32 u32Chan, BSP_U32 *pChannelStatus)
{
    BSP_U32 channelinfo = 0;
    /* �Ƿ��ʼ���ж�*/
	if(DMA_NOTINITIALIZE == g_u32DmaInit)
	{
		BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
				"\rNot initialized! \n",0,0,0,0,0,0);
		return BSP_ERR_DMA_MODULE_NOT_INITED;
	}

    /* �������*/
	if((DMA_MAX_Index <= u32Chan) || (NULL == pChannelStatus))
	{
     	 BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
				"\r BSP_DMA_OneChannelcheck u32Chan = 0x%x,or NULL == pChannelStatus \n",
				u32Chan,0,0,0,0,0);
	    return BSP_ERR_DMA_INVALID_PARA;
	}

    /* ���ص�ǰͨ��������״̬*/
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
* �� �� ��  : BSP_DMA_SingleBlockStart
*
* ��������  : �������ݴ�������
*
* �������  : DMA_SINGLE_BLOCK_S stDMASingleBlock   ���鴫��ʹ�õĲ����ṹ��
*
* �������  : ��
* �� �� ֵ  : DMAC_SUCCESS       �����ɹ�
*             BSP_ERR_DMA_MODULE_NOT_INITED      DMAδ��ʼ��
*             BSP_ERR_DMA_INVALID_PARA  ��������
*             BSP_ERR_DMA_CHANNEL_BUSY  ͨ��æ
*
* �޸ļ�¼  :2008��12��26��   ����  ����
             2010��12��1��    ³��  �޸� 1.�������йص�λ���ɵ����߸�ֵ
                                         2.�ɵ����߱�֤���䳤����λ���������
                                         3.�޸Ĵ��������ж��������������������ص����

*****************************************************************************/
BSP_S32 BSP_DMA_SingleBlockStart(DMA_SINGLE_BLOCK_S * stDMASingleBlock)
{
    /* ����ͨ������*/
    BSP_U32 u32TransferCtrl_h, u32TransferCtrl_l, u32TransferCfg_h, u32TransferCfg_l;
    BSP_U32 u32SrcWidth = 0;/* Դ����λ��*/
    BSP_U32 u32DstWidth = 0;/* Ŀ�Ĵ���λ��*/
    BSP_U32 u32CtlSINC = 0; /* Դ��ַ����ģʽ���������ݼ����ǲ���*/
    BSP_U32 u32CtlDINC = 0; /* Ŀ�ĵ�ַ����ģʽ���������ݼ����ǲ���*/
    BSP_U32 u32BlockTS = 0; /* ���鴫���С*/
    BSP_U32 u32CtlSMS = DMA_MASTER1; /* Դ�豸Master ѡ��λ*/
    BSP_U32 u32CtlDMS = DMA_MASTER1; /* Ŀ���豸Master ѡ��λ*/
    BSP_U32 u32Chan;/* ͨ����*/
    BSP_U32 u32SrcBurstLength = DMA_MSIZE_16;
    BSP_U32 u32DstBurstLength = DMA_MSIZE_16;
    DMA_BUILD_LLI_FOR_BLOCK_S  stDMABuildLliForBlock;
    BSP_U32 *pHeadLLI = NULL;
    BSP_U32 u32LockKey = 0;
    BSP_U32 u32HS_SEL_SRC = DMA_SW_HANDSHAKE;
    BSP_U32 u32HS_SEL_DST = DMA_SW_HANDSHAKE;
    BSP_U32 u32MAX_ABRST = DMA_BURST_LEN16;
    
    /* �Ƿ��ʼ���ж�*/
	if(DMA_NOTINITIALIZE == g_u32DmaInit)
	{
		BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
				"\rNot initialized! \n",0,0,0,0,0,0);
		return BSP_ERR_DMA_MODULE_NOT_INITED;
	}

    u32Chan = stDMASingleBlock->stChnPara.u32Chan;

    BSP_TRACE(BSP_LOG_LEVEL_DEBUG, BSP_MODU_DMAC,
    		"\r stDMASingleBlock=0x%x\n",stDMASingleBlock,0,0,0,0,0);

    /* ����ж� */
    /* ĿǰDMA���ֻ֧�ֵ�64λλ��*/
    /* Ŀǰburst���ֻ֧�ֵ�16*/
    /* ĿǰDMA��֧����������*/
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
    /* ��ͨ��ʹ�ܼĴ���ChEnReg[CH_EN]���ж�ͨ���Ƿ���С�Ϊ0���� */
    if(DMAC_SUCCESS != DMA_CheckChannelBusy(u32Chan))
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
            "\r BSP_DMA_SingleBlockStart:The dmac Channel is busy! \n",0,0,0,0,0,0);

        DMAC_INT_UNLOCK(u32LockKey);
        return BSP_ERR_DMA_CHANNEL_BUSY;
    }

    DMAC_INT_UNLOCK(u32LockKey);
    
    /* ע������жϴ�����*/    
    g_stChannelInfo[u32Chan].pfuncDmaTcIsr = stDMASingleBlock->stChnPara.pfuncDmaTcIsr;
    g_stChannelInfo[u32Chan].s32DmaTcPara = stDMASingleBlock->stChnPara.s32DmaTcPara;
    /* ע������жϴ�����*/
    g_stChannelInfo[u32Chan].pfuncDmaErrIsr = stDMASingleBlock->stChnPara.pfuncDmaErrIsr;
    g_stChannelInfo[u32Chan].s32DmaErrPara = stDMASingleBlock->stChnPara.s32DmaErrPara;

    /* ��INT_TC_CLR��INT_ERR_CLR�Ķ�ӦΪ1�����ѡ��ͨ�����ж�״̬ */
    (BSP_VOID)DMA_ClearAllIntStatus(u32Chan);

    switch(stDMASingleBlock->enTransType)
    {
        case MEM2MEM_DMA:
        {
            /* ������Σ�����������ò�������������
            * ��ʼ��ַ�ʹ����䳤����Ҫ��ϣ�����ʹ����õ�����*/
            DMA_SetTransWidth(stDMASingleBlock->u32SrcAddr, stDMASingleBlock->u32TransLength, &u32SrcWidth);
            DMA_SetTransWidth(stDMASingleBlock->u32DstAddr, stDMASingleBlock->u32TransLength, &u32DstWidth);

            u32CtlSINC = DMA_ADDR_INCREMENT;/* Դ��ַ����ģʽ����*/
            u32CtlDINC = DMA_ADDR_INCREMENT;/* Ŀ�ĵ�ַ����ģʽ����*/
            u32SrcBurstLength = DMA_MSIZE_16;
            u32DstBurstLength = DMA_MSIZE_16;

            break;
        }
        case MEM2PRF_DMA:
        case MEM2PRF_PRF:
        {
            u32SrcWidth = stDMASingleBlock->enSrcWidth;
            u32DstWidth = stDMASingleBlock->enDstWidth;
            /* ÿһ��Ŀ�Ĵ����б����Ƿ�������ߵݼ�Դ��ַ������豸��Դ����FIFO ��һ���̶���ַȡ���ݣ�
              �򽫴�������Ϊ���̶���*/
            u32CtlSINC = DMA_ADDR_INCREMENT;/* Դ��ַ����ģʽ����*/
            u32CtlDINC = DMA_ADDR_NOCHANGE;/* Ŀ�ĵ�ַ����ģʽ�̶�*/

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
            /* ÿһ��Դ�����б����Ƿ�������ߵݼ�Դ��ַ������豸��һ���̶���ַ��Ŀ������FIFO д���ݣ�
              �򽫴�������Ϊ���̶���*/
            u32CtlSINC = DMA_ADDR_NOCHANGE;/* Դ��ַ����ģʽ�̶�*/
            u32CtlDINC = DMA_ADDR_INCREMENT; /* Ŀ�ĵ�ַ����ģʽ����*/

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
            u32CtlSINC = DMA_ADDR_NOCHANGE;/* Ŀ�ĵ�ַ����ģʽ�̶�*/
            u32CtlDINC = DMA_ADDR_NOCHANGE;/* Ŀ�ĵ�ַ����ģʽ�̶�*/

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

        /* ������master1 �Ӻ���master2 */
#ifdef PRODUCT_CFG_CORE_TYPE_MODEM 
        u32CtlSMS = DMA_MASTER1;
        u32CtlDMS = DMA_MASTER1;
#else         
        u32CtlSMS = DMA_MASTER2;
        u32CtlDMS = DMA_MASTER2;
#endif
    
    BSP_TRACE(BSP_LOG_LEVEL_DEBUG, BSP_MODU_DMAC,"\r BSP_DMA_SingleBlockStart:ulChan=%d,u32SrcWidth = 0x%x,u32DstWidth = 0x%x,u32HS_SEL_SRC = 0x%x,u32HS_SEL_DST = 0x%x\n",u32Chan,u32SrcWidth,u32SrcWidth,u32HS_SEL_SRC,u32HS_SEL_DST,0);

    /* ����CFG�Ĵ���*/
    u32TransferCfg_h = DMA_CFG_H_DEFAULT|(stDMASingleBlock->enDstHSHKInterface<<11)
        |(stDMASingleBlock->enSrcHSHKInterface<<7);
    u32TransferCfg_l = DMA_CFG_L_DEFAULT|(u32HS_SEL_SRC<<11)|(u32HS_SEL_DST<<10)|(u32MAX_ABRST<<20);

    /* ����ǳ������������Ҫ���Ϊ��δ���*/
    if((DMA_MAX_SINGLE_BLOCK_TRANS_LENGTH*((BSP_U32)0x1<<u32SrcWidth)) < (stDMASingleBlock->u32TransLength))
    {
        memset((void*)&stDMABuildLliForBlock, 0x0, sizeof(DMA_BUILD_LLI_FOR_BLOCK_S));
        /* ���㵱ǰ������ÿ�����֧�ֵ�������С*/
        stDMABuildLliForBlock.u32OneTimeTransLength = (1<<u32SrcWidth) * DMA_SINGLE_BLOCK_TRANS_LENGTH ;

        BSP_TRACE(BSP_LOG_LEVEL_DEBUG, BSP_MODU_DMAC, "\r u32OneTimeTransLength =  0x%x\n",
                stDMABuildLliForBlock.u32OneTimeTransLength,0,0,0,0,0);

        /* ������Ҫ���Ϊ���δ���*/
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

        /* ��Ҫ��CTL�Ĵ���дΪ����ʹ�� */
        BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CTL(u32Chan), (DMA_LLI_ENABLE|DMA_ENABLE));
        
    }
    else
    {
        /* ����鴫���С����ֵ*/
        u32BlockTS = stDMASingleBlock->u32TransLength/((BSP_U32)1<<u32SrcWidth);    
        /* ����crl�Ĵ���*/
        u32TransferCtrl_h = DMA_CTL_H_DEFAULT | u32BlockTS;
        u32TransferCtrl_l = DMA_CTL_L_DEFAULT | (u32SrcWidth << 0x4) | (u32DstWidth << 0x1)
            | (stDMASingleBlock->enTransType << 20) | (u32CtlDINC << 7) | (u32CtlSINC << 9)
            | (u32DstBurstLength << 11) | (u32SrcBurstLength << 14)
            | (u32CtlSMS << 25) | (u32CtlDMS << 23);

        BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_LLP(u32Chan), 0);

        /* ����ͨ��������ֱ��д�Ĵ������Ͳ����ú����� */
        BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_SAR(u32Chan), stDMASingleBlock->u32SrcAddr);
        BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_DAR(u32Chan), stDMASingleBlock->u32DstAddr);
        BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CTL(u32Chan), u32TransferCtrl_l);
        /* modified for lint e665*/
        BSP_REG_WRITE(DMA_REGBASE_ADR, (DMA_CTL(u32Chan) + 4), u32TransferCtrl_h);
        /* modified for lint e665*/
    }
    /* дCFG��ֵ */
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CFG(u32Chan), u32TransferCfg_l);
    /* modified for lint e665*/
    BSP_REG_WRITE(DMA_REGBASE_ADR, (DMA_CFG(u32Chan)+4), u32TransferCfg_h);
    /* modified for lint e665*/
    /* ʹ��DMA */
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_DMACFGREG, DMA_ENABLE);

    /* ʹ��ͨ�� */
    BSP_REG_WRITE(DMA_REGBASE_ADR,DMA_CHENREG, (0x100<<(u32Chan))|(0x1<<(u32Chan)));

    return DMAC_SUCCESS;
}/*lint !e550*/


/*****************************************************************************
* �� �� ��  : BSP_DMA_LliBuild
*
* ��������  : ����������
*
* �������  : BSP_U32 ulChan        ͨ����
*             DMA_LLI_S *pFirstLLI  ��Դ����ĵ�һ���ڵ�ָ��
* �������  : ��
* �� �� ֵ  : BSP_VOID *  ָ��ײ㽨���õĴ��������ͷ�ڵ��ָ��
*
* �޸ļ�¼  :2009��7��24��   ����  ����
             Modified by liumengcun, 2010-1-12 ��PS MSP���ۣ�ˢcache����ʹ�������

*****************************************************************************/
BSP_VOID * BSP_DMA_LliBuild(BSP_U32 ulChan, DMA_LLI_S *pFirstLLI)
{
    BSP_U32 *pHeadLLI;/* ͷ�ڵ�*/
    DMA_LLI_PHY_S * pstDMALliPhy = NULL;/* ��ʱ�ṹ*/
    DMA_LLI_S *pLocalLLI;/* ѭ������*/
    BSP_U32 u32TempSrcWidth, u32TempDstWidth;/* λ����ʱ����*/
    BSP_U32 u32MemFlushLength = 0;/* ˢ��memory����*/
    BSP_U32 u32CtlSMS = DMA_MASTER1;/* ԴDMA masterѡ��*/
    BSP_U32 u32CtlDMS = DMA_MASTER1;/* Ŀ��DMA masterѡ��*/
    BSP_U32 i = 0;
    
    /* �Ƿ��ʼ���ж�*/
    if(DMA_NOTINITIALIZE == g_u32DmaInit)
    {
    	BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
    			"\rNot initialized! \n",0,0,0,0,0,0);
    	return NULL;
    }

    /* �������*/
	if((DMA_MAX_Index <= ulChan) || (NULL == pFirstLLI))
	{
		BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
			"\r BSP_DMA_LliBuild Input Para Error! ulChan = 0x%x,pFirstLLI = 0x%x\n",
			ulChan,pFirstLLI,0,0,0,0);
	    return NULL;
	}

    pHeadLLI = g_pu32PhyAddrPointer[ulChan];
    //u32MemFlushLength = sizeof(DMA_LLI_PHY_S)*DMA_NODE_NUM_NORMAL;

    /* ������master1 �Ӻ���master2 */
#ifdef PRODUCT_CFG_CORE_TYPE_MODEM 
    u32CtlSMS = DMA_MASTER1;
    u32CtlDMS = DMA_MASTER1;
#else         
    u32CtlSMS = DMA_MASTER2;
    u32CtlDMS = DMA_MASTER2;
#endif                

    pstDMALliPhy = (DMA_LLI_PHY_S *)pHeadLLI;/*lint !e740 !e826*/
    /* ѭ�����ýڵ������ */
    for(pLocalLLI = pFirstLLI; NULL != pLocalLLI; pLocalLLI = pLocalLLI->pNextLLI)
    {
        if(i >= DMA_NODE_NUM_NORMAL)
        {
            BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC, "\r  Node Number is bigger than DMA_NODE_NUM_NORMAL\n",0,0,0,0,0,0);
    		return NULL;
        }
        /* ������*/
    	pstDMALliPhy->SrcAddr = pLocalLLI->SrcAddr;

        /* ����Դλ��*/
    	DMA_SetTransWidth(pLocalLLI->SrcAddr, pLocalLLI->TransLength, &u32TempSrcWidth);

        DMA_SetTransWidth(pLocalLLI->DstAddr, pLocalLLI->TransLength, &u32TempDstWidth);
	    pstDMALliPhy->DstAddr = pLocalLLI->DstAddr;

    	/* �������в�֧�ִ���DMA���鴫����󳤶ȵ����*/
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
    /* ���һ���ڵ��next��0*/
    ((DMA_LLI_PHY_S*)(pstDMALliPhy -1))->NextLLI = 0;

    /* ���һ���ڵ㣬ȥʹ��Դ��Ŀ������ʹ��λ */
    ((DMA_LLI_PHY_S*)(pstDMALliPhy -1))->ulTransferCtrl_l = DMA_CTL_L_DEFAULT | 0x0 | (u32TempSrcWidth<<0x4)
                                                            |(u32TempDstWidth<<0x1)| (u32CtlSMS << 25) | (u32CtlDMS << 23)
                                                            |(DMA_MSIZE_16 << 11) | (DMA_MSIZE_16 << 14);
    u32MemFlushLength = sizeof(DMA_LLI_PHY_S)*i;

    DMAC_CACHE_FLUSH(pHeadLLI, u32MemFlushLength);

    return (BSP_VOID *)DMAC_VIRT_TO_PHYS(pHeadLLI);

}

/*****************************************************************************
* �� �� ��  : BSP_DMA_LliStart
*
* ��������  : �������ݴ�������ר�ú���
*
* �������  : DMA_CHN_PARA_S stChnPara    ����ͨ�������ṹ
*             BSP_VOID *pu32Head��   ����BSP_DMA_LliBuild��õĴ���������ͷָ��
* �������  : ��
* �� �� ֵ  : DMAC_SUCCESS       �����ɹ�
*             BSP_ERR_DMA_MODULE_NOT_INITED      DMAδ��ʼ��
*             BSP_ERR_DMA_INVALID_PARA  ��������
*
* �޸ļ�¼  :2008��12��26��   ����  ����

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

    /* �Ƿ��ʼ���ж�*/
    if(DMA_NOTINITIALIZE == g_u32DmaInit)
    {
    	BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
    	"\rNot initialized! \n",0,0,0,0,0,0);
    	return BSP_ERR_DMA_MODULE_NOT_INITED;
    }

    /* �������*/
	if((DMA_MAX_Index <= stChnPara->u32Chan) || (NULL == pu32Head))
	{
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
            "\r BSP_DMA_LliStart Input Para Error! ulChan = 0x%x,pu32Head = 0x%x\n",
            stChnPara->u32Chan,pu32Head,0,0,0,0);
	    return BSP_ERR_DMA_INVALID_PARA;
	}

    DMAC_INT_LOCK(u32LockKey);
    /* ��ͨ��ʹ�ܼĴ���ChEnReg[CH_EN]���ж�ͨ���Ƿ���С�Ϊ0���� */
    if(DMAC_SUCCESS != DMA_CheckChannelBusy(stChnPara->u32Chan))
    {
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
            "\r BSP_DMA_LliStart:The Channel is busy! \n",0,0,0,0,0,0);

        DMAC_INT_UNLOCK(u32LockKey);
        return BSP_ERR_DMA_CHANNEL_BUSY;
    }

    DMAC_INT_LOCK(u32LockKey);

    u32TransferCfg_l = DMA_CFG_L_DEFAULT|(DMA_SW_HANDSHAKE<<11)|(DMA_SW_HANDSHAKE<<10)|(u32MAX_ABRST<<20); 

    /* ע������жϴ�����*/
    g_stChannelInfo[stChnPara->u32Chan].pfuncDmaTcIsr = stChnPara->pfuncDmaTcIsr;
    g_stChannelInfo[stChnPara->u32Chan].s32DmaTcPara = stChnPara->s32DmaTcPara;

    /* ע������жϴ�����*/
    g_stChannelInfo[stChnPara->u32Chan].pfuncDmaErrIsr = stChnPara->pfuncDmaErrIsr;
    g_stChannelInfo[stChnPara->u32Chan].s32DmaErrPara = stChnPara->s32DmaErrPara;

    /* ���ͨ����ǰһ��DMA �����е������жϡ� */
    (BSP_VOID)DMA_ClearAllIntStatus(stChnPara->u32Chan);

    /* ���üĴ���LLP ΪLLP0 ��ֵ��ָ���һ��LLI ��ָ��
    * Ϊ����ĵ�һ���ڵ��sar �ĵ�ַ
    */
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_LLP(stChnPara->u32Chan), ((BSP_U32)(pHeadLLI)>>2)<<2);

    /* ��Ҫ��CTL�Ĵ���дΪ����ʹ��
    BSP_REG_READ(DMA_REGBASE_ADR, DMA_CTL(u32Chan), u32CtlTemp);*/
    u32CtlTemp |= 0x18000001;
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CTL(stChnPara->u32Chan), u32CtlTemp);

    /* дCFG��ֵ */
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_CFG(stChnPara->u32Chan), u32TransferCfg_l);
    /* modified for lint e665*/
    BSP_REG_WRITE(DMA_REGBASE_ADR, (DMA_CFG(stChnPara->u32Chan)+4), u32TransferCfg_h);
    /* modified for lint e665*/

    /* ʹ��DMA */
    BSP_REG_WRITE(DMA_REGBASE_ADR, DMA_DMACFGREG, DMA_ENABLE);

    /* ʹ��ͨ�� */
    BSP_REG_WRITE(DMA_REGBASE_ADR,DMA_CHENREG, (0x100<<(stChnPara->u32Chan))|(0x1<<(stChnPara->u32Chan)));

    return DMAC_SUCCESS;
}/*lint !e818 !e550 !e529*/

/*****************************************************************************
* �� �� ��  : BSP_DMA_GetDar
*
* ��������  : ����ʹ�øú�����ȡDMA��DAR�Ĵ�����ֵ
*
* �������  : BSP_U32 u32Chan   ͨ����
*
* �������  : BSP_U32 *u32Dar  ��ͨ����Ӧ��Ŀ�ĵ�ַ�Ĵ�����ֵ
* �� �� ֵ  : DMAC_SUCCESS       ��ȡ�ɹ�
*             BSP_ERR_DMA_MODULE_NOT_INITED      DMAδ��ʼ��
*             BSP_ERR_DMA_INVALID_PARA  ��������
*
* �޸ļ�¼  :2010��12��1��   ³��  ����

*****************************************************************************/
BSP_S32 BSP_DMA_GetDar(BSP_U32 u32Chan, BSP_U32 *u32Dar)
{
    BSP_U32 u32ADR = 0;
    
    /* �Ƿ��ʼ���ж�*/
	if(DMA_NOTINITIALIZE == g_u32DmaInit)
	{
		BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
				"\rBSP_DMA_GetIdleChannel Not initialized! \n",0,0,0,0,0,0);
		return BSP_ERR_DMA_MODULE_NOT_INITED;
	}

    /* �������*/
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
* �� �� ��  : BSP_DMA_DisableChannel
*
* ��������  : ����ʹ�øú���ȥʹ��DMACͨ��
*
* �������  : BSP_U32 u32Chan   ͨ����
*
* �������  : ��
* �� �� ֵ  : DMAC_SUCCESS       ��ȡ�ɹ�
*             BSP_ERR_DMA_MODULE_NOT_INITED      DMAδ��ʼ��
*             BSP_ERR_DMA_INVALID_PARA  ��������
*
* �޸ļ�¼  :2010��12��1��   ³��  ����

*****************************************************************************/
BSP_S32 BSP_DMA_DisableChannel(BSP_U32 u32Chan)
{
    /* �Ƿ��ʼ���ж�*/
	if(DMA_NOTINITIALIZE == g_u32DmaInit)
	{
		BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_DMAC,
				"\rBSP_DMA_GetIdleChannel Not initialized! \n",0,0,0,0,0,0);
		return BSP_ERR_DMA_MODULE_NOT_INITED;
	}

    /* �������*/
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

/* DMAC��ά�ɲ� */
/*****************************************************************************
* �� �� ��  : DMAC_GetRegistValue
*
* ��������  : ��ȡDMACģ��Ĵ�����ֵ
*
* �������  : BSP_VOID
* �������  : NA
*
* �� �� ֵ  : NA
*
* ����˵��  : ��ά�ɲ�ӿں���
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
    DMAC_PRINT("ͨ��ʹ�ܼĴ���         DMA_CHENREG:   0x%08x\n", channelinfo);
    DMAC_PRINT("����ж�״̬�Ĵ���     DMA_STATUSINT: 0x%08x\n", u32ChannelStatus);
    DMAC_PRINT("IntTfr �ж�״̬        DMA_STATUSTFR: 0x%08x\n", u32ChannelTCStatus);
    DMAC_PRINT("IntErr �ж�״̬        DMA_STATUSERR: 0x%08x\n", u32ChannelErrStatus);
    DMAC_PRINT("===================================================\n\n");
}

/*****************************************************************************
* �� �� ��  : DMAC_GetChannelInfo
*
* ��������  : ��ȡDMACģ��ͨ����Ϣ
*
* �������  : BSP_U32
* �������  : NA
*
* �� �� ֵ  : NA
*
* ����˵��  : ��ά�ɲ�ӿں���
*
*****************************************************************************/
BSP_VOID DMAC_GetChannelInfo(BSP_U32 u32Chan)
{
    if((u32Chan>=DMA_MAX_CHANNELS)||(u32Chan<0))
    {
        DMAC_PRINT("����ͨ���ŷǷ���\n");
        return;
    }

    DMAC_PRINT("============= DMAC_GetChannelInfo ============\n");
    DMAC_PRINT("channel�߼�״̬      u32ChnUsrStatus = %u\n", g_stChannelInfo[u32Chan].u32ChnUsrStatus);
    DMAC_PRINT("ͨ������             u32ChnAttribute = %u\n", g_stChannelInfo[u32Chan].u32ChnAttribute);
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


