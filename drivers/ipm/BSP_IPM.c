/*************************************************************************
*   ��Ȩ����(C) 1987-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_IPM.c
*
*   ��    �� :  luting
*
*   ��    �� :  ������Ҫ���IP���˽ӿں�����ʵ�����к����е�IP���˹���
*
*   �޸ļ�¼ :  2011��1��11��  v1.00  luting  ����
*
*************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include <generated/FeatureConfig.h>

#if ((defined (BOARD_ASIC) || defined(BOARD_SFT))\
	&& (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))\
    && ((FEATURE_E5 == FEATURE_ON) && (FEATURE_E5_UDP == FEATURE_ON))
#include  <mach/common/mem/bsp_mem.h>
#include "BSP.h"
#include <linux/module.h>

#define IPM_IPMEM_TYPE_DDR

UDI_HANDLE  g_udiIPMHdl;

#ifdef PRODUCT_CFG_CORE_TYPE_APP
//extern BSP_U32 BSP_WAN_FreeSkbCb(BSP_U32 handle, BSP_S32 u32Size);
#endif

//extern BSP_U8* BSP_WAN_GetNode(BSP_VOID);


BSP_S32 BSP_IPM_Init()
{
    UDI_OPEN_PARAM stParam;
    ICC_CHAN_ATTR_S stChanAttr = {0};
    stParam.devid  = UDI_ICC_IPM_ID;
    stParam.pPrivate = &stChanAttr;
    /*UDI stParam˽�в������ã�������ȷ���󲹳�*/

    stChanAttr.u32FIFOInSize = 8192;
    stChanAttr.u32FIFOOutSize = 8192;
    stChanAttr.u32Priority = 0;
    stChanAttr.u32TimeOut = 5000;
    stChanAttr.enChanMode = ICC_CHAN_MODE_PACKET;
    stChanAttr.event_cb = 0;
    #ifdef PRODUCT_CFG_CORE_TYPE_MODEM
    stChanAttr.read_cb = 0;
    #else
    stChanAttr.read_cb = 0;//BSP_WAN_FreeSkbCb;
    #endif
    stChanAttr.write_cb = 0;

    g_udiIPMHdl = udi_open(&stParam);        /*��Device�����ID*/

    if (-1 == g_udiIPMHdl)
    {
        return ERROR;
    }

    return OK;
}

//module_init(BSP_IPM_Init);

/*****************************************************************************
* �� �� ��  : BSP_IPM_GetDescNode
*
* ��������  : ��ȡ��Ϣ���ָ��ӿ�
*
* �������  : BSP_U32 u32ItemNum    IP��������     
*
* �������  : ��
* �� �� ֵ  : �� --- ��ȡʧ��
*             �ǿ� ---  ��ȡ����Ϣ���ָ��
*
* �޸ļ�¼  :2011��1��27��   ³��  ����
*****************************************************************************/
IPM_DESC_NODE_S *BSP_IPM_GetDescNode(BSP_U32 u32ItemNum)
{
    IPM_DESC_NODE_S * pstNode = BSP_NULL;

    pstNode = (IPM_DESC_NODE_S*)BSP_SMALLOC(sizeof(IPM_DESC_NODE_S)+u32ItemNum * sizeof(IPM_DESC_ITEM_S),MEM_ICC_AXI_POOL);
    if(BSP_NULL == pstNode)
    {
        return pstNode;
    }
    
    if(u32ItemNum > 0)
    {
        pstNode->pstDescItem = (IPM_DESC_ITEM_S*)(pstNode + 1); /*lint !e740*/       
    }
    else
    {
        pstNode->pstDescItem = BSP_NULL;
    }
    pstNode->ItemNum = (BSP_U16)u32ItemNum;
    return pstNode;
}

