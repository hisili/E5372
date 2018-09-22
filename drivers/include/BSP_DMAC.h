/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_DMAC.h
*
*   ��    �� :  z67193
*
*   ��    �� :  BSP���Ͷ���ͷ�ļ�
*
*   �޸ļ�¼ :  2012��04��14��  v1.00  z67193  ����
*************************************************************************/

#ifndef	__BSP_DMAC_H__
#define __BSP_DMAC_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/**************************************************************************
  ȫ�ֱ�������
**************************************************************************/

/**************************************************************************
  ö�ٶ���
**************************************************************************/
/*DMA transport type*/
typedef enum tagDMA_TRANS_TYPE_E
{
    MEM2MEM_DMA = 0,    /* �ڴ浽�ڴ棬DMA����*/
    MEM2PRF_DMA,        /* �ڴ浽���裬DMA����*/
    PRF2MEM_DMA,        /* ���赽�ڴ棬DMA����*/
    PRF2PRF_DMA,        /* ���赽���裬DMA����*/
    PRF2MEM_PRF,        /* ���赽�ڴ棬��������*/
    PRF2PRF_SRCPRF = 5, /* ���赽���裬Դ��������*/
    MEM2PRF_PRF,        /* �ڴ浽���裬��������*/
    PRF2PRF_DSTPRF,     /* ���赽���裬Ŀ����������*/
    CTL_TT_FC_BUTTOM    /* ���ֵ���ж���*/
} DMA_TRANS_TYPE_E;

/**************************************************************************
  �궨�� 
**************************************************************************/

/*�����붨��*/
#define EDMA_SUCCESS                    BSP_OK
#define DMAC_SUCCESS                    BSP_OK
#define DMAC_FAIL                       BSP_ERROR


#define BSP_ERR_DMA_MODULE_NOT_INITED   BSP_DEF_ERR(BSP_MODU_DMAC, BSP_ERR_MODULE_NOT_INITED)
#define BSP_ERR_DMA_NULL_PTR            BSP_DEF_ERR(BSP_MODU_DMAC, BSP_ERR_NULL_PTR)
#define BSP_ERR_DMA_INVALID_PARA        BSP_DEF_ERR(BSP_MODU_DMAC, BSP_ERR_INVALID_PARA)

#define BSP_ERR_DMA_CHANNEL_BUSY        BSP_DEF_ERR(BSP_MODU_DMAC, 1)
#define BSP_ERR_DMA_ALL_CHANNEL_BUSY    BSP_DEF_ERR(BSP_MODU_DMAC, 2)
#define BSP_ERR_DMA_STOP_FAIL           BSP_DEF_ERR(BSP_MODU_DMAC, 3)

/**************************************************************************
  ö�ٶ���
**************************************************************************/

/* ͨ���߼�״̬*/
typedef enum tagDMA_CHN_USR_STATUS_E
{
    DMA_CHN_IDLE = 0, /* ͨ������*/
    DMA_CHN_ALLOCED = 1 /* ͨ���Ѿ�������*/
}DMA_CHN_USR_STATUS_E;

/* ͨ������*/
typedef enum tagDMA_CHN_TYPE_E
{
    DMA_FIXED_MODE = 0,   /* �̶�ͨ����ʹ�����֮�󲻻��Զ��ͷţ����ǿ���ͨ���ͷ�ͨ���ӿ��ͷ�*/
    DMA_COMMON_MODE = 1,   /* ��ͨͨ����ʹ�����֮����Զ��ͷţ�Ҳ����ͨ���ͷ�ͨ���ӿ��ͷ�*/
    DMA_SPECIAL_MODE = 2   /* 
����ͨ����ʹ�����֮�󲻻��Զ��ͷţ�ͨ���ͷŽӿ�Ҳ�޷��ͷţ��Ҹ�ͨ����ҪĳЩ���⴦��Ŀǰֻ��Э��ջʹ��*/
}DMA_CHN_TYPE_E;

/* ͨ��״̬ */
typedef enum tagDMA_CHN_STATUS_E
{
    DMA_CHN_FREE = 0,   /* ͨ������ */
    DMA_CHN_BUSY = 1   /* ͨ��æ */
}DMA_CHN_STATUS_E;



