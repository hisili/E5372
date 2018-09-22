/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : IMMmem.c
  版 本 号   : 初稿
  生成日期   : 2011年12月9日
  最近修改   :
  功能描述   : A CPU上内存管理模块
  函数列表   :
  修改历史   :
  1.日    期   : 2011年12月9日
    修改内容   : 创建文件

******************************************************************************/

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include <linux/module.h>
#include <linux/IMMmem.h>
#include <linux/skbuff.h>
#include <linux/interrupt.h>
#include <generated/TtfMemoryMap.h>
#include "BSP.h"




#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/* 存放C核查询A核内存的预留数目的地址 */
#define TTF_MEM_ACPU_FREE_MEM_CNT_ADDR      (ECS_TTF_ACPU_FREE_MEM_CNT_ADDR)

/* A 核向C核上报内存空闲块数的地址 */
unsigned long *                             g_pACpuFreeMemBlkCnt;

/* IMM MEM 数据内存池档位控制结构 */
#if (FEATURE_ON == FEATURE_LTE)
const IMM_MEM_CLUSTER_CFG_INFO_STRU         g_astImmMemSharePoolClusterTableInfo[] =
{
    {IMM_MEM_SHARE_POOL_SIZE_CLUSTER_0,         IMM_MEM_SHARE_POOL_CNT_CLUSTER_0},             /* 内存块长度必须32字节对齐 */
    {IMM_MEM_SHARE_POOL_SIZE_CLUSTER_1,         IMM_MEM_SHARE_POOL_CNT_CLUSTER_1}
};
#else
const IMM_MEM_CLUSTER_CFG_INFO_STRU         g_astImmMemSharePoolClusterTableInfo[] =
{
    {IMM_MEM_SHARE_POOL_SIZE_CLUSTER_0,         IMM_MEM_SHARE_POOL_CNT_CLUSTER_0},             /* 内存块长度必须32字节对齐 */
    {IMM_MEM_SHARE_POOL_SIZE_CLUSTER_1,         IMM_MEM_SHARE_POOL_CNT_CLUSTER_1}
};
#endif

const IMM_MEM_CLUSTER_CFG_INFO_STRU         g_astImmMemCtrlPoolClusterTableInfo[] =
{
    {0,                 IMM_MEM_CTRL_MEM_CNT}
};
EXPORT_SYMBOL(g_astImmMemCtrlPoolClusterTableInfo);


#if (FEATURE_ON == FEATURE_IMM_MEM_DEBUG)
/* IMM MEM 控制头内存池使用的内存长度 Byte(每块36B) */
#define IMM_MEM_CTRL_MEM_TOTAL_SIZE             (IMM_MEM_CTRL_MEM_TOTAL_CNT*36)
#else
/* IMM MEM 控制头内存池使用的内存长度 Byte(每块16B) */
#define IMM_MEM_CTRL_MEM_TOTAL_SIZE             (IMM_MEM_CTRL_MEM_TOTAL_CNT*16)
#endif

#if 0
#if (FEATURE_ON == FEATURE_IMM_MEM_DEBUG)
/* IMM MEM 控制头内存池使用的内存长度 Byte(每块36B) */
#if (FEATURE_ON == FEATURE_LTE)
#define IMM_MEM_CTRL_MEM_TOTAL_SIZE             (192708)        /* 统计结果 192708 字节 */
#else
#if ( FEATURE_MMU_BIG == FEATURE_ON )        /* 满足 84 M */
#define IMM_MEM_CTRL_MEM_TOTAL_SIZE             (158796)        /* 统计结果 154008 字节 */
#elif ( FEATURE_MMU_MEDIUM == FEATURE_ON )   /* 满足 42 M */
#define IMM_MEM_CTRL_MEM_TOTAL_SIZE             (136188)        /* 统计结果 132048 字节 */
#elif ( FEATURE_MMU_SMALL == FEATURE_ON )    /* 满足 28 M */
#define IMM_MEM_CTRL_MEM_TOTAL_SIZE             (127800)        /* 统计结果 127800 字节 */
#endif
#endif

#else

/* IMM MEM 控制头内存池使用的内存长度 Byte(每块16B) */
#if (FEATURE_ON == FEATURE_LTE)
#define IMM_MEM_CTRL_MEM_TOTAL_SIZE             (85648)        /* 统计结果 85648 字节 */
#else
#if ( FEATURE_MMU_BIG == FEATURE_ON )        /* 满足 84 M */
#define IMM_MEM_CTRL_MEM_TOTAL_SIZE             (70576)        /* 统计结果 70576 字节 */
#elif ( FEATURE_MMU_MEDIUM == FEATURE_ON )   /* 满足 42 M */
#define IMM_MEM_CTRL_MEM_TOTAL_SIZE             (60528)        /* 统计结果 60528 字节 */
#elif ( FEATURE_MMU_SMALL == FEATURE_ON )    /* 满足 28 M */
#define IMM_MEM_CTRL_MEM_TOTAL_SIZE             (56800)        /* 统计结果 56800 字节 */
#endif
#endif

#endif
#endif

/* IMM MEM 控制头内存池内存块已使用的长度 */
unsigned long                           g_ulImmMemCtrlMemSuffix = 0;
const  unsigned long * const            g_pulImmMemCtrlMemSuffix = &g_ulImmMemCtrlMemSuffix;
EXPORT_SYMBOL(g_pulImmMemCtrlMemSuffix);

/* IMM MEM 控制头内存池内存 */
unsigned long                           g_aulImmMemCtrlMemSpace[IMM_MEM_CTRL_MEM_TOTAL_SIZE/4];
const unsigned long * const             g_paulImmMemCtrlMemSpace = &g_aulImmMemCtrlMemSpace[0];
EXPORT_SYMBOL(g_paulImmMemCtrlMemSpace);


/* IMM MEM 数据内存池档位个数 */
const unsigned char IMM_MEM_SHARE_POOL_CLUSTER_CNT = (sizeof(g_astImmMemSharePoolClusterTableInfo)/sizeof(IMM_MEM_CLUSTER_CFG_INFO_STRU));

/* IMM MEM 控制头内存池档位个数 */
const unsigned char IMM_MEM_CTRL_POOL_CLUSTER_CNT  = (sizeof(g_astImmMemCtrlPoolClusterTableInfo)/sizeof(IMM_MEM_CLUSTER_CFG_INFO_STRU));

/* IMM MEM 控制块魔术字，检查该区域是否被踩 */
const unsigned short IMM_MAGIC_NUM = 0x4E8F;

/* 零拷贝内存池初始化标志，防止重复调用初始化函数 */
unsigned long                           g_ulImmMemInitFlag  = IMM_FALSE;

