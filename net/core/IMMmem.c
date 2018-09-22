/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : IMMmem.c
  �� �� ��   : ����
  ��������   : 2011��12��9��
  ����޸�   :
  ��������   : A CPU���ڴ����ģ��
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2011��12��9��
    �޸�����   : �����ļ�

******************************************************************************/

/*****************************************************************************
  1 ͷ�ļ�����
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
  2 ȫ�ֱ�������
*****************************************************************************/
/* ���C�˲�ѯA���ڴ��Ԥ����Ŀ�ĵ�ַ */
#define TTF_MEM_ACPU_FREE_MEM_CNT_ADDR      (ECS_TTF_ACPU_FREE_MEM_CNT_ADDR)

/* A ����C���ϱ��ڴ���п����ĵ�ַ */
unsigned long *                             g_pACpuFreeMemBlkCnt;

/* IMM MEM �����ڴ�ص�λ���ƽṹ */
#if (FEATURE_ON == FEATURE_LTE)
const IMM_MEM_CLUSTER_CFG_INFO_STRU         g_astImmMemSharePoolClusterTableInfo[] =
{
    {IMM_MEM_SHARE_POOL_SIZE_CLUSTER_0,         IMM_MEM_SHARE_POOL_CNT_CLUSTER_0},             /* �ڴ�鳤�ȱ���32�ֽڶ��� */
    {IMM_MEM_SHARE_POOL_SIZE_CLUSTER_1,         IMM_MEM_SHARE_POOL_CNT_CLUSTER_1}
};
#else
const IMM_MEM_CLUSTER_CFG_INFO_STRU         g_astImmMemSharePoolClusterTableInfo[] =
{
    {IMM_MEM_SHARE_POOL_SIZE_CLUSTER_0,         IMM_MEM_SHARE_POOL_CNT_CLUSTER_0},             /* �ڴ�鳤�ȱ���32�ֽڶ��� */
    {IMM_MEM_SHARE_POOL_SIZE_CLUSTER_1,         IMM_MEM_SHARE_POOL_CNT_CLUSTER_1}
};
#endif

const IMM_MEM_CLUSTER_CFG_INFO_STRU         g_astImmMemCtrlPoolClusterTableInfo[] =
{
    {0,                 IMM_MEM_CTRL_MEM_CNT}
};
EXPORT_SYMBOL(g_astImmMemCtrlPoolClusterTableInfo);


#if (FEATURE_ON == FEATURE_IMM_MEM_DEBUG)
/* IMM MEM ����ͷ�ڴ��ʹ�õ��ڴ泤�� Byte(ÿ��36B) */
#define IMM_MEM_CTRL_MEM_TOTAL_SIZE             (IMM_MEM_CTRL_MEM_TOTAL_CNT*36)
#else
/* IMM MEM ����ͷ�ڴ��ʹ�õ��ڴ泤�� Byte(ÿ��16B) */
#define IMM_MEM_CTRL_MEM_TOTAL_SIZE             (IMM_MEM_CTRL_MEM_TOTAL_CNT*16)
#endif

#if 0
#if (FEATURE_ON == FEATURE_IMM_MEM_DEBUG)
/* IMM MEM ����ͷ�ڴ��ʹ�õ��ڴ泤�� Byte(ÿ��36B) */
#if (FEATURE_ON == FEATURE_LTE)
#define IMM_MEM_CTRL_MEM_TOTAL_SIZE             (192708)        /* ͳ�ƽ�� 192708 �ֽ� */
#else
#if ( FEATURE_MMU_BIG == FEATURE_ON )        /* ���� 84 M */
#define IMM_MEM_CTRL_MEM_TOTAL_SIZE             (158796)        /* ͳ�ƽ�� 154008 �ֽ� */
#elif ( FEATURE_MMU_MEDIUM == FEATURE_ON )   /* ���� 42 M */
#define IMM_MEM_CTRL_MEM_TOTAL_SIZE             (136188)        /* ͳ�ƽ�� 132048 �ֽ� */
#elif ( FEATURE_MMU_SMALL == FEATURE_ON )    /* ���� 28 M */
#define IMM_MEM_CTRL_MEM_TOTAL_SIZE             (127800)        /* ͳ�ƽ�� 127800 �ֽ� */
#endif
#endif

#else