/* DESCRIPTION
 * This data type is used for selecting the transfer width for the
 *  source and/or destination on a DMA channel when using the specified
 *  driver API functions. This data type maps directly to the AMBA AHB
 *  HSIZE parameter.
 * NOTES
 *  This data type relates directly to the following DMA Controller
 *  register(s) / bit field(s): (x = channel number)
 *    - CTLx.SRC_TR_WIDTH, CTLx.DST_TR_WIDTH
 */

typedef enum tagDMA_TRANS_WIDTH_E {
    DMA_TRANS_WIDTH_8   = 0x0,/* 8bitλ��*/
    DMA_TRANS_WIDTH_16  = 0x1,/* 16bitλ��*/
    DMA_TRANS_WIDTH_32  = 0x2,/* 32bitλ��*/
    DMA_TRANS_WIDTH_64  = 0x3,/* 64bitλ��*/
    DMA_TRANS_WIDTH_128 = 0x4,/* 128bitλ��*/
    DMA_TRANS_WIDTH_256 = 0x5 /* 256bitλ��*/
}DMA_TRANS_WIDTH_E;

/* DESCRIPTION
 *  This data type is used for selecting the burst transfer length
 *  on the source and/or destination of a DMA channel when using the
 *  specified driver API functions. These transfer length values do
 *  not relate to the AMBA HBURST parameter.
 * NOTES
 *  This data type relates directly to the following DMA Controller
 *  register(s) / bit field(s): (x = channel number)
 *    - CTLx.SRC_MSIZE, CTLx.DEST_MSIZE
 */
typedef enum tagDMA_BURST_TRANS_LENGTH_E
{
    DMA_MSIZE_1   = 0x0,/* burst���ȣ���һ�δ���ĸ���Ϊ1��*/
    DMA_MSIZE_4   = 0x1,/* burst���ȣ���һ�δ���ĸ���Ϊ4��*/
    DMA_MSIZE_8   = 0x2,/* burst���ȣ���һ�δ���ĸ���Ϊ8��*/
    DMA_MSIZE_16  = 0x3,/* burst���ȣ���һ�δ���ĸ���Ϊ16��*/
    DMA_MSIZE_32  = 0x4,/* burst���ȣ���һ�δ���ĸ���Ϊ32��*/
    DMA_MSIZE_64  = 0x5,/* burst���ȣ���һ�δ���ĸ���Ϊ64��*/
    DMA_MSIZE_128 = 0x6,/* burst���ȣ���һ�δ���ĸ���Ϊ128��*/
    DMA_MSIZE_256 = 0x7 /* burst���ȣ���һ�δ���ĸ���Ϊ256��*/
}DMA_BURST_TRANS_LENGTH_E;

/* DMA Ӳ�����ֽӿ�*/
typedef enum tagDMA_HS_HK_INTERFACE_E 
{
    DMA_SPI0_RX = 0x0,
    DMA_SPI0_TX = 0x1,
    DMA_SPI1_RX = 0x2,
    DMA_SPI1_TX = 0x3,
    DMA_SIO_RX = 0x4,
    DMA_SIO_TX = 0x5,
	DMA_SCI0_RX = 0x6,
    DMA_SCI0_TX = 0x7,
    DMA_HSUART_RX = 0x8,
    DMA_HSUART_TX = 0x9,
    DMA_UART0_RX = 0xa,
    DMA_UART0_TX = 0xb,
    DMA_UART1_RX = 0xc,
    DMA_UART1_TX = 0xd,
    DMA_SCI1_RX = 0xe,
    DMA_SCI1_TX = 0xf,
    DMA_HK_BUTT
}DMA_HS_HK_INTERFACE_E;

/**************************************************************************
  STRUCT����
**************************************************************************/
/*����������ṹ��*/
typedef struct tagDMA_LLI_S
{
    BSP_U32 u32Reserved[2]; /* reserved 8 bytes  PSҪ���ʽ��ʹ��ʱ��������*/
    struct  tagDMA_LLI_S *pNextLLI; /* next node */
    BSP_U32 TransLength;  /* transfer length */
    BSP_U32 SrcAddr; /*source address*/    
    BSP_U32 DstAddr; /*destination address��PS���Բ���ע�������*/
} DMA_LLI_S;