/* IMM MEM 数据内存池总控制结构 */
IMM_MEM_POOL_STRU                       g_astImmMemPool[IMM_MEM_POOL_ID_BUTT];
EXPORT_SYMBOL(g_astImmMemPool);

/* IMM MEM 外部内存池总控制结构指针 */
IMM_MEM_POOL_STRU *const                g_pstImmExtMemPool = &g_astImmMemPool[IMM_MEM_POOL_ID_EXT];
EXPORT_SYMBOL(g_pstImmExtMemPool);


/* 内存申请失败指针数据 */
unsigned short *const                   g_apusImmAllocFailCnt[IMM_MEM_POOL_ID_BUTT] =
                                        {
                                            &g_astImmMemPool[IMM_MEM_POOL_ID_SHARE].usImmAllocFailCnt,
                                            &g_astImmMemPool[IMM_MEM_POOL_ID_EXT].usImmAllocFailCnt
                                        };
EXPORT_SYMBOL(g_apusImmAllocFailCnt);


/* 内存申请失败超过门限次数 */
unsigned short *const                   g_apusImmExcThresholdCnt[IMM_MEM_POOL_ID_BUTT] =
                                        {
                                            &g_astImmMemPool[IMM_MEM_POOL_ID_SHARE].usImmExcThresholdCnt,
                                            &g_astImmMemPool[IMM_MEM_POOL_ID_EXT].usImmExcThresholdCnt
                                        };
EXPORT_SYMBOL(g_apusImmExcThresholdCnt);


/* IMM MEM MNTN管理控制全局变量 */
IMM_MEM_REG_MNTN_FUNC_STRU              g_stImmRegMntnFuc;
IMM_MEM_REG_MNTN_FUNC_STRU * const      g_pstImmRegMntnFuc = &g_stImmRegMntnFuc;
EXPORT_SYMBOL(g_pstImmRegMntnFuc);

/* IMM MEM 获取该长度所在的档位参数 */
#define IMM_GET_LEN_INDEX(pstImmMemPoolInfo, usLen) \
            ((pstImmMemPoolInfo)->aucLenIndexTable[(usLen)])

#if(IMM_DEBUG_TRACE_FUNC == FEATURE_ON)
/* IMM 内存块调试打印开关 */
unsigned char                           g_ucImmMemDebugPrint = IMM_FALSE;
EXPORT_SYMBOL(g_ucImmMemDebugPrint);
#endif

/* 每次发生内存上报事件时，每次所能内存事件次数门限 */
unsigned short                           g_usImmAllocFailCntThreshold = 1000;
EXPORT_SYMBOL(g_usImmAllocFailCntThreshold);

/* IMM MEM FREE 可维可测实体 */
IMM_MEM_FREE_MNTN_INFO_STRU             g_stImmMemFreeMntnEntity;
EXPORT_SYMBOL(g_stImmMemFreeMntnEntity);

/* 保存内存释放空指针信息 */
#define IMM_MEM_FREE_SAVE_NULL_PTR_INFO(stNullPtrInfo,usFileId,usLineNum) \
            do{\
                (stNullPtrInfo).ulImmMemFreeNullPtrCnt++;\
                (stNullPtrInfo).usImmMemFreeFileId      = (usFileId);\
                (stNullPtrInfo).usImmMemFreeLineNum     = (usLineNum);\
            }while(0)

/* 保存内存释放错误内存信息 */
#define IMM_MEM_FREE_SAVE_INVALID_MEM_INFO(astImmMemFreeInvalidMemInfo,pstTtfMem,ucType,usInvalidvalue,usFileId,usLineNum) \
            do{\
                (astImmMemFreeInvalidMemInfo).ulInvalidImmMemCnt++;\
                (astImmMemFreeInvalidMemInfo).ulInvalidImmMemAddr   = (unsigned long)pstTtfMem;\
                (astImmMemFreeInvalidMemInfo).usImmMemFreeFileId    = (usFileId);\
                (astImmMemFreeInvalidMemInfo).usImmMemFreeLineNum   = (usLineNum);\
                (astImmMemFreeInvalidMemInfo).ucImmMemInvalidType   = (ucType);\
                (astImmMemFreeInvalidMemInfo).usImmMemInvalidValue  = (usInvalidvalue);\
            }while(0)


/*****************************************************************************
 函 数 名  : IMM_MemCtrlMemAlloc
 功能描述  : 获取IMM_MEM结构中的内存，代替malloc
 输入参数  : unsigned long                 ulSize        内存大小
 输出参数  : 无
 返 回 值  : 对应的内存池结构的地址
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年12月15日
    修改内容   : 新生成函数

*****************************************************************************/
void *IMM_MemCtrlMemAlloc(unsigned long ulSize)
{
    unsigned char                       *pucSpace;
    void                                *pAlloc;


    /* 申请长度范围检查 */
    if ( IMM_MEM_CTRL_MEM_TOTAL_SIZE < (g_ulImmMemCtrlMemSuffix + ulSize) )
    {
        panic(" IMM_MemCtrlMemAlloc Fail, Ctrl MEM Over run! \n");
        return NULL;
    }

    pucSpace = (unsigned char *)g_aulImmMemCtrlMemSpace;

    pAlloc   = pucSpace + g_ulImmMemCtrlMemSuffix;

    /* 四字节对齐，控制块使用长度偏移 */
    g_ulImmMemCtrlMemSuffix += IMM_GET_4BYTE_ALIGN_VALUE(ulSize);

    return pAlloc;

} /* IMM_MemCtrlMemAlloc */


