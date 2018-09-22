/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_NANDF.h
*
*   ��    �� :  z67193
*
*   ��    �� :  BSP���Ͷ���ͷ�ļ�
*
*   �޸ļ�¼ :  2012��04��14��  v1.00  z67193  ����
*************************************************************************/

#ifndef	__BSP_NANDF_H__
#define __BSP_NANDF_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */

/*************************NANDF BEGIN************************/
/**************************************************************************
  �궨�� 
**************************************************************************/
/*������*/
#define NANDF_OK              BSP_OK
#define NANDF_ERROR_INIT      BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_MODULE_NOT_INITED)  /*û�г�ʼ��*/
#define NANDF_ERROR_ARGS      BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_INVALID_PARA)  /*��������*/
#define NANDF_ERROR_MALLOC    BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_BUF_ALLOC_FAILED)  /*�ڴ�ռ����ʧ��*/
#define NANDF_ERROR_TIMEOUT   BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_RETRY_TIMEOUT)  /*ѭ�������ﵽ���*/
#define NANDF_BAD_BLOCK       BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 1)  /*����*/
#define NANDF_GOOD_BLOCK      BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 2)  /*�ÿ�*/
#define NANDF_ERROR_READ      BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 3)  /*������ʧ��*/
#define NANDF_ERROR_WRITE     BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 4)  /*д����ʧ��*/
#define NANDF_ERROR_ERASE     BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 5)  /*��������ʧ��*/
#define NANDF_ERROR_MARKBAD   BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 6)  /*��־����ʧ��*/
#define NANDF_ERROR_ERASE_MARKBAD   BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 7)  /*����ʧ�ܲ��ұ�־����ʧ��*/
#define NANDF_ERROR_WRITE_MARKBAD   BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 8)  /*д��ʧ�ܲ��ұ�־����ʧ��*/
#define NANDF_ERROR_STATUS_CHECK   BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 9)  /*��ȡ״̬�Ĵ�����״̬����ȷ*/
#define NANDF_ERROR_SEMTAKE   BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 10)  /*�����ź���ʧ��*/
#define NANDF_ERROR_SEMGIVE   BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 11)  /*�ͷ��ź���ʧ��*/
#define NANDF_ERROR_ADDR_OVERFLOW BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 12)  /* ��ַԽ�� */
#define NANDF_ERROR_MODULE_LOCKED  BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 13)

BSP_S32 BSP_NANDF_DrxBakReg();
BSP_VOID BSP_NANDF_DrxRestoreReg();


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_NANDF_H__ */