/*������Ҫ�����ķ���dmaӲ��Ҫ������ݽṹ*/
typedef struct tagDMA_LLI_PHY_S
{
    BSP_U32 SrcAddr; /*source address*/
    BSP_U32 DstAddr; /*destination address*/
    BSP_U32 NextLLI; /*pointer to next LLI  the pSrcAddr of next node*/
    BSP_U32 ulTransferCtrl_l; /*control word low 32 bits */
    BSP_U32 ulTransferCtrl_h; /*control word high 32 bits */
    BSP_U32 s_stat; /*source state*/
    BSP_U32 d_stat; /*dest state*/
} DMA_LLI_PHY_S;

/*����ͨ�������ṹ*/
typedef struct tagDMA_CHN_PARA_S
{
    BSP_U32 u32Chan; /*ͨ����*/
    BSP_VOID * pfuncDmaTcIsr;/* ����жϻص�����*/
    BSP_S32 s32DmaTcPara; /* ����жϻص���������*/
    BSP_VOID * pfuncDmaErrIsr;/* �����жϻص�����*/
    BSP_S32 s32DmaErrPara; /* �����жϻص���������*/
} DMA_CHN_PARA_S;

/*sturcture for single block*/
typedef struct tagDMA_SINGLE_BLOCK_S
{
    DMA_CHN_PARA_S stChnPara;/*����ͨ�������ṹ*/
    DMA_TRANS_TYPE_E enTransType; /*������������*/
    DMA_HS_HK_INTERFACE_E enSrcHSHKInterface;/* Ӳ�����ֽӿ�*/
    DMA_HS_HK_INTERFACE_E enDstHSHKInterface;/* Ӳ�����ֽӿ�*/
    DMA_TRANS_WIDTH_E enSrcWidth; /*Դλ��*/
    DMA_TRANS_WIDTH_E enDstWidth; /*Ŀ��λ�� */
    DMA_BURST_TRANS_LENGTH_E enSrcBurstLength; /*Դburst���� */
    DMA_BURST_TRANS_LENGTH_E enDstBurstLength; /*Ŀ��burst����*/
    BSP_U32 u32SrcAddr;/* Դ��ַ*/
    BSP_U32 u32DstAddr;/* Ŀ�ĵ�ַ*/
    BSP_U32 u32TransLength;/* �����ܳ���*/
} DMA_SINGLE_BLOCK_S;

/*ADD by w00169995*/
typedef struct tagAXI_DMA_TASK_HANDLE_S
{
	BSP_U32 u32SrcAddr;
	BSP_U32 u32DstAddr;
	BSP_U32 ulLength;
	BSP_U32 ulChIdx;
}AXI_DMA_TASK_HANDLE_S;


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
* �� �� ��  : BSP_DMA_Init
*
* ��������  : DMA��ʼ��
*
* �������  : ��
* �������  : ��
* �� �� ֵ  : ��
*
* �޸ļ�¼  :2009��1��20��   ����  ����

*****************************************************************************/
BSP_S32 BSP_DMA_Init(BSP_VOID);
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
* �� �� ֵ  :  DMAC_SUCCESS       ��װ�ɹ�
*             BSP_ERR_DMA_MODULE_NOT_INITED      DMAδ��ʼ��
*             BSP_ERR_DMA_INVALID_PARA  ��������
*             BSP_ERR_DMA_ALL_CHANNEL_BUSY ����ͨ��æ
*
* �޸ļ�¼  :2009��10��15��   liumengcun  ����

*****************************************************************************/
BSP_S32  BSP_DMA_GetIdleChannel(DMA_CHN_TYPE_E enChnType, BSP_U32 *pulChan);

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
*             DMA_NOT_INIT      DMAδ��ʼ��
*             DMA_PARA_INVALID  ��������
*             DMA_CHANNEL_BUSY  ͨ��æ
*
* �޸ļ�¼  :2008��12��26��   ����  ����

*****************************************************************************/
BSP_U32  BSP_DMA_ChannelRelease(BSP_U32 u32Chan);