/*****************************************************************************
 函 数 名  : IMM_MemPoolInit
 功能描述  : 初始化内存池
 输入参数  : unsigned char     ucPoolId           内存池 ID
             unsigned char   **ppucDataMemAddr    不可Cache内存首地址的地址
 输出参数  :
 返 回 值  : IMM_SUCC,IMM_FAIL
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月28日
    修改内容   : 新生成函数

*****************************************************************************/
unsigned long IMM_MemPoolInit
(
    unsigned char                            ucPoolId,
    unsigned char                          **ppucDataMemAddr
)
{
    IMM_MEM_POOL_STRU                  *pstImmMemPool;
    IMM_MEM_STRU                      **ppst1stImmMem;
    IMM_MEM_STRU                       *pstImmMem;
    IMM_MEM_CLUSTER_STRU               *pstImmMemCluster;
    unsigned long                       ulImmMemCnt;
    unsigned char                       ucClusterId;
    IMM_MEM_STRU                      **ppTempMem;

    g_pACpuFreeMemBlkCnt = (unsigned long *)DRV_AXI_PHY_TO_VIRT(TTF_MEM_ACPU_FREE_MEM_CNT_ADDR);

    pstImmMemPool = IMM_MEM_GET_POOL(ucPoolId);

    /* 申请对应内存池的可用内存索引表，每个POOL申请一次，后面分割给每个cluster使用 */
    ppst1stImmMem = (IMM_MEM_STRU **)IMM_MemCtrlMemAlloc(sizeof(IMM_MEM_STRU *) * pstImmMemPool->usImmMemTotalCnt);

    if (NULL == ppst1stImmMem)
    {
        return IMM_FAIL;
    }

    /* 申请对应内存池的IMM_MEM，每个POOL申请一次，后面分割给每个cluster使用 */
    pstImmMem = (IMM_MEM_STRU *)IMM_MemCtrlMemAlloc(sizeof(IMM_MEM_STRU) * pstImmMemPool->usImmMemTotalCnt);

    if (NULL == pstImmMem)
    {
        return IMM_FAIL;
    }

    memset(ppst1stImmMem, 0, sizeof(IMM_MEM_STRU *) * pstImmMemPool->usImmMemTotalCnt);
    memset(pstImmMem, 0, sizeof(IMM_MEM_STRU) * pstImmMemPool->usImmMemTotalCnt);

    ppTempMem = ppst1stImmMem;

    /* 记录该内存池所有IMM_MEM指针，检查内存泄露使用 */
    pstImmMemPool->pstImmMemStStartAddr = pstImmMem;

    /* 建立每个cluster的IMM_MEM */
    for (ucClusterId = 0; ucClusterId < pstImmMemPool->ucClusterCnt; ++ucClusterId )
    {
        pstImmMemCluster    = &(pstImmMemPool->astClusterTable[ucClusterId]);

        /* 建立级数索引表和可用内存索引表的关系 */
        pstImmMemCluster->apstFreeStack = ppTempMem;

        for(ulImmMemCnt = 0;
            ulImmMemCnt < pstImmMemCluster->usFreeCnt;
            ulImmMemCnt ++)
        {
            pstImmMem->usMagicNum   = IMM_MAGIC_NUM;
            pstImmMem->enPoolId     = ucPoolId;
            pstImmMem->ucClusterId  = ucClusterId;

            if ( IMM_MEM_POOL_ID_EXT == pstImmMem->enPoolId )
            {
                pstImmMem->pstMemBlk = NULL;
            }
            else
            {
                pstImmMem->pstMemBlk    = *ppucDataMemAddr; /* 建立IMM_MEM_STRU和对应数据的关系 */
            }

            /* 建立可用内存索引表和IMM_MEM_STRU的关系 */
           *ppTempMem               = pstImmMem;

            pstImmMem++;
            ppTempMem++;

            *ppucDataMemAddr       += pstImmMemCluster->usLen;
        }

    }

    return IMM_SUCC;
} /* IMM_MemPoolInit */


/*****************************************************************************
 函 数 名  : IMM_MemPoolCreate
 功能描述  : 内存池对象创建，记录内存池信息
 输入参数  : IMM_MEM_POOL_CFG_INFO_STRU   *pstPoolCfg 内存池配置信息
             unsigned char       **ppucDataMemAddr  用户的级数配置首地址
 输出参数  : 无
 返 回 值  : IMM_SUCC,IMM_FAIL
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年10月28日
    修改内容   : 新生成函数


*****************************************************************************/
unsigned int IMM_MemPoolCreate
(
    const IMM_MEM_POOL_CFG_INFO_STRU   *pstPoolCfg,
    unsigned char                     **ppucDataMemAddr

)
{
    IMM_MEM_POOL_STRU                  *pstImmMemPool;
    IMM_MEM_CLUSTER_CFG_INFO_STRU      *pstPoolClusterCfgTable;
    IMM_MEM_CLUSTER_CFG_INFO_STRU      *pstClusterCfgInfo;
    IMM_MEM_CLUSTER_STRU               *pstMemCluster;
    unsigned short                      usLastClusterMaxLen;
    unsigned short                      usDataLen;
    unsigned char                       ucPoolId;
    unsigned char                       ucClusterId;
    unsigned char                      *pucDataMemAddr;
    unsigned char                       ucClusterCnt = 0;


    if ( NULL == *ppucDataMemAddr )
    {
        panic("IMM_MemPoolCreate Fail, input *ppucDataMemAddr is NULL PRT \n");
        return  IMM_FAIL;
    }

    if ( IMM_MEM_POOL_ID_BUTT <= pstPoolCfg->enPoolId )
    {
        panic("IMM_MemPoolCreate Fail, ulPoolId %d exceed the Ranger \n",
            pstPoolCfg->enPoolId);
        return  IMM_FAIL;
    }


    pucDataMemAddr  = (unsigned char *)(*ppucDataMemAddr);

    ucPoolId        = pstPoolCfg->enPoolId;

    pstImmMemPool   = IMM_MEM_GET_POOL(ucPoolId);

    /* 检查对应ID的内存池是否有被重复创建 */
    if ( IMM_TRUE == pstImmMemPool->ucUsedFlag )
    {
        panic("IMM_MemPoolCreate Fail, ulPoolId %d is already used\r\n",
            ucPoolId);
        return IMM_FAIL;
    }


    ucClusterCnt                    = pstPoolCfg->ucClusterCnt;
    pstPoolClusterCfgTable          = pstPoolCfg->pstClusterCfgInfo;

    /* 初始化内存池的通用变量 */
    pstImmMemPool->ucUsedFlag       = IMM_FALSE;
    pstImmMemPool->ucClusterCnt     = ucClusterCnt;
    pstImmMemPool->usImmMemTotalCnt = 0;

    /* 每个POOL的最后一级size最大，所以偏移到最后一级，求出该POOL的最大size */
    pstClusterCfgInfo               = pstPoolClusterCfgTable + (ucClusterCnt - 1);
    pstImmMemPool->usMaxByteLen     = pstClusterCfgInfo->usSize;


    /* 申请长度索引表，以长度为数组下标，所以最大长度要比数组个数小1 */
    if (pstImmMemPool->usMaxByteLen > (IMM_MEM_BLK_MAX_BYTE_LEN - 1))
    {
        panic("IMM_MemPoolCreate Fail, MaxByteLen:%d too big! \n",
            pstImmMemPool->usMaxByteLen);
        return IMM_FAIL;
    }

    if (pstImmMemPool->ucClusterCnt > IMM_MEM_BLK_MAX_CLUSTER_NUM)
    {
        panic("IMM_MemPoolCreate Fail, ClusterCnt:%d exceed! \n",
            pstImmMemPool->ucClusterCnt);
        return IMM_FAIL;
    }

    memset(pstImmMemPool->aucLenIndexTable, 0, IMM_MEM_BLK_MAX_BYTE_LEN);

    memset(pstImmMemPool->astClusterTable, 0,
        sizeof(IMM_MEM_CLUSTER_STRU) * IMM_MEM_BLK_MAX_CLUSTER_NUM);

    usLastClusterMaxLen   = 1;

    for (ucClusterId = 0; ucClusterId < ucClusterCnt; ucClusterId++)
    {
        pstClusterCfgInfo               = pstPoolClusterCfgTable + ucClusterId;
        pstMemCluster                   = &(pstImmMemPool->astClusterTable[ucClusterId]);

        pstMemCluster->usTotalCnt       = pstClusterCfgInfo->usCnt;
        pstMemCluster->usFreeCnt        = pstClusterCfgInfo->usCnt;
        pstMemCluster->usLen            = pstClusterCfgInfo->usSize;

       *ppucDataMemAddr                += (pstClusterCfgInfo->usSize) * (pstClusterCfgInfo->usCnt);

        for(usDataLen = usLastClusterMaxLen;
            usDataLen <= pstClusterCfgInfo->usSize;
            usDataLen++)
        {
            pstImmMemPool->aucLenIndexTable[usDataLen]      = ucClusterId;
        }

        usLastClusterMaxLen                 = usDataLen;

        pstImmMemPool->usImmMemTotalCnt    += pstClusterCfgInfo->usCnt;
    }

    if( IMM_SUCC != IMM_MemPoolInit(ucPoolId, &pucDataMemAddr))
    {
        return IMM_FAIL;
    }

    pstImmMemPool->ucUsedFlag = IMM_TRUE;

    return IMM_SUCC;
} /* IMM_MemPoolCreate */

