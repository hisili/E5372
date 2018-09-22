/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : TtfMemoryMap.h
  版 本 号   : 初稿
  生成日期   : 2011年12月23日
  最近修改   :
  功能描述   : MemoryMap.h 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2011年12月23日
    修改内容   : 创建文件

******************************************************************************/

#ifndef __TTFMEMORYMAP_H__
#define __TTFMEMORYMAP_H__


/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include "FeatureConfigTTF.h"
#include "MemoryMap.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif




/*****************************************************************************
  2 宏定义
*****************************************************************************/

/* 32 字节对齐 */
#define TTF_GET_32BYTE_ALIGN_VALUE(ulAddr)   (((unsigned long)(ulAddr) + 0x1F) & (~0x1F))



    /*******Uncache mem(4M) deploy begin********/
    /*->startAddr|                      |       */
    /*           | DataMover Link (100k)|       */ //Modified Mem Reduce
    /*           |                      |       */
    /*           |----------------------|       */
    /*           |                      |       */
    /*           | Cipher Master Link   |       */
    /*           |       (10k)          |       */
    /*           |                      |       */
    /*           |----------------------|       */
    /*           |                      |       */
    /*           | TTF Mem Pool         |       */
    /*           |       (4M-110K)      |       */
    /*           |                      |       */
    /*           |----------------------|       */

    /*V5 cipher*/
    /*********Uncache mem(4M) deploy begin*********/
    /*->startAddr|                        |       */
    /*           |  DataMover Link (100k) |       */  //Modified Mem Reduce
    /*           |------------------------|       */
    /*           |UL CIPHER PARA NODE(18k)|       */
    /*           |------------------------|       */
    /*           |UL CIPHER SEG DATA (19k)|       */
    /*           |------------------------|       */
    /*           |   UL CIPHER KEY (1k)   |       */
    /*           |------------------------|       */
    /*           |DL CIPHER PARA NODE(11k)|       */
    /*           |------------------------|       */
    /*           |DL CIPHER SEG DATA(155k)|       */
    /*           |------------------------|       */
    /*           |   DL CIPHER KEY (1k)   |       */
    /*           |------------------------|       */
    /*           |     TTF Mem Pool       |       */
    /*           |       (4M-244K)        |       */
    /*           |                        |       */
    /*           |------------------------|       */

    /* 分配给TTF的总内存大小和基地址 */

/* 各节点参数所需的内存 */

#define TTF_UL_CIPHER_PARA_NODE_ADDR(BaseAddr) BaseAddr

/*1096(WTTF_MAX_PDU_NUM_PER_UL_LOCH) * 32Byte(RLC_PDU_SEG_CIPHER_PARA_STRU) ≈ 35KByte*/
#define TTF_UL_CIPHER_PARA_NODE_LEN     (35*1024)


#define TTF_UL_CIPHER_SEG_DATA_ADDR(BaseAddr)\
           (TTF_UL_CIPHER_PARA_NODE_ADDR(BaseAddr) + TTF_UL_CIPHER_PARA_NODE_LEN)

#define TTF_UL_CIPHER_SEG_DATA_LEN      (51*1024)

#define TTF_UL_CIPHER_KEY_ADDR(BaseAddr)\
           (TTF_UL_CIPHER_SEG_DATA_ADDR(BaseAddr) + TTF_UL_CIPHER_SEG_DATA_LEN)

/* 上行加密密钥使用内存长度和基地址
   3(RLC_CPH_CFG_NUM) * 16Byte(WUE_CIPHER_KEY_LEN) = 48Byte 取整为1K */
#define TTF_UL_CIPHER_KEY_LEN           (1*1024)

#define TTF_DL_CIPHER_PARA_NODE_ADDR(BaseAddr)\
           (TTF_UL_CIPHER_KEY_ADDR(BaseAddr) + TTF_UL_CIPHER_KEY_LEN)

/* 下行解密参数节点使用内存长度和基地址
    350(WTTF_MAX_PDU_NUM_PER_DL_LOCH) * 32Byte(RLC_PDU_SEG_CIPHER_PARA_STRU) ≈ 11KByte */
#define TTF_DL_CIPHER_PARA_NODE_LEN     (11*1024)

/* Modified Mem Reduce End */
#define TTF_DL_CIPHER_SEG_DATA_ADDR(BaseAddr)\
           (TTF_DL_CIPHER_PARA_NODE_ADDR(BaseAddr) + TTF_DL_CIPHER_PARA_NODE_LEN)

