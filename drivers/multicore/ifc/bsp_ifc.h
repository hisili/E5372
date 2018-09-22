/*************************************************************************
*   ��Ȩ����(C) 1987-2004, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_IFC.h
*
*   ��    �� :  liumengcun
*
*   ��    �� :  IFC�����ṩͷ�ļ�
*
*   �޸ļ�¼ :  2011��1��11��  v1.00  liumengcun  ����
*
*************************************************************************/

#ifndef _BSP_IFC_H_
#define _BSP_IFC_H_

#ifdef __cplusplus
extern "C" 
{
#endif

#include "BSP.h"

/* ������*/
/*ר�ô�����*/
#define BSP_ERR_IFC_SEND_FAIL		BSP_DEF_ERR(BSP_MODU_IFC,(BSP_ERR_SPECIAL + 1)) /*0x10120001*/
#define BSP_ERR_IFC_SEND_TIMEOUT	BSP_DEF_ERR(BSP_MODU_IFC,(BSP_ERR_SPECIAL + 2)) /*0x10120002*/
#define BSP_ERR_IFC_RCV_FAIL		BSP_DEF_ERR(BSP_MODU_IFC,(BSP_ERR_SPECIAL + 3)) /*0x10120003*/

#define IFC_ICC_CHAN_IN_FIFOSIZE    4096
#define IFC_ICC_CHAN_OUT_FIFOSIZE   4096
/*IFC������󳤶�*/
#define BSP_IFC_SEND_MAX_LEN  IFC_ICC_CHAN_IN_FIFOSIZE

BSP_S32 IFC_Init(void);


#ifdef __cplusplus
}
#endif


#endif //end of "_BSP_IFC_H_"