unsigned long IMM_MemGetBaseAddr(void)
{
    BSP_DDR_SECT_QUERY             stQuery;
    BSP_DDR_SECT_INFO              stInfo;
    unsigned long                  ulBaseAddr;

    stQuery.enSectType = BSP_DDR_SECT_TYPE_TTF;
    DRV_GET_FIX_DDR_ADDR(&stQuery, &stInfo);

    ulBaseAddr = stInfo.ulSectVirtAddr;

    return (IMM_MEM_POOL_BASE_ADDR(ulBaseAddr));
}

/*****************************************************************************
 函 数 名  : IMM_MemBlkInit
 功能描述  : IMM_Mem总初始化函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : unsigned long
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月28日
    修改内容   : 新生成函数

*****************************************************************************/
unsigned long IMM_MemBlkInit(void)
{
    unsigned int                        ulRet1;
    unsigned int                        ulRet2;
    unsigned char                      *pucBaseAddr;
    IMM_MEM_POOL_CFG_INFO_STRU          stSharePoolCfgInfo;
    IMM_MEM_POOL_CFG_INFO_STRU          stExtPoolCfgInfo;
    unsigned long                       ulBaseAddr;

    /*初始化函数只会被skbuf_init 调用 */
    if ( IMM_TRUE == g_ulImmMemInitFlag )
    {
        printk( "IMM_MemBlkInit, Warning, Memory pools were already initialized!\n");

        return IMM_SUCC;
    }

    /* 设置内存池信息 */
    memset(g_astImmMemPool, 0, sizeof(IMM_MEM_POOL_STRU)* IMM_MEM_POOL_ID_BUTT);
    memset(&g_stImmRegMntnFuc, 0, sizeof(IMM_MEM_REG_MNTN_FUNC_STRU));

    /* 初始化 IMM MEM FREE 可维可测实体 */
    memset(&g_stImmMemFreeMntnEntity, 0, sizeof(IMM_MEM_FREE_MNTN_INFO_STRU));

    memset(&stSharePoolCfgInfo, 0, sizeof(IMM_MEM_POOL_CFG_INFO_STRU));
    memset(&stExtPoolCfgInfo, 0, sizeof(IMM_MEM_POOL_CFG_INFO_STRU));

    /* 保存共享内存池配置参数 */
    stSharePoolCfgInfo.pstClusterCfgInfo       = (IMM_MEM_CLUSTER_CFG_INFO_STRU *)g_astImmMemSharePoolClusterTableInfo;
    stSharePoolCfgInfo.ucClusterCnt            = IMM_MEM_SHARE_POOL_CLUSTER_CNT;
    stSharePoolCfgInfo.enPoolId                = IMM_MEM_POOL_ID_SHARE;

    /* 保存外部内存池配置参数 */
    stExtPoolCfgInfo.pstClusterCfgInfo        = (IMM_MEM_CLUSTER_CFG_INFO_STRU *)g_astImmMemCtrlPoolClusterTableInfo;
    stExtPoolCfgInfo.ucClusterCnt             = IMM_MEM_CTRL_POOL_CLUSTER_CNT;
    stExtPoolCfgInfo.enPoolId                 = IMM_MEM_POOL_ID_EXT;

    ulBaseAddr = IMM_MemGetBaseAddr();

    /* 内存块基地址，进行实地址转虚地址 */
    pucBaseAddr = (unsigned char *)ulBaseAddr;

    /* 创建共享内存池 */
    ulRet1 = IMM_MemPoolCreate(&stSharePoolCfgInfo, &pucBaseAddr);

    /* 创建外部内存池 */
    ulRet2 = IMM_MemPoolCreate(&stExtPoolCfgInfo, &pucBaseAddr);

    /* 置上已被初始化的标志 */
    g_ulImmMemInitFlag = IMM_TRUE;

    if ( ( IMM_FAIL == ulRet1 ) || ( IMM_FAIL == ulRet2 ) )
    {
        panic("Create IMM failed ulRet1 = %d, ulRet2 = %d ! \n", ulRet1, ulRet2);

        return IMM_FAIL;
    }

    return IMM_SUCC;

} /* IMM_MemBlkInit */