/* IMM MEM ����ͷ�ڴ��ʹ�õ��ڴ泤�� Byte(ÿ��16B) */
#if (FEATURE_ON == FEATURE_LTE)
#define IMM_MEM_CTRL_MEM_TOTAL_SIZE             (85648)        /* ͳ�ƽ�� 85648 �ֽ� */
#else
#if ( FEATURE_MMU_BIG == FEATURE_ON )        /* ���� 84 M */
#define IMM_MEM_CTRL_MEM_TOTAL_SIZE             (70576)        /* ͳ�ƽ�� 70576 �ֽ� */
#elif ( FEATURE_MMU_MEDIUM == FEATURE_ON )   /* ���� 42 M */
#define IMM_MEM_CTRL_MEM_TOTAL_SIZE             (60528)        /* ͳ�ƽ�� 60528 �ֽ� */
#elif ( FEATURE_MMU_SMALL == FEATURE_ON )    /* ���� 28 M */
#define IMM_MEM_CTRL_MEM_TOTAL_SIZE             (56800)        /* ͳ�ƽ�� 56800 �ֽ� */
#endif
#endif

#endif
#endif

/* IMM MEM ����ͷ�ڴ���ڴ����ʹ�õĳ��� */
unsigned long                           g_ulImmMemCtrlMemSuffix = 0;
const  unsigned long * const            g_pulImmMemCtrlMemSuffix = &g_ulImmMemCtrlMemSuffix;
EXPORT_SYMBOL(g_pulImmMemCtrlMemSuffix);

/* IMM MEM ����ͷ�ڴ���ڴ� */
unsigned long                           g_aulImmMemCtrlMemSpace[IMM_MEM_CTRL_MEM_TOTAL_SIZE/4];
const unsigned long * const             g_paulImmMemCtrlMemSpace = &g_aulImmMemCtrlMemSpace[0];
EXPORT_SYMBOL(g_paulImmMemCtrlMemSpace);


/* IMM MEM �����ڴ�ص�λ���� */
const unsigned char IMM_MEM_SHARE_POOL_CLUSTER_CNT = (sizeof(g_astImmMemSharePoolClusterTableInfo)/sizeof(IMM_MEM_CLUSTER_CFG_INFO_STRU));

/* IMM MEM ����ͷ�ڴ�ص�λ���� */
const unsigned char IMM_MEM_CTRL_POOL_CLUSTER_CNT  = (sizeof(g_astImmMemCtrlPoolClusterTableInfo)/sizeof(IMM_MEM_CLUSTER_CFG_INFO_STRU));

/* IMM MEM ���ƿ�ħ���֣����������Ƿ񱻲� */
const unsigned short IMM_MAGIC_NUM = 0x4E8F;

/* �㿽���ڴ�س�ʼ����־����ֹ�ظ����ó�ʼ������ */
unsigned long                           g_ulImmMemInitFlag  = IMM_FALSE;

/* IMM MEM �����ڴ���ܿ��ƽṹ */
IMM_MEM_POOL_STRU                       g_astImmMemPool[IMM_MEM_POOL_ID_BUTT];
EXPORT_SYMBOL(g_astImmMemPool);

/* IMM MEM �ⲿ�ڴ���ܿ��ƽṹָ�� */
IMM_MEM_POOL_STRU *const                g_pstImmExtMemPool = &g_astImmMemPool[IMM_MEM_POOL_ID_EXT];
EXPORT_SYMBOL(g_pstImmExtMemPool);


/* �ڴ�����ʧ��ָ������ */
unsigned short *const                   g_apusImmAllocFailCnt[IMM_MEM_POOL_ID_BUTT] =
                                        {
                                            &g_astImmMemPool[IMM_MEM_POOL_ID_SHARE].usImmAllocFailCnt,
                                            &g_astImmMemPool[IMM_MEM_POOL_ID_EXT].usImmAllocFailCnt
                                        };
EXPORT_SYMBOL(g_apusImmAllocFailCnt);


/* �ڴ�����ʧ�ܳ������޴��� */
unsigned short *const                   g_apusImmExcThresholdCnt[IMM_MEM_POOL_ID_BUTT] =
                                        {
                                            &g_astImmMemPool[IMM_MEM_POOL_ID_SHARE].usImmExcThresholdCnt,
                                            &g_astImmMemPool[IMM_MEM_POOL_ID_EXT].usImmExcThresholdCnt
                                        };
EXPORT_SYMBOL(g_apusImmExcThresholdCnt);


/* IMM MEM MNTN�������ȫ�ֱ��� */
IMM_MEM_REG_MNTN_FUNC_STRU              g_stImmRegMntnFuc;
IMM_MEM_REG_MNTN_FUNC_STRU * const      g_pstImmRegMntnFuc = &g_stImmRegMntnFuc;
EXPORT_SYMBOL(g_pstImmRegMntnFuc);

/* IMM MEM ��ȡ�ó������ڵĵ�λ���� */
#define IMM_GET_LEN_INDEX(pstImmMemPoolInfo, usLen) \
            ((pstImmMemPoolInfo)->aucLenIndexTable[(usLen)])