EXPORT_SYMBOL(BSP_IPM_GetDescNode);
/*****************************************************************************
* �� �� ��  : BSP_IPM_FreeDescNode
*
* ��������  : �ͷ���Ϣ���ָ��ӿ�
*
* �������  : IPM_DESC_NODE_S *pstDescNode   
*                  BSP_BOOL bNodeOnly   
*                 trueֻ�ͷŽ��      false �ͷŽ�����֯�ڴ�
*
* �������  : ��
* �� �� ֵ  : ��
*
* �޸ļ�¼  :2011��1��27��   ³��  ����
*
*****************************************************************************/
BSP_S32 BSP_IPM_FreeDescNode(IPM_DESC_NODE_S *pstDescNode, BSP_BOOL bNodeOnly)
{
    BSP_U32 i = 0;
    IPM_DESC_ITEM_S * pTemp;
    
    if(BSP_NULL == pstDescNode)
    {
       #if 0
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,
                    "\r BSP_IPM_FreeDescNode pstDescNode NULL! \n",0,0,0,0,0,0);
	#endif
        return IPM_ERROR;
    }
    /* ֻ�ͷŽ�� */
    if(bNodeOnly)
    {
        BSP_SFREE(pstDescNode);
        pstDescNode = BSP_NULL;
    }

    /* �ͷŽ���IP �����ڴ� */
    else
    {
        //logMsg("BSP_IPM_FreeDescNode  free ip+node pstDescNode =0x%x\n",pstDescNode,0,0,0,0,0);
        if(pstDescNode->StartAddr != 0)
        {
            //BSP_SFREE((BSP_VOID*)pstDescNode->StartAddr);
            #ifdef PRODUCT_CFG_CORE_TYPE_MODEM
            pTemp = pstDescNode->pstDescItem;
            pTemp->Addr -= 14;
            if (-1 == udi_write(g_udiIPMHdl, &pTemp->Addr, 4))
            {
                logMsg(" BSP_IPM_FreeBspBuf udi_write err\n ");
            }
            #endif
            pstDescNode->StartAddr = 0;
        }
        else
        {
            pTemp = pstDescNode->pstDescItem;
            for(i = 0; i < pstDescNode->ItemNum; i++)
            {
                if(pTemp->Addr != 0)
                {
                    //BSP_SFREE((BSP_VOID*)pTemp->Addr);
                    #ifdef PRODUCT_CFG_CORE_TYPE_MODEM
                    if (-1 == udi_write(g_udiIPMHdl, &pTemp->Addr, 4))
                    {
                        logMsg(" BSP_IPM_FreeBspBuf udi_write err\n ");
                    }
                    #endif
                }
                pTemp += 1;
            }
        }
        BSP_SFREE(pstDescNode);
        pstDescNode = BSP_NULL;        
    }
    return IPM_SUCCESS;
}

EXPORT_SYMBOL(BSP_IPM_FreeDescNode);

/*****************************************************************************
* �� �� ��  : BSP_IPM_GetBspBuf
*
* ��������  : �����ڴ�ӿ�
*
* �������  : BSP_U32 u32len    ��Ҫ�����buf����     
*
* �������  : ��
* �� �� ֵ  : ��ȡ��bufָ��
*
* �޸ļ�¼  :2011��1��27��   ³��  ����
*****************************************************************************/
BSP_U8 *BSP_IPM_GetBspBuf(BSP_U32 u32len)
{
    //BSP_U8* ptr = NULL;

    return NULL;//BSP_WAN_GetNode();
#if 0
    #ifdef IPM_IPMEM_TYPE_DDR
    ptr = (BSP_U8*)BSP_SMALLOC(u32len, MEM_ICC_DDR_POOL);
    #else
    ptr = (BSP_U8*)BSP_SMALLOC(u32len, MEM_ICC_AXI_POOL);
    #endif
#endif
    //return ptr;
}

EXPORT_SYMBOL(BSP_IPM_GetBspBuf);

/*****************************************************************************
* �� �� ��  : BSP_IPM_FreeBspBuf
*
* ��������  : �ͷ��ڴ�ӿ�
*
* �������  : BSP_U8 *pBuf ��Ҫ�ͷŵ�ָ��    
*
* �������  : ��
* �� �� ֵ  : ��
*
* �޸ļ�¼  :2011��1��27��   ³��  ����
*****************************************************************************/
BSP_VOID BSP_IPM_FreeBspBuf(BSP_U8 *pBuf)
{
    BSP_U8* tmp = pBuf;
    if(pBuf != BSP_NULL)
    {
        //BSP_SFREE(pBuf);
        #ifdef PRODUCT_CFG_CORE_TYPE_MODEM
        if (-1 == udi_write(g_udiIPMHdl, &tmp, 4))
        {
            logMsg(" BSP_IPM_FreeBspBuf udi_write err\n ");
        }
        #endif
    }
    //logMsg("BSP_IPM_FreeBspBuf pBuf=0x%x\n", pBuf);
}

EXPORT_SYMBOL(BSP_IPM_FreeBspBuf);

#else
#include  <mach/common/mem/bsp_mem.h>
#include "BSP.h"
#include <linux/module.h>

#define IPM_IPMEM_TYPE_DDR

/*****************************************************************************
* �� �� ��  : BSP_IPM_GetDescNode
*
* ��������  : ��ȡ��Ϣ���ָ��ӿ�
*
* �������  : BSP_U32 u32ItemNum    IP��������     
*
* �������  : ��
* �� �� ֵ  : �� --- ��ȡʧ��
*             �ǿ� ---  ��ȡ����Ϣ���ָ��
*
* �޸ļ�¼  :2011��1��27��   ³��  ����
*****************************************************************************/
IPM_DESC_NODE_S *BSP_IPM_GetDescNode(BSP_U32 u32ItemNum)
{
    IPM_DESC_NODE_S * pstNode = BSP_NULL;

    pstNode = (IPM_DESC_NODE_S*)BSP_SMALLOC(sizeof(IPM_DESC_NODE_S)+u32ItemNum * sizeof(IPM_DESC_ITEM_S),MEM_ICC_AXI_POOL);
    if(BSP_NULL == pstNode)
    {
        return pstNode;
    }
    
    if(u32ItemNum > 0)
    {
        pstNode->pstDescItem = (IPM_DESC_ITEM_S*)(pstNode + 1); /*lint !e740*/       
    }
    else
    {
        pstNode->pstDescItem = BSP_NULL;
    }
    pstNode->ItemNum = (BSP_U16)u32ItemNum;
    return pstNode;
}