/*****************************************************************************
 函 数 名  : IMM_MemFreeMemCheck
 功能描述  : 检查待释放内存检查
 输入参数  : IMM_MEM_STRU *pstImm
 输出参数  : 无
 返 回 值  : unsigned long
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年12月20日
    修改内容   : 新生成函数

*****************************************************************************/
unsigned long IMM_MemFreeMemCheck(unsigned char * pucFileId, unsigned short usLineNum, IMM_MEM_STRU **ppstImm )
{
    if ( NULL == ppstImm )
    {
        printk( "Error: IMM_MemFreeMemCheck Input pstMem is NULL! FileId=%s,LineNum=%d \n", pucFileId, usLineNum);
        IMM_MEM_FREE_SAVE_NULL_PTR_INFO(g_stImmMemFreeMntnEntity.astImmMemFreeNullPPtrInfo,0,usLineNum);
        return IMM_FAIL;
    }

    if ( NULL == *ppstImm )
    {
        printk("Error: IMM_MemFreeMemCheck Input *pstMem is NULL! FileId=%s,LineNum=%d \n", pucFileId, usLineNum);
        IMM_MEM_FREE_SAVE_NULL_PTR_INFO(g_stImmMemFreeMntnEntity.astImmMemFreeNullPtrInfo,0,usLineNum);
        return IMM_FAIL;
    }

    if ( IMM_MAGIC_NUM != (*ppstImm)->usMagicNum)
    {
        printk("IMM_MemFreeMemCheck, Magic Num %d is invalid! FileId=%s,LineNum=%d \n",
                    (int)(*ppstImm)->usMagicNum, pucFileId, usLineNum);
        IMM_MEM_FREE_SAVE_INVALID_MEM_INFO(g_stImmMemFreeMntnEntity.astImmMemFreeInvalidMemInfo,*ppstImm,IMM_INVALID_MEM_TYPE_MAGICNUM,(*ppstImm)->usMagicNum,0,usLineNum);

        return IMM_FAIL;
    }

    if( IMM_MEM_POOL_ID_BUTT <= (*ppstImm)->enPoolId )
    {
        printk("IMM_MemFreeMemCheck, PoolId %d is invalid! FileId=%s,LineNum=%d \n",
                    (int)(*ppstImm)->enPoolId,pucFileId, usLineNum );

        IMM_MEM_FREE_SAVE_INVALID_MEM_INFO(g_stImmMemFreeMntnEntity.astImmMemFreeInvalidMemInfo,*ppstImm,IMM_INVALID_MEM_TYPE_POOLID,(*ppstImm)->enPoolId,0,usLineNum);

        return IMM_FAIL;
    }

    return IMM_SUCC;
}


#if (FEATURE_ON == FEATURE_IMM_MEM_DEBUG)
/*****************************************************************************
 函 数 名  : IMM_MemGetSlice
 功能描述  : 保存内存申请过程中的CPU SILCE
 输入参数  : void
 输出参数  : 无
 返 回 值  : unsigned long CPU SLICE
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年12月29日
    修改内容   : 新生成函数

*****************************************************************************/
unsigned long IMM_MemGetSlice(void)
{
    if ( NULL == g_pstImmRegMntnFuc->pImmOmGetSliceFunc )
    {
        return 0;
    }

    return g_pstImmRegMntnFuc->pImmOmGetSliceFunc();
}/* IMM_MemGetSlice */


/*****************************************************************************
 函 数 名  : IMM_MemSaveAllocDebugInfo
 功能描述  : 保存内存申请过程中的可维可测信息
 输入参数  : IMM_MEM_STRU *pstImmMem 数据块指针
             unsigned short usFileID 申请内存文件ID
             unsigned short usLineNum 申请内存行号
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年12月29日
    修改内容   : 新生成函数

*****************************************************************************/
void IMM_MemSaveAllocDebugInfo
(
    IMM_MEM_STRU *pstImmMem,
    unsigned short usFileID,
    unsigned short usLineNum
)
{
    IMM_BLK_MEM_DEBUG_STRU             *pstDbgInfo  =  &pstImmMem->stDbgInfo;


    IMM_DEBUG_TRACE_FUNC_ENTER();

    if ( NULL != pstDbgInfo )
    {
        pstDbgInfo->enMemStateFlag  = MEM_BLK_STATE_ALLOC;
        pstDbgInfo->usAllocFileID   = usFileID;
        pstDbgInfo->usAllocLineNum  = usLineNum;
        pstDbgInfo->ulAllocTick     = IMM_MemGetSlice();
        pstDbgInfo->usTraceFileID   = usFileID;
        pstDbgInfo->usTraceLineNum  = usLineNum;
    }
    else
    {
        printk("WARNING: IMM_MemSaveAllocDebugInfo, Blk Mem debug info is null! FileId=%d,LineNum=%d!\n",usFileID,usLineNum);
    }

    IMM_DEBUG_TRACE_FUNC_LEAVE();

    return;
}/* IMM_MemSaveAllocDebugInfo */


/*****************************************************************************
 函 数 名  : IMM_MemFreeSaveDebugInfo
 功能描述  : IMM内存释放函数填写Debug信息, 仅由Free函数内部调用
 输入参数  : IMM_MEM_STRU *pstImmMem 待释放的内存块指针
 输出参数  : 无
 返 回 值  : IMM_FAIL 内存块状态非法
             IMM_SUCC 保存释放信息成功
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年12月19日
    修改内容   : 新生成函数

*****************************************************************************/
unsigned char IMM_MemSaveFreeDebugInfo(unsigned char * pucFileId, unsigned short usLineNum, IMM_MEM_STRU *pstImmMem)
{
    unsigned long                ulMemAddr;
    IMM_BLK_MEM_DEBUG_STRU      *pstImmMemDebugInfo = &pstImmMem->stDbgInfo;


    IMM_DEBUG_TRACE_FUNC_ENTER();

    ulMemAddr = (unsigned long)pstImmMem;

    /*pstImmMem 地址范围检查*/
    if (( (unsigned long)g_paulImmMemCtrlMemSpace > ulMemAddr )
        ||(((unsigned long)g_paulImmMemCtrlMemSpace + (*g_pulImmMemCtrlMemSuffix)) < ulMemAddr))
    {
        IMM_MEM_FREE_SAVE_INVALID_MEM_INFO(g_stImmMemFreeMntnEntity.astImmMemFreeInvalidMemInfo, pstImmMem,IMM_INVALID_MEM_TYPE_CTRLADDR, 0, 0, usLineNum);
        printk( "Error: IMM_MemFreeSaveDebugInfo, Mem addr %d is invalid! \n", (int)pstImmMem);

        return IMM_FAIL;
    }

    /* 判断内存指针状态是否正确 */
    if ( MEM_BLK_STATE_ALLOC != pstImmMemDebugInfo->enMemStateFlag )
    {
        IMM_MEM_FREE_SAVE_INVALID_MEM_INFO(g_stImmMemFreeMntnEntity.astImmMemFreeInvalidMemInfo, pstImmMem,IMM_INVALID_MEM_TYPE_STUTAS, (unsigned short)pstImmMemDebugInfo->enMemStateFlag, 0, usLineNum);
        printk("Error: IMM_MemFreeSaveDebugInfo, free a free status block is not allowed \n");

        printk("Double Free Second   FileID %s, LineNum %d \n",pucFileId,usLineNum);

        printk("Double Free First   FileID %s, LineNum %d \n",(char *)pstImmMemDebugInfo->ulTraceTick,pstImmMemDebugInfo->usAllocLineNum);
        return IMM_FAIL;
    }

    pstImmMemDebugInfo->enMemStateFlag   = MEM_BLK_STATE_FREE;
    pstImmMemDebugInfo->ulAllocTick      = IMM_MemGetSlice();
    pstImmMemDebugInfo->ulTraceTick      = (unsigned long)pucFileId;
    pstImmMemDebugInfo->usAllocLineNum   = usLineNum;

    IMM_DEBUG_TRACE_FUNC_LEAVE();

    return IMM_SUCC;

}/* IMM_MemFreeSaveDebugInfo */
#endif


