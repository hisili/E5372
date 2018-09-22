/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_FTPLOAD.h
*
*   ��    �� :  z67193
*
*   ��    �� :  BSP���Ͷ���ͷ�ļ�
*
*   �޸ļ�¼ :  2012��04��14��  v1.00  z67193  ����
*************************************************************************/

#ifndef	__BSP_FTPLOAD_H__
#define __BSP_FTPLOAD_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/**************************************************************************
  �궨�� 
**************************************************************************/

/**************************************************************************
  �����붨��
**************************************************************************/
#define BSP_ERR_FTP_NOT_INIT            BSP_DEF_ERR(BSP_MODU_FTP, BSP_ERR_MODULE_NOT_INITED)    /*û�г�ʼ��*/
#define BSP_ERR_FTP_NULL_PTR            BSP_DEF_ERR(BSP_MODU_FTP, BSP_ERR_NULL_PTR)             /*��������*/
#define BSP_ERR_FTP_INVALID_PARAM       BSP_DEF_ERR(BSP_MODU_FTP, BSP_ERR_INVALID_PARA)         /*�ڴ�ռ����ʧ��*/
#define BSP_ERR_FTP_NO_MEM              BSP_DEF_ERR(BSP_MODU_FTP, BSP_ERR_BUF_ALLOC_FAILED)     /*ѭ�������ﵽ���*/

#define BSP_ERR_FTP_NET                 BSP_DEF_ERR(BSP_MODU_FTP, 0x10)                         /*�������*/
#define BSP_ERR_FTP_XFER                BSP_DEF_ERR(BSP_MODU_FTP, 0x11)                         /*FTP���鴫��ʧ��*/
#define BSP_ERR_FTP_DATA_TRANSFER       BSP_DEF_ERR(BSP_MODU_FTP, 0x12)                         /*���ݴ���ʧ��*/
#define BSP_ERR_FTP_CMD                 BSP_DEF_ERR(BSP_MODU_FTP, 0x13)                         /*�����ʧ��*/
#define BSP_ERR_FTP_READ                BSP_DEF_ERR(BSP_MODU_FTP, 0x14)                         /*��ʧ��*/
#define BSP_ERR_FTP_WRITE               BSP_DEF_ERR(BSP_MODU_FTP, 0x15)                         /*дʧ��*/

/**************************************************************************
  ö�ٶ���
**************************************************************************/

/*FTP transport type*/
typedef enum tagFTP_LOAD_TYPE_E
{
    FTP_LOADTYPE_VXWORKS_IMAGE = 0,
	FTP_LOADTYPE_TENCILICA_TEXT,
	FTP_LOADTYPE_TENCILICA_BOOTCODE,
    FTP_LOADTYPE_BOOTSECT,
    FTP_LOADTYPE_COMMONFILE,
    FTP_LOADTYPE_IMPORT,
    FTP_LOADTYPE_IPCFG,
    FTP_LOADTYPE_WITHSPARE,
    FTP_LOADTYPE_SYSTEMIMG,
    FTP_LOADTYPE_BUTTOM
} FTP_LOAD_TYPE_E;


/*****************************************************************************
* �� �� ��  : LOADF
*
* ��������  : 
*
* �������  : BSP_U8 * pu8FileName, BSP_U32 u32DestAddr, FTP_LOAD_TYPE_E enLoadType
* 
* �������  : ��
*
* �� �� ֵ  : 
*
* ����˵��  : 
*
*****************************************************************************/
BSP_S32 LOADF(BSP_U8 * pu8FileName, BSP_U32 u32DestAddr, FTP_LOAD_TYPE_E enLoadType);

/*****************************************************************************
* �� �� ��  : LOADB
*
* ��������  : 
*
* �������  : ��
* 
* �������  : ��
*
* �� �� ֵ  : 
*
* ����˵��  : 
*
*****************************************************************************/
BSP_S32 LOADB(void);

/*****************************************************************************
* �� �� ��  : BSP_LoadDsp
*
* ��������  : 
*
* �������  : ��
* 
* �������  : ��
*
* �� �� ֵ  : 
*
* ����˵��  : 
*
*****************************************************************************/
BSP_S32 BSP_LoadDsp(void);

/*****************************************************************************
* �� �� ��  : BSP_RunDsp
*
* ��������  : 
*
* �������  : ��
* 
* �������  : ��
*
* �� �� ֵ  : 
*
* ����˵��  : 
*
*****************************************************************************/
BSP_VOID BSP_RunDsp(void);

/*****************************************************************************
* �� �� ��  : BSP_BootDsp
*
* ��������  : 
*
* �������  : ��
* 
* �������  : ��
*
* �� �� ֵ  : 
*
* ����˵��  : 
*
*****************************************************************************/
BSP_S32 BSP_BootDsp(void);
BSP_VOID BSP_ReRunDsp();


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_FTPLOAD_H__ */