#if(IMM_DEBUG_TRACE_FUNC == FEATURE_ON)
/* IMM �ڴ����Դ�ӡ���� */
unsigned char                           g_ucImmMemDebugPrint = IMM_FALSE;
EXPORT_SYMBOL(g_ucImmMemDebugPrint);
#endif

/* ÿ�η����ڴ��ϱ��¼�ʱ��ÿ�������ڴ��¼��������� */
unsigned short                           g_usImmAllocFailCntThreshold = 1000;
EXPORT_SYMBOL(g_usImmAllocFailCntThreshold);

/* IMM MEM FREE ��ά�ɲ�ʵ�� */
IMM_MEM_FREE_MNTN_INFO_STRU             g_stImmMemFreeMntnEntity;
EXPORT_SYMBOL(g_stImmMemFreeMntnEntity);

/* �����ڴ��ͷſ�ָ����Ϣ */
#define IMM_MEM_FREE_SAVE_NULL_PTR_INFO(stNullPtrInfo,usFileId,usLineNum) \
            do{\
                (stNullPtrInfo).ulImmMemFreeNullPtrCnt++;\
                (stNullPtrInfo).usImmMemFreeFileId      = (usFileId);\
                (stNullPtrInfo).usImmMemFreeLineNum     = (usLineNum);\
            }while(0)

/* �����ڴ��ͷŴ����ڴ���Ϣ */
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
 �� �� ��  : IMM_MemCtrlMemAlloc
 ��������  : ��ȡIMM_MEM�ṹ�е��ڴ棬����malloc
 �������  : unsigned long                 ulSize        �ڴ��С
 �������  : ��
 �� �� ֵ  : ��Ӧ���ڴ�ؽṹ�ĵ�ַ
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��15��
    �޸�����   : �����ɺ���

*****************************************************************************/
void *IMM_MemCtrlMemAlloc(unsigned long ulSize)
{
    unsigned char                       *pucSpace;
    void                                *pAlloc;


    /* ���볤�ȷ�Χ��� */
    if ( IMM_MEM_CTRL_MEM_TOTAL_SIZE < (g_ulImmMemCtrlMemSuffix + ulSize) )
    {
        panic(" IMM_MemCtrlMemAlloc Fail, Ctrl MEM Over run! \n");
        return NULL;
    }

    pucSpace = (unsigned char *)g_aulImmMemCtrlMemSpace;

    pAlloc   = pucSpace + g_ulImmMemCtrlMemSuffix;

    /* ���ֽڶ��룬���ƿ�ʹ�ó���ƫ�� */
    g_ulImmMemCtrlMemSuffix += IMM_GET_4BYTE_ALIGN_VALUE(ulSize);

    return pAlloc;

} /* IMM_MemCtrlMemAlloc */


/*****************************************************************************
 �� �� ��  : IMM_MemPoolInit
 ��������  : ��ʼ���ڴ��
 �������  : unsigned char     ucPoolId           �ڴ�� ID
             unsigned char   **ppucDataMemAddr    ����Cache�ڴ��׵�ַ�ĵ�ַ
 �������  :
 �� �� ֵ  : IMM_SUCC,IMM_FAIL
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��11��28��
    �޸�����   : �����ɺ���

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

    /* �����Ӧ�ڴ�صĿ����ڴ�������ÿ��POOL����һ�Σ�����ָ��ÿ��clusterʹ�� */
    ppst1stImmMem = (IMM_MEM_STRU **)IMM_MemCtrlMemAlloc(sizeof(IMM_MEM_STRU *) * pstImmMemPool->usImmMemTotalCnt);

    if (NULL == ppst1stImmMem)
    {
        return IMM_FAIL;
    }

    /* �����Ӧ�ڴ�ص�IMM_MEM��ÿ��POOL����һ�Σ�����ָ��ÿ��clusterʹ�� */
    pstImmMem = (IMM_MEM_STRU *)IMM_MemCtrlMemAlloc(sizeof(IMM_MEM_STRU) * pstImmMemPool->usImmMemTotalCnt);

    if (NULL == pstImmMem)
    {
        return IMM_FAIL;
    }

    memset(ppst1stImmMem, 0, sizeof(IMM_MEM_STRU *) * pstImmMemPool->usImmMemTotalCnt);
    memset(pstImmMem, 0, sizeof(IMM_MEM_STRU) * pstImmMemPool->usImmMemTotalCnt);

    ppTempMem = ppst1stImmMem;

    /* ��¼���ڴ������IMM_MEMָ�룬����ڴ�й¶ʹ�� */
    pstImmMemPool->pstImmMemStStartAddr = pstImmMem;

    /* ����ÿ��cluster��IMM_MEM */
    for (ucClusterId = 0; ucClusterId < pstImmMemPool->ucClusterCnt; ++ucClusterId )
    {
        pstImmMemCluster    = &(pstImmMemPool->astClusterTable[ucClusterId]);

        /* ��������������Ϳ����ڴ�������Ĺ�ϵ */
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
                pstImmMem->pstMemBlk    = *ppucDataMemAddr; /* ����IMM_MEM_STRU�Ͷ�Ӧ���ݵĹ�ϵ */
            }

            /* ���������ڴ��������IMM_MEM_STRU�Ĺ�ϵ */
           *ppTempMem               = pstImmMem;

            pstImmMem++;
            ppTempMem++;

            *ppucDataMemAddr       += pstImmMemCluster->usLen;
        }

    }

    return IMM_SUCC;
} /* IMM_MemPoolInit */