/*****************************************************************************
 函 数 名  : IMM_MemPoolGet
 功能描述  : 获取内存池地址
 输入参数  : unsigned char ucPoolId  内存池ID
 输出参数  : 无
 返 回 值  : 对应的内存池结构的地址
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年10月28日
    作    者   : S00164817
    修改内容   : 新生成函数

*****************************************************************************/
IMM_MEM_POOL_STRU *IMM_MemPoolGet(unsigned char ucPoolId)
{
    if ( IMM_MEM_POOL_ID_BUTT > ucPoolId )
    {
        return &g_astImmMemPool[ucPoolId];
    }
    else
    {
        return NULL;
    }

} /* IMM_MemPoolGet */
EXPORT_SYMBOL(IMM_MemPoolGet);

/*****************************************************************************
 函 数 名  : IMM_ZcGetLocalFreeMemCnt
 功能描述  : 对外提供接口, 获取内存块使用情况
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年12月1日
    修改内容   : 新生成函数

*****************************************************************************/
unsigned long IMM_ZcGetLocalFreeMemCnt(void)
{
    IMM_DEBUG_TRACE_FUNC_ENTER();
    IMM_DEBUG_TRACE_FUNC_LEAVE();

    return *g_pACpuFreeMemBlkCnt;
} /* IMM_ZcGetLocalFreeMemCnt */
EXPORT_SYMBOL(IMM_ZcGetLocalFreeMemCnt);

/*****************************************************************************
 函 数 名  : IMM_ZcSetLocalFreeMemCnt
 功能描述  : 设置A核最大档的空闲块的数量。
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年12月1日
    修改内容   : 新生成函数

*****************************************************************************/
void IMM_ZcSetLocalFreeMemCnt(unsigned long ulMemValue)
{
    IMM_DEBUG_TRACE_FUNC_ENTER();
    IMM_DEBUG_TRACE_FUNC_LEAVE();

    *g_pACpuFreeMemBlkCnt = ulMemValue;
    return;
}/* IMM_ZcSetLocalFreeMemCnt */


/*****************************************************************************
 函 数 名  : IMM_MemAlloc_Debug
 功能描述  : IMM MEM申请函数
 输入参数  : unsigned long ulFileID    申请的文件名
             unsigned long ulLineNum   申请的行号
             unsigned long ulPid       对应的Pid
             unsigned long ulPoolId    对应的内存池
             unsigned short usLen       申请的IMM_MEM_ST的长度
 输出参数  : 无
 返 回 值  : IMM_MEM_STRU*  申请成功时申请的IMM_MEM_ST地址
             NULL   申请失败
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年10月28日
    修改内容   : 新生成函数

*****************************************************************************/
IMM_MEM_STRU *IMM_MemAlloc_Debug(unsigned short usFileID, unsigned short usLineNum,
                        unsigned char ucPoolId, unsigned int ulLen)
{
    IMM_MEM_POOL_STRU              *pstImmMemPool;
    IMM_MEM_CLUSTER_STRU           *pstImmMemCluster;
    IMM_MEM_STRU                   *pMem;
    unsigned long                   ulMaxClusterFreeCnt;
    unsigned char                   ucClusterId;
    unsigned char                   ucMostFitLev;
    unsigned long                   ulSaveFlags  = 0;


    IMM_DEBUG_TRACE_FUNC_ENTER();

    if ( IMM_MEM_POOL_ID_BUTT <= ucPoolId )
    {
        printk("IMM_MemBlkAlloc_Debug,Poolid %d is invalid,FileId=%d,LineNum=%d!\n", ucPoolId,usFileID,usLineNum);
        return NULL;
    }

     /* 获取内存池 */
    pstImmMemPool = IMM_MEM_GET_POOL(ucPoolId);

    /* 检查内存池是否已经初始化 */
    if ( IMM_TRUE != pstImmMemPool->ucUsedFlag )
    {
        printk("IMM_MemBlkAlloc_Debug,Pool %d is not used,FileId=%d,LineNum=%d!\n", ucPoolId,usFileID,usLineNum);
        return NULL;
    }

     /* 申请长度是否在该内存内 */
    if ( IMM_MEM_POOL_ID_EXT == ucPoolId )
    {
        ulLen = 0;
    }
    else
    {
        if ( (0 == ulLen ) || (ulLen > pstImmMemPool->usMaxByteLen) )
        {
            printk("IMM_MemBlkAlloc_Debug,usLen %d exceed the Pool %d ranger,FileId=%d,LineNum=%d!\n", (int)ulLen, ucPoolId,usFileID,usLineNum);
            return NULL;
        }
    }

    /* 从内存池的多个簇里寻找合适的IMM_MEM */
    ucMostFitLev = IMM_GET_LEN_INDEX(pstImmMemPool, ulLen);

    for(ucClusterId = ucMostFitLev;
        ucClusterId < pstImmMemPool->ucClusterCnt;
         ++ucClusterId )
    {
        pstImmMemCluster  = &(pstImmMemPool->astClusterTable[ucClusterId]);

        local_irq_save(ulSaveFlags);

        if (0 != pstImmMemCluster->usFreeCnt)
        {
            /* 获取一个没有使用的IMM_MEM_ST结点 */
            pMem    = pstImmMemCluster->apstFreeStack[--pstImmMemCluster->usFreeCnt];
            ++pstImmMemPool->usImmMemUsedCnt;
            ulMaxClusterFreeCnt = pstImmMemPool->astClusterTable[pstImmMemPool->ucClusterCnt - 1].usFreeCnt;

            local_irq_restore(ulSaveFlags);

            /* 向流控模块上报内存申请事件 */
            if (NULL != pstImmMemPool->pMemAllocEvent)
            {
                pstImmMemPool->pMemAllocEvent( ulMaxClusterFreeCnt );
                IMM_ZcSetLocalFreeMemCnt(ulMaxClusterFreeCnt);
            }

#if (FEATURE_ON == FEATURE_IMM_MEM_DEBUG)
            IMM_MemSaveAllocDebugInfo( pMem, usFileID, usLineNum );
#endif
            IMM_DEBUG_TRACE_FUNC_LEAVE();

            return pMem;
        }


        local_irq_restore(ulSaveFlags);
    }

    (*g_apusImmAllocFailCnt[ucPoolId])++;

    if ( *g_apusImmAllocFailCnt[ucPoolId] < g_usImmAllocFailCntThreshold )
    {
        return NULL;
    }

    *g_apusImmAllocFailCnt[ucPoolId] = 0;
    (*g_apusImmExcThresholdCnt[ucPoolId])++;

    /* 向SDT上报内存申请失败事件 */
    if ( NULL != g_pstImmRegMntnFuc->pImmMemEventRptFunc)
    {
        g_pstImmRegMntnFuc->pImmMemEventRptFunc(ucPoolId, IMM_MEM_TRIG_TYPE_ALLOC_FAIL);
    }


    return NULL;
} /* IMM_MemAlloc_Debug */
EXPORT_SYMBOL(IMM_MemAlloc_Debug);


