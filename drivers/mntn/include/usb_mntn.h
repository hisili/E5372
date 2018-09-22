/*************************************************************************
*   ��Ȩ����(C) 1987-2011, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  usb_mntn.h
*
*   ��    �� :  yudongbin
*
*   ��    �� :  ���ļ�����Ϊ"usb_mntn.h"
*
*   �޸ļ�¼ :  2011��9��19��  v1.00  yudongbin����
*************************************************************************/
#ifndef __USB_MNTN_H__
#define __USB_MNTN_H__

#include "omDrv.h"/*·��һ��Ҫע��*/
#define SEM_FULL                   (1)
#define DRV_MNTN_USB_INFO_MAXSIZE  (64)
#define DRV_MNTN_HEADER_FLAG       (0xAAAA)

#define DRV_MNTN_USB_CMD_IS_IN     1
#define DRV_MNTN_USB_CMD_IS_OUT    0

#define DRV_MNTN_USB_REG           (1 << 0)
#define DRV_MNTN_USB_AT            (1 << 1)
#define DRV_MNTN_USB_CTRL          (1 << 2)
#define DRV_MNTN_USB_INTR          (1 << 3)
#define DRV_MNTN_USB_PFS           (1 << 4)
#define DRV_MNTN_USB_ERRLOG        (1 << 5)
#define DRV_MNTN_USB_MNTN          (1 << 6)
#define DRV_MNTN_USB_SCSI          (1 << 7)
#define DRV_MNTN_USB_NCM           (1 << 8)
#define DRV_MNTN_USB_MODEM         (1 << 9)
#define DRV_MNTN_USB_INT           (1 << 10)
#define DRV_MNTN_USB_RNDIS         (1 << 11)

#define PRIM_IS_ON(x)   ((0 == BSP_MNTN_DrvPrimCanSendOrNot(x)) ? 0 : 1)

#define DRV_MNTN_BUF_SIZE   (sizeof(DRV_TRACE_IND_STRU)  \
                             + sizeof(drv_mntn_info_header) - 4)
                                                    
typedef enum
{
    DRV_PRIM_USB_REG             = 0X9003,
    DRV_PRIM_USB_AT              = 0X9004,
    DRV_PRIM_USB_CTRL            = 0X9005,
    DRV_PRIM_USB_INTR            = 0X9006,
    DRV_PRIM_USB_PFS             = 0X9007,
    /*��A����û�д�����Ϣ���Բ���ֲ,��������*/
    DRV_PRIM_USB_ERRLOG          = 0X9008, 
    /*��A����û�д�����Ϣ���Բ���ֲ,��������*/
    DRV_PRIM_USB_MNTN            = 0X9009,
    DRV_PRIM_USB_SCSI            = 0X900A,
    DRV_PRIM_USB_NCM             = 0X900B,
    DRV_PRIM_USB_MODEM           = 0X900C,
    DRV_PRIM_USB_INT             = 0X900D,
    DRV_PRIM_USB_RNDIS           = 0X900E,
}PRIMID;

typedef struct
{
    BSP_U16 header_flag;
    BSP_U16 record_index;
    BSP_U16 time_stamp;
    BSP_U16 module_id;
    BSP_U16 prim_id;
    BSP_U16 length;
    BSP_U16 attribute;
    BSP_U8  data[DRV_MNTN_USB_INFO_MAXSIZE];
}drv_mntn_info_header;

typedef struct
{
    BSP_U16 length;
    BSP_U16 attribute;
    void    *data;
}usb_mntn_info_t;
//PMU ��ά�ɲ�
typedef struct
{
	UINT32	ErrNo;	//Error ID for recording
	UINT32	length;	//the following data length of data[0];
	UINT8	data[0];	//data body, ��ErrNo���
}IFC_ERRLOG_USBSTATE_STRU;
//PMU ��ά�ɲ�
extern BSP_U32 g_usb_mntn_func_type_on;
extern void    BSP_MNTN_DrvInfoInit(void);
extern void    BSP_MNTN_DrvInfoUnInit(void);
extern void    BSP_MNTN_DrvInfoSave( BSP_U16  module_id,
                                     BSP_U16  prim_id,
                                     void     *buffer,
                                     void     *reserved);
                                                                  
#endif