/* 下行MAC-ehs SDU分段使用内存长度和基地址 */
/*13200(WTTF_MAC_EHS_SDU_SEG_DATA_NUM) * 12Byte(SEG_DATA_STRU大小) ≈ 155KByte */
#define TTF_DL_CIPHER_SEG_DATA_LEN          (155*1024)

#define TTF_DL_CIPHER_KEY_ADDR(BaseAddr)\
           (TTF_DL_CIPHER_SEG_DATA_ADDR(BaseAddr) + TTF_DL_CIPHER_SEG_DATA_LEN)

/* 下行解密密钥使用内存长度和基地址
   3(RLC_CPH_CFG_NUM) * 16Byte(WUE_CIPHER_KEY_LEN) = 48Byte 取整为1K */
#define TTF_DL_CIPHER_KEY_LEN               (1*1024)


#define TTF_UL_CIPHER_PDU_NODE_TOTAL_LEN    (TTF_UL_CIPHER_PARA_NODE_LEN + TTF_UL_CIPHER_SEG_DATA_LEN)
#define TTF_DL_CIPHER_PDU_NODE_TOTAL_LEN    (TTF_DL_CIPHER_PARA_NODE_LEN + TTF_DL_CIPHER_SEG_DATA_LEN)
/* 供MASTER设备使用的内存总长度 */
#define TTF_CIPHER_MASTER_LINK_LEN          (TTF_UL_CIPHER_PDU_NODE_TOTAL_LEN + TTF_UL_CIPHER_KEY_LEN + \
                                             TTF_DL_CIPHER_PDU_NODE_TOTAL_LEN + TTF_DL_CIPHER_KEY_LEN)

/* 控制内存池块数 */
#if ( FEATURE_ON == FEATURE_LTE )
/* LTE所需内存总长度:   WithWifi      12,854,528 字节
                        WithOutWifi   12,620,608 字节 */
/* ==================================================== */
/* IMM MEM 控制内存池块数 */
#define IMM_MEM_CTRL_MEM_CNT                        (2103)

/*******************************************************************************
const IMM_MEM_CLUSTER_CFG_INFO_STRU   g_astImmMemSharePoolClusterTableInfo[] =
{
        {864,                           1395},
#if(FEATURE_WIFI == FEATURE_ON )
        {1952,                          1855}
#else
        {1888,                          1855}
#endif
};

内存池总长度: WithWifi      4,826,240字节
              WithOutWifi   4,707,520字节
*******************************************************************************/
/* IMM MEM 共享内存各档位内存块大小 */
#define IMM_MEM_SHARE_POOL_SIZE_CLUSTER_0           (640)
#if((defined(VERSION_V3R2)) && (FEATURE_WIFI == FEATURE_ON ))
#define IMM_MEM_SHARE_POOL_SIZE_CLUSTER_1           (1728)
#else
#define IMM_MEM_SHARE_POOL_SIZE_CLUSTER_1           (1664)
#endif

/* IMM MEM 共享内存各档位内存块数 */
#define IMM_MEM_SHARE_POOL_CNT_CLUSTER_0            (1395)
#define IMM_MEM_SHARE_POOL_CNT_CLUSTER_1            (1855)

/* IMM MEM 共享内存总块数 */
#define IMM_MEM_SHARE_POOL_MEM_CNT                  (IMM_MEM_SHARE_POOL_CNT_CLUSTER_0 + IMM_MEM_SHARE_POOL_CNT_CLUSTER_1)

/* IMM MEM 共享内存总大小 */
#define IMM_MEM_SHARE_POOL_TOTAL_LEN                ((IMM_MEM_SHARE_POOL_SIZE_CLUSTER_0 * IMM_MEM_SHARE_POOL_CNT_CLUSTER_0) + \
                                                     (IMM_MEM_SHARE_POOL_SIZE_CLUSTER_1 * IMM_MEM_SHARE_POOL_CNT_CLUSTER_1))

/* TTF MEM 控制内存池块数 */
#define TTF_MEM_CTRL_MEM_CNT                        (IMM_MEM_SHARE_POOL_MEM_CNT)

#else

/* LTE所需内存总长度:   WithWifi      9,789,888 字节
                        WithOutWifi   9,656,448 字节 */
/* IMM MEM 控制内存池块数 */
#define IMM_MEM_CTRL_MEM_CNT                        (533)

