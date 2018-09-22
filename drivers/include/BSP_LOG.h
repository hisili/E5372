/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_LOG.h
*
*   作    者 :  z67193
*
*   描    述 :  BSP打印功能定义头文件
*
*   修改记录 :  2012年04月14日  v1.00  z67193  创建
*************************************************************************/

#ifndef	__BSP_LOG_H__
#define __BSP_LOG_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */

/* 模块定义 */
typedef enum tagBSP_MODULE_E
{
    BSP_MODU_NANDF = 0x0, /* 0x0 */
    BSP_MODU_TIMER, /* 0x1 */
    BSP_MODU_VIC,   /* 0x2 */
    BSP_MODU_UART,  /* 0x3 */
    BSP_MODU_YAFFS, /* 0x4 */
    BSP_MODU_ETH,   /* 0x5 */
    BSP_MODU_DMAC,  /* 0x6 */
    BSP_MODU_SPI,   /* 0x7 */
    BSP_MODU_GPIO,  /* 0x8 */
    BSP_MODU_WDT,   /* 0x9 */
    BSP_MODU_SCI,   /* 0xA */
    BSP_MODU_USB,   /* 0xB */
    BSP_MODU_DRX,   /* 0xC */
    BSP_MODU_SDIO,  /* 0xD */
    BSP_MODU_FTP,   /* 0xE */
    BSP_MODU_GMAC,	  /* 0xF */
    BSP_MODU_SEEKER,	/* 0x10 */
    BSP_MODU_UPGRADE,	/* 0x11 */
    BSP_MODU_AXI,	/* 0x12 */
    BSP_MODU_NET,	/* 0x13 */
    BSP_MODU_USB_ACM,	/* 0x14 */
    BSP_MODU_USB_NCM,	/* 0x15 */
    BSP_MODU_AXI_MONITOR,
    BSP_MODU_IFC,
    BSP_MODU_ICC,
    BSP_MODU_IPF,
    BSP_MODU_ACC,	/* 0x1A */
    BSP_MODU_BBP,
    BSP_MODU_MEM,
    BSP_MODU_UDI,
    BSP_MODU_DLOAD, 
    BSP_MODU_SOCP,
    BSP_MODU_IFCP,  /* 0x20 */
    BSP_MODU_PWRCTRL,
    BSP_MODU_SERIAL_UPGRADE,
    BSP_MODU_SYNC,
    BSP_MODU_IPC,
    BSP_MODU_MNTN,
    BSP_MODU_SSI,
    BSP_MODU_LSW,
    BSP_MODU_EMI,
    BSP_MODU_LCD,
    BSP_MODU_SECURITY,
    BSP_MODU_PMU,
    BSP_MODU_HIFI,
    BSP_MODU_LED,
    BSP_MODU_HKADC,
    BSP_MODU_SLAVE,
    BSP_MODU_CHGC,
	BSP_MODU_USB_INOUT, 
	BSP_MODU_ONLINE,
	BSP_MODU_DFS,
    BSP_MODU_ALL,   /* 代表所有的模块 */
    BSP_MODU_MAX    /* 边界值 */
} BSP_MODULE_E;

/* 打印级别定义 */
typedef enum tagBSP_LOG_LEVEL_E
{
    BSP_LOG_LEVEL_DEBUG = 0,  /* 0x0:debug-level                                  */
    BSP_LOG_LEVEL_INFO,      /* 0x1:informational                                */
    BSP_LOG_LEVEL_NOTICE,     /* 0x2:normal but significant condition             */
    BSP_LOG_LEVEL_WARNING,    /* 0x3:warning conditions                           */
    BSP_LOG_LEVEL_ERROR,      /* 0x4:error conditions                             */
    BSP_LOG_LEVEL_CRIT,       /* 0x5:critical conditions                          */
    BSP_LOG_LEVEL_ALERT,      /* 0x6:action must be taken immediately             */
    BSP_LOG_LEVEL_FATAL,      /* 0x7:just for compatibility with previous version */
    BSP_LOG_LEVEL_MAX         /* 边界值 */
} BSP_LOG_LEVEL_E;

BSP_VOID BSP_ClearLogModule( BSP_MODULE_E enModule );

/* 通用错误码，从0x1001开始 */
#define BSP_ERR_COMMON (0x1000)
#define BSP_ERR_MODULE_NOT_INITED (BSP_ERR_COMMON + 1)
#define BSP_ERR_NULL_PTR (BSP_ERR_COMMON + 2)
#define BSP_ERR_INVALID_PARA (BSP_ERR_COMMON + 3)
#define BSP_ERR_RETRY_TIMEOUT (BSP_ERR_COMMON + 4)
#define BSP_ERR_BUF_ALLOC_FAILED (BSP_ERR_COMMON + 5)
#define BSP_ERR_BUF_FREE_FAILED (BSP_ERR_COMMON + 6)

/* 各模块专用错误码，从0x1开始  */
#define BSP_ERR_SPECIAL (0x0)




BSP_VOID BSP_SetLogModule( BSP_MODULE_E enModule );
BSP_U32  BSP_GetLogModule(BSP_VOID);
BSP_VOID BSP_SetLogLevel(BSP_LOG_LEVEL_E enLogLevel);
BSP_U32  BSP_GetLogLevel(BSP_VOID);
BSP_VOID BSP_SetPrintPos( BSP_BOOL bPrintPos);
BSP_BOOL BSP_GetPrintPos(BSP_VOID);

BSP_S32  BSP_ErrnoGet (BSP_VOID);
BSP_S32  BSP_TaskErrnoGet (BSP_S32 taskId);
BSP_S32  BSP_ErrnoSet (BSP_S32 s32ErrorValue);
BSP_S32  BSP_TaskErrnoSet (BSP_S32 s32TaskId, BSP_S32 s32ErrorValue);
BSP_VOID BSPLogHelp(BSP_VOID);


#undef __BSP_DEBUG__
#ifdef __BSP_DEBUG__
#include <logLib.h>

#define BSP_ASSERT(expr) \
{\
    if (!(expr)) { \
        logMsg("ASSERT failed at:\n  >File name: %s\n  >Function : %s\n  >Line No. : %d\n  >Condition: %s\n", \
               (BSP_S32)__FILE__, (BSP_S32)__FUNCTION__, (BSP_S32)__LINE__, (BSP_S32) # expr, (BSP_S32)5, (BSP_S32)6); \
    return BSP_ERROR;\
    } \
}

#define BSP_ASSERT_HALT (expr) \
{\
    if (!(expr)) { \
        logMsg("ASSERT failed at:\n  >File name: %s\n  >Function : %s\n  >Line No. : %d\n  >Condition: %s\n", \
               (BSP_S32)__FILE__, (BSP_S32)__FUNCTION__, (BSP_S32)__LINE__, (BSP_S32) # expr, (BSP_S32)5, (BSP_S32)6); \
        for (;;) ;\
    } \
}
#else
#define BSP_ASSERT(expr)
#define BSP_ASSERT_HALT(expr)
#endif


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_LOG_H__ */

