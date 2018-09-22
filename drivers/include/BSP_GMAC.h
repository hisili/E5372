/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_GMAC.h
*
*   ��    �� :  z67193
*
*   ��    �� :  BSP���Ͷ���ͷ�ļ�
*
*   �޸ļ�¼ :  2012��04��14��  v1.00  z67193  ����
*************************************************************************/

#ifndef	__BSP_GMAC_H__
#define __BSP_GMAC_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */



/*************************GMAC BEGIN************************/


/******************************************************************************
* Function     :   BSP_GMAC_PowerDown
* 
* Description  :   
* 
* Input        :  
* 
* Output       :   ��
* 
* return       :   
******************************************************************************/
BSP_S32 BSP_GMAC_PowerDown(BSP_VOID);
/******************************************************************************
* Function     :   BSP_GMAC_PowerUp
* 
* Description  :   
* 
* Input        :  
* 
* Output       :   ��
* 
* return       :   
******************************************************************************/
BSP_S32 BSP_GMAC_PowerUp(BSP_VOID);
/******************************************************************************
* Function     :   BSP_GMAC_RegisterUpLinkRxCb
* 
* Description  :   ע��������·���ջص����� 
* 
* Input        :  GMAC_OWNER_E eOwner,     ������ 
                  UpLinkRxFunc pfunc       ������·���ջص����� 
* 
* Output       :   ��
* 
* return       :   
******************************************************************************/
BSP_S32 BSP_GMAC_RegisterUpLinkRxCb(GMAC_OWNER_E eOwner,   UpLinkRxFunc pfunc );
/******************************************************************************
* Function     :   BSP_GMAC_RegisterFreePktEncapCb
* 
* Description  :    ע�����װ�ͷŻص�����  
* 
* Input        :  GMAC_OWNER_E eOwner,     ������ 
                  FreePktEncap pfunc      ����װ�ͷŻص����� 
* 
* Output       :   ��
* 
* return       :   
******************************************************************************/
BSP_S32 BSP_GMAC_RegisterFreePktEncapCb( GMAC_OWNER_E eOwner, FreePktEncap pfunc );
/******************************************************************************
* Function     :   BSP_GMAC_SetPktEncapInfo
* 
* Description  :    ���ð���װ�ṹ��Ϣ
* 
* Input        :  GMAC_OWNER_E eOwner,    ������
                  BSP_S32 s32BufOft,      bufƫ����
                  BSP_S32 s32LenOft,      lenƫ����
                  BSP_S32 s32NextOft      nextƫ����
* 
* Output       :   ��
* 
* return       :   
******************************************************************************/
BSP_S32 BSP_GMAC_SetPktEncapInfo(GMAC_OWNER_E eOwner, BSP_S32 s32BufOft, BSP_S32 s32LenOft,BSP_S32 s32NextOft);
/******************************************************************************
* Function     :   BSP_GMAC_DownLinkTx
* 
* Description  :    �������ݷ��ͺ���
* 
* Input        :  GMAC_OWNER_E eOwner,    ����ģ��
                  BSP_VOID *pPktEncap     ����װ�׵�ַ
* 
* Output       :   ��
* 
* return       :   
******************************************************************************/
BSP_S32 BSP_GMAC_DownLinkTx(GMAC_OWNER_E eOwner, BSP_VOID *pPktEncap );
/*************************GMAC END************************/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_GMAC_H__ */