/*****************************************************************************
 函 数 名  : IMM_MemFree_Debug
 功能描述  : 释放IMM MEM BLK内存
 输入参数  : IMM_MEM_STRU *pstImmMem      释放的数据结构
 输出参数  : 无
 返 回 值  : IMM_MEM的下一个
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年11月28号
    修改内容   : 新生成函数

*****************************************************************************/
void IMM_MemFree_Debug(unsigned char * pucFileId, unsigned short usLineNum, IMM_MEM_STRU **ppstMem)
{
    IMM_MEM_POOL_STRU                  *pstImmMemPool;
    IMM_MEM_CLUSTER_STRU               *pstImmMemCluster;
    IMM_MEM_STRU                       *pstImmMem;
    unsigned long                       ulMaxClusterFreeCnt;
    unsigned long                       ulSaveFlags  = 0;
    unsigned char                      *pucExtMem    = NULL;


    IMM_DEBUG_TRACE_FUNC_ENTER();

    /* 待释放内存合法性检查 */
    if ( IMM_SUCC != IMM_MemFreeMemCheck(pucFileId, usLineNum, ppstMem) )
    {
        printk( "Error: IMM_MemFree_Debug input ppstMem is invalid! FileId=%s,LineNum=%d \n", pucFileId, usLineNum);
        return;
    }

    pstImmMem       = *ppstMem;
    pstImmMemPool   = IMM_MEM_GET_POOL(pstImmMem->enPoolId);

    /* 检查对应的内存是否使用 */
    if (IMM_TRUE != pstImmMemPool->ucUsedFlag)
    {
        printk("IMM_MemFree_Debug, Pool %d is not used! \n",
                pstImmMem->enPoolId);
        return ;
    }

#if (FEATURE_ON == FEATURE_IMM_MEM_DEBUG)
    if ( IMM_SUCC != IMM_MemSaveFreeDebugInfo(pucFileId, usLineNum, pstImmMem) )
    {
        printk("IMM_MemFree_Debug, free IMM MEM fail! FileId=%s, LineNum=%d \n", pucFileId, usLineNum);
        return;
    }
#endif

    /* 外部内存池，需要释放外部内存块 */
    if ( IMM_MEM_POOL_ID_EXT == pstImmMem->enPoolId )
    {
        pucExtMem = pstImmMem->pstMemBlk;
        /* 清空外部内存块指针 */
        pstImmMem->pstMemBlk = NULL;
    }

    /* 获取内存块所在的档位 */
    pstImmMemCluster = &(pstImmMemPool->astClusterTable[pstImmMem->ucClusterId]);

    local_irq_save(ulSaveFlags);

    if (pstImmMemCluster->usFreeCnt >= pstImmMemCluster->usTotalCnt)
    {
        local_irq_restore(ulSaveFlags);
        printk("IMM_MemFree_Debug, free cnt error:FreeCnt %d, TotalCnt %d ! \n",
                pstImmMemCluster->usFreeCnt, pstImmMemCluster->usTotalCnt);

        return ;
    }

    /* 将内存块挂接回内存池 */
    pstImmMemCluster->apstFreeStack[pstImmMemCluster->usFreeCnt] = pstImmMem;
    ++pstImmMemCluster->usFreeCnt;
    --pstImmMemPool->usImmMemUsedCnt;
    /* 计算内存池最大档位剩余内存块数 */
    ulMaxClusterFreeCnt = pstImmMemPool->astClusterTable[pstImmMemPool->ucClusterCnt - 1].usFreeCnt;

    local_irq_restore(ulSaveFlags);

    if (( NULL != pstImmMemPool->pMemExtFreeFunc )&&( NULL != pucExtMem ))
    {
        /* 释放外部内存 */
        pstImmMemPool->pMemExtFreeFunc(pucExtMem);
    }

    /* 向流控模块上报内存释放事件 */
    if ( NULL != pstImmMemPool->pMemFreeEvent )
    {
        pstImmMemPool->pMemFreeEvent(ulMaxClusterFreeCnt );
        IMM_ZcSetLocalFreeMemCnt(ulMaxClusterFreeCnt);
    }

    *ppstMem = NULL;

    IMM_DEBUG_TRACE_FUNC_LEAVE();

    return ;
} /* IMM_MemFree_Debug */
EXPORT_SYMBOL(IMM_MemFree_Debug);


/*****************************************************************************
 函 数 名  :  IMM_MemRegExtFreeCallBack
 功能描述  :  注册IMM MEM外部内存池释放函数
 输入参数  :  enPoolId   -- 内存池ID
              pMemExtFreeFunc   -- 注册的内存释放回调函数
 输出参数  :  无
 返 回 值  :  IMM_FAIL 注册失败
              IMM_SUCC 注册成功
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年12月12日
    修改内容   : 新生成函数

*****************************************************************************/
unsigned long IMM_MemRegExtFreeCallBack
(
    IMM_MEM_EXT_FREE_CALLBACK           pMemExtFreeFunc
)
{
    IMM_MEM_POOL_STRU * const           pstIMMMemPool = g_pstImmExtMemPool;


    if ( NULL == pMemExtFreeFunc )
    {
        return IMM_FAIL;
    }

    pstIMMMemPool->pMemExtFreeFunc = pMemExtFreeFunc;

    return IMM_SUCC;
} /* IMM_MemRegExtFreeCallBack */
EXPORT_SYMBOL(IMM_MemRegExtFreeCallBack);


