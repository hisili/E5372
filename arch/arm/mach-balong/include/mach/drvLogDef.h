/******************************************************************
* Copyright (C), 2005-2012, HISILICON Tech. Co., Ltd.             *
*                                                                 *
* File name: drvLogdef.h                                             *
*                                                                 *
* Description:                                                    *
*     driver log related functions                                *
*                                                                 *
*                                                                 *
* Version: v0.1                                                   *
*                                                                 *
* Function List:                                                  *
*   1.  Volume Control functions list                             *
*   2.  Setting functions                                         *
*        setDrvLogFunc                                            *
*                                                                 *
* Date:    2012-01-06                                             *
*                                                                 *
* History:                                                        *
*                                                                 *
*******************************************************************/

#ifndef __DRVLOGF_H__
#define __DRVLOGF_H__


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define     LOG_MIN_FILE_ID_DRV         0x0800


/* 需要分配skb的DRV源文件ID号，共192个 */
typedef enum
{
        /* 0x0800 0d2048 */ DRV_FILE_ID_BSP_USB_ECM_APP_C = LOG_MIN_FILE_ID_DRV, 
        /* 0x0801 0d2049 */ DRV_FILE_ID_BSP_USB_ACM_SIO_C,
        /* 0x0802 0d2050 */ DRV_FILE_ID_NET_DEV_C,
        /* 0x0803 0d2051 */ DRV_FILE_ID_BSP_USB_NCM_APP_C,
}DRV_SKB_ALLOC_FILE_ID_DEFINE_ENUM;


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __DRVLOGF_H__ */