/*****************************************************************************
* �� �� ��  : BSP_DMA_AllChannelCheck
*
* ��������  : DMAȫ��ͨ���Ƿ���м�⺯��
*
* �������  : ��
* �������  : pChannelInfo   ����ͨ��ʹ��״̬����bit�����ʾ�����ڴ������ݵ�ͨ��bitλ��1��
*               bit0��Ӧ0ͨ����bit1��Ӧ1ͨ�����Դ�����
* �� �� ֵ  : DMA_ERROR
*             DMAC_SUCCESS
* �޸ļ�¼  :2008��12��26��   ����  ����

*****************************************************************************/
BSP_S32  BSP_DMA_AllChannelCheck(BSP_U32 *pChannelInfo);

/*****************************************************************************
* �� �� ��  : BSP_DMA_OneChannelCheck
*
* ��������  : DMA����ͨ������״̬���
*
* �������  : BSP_U32 ulChan   ������ͨ����
* �������  : ��
* �� �� ֵ  : DMAC_SUCCESS    ��ͨ���������
*          DMA_TRXFER_ERROR   ��ͨ���������
*          DMA_NOT_FINISHED   ��ͨ������δ���
*          DMA_ERROR    δ֪״̬
*
* �޸ļ�¼  :2008��12��26��   ����  ����

*****************************************************************************/
BSP_S32 BSP_DMA_OneChannelCheck(BSP_U32 ulChan, BSP_U32 *pChannelStatus);

/*****************************************************************************
* �� �� ��  : BSP_DMA_SingleBlockStart
*
* ��������  : �������ݴ�������
*
* �������  : DMA_SINGLE_BLOCK_S stDMASingleBlock   ���鴫��ʹ�õĲ����ṹ��
*
* �������  : ��
* �� �� ֵ  : DMAC_SUCCESS    �����������
*          DMA_CHANNEL_INVALID   ����ͨ���Ŵ���
*          DMA_TRXFERSIZE_INVALID   ���봫�䳤�ȷǷ�
*          DMA_CHANNEL_BUSY         ͨ��æ
*
* �޸ļ�¼  :2008��12��26��   ����  ����

*****************************************************************************/
BSP_S32 BSP_DMA_SingleBlockStart(DMA_SINGLE_BLOCK_S * stDMASingleBlock);

/*****************************************************************************
* �� �� ��  : BSP_DMA_SetDstAddr
*
* ��������  : �ײ��ṩ��Э��ջ������Ŀ�ĵ�ַ������Э��ջĿ�ĵ�ַ�̶�����仯
*
* �������  : BSP_U32 u32Addr  �������Ŀ�ĵ�ַ
* �������  : ��
* �� �� ֵ  : DMAC_SUCCESS  ���óɹ�
*
* �޸ļ�¼  :2009��7��24��   ����  ����

*****************************************************************************/
BSP_U32 BSP_DMA_SetDstAddr(BSP_U32 u32Addr);

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

*****************************************************************************/
BSP_VOID * BSP_DMA_LliBuild(BSP_U32 ulChan, DMA_LLI_S *pFirstLLI);

/*****************************************************************************
* �� �� ��  : BSP_DMA_LliStart
*
* ��������  : �������ݴ�������ר�ú���
*
* �������  : BSP_U32 ulChan        ͨ����
*             BSP_VOID *pu32Head��   ����BSP_DMA_LliBuild��õĴ���������ͷָ��
* �������  : ��
* �� �� ֵ  : DMAC_SUCCESS    �����������
*          DMA_LLIHEAD_ERROR   ���������ͷ����
*          DMA_CHANNEL_INVALID   ����ͨ���Ŵ���
*          DMA_ERROR    ͨ�������л���DMAδ��ȷ��ʼ��
*
* �޸ļ�¼  :2008��12��26��   ����  ����

*****************************************************************************/
BSP_U32 BSP_DMA_LliStart(DMA_CHN_PARA_S * stChnPara, BSP_VOID *pu32Head);

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
BSP_S32 BSP_DMA_GetDar(BSP_U32 u32Chan, BSP_U32 *u32Dar);

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
BSP_S32 BSP_DMA_DisableChannel(BSP_U32 u32Chan);