/*****************************************************************************
 �� �� ��  : IMM_MemPoolCreate
 ��������  : �ڴ�ض��󴴽�����¼�ڴ����Ϣ
 �������  : IMM_MEM_POOL_CFG_INFO_STRU   *pstPoolCfg �ڴ��������Ϣ
             unsigned char       **ppucDataMemAddr  �û��ļ��������׵�ַ
 �������  : ��
 �� �� ֵ  : IMM_SUCC,IMM_FAIL
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��10��28��
    �޸�����   : �����ɺ���


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

    /* ����ӦID���ڴ���Ƿ��б��ظ����� */
    if ( IMM_TRUE == pstImmMemPool->ucUsedFlag )
    {
        panic("IMM_MemPoolCreate Fail, ulPoolId %d is already used\r\n",
            ucPoolId);
        return IMM_FAIL;
    }


    ucClusterCnt                    = pstPoolCfg->ucClusterCnt;
    pstPoolClusterCfgTable          = pstPoolCfg->pstClusterCfgInfo;

    /* ��ʼ���ڴ�ص�ͨ�ñ��� */
    pstImmMemPool->ucUsedFlag       = IMM_FALSE;
    pstImmMemPool->ucClusterCnt     = ucClusterCnt;
    pstImmMemPool->usImmMemTotalCnt = 0;

    /* ÿ��POOL�����һ��size�������ƫ�Ƶ����һ���������POOL�����size */
    pstClusterCfgInfo               = pstPoolClusterCfgTable + (ucClusterCnt - 1);
    pstImmMemPool->usMaxByteLen     = pstClusterCfgInfo->usSize;


    /* ���볤���������Գ���Ϊ�����±꣬������󳤶�Ҫ���������С1 */
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
 �� �� ��  : IMM_MemBlkInit
 ��������  : IMM_Mem�ܳ�ʼ������
 �������  : ��
 �������  : ��
 �� �� ֵ  : unsigned long
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��11��28��
    �޸�����   : �����ɺ���

