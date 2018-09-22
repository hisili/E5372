/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_SSI.h
*
*   ��    �� :  z67193
*
*   ��    �� :  BSP���Ͷ���ͷ�ļ�
*
*   �޸ļ�¼ :  2012��04��14��  v1.00  z67193  ����
*************************************************************************/

#ifndef	__BSP_SSI_H__
#define __BSP_SSI_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/*SSI ����ӿ�begin*/
/*�����붨��*/
#define BSP_ERR_SSI_MODULE_NOT_INITED  BSP_DEF_ERR(BSP_MODU_SSI, BSP_ERR_MODULE_NOT_INITED)
#define BSP_ERR_SSI_INVALID_PARA        BSP_DEF_ERR(BSP_MODU_SSI, BSP_ERR_INVALID_PARA)
#define BSP_ERR_SSI_RETRY_TIMEOUT       BSP_DEF_ERR(BSP_MODU_SSI, BSP_ERR_RETRY_TIMEOUT)

#define BSP_ERR_SSI_SEM_CREATE           BSP_DEF_ERR(BSP_MODU_SSI, 1)
#define BSP_ERR_SSI_SEM_LOCK             BSP_DEF_ERR(BSP_MODU_SSI, 2)
#define BSP_ERR_SSI_SEM_UNLOCK           BSP_DEF_ERR(BSP_MODU_SSI, 3)
#define BSP_ERR_SSI_ATTR_NOTSET          BSP_DEF_ERR(BSP_MODU_SSI, 4)
#define BSP_ERR_SSI_TXFIFO_NOTEMPTY      BSP_DEF_ERR(BSP_MODU_SSI, 5)
#define BSP_ERR_SSI_RXFIFO_EMPTY         BSP_DEF_ERR(BSP_MODU_SSI, 6)
#define BSP_ERR_SSI_RXFIFO_NOTEMPTY      BSP_DEF_ERR(BSP_MODU_SSI, 7)
#define BSP_ERR_SSI_RXNUM_WRONG          BSP_DEF_ERR(BSP_MODU_SSI, 8)

/*SSI��ID��ASIC��ʹ��3ƬSSI*/
typedef enum tagSSI_DEV_ID
{
#ifdef ABB_COMSTAR
    SSI_ID0,
#else
    SSI_ID0,
    SSI_ID1,
    SSI_ID2,
    SSI_ID3,
#endif
    SSI_ID_MAX
}SSI_ID_E;

/*���ݳ��� */
typedef enum tagssi_DATALEN                                                       
{
    SSI_DATALEN_2BITS =  0x1,     /*  2 bit address */
    SSI_DATALEN_3BITS =  0x2,     /*  3 bit address */
    SSI_DATALEN_4BITS  = 0x3,     /*  4 bit address */                           
    SSI_DATALEN_5BITS  = 0x4,     /*  5 bit address */                           
    SSI_DATALEN_6BITS  = 0x5,     /*  6 bit address */                           
    SSI_DATALEN_7BITS  = 0x6,     /*  7 bit address */                           
    SSI_DATALEN_8BITS  = 0x7,     /*  8 bit address */                           
    SSI_DATALEN_9BITS  = 0x8,     /*  9 bit address */                           
    SSI_DATALEN_10BITS = 0x9,     /* 10 bit address */                           
    SSI_DATALEN_11BITS = 0xA,     /* 11 bit address */                           
    SSI_DATALEN_12BITS = 0xB,     /* 12 bit address */                           
    SSI_DATALEN_13BITS = 0xC,     /* 13 bit address */                           
    SSI_DATALEN_14BITS = 0xD,     /* 14 bit address */                           
    SSI_DATALEN_15BITS = 0xE,     /* 15 bit address */                           
    SSI_DATALEN_16BITS = 0xF,     /* 16 bit address */
    SSI_DATALEN_17BITS = 0x10,    /* 17 bit address */
    SSI_DATALEN_18BITS = 0x11,    /* 18 bit address */
    SSI_DATALEN_19BITS = 0x12,    /* 19 bit address */
    SSI_DATALEN_20BITS = 0x13,    /* 20 bit address */
    SSI_DATALEN_21BITS = 0x14,    /* 21 bit address */
    SSI_DATALEN_22BITS = 0x15,    /* 22 bit address */
    SSI_DATALEN_23BITS = 0x16,    /* 23 bit address */
    SSI_DATALEN_24BITS = 0x17,    /* 24 bit address */
    SSI_DATALEN_MAX
} SSI_DATA_LEN_E;

/*��ַ����*/
typedef enum tagssi_ADDRLEN                                                      
{                                                                              
    SSI_ADDRLEN_1BITS  = 0x0,     /*  4 bit data */                           
    SSI_ADDRLEN_2BITS  = 0x1,     /*  5 bit data */                           
    SSI_ADDRLEN_3BITS  = 0x2,     /*  6 bit data */                           
    SSI_ADDRLEN_4BITS  = 0x3,     /*  7 bit data */                           
    SSI_ADDRLEN_5BITS  = 0x4,     /*  8 bit data */                           
    SSI_ADDRLEN_6BITS  = 0x5,     /*  9 bit data */                           
    SSI_ADDRLEN_7BITS  = 0x6,     /* 10 bit data */                           
    SSI_ADDRLEN_8BITS  = 0x7,     /* 11 bit data */     
    SSI_ADDRLEN_MAX
} SSI_ADDR_LEN_E;    

