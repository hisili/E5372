/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_GPIO.h
*
*   ��    �� :  
*
*   ��    �� :  IPFģ���û��ӿ��ļ�
*
*   �޸ļ�¼ :  2012��04��16��  v1.00  ����
*************************************************************************/

#ifndef __BSP_SOCP_H__
#define __BSP_SOCP_H__

#ifdef __cplusplus
extern "C" 
{ 
#endif /* __cplusplus */


/*************************SOCP BEGIN************************/
/**************************************************************************
  �궨�� 
**************************************************************************/

BSP_S32 BSP_SOCP_DrxBakReg() ;
BSP_VOID BSP_SOCP_DrxRestoreReg() ;

BSP_S32 BSP_BBPDMA_DrxBakReg();
BSP_S32 BSP_BBPDMA_DrxRestoreReg();
BSP_U32 BSP_SOCP_CanSleep(BSP_VOID);
BSP_VOID BSP_SOCP_DrxRestoreRegAppOnly(BSP_VOID);
/*****************************************************************************
* �� �� ��  : BSP_SOCP_CoderSetHifiSrcChan
* ��������  : ����̶�ͨ��
* �������  : Ŀ��ͨ��ID
* �������  : ��
* �� �� ֵ  :
*****************************************************************************/
extern BSP_S32 BSP_SOCP_CoderSetHifiSrcChan(BSP_U32 u32DestChanID);
#define  DRV_SOCP_CoderSetHifiSrcChan(u32DestChanID)  BSP_SOCP_CoderSetHifiSrcChan(u32DestChanID)

/*****************************************************************************
* �� �� ��  : BSP_SOCP_StartHifiChan
* ��������  : �����̶�ͨ��
* �������  : ��
* �������  : ��
* �� �� ֵ  :
*****************************************************************************/
extern BSP_VOID BSP_SOCP_StartHifiChan(BSP_VOID);
#define DRV_SOCP_StartHifiChan BSP_SOCP_StartHifiChan

/*************************SOCP END************************/




#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif /* __cplusplus */

#endif