*****************************************************************************/
unsigned long IMM_MemBlkInit(void)
{
    unsigned int                        ulRet1;
    unsigned int                        ulRet2;
    unsigned char                      *pucBaseAddr;
    IMM_MEM_POOL_CFG_INFO_STRU          stSharePoolCfgInfo;
    IMM_MEM_POOL_CFG_INFO_STRU          stExtPoolCfgInfo;
    unsigned long                       ulBaseAddr;

    /*��ʼ������ֻ�ᱻskbuf_init ���� */
    if ( IMM_TRUE == g_ulImmMemInitFlag )
    {
        printk( "IMM_MemBlkInit, Warning, Memory pools were already initialized!\n");

        return IMM_SUCC;
    }

    /* �����ڴ����Ϣ */
    memset(g_astImmMemPool, 0, sizeof(IMM_MEM_POOL_STRU)* IMM_MEM_POOL_ID_BUTT);
    memset(&g_stImmRegMntnFuc, 0, sizeof(IMM_MEM_REG_MNTN_FUNC_STRU));

    /* ��ʼ�� IMM MEM FREE ��ά�ɲ�ʵ�� */
    memset(&g_stImmMemFreeMntnEntity, 0, sizeof(IMM_MEM_FREE_MNTN_INFO_STRU));

    memset(&stSharePoolCfgInfo, 0, sizeof(IMM_MEM_POOL_CFG_INFO_STRU));
    memset(&stExtPoolCfgInfo, 0, sizeof(IMM_MEM_POOL_CFG_INFO_STRU));

    /* ���湲���ڴ�����ò��� */
    stSharePoolCfgInfo.pstClusterCfgInfo       = (IMM_MEM_CLUSTER_CFG_INFO_STRU *)g_astImmMemSharePoolClusterTableInfo;
    stSharePoolCfgInfo.ucClusterCnt            = IMM_MEM_SHARE_POOL_CLUSTER_CNT;
    stSharePoolCfgInfo.enPoolId                = IMM_MEM_POOL_ID_SHARE;

    /* �����ⲿ�ڴ�����ò��� */
    stExtPoolCfgInfo.pstClusterCfgInfo        = (IMM_MEM_CLUSTER_CFG_INFO_STRU *)g_astImmMemCtrlPoolClusterTableInfo;
    stExtPoolCfgInfo.ucClusterCnt             = IMM_MEM_CTRL_POOL_CLUSTER_CNT;
    stExtPoolCfgInfo.enPoolId                 = IMM_MEM_POOL_ID_EXT;

    ulBaseAddr = IMM_MemGetBaseAddr();

    /* �ڴ�����ַ������ʵ��ַת���ַ */
    pucBaseAddr = (unsigned char *)ulBaseAddr;

    /* ���������ڴ�� */
    ulRet1 = IMM_MemPoolCreate(&stSharePoolCfgInfo, &pucBaseAddr);

    /* �����ⲿ�ڴ�� */
    ulRet2 = IMM_MemPoolCreate(&stExtPoolCfgInfo, &pucBaseAddr);

    /* �����ѱ���ʼ���ı�־ */
    g_ulImmMemInitFlag = IMM_TRUE;

    if ( ( IMM_FAIL == ulRet1 ) || ( IMM_FAIL == ulRet2 ) )
    {
        panic("Create IMM failed ulRet1 = %d, ulRet2 = %d ! \n", ulRet1, ulRet2);

        return IMM_FAIL;
    }

    return IMM_SUCC;

} /* IMM_MemBlkInit */