/*******************************************************************************
const IMM_MEM_CLUSTER_CFG_INFO_STRU    g_astImmMemSharePoolClusterTableInfo[] =
{
        {864,                           1395},
#if(FEATURE_WIFI == FEATURE_ON )
        {1952,                          1855}
#else
        {1888,                          1855}
#endif
};


内存池总长度: WithWifi      4,826,240字节
              WithOutWifi   4,707,520字节
*******************************************************************************/
/* IMM MEM 共享内存各档位内存块大小 */
#define IMM_MEM_SHARE_POOL_SIZE_CLUSTER_0           (640)
#if(FEATURE_WIFI == FEATURE_ON )
#define IMM_MEM_SHARE_POOL_SIZE_CLUSTER_1           (1728)
#else
#define IMM_MEM_SHARE_POOL_SIZE_CLUSTER_1           (1664)
#endif

/* IMM MEM 共享内存各档位内存块数 */
#define IMM_MEM_SHARE_POOL_CNT_CLUSTER_0            (1395)
#define IMM_MEM_SHARE_POOL_CNT_CLUSTER_1            (1855)

/* IMM MEM 共享内存总块数 */
#define IMM_MEM_SHARE_POOL_MEM_CNT                  (IMM_MEM_SHARE_POOL_CNT_CLUSTER_0 + IMM_MEM_SHARE_POOL_CNT_CLUSTER_1)

/* IMM MEM 共享内存总大小 */
#define IMM_MEM_SHARE_POOL_TOTAL_LEN                ((IMM_MEM_SHARE_POOL_SIZE_CLUSTER_0 * IMM_MEM_SHARE_POOL_CNT_CLUSTER_0) + \
                                                     (IMM_MEM_SHARE_POOL_SIZE_CLUSTER_1 * IMM_MEM_SHARE_POOL_CNT_CLUSTER_1))

/* TTF MEM 控制内存池块数 */
#define TTF_MEM_CTRL_MEM_CNT                        (IMM_MEM_SHARE_POOL_MEM_CNT)

#endif

/*
    TTFMem.c中的g_ulTtfActiveMemTotalLen记录了各平台中ECS_TTF_SIZE中实际使用的总大小
    当修改加密参数内存或内存池大小时，请使用COMM_CODE_GU/Config目录对应平台的FeatureXXX.h文件，
    覆盖COMM_CODE_GU\Balong_GU_Inc\win32目录下的文件，然后运行UT工程，获得g_ulTtfActiveMemTotalLen
    确保g_ulTtfActiveMemTotalLen不超过MemoryMap.h中底软划分的ECS_TTF_SIZE
*/

/* 各种内存区域总长，目前只有加解密相关参数内存 */
#define TTF_PARA_MEM_TOTAL_LEN                  (TTF_CIPHER_MASTER_LINK_LEN)

/* A CPU 不可cache内存基地址 */
#define TTF_ACORE_POOL_MEM_START_ADDR(BaseAddr) (BaseAddr + TTF_PARA_MEM_TOTAL_LEN)

/* A CPU IMM内存池基地址 */
#define IMM_MEM_POOL_BASE_ADDR(BaseAddr)        (TTF_GET_32BYTE_ALIGN_VALUE(TTF_ACORE_POOL_MEM_START_ADDR(BaseAddr)))

/* A CPU 共享内存池起始地址 */
#define IMM_SHARE_MEM_POOL_ADDR(BaseAddr)       (IMM_MEM_POOL_BASE_ADDR(BaseAddr))

/* A CPU 所有内存池总大小 */
#define TTF_ACORE_POOL_MEM_TOTAL_LEN            (IMM_MEM_SHARE_POOL_TOTAL_LEN)

/* C CPU 不可cache内存基地址 */
#define TTF_CCORE_POOL_MEM_START_ADDR(BaseAddr) (TTF_ACORE_POOL_MEM_START_ADDR(BaseAddr) + TTF_ACORE_POOL_MEM_TOTAL_LEN)

/* C CPU TTF MEM内存池基地址 */
#define TTF_MEM_POOL_BASE_ADDR(BaseAddr)        (TTF_GET_32BYTE_ALIGN_VALUE(TTF_CCORE_POOL_MEM_START_ADDR(BaseAddr)))

/* IMM MEM 所有内存所需控制头总块数*/
#define IMM_MEM_CTRL_MEM_TOTAL_CNT              (IMM_MEM_SHARE_POOL_MEM_CNT + IMM_MEM_CTRL_MEM_CNT)




/* 用户面融合内存释放共享队列内存分配 */
#define TTF_MEM_FREE_AREA_SIZE                  (16*1024)
#define TTF_MEM_FREE_QUE_LEN                    (4000)

#define IMM_MEM_FREE_AREA_SIZE                  (16*1024)
#define IMM_MEM_FREE_QUE_LEN                    (4000)

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of TtfMemoryMap.h */

