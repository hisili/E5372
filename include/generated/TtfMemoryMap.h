/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : TtfMemoryMap.h
  �� �� ��   : ����
  ��������   : 2011��12��23��
  ����޸�   :
  ��������   : MemoryMap.h ��ͷ�ļ�
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2011��12��23��
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef __TTFMEMORYMAP_H__
#define __TTFMEMORYMAP_H__


/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "FeatureConfigTTF.h"
#include "MemoryMap.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif




/*****************************************************************************
  2 �궨��
*****************************************************************************/

/* 32 �ֽڶ��� */
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

    /* �����TTF�����ڴ��С�ͻ���ַ */

/* ���ڵ����������ڴ� */

#define TTF_UL_CIPHER_PARA_NODE_ADDR(BaseAddr) BaseAddr

/*1096(WTTF_MAX_PDU_NUM_PER_UL_LOCH) * 32Byte(RLC_PDU_SEG_CIPHER_PARA_STRU) �� 35KByte*/
#define TTF_UL_CIPHER_PARA_NODE_LEN     (35*1024)


#define TTF_UL_CIPHER_SEG_DATA_ADDR(BaseAddr)\
           (TTF_UL_CIPHER_PARA_NODE_ADDR(BaseAddr) + TTF_UL_CIPHER_PARA_NODE_LEN)

#define TTF_UL_CIPHER_SEG_DATA_LEN      (51*1024)

#define TTF_UL_CIPHER_KEY_ADDR(BaseAddr)\
           (TTF_UL_CIPHER_SEG_DATA_ADDR(BaseAddr) + TTF_UL_CIPHER_SEG_DATA_LEN)

/* ���м�����Կʹ���ڴ泤�Ⱥͻ���ַ
   3(RLC_CPH_CFG_NUM) * 16Byte(WUE_CIPHER_KEY_LEN) = 48Byte ȡ��Ϊ1K */
#define TTF_UL_CIPHER_KEY_LEN           (1*1024)

#define TTF_DL_CIPHER_PARA_NODE_ADDR(BaseAddr)\
           (TTF_UL_CIPHER_KEY_ADDR(BaseAddr) + TTF_UL_CIPHER_KEY_LEN)

/* ���н��ܲ����ڵ�ʹ���ڴ泤�Ⱥͻ���ַ
    350(WTTF_MAX_PDU_NUM_PER_DL_LOCH) * 32Byte(RLC_PDU_SEG_CIPHER_PARA_STRU) �� 11KByte */
#define TTF_DL_CIPHER_PARA_NODE_LEN     (11*1024)

/* Modified Mem Reduce End */
#define TTF_DL_CIPHER_SEG_DATA_ADDR(BaseAddr)\
           (TTF_DL_CIPHER_PARA_NODE_ADDR(BaseAddr) + TTF_DL_CIPHER_PARA_NODE_LEN)

/* ����MAC-ehs SDU�ֶ�ʹ���ڴ泤�Ⱥͻ���ַ */
/*13200(WTTF_MAC_EHS_SDU_SEG_DATA_NUM) * 12Byte(SEG_DATA_STRU��С) �� 155KByte */
#define TTF_DL_CIPHER_SEG_DATA_LEN          (155*1024)

#define TTF_DL_CIPHER_KEY_ADDR(BaseAddr)\
           (TTF_DL_CIPHER_SEG_DATA_ADDR(BaseAddr) + TTF_DL_CIPHER_SEG_DATA_LEN)

/* ���н�����Կʹ���ڴ泤�Ⱥͻ���ַ
   3(RLC_CPH_CFG_NUM) * 16Byte(WUE_CIPHER_KEY_LEN) = 48Byte ȡ��Ϊ1K */
#define TTF_DL_CIPHER_KEY_LEN               (1*1024)


#define TTF_UL_CIPHER_PDU_NODE_TOTAL_LEN    (TTF_UL_CIPHER_PARA_NODE_LEN + TTF_UL_CIPHER_SEG_DATA_LEN)
#define TTF_DL_CIPHER_PDU_NODE_TOTAL_LEN    (TTF_DL_CIPHER_PARA_NODE_LEN + TTF_DL_CIPHER_SEG_DATA_LEN)
/* ��MASTER�豸ʹ�õ��ڴ��ܳ��� */
#define TTF_CIPHER_MASTER_LINK_LEN          (TTF_UL_CIPHER_PDU_NODE_TOTAL_LEN + TTF_UL_CIPHER_KEY_LEN + \
                                             TTF_DL_CIPHER_PDU_NODE_TOTAL_LEN + TTF_DL_CIPHER_KEY_LEN)

/* �����ڴ�ؿ��� */
#if ( FEATURE_ON == FEATURE_LTE )
/* LTE�����ڴ��ܳ���:   WithWifi      12,854,528 �ֽ�
                        WithOutWifi   12,620,608 �ֽ� */
/* ==================================================== */
/* IMM MEM �����ڴ�ؿ��� */
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

�ڴ���ܳ���: WithWifi      4,826,240�ֽ�
              WithOutWifi   4,707,520�ֽ�
*******************************************************************************/
/* IMM MEM �����ڴ����λ�ڴ���С */
#define IMM_MEM_SHARE_POOL_SIZE_CLUSTER_0           (640)
#if((defined(VERSION_V3R2)) && (FEATURE_WIFI == FEATURE_ON ))
#define IMM_MEM_SHARE_POOL_SIZE_CLUSTER_1           (1728)
#else
#define IMM_MEM_SHARE_POOL_SIZE_CLUSTER_1           (1664)
#endif

/* IMM MEM �����ڴ����λ�ڴ���� */
#define IMM_MEM_SHARE_POOL_CNT_CLUSTER_0            (1395)
#define IMM_MEM_SHARE_POOL_CNT_CLUSTER_1            (1855)

