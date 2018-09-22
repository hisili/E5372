/*************************************************************************
*   版权所有(C) 1987-2004, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_IFC.h
*
*   作    者 :  liumengcun
*
*   描    述 :  IFC对外提供头文件
*
*   修改记录 :  2011年1月11日  v1.00  liumengcun  创建
*
*************************************************************************/

#ifndef _BSP_IFC_H_
#define _BSP_IFC_H_

#ifdef __cplusplus
extern "C" 
{
#endif

#include "BSP.h"

/* 错误码*/
/*专用错误码*/
#define BSP_ERR_IFC_SEND_FAIL		BSP_DEF_ERR(BSP_MODU_IFC,(BSP_ERR_SPECIAL + 1)) /*0x10120001*/
#define BSP_ERR_IFC_SEND_TIMEOUT	BSP_DEF_ERR(BSP_MODU_IFC,(BSP_ERR_SPECIAL + 2)) /*0x10120002*/
#define BSP_ERR_IFC_RCV_FAIL		BSP_DEF_ERR(BSP_MODU_IFC,(BSP_ERR_SPECIAL + 3)) /*0x10120003*/

#define IFC_ICC_CHAN_IN_FIFOSIZE    4096
#define IFC_ICC_CHAN_OUT_FIFOSIZE   4096
/*IFC发送最大长度*/
#define BSP_IFC_SEND_MAX_LEN  IFC_ICC_CHAN_IN_FIFOSIZE

BSP_S32 IFC_Init(void);


#ifdef __cplusplus
}
#endif


#endif //end of "_BSP_IFC_H_"



