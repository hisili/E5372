#ifndef __OMDRV_H__
#define __OMDRV_H__

#include "BSP.h"
/*#define MNTN_DBG*/
#define OSAL_OK            (0)
#define OSAL_ERROR         (-1)

#define DRV_FUNCTION_TRACE  50

#define DRV_PRIM_SEND_CONF_REQ  0x9001
#define DRV_PRIM_SEND_CONF_RSP  0x9002

#define INVALID_PRIM_ID         0x0000

#define PRIM_SEND_PERMIT        0x0001
#define PRIM_SEND_NOT_PERMIT    0x0000

/****Driver模块ID号,共31个 *******/
/****请不要在中间插入moduleId*****/
/**要增加moduleId请在最后依次增加*/
typedef enum
{
    DRV_MODULE_ID_AMTS   = 0x8101,
    DRV_MODULE_ID_AUDIO ,
    DRV_MODULE_ID_BSP ,
    DRV_MODULE_ID_CAMERA ,
    DRV_MODULE_ID_DM  ,
    DRV_MODULE_ID_DMA ,
    DRV_MODULE_ID_DSPC,
    DRV_MODULE_ID_END ,
    DRV_MODULE_ID_FDI,
    DRV_MODULE_ID_FLASH ,
    DRV_MODULE_ID_FM,
    DRV_MODULE_ID_GPIO,
    DRV_MODULE_ID_HICONTRL,
    DRV_MODULE_ID_HKADC,
    DRV_MODULE_ID_I2C,
    DRV_MODULE_ID_KEYPAD,
    DRV_MODULE_ID_LCD,
    DRV_MODULE_ID_LOGGER,
    /*DRV_MODULE_ID_MNTN,*/
    DRV_MODULE_ID_PM,
    DRV_MODULE_ID_RTC,
    DRV_MODULE_ID_SCI,
    DRV_MODULE_ID_SDMMC,
    DRV_MODULE_ID_SPI,
    DRV_MODULE_ID_USB,
    DRV_MODULE_ID_WDT,
    DRV_MODULE_ID_NVIM,
    DRV_MODULE_ID_SYSCTRL,
    DRV_MODULE_ID_MNTN ,
    DRV_MODULE_ID_TIMER,
    DRV_MODULE_ID_VIC,
    DRV_MODULE_ID_YAFFS,
    DRV_MODULE_ID_SDIO,
    DRV_MODULE_ID_WIFIEND,
    DRV_MODULE_ID_WIFIMVL,
    DRV_MODULE_ID_OLED,
    DRV_MODULE_ID_ONOFF,
#if(FEATURE_ICC_DEBUG == FEATURE_ON)
    DRV_MODULE_ID_ICC=0x812E,
#endif	
}LOG_MODULE_ID_EN;


typedef unsigned int        UINT_32;
typedef unsigned short      UINT_16;
typedef unsigned char       UINT_8;

typedef  struct
{
    BSP_U8         ucModuleId;
    BSP_U8         ucReserved;
    BSP_U16        usLength;
    BSP_U8         aucFeedback[4];
} OM_RSP_PACKET_STRU;

typedef struct
{
    BSP_U8      ucFuncType;
    BSP_U8      ucReserve;
    BSP_U16     usLength;
    BSP_U16     usPrimId;
    BSP_U16     usReserve;
    BSP_U8      aucData[4];
}DRV_TRACE_REQ_STRU;

typedef struct
{
    BSP_U8      ucModuleId;
    BSP_U8      ucReserved;
    BSP_U16     usLength;
    BSP_U8      aucFeedback[4];
}OM_REQ_PACKET_STRU;

typedef struct
{
    BSP_U8  ucFuncType;
    BSP_U8  ucReserve;
    BSP_U16 usLength;
    BSP_U32 ulSn;
    BSP_U32 ulTimeStamp;
    BSP_U16 usPrimId;
    BSP_U16 usReserve;
    BSP_U32 ulModuleId;
    BSP_U8  aucData[4];
}DRV_TRACE_IND_STRU;

typedef struct
{
    BSP_U16 usVal;
    BSP_U16 usPrimId;
}DRV_TRACE_PERMISSION;

typedef BSP_U32 OM_RSP_FUNC(OM_RSP_PACKET_STRU *pRspPacket, BSP_U16 usLength);
typedef void    OM_REQUEST_PROCEDURE (OM_REQ_PACKET_STRU *pReqPacket,OM_RSP_FUNC  *pRspFuncPtr);
typedef BSP_U32 OM_RegRqsProc( BSP_U8  ucModuleId, OM_REQUEST_PROCEDURE *pReqPro);
typedef  void OM_AddSNTimeReg (BSP_U32 *pulSN, BSP_U32 *pulTimeStamp);
typedef BSP_U32 OM_PrintWithModule(BSP_U32 ulModuleId,BSP_U32 ulLevel, BSP_CHAR *pcformat, ... );
typedef BSP_U32 OM_Print(BSP_CHAR *pcformat, ... );
typedef BSP_U32 OM_PrintGetModuleIdLev(BSP_U32 ulModuleId);
typedef  unsigned long OM_nv_read_fun( unsigned short usID, void *pItem, unsigned int ulLength );
typedef  unsigned long OM_nv_write_fun( unsigned short usID, void *pItem, unsigned int ulLength );
typedef struct tagMSP_PROC_S
{
    OM_RegRqsProc*      OM_RegisterRequestProcedure;
	OM_AddSNTimeReg*    OM_AddSNTime;
    OM_PrintWithModule* OM_PrintfWithModule;
    OM_Print*           OM_Printf;
    OM_PrintGetModuleIdLev* OM_PrintfGetModuleIdLev;
    OM_nv_read_fun* OM_read_nv;
    OM_nv_write_fun* OM_write_nv;
}MPS_PROC_S;

extern MPS_PROC_S g_stMspRegFunc;
void BSP_MNTN_OmDrvTraceReqEntry(OM_REQ_PACKET_STRU *pReqPacket,  OM_RSP_FUNC *pRsqFuncPtr);
int  BSP_MNTN_OmDrvTraceSend(BSP_U16 usPrimId, BSP_U32 ulModuleId, BSP_U8 *buffer, BSP_U32 ulLength);
int  BSP_MNTN_DrvPrimCanSendOrNot(BSP_U16 usPrimId);

#endif