/* IMM MEM �����ڴ��ܿ��� */
#define IMM_MEM_SHARE_POOL_MEM_CNT                  (IMM_MEM_SHARE_POOL_CNT_CLUSTER_0 + IMM_MEM_SHARE_POOL_CNT_CLUSTER_1)

/* IMM MEM �����ڴ��ܴ�С */
#define IMM_MEM_SHARE_POOL_TOTAL_LEN                ((IMM_MEM_SHARE_POOL_SIZE_CLUSTER_0 * IMM_MEM_SHARE_POOL_CNT_CLUSTER_0) + \
                                                     (IMM_MEM_SHARE_POOL_SIZE_CLUSTER_1 * IMM_MEM_SHARE_POOL_CNT_CLUSTER_1))

/* TTF MEM �����ڴ�ؿ��� */
#define TTF_MEM_CTRL_MEM_CNT                        (IMM_MEM_SHARE_POOL_MEM_CNT)

#else

/* LTE�����ڴ��ܳ���:   WithWifi      9,789,888 �ֽ�
                        WithOutWifi   9,656,448 �ֽ� */
/* IMM MEM �����ڴ�ؿ��� */
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


�ڴ���ܳ���: WithWifi      4,826,240�ֽ�
              WithOutWifi   4,707,520�ֽ�
*******************************************************************************/
/* IMM MEM �����ڴ����λ�ڴ���С */
#define IMM_MEM_SHARE_POOL_SIZE_CLUSTER_0           (640)
#if(FEATURE_WIFI == FEATURE_ON )
#define IMM_MEM_SHARE_POOL_SIZE_CLUSTER_1           (1728)
#else
#define IMM_MEM_SHARE_POOL_SIZE_CLUSTER_1           (1664)
#endif

/* IMM MEM �����ڴ����λ�ڴ���� */
#define IMM_MEM_SHARE_POOL_CNT_CLUSTER_0            (1395)
#define IMM_MEM_SHARE_POOL_CNT_CLUSTER_1            (1855)

/* IMM MEM �����ڴ��ܿ��� */
#define IMM_MEM_SHARE_POOL_MEM_CNT                  (IMM_MEM_SHARE_POOL_CNT_CLUSTER_0 + IMM_MEM_SHARE_POOL_CNT_CLUSTER_1)

/* IMM MEM �����ڴ��ܴ�С */
#define IMM_MEM_SHARE_POOL_TOTAL_LEN                ((IMM_MEM_SHARE_POOL_SIZE_CLUSTER_0 * IMM_MEM_SHARE_POOL_CNT_CLUSTER_0) + \
                                                     (IMM_MEM_SHARE_POOL_SIZE_CLUSTER_1 * IMM_MEM_SHARE_POOL_CNT_CLUSTER_1))

/* TTF MEM �����ڴ�ؿ��� */
#define TTF_MEM_CTRL_MEM_CNT                        (IMM_MEM_SHARE_POOL_MEM_CNT)

#endif

/*
    TTFMem.c�е�g_ulTtfActiveMemTotalLen��¼�˸�ƽ̨��ECS_TTF_SIZE��ʵ��ʹ�õ��ܴ�С
    ���޸ļ��ܲ����ڴ���ڴ�ش�Сʱ����ʹ��COMM_CODE_GU/ConfigĿ¼��Ӧƽ̨��FeatureXXX.h�ļ���
    ����COMM_CODE_GU\Balong_GU_Inc\win32Ŀ¼�µ��ļ���Ȼ������UT���̣����g_ulTtfActiveMemTotalLen
    ȷ��g_ulTtfActiveMemTotalLen������MemoryMap.h�е����ֵ�ECS_TTF_SIZE
*/

/* �����ڴ������ܳ���Ŀǰֻ�мӽ�����ز����ڴ� */
#define TTF_PARA_MEM_TOTAL_LEN                  (TTF_CIPHER_MASTER_LINK_LEN)

/* A CPU ����cache�ڴ����ַ */
#define TTF_ACORE_POOL_MEM_START_ADDR(BaseAddr) (BaseAddr + TTF_PARA_MEM_TOTAL_LEN)

/* A CPU IMM�ڴ�ػ���ַ */
#define IMM_MEM_POOL_BASE_ADDR(BaseAddr)        (TTF_GET_32BYTE_ALIGN_VALUE(TTF_ACORE_POOL_MEM_START_ADDR(BaseAddr)))

/* A CPU �����ڴ����ʼ��ַ */
#define IMM_SHARE_MEM_POOL_ADDR(BaseAddr)       (IMM_MEM_POOL_BASE_ADDR(BaseAddr))

/* A CPU �����ڴ���ܴ�С */
#define TTF_ACORE_POOL_MEM_TOTAL_LEN            (IMM_MEM_SHARE_POOL_TOTAL_LEN)

/* C CPU ����cache�ڴ����ַ */
#define TTF_CCORE_POOL_MEM_START_ADDR(BaseAddr) (TTF_ACORE_POOL_MEM_START_ADDR(BaseAddr) + TTF_ACORE_POOL_MEM_TOTAL_LEN)

/* C CPU TTF MEM�ڴ�ػ���ַ */
#define TTF_MEM_POOL_BASE_ADDR(BaseAddr)        (TTF_GET_32BYTE_ALIGN_VALUE(TTF_CCORE_POOL_MEM_START_ADDR(BaseAddr)))

/* IMM MEM �����ڴ��������ͷ�ܿ���*/
#define IMM_MEM_CTRL_MEM_TOTAL_CNT              (IMM_MEM_SHARE_POOL_MEM_CNT + IMM_MEM_CTRL_MEM_CNT)




/* �û����ں��ڴ��ͷŹ�������ڴ���� */
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