/*****************************************************************************
 �� �� ��  : IMM_MemFreeMemCheck
 ��������  : �����ͷ��ڴ���
 �������  : IMM_MEM_STRU *pstImm
 �������  : ��
 �� �� ֵ  : unsigned long
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��20��
    �޸�����   : �����ɺ���

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
 �� �� ��  : IMM_MemGetSlice
 ��������  : �����ڴ���������е�CPU SILCE
 �������  : void
 �������  : ��
 �� �� ֵ  : unsigned long CPU SLICE
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��29��
    �޸�����   : �����ɺ���

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
 �� �� ��  : IMM_MemSaveAllocDebugInfo
 ��������  : �����ڴ���������еĿ�ά�ɲ���Ϣ
 �������  : IMM_MEM_STRU *pstImmMem ���ݿ�ָ��
             unsigned short usFileID �����ڴ��ļ�ID
             unsigned short usLineNum �����ڴ��к�
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��29��
    �޸�����   : �����ɺ���

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
 �� �� ��  : IMM_MemFreeSaveDebugInfo
 ��������  : IMM�ڴ��ͷź�����дDebug��Ϣ, ����Free�����ڲ�����
 �������  : IMM_MEM_STRU *pstImmMem ���ͷŵ��ڴ��ָ��
 �������  : ��
 �� �� ֵ  : IMM_FAIL �ڴ��״̬�Ƿ�
             IMM_SUCC �����ͷ���Ϣ�ɹ�
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��19��
    �޸�����   : �����ɺ���

*****************************************************************************/
unsigned char IMM_MemSaveFreeDebugInfo(unsigned char * pucFileId, unsigned short usLineNum, IMM_MEM_STRU *pstImmMem)
{
    unsigned long                ulMemAddr;
    IMM_BLK_MEM_DEBUG_STRU      *pstImmMemDebugInfo = &pstImmMem->stDbgInfo;


    IMM_DEBUG_TRACE_FUNC_ENTER();

    ulMemAddr = (unsigned long)pstImmMem;

    /*pstImmMem ��ַ��Χ���*/
    if (( (unsigned long)g_paulImmMemCtrlMemSpace > ulMemAddr )
        ||(((unsigned long)g_paulImmMemCtrlMemSpace + (*g_pulImmMemCtrlMemSuffix)) < ulMemAddr))
    {
        IMM_MEM_FREE_SAVE_INVALID_MEM_INFO(g_stImmMemFreeMntnEntity.astImmMemFreeInvalidMemInfo, pstImmMem,IMM_INVALID_MEM_TYPE_CTRLADDR, 0, 0, usLineNum);
        printk( "Error: IMM_MemFreeSaveDebugInfo, Mem addr %d is invalid! \n", (int)pstImmMem);

        return IMM_FAIL;
    }

    /* �ж��ڴ�ָ��״̬�Ƿ���ȷ */
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
 �� �� ��  : IMM_MemPoolGet
 ��������  : ��ȡ�ڴ�ص�ַ
 �������  : unsigned char ucPoolId  �ڴ��ID
 �������  : ��
 �� �� ֵ  : ��Ӧ���ڴ�ؽṹ�ĵ�ַ
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��10��28��
    ��    ��   : S00164817
    �޸�����   : �����ɺ���

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
 �� �� ��  : IMM_ZcGetLocalFreeMemCnt
 ��������  : �����ṩ�ӿ�, ��ȡ�ڴ��ʹ�����
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��1��
    �޸�����   : �����ɺ���

*****************************************************************************/
unsigned long IMM_ZcGetLocalFreeMemCnt(void)
{
    IMM_DEBUG_TRACE_FUNC_ENTER();
    IMM_DEBUG_TRACE_FUNC_LEAVE();

    return *g_pACpuFreeMemBlkCnt;
} /* IMM_ZcGetLocalFreeMemCnt */
EXPORT_SYMBOL(IMM_ZcGetLocalFreeMemCnt);

/*****************************************************************************
 �� �� ��  : IMM_ZcSetLocalFreeMemCnt
 ��������  : ����A����󵵵Ŀ��п��������
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��1��
    �޸�����   : �����ɺ���

*****************************************************************************/
void IMM_ZcSetLocalFreeMemCnt(unsigned long ulMemValue)
{
    IMM_DEBUG_TRACE_FUNC_ENTER();
    IMM_DEBUG_TRACE_FUNC_LEAVE();

    *g_pACpuFreeMemBlkCnt = ulMemValue;
    return;
}/* IMM_ZcSetLocalFreeMemCnt */


/*****************************************************************************
 �� �� ��  : IMM_MemAlloc_Debug
 ��������  : IMM MEM���뺯��
 �������  : unsigned long ulFileID    ������ļ���
             unsigned long ulLineNum   ������к�
             unsigned long ulPid       ��Ӧ��Pid
             unsigned long ulPoolId    ��Ӧ���ڴ��
             unsigned short usLen       �����IMM_MEM_ST�ĳ���
 �������  : ��
 �� �� ֵ  : IMM_MEM_STRU*  ����ɹ�ʱ�����IMM_MEM_ST��ַ
             NULL   ����ʧ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��10��28��
    �޸�����   : �����ɺ���

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

     /* ��ȡ�ڴ�� */
    pstImmMemPool = IMM_MEM_GET_POOL(ucPoolId);

    /* ����ڴ���Ƿ��Ѿ���ʼ�� */
    if ( IMM_TRUE != pstImmMemPool->ucUsedFlag )
    {
        printk("IMM_MemBlkAlloc_Debug,Pool %d is not used,FileId=%d,LineNum=%d!\n", ucPoolId,usFileID,usLineNum);
        return NULL;
    }

     /* ���볤���Ƿ��ڸ��ڴ��� */
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

    /* ���ڴ�صĶ������Ѱ�Һ��ʵ�IMM_MEM */
    ucMostFitLev = IMM_GET_LEN_INDEX(pstImmMemPool, ulLen);

    for(ucClusterId = ucMostFitLev;
        ucClusterId < pstImmMemPool->ucClusterCnt;
         ++ucClusterId )
    {
        pstImmMemCluster  = &(pstImmMemPool->astClusterTable[ucClusterId]);

        local_irq_save(ulSaveFlags);

        if (0 != pstImmMemCluster->usFreeCnt)
        {
            /* ��ȡһ��û��ʹ�õ�IMM_MEM_ST��� */
            pMem    = pstImmMemCluster->apstFreeStack[--pstImmMemCluster->usFreeCnt];
            ++pstImmMemPool->usImmMemUsedCnt;
            ulMaxClusterFreeCnt = pstImmMemPool->astClusterTable[pstImmMemPool->ucClusterCnt - 1].usFreeCnt;

            local_irq_restore(ulSaveFlags);

            /* ������ģ���ϱ��ڴ������¼� */
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

    /* ��SDT�ϱ��ڴ�����ʧ���¼� */
    if ( NULL != g_pstImmRegMntnFuc->pImmMemEventRptFunc)
    {
        g_pstImmRegMntnFuc->pImmMemEventRptFunc(ucPoolId, IMM_MEM_TRIG_TYPE_ALLOC_FAIL);
    }


    return NULL;
} /* IMM_MemAlloc_Debug */
EXPORT_SYMBOL(IMM_MemAlloc_Debug);


/*****************************************************************************
 �� �� ��  : IMM_MemFree_Debug
 ��������  : �ͷ�IMM MEM BLK�ڴ�
 �������  : IMM_MEM_STRU *pstImmMem      �ͷŵ����ݽṹ
 �������  : ��
 �� �� ֵ  : IMM_MEM����һ��
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��11��28��
    �޸�����   : �����ɺ���

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

    /* ���ͷ��ڴ�Ϸ��Լ�� */
    if ( IMM_SUCC != IMM_MemFreeMemCheck(pucFileId, usLineNum, ppstMem) )
    {
        printk( "Error: IMM_MemFree_Debug input ppstMem is invalid! FileId=%s,LineNum=%d \n", pucFileId, usLineNum);
        return;
    }

    pstImmMem       = *ppstMem;
    pstImmMemPool   = IMM_MEM_GET_POOL(pstImmMem->enPoolId);

    /* ����Ӧ���ڴ��Ƿ�ʹ�� */
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

    /* �ⲿ�ڴ�أ���Ҫ�ͷ��ⲿ�ڴ�� */
    if ( IMM_MEM_POOL_ID_EXT == pstImmMem->enPoolId )
    {
        pucExtMem = pstImmMem->pstMemBlk;
        /* ����ⲿ�ڴ��ָ�� */
        pstImmMem->pstMemBlk = NULL;
    }

    /* ��ȡ�ڴ�����ڵĵ�λ */
    pstImmMemCluster = &(pstImmMemPool->astClusterTable[pstImmMem->ucClusterId]);

    local_irq_save(ulSaveFlags);

    if (pstImmMemCluster->usFreeCnt >= pstImmMemCluster->usTotalCnt)
    {
        local_irq_restore(ulSaveFlags);
        printk("IMM_MemFree_Debug, free cnt error:FreeCnt %d, TotalCnt %d ! \n",
                pstImmMemCluster->usFreeCnt, pstImmMemCluster->usTotalCnt);

        return ;
    }

    /* ���ڴ��ҽӻ��ڴ�� */
    pstImmMemCluster->apstFreeStack[pstImmMemCluster->usFreeCnt] = pstImmMem;
    ++pstImmMemCluster->usFreeCnt;
    --pstImmMemPool->usImmMemUsedCnt;
    /* �����ڴ�����λʣ���ڴ���� */
    ulMaxClusterFreeCnt = pstImmMemPool->astClusterTable[pstImmMemPool->ucClusterCnt - 1].usFreeCnt;

    local_irq_restore(ulSaveFlags);

    if (( NULL != pstImmMemPool->pMemExtFreeFunc )&&( NULL != pucExtMem ))
    {
        /* �ͷ��ⲿ�ڴ� */
        pstImmMemPool->pMemExtFreeFunc(pucExtMem);
    }

    /* ������ģ���ϱ��ڴ��ͷ��¼� */
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
 �� �� ��  :  IMM_MemRegExtFreeCallBack
 ��������  :  ע��IMM MEM�ⲿ�ڴ���ͷź���
 �������  :  enPoolId   -- �ڴ��ID
              pMemExtFreeFunc   -- ע����ڴ��ͷŻص�����
 �������  :  ��
 �� �� ֵ  :  IMM_FAIL ע��ʧ��
              IMM_SUCC ע��ɹ�
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��12��
    �޸�����   : �����ɺ���

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
 �� �� ��  : IMM_MemRegZcFuncCallBack
 ��������  : �ṩIMM��ά�ɲ����ĺ���ע��
 �������  : ��
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��15��
    �޸�����   : �����ɺ���

*****************************************************************************/
void IMM_MemRegMntnFuncCallBack
(
    IMM_OM_GET_SLICE_CALLBACK               pImmOmGetSliceFunc,
    IMM_MEM_USED_INFO_EVENT_RPT_CALLBACK    pImmMemEventRptFunc
)
{
    /* ������飬pImmOmGetSliceFunc �����ڷ�DEBUGģʽ��Ϊ�� */
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
 �� �� ��  : IMM_MemRegEventCallBack
 ��������  : �ṩIMMЭ��ջ���ִ���ĺ���ע��
 �������  : ��
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��15��
    �޸�����   : �����ɺ���

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

    /* ��ȡ���λ�ڴ��ʣ����� */
    ulMaxClusterFreeCnt = pstImmMemPool->astClusterTable[pstImmMemPool->ucClusterCnt - 1].usFreeCnt;
    IMM_ZcSetLocalFreeMemCnt(ulMaxClusterFreeCnt);

    return IMM_SUCC;
} /* IMM_MemRegEventCallBack */
EXPORT_SYMBOL(IMM_MemRegEventCallBack);


/*****************************************************************************
 �� �� ��  : IMM_MemFreeShowMntnInfo
 ��������  : ��ӡ IMM MEM �ڴ��ά�ɲ���Ϣ
 �������  : void
 �������  : ��
 �� �� ֵ  : void
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2012��3��7��
    �޸�����   : �����ɺ���

*****************************************************************************/
void IMM_MemFreeShowMntnInfo( void )
{
    IMM_MEM_FREE_INVALIED_MEM_INFO_STRU    *pstInvalidMemInfo   = &g_stImmMemFreeMntnEntity.astImmMemFreeInvalidMemInfo;
    IMM_MEM_FREE_NULL_PTR_INFO_STRU        *pstNullPtrInfo      = &g_stImmMemFreeMntnEntity.astImmMemFreeNullPtrInfo;
    IMM_MEM_FREE_NULL_PTR_INFO_STRU        *pstNullPPtrInfo     = &g_stImmMemFreeMntnEntity.astImmMemFreeNullPPtrInfo;


    printk( "IMM MEM FREE ��ά�ɲ���Ϣ :\n");
    printk( "==========================================\n");
    printk( "IMM MEM FREE �Ƿ��ڴ���Ϣ :\n");
    printk( "IMM MEM FREE �յ��ķǷ��ڴ����       : %u \n", (unsigned int)pstInvalidMemInfo->ulInvalidImmMemCnt);
    printk( "IMM MEM FREE �Ƿ�IMM MEM �ڴ��ַ     : 0x%u \n", (unsigned int)pstInvalidMemInfo->ulInvalidImmMemAddr);
    printk( "IMM MEM FREE �Ƿ�IMM MEM �ڴ��ļ�ID   : %d \n", pstInvalidMemInfo->usImmMemFreeFileId);
    printk( "IMM MEM FREE �Ƿ�IMM MEM �ڴ��к���Ϣ : %d \n", pstInvalidMemInfo->usImmMemFreeLineNum);
    printk( "IMM MEM FREE �Ƿ�IMM MEM �ڴ�����     : %d \n", pstInvalidMemInfo->ucImmMemInvalidType);
    printk( "IMM MEM FREE �Ƿ�IMM MEM �ڴ�ֵ       : %d \n", pstInvalidMemInfo->usImmMemInvalidValue);
    printk( "IMM MEM �ڴ����ͷ��ʼ��ַ            : 0x%u ~ 0x%u \n\n", (unsigned int)g_paulImmMemCtrlMemSpace,((unsigned int)((unsigned int)g_paulImmMemCtrlMemSpace + (*g_pulImmMemCtrlMemSuffix))) );


    printk( "==========================================\n");
    printk( "IMM MEM FREE ��ָ����Ϣ :\n");
    printk( "IMM MEM FREE һ��ָ��Ϊ�յĴ���    : %u \n", (unsigned int)pstNullPtrInfo->ulImmMemFreeNullPtrCnt);
    printk( "IMM MEM FREE ����ָ���ļ�ID        : %d \n", pstNullPtrInfo->usImmMemFreeFileId);
    printk( "IMM MEM FREE ����ָ���ڴ��к���Ϣ  : %d \n", pstNullPtrInfo->usImmMemFreeLineNum);

    printk( "IMM MEM FREE ����ָ��Ϊ�յĴ���    : %u \n", (unsigned int)pstNullPPtrInfo->ulImmMemFreeNullPtrCnt);
    printk( "IMM MEM FREE ����ָ���ڴ��ļ�ID    : %d \n", pstNullPPtrInfo->usImmMemFreeFileId);
    printk( "IMM MEM FREE ����ָ���ڴ��к���Ϣ  : %d \n", pstNullPPtrInfo->usImmMemFreeLineNum);

    printk( "IMM MEM Pool Status: IMM_MEM_POOL_ID_DL_SHARE %d\n", g_astImmMemPool[IMM_MEM_POOL_ID_SHARE].ucUsedFlag);
    printk( "                     IMM_MEM_POOL_ID_EXT      %d\n", g_astImmMemPool[IMM_MEM_POOL_ID_EXT].ucUsedFlag);

    printk( "IMM_MEM_POOL_ID_EXT �ͷź���      : 0x%u\n", (unsigned int)g_astImmMemPool[IMM_MEM_POOL_ID_EXT].pMemExtFreeFunc);
    printk( "IMM Ext MEM FREE ʧ�ܴ���         : %u \n", (unsigned int)g_stImmMemFreeMntnEntity.ulImmMemExtFreeFailCnt);

}
EXPORT_SYMBOL(IMM_MemFreeShowMntnInfo);





#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

