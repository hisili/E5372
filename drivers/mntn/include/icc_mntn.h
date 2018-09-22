/*************************************************************************
*   ��Ȩ����(C) 1987-2011, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  icc_mntn.h
*
*   ��    �� :  zhaikuixiu
*
*   ��    �� :  ���ļ�����Ϊ"icc_mntn.h"
*
*   �޸ļ�¼ :  2011��12��07��  v1.00  zhaikuixiu����
*************************************************************************/
#include "omDrv.h"/*·��һ��Ҫע��*/
//#define SEM_FULL                   (1)
//#define DRV_MNTN_USB_INFO_MAXSIZE  (64)
#define DRV_MNTN_HEADER_FLAG       (0xAAAA)/*�Ƿ���Ҫ���������Ҫ������usb_mntn.h��ɾ��*/
#if(0)
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
#endif

#define ICC_MNTN_ERRLOG_SIZE   (sizeof(DRV_TRACE_IND_STRU)  \
                             + sizeof(icc_mntn_info_header) - 4)

typedef enum
{
    DRV_PRIM_ICC_REQ= 0X9501,
    DRV_PRIM_ICC_CNF= 0X9502,
    DRV_PRIM_ICC_ERRLOG= 0X9503,

}ICC_PRIMID;

typedef struct
{
    BSP_U32 header_flag;
    BSP_U32 u32ChanID;
    BSP_S32 s32Errlog;
    BSP_U16 module_id;
    BSP_U16 prim_id;
    BSP_S32 s32Datesize;
    BSP_U32 time_stamp;
}icc_mntn_info_header;


typedef struct
{
    BSP_U32			u32ChanID;
    BSP_S32                      s32Errlog;
    BSP_S32                      s32Datesize;
}icc_mntn_info_t;


//extern BSP_U32 g_usb_mntn_func_type_on;
extern void    BSP_MNTN_IccErrlogInit(void);
extern void    BSP_MNTN_IccErrlogUnInit(void);
extern void    BSP_ICC_Errlog_Save( BSP_U16  module_id,
                                     BSP_U16  prim_id,
                                     void     *buffer);
                                                                  

