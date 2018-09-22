/*************************************************************************
*   版权所有(C) 1987-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_IPM.c
*
*   作    者 :  luting
*
*   描    述 :  本文主要完成IP过滤接口函数，实现上行和下行的IP过滤功能
*
*   修改记录 :  2011年1月11日  v1.00  luting  创建
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
    /*UDI stParam私有参数配置，待底软确定后补充*/

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

    g_udiIPMHdl = udi_open(&stParam);        /*打开Device，获得ID*/

    if (-1 == g_udiIPMHdl)
    {
        return ERROR;
    }

    return OK;
}

//module_init(BSP_IPM_Init);

/*****************************************************************************
* 函 数 名  : BSP_IPM_GetDescNode
*
* 功能描述  : 获取信息结点指针接口
*
* 输入参数  : BSP_U32 u32ItemNum    IP包的数量     
*
* 输出参数  : 无
* 返 回 值  : 空 --- 获取失败
*             非空 ---  获取的信息结点指针
*
* 修改记录  :2011年1月27日   鲁婷  创建
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
* 函 数 名  : BSP_IPM_FreeDescNode
*
* 功能描述  : 释放信息结点指针接口
*
* 输入参数  : IPM_DESC_NODE_S *pstDescNode   
*                  BSP_BOOL bNodeOnly   
*                 true只释放结点      false 释放结点和组织内存
*
* 输出参数  : 无
* 返 回 值  : 无
*
* 修改记录  :2011年1月27日   鲁婷  创建
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
    /* 只释放结点 */
    if(bNodeOnly)
    {
        BSP_SFREE(pstDescNode);
        pstDescNode = BSP_NULL;
    }

    /* 释放结点加IP 包的内存 */
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
* 函 数 名  : BSP_IPM_GetBspBuf
*
* 功能描述  : 申请内存接口
*
* 输入参数  : BSP_U32 u32len    需要申请的buf长度     
*
* 输出参数  : 无
* 返 回 值  : 获取的buf指针
*
* 修改记录  :2011年1月27日   鲁婷  创建
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
* 函 数 名  : BSP_IPM_FreeBspBuf
*
* 功能描述  : 释放内存接口
*
* 输入参数  : BSP_U8 *pBuf 需要释放的指针    
*
* 输出参数  : 无
* 返 回 值  : 无
*
* 修改记录  :2011年1月27日   鲁婷  创建
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
* 函 数 名  : BSP_IPM_GetDescNode
*
* 功能描述  : 获取信息结点指针接口
*
* 输入参数  : BSP_U32 u32ItemNum    IP包的数量     
*
* 输出参数  : 无
* 返 回 值  : 空 --- 获取失败
*             非空 ---  获取的信息结点指针
*
* 修改记录  :2011年1月27日   鲁婷  创建
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
* 函 数 名  : BSP_IPM_FreeDescNode
*
* 功能描述  : 释放信息结点指针接口
*
* 输入参数  : IPM_DESC_NODE_S *pstDescNode   
*                  BSP_BOOL bNodeOnly   
*                 true只释放结点      false 释放结点和组织内存
*
* 输出参数  : 无
* 返 回 值  : 无
*
* 修改记录  :2011年1月27日   鲁婷  创建
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
    /* 只释放结点 */
    if(bNodeOnly)
    {
        BSP_SFREE(pstDescNode);
        pstDescNode = BSP_NULL;
    }

    /* 释放结点加IP 包的内存 */
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
* 函 数 名  : BSP_IPM_GetBspBuf
*
* 功能描述  : 申请内存接口
*
* 输入参数  : BSP_U32 u32len    需要申请的buf长度     
*
* 输出参数  : 无
* 返 回 值  : 获取的buf指针
*
* 修改记录  :2011年1月27日   鲁婷  创建
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
* 函 数 名  : BSP_IPM_FreeBspBuf
*
* 功能描述  : 释放内存接口
*
* 输入参数  : BSP_U8 *pBuf 需要释放的指针    
*
* 输出参数  : 无
* 返 回 值  : 无
*
* 修改记录  :2011年1月27日   鲁婷  创建
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


