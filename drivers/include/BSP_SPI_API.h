/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_SPI_API.h
*
*   ��    �� :  z67193
*
*   ��    �� :  BSP���Ͷ���ͷ�ļ�
*
*   �޸ļ�¼ :  2012��04��14��  v1.00  z67193  ����
*************************************************************************/

#ifndef	__BSP_SPI_API_H__
#define __BSP_SPI_API_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/*�����붨��*/
#define BSP_ERR_SPI_MODULE_NOT_INITED BSP_DEF_ERR(BSP_MODU_SPI, BSP_ERR_MODULE_NOT_INITED)
#define BSP_ERR_SPI_INVALID_PARA      BSP_DEF_ERR(BSP_MODU_SPI, BSP_ERR_INVALID_PARA)
#define BSP_ERR_SPI_RETRY_TIMEOUT     BSP_DEF_ERR(BSP_MODU_SPI, BSP_ERR_RETRY_TIMEOUT)

#define BSP_ERR_SPI_SEM_CREATE        BSP_DEF_ERR(BSP_MODU_SPI, 1)
#define BSP_ERR_SPI_SEM_LOCK          BSP_DEF_ERR(BSP_MODU_SPI, 2)
#define BSP_ERR_SPI_SEM_UNLOCK        BSP_DEF_ERR(BSP_MODU_SPI, 3)
#define BSP_ERR_SPI_DATASIZE_INVALID  BSP_DEF_ERR(BSP_MODU_SPI, 4)
#define BSP_ERR_SPI_ATTR_NOTSET       BSP_DEF_ERR(BSP_MODU_SPI, 5)
#define BSP_ERR_SPI_PROT_INVALID      BSP_DEF_ERR(BSP_MODU_SPI, 6)
#define BSP_ERR_SPI_TMOD_INVALID      BSP_DEF_ERR(BSP_MODU_SPI, 7)
#define BSP_ERR_SPI_RECEIVE_POLL      BSP_DEF_ERR(BSP_MODU_SPI, 8)


/*SPI��ID��ASIC��ʹ��3ƬSPI*/
typedef enum tagSPI_DEV_ID
{
    SPI_ID0,
    SPI_ID1,
    SPI_ID_MAX
}SPI_DEV_ID_E;


/*ÿ��SPI�ϵ�Ƭѡ�ţ���ǰÿ��SPI��4��Ƭѡ*/
typedef enum tagSPI_DEV_CS
{
    SPI_DEV_CS0,
    SPI_DEV_CS1,
    SPI_DEV_CS2,
    SPI_DEV_CS3,
    SPI_DEV_CS_MAX,
    NO_OWNER = -1
}SPI_DEV_CS_E;


/*���ݴ��䷽ʽ������DMA��ʽ����ѭ��ʽ*/
typedef enum tagSPI_SENDMOD
{
    SPI_SENDMOD_POLLING,                                                          
    SPI_SENDMOD_DMA,
    SPI_SENDMOD_MAX
}RXTX_MODE_E;


/* ��¼SPI��ID��Ƭѡ�ŵĽṹ��*/
typedef struct tagSPI_DEV_STRUCTION
{
    SPI_DEV_ID_E enSpiId;
    SPI_DEV_CS_E enSpiCs;
}SPI_DEV_S;

/* ���������ֳ��� */
typedef enum tagspi_CommandLen                                                       
{
    SPI_COMMANDSIZE_1BITS =  0x0,     /*  1 bit command */
    SPI_COMMANDSIZE_2BITS =  0x1,     /*  2 bit command */
    SPI_COMMANDSIZE_3BITS =  0x2,     /*  3 bit command */
    SPI_COMMANDSIZE_4BITS  = 0x3,     /*  4 bit command */                           
    SPI_COMMANDSIZE_5BITS  = 0x4,     /*  5 bit command */                           
    SPI_COMMANDSIZE_6BITS  = 0x5,     /*  6 bit command */                           
    SPI_COMMANDSIZE_7BITS  = 0x6,     /*  7 bit command */                           
    SPI_COMMANDSIZE_8BITS  = 0x7,     /*  8 bit command */                           
    SPI_COMMANDSIZE_9BITS  = 0x8,     /*  9 bit command */                           
    SPI_COMMANDSIZE_10BITS = 0x9,     /* 10 bit command */                           
    SPI_COMMANDSIZE_11BITS = 0xA,     /* 11 bit command */                           
    SPI_COMMANDSIZE_12BITS = 0xB,     /* 12 bit command */                           
    SPI_COMMANDSIZE_13BITS = 0xC,     /* 13 bit command */                           
    SPI_COMMANDSIZE_14BITS = 0xD,     /* 14 bit command */                           
    SPI_COMMANDSIZE_15BITS = 0xE,     /* 15 bit command */                           
    SPI_COMMANDSIZE_16BITS = 0xF,      /* 16 bit command */ 
    SPI_COMMANDSIZE_MAX
} SPI_COMMAND_LEN_E;