/*****************************************************************************
 函 数 名  : IMM_MemRegZcFuncCallBack
 功能描述  : 提供IMM可维可测代码的函数注册
 输入参数  : 无
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年12月15日
    修改内容   : 新生成函数

*****************************************************************************/
void IMM_MemRegMntnFuncCallBack
(
    IMM_OM_GET_SLICE_CALLBACK               pImmOmGetSliceFunc,
    IMM_MEM_USED_INFO_EVENT_RPT_CALLBACK    pImmMemEventRptFunc
)
{
    /* 参数检查，pImmOmGetSliceFunc 可以在非DEBUG模式下为空 */
    if ( NULL == pImmMemEventRptFunc )
    {
        printk("IMM_MemRegPsFuncCallBack fail: invalid input paras! \n");
        return;
    }

    g_pstImmRegMntnFuc->pImmOmGetSliceFunc    = pImmOmGetSliceFunc;
    g_pstImmRegMntnFuc->pImmMemEventRptFunc   = pImmMemEventRptFunc;

    return;
}
EXPORT_SYMBOL(IMM_MemRegMntnFuncCallBack);


/*****************************************************************************
 函 数 名  : IMM_MemRegEventCallBack
 功能描述  : 提供IMM协议栈部分代码的函数注册
 输入参数  : 无
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年12月15日
    修改内容   : 新生成函数

*****************************************************************************/
unsigned long IMM_MemRegEventCallBack
(
    IMM_MEM_POOL_ID_ENUM_UINT8          enPoolId,
    IMM_MEM_EVENT_CALLBACK              pMemAllocEvent,
    IMM_MEM_EVENT_CALLBACK              pMemFreeEvent
)
{
    IMM_MEM_POOL_STRU              *pstImmMemPool;
    unsigned long                   ulMaxClusterFreeCnt;

    pstImmMemPool   = IMM_MemPoolGet(enPoolId);

    if ( NULL == pstImmMemPool )
    {
        return IMM_FAIL;
    }

    pstImmMemPool->pMemAllocEvent = pMemAllocEvent;
    pstImmMemPool->pMemFreeEvent  = pMemFreeEvent;

    /* 获取最大档位内存的剩余块数 */
    ulMaxClusterFreeCnt = pstImmMemPool->astClusterTable[pstImmMemPool->ucClusterCnt - 1].usFreeCnt;
    IMM_ZcSetLocalFreeMemCnt(ulMaxClusterFreeCnt);

    return IMM_SUCC;
} /* IMM_MemRegEventCallBack */
EXPORT_SYMBOL(IMM_MemRegEventCallBack);


/*****************************************************************************
 函 数 名  : IMM_MemFreeShowMntnInfo
 功能描述  : 打印 IMM MEM 内存可维可测信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年3月7日
    修改内容   : 新生成函数

*****************************************************************************/
void IMM_MemFreeShowMntnInfo( void )
{
    IMM_MEM_FREE_INVALIED_MEM_INFO_STRU    *pstInvalidMemInfo   = &g_stImmMemFreeMntnEntity.astImmMemFreeInvalidMemInfo;
    IMM_MEM_FREE_NULL_PTR_INFO_STRU        *pstNullPtrInfo      = &g_stImmMemFreeMntnEntity.astImmMemFreeNullPtrInfo;
    IMM_MEM_FREE_NULL_PTR_INFO_STRU        *pstNullPPtrInfo     = &g_stImmMemFreeMntnEntity.astImmMemFreeNullPPtrInfo;


    printk( "IMM MEM FREE 可维可测信息 :\n");
    printk( "==========================================\n");
    printk( "IMM MEM FREE 非法内存信息 :\n");
    printk( "IMM MEM FREE 收到的非法内存个数       : %u \n", (unsigned int)pstInvalidMemInfo->ulInvalidImmMemCnt);
    printk( "IMM MEM FREE 非法IMM MEM 内存地址     : 0x%u \n", (unsigned int)pstInvalidMemInfo->ulInvalidImmMemAddr);
    printk( "IMM MEM FREE 非法IMM MEM 内存文件ID   : %d \n", pstInvalidMemInfo->usImmMemFreeFileId);
    printk( "IMM MEM FREE 非法IMM MEM 内存行号信息 : %d \n", pstInvalidMemInfo->usImmMemFreeLineNum);
    printk( "IMM MEM FREE 非法IMM MEM 内存类型     : %d \n", pstInvalidMemInfo->ucImmMemInvalidType);
    printk( "IMM MEM FREE 非法IMM MEM 内存值       : %d \n", pstInvalidMemInfo->usImmMemInvalidValue);
    printk( "IMM MEM 内存控制头起始地址            : 0x%u ~ 0x%u \n\n", (unsigned int)g_paulImmMemCtrlMemSpace,((unsigned int)((unsigned int)g_paulImmMemCtrlMemSpace + (*g_pulImmMemCtrlMemSuffix))) );


    printk( "==========================================\n");
    printk( "IMM MEM FREE 空指针信息 :\n");
    printk( "IMM MEM FREE 一级指针为空的次数    : %u \n", (unsigned int)pstNullPtrInfo->ulImmMemFreeNullPtrCnt);
    printk( "IMM MEM FREE 输入指针文件ID        : %d \n", pstNullPtrInfo->usImmMemFreeFileId);
    printk( "IMM MEM FREE 输入指针内存行号信息  : %d \n", pstNullPtrInfo->usImmMemFreeLineNum);

    printk( "IMM MEM FREE 二级指针为空的次数    : %u \n", (unsigned int)pstNullPPtrInfo->ulImmMemFreeNullPtrCnt);
    printk( "IMM MEM FREE 输入指针内存文件ID    : %d \n", pstNullPPtrInfo->usImmMemFreeFileId);
    printk( "IMM MEM FREE 输入指针内存行号信息  : %d \n", pstNullPPtrInfo->usImmMemFreeLineNum);

    printk( "IMM MEM Pool Status: IMM_MEM_POOL_ID_DL_SHARE %d\n", g_astImmMemPool[IMM_MEM_POOL_ID_SHARE].ucUsedFlag);
    printk( "                     IMM_MEM_POOL_ID_EXT      %d\n", g_astImmMemPool[IMM_MEM_POOL_ID_EXT].ucUsedFlag);

    printk( "IMM_MEM_POOL_ID_EXT 释放函数      : 0x%u\n", (unsigned int)g_astImmMemPool[IMM_MEM_POOL_ID_EXT].pMemExtFreeFunc);
    printk( "IMM Ext MEM FREE 失败次数         : %u \n", (unsigned int)g_stImmMemFreeMntnEntity.ulImmMemExtFreeFailCnt);

}
EXPORT_SYMBOL(IMM_MemFreeShowMntnInfo);





#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