BSP_S32  BSP_AXIDMAC_DrxBakReg(BSP_VOID);
BSP_VOID BSP_AXIDMAC_DrxRestoreReg(BSP_VOID);


/*****************************************************************************
* �� �� ��  : BSP_EDMA_CheckChannelBusy
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
extern BSP_S32 BSP_EDMA_CheckChannelBusy(BSP_U32 u32Channel);
#define DRV_EDMA_CHECK_CHANNEL_BUSY(u32Channel) BSP_EDMA_CheckChannelBusy(u32Channel)

/*****************************************************************************
* �� �� ��  : BSP_EDMA_SingleBlockStart
*
* ��������  : �������ݴ�������
*
* �������  : DMA_SINGLE_BLOCK_S stDMASingleBlock   ���鴫��ʹ�õĲ����ṹ��
*
* �������  : ��
* �� �� ֵ  : DMAC_SUCCESS    �����������
*          DMA_CHANNEL_INVALID   ����ͨ���Ŵ���
*          DMA_TRXFERSIZE_INVALID   ���봫�䳤�ȷǷ�
*          DMA_CHANNEL_BUSY         ͨ��æ
*
* �޸ļ�¼  :2008��12��26��   ����  ����

*****************************************************************************/
extern BSP_S32 BSP_EDMA_SingleBlockStart(DMA_SINGLE_BLOCK_S * stDMASingleBlock);
#define DRV_EDMA_SINGLE_BLOCK_START(stDMASingleBlock) BSP_EDMA_SingleBlockStart(stDMASingleBlock)

/*****************************************************************************
* �� �� ��  : BSP_EDMA_LliBuild
*
* ��������  : ����������
*
* �������  : BSP_U32 ulChan        ͨ����
*             DMA_LLI_S *pFirstLLI  ��Դ����ĵ�һ���ڵ�ָ��
* �������  : ��
* �� �� ֵ  : BSP_VOID *  ָ��ײ㽨���õĴ��������ͷ�ڵ��ָ��
*
* �޸ļ�¼  :2009��7��24��   ����  ����

*****************************************************************************/
extern BSP_VOID * BSP_EDMA_LliBuild(BSP_U32 ulChan, DMA_LLI_S *pFirstLLI);
#define DRV_EDMA_LLI_BUILD(ulChan,pFirstLLI) BSP_EDMA_LliBuild(ulChan,pFirstLLI)

/*****************************************************************************
* �� �� ��  : BSP_EDMA_LliStart
*
* ��������  : �������ݴ�������ר�ú���
*
* �������  : BSP_U32 ulChan        ͨ����
*             BSP_VOID *pu32Head��   ����BSP_DMA_LliBuild��õĴ���������ͷָ��
* �������  : ��
* �� �� ֵ  : DMAC_SUCCESS    �����������
*          DMA_LLIHEAD_ERROR   ���������ͷ����
*          DMA_CHANNEL_INVALID   ����ͨ���Ŵ���
*          DMA_ERROR    ͨ�������л���DMAδ��ȷ��ʼ��
*
* �޸ļ�¼  :2008��12��26��   ����  ����

*****************************************************************************/
extern BSP_U32 BSP_EDMA_LliStart(DMA_CHN_PARA_S * stChnPara, BSP_VOID *pu32Head);
#define DRV_EDMA_LLI_START(stChnPara,pu32Head) BSP_EDMA_LliStart(stChnPara,pu32Head)

/*****************************************************************************
* �� �� ��  : BSP_EDMA_GetRegistValue
*
* ��������  : ��ȡ�쳣ʱ���EDMACģ��Ĵ�����ֵ
*
* �������  : BSP_VOID
* �������  : NA
*
* �� �� ֵ  : NA
*
* ����˵��  : ��ά�ɲ�ӿں���
*
*****************************************************************************/
extern BSP_VOID BSP_EDMA_GetRegistValue(BSP_VOID);
#define DRV_EDMA_GET_REGIST_VALUE() BSP_EDMA_GetRegistValue()


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_DMAC_H__ */