/*��ʱ�ȴ�*/
typedef enum tagssi_WAITTIME
{
    SSI_WAITTIME_1CYCLES = 0x0,
    SSI_WAITTIME_2CYCLES = 0x1,
    SSI_WAITTIME_3CYCLES = 0x2,
    SSI_WAITTIME_4CYCLES = 0x3,
    SSI_WAITTIME_5CYCLES = 0x4,
    SSI_WAITTIME_6CYCLES = 0x5,
    SSI_WAITTIME_7CYCLES = 0x6,
    SSI_WAITTIME_8CYCLES = 0x7,
    SSI_WAITTIME_9CYCLES = 0x8,
    SSI_WAITTIME_10CYCLES = 0x9,
    SSI_WAITTIME_11CYCLES = 0xA,
    SSI_WAITTIME_12CYCLES = 0xB,
    SSI_WAITTIME_13CYCLES = 0xC,
    SSI_WAITTIME_14CYCLES = 0xD,
    SSI_WAITTIME_15CYCLES = 0xE,
    SSI_WAITTIME_16CYCLES = 0xF,
    SSI_WAITTIME_MAX
}SSI_WAIT_TIME_E;

/*SSI����豸���Խṹ�壬�����豸�����ݳ��ȣ���ַ���ȣ��ȴ���ʱʱ��� */
typedef struct tagSSI_DEV_ATTR
{
    SSI_ADDR_LEN_E enAddrLen;
    SSI_DATA_LEN_E enDataLen;
    SSI_WAIT_TIME_E enWaitTime;
}SSI_SLAVE_ATTR_S;

/*****************************************************************************
* �� �� ��  : BSP_SSI_SetAttr
*
* ��������  : ����SSI�ļĴ��������ÿ��������ֳ��ȡ�����֡���ȵȡ�
*
* �������  : enSSIID        ��Ҫ���õ�SSI�ţ��Լ�������ƬƬѡ�������õ�Ƭѡ�š�
*             pstSSIDevAttr  ��¼SSI����豸���ԵĽṹ��ָ�룬�ṹ���Ա�����豸
                             ��Ҫ�������ֳ��ȣ�����֡���ȣ�ʹ��Э�飬�����ʵȡ�
*
* �������  : ��
*
* �� �� ֵ  : OK    ���ճɹ�
*             ERROR ����ʧ��
*****************************************************************************/
BSP_S32 BSP_SSI_SetAttr(SSI_ID_E enSsiId, SSI_SLAVE_ATTR_S *pstSsiDevAttr);

/*****************************************************************************
* �� �� ��  : BSP_SSI_Write
*
* ��������  : ͨ��SSI���豸д������
*
* �������  : pstWriteData ��¼��SSI���ݴ����й���Ϣ�Ľṹ��ָ�룬��Ա����
                           Ҫ��д��SSI�ţ�Ƭѡ�ţ�����ģʽ��
*             pSendData    �洢���յ����ݻ�����ָ��
*             u32Length    �����յ����ݳ���
*
* �������  : ��
*
* �� �� ֵ  : OK    ���ճɹ�
*             ERROR ����ʧ��
*****************************************************************************/
BSP_S32 BSP_SSI_Write(SSI_ID_E enSsiId, BSP_VOID *pSrcAddr, BSP_U32 u32Length);

/*****************************************************************************
* �� �� ��  : BSP_SSI_Read
*
* ��������  : ͨ��SSI��ȡ�豸����
*
* �������  : pstReadData  ��¼��SSI���ݴ����й���Ϣ�Ľṹ��ָ�룬��Ա����
                           Ҫ��д��SSI�ţ�Ƭѡ�ţ�����ģʽ�ȡ� 
*             u32Length    �����յ����ݳ���
*
* �������  : pRecData     �洢���յ����ݻ�����ָ�롣
*
* �� �� ֵ  : OK    ���ճɹ�
*             ERROR ����ʧ��
*****************************************************************************/
BSP_S32 BSP_SSI_Read(SSI_ID_E enSsiId,BSP_VOID *pSrcAddr,BSP_VOID *pDescAddr,BSP_U32 u32Length);

/*****************************************************************************
* �� �� ��  : BSP_SSI_GetAttr
*
* ��������  : ��ȡ��ǰSSI���õ�����
*
* �������  : enSSIId    Ҫ��ѯ��SSI�š�                        
*            
* �������  : pstDevAttr ���SSI���ԵĽṹ��ָ�롣
*
* �� �� ֵ  : OK    ���ճɹ�
*             ERROR ����ʧ��
*****************************************************************************/
BSP_S32 BSP_SSI_GetAttr(SSI_ID_E enSsiId, SSI_SLAVE_ATTR_S *pstDevAttr);
/*SSI����ӿ�End*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_SSI_H__ */