/*����֡����*/
typedef enum tagspi_DataLen                                                       
{                                                                              
    SPI_DATASIZE_4BITS  = 0x3,     /*  4 bit data */                           
    SPI_DATASIZE_5BITS  = 0x4,     /*  5 bit data */                           
    SPI_DATASIZE_6BITS  = 0x5,     /*  6 bit data */                           
    SPI_DATASIZE_7BITS  = 0x6,     /*  7 bit data */                           
    SPI_DATASIZE_8BITS  = 0x7,     /*  8 bit data */                           
    SPI_DATASIZE_9BITS  = 0x8,     /*  9 bit data */                           
    SPI_DATASIZE_10BITS = 0x9,     /* 10 bit data */                           
    SPI_DATASIZE_11BITS = 0xA,     /* 11 bit data */                           
    SPI_DATASIZE_12BITS = 0xB,     /* 12 bit data */                           
    SPI_DATASIZE_13BITS = 0xC,     /* 13 bit data */                           
    SPI_DATASIZE_14BITS = 0xD,     /* 14 bit data */                           
    SPI_DATASIZE_15BITS = 0xE,     /* 15 bit data */                           
    SPI_DATASIZE_16BITS = 0xF,     /* 16 bit data */
    SPI_DATASIZE_MAX
} SPI_DATA_LEN_E;    

/* SPI֧�ֵ�Э��*/
typedef enum tagSPI_PROT
{
    SPI_PROT_SPI,
    SPI_PROT_SSP,
    SPI_PROT_NSM,
    SPI_PROT_MAX
}SPI_PROT_E;

typedef enum tagSPI_SCPOL
{
	SPI_SCPOL_LOW, //�½��ش���
	SPI_SCPOL_HIGH,//�����ش���
	SPI_SCPOL_MAX
}SPI_SCPOL_E;

/*SPI����豸���Խṹ�壬�����豸��Ҫ�������ֳ��ȣ�����֡���ȣ�ʹ��Э��� */
typedef struct tagSPI_DEV_ATTR
{
    SPI_COMMAND_LEN_E enCommandLen;
    SPI_DATA_LEN_E enDataLen;
    SPI_PROT_E enSpiProt;
	SPI_SCPOL_E enSpiScpol;
    BSP_U16 u16SpiBaud;
}SPI_DEV_ATTR_S;

/*��¼��SPI���ݴ��������Ϣ�Ľṹ�壬��Ա����Ҫ��д��SPI�ţ�Ƭѡ�ţ�����ģʽ��
  ���ݵ�ַ�����ݳ��ȵ�*/                                     
typedef struct tagSPI_DATA_HANDLE
{
    SPI_DEV_ID_E enSpiID;
    SPI_DEV_CS_E enCsID;
    RXTX_MODE_E enMode;
    void *pvCmdData;
    BSP_U32 u32length;
}SPI_DATA_HANDLE_S;


/*****************************************************************************
* �� �� ��  : BSP_SPI_SetAttr
*
* ��������  : ����SPI�ļĴ��������ÿ��������ֳ��ȡ�����֡���ȵȡ�
*
* �������  : enSpiID        ��Ҫ���õ�SPI�ţ��Լ�������ƬƬѡ�������õ�Ƭѡ�š�
*             pstSpiDevAttr  ��¼SPI����豸���ԵĽṹ��ָ�룬�ṹ���Ա�����豸
                             ��Ҫ�������ֳ��ȣ�����֡���ȣ�ʹ��Э�飬�����ʵȡ�
*
* �������  : ��
*
* �� �� ֵ  : OK    ���ճɹ�
*             ERROR ����ʧ��
*****************************************************************************/
BSP_S32 BSP_SPI_SetAttr(SPI_DEV_S *enSpiID,SPI_DEV_ATTR_S *pstSpiDevAttr);

/*****************************************************************************
* �� �� ��  : BSP_SPI_Write
*
* ��������  : ͨ��SPI���豸д������
*
* �������  : pstWriteData ��¼��SPI���ݴ����й���Ϣ�Ľṹ��ָ�룬��Ա����
                           Ҫ��д��SPI�ţ�Ƭѡ�ţ�����ģʽ��
*             pSendData    �洢���յ����ݻ�����ָ��
*             u32Length    �����յ����ݳ���
*
* �������  : ��
*
* �� �� ֵ  : OK    ���ճɹ�
*             ERROR ����ʧ��
*****************************************************************************/
BSP_S32 BSP_SPI_Write(SPI_DATA_HANDLE_S *pstWriteData,BSP_VOID *pSendData, BSP_U32 u32Length);

/*****************************************************************************
* �� �� ��  : BSP_SPI_Read
*
* ��������  : ͨ��SPI��ȡ�豸����
*
* �������  : pstReadData  ��¼��SPI���ݴ����й���Ϣ�Ľṹ��ָ�룬��Ա����
                           Ҫ��д��SPI�ţ�Ƭѡ�ţ�����ģʽ�ȡ� 
*             u32Length    �����յ����ݳ���
*
* �������  : pRecData     �洢���յ����ݻ�����ָ�롣
*
* �� �� ֵ  : OK    ���ճɹ�
*             ERROR ����ʧ��
*****************************************************************************/
BSP_S32 BSP_SPI_Read(SPI_DATA_HANDLE_S *pstReadData,BSP_VOID *pRecData, BSP_U32 u32Length);

/*****************************************************************************
* �� �� ��  : BSP_SPI_GetAttr
*
* ��������  : ��ȡ��ǰSPI���õ�����
*
* �������  : enSpiId    Ҫ��ѯ��SPI�š�                        
*            
* �������  : pstDevAttr ���SPI���ԵĽṹ��ָ�롣
*
* �� �� ֵ  : OK    ���ճɹ�
*             ERROR ����ʧ��
*****************************************************************************/
BSP_S32 BSP_SPI_GetAttr(SPI_DEV_ID_E enSpiId, SPI_DEV_ATTR_S *pstDevAttr);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_SPI_API_H__ */


