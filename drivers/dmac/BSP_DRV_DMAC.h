/******************************************************************
* Copyright (C), 2005-2007, HISILICON Tech. Co., Ltd.             *
*                                                                 *
* File name: dmaDrv.h                                          *
*                                                                 *
* Description:                                                    *
*      DMA controller driver header file                          *
*                                                                 *
* Author:                                                         *
*                                                                 *
* Version: v0.1                                                   *
*                                                                 *
* Function List:                                                  *
*                                                                 *
* Date:                                                           *
*                                                                 *
* History:                                                        *
*                                                                 *
1.date:2008-07-20
 question number:AT2D04493
 modify by: k60638
 modify reasion:modify pclint warnings
*******************************************************************/


#ifndef    _BSP_DMA_DRV_H_
#define    _BSP_DMA_DRV_H_

#ifdef __cplusplus
extern "C" { /* allow C++ to use these headers */
#endif /* __cplusplus */

#ifdef __VXWORKS__
#include "Drvinterface.h"
#define DMAC_PRINT          printf
#else
#include "BSP.h"
#define DMAC_PRINT          printk
#endif

#define DMAC_SUCCESS        BSP_OK
/**************************************************************************
  �궨��
**************************************************************************/
#define DMA_VERSION                0        /* version number increase from 0 */
#define DMA_MEM_ALIGN              32   /* For aligining to 4 words. */
#define DMA_MAX_PERIPHERALS        32

#define DMA_MAX_CHANNELS         4
#define DMA_CHANNELS_M           3 

#ifdef PRODUCT_CFG_CORE_TYPE_MODEM
#ifdef DMAC_TEST_DSP
#define  DMA_SPECIAL_MODE_NUM   1
#else
#define  DMA_SPECIAL_MODE_NUM   0
#endif
#define DMA_MIN_Index         2
#define DMA_MAX_Index         DMA_CHANNELS_M
#else
#define  DMA_SPECIAL_MODE_NUM   0
#define DMA_MIN_Index         DMA_CHANNELS_M
#define DMA_MAX_Index         DMA_MAX_CHANNELS
#endif

#define DMA_INT_CLEAR            0xfF
#define DMA_INT_MASK_WE          0xff00
      

#define DMA_INT_UNMASK           0x0f
#define DMA_ERRINT_UNMASK        0x0f
#define DMA_INT_MASK             0
#define DMA_CHN_EN               0x0f

#define DMA_MAX_SINGLE_BLOCK_TRANS_LENGTH  0xfff
#define DMA_SINGLE_BLOCK_TRANS_LENGTH  4080

#define DMA_NODE_NUM_NORMAL       258

#define DMA_CHANNEL_ENABLE         1
#define DMA_CHANNEL_DISABLE        0

/* ����ģ���Ƿ��Ѿ���ʼ�� */
#define DMA_NOTINITIALIZE  0
#define DMA_INITIALIZED    1

#define BSP_INT_VEC_DMA (45)

/*define the address for the DMA control and status register*/
#define DMA_CxDISABLE                      0x00
#define DMA_CxENABLE                       0x01


/* ��������Ĵ�����ƫ�� */
#define DMA_SAR(i)   (0x0 + i * 0x58)        /* ͨ��Դ��ַ�Ĵ��� */
#define DMA_DAR(i)    (0x8 + i * 0x58)        /* ͨ��Ŀ�ĵ�ַ�Ĵ��� */
#define DMA_LLP(i)   (0x10 + i * 0x58)        /* ͨ������ָ��Ĵ��� */
#define DMA_CTL(i)   (0x18 + i * 0x58)        /* ͨ�����ƼĴ��� */
#define DMA_SSTAT(i)   (0x20 + i * 0x58)        /* ͨ��Դ״̬�Ĵ��� */
#define DMA_DSTAT(i)   (0x28 + i * 0x58)        /* ͨ��Ŀ��״̬�Ĵ��� */
#define DMA_SSTATAR(i)   (0x30 + i * 0x58)       /* ͨ��Դ״̬��ַ�Ĵ��� */
#define DMA_DSTATAR(i)   (0x38 + i * 0x58)       /* ͨ��Ŀ��״̬��ַ�Ĵ��� */
#define DMA_CFG(i)    (0x40 + i * 0x58)        /* ͨ�����üĴ��� */
#define DMA_SGR(i)    (0x48 + i * 0x58)        /* ͨ��Դ�ۼ��Ĵ��� */
#define DMA_DSR(i)    (0x50 + i * 0x58)        /* ͨ��Ŀ�ķ�ɢ�Ĵ��� */
#define DMA_RAWTFR    0x2c0        /* intTfr�ж�ԭʼ״̬�Ĵ��� */
#define DMA_RAWBLOCK    0x2c8        /* intBlock�ж�ԭʼ״̬�Ĵ��� */
#define DMA_RAWSRCTRAN   0x2d0        /* intSrcTran�ж�ԭʼ״̬�Ĵ��� */
#define DMA_RAWDSTTRAN    0x2d8        /* intDstTran�ж�ԭʼ״̬�Ĵ��� */
#define DMA_RAWERR    0x2e0        /* intErr�ж�ԭʼ״̬�Ĵ��� */
#define DMA_STATUSTFR    0x2e8        /* intTfr�ж�״̬�Ĵ��� */
#define DMA_STATUSBLOCK    0x2f0        /* intBlock�ж�״̬�Ĵ��� */
#define DMA_STATUSSCRTRAN   0x2f8        /* intSrcTran�ж�״̬�Ĵ��� */
#define DMA_STATUSDSTTRAN    0x300        /* intDstTran�ж�״̬�Ĵ��� */
#define DMA_STATUSERR    0x308        /* intErr�ж�״̬�Ĵ��� */
#define DMA_MASKTFR    0x310        /* intTfr�ж����μĴ��� */
#define DMA_MASKBLOCK    0x318        /* intBlock�ж����μĴ��� */
#define DMA_MASKSRCTRAN    0x320        /* intSrcTran�ж����μĴ��� */
#define DMA_MASKDSTTRAN    0x328        /* intDstTran�ж����μĴ��� */
#define DMA_MASKERR    0x330        /* intErr�ж����μĴ��� */
#define DMA_CLEARTFR    0x338        /* intTfr�ж�����Ĵ��� */
#define DMA_CLEARBLOCK    0x340        /* intBlock�ж�����Ĵ��� */
#define DMA_CLEARSRCTRAN    0x348        /* intSrcTran�ж�����Ĵ��� */
#define DMA_CLEARDSTTRAN   0x350        /* intDstTran�ж�����Ĵ��� */
#define DMA_CLEARERR    0x358        /* intErr�ж�����Ĵ��� */
#define DMA_STATUSINT    0x360        /* ����ж�״̬�Ĵ��� */
#define DMA_REQSRCREG    0x368        /* Դ�����������Ĵ��� */
#define DMA_REQDSTREG    0x370        /* Ŀ�������������Ĵ��� */
#define DMA_SGLREQSRCREG    0x378        /* Դ������δ�������Ĵ��� */
#define DMA_SGLREGDSTREG    0x380        /* Ŀ��������δ�������Ĵ��� */
#define DMA_LSTSRCREG    0x388        /* ���һ��Դ�����������Ĵ��� */
#define DMA_LSTDSTREG    0x390        /* ���һ��Ŀ�������������Ĵ��� */
#define DMA_DMACFGREG    0x398        /* DMA���üĴ��� */
#define DMA_CHENREG    0x3a0        /* DMAͨ��ʹ�ܼĴ��� */
#define DMA_DMAIDREG    0x3a8        /* DMA  ID �Ĵ��� */
#define DMA_DMATESTREG    0x3b0        /* DMA  ���ԼĴ��� */
#define DMA_DMAVERREG    0x3f8        /* DMA  Ԫ���汾�Ĵ��� */

#define DMA_INT_ADDRESS_SPAN      0x8
#define DMA_RAW_INT_BASE     DMA_RAWTFR
#define DMA_STATUS_INT_BASE    DMA_STATUSTFR
#define DMA_MASK_INT_BASE     DMA_MASKTFR
#define DMA_CLEAR_INT_BASE     DMA_CLEARTFR

#define DMA_ENABLE   1
#define DMA_DISABLE   0

/* �����ƽֵ */
#define DMA_HIGH_LEVEL   1
#define DMA_LOW_LEVEL   0

/* ����Ĭ�ϵ�CTL��CFG��ֵ */
#define DMA_LLI_ENABLE   0x18000000
#define DMA_CTL_H_DEFAULT  0x0
#define DMA_CTL_L_DEFAULT  0x1

#define DMA_CFG_H_DEFAULT  0x0
#define DMA_CFG_L_DEFAULT  0x0

/* ���鴫��ʱһ�ξͿ��Դ���*/
#define DMA_SINGLE_TRANS 0
/* ���鴫��ʱ��������С���ڵ����������󳤶ȣ�����Ҫ�ֶ�δ���*/
#define DMA_MUTI_TRANS   1
#define DMA_BURST_LEN16 0x10

/**************************************************************************
  ȫ�ֱ�������
**************************************************************************/


/**************************************************************************
  ö�ٶ���
**************************************************************************/
/* �ж����� */
typedef enum tagDMA_INT_TYPE_NUM_E
{
    INT_TFR = 0,
    INT_BLOCK,
    INT_SRC_TRAN,
    INT_DST_TRAN,
    INT_ERR,
    INT_BUTTOM_BIT
}DMA_INT_TYPE_NUM_E;

 /* DESCRIPTION
 *  This data type is used for selecting the address increment
 *  type for the source and/or destination on a DMA channel when using
 *  the specified driver API functions.
 * NOTES
 *  This data type relates directly to the following DMA Controller
 *  register(s) / bit-field(s): (x = channel number)
 *    - CTLx.SINC, CTLx.DINC
 * SEE ALSO
 *  dw_dmac_setAddressInc(), dw_dmac_getAddressInc()
 * SOURCE
 */
typedef enum tagDMA_ADDR_INCREMENT_E
{
    DMA_ADDR_INCREMENT = 0x0, /* ��ַ����*/
    DMA_ADDR_DECREMENT = 0x1, /* ��ַ�ݼ�*/
    DMA_ADDR_NOCHANGE  = 0x2  /* ��ַ�̶�����*/
}DMA_ADDR_INCREMENT_E;

/* Э��ջƹ�ҵ�ַʹ�����ö��*/
typedef enum tagDMA_ADDR_USE_STATUS_E
{
    DMA_NO_ADDR_INUSE = 0x0,    /* û�е�ַ����*/
    DMA_FIRST_ADDR_INUSE = 0x1, /* ��һ����ַ����*/
    DMA_SECOND_ADDR_INUSE  = 0x2/* �ڶ�����ַ����*/
}DMA_ADDR_USE_STATUS_E;

/* DMA masterѡ��*/
typedef enum tagDMA_MASTER_E
{
    DMA_MASTER1 = 0x0,
    DMA_MASTER2 = 0x1,
    DMA_MASTER3 = 0x2,
    DMA_MASTER4 = 0x3
}DMA_MASTER_E;

/* DMA ���ַ�ʽѡ��*/
typedef enum tagDMA_HS_SEL_E
{
    DMA_HW_HANDSHAKE = 0x0,
    DMA_SW_HANDSHAKE = 0x1
}DMA_HS_SEL_E;

/**************************************************************************
  STRUCT����
**************************************************************************/
typedef BSP_S32 (*DMAC_FUNCPTR)(BSP_S32);

/* ͨ����Ϣ�ṹ��*/
typedef struct tagDMA_CHN_INFO_S
{
    DMAC_FUNCPTR pfuncDmaTcIsr; /* ����ж�ע�ắ��*/
    BSP_S32 s32DmaTcPara; /* ����ж�ע�ắ������*/
    DMAC_FUNCPTR pfuncDmaErrIsr; /* �����ж�ע�ắ��*/
    BSP_S32 s32DmaErrPara; /* �����ж�ע�ắ������*/
    BSP_U32 u32ChnUsrStatus; /* �Լ�ά����channel�߼�״̬ */
    BSP_U32 u32ChnAttribute;/* ͨ������*/
    BSP_U32 *pBlockToLLIHead; /* ���󵥿��ֳ�����ʱʹ�õ�ͷ�ڵ�*/
} DMA_CHN_INFO_S;

/* ͨ����Ϣ�ṹ��*/
typedef struct tagDMA_BUILD_LLI_FOR_BLOCK_S
{
    BSP_U32 u32NodeNum;
    BSP_U32 u32OneTimeTransLength;
    BSP_U32 u32CtlSINC; /* Դ��ַ����ģʽ���������ݼ����ǲ���*/
    BSP_U32 u32CtlDINC; /* Ŀ�ĵ�ַ����ģʽ���������ݼ����ǲ���*/
    BSP_U32 u32CtlSMS; /* Դ�豸Master ѡ��λ*/
    BSP_U32 u32CtlDMS; /* Ŀ���豸Master ѡ��λ*/
    BSP_U32 u32SrcBurstLength; /*Դburst���� */
    BSP_U32 u32DstBurstLength; /*Ŀ��burst����*/
} DMA_BUILD_LLI_FOR_BLOCK_S;

/**************************************************************************
  UNION����
**************************************************************************/


/**************************************************************************
  OTHERS����
**************************************************************************/


/**************************************************************************
  ��������
**************************************************************************/

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
BSP_VOID DMAC_GetRegistValue(BSP_VOID);

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
BSP_VOID DMAC_GetChannelInfo(BSP_U32 u32Chan);

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif /* __cplusplus */

#endif    /* End of _BSP_DMA_DRV_H_ */