EXPORT_SYMBOL(BSP_IPM_GetDescNode);
/*****************************************************************************
* �� �� ��  : BSP_IPM_FreeDescNode
*
* ��������  : �ͷ���Ϣ���ָ��ӿ�
*
* �������  : IPM_DESC_NODE_S *pstDescNode   
*                  BSP_BOOL bNodeOnly   
*                 trueֻ�ͷŽ��      false �ͷŽ�����֯�ڴ�
*
* �������  : ��
* �� �� ֵ  : ��
*
* �޸ļ�¼  :2011��1��27��   ³��  ����
*
*****************************************************************************/
BSP_S32 BSP_IPM_FreeDescNode(IPM_DESC_NODE_S *pstDescNode, BSP_BOOL bNodeOnly)
{
    BSP_U32 i = 0;
    IPM_DESC_ITEM_S * pTemp;
    
    if(BSP_NULL == pstDescNode)
    {
       #if 0
        BSP_TRACE(BSP_LOG_LEVEL_ERROR, BSP_MODU_IPF,
                    "\r BSP_IPM_FreeDescNode pstDescNode NULL! \n",0,0,0,0,0,0);
	#endif
        return IPM_ERROR;
    }
    /* ֻ�ͷŽ�� */
    if(bNodeOnly)
    {
        BSP_SFREE(pstDescNode);
        pstDescNode = BSP_NULL;
    }

    /* �ͷŽ���IP �����ڴ� */
    else
    {
        //logMsg("BSP_IPM_FreeDescNode  free ip+node pstDescNode =0x%x\n",pstDescNode,0,0,0,0,0);
        if(pstDescNode->StartAddr != 0)
        {
            BSP_SFREE((BSP_VOID*)pstDescNode->StartAddr);
            pstDescNode->StartAddr = 0;
        }
        else
        {
            pTemp = pstDescNode->pstDescItem;
            for(i = 0; i < pstDescNode->ItemNum; i++)
            {
                if(pTemp->Addr != 0)
                {
                    BSP_SFREE((BSP_VOID*)pTemp->Addr);
                }
                pTemp += 1;
            }
        }
        BSP_SFREE(pstDescNode);
        pstDescNode = BSP_NULL;        
    }
    return IPM_SUCCESS;
}

EXPORT_SYMBOL(BSP_IPM_FreeDescNode);
/*****************************************************************************
* �� �� ��  : BSP_IPM_GetBspBuf
*
* ��������  : �����ڴ�ӿ�
*
* �������  : BSP_U32 u32len    ��Ҫ�����buf����     
*
* �������  : ��
* �� �� ֵ  : ��ȡ��bufָ��
*
* �޸ļ�¼  :2011��1��27��   ³��  ����
*****************************************************************************/
BSP_U8 *BSP_IPM_GetBspBuf(BSP_U32 u32len)
{
    BSP_U8* ptr = NULL;

    #ifdef IPM_IPMEM_TYPE_DDR
    ptr = (BSP_U8*)BSP_SMALLOC(u32len, MEM_ICC_DDR_POOL);
    #else
    ptr = (BSP_U8*)BSP_SMALLOC(u32len, MEM_ICC_AXI_POOL);
    #endif
    
    return ptr;
}

EXPORT_SYMBOL(BSP_IPM_GetBspBuf);
/*****************************************************************************
* �� �� ��  : BSP_IPM_FreeBspBuf
*
* ��������  : �ͷ��ڴ�ӿ�
*
* �������  : BSP_U8 *pBuf ��Ҫ�ͷŵ�ָ��    
*
* �������  : ��
* �� �� ֵ  : ��
*
* �޸ļ�¼  :2011��1��27��   ³��  ����
*****************************************************************************/
BSP_VOID BSP_IPM_FreeBspBuf(BSP_U8 *pBuf)
{
    if(pBuf != BSP_NULL)
    {
        BSP_SFREE(pBuf);
    }
    //logMsg("BSP_IPM_FreeBspBuf pBuf=0x%x\n", pBuf);
}

EXPORT_SYMBOL(BSP_IPM_FreeBspBuf);
#endif

#ifdef __cplusplus
}
#endif


