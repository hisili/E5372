/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_OM_API.h
*
*   ��    �� :  z67193
*
*   ��    �� :  BSP���Ͷ���ͷ�ļ�
*
*   �޸ļ�¼ :  2012��04��14��  v1.00  z67193  ����
*************************************************************************/

#ifndef	__BSP_OM_API_H__
#define __BSP_OM_API_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */



/*************************OM BEGIN************************/

typedef enum{
     MEMCHKTYPE16BIT,
     MEMCHKTYPE32BIT
}ENMEMCHKTYPE;

/*****************************************************************************
* �� �� ��  : BSP_OM_GetFlashSpec
*
* ��������  : ��ȡFLASH���
*
* �������  : ��
*
* �������  : BSP_U32 *pu32FlashTotalSize  : FLASH�ܴ�С
*             BSP_U32 *pu32FlashBlkSize     : FLASH���С
*             BSP_U32 *pu32FlashPageSize    : FLASHҳ��С
*
* �� �� ֵ  : BSP_ERROR:��ȡʧ��
*             BSP_OK:��ȡ�ɹ�
*
* ����˵��  : ��
*
*****************************************************************************/
BSP_S32 BSP_OM_GetFlashSpec(BSP_U32 *pu32FlashTotalSize, BSP_U32 *pu32FlashBlkSize, BSP_U32 *pu32FlashPageSize);
/*****************************************************************************
* �� �� ��  : BSP_OM_GetVerTime
*
* ��������  : ���verʱ��
*
* �������  : ��
*
* �������  : 
*
* �� �� ֵ  : 
*
* ����˵��  : ��
*
*****************************************************************************/
BSP_VOID BSP_OM_GetVerTime(BSP_S8* strVerTime);

/******************************************************************************
* Function     :   BSP_OM_MemRead
* 
* Description  :   
* 
* Input        :  
* 
* Output       :   ��
* 
* return       :   
******************************************************************************/
BSP_S32 BSP_OM_MemRead(BSP_U32 u32MemAddr, ENADDRTYPE enAddrType, BSP_U32 *pu32Value);
/******************************************************************************
* Function     :   BSP_OM_MemWrite
* 
* Description  :   
* 
* Input        :  
* 
* Output       :   ��
* 
* return       :   
******************************************************************************/
BSP_S32 BSP_OM_MemWrite(BSP_U32 u32MemAddr, ENADDRTYPE enAddrType, BSP_U32 u32Value);
/******************************************************************************
* Function     :   BSP_OM_MemNWrite
* 
* Description  :   
* 
* Input        :  
* 
* Output       :   ��
* 
* return       :   
******************************************************************************/
BSP_S32 BSP_OM_MemNWrite(BSP_U32 u32MemAddr, BSP_U32 u32Value, BSP_U32 u32Count);
/******************************************************************************
* Function     :   BSP_OM_MemPrint
* 
* Description  :   
* 
* Input        :  
* 
* Output       :   ��
* 
* return       :   
******************************************************************************/
BSP_S32 BSP_OM_MemPrint(BSP_U32 u32MemAddr,ENADDRTYPE enAddrType);
/******************************************************************************
* Function     :   BSP_OM_MemChk
* 
* Description  :   
* 
* Input        :  
* 
* Output       :   ��
* 
* return       :   
******************************************************************************/
BSP_S32 BSP_OM_MemChk(BSP_U32 u32StartAddr, BSP_U32 u32EndAddr, ENMEMCHKTYPE enChkType);
/******************************************************************************
* Function     :   BSP_OM_GetVersion
* 
* Description  :   
* 
* Input        :  
* 
* Output       :   ��
* 
* return       :   
******************************************************************************/
BSP_VOID BSP_OM_GetVersion(void);

/******************************************************************************
* Function     :   BSP_OM_GetFPGAVer
* 
* Description  :   
* 
* Input        :  
* 
* Output       :   ��
* 
* return       :   
******************************************************************************/
BSP_S32 BSP_OM_GetFPGAVer(void);

/******************************************************************************
* Function     :   BSP_OM_ShellLock
* 
* Description  :   
* 
* Input        :  
* 
* Output       :   ��
* 
* return       :   
******************************************************************************/
BSP_S32 BSP_OM_ShellLock(BSP_BOOL bRequest);

/*****************************************************************************
* �� �� ��  : BSP_OM_GetBoardType
*
* ��������  : ��ȡ��������
*
* �������  : ��
*
* �������  : ��
*
* �� �� ֵ  : BSP��������ö��
*
* ����˵��  : ��
*
*****************************************************************************/
BOARD_TYPE_E BSP_OM_GetBoardType(BSP_VOID);

/*****************************************************************************
* �� �� ��  : BSP_GetHostCore
*
* ��������  : ��ѯ��ǰCPU���Ӻ�����
*
* �������  : ��
*
* �������  : ��
*
* �� �� ֵ  : CPU���Ӻ�����
*
* ����˵��  : ��
*
*****************************************************************************/
BSP_CORE_TYPE_E BSP_GetHostCore(BSP_VOID);

/*****************************************************************************
* �� �� ��  : BSP_OM_GetChipType
*
* ��������  : ��ȡоƬ����
*
* �������  : BSP_VOID  
*
* �������  : ��
*
* �� �� ֵ  : оƬ����
*             PV500_CHIP:PV500оƬ
*             V7R1_CHIP: V7R1оƬ
*               
* ����˵��  : ��
*
*****************************************************************************/
BSP_CHIP_TYPE_E BSP_OM_GetChipType(BSP_VOID);

/*****************************************************************************
* �� �� ��  : BSP_OM_GetTcmAdr
*
* ��������  : ��ȡDSP�������ַ
*
* �������  : BSP_VOID  
* �������  : ��
*
* �� �� ֵ  : DSP�������ַ
*
* ����˵��  : ����ֵ0Ϊ�Ƿ�ֵ
*
*****************************************************************************/
BSP_U32 BSP_OM_GetTcmAdr(BSP_VOID);

/*****************************************************************************
* �� �� ��  : BSP_MspProcReg
*
* ��������  : DRV�ṩ��OM��ע�ắ��
*
* �������  : MSP_PROC_ID_E eFuncID, BSP_MspProc pFunc
* �������  : NA
*
* �� �� ֵ  : NA
*
* ����˵��  : ��ά�ɲ�ӿں���
*
*****************************************************************************/
extern void BSP_MspProcReg(MSP_PROC_ID_E eFuncID, BSP_MspProc pFunc);


#ifdef __VXWORKS__
/*****************************************************************************
Function   : DR_NV_Read
Description: Read NV from the file without Auth.
Input      : VOS_UINT16 usID -> NV ID
             VOS_VOID *pItem -> Save NV data buffer
             VOS_UINT32 ulLength -> buffer length
Return     : Ok or Err.
Other      :
*****************************************************************************/
unsigned int DR_NV_Read(unsigned short usID,void *pItem,unsigned int ulLength);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_OM_API_H__ */


