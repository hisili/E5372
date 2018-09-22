
/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_SEC.h
*
*   ��    �� :  
*
*   ��    �� :  SECģ���û��ӿ��ļ�
*
*   �޸ļ�¼ :  2012��04��16��  v1.00  ����
*************************************************************************/

#ifndef __BSP_SEC_H__
#define __BSP_SEC_H__

#ifdef __cplusplus
extern "C" 
{ 
#endif /* __cplusplus */



/*----------��ȫģ�� ----------------------- */
#define MAX_N_LENGTH    32

#define BSP_ERR_SEC_BASE            BSP_DEF_ERR(BSP_MODU_SECURITY, 0)
#define BSP_ERR_SEC_OEMCA             (BSP_ERR_SEC_BASE + 1)     /*OEM CA У��ERROR*/
#define BSP_ERR_SEC_IMG             (BSP_ERR_SEC_BASE + 2)     /*ӳ��У��ERROR*/

#define BSP_ERR_EFUSE_WRT_REFUSE    (BSP_ERR_SEC_BASE + 3)     /*Efsue��������д*/
#define BSP_ERR_EFUSE_OP_TIMEOUT    (BSP_ERR_SEC_BASE + 4)     /*Efsue��������д*/
/* ��Կ��ʽ */
typedef struct
{
    BSP_U16  eLen;    /* ��λΪbits,���ܴ���1024 */
    BSP_U16  nLen;    /* ��λΪbits,���ܴ���1024 */
    BSP_U32  e[MAX_N_LENGTH];       /* ��e(��Կ) */
    BSP_U32  n[MAX_N_LENGTH];       /* ģ�� */
}KEY_STRUCT;


/*****************************************************************************
* �� �� ��  : efuseRead
*
* ��������  : �����ȡEFUSE�е�����
*
* �������  : group  ��ʼgroup
*                   num  ���鳤��(word��,������512/4=128)
* �������  : pBuf ��EFUSE�е�����
*
* �� �� ֵ  : 
*
* ����˵��  : 
*
*****************************************************************************/
int efuseRead(BSP_U32 group, BSP_U32* pBuf, BSP_U32 num);

/*****************************************************************************
* �� �� ��  : secInit
*
* ��������  : ��ȫ������ʼ��
*
* �������  :
* �������  :
*
* �� �� ֵ  :
*
* ����˵��  :
*
*****************************************************************************/
BSP_S32 secInit(BSP_VOID);
/*****************************************************************************
* �� �� ��  : secDataRsa
*  Description:
*      RSA.
*
*  Calls:
*      SsdmaRsaInit, SetSsdmaRsaMode
*
*  Input:
*         pstCa: ��Կ
*         pucInBuf: �������ݵ�ָ��(�������ݵĳ��Ȳ�����128Byte)
*         ulInLen:�������ݵĳ���(������128Byte)
*
*
*  Output:
*         pOutdata: ������ݵ�ָ��(������ݵĳ��Ȳ�����128Byte)
*         pulOutLen: ������ݵĳ���(128Byte)
*
*
*  Return:
*         BSP_OK: the function successful returned.
*         BSP_ERROR: the function encounter SSDMA _ERROR while running.
*  Other: ������RSA��������ݵĳ��Ⱥ�RSA�������������ݵĳ���
*****************************************************************************/
BSP_S32 secDataRsa(const KEY_STRUCT *pstCa, const BSP_U8*  pucInBuf, BSP_U32 ulInLen, BSP_U8* pucOutBuf, BSP_U32* pulOutLen);

/******************************************************************************
 *  Function:  secDataSha
 *  Description:
 *      SHA1.
 *  Calls:
 *      SsdmaSha1Init, SetSsdmaSha1Iv, RunSsdmaSha1, byteCopy
 *  Called By:
 *      identify
 *  Input:
 *         pucInBuf:   ָ���������ݵ�ַ��ָ��
 *         pucData:   �������ݵĳ���(�ֽ���)
 *         pucOutBuf: ָ��������ݵ�ַ��ָ��
 *  Output:
 *         hash: ָ���������ݽ���SHA1�����Ĺ�ϣֵ��ָ��(��ϣֵ�ĳ���Ϊ20Byte)
 *  Return:
 *         BSP_OK: the function successful returned.
 *         BSP_ERROR: the function encounter BSP_ERROR while running.
 *
 ********************************************************************************/
BSP_S32 secDataSha(const BSP_U8*  pucInBuf, BSP_U32 ulInLen, BSP_U8* pucOutBuf);

/*****************************************************************************
* �� �� ��  : secIdioChk
*
* ��������  :   ͨ��У���������ݵ�hashֵ�� ��
*                    ����ǩ����RSA���ܺ��ֵһ��
*                    ��У���������ݵĺϷ��� *
*
* �������  :   pucData--�������ݵĵ�ַ
*                   dataLen--�������ݵĳ���
*                   pubKey--��Կ
*                   pIdio--��������ǩ����ָ��
* �������  :
*
* �� �� ֵ  : SSDMA_OK/ SSDMA_ERROR
*
* ����˵��  :
*
*****************************************************************************/
BSP_S32 secIdioChk(const BSP_U8* pucData, BSP_U32 dataLen, const KEY_STRUCT *pubKey, const BSP_U32* pIdio);

/*****************************************************************************
* �� �� ��  : secCheck
*
* ��������  :   ͨ��У���������ݵ�hashֵ�� ��
*                    ����ǩ����RSA���ܺ��ֵһ��
*                    ��У���������ݵĺϷ��� *
*
* �������  : KEY_STRUCT *pubKey����Կ
*                   ulImgAddr--ӳ���ַ
*                  ulImgLen--���񳤶�
* �������  :
*
* �� �� ֵ  : BSP_OK           ��ȫУ��ͨ��
*                BSP_ERR_SEC_OEMCA        OEM CAУ�����
*                BSP_ERR_SEC_IMG        ����ǩ��У�����
*
* ����˵��  :
*
*****************************************************************************/
BSP_S32 secCheck(const KEY_STRUCT *pubKey, BSP_U32 ulImgAddr, BSP_U32 ulImgLen);

/*****************************************************************************
* �� �� ��  : efuseWriteAt
*
* ��������  : ��AT�������ֱ����дEfsue
*
* �������  : ��
* �������  :
*
* �� �� ֵ  : BSP_OK,��д�ɹ�
*             BSP_ERROR,��дʧ��
*
* ����˵��  :
*
*****************************************************************************/
BSP_S32 efuseWriteAt(BSP_VOID);

/*****************************************************************************
* �� �� ��  : secIsSecVer
*
* ��������  : �жϵ�ǰ���а汾�Ƿ�Ϊ��ȫ�汾
*
* �������  : 
* �������  : 
*
* �� �� ֵ  : 
*
* ����˵��  : 
*
*****************************************************************************/
BSP_BOOL secIsSecVer( BSP_VOID );


#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif /* __cplusplus */

#endif

