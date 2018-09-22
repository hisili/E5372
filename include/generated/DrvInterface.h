/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  DrvInterface.h
*
*   作    者 :  yangzhi
*
*   描    述 :  本文件命名为"DrvInterface.h", 给出V7R1底软和协议栈之间的API接口统计
*
*   修改记录 :  2011年1月18日  v1.00  yangzhi创建
*************************************************************************/

#ifndef __DRV_INTERFACE_H__
#define __DRV_INTERFACE_H__

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM) || defined(__VXWORKS__)

#include "FeatureConfigDRV.h"
#include "FeatureConfig.h"

#ifndef _lint
#include "MemoryMap.h"
#endif

#else

#ifndef _lint
#include <asm/io.h>
#include <mach/hardware.h>
#include <generated/FeatureConfig.h>
#include <mach/drvLogDef.h>
#include <linux/scatterlist.h>
#include <generated/MemoryMap.h>
#endif
#endif


/*此文件默认是4字节对齐，关闭下述两个lint告警*/
/*lint -e958 -e959*/
#pragma pack(4)

/*************************GLOBAL BEGIN*****************************/
/* 基本数据类型定义 */
#ifndef _WIN32_COMPILE
typedef signed long long    BSP_S64;
#else
typedef double              BSP_S64;
#endif
typedef signed int          BSP_S32;
typedef signed short        BSP_S16;
typedef signed char         BSP_S8;
typedef char                BSP_CHAR;

#ifndef _WIN32_COMPILE
typedef unsigned long long  BSP_U64;
#else
typedef double              BSP_U64;
#endif
typedef unsigned int        BSP_U32;
typedef unsigned short      BSP_U16;
typedef unsigned char       BSP_U8;

typedef int                 BSP_BOOL;
typedef void                BSP_VOID;
typedef int                 BSP_STATUS;

#ifndef _WIN32_COMPILE
typedef signed long long*   BSP_PS64;
#else
typedef double*             BSP_PS64;
#endif
typedef signed int*         BSP_PS32;
typedef signed short*       BSP_PS16;
typedef signed char*        BSP_PS8;

#ifndef _WIN32_COMPILE
typedef unsigned long long* BSP_PU64;
#else
typedef double*             BSP_PU64;
#endif
typedef unsigned int*       BSP_PU32;
typedef unsigned short*     BSP_PU16;
typedef unsigned char*      BSP_PU8;

typedef unsigned char       UINT8;
typedef unsigned int        UINT32;
typedef int*                BSP_PBOOL;
typedef void*               BSP_PVOID;
typedef int*                BSP_PSTATUS;

typedef void                VOID;
typedef BSP_S32             STATUS;
typedef BSP_S32             UDI_HANDLE;

#ifndef BSP_CONST
#define BSP_CONST           const
#endif

#ifndef OK
#define OK                  (0)
#endif

#ifndef ERROR
#define ERROR               (-1)
#endif

#ifndef TRUE
#define TRUE                (1)
#endif

#ifndef FALSE
#define FALSE               (0)
#endif

#ifndef BSP_OK
#define BSP_OK              (0)
#endif

#ifndef BSP_ERROR
#define BSP_ERROR           (-1)
#endif

#ifndef BSP_TRUE
#define BSP_TRUE            (1)
#endif

#ifndef BSP_FALSE
#define BSP_FALSE           (0)
#endif

#ifndef BSP_NULL
#define BSP_NULL            (void*)0
#endif

#ifndef _LINUX_KERNEL_STAT_H  /* FUNCPTR defined in kernel_stat.h */
#ifndef FUNCPTR_IS_DEFINED
typedef BSP_S32 (*FUNCPTR)(void);
#define FUNCPTR_IS_DEFINED  1
#endif
#endif

typedef BSP_S32 (*FUNCPTR_1)(int);
typedef int (*PWRCTRLFUNCPTRVOID)(void);

#ifndef PWRCTRLFUNCPTR_D
typedef unsigned int (*PWRCTRLFUNCPTR)(unsigned int arg);     /* ptr to function returning int */
#define PWRCTRLFUNCPTR_D
#endif


#ifndef INLINE
#define INLINE              __inline__
#endif

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
typedef BSP_VOID (*VOIDFUNCPTR)();
#else
typedef BSP_VOID (*VOIDFUNCPTR)(BSP_U32);
#define SEM_FULL            (1)
#define SEM_EMPTY           (0)
#define LOCAL                   static
#define IVEC_TO_INUM(intVec)    ((int)(intVec))
#endif


/* 错误码宏定义 */
#define BSP_ERR_MODULE_OFFSET (0x1000)    /* 防止和系统的错误码重叠 */
#define BSP_DEF_ERR( mod, errid) \
    ((((BSP_U32) mod + BSP_ERR_MODULE_OFFSET) << 16) | (errid))

#define BSP_REG(base, reg) (*(volatile BSP_U32 *)((BSP_U32)base + (reg)))

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM) || defined(__VXWORKS__)
#define BSP_REG_READ(base, reg, result) \
    ((result) = BSP_REG(base, reg))

#define BSP_REG_WRITE(base, reg, data) \
    (BSP_REG(base, reg) = (data))

#else
#define BSP_REG_READ(base, reg, resule) \
    (resule = readl(base + reg))

#define BSP_REG_WRITE(base, reg, data) \
    (writel(data, (base + reg)))
#endif

/*****************************************************************************
  1 GU侧驱动对外接口，GUPorting项目添加.
*****************************************************************************/

/*****************************************************************************
  2 宏定义
*****************************************************************************/

#define DRV_OK                          (0)
#define DRV_ERROR                       (-1)
#define DRV_INTERFACE_RSLT_OK           (0)
#define BSP_BBP_OK   0x1
#define BSP_BBP_ERROR    0xffff

/* 基本数据类型定义 */
typedef void (*PVOIDFUNC)(void);

typedef int  (*pFUNCPTR)(void);

typedef unsigned long (*pFUNCPTR2)( unsigned long ulPara1, unsigned long ulPara2);

typedef unsigned int tagUDI_DEVICE_ID_UINT32;

/*需要移到别的头文件中 start*/
/*升级Flash信息*/
typedef struct
{
    BSP_U32 ulblockCount;    /*Block个数*/
    BSP_U32 ulpageSize;    /*page页大小*/
    BSP_U32 ulpgCntPerBlk;    /*一个Block中的page个数*/
}DLOAD_FLASH_STRU;
/*需要移到别的头文件中 end*/

/* 上行线路收包函数指针 */
typedef BSP_VOID (*UpLinkRxFunc)(BSP_U8 *buf, BSP_U32 len);

/* 包封装释放函数指针 */
typedef BSP_VOID (*FreePktEncap)(BSP_VOID *PktEncap);

/* GMAC调用者枚举 */
typedef enum tagGMAC_OWNER_E
{
    GMAC_OWNER_VXWORKS = 0, /* Vxworks网络协议栈 */
    GMAC_OWNER_PS,          /* LTE网络协议栈 */
    GMAC_OWNER_MSP,         /* MSP */
    GMAC_OWNER_MAX          /* 边界值 */
}GMAC_OWNER_E;

typedef enum tagWDT_TIMEOUT_E
{
    WDT_TIMEOUT_1   = 0,        /*0xFFFF000/WDT_CLK_FREQ,  about 3657ms*/      /*WDT_CLK_FREQ = ARM_FREQ/6 = 70M*/
    WDT_TIMEOUT_2,              /*0x1FFFE000/WDT_CLK_FREQ, about 7314ms*/
    WDT_TIMEOUT_3,              /*0x3FFFC000/WDT_CLK_FREQ, about 14628ms*/
    WDT_TIMEOUT_4,              /*0x7FFF8000/WDT_CLK_FREQ, about 29257ms*/
    WDT_TIMEOUT_BUTT
}WDT_TIMEOUT_E;

BSP_U8* BSP_GetMacAddr();
extern int ddmPhaseScoreBoot(const char * phaseName, unsigned int param);

/*************************GLOBAL END****************************/

/*************************VERSION START*****************************/
/* 硬件产品信息适配 */
#define HW_VER_INVALID              (BSP_U16)0xFFFF

/* Porting板和P500板兼容 */
#define HW_VER_PRODUCT_PORTING      (BSP_U16)0xFFFE
#define HW_VER_PRODUCT_PV500        (BSP_U16)0xFFFD

/* 测试板 */
#define HW_VER_PRODUCT_UDP          (BSP_U16)0xFFFC

/* Stick */
#define HW_VER_PRODUCT_E392S_U      (BSP_U16)HW_VER_INVALID
#define HW_VER_PRODUCT_E3276S_150   (BSP_U16)0x0201

/* E5 */
#define HW_VER_PRODUCT_E5_SBM       (BSP_U16)0x0001

/*E5776*/
#define HW_VER_PRODUCT_E5776_EM     (BSP_U16)0x0000

#define HW_VER_PRODUCT_E5_CMCC   	(BSP_U16)0x0003

/*E5371-DCM*/
#define HW_VER_PRODUCT_E5371_DCM     (BSP_U16)0x0100

/*E5372-32*/
#define HW_VER_PRODUCT_E5372_32      (BSP_U16)0x0103

/*E5375*/
#define HW_VER_PRODUCT_E5375          (BSP_U16)0x0104

/*E5375 第二版*/
#define HW_VER_PRODUCT_E5375_SEC      (BSP_U16)0x0106

#define HW_VER_PRODUCT_E5775S_925     (BSP_U16)0x0107
#define HW_VER_PRODUCT_E5372_601      (BSP_U16)0x0109
#define HW_VER_PRODUCT_E5372TS_32     (BSP_U16)0x0903

#define HW_VER_PRODUCT_E5372S_22     (BSP_U16)0x0900
#define HW_VER_PRODUCT_R215      (BSP_U16)0x0901

/*BEGIN DST2012092001529 liangshukun 20121025 ADDED*/
#define HW_VER_PRODUCT_E5_CMCC_CY   (BSP_U16)0x0007
/*END DST2012092001529 liangshukun 20121025 ADDED*/

/*BEGIN DST2012092001529 liangshukun 20121025 ADDED*/
/* 无LNA的板，PCB标识 3~5显示为A~C */
#define HW_CMCC_NON_LNA_PCB_VER_START_VALUE     3

/* CMCC 贴错BOM的硬件标识*/
#define HW_VER_PRODUCT_E5_CMCC_BOM_ERR  (BSP_U16)0x0603
/*BEGIN DST2012092001529 liangshukun 20121025 ADDED*/

/*Epad, S10 101u */
#define HW_VER_PRODUCT_S10_101U     (BSP_U16)0x0401
#define HW_VER_PRODUCT_S10_101U_1   HW_VER_PRODUCT_S10_101U
#define HW_VER_PRODUCT_S10_101U_2   (BSP_U16)0x0402
#define HW_VER_PRODUCT_S10_101U_3   (BSP_U16)0x0403
#define HW_VER_PRODUCT_S10_101U_4   (BSP_U16)0x0404

/*Epad, S10 102u */
#define HW_VER_PRODUCT_S10_102U      (BSP_U16)0x0406
#define HW_VER_PRODUCT_S10_102U_1    HW_VER_PRODUCT_S10_102U
#define HW_VER_PRODUCT_S10_102U_2    (BSP_U16)0x0407
#define HW_VER_PRODUCT_S10_102U_3    (BSP_U16)0x0408
#define HW_VER_PRODUCT_S10_102U_4    (BSP_U16)0x0409

/*Epad, S10 LTE */
#define HW_VER_PRODUCT_S10_101L     (BSP_U16)0x0501
#define HW_VER_PRODUCT_S10_101L_1   HW_VER_PRODUCT_S10_101L
#define HW_VER_PRODUCT_S10_101L_2   (BSP_U16)0x0502
#define HW_VER_PRODUCT_S10_101L_3   (BSP_U16)0x0503
#define HW_VER_PRODUCT_S10_101L_4   (BSP_U16)0x0504

/*Epad, S10 Reserved HW ID*/
#define HW_VER_PRODUCT_S10_RESERVED_1   (BSP_U16)0x0506
#define HW_VER_PRODUCT_S10_RESERVED_2   (BSP_U16)0x0507
#define HW_VER_PRODUCT_S10_RESERVED_3   (BSP_U16)0x0508
#define HW_VER_PRODUCT_S10_RESERVED_4   (BSP_U16)0x0509

/*Epad, S10 Modem */
#define HW_VER_PRODUCT_S10_MODEM     (BSP_U16)0x0000
#define HW_VER_PRODUCT_S10_MODEM_1   (BSP_U16)0x0005
#define HW_VER_PRODUCT_S10_MODEM_2   HW_VER_PRODUCT_S10_MODEM
#define HW_VER_PRODUCT_S10_MODEM_3   (BSP_U16)0x0500

/*Epad, S10 Temp */
#define HW_VER_PRODUCT_S10_TEMP     (BSP_U16)0x0005

/*****************************************************************************
* 函 数 名  : BSP_HwGetVerMain
* 功能描述  : 获取产品版本号
* 输入参数  : 无
* 输出参数  : 产品版本号
* 返 回 值  : 0：正确，非0: 失败
* 修改记录  : 2011-3-30 wuzechun creat
*****************************************************************************/
BSP_U16 BSP_HwGetVerMain( BSP_VOID );

/*****************************************************************************
* 函 数 名  : BSP_GetProductName
* 功能描述  : 获取产品名称
* 输入参数  : char* pProductName,字符串指针，保证不小于32字节
*             BSP_U32 ulLength,缓冲区长度
* 输出参数  : 无
* 返 回 值  : 0：正确，非0: 失败
* 修改记录  : 2011-3-30 wuzechun creat
*****************************************************************************/
BSP_S32 BSP_GetProductName (char * pProductName, unsigned int ulLength);

/*************************VERSION END  *****************************/


/*************************MMI START*****************************/

/***************************************************************
*  函数名        :   BSP_MMI_TestResultSet
*  函数功能      :   本接口是将MMI的测试标志写入NV
                     若ulFlag == 1 则写入 “ST P”
                     若ulFlag == 0 则写入 “ST F”
*  输入参数      :   BSP_U32 ulFlag
*  输出参数      :   无
*  返回值        :   成功(0)  失败(-1)
***************************************************************/
int BSP_MMI_TestResultSet(unsigned int ulFlag);
#define DVR_MMI_TEST_RESULT_SET(ulFlag) BSP_MMI_TestResultSet(ulFlag)

/***************************************************************
*  函数名        :   BSP_MMI_TestResultGet
*  函数功能      :   本接口比较MMI NV中的是否是“ST P”
                                  是返回1   不是返回0
*  输入参数      :   无
*  输出参数      :   无
*  返回值        :   成功(1)  失败(0)
***************************************************************/
unsigned int BSP_MMI_TestResultGet(void);
#define DVR_MMI_TEST_RESULT_GET() BSP_MMI_TestResultGet()

/*************************MMI END*****************************/

/*************************OLED START**************************/

/*****************************************************************************
*  Function:  DRV_OLED_CLEAR_WHOLE_SCREEN
*  Description: oled clear *
*  Called By:AP
*  Table Accessed:
*  Table Updated:
*  Input:
*         N/A
*  Output:
*         N/A
*  Return:
*         N/A
*****************************************************************************/
extern BSP_VOID DRV_OLED_CLEAR_WHOLE_SCREEN(BSP_VOID);

/*****************************************************************************
*  Function:  DRV_OLED_UPDATE_STATE_DISPLAY
*  Description: oled display right or not right  *
*  Called By:AP
*  Table Accessed:
*  Table Updated:
*  Input:
*         N/A
*  Output:
*         N/A
*  Return:
*         N/A
*****************************************************************************/
extern  void DRV_OLED_UPDATE_STATE_DISPLAY(int UpdateStatus);

/*****************************************************************************
 函 数 名  : DRV_OLED_UPDATE_DISPLAY
 功能描述  : SD卡烧片版本升级，OLED显示
 输入参数  : BOOL UpdateStatus
 输出参数  : 无。
 返回值：   无
 其它:  升级成功，NV备份完成后调用该函数打勾
        升级失败后调用该函数打叉
*****************************************************************************/
extern void DRV_OLED_UPDATE_DISPLAY(int UpdateStatus);

/*****************************************************************************
 函 数 名  : DRV_OLED_STRING_DISPLAY
 功能描述  : oled字符串显示函数
 输入参数  :
 *           要显示字符串的起始坐标
 *           要显示的字符串
 输出参数  : none
 返 回 值  : void
*****************************************************************************/
extern BSP_VOID DRV_OLED_STRING_DISPLAY(UINT8 ucX, UINT8 ucY, UINT8 *pucStr);

/*****************************************************************************
 函 数 名  : DRV_OLED_POWER_OFF
 功能描述  : oled下电函数
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/
extern BSP_VOID DRV_OLED_POWER_OFF(BSP_VOID);

/*************************OLED END******************************/

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)

#else
/*************************LCD START*****************************/

typedef enum
{
    LCD_LIGHT = 3,
    LCD_SLEEP,
    LCD_DIM,
    LCD_RESET,
    LCD_UPDATE,
    LCD_CONTRAST,
    LCD_BRIGHTNESS,
    LCD_POWER_ON,
    LCD_POWER_OFF,
    LCD_DEL_TIMER,/*delete the power on animation timer*/
    LCD_SYS_POWER_DOWN,
    LCD_INIT_IMAGE,
    LCD_POWER_OFF_CHARGING_START,/*restart power off charging animation*/
    LCD_POWER_OFF_CHARGING_STOP,/*stop power off charging animation*/
    LCD_GET_FRAME_BUFFER,
    LCD_WRITE_IMAGE,
    LCD_ALL_WHITE,
    LCD_ALL_BLACK,
    LCD_CMD_MAX
}LCD_IOCTL_CMD;


/*****************************************************************************
 函 数 名  : lcdRefresh
 功能描述  : lcd在指定位置刷屏函数
 输入参数  :
 输出参数  :
*          UINT32 ulXStart, UINT32 ulYStart－－起始坐标
*          UINT32 ulXOffset, UINT32 ulYOffset－－横向纵向偏移量
*          UINT8 *pucBuffer－－显示数据
 返 回 值  : void
*****************************************************************************/
extern int lcdRefresh(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,const unsigned char * test_buf);

/*****************************************************************************
 函 数 名  : balong_lcd_ioctl
 功能描述  : lcd ioctl
 输入参数  :
*            int cmd --command ID
*            arg--para
 输出参数  : none
 返 回 值  : BSP_S32
*****************************************************************************/
extern BSP_S32 balong_lcd_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

extern BSP_VOID lcdClearWholeScreen(BSP_VOID);
extern BSP_VOID lcdStringDisplay(UINT8 ucX, UINT8 ucY, UINT8 *pucStr);

/*************************LCD END*******************************/
#endif



/*************************SCI BEGIN*****************************/

/*  SIM  state*/
typedef enum tagSCI_CARD_STATE_E
{
    SCI_CARD_STATE_READY = 0,           	/* Ready */
    SCI_CARD_STATE_NOCARD,                	/* No card */
    SCI_CARD_STATE_BUSY,               		/* In initialization*/
	SCI_CARD_STATE_BUTT,               		/* Butt*/
} SCI_CARD_STATE_E;

/*****************************************************************************
* 函 数 名  : BSP_SCI_Reset
*
* 功能描述  : 本接口用于复位SCI（Smart Card Interface）驱动和USIM（Universal
*           Subscriber Identity Module）卡
*
* 输入参数  : BSP_VOID
* 输出参数  : 无
*
* 返 回 值  : OK  复位成功
*
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_Reset(BSP_VOID);
#define DRV_USIMMSCI_RST()    BSP_SCI_Reset()

/********************************************************************************************************************
 函 数 名  : BSP_GUSCI_GetCardStatus
 功能描述  : 本接口用于获得卡当前的状态，目前LTE提供接口参数类型不一致
 输入参数  : 无。
 输出参数  :
 返 回 值  : 0:  卡处于Ready；
             -1：卡未处于Ready。
 注意事项  ：有效调用BSP_SCI_Reset()函数后，再调用此函数。
********************************************************************************************************************/
extern int BSP_GUSCI_GetCardStatus(BSP_VOID);
#define DRV_USIMMSCI_GET_CARD_STAU()    BSP_GUSCI_GetCardStatus()

/*****************************************************************************
* 函 数 名  : DRV_USIMMSCI_SND_DATA
*
* 功能描述  : 本接口用于发送一段数据到USIM卡
*
* 输入参数  : BSP_U32 u32DataLength 发送数据的有效长度。取值范围1～256，单位为字节
*             BSP_U8 *pu8DataBuffer 发送数据所在内存的首地址，如是动态分配，调用接口
*                                   后不能立即释放，依赖于硬件发完数据
* 输出参数  : 无
*
* 返 回 值  : OK
*           BSP_ERR_SCI_NOTINIT
*           BSP_ERR_SCI_INVALIDPARA
*           BSP_ERR_SCI_DISABLED
*           BSP_ERR_SCI_NOCARD
*           BSP_ERR_SCI_NODATA
*
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/
BSP_S32 DRV_USIMMSCI_SND_DATA(BSP_U32 u32DataLength, BSP_U8 *pu8DataBuffer);

/*****************************************************************************
* 函 数 名  : DRV_USIMMSCI_RCV
*
* 功能描述  : 本接口用于USIM Manager读取来自SCI Driver的卡返回数据
*             该接口为阻塞接口，只有SCI接收到足够的数据量后才会返回；
*             该接口的超时门限为1s
*
* 输入参数  : BSP_U32 u32DataLength USIM Manager欲从SCI Driver读取的数据长度。
* 输出参数  : BSP_U8 *pu8DataBuffer USIM Manager指定的Buffer，SCI Driver将数据拷贝到本Buffer。
* 返 回 值  : OK
*             BSP_ERR_SCI_NOTINIT
*             BSP_ERR_SCI_INVALIDPARA
*
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/
BSP_S32 DRV_USIMMSCI_RCV(BSP_U32 u32Length,BSP_U8 *pu8Data);

/*****************************************************************************
* 函 数 名  : DRV_USIMMSCI_GET_ATR
*
* 功能描述  : 本接口用于将Driver层缓存的ATR数据和数据个数返回给USIM Manager层
*
* 输入参数  : BSP_VOID
* 输出参数  : unsigned long *u8DataLength  Driver读取的ATR数据长度，返回给USIM Manager。
*                                   取值范围0～32，单位是字节
*           BSP_U8 *pu8ATR          USIM Manager指定的Buffer，SCI Driver将ATR
*                                   数据拷贝到本Buffer。一般为操作系统函数动态分配
*                                   或者静态分配的地址
*
*
* 返 回 值  : OK
*          BSP_ERR_SCI_NOTINIT
*          BSP_ERR_SCI_INVALIDPARA
*
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/
BSP_S32 DRV_USIMMSCI_GET_ATR (unsigned long *u32DataLength, BSP_U8 *pu8ATR);

/*****************************************************************************
* 函 数 名  : DRV_USIMMSCI_CLASS_SWITCH
*
* 功能描述  : 本接口用于支持PS对卡的电压类型进行切换，从1.8V切换到3V
*
* 输入参数  : BSP_VOID
* 输出参数  : 无
*
* 返 回 值  :  OK    当前电压不是最高的，进行电压切换操作
*           BSP_ERR_SCI_CURRENT_STATE_ERR 切换失败 current SCI driver state is ready/rx/tx
*           BSP_ERR_SCI_VLTG_HIGHEST   当前电压已经是最高电压，没有进行电压切换
*           BSP_ERR_SCI_NOTINIT
*           BSP_ERR_SCI_CURRENT_VLTG_ERR 当前电压值异常（非class B或者C）
*
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/
BSP_S32 DRV_USIMMSCI_CLASS_SWITCH(BSP_VOID);

/*****************************************************************************
* 函 数 名  : DRV_USIMMSCI_TM_STOP
*
* 功能描述  : 本接口用于支持PS关闭SIM卡时钟
*
* 输入参数  :
*           SCI_CLK_STOP_TYPE_E enTimeStopCfg 时钟停止模式
*
* 输出参数  : 无
*
* 返 回 值  : OK - successful completion
*               ERROR - failed
*               BSP_ERR_SCI_NOTINIT
*               BSP_ERR_SCI_INVALIDPARA - invalid mode specified
*               BSP_ERR_SCI_UNSUPPORTED - not support such a operation
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/
BSP_VOID DRV_USIMMSCI_TM_STOP(BSP_U32 ulStopType);

/*****************************************************************************
* 函 数 名  : BSP_SCI_Deactive
*
* 功能描述  : 本接口用于对SIM卡的去激活操作
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : OK
*             BSP_ERR_SCI_NOTINIT
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_Deactive(BSP_VOID);
#define DRV_USIMMSCI_DEACT()    BSP_SCI_Deactive()

/*****************************************************************************
* 函 数 名  : BSP_SCI_GetClkStatus
*
* 功能描述  : 本接口用于获取当前SIM卡时钟状态
*
* 输入参数  : BSP_U32 *pu32SciClkStatus   变量指针，用于返回SIM卡时钟状态：
*                                       0：时钟已打开；
*                                       1：时钟停止
* 输出参数  : 无
*
* 返 回 值  : OK    操作成功
*          BSP_ERR_SCI_INVALIDPARA
* 修改记录  : 2009年6月29日   liumengcun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_GetClkStatus(BSP_U32 *pu32SciClkStatus);
#define DRV_USIMMSCI_GET_CLK_STAU(pulSciClkStatus)    BSP_SCI_GetClkStatus(pulSciClkStatus)

/*****************************************************************************
* 函 数 名  : BSP_SCI_GetClkFreq
*
* 功能描述  : 本接口用于获取当前SIM卡时钟频率
*
* 输入参数  : 无
*
* 输出参数  : unsigned long *pLen   时钟频率数据的长度
*             BSP_U8 *pBuf    时钟频率数据
* 返 回 值  : OK    操作成功
*             BSP_ERR_SCI_INVALIDPARA
* 修改记录  : 2010年8月24日   zhouluojun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_GetClkFreq(unsigned long *pLen, BSP_U8 *pBuf);
#define DRV_PCSC_GET_CLK_FREQ(pDataLen, Buffer)    BSP_SCI_GetClkFreq(pDataLen, Buffer)

/*****************************************************************************
* 函 数 名  : BSP_SCI_GetBaudRate
*
* 功能描述  : 本接口用于获取当前SIM卡时钟频率
*
* 输入参数  : 无
*
* 输出参数  : unsigned long *pLen   波特率数据的长度
*             BSP_U8 *pBuf    波特率数据
*
* 返 回 值  : OK    操作成功
*             BSP_ERR_SCI_INVALIDPARA
* 修改记录  : 2010年8月24日   zhouluojun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_GetBaudRate(unsigned long *pLen, BSP_U8 *pBuf);
#define DRV_PCSC_GET_BAUD_RATE(pDataLen, Buffer)    BSP_SCI_GetBaudRate(pDataLen, Buffer)

/*****************************************************************************
* 函 数 名  : BSP_SCI_GetPCSCParameter
*
* 功能描述  : 本接口用于获取当前SIM卡的PCSC相关参数
*
* 输入参数  : 无
*
* 输出参数  : BSP_U8 *pBuf    PCSC相关参数
*
* 返 回 值  : OK    操作成功
*             BSP_ERR_SCI_INVALIDPARA
* 修改记录  : 2010年8月24日   zhouluojun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_GetPCSCParameter(BSP_U8 *pBuf);
#define DRV_PCSC_GET_SCI_PARA(Para)    BSP_SCI_GetPCSCParameter(Para)

/*****************************************************************************
 函 数 名  : sciRecordDataSave
 功能描述  : 本接口用于获取当前SIM卡交互数据。属于异常处理模块，V7先打桩
 输入参数  : 无。
 输出参数  : 无
 返 回 值  : 0:  操作成功；
             -1：操作失败，指针参数为空。
 注意事项  ：
*****************************************************************************/
/*extern int sciRecordDataSave(void);*/
extern int BSP_USIMMSCI_STUB(void);
#define DRV_USIMMSCI_RECORD_DATA_SAVE()        BSP_USIMMSCI_STUB()

typedef enum
{
    SIM_CARD_OUT = 0,
    SIM_CARD_IN  = 1
} SCI_DETECT_STATE;

typedef void (*OMSCIFUNCPTR)(unsigned int ulVal);

/*****************************************************************************
* 函 数 名  : BSP_SCIFuncRegister
*
* 功能描述  : 本接口用于注册OAM的回调函数
*
* 输入参数  : omSciFuncPtr
* 输出参数  : 无
*
* 返 回 值  : NA
*
*
*****************************************************************************/
void BSP_SCIFuncRegister(OMSCIFUNCPTR omSciFuncPtr);
#define DRV_USIMMSCI_FUNC_REGISTER(omSciFuncPtr)    BSP_SCIFuncRegister(omSciFuncPtr)

/*************************SCI END*************************************/

/*************************PMU BEGIN***********************************/

/*****************************************************************************
 函 数 名  : DRV_GET_PMU_STATE
 功能描述  : 获取PMU模块开机方式、充电状态、电池电量、电池在位状态。
 输入参数  : 无。
 输出参数  : Pmu_State :开机方式、充电状态、电池电量、电池在位状态。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
 注意事项　：该接口仅在PS的PC工程中使用，目前没有应用，暂保留。
*****************************************************************************/
extern unsigned int DRV_GET_PMU_STATE(void*  Pmu_State);

/*****************************************************************************
 函 数 名  : BSP_PMU_UsbEndRegActionFunc
 功能描述  : 本接口是USB插入/拔出回调注册函数。
 输入参数  : srcFunc：注册的USB插入或拔出动作发生时的回调函数指针。
             actionIndex：动作指示。
                          0：参数srcFunc是注册的USB插入动作的回调函数；
                          1：参数srcFunc是注册的USB拔出动作的回调函数。

 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
 注意事项  ：调用此接口，将所要执行的函数指针传入，即在USB插入或拔出时调用所要执行的函数。
*****************************************************************************/
extern int BSP_PMU_UsbEndRegActionFunc(pFUNCPTR srcFunc, unsigned char actionIndex);
#define DRV_USB_ENDREGACTION_FUNC(srcFunc,actionIndex) \
                BSP_PMU_UsbEndRegActionFunc(srcFunc,actionIndex)


/*****************************************************************************
函数名：   BSP_USB_PortTypeValidCheck
功能描述:  提供给上层查询设备端口形态配置合法性接口
           1、端口为已支持类型，2、包含PCUI口，3、无重复端口，4、端点数不超过16，
           5、第一个设备不为MASS类
输入参数： pucPortType  端口形态配置
           ulPortNum    端口形态个数
返回值：   0:    端口形态合法
           其他：端口形态非法
*****************************************************************************/
unsigned int BSP_USB_PortTypeValidCheck(unsigned char *pucPortType, unsigned long ulPortNum);
#define DRV_USB_PORT_TYPE_VALID_CHECK(pucPortType, ulPortNum)  \
                    BSP_USB_PortTypeValidCheck(pucPortType, ulPortNum)

/*****************************************************************************
函数名：   BSP_USB_GetAvailabePortType
功能描述:  提供给上层查询当前设备支持端口形态列表接口
输入参数： ulPortMax    协议栈支持最大端口形态个数
输出参数:  pucPortType  支持的端口形态列表
           pulPortNum   支持的端口形态个数
返回值：   0:    获取端口形态列表成功
           其他：获取端口形态列表失败
*****************************************************************************/
unsigned int BSP_USB_GetAvailabePortType(unsigned char *pucPortType,
                            unsigned long *pulPortNum, unsigned long ulPortMax);
#define DRV_USB_GET_AVAILABLE_PORT_TYPE(pucPortType, pulPortNum, ulPortMax)  \
                BSP_USB_GetAvailabePortType(pucPortType, pulPortNum, ulPortMax)

/*************************PMU END*************************************/

/*************************INT BEGIN***********************************/

/*****************************************************************************
* 函 数 名  : BSP_INT_Enable
*
* 功能描述  : 使能某个中断
*
* 输入参数  : INT32 ulLvl 要使能的中断号，取值范围0～40
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2009年3月5日   zhanghailun  creat
*****************************************************************************/
BSP_S32 BSP_INT_Enable ( BSP_S32 s32Lvl);
#define DRV_VICINT_ENABLE(ulLvl)    BSP_INT_Enable(ulLvl)

/*****************************************************************************
 * 函 数 名  : BSP_INT_Disable
 *
 * 功能描述  : 去使能某个中断
 *
 * 输入参数  : INT32 ulLvl 要使能的中断号，取值范围0～40
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 修改记录  : 2009年3月5日   zhanghailun  creat
 *****************************************************************************/
BSP_S32 BSP_INT_Disable ( BSP_S32 s32Lvl);
#define  DRV_VICINT_DISABLE(ulLvl)    BSP_INT_Disable(ulLvl)

/*****************************************************************************
 * 函 数 名  : BSP_INT_Connect
 *
 * 功能描述  : 注册某个中断
 *
 * 输入参数  : VOIDFUNCPTR * vector 中断向量号，取值范围0～40
 *           VOIDFUNCPTR routine  中断服务程序
 *           INT32 parameter      中断服务程序参数
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 修改记录  : 2009年3月5日   zhanghailun  creat
 *****************************************************************************/
BSP_S32 BSP_INT_Connect  (VOIDFUNCPTR * vector,VOIDFUNCPTR routine, BSP_S32 parameter);
#define DRV_VICINT_CONNECT(vector,routine,parameter)    BSP_INT_Connect(vector,routine,parameter)

/*************************INT END*************************************/

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)

/*************************WDT BEGIN***********************************/

/*****************************************************************************
* 函 数 名  : BSP_WDT_Init
*
* 功能描述  : 初始化定制看门狗，设置超时时间，挂接中断服务函数
*
* 输入参数  : wdtId         看门狗ID,芯片提供1个看门狗，ID可以为0
*             wdtTimeOuts   看门狗超时时间，单位秒,时间最大不能超过51秒
*
* 输出参数  : 无
*
* 返 回 值  : OK& ERROR
*
* 修改记录 :  2009年3月5日  v1.00  wangxuesong  创建
*****************************************************************************/
BSP_S32 BSP_WDT_Init(BSP_U8 wdtId , BSP_U32 wdtTimeOuts);
#define DRV_WDT_INIT(wdtId,wdtTimeOutms)      BSP_WDT_Init(wdtId,wdtTimeOutms)

/*****************************************************************************
* 函 数 名  : BSP_WDT_EnableWdt
*
* 功能描述  : 开启看门狗
*
* 输入参数  : wdtId  看门狗ID
*
* 输出参数  : 无
*
* 返 回 值  : OK& ERROR
*
* 修改记录 :  2009年3月5日  v1.00  wangxuesong  创建
*****************************************************************************/
BSP_S32 BSP_WDT_Enable(BSP_U8 wdtId);
#define DRV_WDT_ENABLE(wdtId)    BSP_WDT_Enable(wdtId)

/*****************************************************************************
* 函 数 名  : BSP_WDT_Disable
*
* 功能描述  : 关闭看门狗
*
* 输入参数  : wdtId  看门狗ID
*
* 输出参数  : 无
*
* 返 回 值  : OK& ERROR
*
* 修改记录 :  2009年3月5日  v1.00  wangxuesong  创建
*****************************************************************************/
BSP_S32 BSP_WDT_Disable(BSP_U8 wdtId);
#define DRV_WDT_DISABLE(wdtId)    BSP_WDT_Disable(wdtId)
#endif

/*****************************************************************************
* 函 数 名  : BSP_WDT_HardwareFeed
*
* 功能描述  : 重置看门狗计数寄存器（喂狗）
*
* 输入参数  : wdtId  看门狗ID
*
* 输出参数  : 无
*
* 返 回 值  : OK& ERROR
*
* 修改记录 :  2009年3月5日  v1.00  wangxuesong  创建
*****************************************************************************/
BSP_S32 BSP_WDT_HardwareFeed(BSP_U8 wdtId);
#define DRV_WDT_FEED(wdtId)    BSP_WDT_HardwareFeed(wdtId)

/*************************WDT END*************************************/

/*************************SD BEGIN************************************/
#define WRFlAG     1
#define RDFlAG     0

typedef enum
{
    TFUP_FAIL = 0,  /*打叉*/
    TFUP_OK         /*打勾*/
}TFUP_DISPLAY_TYPE;

enum SD_MMC_OPRT_ENUM
{
    SD_MMC_OPRT_FORMAT = 0,
    SD_MMC_OPRT_ERASE_ALL,
    SD_MMC_OPRT_ERASE_SPEC_ADDR,
    SD_MMC_OPRT_WRITE,
    SD_MMC_OPRT_READ,
    SD_MMC_OPRT_BUTT
};

typedef unsigned int SD_MMC_OPRT_ENUM_UINT32;


/*SD卡设备类型*/
typedef struct _sd_dev_type_str
{
    unsigned long   devNameLen;
    char            devName[16];
}SD_DEV_TYPE_STR;

/*****************************************************************************
 函 数 名  : BSP_SDMMC_ATProcess
 功能描述  : at^sd,SD卡操作，写，擦除，格式化操作
 输入参数  : 操作类型 ulOp:
            0  格式化SD卡
            1  擦除整个SD卡内容；
            2  用于指定地址内容的擦除操作，指定擦除的内容长度为512字节。擦除后的地址中写全1
            3  写数据到SD卡的指定地址中，需要带第二个和第三个参数
            4  读取ulAddr指定的地址(ulAddr*512)的512个字节的内容到pucBuffer中

            ulAddr < address >  地址，以512BYTE为一个单位，用数字n表示

            ulData
             < data >            数据内容，表示512BYTE的内容，每个字节的内容均相同。
             0       字节内容为0x00
             1       字节内容为0x55
             2       字节内容为0xAA
             3       字节内容为0xFF

 输出参数  : pulErr
 返 回 值  : 0 ：OK  非 0 ：Error

            具体的错误值填充在*pulErr中
            0 表示SD卡不在位
            1 表示SD卡初始化失败
            2 表示<opr>参数非法，对应操作不支持(该错误由AT使用,不需要底软使用)
            3 表示<address>地址非法，超过SD卡本身容量
            4 其他未知错误
*****************************************************************************/
extern  unsigned long  BSP_SDMMC_ATProcess(SD_MMC_OPRT_ENUM_UINT32 ulOp,
                unsigned long ulAddr,  unsigned long ulData,unsigned char *pucBuffer,unsigned long *pulErr);
#define DRV_SDMMC_AT_PROCESS(ulOp,ulAddr,ulData,pucBuffer,pulErr)   \
                     BSP_SDMMC_ATProcess (ulOp,ulAddr,ulData,pucBuffer,pulErr)

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#else
/*****************************************************************************
* 函 数 名  : sd_mmc_blk_w
*
* 功能描述  :写卡接口
*
* 输入参数  : u32StartBlk 起始块
				  pu8DataBuf  写数据缓存
				  u32Len 写数据大小，必须为512字节的整数倍
* 输出参数  : NA
*
* 返 回 值  :  成功返回0
*
* 其它说明  : NA
*
*****************************************************************************/
extern BSP_S32 sd_mmc_blk_w(BSP_U32 u32StartBlk,BSP_U8 *pu8DataBuf,BSP_U32 u32Len);
/*****************************************************************************
* 函 数 名  : sd_mmc_blk_r
*
* 功能描述  : 读卡接口
*
* 输入参数  : u32StartBlk 起始块
				  pu8DataBuf  读数据缓存
				  u32Len  读数据大小，必须为512字节的整数倍
* 输出参数  : NA
*
* 返 回 值  :  成功返回0
*
* 其它说明  : NA
*
*****************************************************************************/
extern BSP_S32 sd_mmc_blk_r(BSP_U32 u32StartBlk,BSP_U8 *pu8DataBuf,BSP_U32 u32Len);
/*****************************************************************************
* 函 数 名  : sd_mmc_blk_erase
*
* 功能描述  : 卡块擦除接口
*
* 输入参数  : u32StartBlk 起始块
				  u32EndBlk  终止块
* 输出参数  : NA
*
* 返 回 值  :  成功返回0
*
* 其它说明  : NA
*
*****************************************************************************/
extern BSP_S32 sd_mmc_blk_erase(BSP_U32 u32StartBlk,BSP_U32 u32EndBlk);
/*****************************************************************************
* 函 数 名  : sd_mmc_get_status
*
* 功能描述  : 卡在位查询
*
* 输入参数  : void
* 输出参数  : NA
*
* 返 回 值  : 0 : 在位；-1: 不在位
*
* 其它说明  : NA
*
*****************************************************************************/
extern BSP_S32 sd_mmc_get_status(BSP_VOID);
#endif

/*****************************************************************************
 函 数 名  : BSP_SDMMC_GetOprtStatus
 功能描述  : at^sd,SD卡当前操作状态
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
            0: 未操作或操作已经完成;
            1: 操作中
            2: 上次操作失败
*****************************************************************************/
extern unsigned long BSP_SDMMC_GetOprtStatus(void);
#define DRV_SDMMC_GET_OPRT_STATUS()    BSP_SDMMC_GetOprtStatus()

/*****************************************************************************
 函 数 名  : BSP_SDMMC_AddHook
 功能描述  : TF模块钩子函数注册。
 输入参数  : hookType：回调函数类型，
                       0：获得升级文件信息后调用的回调函数；
                       1：TF卡插入通知OM的回调函数；
                       2：TF卡拔出通知OM的回调函数；
                       3； TF卡初始化后通知U盘的回调函数。
             p：回调函数指针。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
 注意事项  ：对于同一类型回调函数，重复注册会覆盖以前设置。
*****************************************************************************/
//#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
extern int sdmmcAddHook (int hookType , void * p);/*y00186965 for sd_update*/
#define DRV_SDMMC_ADD_HOOK(hookType,p)    sdmmcAddHook(hookType,p)
//#else
//extern int BSP_SDMMC_AddHook(int hookType, void * p);
//#define DRV_SDMMC_ADD_HOOK(hookType,p)    BSP_SDMMC_AddHook(hookType,p)
//#endif

/*****************************************************************************
 函 数 名  : BSP_SDMMC_UsbGetStatus
 功能描述  : 返回SD卡状态
 输入参数  : 无。
 输出参数  : 无。
 返回值：   0  为可用
           非0 不可用

*****************************************************************************/
extern unsigned int BSP_SDMMC_UsbGetStatus(void);
#define DRV_SDMMC_USB_STATUS()    BSP_SDMMC_UsbGetStatus();

/*****************************************************************************
* 函 数 名  : sdmmc_ClearWholeScreen
* 功能描述  : SD卡升级NV恢复后的显示，清屏
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  :无
* 其它说明  : 无
*****************************************************************************/
extern void sdmmc_ClearWholeScreen(void);
#define DRV_SDMMC_CLEAR_WHOLE_SCREEN()    sdmmc_ClearWholeScreen()

/*****************************************************************************
* 函 数 名  : sdmmc_UpdateDisplay
* 功能描述  : SD卡升级NV恢复后的显示，显示
* 输入参数  : BOOL UpdateStatus
* 输出参数  : 无
* 返 回 值  :无
* 其它说明  : 升级成功，NV备份完成后调用该函数打勾
              升级失败后调用该函数打叉
*****************************************************************************/
extern void sdmmc_UpdateDisplay(int UpdateStatus);
#define DRV_SDMMC_UPDATE_DISPLAY(state)    sdmmc_UpdateDisplay(state)

/*****************************************************************************
* 函 数 名  : DRV_SD_GET_STATUS
*
* 功能描述  : 卡在位查询
*
* 输入参数  : void
* 输出参数  : NA
*
* 返 回 值  : 0 : 在位；-1: 不在位
*
* 其它说明  : NA
*
*****************************************************************************/
extern int DRV_SD_GET_STATUS(void);

/*****************************************************************************
* 函 数 名  : DRV_SD_GET_CAPACITY
*
* 功能描述  : 卡容量查询
*
* 输入参数  : void
* 输出参数  : NA
*
* 返 回 值  : 0 : 失败；>0: 卡容量
*
* 其它说明  : NA
*
*****************************************************************************/
extern int DRV_SD_GET_CAPACITY(void);

/*****************************************************************************
* 函 数 名  : DRV_SD_TRANSFER
*
* 功能描述  : 数据传输
*
* 输入参数  : struct scatterlist *sg    待传输数据结构体指针
                            unsigned dev_addr   待写入的SD block 地址
                            unsigned blocks    待写入的block个数
                            unsigned blksz      每个block的大小，单位字节
                            int wrflags    读写标志位，写:WRFlAG ; 读:RDFlAG
* 输出参数  : NA
*
* 返 回 值  : 0 : 成功；其它: 失败
*
* 其它说明  : NA
*
*****************************************************************************/
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#else
extern int DRV_SD_TRANSFER(struct scatterlist *sg, unsigned dev_addr,unsigned blocks, unsigned blksz, int wrflags);
#endif
/*****************************************************************************
* 函 数 名  : DRV_SD_SG_INIT_TABLE
*
* 功能描述  : SD多块数据传输sg list初始化
*
* 输入参数  : const void *buf		待操作的buffer地址
				  unsigned int buflen	待操作的buffer大小，小于32K, 大小为512B的整数倍
				  					大于32K, 大小为32KB的整数倍，最大buffer为128K
* 输出参数  : NA
*
* 返 回 值  :  0 : 成功;  其它:失败
* 其它说明  : NA
*
*****************************************************************************/
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#else
extern int DRV_SD_SG_INIT_TABLE(const void *buf,unsigned int buflen);
#endif
/*****************************************************************************
* 函 数 名  : DRV_SD_MULTI_TRANSFER
*
* 功能描述  : SD多块数据传输
*
* 输入参数  : unsigned dev_addr	待写入的SD block地址
				  unsigned blocks		待写入的block 个数
				  unsigned blksz		每个block 的大小，单位字节
				  int write			读写标志位，写:1;	读:0
* 输出参数  : NA
*
* 返 回 值  :  0 : 成功;  其它:失败
* 其它说明  : NA
*
*****************************************************************************/
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
#else
extern int DRV_SD_MULTI_TRANSFER(unsigned dev_addr,unsigned blocks,unsigned blksz,int write);
#endif
/*************************SD END**************************************/


/*************************DLOAD BEGIN*****************************/
/*--------------------------------------------------------------*
 * 宏定义                                                       *
 *--------------------------------------------------------------*/
/* 错误码 */
#define DLOAD_OK                    BSP_OK
#define DLOAD_ERROR                 BSP_ERROR
#define DLOAD_ERR_NOT_INITED        BSP_DEF_ERR(BSP_MODU_DLOAD,BSP_ERR_MODULE_NOT_INITED)
#define DLOAD_ERR_NULL_PTR          BSP_DEF_ERR(BSP_MODU_DLOAD,BSP_ERR_NULL_PTR)
#define DLOAD_ERR_INVALID_PARA      BSP_DEF_ERR(BSP_MODU_DLOAD,BSP_ERR_INVALID_PARA)
#define DLOAD_ERR_ALLOC_FAILED      BSP_DEF_ERR(BSP_MODU_DLOAD,BSP_ERR_BUF_ALLOC_FAILED)
#define DLOAD_ERR_FREE_FAILED       BSP_DEF_ERR(BSP_MODU_DLOAD,BSP_ERR_BUF_FREE_FAILED)
#define DLOAD_ERR_RETRY_TIMEOUT     BSP_DEF_ERR(BSP_MODU_DLOAD,BSP_ERR_RETRY_TIMEOUT)

#define DLOAD_ERR_NO_BACKUP         BSP_DEF_ERR(BSP_MODU_DLOAD, BSP_ERR_SPECIAL + 1)  /* 无备份版本 */
#define DLOAD_ERR_OPEN_FAILED       BSP_DEF_ERR(BSP_MODU_DLOAD, BSP_ERR_SPECIAL + 2)  /* 虚拟串口打开失败 */
#define DLOAD_ERR_SEM_CREAT         BSP_DEF_ERR(BSP_MODU_DLOAD, BSP_ERR_SPECIAL + 3)  /* 信号量创建失败 */
#define DLOAD_ERR_ABORT             BSP_DEF_ERR(BSP_MODU_DLOAD, BSP_ERR_SPECIAL + 4)  /* 升级终止 */
#define DLOAD_ERR_MODE_MISMATCH     BSP_DEF_ERR(BSP_MODU_DLOAD, BSP_ERR_SPECIAL + 5)  /* 工作模式不匹配 */
#define DLOAD_ERR_INVALID_ATSTRING  BSP_DEF_ERR(BSP_MODU_DLOAD, BSP_ERR_SPECIAL + 6)  /* AT字符串非法 */


#define MODE_GSM      0x01
#define MODE_CDMA     0x02
#define MODE_WCDMA    0x04


/*--------------------------------------------------------------*
 * 枚举定义                                                     *
 *--------------------------------------------------------------*/
enum UPDATE_STATUS_I
{
	DL_UPDATE = 0,
	DL_NOT_UPDATE = 1
};

typedef enum
{
	NORMAL_DLOAD = 0,	/*正常升级模式*/
	FORCE_DLOAD = 1    /*强制升级模式*/
}DLOAD_TYPE;

/* 下载模式枚举 */
typedef enum tagDLOAD_MODE_E
{
    DLOAD_MODE_DOWNLOAD = 0,
    DLOAD_MODE_NORMAL,
    DLOAD_MODE_DATA,
    DLOAD_MODE_MAX
}DLOAD_MODE_E;

/*--------------------------------------------------------------*
 * 函数指针类型定义                                             *
 *--------------------------------------------------------------*/
typedef BSP_U32 (*BSP_DLOAD_AtCallBack)( BSP_VOID* pDataIn, BSP_U32 ulLen);
typedef BSP_U32 (*BSP_DLOAD_NVGetInfoCb)( BSP_VOID* pDataOut, BSP_U32 ulLen);
typedef BSP_U32 (*BSP_DLOAD_NVSetInfoCb)( BSP_VOID* pDataIn, BSP_U32 ulLen);

/*****************************************************************************
* 函 数 名  : BSP_DLOAD_GetProductId
*
* 功能描述  : 获取产品名称字符串
*
* 输入参数  : BSP_S8 *str   :字符串缓冲区
*             BSP_S32 len   :字符串长度
* 输出参数  : BSP_S8 *str   :字符串缓冲区
*
* 返 回 值  : DLOAD_ERR_INVALID_PARA    :输入参数非法
*             DLOAD_ERROR               :失败
*             其它                      :返回字符串长度
*
* 其它说明  : AT模块调用
*             正常模式支持
*             下载模式支持
*
*****************************************************************************/
BSP_S32 BSP_DLOAD_GetProductId(BSP_CHAR *str, BSP_U32 len);
/*****************************************************************************
* 函 数 名  : BSP_DLOAD_GetDloadType
*
* 功能描述  : 获取下载类型
*
* 输入参数  : BSP_VOID
* 输出参数  : 无
*
* 返 回 值  : 0 :正常模式
*             1 :强制模式
*
* 其它说明  : AT模块调用
*             正常模式支持
*             下载模式支持
*
*****************************************************************************/
BSP_S32 BSP_DLOAD_GetDloadType(BSP_VOID);
/*****************************************************************************
* 函 数 名  : BSP_DLOAD_GetCurMode
*
* 功能描述  : 设置当前工作模式
*
* 输入参数  : BSP_VOID
* 输出参数  : DLOAD_MODE_NORMAL     :正常模式
*             DLOAD_MODE_DATA       :数据模式
*             DLOAD_MODE_DOWNLOAD   :下载模式
*
* 返 回 值  : 无
*
* 其它说明  : AT模块调用
*             正常模式支持的有:
*                 BSP_DLOAD_GetCurMode(DLOAD_MODE_DOWNLOAD)
*             下载模式支持的有:
*                 BSP_DLOAD_GetCurMode(DLOAD_MODE_DATA)
*
*****************************************************************************/
BSP_VOID BSP_DLOAD_SetCurMode(DLOAD_MODE_E eDloadMode);
/*****************************************************************************
* 函 数 名  : BSP_DLOAD_GetCurMode
*
* 功能描述  : 获取当前工作模式
*
* 输入参数  : BSP_VOID
* 输出参数  : 无
*
* 返 回 值  : DLOAD_MODE_NORMAL     :正常模式
*             DLOAD_MODE_DATA       :数据模式
*             DLOAD_MODE_DOWNLOAD   :下载模式
*
* 其它说明  : 无
*
*****************************************************************************/
DLOAD_MODE_E BSP_DLOAD_GetCurMode(BSP_VOID);
/*****************************************************************************
* 函 数 名  : BSP_DLOAD_SetSoftLoad
*
* 功能描述  : 设置启动标志
*
* 输入参数  : BSP_BOOL bSoftLoad  :
*             BSP_FALSE :从vxWorks启动
*             BSP_TRUE  :从bootrom启动
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_VOID BSP_DLOAD_SetSoftLoad (BSP_BOOL bSoftLoad);


/*****************************************************************************
* 函 数 名  : BSP_DLOAD_GetSoftLoad
*
* 功能描述  : 获取启动标志
*
* 输入参数  : BSP_VOID
* 输出参数  : 无
*
* 返 回 值  : BSP_TRUE  :从bootrom启动
*             BSP_FALSE :从vxWorks启动
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_BOOL BSP_DLOAD_GetSoftLoad (BSP_VOID);

/*****************************************************************************
* 函 数 名  : BSP_DLOAD_SendData
*
* 功能描述  : 通过虚拟串口向PC端发送数据
*
* 输入参数  : pBuf      :数据缓冲区
*             u32Len    :数据缓冲区长度
* 输出参数  : pBuf      :数据缓冲区
*
* 返 回 值  : 无
*
* 其它说明  : pBuf必须保证cache line(32字节)对齐
*
*****************************************************************************/
BSP_S32 BSP_DLOAD_SendData(BSP_CHAR *pBuf, BSP_U32 u32Len);
/*****************************************************************************
* 函 数 名  : BSP_DLOAD_AtProcReg
*
* 功能描述  : 注册AT处理函数
*
* 输入参数  : pFun
* 输出参数  : 无
*
* 返 回 值  : DLOAD_OK:成功
*
*****************************************************************************/
BSP_S32 BSP_DLOAD_AtProcReg (BSP_DLOAD_AtCallBack pFun);

/*****************************************************************************
* 函 数 名  : BSP_DLOAD_GetSoftwareVer
*
* 功能描述  : 获取软件版本号
*
* 输入参数  : BSP_S8 *str   :字符串缓冲区
*             BSP_S32 len   :字符串长度
* 输出参数  : BSP_S8 *str   :字符串缓冲区
*
* 返 回 值  : DLOAD_ERROR   :输入参数非法
*             其它          :返回字符串长度
*
* 其它说明  : AT模块调用
*             正常模式支持
*             下载模式支持
*
*****************************************************************************/
BSP_S32 BSP_DLOAD_GetSoftwareVer(BSP_CHAR *str, BSP_U32 len);


/*****************************************************************************
* 函 数 名  : BSP_DLOAD_GetDloadVer
*
* 功能描述  : 查询下载协议版本号。该信息BSP固定写为2.0。
*
* 输入参数  : BSP_S8 *str   :字符串缓冲区
*             BSP_S32 len   :字符串长度
* 输出参数  : BSP_S8 *str   :字符串缓冲区
*
* 返 回 值  : DLOAD_ERROR   :输入参数非法
*             其它          :返回字符串长度
*
* 其它说明  : AT模块调用
*             正常模式支持
*             下载模式支持
*****************************************************************************/
BSP_S32 BSP_DLOAD_GetDloadVer(BSP_CHAR *str, BSP_U32 len);

/*****************************************************************************
* 函 数 名  : BSP_DLOAD_GetISOVer
*
* 功能描述  : 获取后台版本号字符串
*
* 输入参数  : BSP_S8 *str   :字符串缓冲区
*             BSP_S32 len   :字符串长度
* 输出参数  : BSP_S8 *str   :字符串缓冲区
*
* 返 回 值  : DLOAD_ERROR   :输入参数非法
*             其它          :返回字符串长度
*
* 其它说明  : AT模块调用
*             正常模式支持
*             下载模式支持
*
*****************************************************************************/
BSP_S32 BSP_DLOAD_GetISOVer(BSP_CHAR *str, BSP_U32 len);
#define DRV_GET_CDROM_VERSION(pVersionInfo,ulLength)    BSP_DLOAD_GetISOVer(pVersionInfo, ulLength)

/*****************************************************************************
 函 数 名  : DRV_SET_UPDATA_FLAG
 功能描述  : 设置升级加载标志。
 输入参数  : flag：升级加载标志，
                   0：启动后进入bootrom，进行升级加载。
                   1：启动后不进入bootrom，正常启动。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int DRV_SET_UPDATA_FLAG(int flag);


/*****************************************************************************
 函 数 名  : DRV_USB_DISCONNECT
 功能描述  : 升级时断开USB连接
 输入参数  : 无
                  
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
BSP_VOID DRV_USB_DISCONNECT();
	
/*****************************************************************************
* 函 数 名  : BSP_DLOAD_SetCdromMarker
*
* 功能描述  : 设置设备形态标志
*
* 输入参数  : BSP_BOOL bCdromMarker  :
*             BSP_TRUE  :设置正常设备形态
*             BSP_FALSE :设置bootrom设备形态
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 其它说明  : 当升级后台文件时，设置为上报正常设备形态，不恢复NV；
*             当升级前台文件时，设置为上报bootrom设备形态，恢复NV；
*
*****************************************************************************/
BSP_VOID BSP_DLOAD_SetCdromMarker(BSP_BOOL bCdromMarker);
#define DRV_SET_CDROM_FLAG(flag)    BSP_DLOAD_SetCdromMarker(flag)

/*****************************************************************************
* 函 数 名  : BSP_DLOAD_GetCdromMarker
*
* 功能描述  : 获取设备形态标志
*
* 输入参数  : BSP_VOID
* 输出参数  : 无
*
* 返 回 值  : BSP_TRUE  :上报正常设备形态
*             BSP_FALSE :上报bootrom设备形态
*
* 其它说明  : 当升级后台文件时，设置为上报正常设备形态，不恢复NV；
*             当升级前台文件时，设置为上报bootrom设备形态，恢复NV；
*
*****************************************************************************/
BSP_BOOL BSP_DLOAD_GetCdromMarker (BSP_VOID);
#define DRV_GET_CDROM_FLAG()    BSP_DLOAD_GetCdromMarker()

/********************************************************************************************************
 函 数 名  : BSP_DLOAD_NVBackupRead
 功能描述  : 从Flash中的NV项备份区读取数据，实现NV项的恢复功能。
 输入参数  : len：从NV项备份区起始处开始，需要读取的NV项长度（字节数），不超过1Block。
 输出参数  : pRamAddr：目的RAM地址，用于存放读出的NV项数据。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
********************************************************************************************************/
extern int BSP_DLOAD_NVBackupRead(unsigned char *pRamAddr, unsigned int len);
#define DRV_NVBACKUP_READ(pRamAddr,len)    BSP_DLOAD_NVBackupRead(pRamAddr, len)

/********************************************************************************************************
 函 数 名  : BSP_DLOAD_NVBackupWrite
 功能描述  : 将特定数据写入Flash中的NV项备份区，实现NV项的备份功能。
 输入参数  : pRamAddr：源RAM地址，用于存放需要写入的NV项数据。
             len：从NV项备份区起始处开始，需要写入的NV项长度（字节数），不超过1Block。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
********************************************************************************************************/
extern int BSP_DLOAD_NVBackupWrite(unsigned char *pRamAddr, unsigned int len);
#define DRV_NVBACKUP_WRITE(pRamAddr, len)   BSP_DLOAD_NVBackupWrite(pRamAddr, len)


/********************************************************************************************************
 函 数 名  : NVBackupFlashDataWrite
 功能描述  : 实现FLASH 中NV备份恢复区从指定位置读功能。
 输入参数  : pRamAddr:源RAM地址
*         offset :从NV备份区0地址开始的偏移
*         len: 需要写入的长度
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
********************************************************************************************************/
extern int NVBackupFlashDataWrite(unsigned char* pRamAddr, unsigned int offset,unsigned int len);
#define DRV_NV_FLASH_WRITE(pRamAddr, offset,len) NVBackupFlashDataWrite(pRamAddr, offset,len)


/*****************************************************************************
 函 数 名  : BSP_DLOAD_GetTFUpdateFlag
 功能描述  : 判断是否是TF卡升级
 输入参数  : None
 输出参数  : None
 返 回 值  : 返回1是TF升级，NV恢复成功，需要点蓝灯
             返回0不是TF升级，NV恢复成功，不需要点蓝灯

*****************************************************************************/
extern int BSP_DLOAD_GetTFUpdateFlag(void);
#define DRV_GET_TFUPDATE_FLAG()    BSP_DLOAD_GetTFUpdateFlag()

/*****************************************************************************
 函 数 名  : DRV_GET_DLOAD_VERSION
 功能描述  : Get dload version
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern BSP_S32 DRV_GET_DLOAD_VERSION(BSP_U8 *str, int len);


/*****************************************************************************
 函 数 名  : DRV_GET_DLOAD_INFO
 功能描述  : Get dload infomation
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/

extern BSP_S32 DRV_GET_DLOAD_INFO(unsigned char atCmdBuf[], unsigned int dloadType);

/*****************************************************************************
 函 数 名  : DRV_GET_AUTHORITY_VERSION
 功能描述  : Get Authority version
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern BSP_S32 DRV_GET_AUTHORITY_VERSION(BSP_U8 *str, BSP_S32 len);

/*****************************************************************************
 函 数 名  : DRV_GET_AUTHORITY_ID
 功能描述  : 获取下载鉴权协议Id
 输入参数  : unsigned char *buf
                          int len
 输出参数  : 无
 返 回 值  : 0:  操作成功；
                      -1：操作失败。
*****************************************************************************/
extern BSP_S32 DRV_GET_AUTHORITY_ID(unsigned char *buf, BSP_S32 len);

/*****************************************************************************
 函 数 名  : BSP_DLOAD_GetDloadNetMode
 功能描述  : Get net mode
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern BSP_S32  BSP_DLOAD_GetDloadNetMode(BSP_U32 *netMode);
#define DRV_GET_DLOAD_NETMODE(netMode)    BSP_DLOAD_GetDloadNetMode (netMode)

/*****************************************************************************
 函 数 名  : BSP_DLOAD_GetDloadFlashInfo
 功能描述  : Get dload flash infomation
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern BSP_S32 BSP_DLOAD_GetDloadFlashInfo(DLOAD_FLASH_STRU* pFlashInfo);
#define DRV_GET_DLOAD_FLASHINFO(pFlashInfo)    BSP_DLOAD_GetDloadFlashInfo(pFlashInfo)


/*****************************************************************************
 函 数 名  : BSP_DLOAD_GetWebUIVersion
 功能描述  : 获得WEBUI 版本信息
 输入参数  : pVersionInfo: 存放返回的版本信息的内存地址
                           ulLength: 存放返回的版本信息的内存长度，目前固定
                           128
 输出参数  : pVersionInfo: 返回的版本信息的内存地址
 返 回 值  :  0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern BSP_S32 BSP_DLOAD_GetWebUIVersion(BSP_CHAR *pVersionInfo, BSP_U32 u32Length);
#define DRV_GET_WEBUI_VERSION(pVersionInfo, u32Length) BSP_DLOAD_GetWebUIVersion(pVersionInfo,u32Length)

/*****************************************************************************
 函 数 名  : BSP_TFUP_CompleteDeal
 功能描述  : TF卡升级完成后处理
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
extern void BSP_TFUP_CompleteDeal(void);
#define DRV_TFUP_COMPLETEDEAL() BSP_TFUP_CompleteDeal()

/*****************************************************************************
 函 数 名  : BSP_TFUP_CompleteDeal
 功能描述  : 在线升级完成后处理
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
void BSP_ONUP_CompleteDeal(void);
#define DRV_ONUP_COMPLETEDEAL() BSP_ONUP_CompleteDeal()

/*****************************************************************************
* 函 数 名  : BSP_DLOAD_GetNVBackupFlag
*
* 功能描述  : 获取升级前是否进行NV备份标志
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : BSP_TRUE  :备份NV
*            BSP_FALSE :不备份NV
*
* 其它说明  : 此接口只对非一键式升级方式（SD升级/在线升级）有效，一键式升级会发AT命令设置
*
*****************************************************************************/
BSP_BOOL BSP_DLOAD_GetNVBackupFlag(BSP_VOID);
#define DRV_DLOAD_GETNVBACKUPFLAG() BSP_DLOAD_GetNVBackupFlag()

/*************************DLOAD END***********************************/

/*************************GPIO START**********************************/

typedef unsigned int GPIO_OPRT_ENUM_UINT32;

/*****************************************************************************
 函 数 名  : BSP_GPIO_Oprt
 功能描述  : at^GPIOPL,设置和查询GPIO的电平
 输入参数  : 操作类型 ulOp:
             0  设置各GPIO的PL值
             1  查询当前各GPIO的PL值

             pucPL 对应20Byte的数组,每个BYTE代表一个8个管脚的值

             设置操作时,pucPL为设置的20的Byte
             查询操作时,pucPL为当前获取到的PL的实际值组成的16进制数据
             例如用户设置at^GPIOPL = ABCDEF,则对应pucPL的数组值为{A,B,C,D,E,F,0,0,...}

 输出参数  : 无
 返 回 值  :  0 ：OK  非 0 ：Error

*****************************************************************************/
extern unsigned long BSP_GPIO_Oprt(unsigned long ulOp, unsigned char *pucPL);
#define DRV_GPIO_OPRT(ulOp,pucPL)    BSP_GPIO_Oprt(ulOp,pucPL)

/*************************GPIO END************************************/

/*************************IPCM BEGIN**********************************/

/* 处理器类型*/
typedef enum tagIPC_INT_CORE_E
{
    IPC_CORE_ARM11 = 0x0,   /* P500上的IPC */
    IPC_CORE_A9,
    IPC_CORE_CEVA,
    IPC_CORE_TENS0,
    IPC_CORE_TENS1,
    IPC_CORE_DSP,
    IPC_CORE_APPARM = 0x0,  /* V7R1/V3R2上的IPC */
    IPC_CORE_COMARM,
    IPC_CORE_LTEDSP,
    IPC_CORE_VDSP,
    IPC_CORE_ZSP,
	IPC_CORE_TDSDSP,
    IPC_CORE_DSP_GU,
    IPC_CORE_BUTTOM
}IPC_INT_CORE_E;

typedef enum tagIPC_INT_LEV_E
{
    IPC_INT_DSP_MODEM = 0,
    IPC_INT_DSP_APP = 1,
    IPC_INT_DSP_MSP = 0,
    IPC_INT_DSP_PS = 1,
    IPC_INT_MODEM_APP = 2,
    IPC_INT_DRX_APP = 3,
    IPC_INT_GUDSP_MODEM = 4,
    IPC_INT_MEDDSP_MODEM = 5,
    IPC_INT_DICC_USRDATA = 6,
    IPC_INT_DICC_RELDATA = 7,
    IPC_INT_DSP_HALT =8,  /*DSP通知ARM睡眠*/
    IPC_INT_DSP_RESUME,   /*DSP通知ARM完成唤醒后的恢复操作*/
    IPC_INT_DSP_WAKE,     /*ARM唤醒DSP*/
    IPC_INT_ARM_SLEEP = 8,
    IPC_INT_WAKE_GU =11,     /*主模唤醒从模中断*/
    IPC_INT_SLEEP_GU,     /*从模睡眠中断*/
    IPC_INT_CDRX_DSP_HALT,	/*DSP通知MSP进入CDRX流程*/
	IPC_INT_TDSDSP_HALT =14,
	IPC_INT_TDSDSP_IDLE =15,
	IPC_INT_TDSDSP_RESUME =16,
	IPC_INT_WAKE_TDS = 17,  /*唤醒TDS从模中断*/
	IPC_INT_WAKE_LTE = 18,  /*唤醒LTE从模中断*/
	IPC_INT_SLEEP_TDS = 19, /*TDS从模睡眠中断*/
	IPC_INT_TDSDSP_MSP = 20, /*用于TDS DSP 给 MSP 发中断*/
	IPC_INT_HIFI_FULL = 27,
    IPC_INT_HIFI_ON,
    IPC_INT_HIFI_OFF,
    IPC_INT_ICC_MODEM = 30,
    IPC_INT_ICC_APP,
    IPC_INT_BUTTOM
}IPC_INT_LEV_E;

typedef enum tagIPC_SEM_ID_E
{
    IPC_SEM_ICC,
    IPC_SEM_NAND,
    IPC_SEM_MEM,
    IPC_SEM_DICC,
    IPC_SEM_RFILE_LOG,          /*用于flashless可维可测*/
    IPC_SEM_SYNC = 25,
    IPC_SEM_SYSCTRL = 30,
    IPC_SEM_ZSP_HALT = 31,
    IPC_SEM_BUTTOM
}IPC_SEM_ID_E;


#define INTSRC_NUM                     32

/*****************************************************************************
* 函 数 名  : BSP_DRV_IPCIntInit
*
* 功能描述  : IPC模块初始化
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2009年3月5日   wangjing  creat
*****************************************************************************/
BSP_S32 BSP_DRV_IPCIntInit(void);


/*****************************************************************************
* 函 数 名  : DRV_IPC_SEMCREATE
*
* 功能描述  : 信号量创建函数
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
BSP_S32 BSP_IPC_SemCreate(BSP_U32 u32SignalNum);
extern BSP_S32 DRV_IPC_SEMCREATE(BSP_U32 u32SignalNum);

/*****************************************************************************
* 函 数 名  : DRV_IPC_SEMDELETE
*
* 功能描述  : 删除信号量
*
* 输入参数  :   BSP_U32 u32SignalNum 要删除的信号量编号

* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
BSP_S32 BSP_IPC_SemDelete(BSP_U32 u32SignalNum);
extern BSP_S32 DRV_IPC_SEMDELETE(BSP_U32 u32SignalNum);

/*****************************************************************************
* 函 数 名  : BSP_IPC_IntEnable
*
* 功能描述  : 使能某个中断
*
* 输入参数  :
                BSP_U32 ulLvl 要使能的中断号，取值范围0～31
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
BSP_S32 BSP_IPC_IntEnable (IPC_INT_LEV_E ulLvl);
extern BSP_S32 DRV_IPC_INTENABLE(IPC_INT_LEV_E ulLvl);

/*****************************************************************************
* 函 数 名  : DRV_IPC_INTDISABLE
*
* 功能描述  : 去使能某个中断
*
* 输入参数  :
            BSP_U32 ulLvl 要使能的中断号，取值范围0～31
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
BSP_S32 BSP_IPC_IntDisable (IPC_INT_LEV_E ulLvl);
extern BSP_S32 DRV_IPC_INTDISABLE(IPC_INT_LEV_E ulLvl);

/*****************************************************************************
* 函 数 名  : BSP_IPC_IntConnect
*
* 功能描述  : 注册某个中断
*
* 输入参数  :
           BSP_U32 ulLvl 要使能的中断号，取值范围0～31
           VOIDFUNCPTR routine 中断服务程序
*             BSP_U32 parameter      中断服务程序参数
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
BSP_S32 BSP_IPC_IntConnect  (IPC_INT_LEV_E ulLvl,VOIDFUNCPTR routine, BSP_U32 parameter);
extern BSP_S32 DRV_IPC_INTCONNECT(IPC_INT_LEV_E ulLvl,VOIDFUNCPTR routine, BSP_U32 parameter);

/*****************************************************************************
* 函 数 名  : BSP_IPC_IntDisonnect
*
* 功能描述  : 取消注册某个中断
*
* 输入参数  :
*              BSP_U32 ulLvl 要使能的中断号，取值范围0～31
*              VOIDFUNCPTR routine 中断服务程序
*             BSP_U32 parameter      中断服务程序参数
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
BSP_S32 BSP_IPC_IntDisonnect  (IPC_INT_LEV_E ulLvl,VOIDFUNCPTR routine, BSP_U32 parameter);

/*****************************************************************************
* 函 数 名  : DRV_IPC_INTSEND
*
* 功能描述  : 发送中断
*
* 输入参数  :
                IPC_INT_CORE_E enDstore 要接收中断的core
                BSP_U32 ulLvl 要发送的中断号，取值范围0～31
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
BSP_S32 BSP_IPC_IntSend(IPC_INT_CORE_E enDstCore, IPC_INT_LEV_E ulLvl);
extern BSP_S32 DRV_IPC_INTSEND(IPC_INT_CORE_E enDstCore, IPC_INT_LEV_E ulLvl);

/*****************************************************************************
* 函 数 名  : DRV_IPC_SEMTAKE
*
* 功能描述  : 获取信号量
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
BSP_S32 BSP_IPC_SemTake(BSP_U32 u32SignalNum,BSP_S32 s32timeout);
extern BSP_S32 DRV_IPC_SEMTAKE(BSP_U32 u32SignalNum, BSP_S32 s32timeout);

/*****************************************************************************
* 函 数 名  : DRV_IPC_SEMGIVE
*
* 功能描述  : 释放信号量
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
BSP_VOID BSP_IPC_SemGive(BSP_U32 u32SignalNum);
extern BSP_VOID DRV_IPC_SEMGIVE(BSP_U32 u32SignalNum);

/*****************************************************************************
* 函 数 名  : BSP_IPC_SpinLock
*
* 功能描述  : 获取信号量
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
BSP_VOID BSP_IPC_SpinLock (BSP_U32 u32SignalNum);
extern BSP_VOID DRV_SPIN_LOCK (BSP_U32 u32SignalNum);

/*****************************************************************************
* 函 数 名  : DRV_SPIN_UNLOCK
*
* 功能描述  : 释放信号量
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2011年4月11日 wangjing creat
*****************************************************************************/
BSP_VOID BSP_IPC_SpinUnLock (BSP_U32 u32SignalNum);
extern BSP_VOID DRV_SPIN_UNLOCK(BSP_U32 u32SignalNum);


BSP_S32 BSP_SGI_Connect(BSP_U32 ulLvl,VOIDFUNCPTR routine, BSP_U32 parameter);
BSP_S32 BSP_SGI_IntSend(BSP_U32 ulLvl);

/*************************IPCM END************************************/


/*************************UDI START***********************************/

/**************************************************************************
  宏定义
**************************************************************************/
#define UDI_INVALID_HANDLE     (-1)

#define UDI_CAPA_BLOCK_READ    ((BSP_U32)(1<<0)) /* 该设备的read接口为阻塞接口 */
#define UDI_CAPA_BLOCK_WRITE   ((BSP_U32)(1<<1)) /* 该设备的write接口为阻塞接口 */
#define UDI_CAPA_READ_CB       ((BSP_U32)(1<<2)) /* 该设备支持read 回调函数 */
#define UDI_CAPA_WRITE_CB      ((BSP_U32)(1<<3)) /* 该设备支持write 回调函数 */
#define UDI_CAPA_BUFFER_LIST   ((BSP_U32)(1<<4)) /* 该设备的读写buffer为内存链表结构(默认为普通内存块) */
#define UDI_CAPA_CTRL_OPT      ((BSP_U32)(1<<5)) /* 该设备支持控制操作 */

#define MIN_UDI_HANDLE     0x5a0000
#define MAX_UDI_HANDLE     0x5a003f

/**************************************************************************
  结构定义
**************************************************************************/
#define UDI_BUILD_DEV_ID(dev, type) (((BSP_U32)(dev) << 8) | ((BSP_U32)(type) & 0x00ff))
#define UDI_BUILD_CMD_ID(dev, cmd) (((BSP_U32)(dev) << 8) | ((BSP_U32)(cmd) & 0xffff))


/* 主设备定义 */
typedef enum tagUDI_DEVICE_MAIN_ID
{
    UDI_DEV_USB_ACM = 0,
    UDI_DEV_USB_NCM,
    UDI_DEV_ICC,
    UDI_DEV_UART,

    UDI_DEV_MAX                 /* 必须在最后, 用于边界值 */
}UDI_DEVICE_MAIN_ID;

/* 各设备类型定义(要和 usb 多设备形态统一起来) */
/* ACM */
typedef enum tagUDI_ACM_DEV_TYPE
{
    UDI_USB_ACM_CTRL,
    UDI_USB_ACM_AT,
    UDI_USB_ACM_SHELL,
    UDI_USB_ACM_LTE_DIAG,
    UDI_USB_ACM_OM,
    UDI_USB_ACM_MODEM,
    UDI_USB_ACM_GPS,      /*HISO*/
    UDI_USB_ACM_3G_GPS,   /*ashell*/
    UDI_USB_ACM_3G_PCVOICE, /*预留*/
    UDI_USB_ACM_PCVOICE,    /*预留*/
    /*UDI_USB_ACM_3G_AT,    //3G AT*/
    /*UDI_USB_ACM_3G_MODEM, //3G PPP*/
    UDI_USB_HSIC_ACM0,
    UDI_USB_HSIC_ACM1,
    UDI_USB_HSIC_ACM2,
    UDI_USB_HSIC_ACM3,
    UDI_USB_HSIC_ACM4,
    UDI_USB_HSIC_ACM5,
    UDI_USB_HSIC_ACM6,
    UDI_USB_HSIC_ACM7,
    UDI_USB_HSIC_ACM8,
    UDI_USB_HSIC_ACM9,
    UDI_USB_HSIC_ACM10,
    UDI_USB_HSIC_ACM11,
    UDI_USB_HSIC_ACM12,
    UDI_USB_HSIC_ACM13,
	UDI_USB_HSIC_ACM14,
    UDI_USB_ACM_MAX                /* 必须在最后, 用于边界值 */
}UDI_ACM_DEV_TYPE;

/* NCM */
typedef enum tagUDI_NCM_DEV_TYPE
{
    UDI_USB_NCM_NDIS,
    UDI_USB_NCM_CTRL,
    UDI_USB_HSIC_NCM0,
    UDI_USB_HSIC_NCM1,
    UDI_USB_HSIC_NCM2,

    UDI_USB_NCM_MAX                /* 必须在最后, 用于边界值 */
}UDI_NCM_DEV_TYPE;

/* ICC */
typedef enum tagUDI_ICC_DEV_TYPE
{
    UDI_ICC_GUOM0 = 16,
    UDI_ICC_GUOM1,
    UDI_ICC_GUOM2,
    UDI_ICC_GUOM3,
    UDI_ICC_GUOM4,
    UDI_ICC_GUOM5,

    UDI_ICC_MAX                /* 必须在最后, 用于边界值 */
}UDI_ICC_DEV_TYPE;


/* 设备ID号定义 */
typedef enum tagUDI_DEVICE_ID
{
    /* USB ACM */
    UDI_ACM_CTRL_ID =  UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_CTRL),
    UDI_ACM_AT_ID =    UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_AT),
    UDI_ACM_SHELL_ID = UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_SHELL),
    UDI_ACM_LTE_DIAG_ID = UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_LTE_DIAG),
    UDI_ACM_OM_ID = UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_OM),
    UDI_ACM_MODEM_ID = UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_MODEM),
    UDI_ACM_GPS_ID      = UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_GPS),     /*HISO*/
    UDI_ACM_3G_GPS_ID   = UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_3G_GPS),  /*ashell*/
    UDI_ACM_3G_PCVOICE_ID = UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_3G_PCVOICE), /*预留*/
    UDI_ACM_PCVOICE_ID    = UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_PCVOICE),    /*预留*/
    /*UDI_ACM_3G_AT_ID      = UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_3G_AT),    //3G AT*/
    /*UDI_ACM_3G_MODEM_ID   = UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_ACM_3G_MODEM), //3G PPP*/
    UDI_ACM_HSIC_ACM0_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM0),
    UDI_ACM_HSIC_ACM1_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM1),
    UDI_ACM_HSIC_ACM2_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM2),
    UDI_ACM_HSIC_ACM3_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM3),
    UDI_ACM_HSIC_ACM4_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM4),
    UDI_ACM_HSIC_ACM5_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM5),
    UDI_ACM_HSIC_ACM6_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM6),
    UDI_ACM_HSIC_ACM7_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM7),
    UDI_ACM_HSIC_ACM8_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM8),
    UDI_ACM_HSIC_ACM9_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM9),
    UDI_ACM_HSIC_ACM10_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM10),
    UDI_ACM_HSIC_ACM11_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM11),
    UDI_ACM_HSIC_ACM12_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM12),
    UDI_ACM_HSIC_ACM13_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM13),
	UDI_ACM_HSIC_ACM14_ID=UDI_BUILD_DEV_ID(UDI_DEV_USB_ACM, UDI_USB_HSIC_ACM14),

    /* USB NCM */
    UDI_NCM_NDIS_ID =  UDI_BUILD_DEV_ID(UDI_DEV_USB_NCM, UDI_USB_NCM_NDIS),
    UDI_NCM_CTRL_ID =  UDI_BUILD_DEV_ID(UDI_DEV_USB_NCM, UDI_USB_NCM_CTRL),
    UDI_NCM_HSIC_NCM0_ID =  UDI_BUILD_DEV_ID(UDI_DEV_USB_NCM, UDI_USB_HSIC_NCM0),
    UDI_NCM_HSIC_NCM1_ID =  UDI_BUILD_DEV_ID(UDI_DEV_USB_NCM, UDI_USB_HSIC_NCM1),
    UDI_NCM_HSIC_NCM2_ID =  UDI_BUILD_DEV_ID(UDI_DEV_USB_NCM, UDI_USB_HSIC_NCM2),

    /* UART */
    UDI_UART_0_ID =  UDI_BUILD_DEV_ID(UDI_DEV_UART, 0),
    UDI_UART_1_ID =  UDI_BUILD_DEV_ID(UDI_DEV_UART, 1),

    /* ICC */
    UDI_ICC_IFC_ID  =  UDI_BUILD_DEV_ID(UDI_DEV_ICC, 0),
    UDI_ICC_IPM_ID  =  UDI_BUILD_DEV_ID(UDI_DEV_ICC, 1),
    UDI_ICC_LMSP_ID =  UDI_BUILD_DEV_ID(UDI_DEV_ICC, 2),

    UDI_ICC_GUOM0_ID =  UDI_BUILD_DEV_ID(UDI_DEV_ICC, UDI_ICC_GUOM0),
    UDI_ICC_GUOM1_ID =  UDI_BUILD_DEV_ID(UDI_DEV_ICC, UDI_ICC_GUOM1),
    UDI_ICC_GUOM2_ID =  UDI_BUILD_DEV_ID(UDI_DEV_ICC, UDI_ICC_GUOM2),
    UDI_ICC_GUOM3_ID =  UDI_BUILD_DEV_ID(UDI_DEV_ICC, UDI_ICC_GUOM3),
    UDI_ICC_GUOM4_ID =  UDI_BUILD_DEV_ID(UDI_DEV_ICC, UDI_ICC_GUOM4),
    UDI_ICC_GUOM5_ID =  UDI_BUILD_DEV_ID(UDI_DEV_ICC, UDI_ICC_GUOM5),

    /* MUST BE LAST */
    UDI_INVAL_DEV_ID = 0xFFFF
} UDI_DEVICE_ID;


/* 设备的打开参数 */
typedef struct tagUDI_OPEN_PARAM
{
    UDI_DEVICE_ID devid;                        /* 设备ID */
    void   *pPrivate;                            /* 模块特有的数据 */
} UDI_OPEN_PARAM;

/* IOCTL 命令码,需要的命令码在此添加 */
typedef enum tagUDI_IOCTL_CMD_TYPE
{
    UDI_IOCTL_SET_WRITE_CB = 0xF001,            /* 设置一个起始码值防止与系统定义冲突 */
    UDI_IOCTL_SET_READ_CB,

    UDI_IOCTL_INVAL_CMD = 0xFFFFFFFF
} UDI_IOCTL_CMD_TYPE;

/**************************************************************************
  函数声明
**************************************************************************/
/*****************************************************************************
* 函 数 名  : udi_get_capability
*
* 功能描述  : 根据设备ID获取当前设备支持的特性
*
* 输入参数  : devId: 设备ID
* 输出参数  : 无
* 返 回 值  : 支持的特性值
*****************************************************************************/
BSP_S32 udi_get_capability(UDI_DEVICE_ID devId);

/*****************************************************************************
* 函 数 名  : udi_open
*
* 功能描述  : 打开设备(数据通道)
*
* 输入参数  : pParam: 设备的打开配置参数
* 输出参数  : 无
* 返 回 值  : -1:失败 / 其他:成功
*****************************************************************************/
UDI_HANDLE udi_open(UDI_OPEN_PARAM *pParam);
#define DRV_UDI_OPEN(pParam)    udi_open(pParam)

/*****************************************************************************
* 函 数 名  : udi_close
*
* 功能描述  : 关闭设备(数据通道)
*
* 输入参数  : handle: 设备的handle
* 输出参数  : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
BSP_S32 udi_close(UDI_HANDLE handle);
#define DRV_UDI_CLOSE(handle)    udi_close(handle)

/*****************************************************************************
* 函 数 名  : udi_write
*
* 功能描述  : 数据写
*
* 输入参数  : handle:  设备的handle
*             pMemObj: buffer内存 或 内存链表对象
*             u32Size: 数据写尺寸 或 内存链表对象可不设置
* 输出参数  :
*
* 返 回 值  : 完成字节数 或 成功/失败
*****************************************************************************/
BSP_S32 udi_write(UDI_HANDLE handle, void* pMemObj, BSP_U32 u32Size);
#define  DRV_UDI_WRITE(handle,pMemObj,u32Size)     udi_write (handle,pMemObj,u32Size)

/*****************************************************************************
* 函 数 名  : udi_read
*
* 功能描述  : 数据读
*
* 输入参数  : handle:  设备的handle
*             pMemObj: buffer内存 或 内存链表对象
*             u32Size: 数据读尺寸 或 内存链表对象可不设置
* 输出参数  :
*
* 返 回 值  : 完成字节数 或 成功/失败
*****************************************************************************/
BSP_S32 udi_read(UDI_HANDLE handle, void* pMemObj, BSP_U32 u32Size);
#define DRV_UDI_READ(handle,pMemObj,u32Size)  udi_read(handle,pMemObj,u32Size)

/*****************************************************************************
* 函 数 名  : udi_ioctl
*
* 功能描述  : 数据通道属性配置
*
* 输入参数  : handle: 设备的handle
*             u32Cmd: IOCTL命令码
*             pParam: 操作参数
* 输出参数  :
*
* 返 回 值  : 成功/失败
*****************************************************************************/
BSP_S32 udi_ioctl(UDI_HANDLE handle, BSP_U32 u32Cmd, BSP_VOID* pParam);
#define DRV_UDI_IOCTL(handle,u32Cmd,pParam)    udi_ioctl(handle,u32Cmd,pParam)

/*************************UDI END*************************************/

/*************************MEMORY START********************************/

/**************************************************************************
  宏定义
**************************************************************************/
/* 内存池类型, Flags标记用 */
typedef enum tagMEM_POOL_TYPE
{
    MEM_NORM_DDR_POOL = 0,
    MEM_ICC_DDR_POOL,
    MEM_ICC_AXI_POOL,
    MEM_POOL_MAX
}MEM_POOL_TYPE;

/**************************************************************************
  接口声明
**************************************************************************/
BSP_VOID* BSP_Malloc(BSP_U32 u32Size, MEM_POOL_TYPE enFlags);
BSP_VOID* BSP_MallocDbg(BSP_U32 u32Size, MEM_POOL_TYPE enFlags, BSP_U8* pFileName, BSP_U32 u32Line);
BSP_VOID  BSP_Free(BSP_VOID* pMem);
BSP_VOID  BSP_FreeDbg(BSP_VOID* pMem, BSP_U8* pFileName, BSP_U32 u32Line);

/*****************************************************************************
* 函 数 名  : BSP_MALLOC
*
* 功能描述  : BSP 动态内存分配
*
* 输入参数  : sz: 分配的大小(byte)
*             flags: 内存属性(暂不使用,预留)
* 输出参数  : 无
* 返 回 值  : 分配出来的内存指针
*****************************************************************************/
#ifdef __BSP_DEBUG__
#define BSP_MALLOC(sz, flags) BSP_MallocDbg(sz, flags, __FILE__, __LINE__)
#else
#define BSP_MALLOC(sz, flags) BSP_Malloc(sz, flags)
#endif

/*****************************************************************************
* 函 数 名  : BSP_FREE
*
* 功能描述  : BSP 动态内存释放
*
* 输入参数  : ptr: 动态内存指针
* 输出参数  : 无
* 返 回 值  : 无
*****************************************************************************/
#ifdef __BSP_DEBUG__
#define BSP_FREE(ptr) BSP_FreeDbg(ptr, __FILE__, __LINE__)
#else
#define BSP_FREE(ptr) BSP_Free(ptr)
#endif



/*****************************************************************************
* 函 数 名  : BSP_SFree
*
* 功能描述  : BSP 动态内存释放(加spin lock保护,多核场景使用)
*
* 输入参数  : pMem: 动态内存指针
* 输出参数  : 无
* 返 回 值  : 无
*****************************************************************************/
BSP_VOID  BSP_SFree(BSP_VOID* pMem);
/*****************************************************************************
* 函 数 名  : BSP_SMalloc
*
* 功能描述  : BSP 动态内存分配(加spin lock保护,多核场景使用)
*
* 输入参数  : u32Size: 分配的大小(byte)
*             enFlags: 内存属性(暂不使用,预留)
* 输出参数  : 无
* 返 回 值  : 分配出来的内存指针
*****************************************************************************/
BSP_VOID* BSP_SMalloc(BSP_U32 u32Size, MEM_POOL_TYPE enFlags);


#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
extern BSP_VOID* cacheDmaMalloc(BSP_U32  bytes);
#define DRV_CACHEDMAM_ALLOC(bytes) cacheDmaMalloc(bytes)
#else
#define DRV_CACHEDMAM_ALLOC(bytes) kmalloc(bytes, GFP_KERNEL)
#endif

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
extern BSP_S32 cacheDmaFree(BSP_VOID*  pBuf);
#define DRV_CACHEDMAM_FREE(pBuf) cacheDmaFree(pBuf)
#else
#define DRV_CACHEDMAM_FREE(pBuf) kfree(pBuf)
#endif

/************************************************************************
 * FUNCTION
 *       vmEnable
 * DESCRIPTION
 *       内存读写保护功能使能函数
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
 *************************************************************************/
extern int vmEnable(int enable);
#define DRV_VM_ENABLE(flag)   vmEnable(flag)

/*****************************************************************************
 函 数 名  : vmStateSet
 功能描述  : 设置地址空间状态
 输入参数  : 无

 输出参数  : 无。
 返 回 值  : 无
 注意事项  ：
*****************************************************************************/
#define DRV_VM_STATESET(context, virtAdrs, len, stateMask, state) vmStateSet(context, virtAdrs, len, stateMask, state)

/*****************************************************************************
 函 数 名  : BSP_CACHE_DATA_FLUSH
 功能描述  :
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/
extern void BSP_CACHE_DATA_FLUSH(void * addr, int size);

/*************************MEMORY END**********************************/

/*************************SOCP START**********************************/

/**************************************************************************
  宏定义
**************************************************************************/
#define SOCP_CODER_SRC_CHAN                 0x00
#define SOCP_CODER_DEST_CHAN                0x01
#define SOCP_DECODER_SRC_CHAN               0x02
#define SOCP_DECODER_DEST_CHAN              0x03

#define SOCP_MAX_ENCSRC_BACKNUM                 (1)
#define SOCP_MAX_ENCDST_BACKNUM                 (1)


#define SOCP_CHAN_DEF(chan_type, chan_id)   ((chan_type<<16)|chan_id)
#define SOCP_REAL_CHAN_ID(unique_chan_id)   (unique_chan_id & 0xFFFF)
#define SOCP_REAL_CHAN_TYPE(unique_chan_id) (unique_chan_id>>16)

#define SOCP_CODER_DEST_CHAN_0      SOCP_CHAN_DEF(SOCP_CODER_DEST_CHAN, 0)
#define SOCP_CODER_DEST_CHAN_1      SOCP_CHAN_DEF(SOCP_CODER_DEST_CHAN, 1)
#define SOCP_CODER_DEST_CHAN_2      SOCP_CHAN_DEF(SOCP_CODER_DEST_CHAN, 2)
#define SOCP_CODER_DEST_CHAN_3      SOCP_CHAN_DEF(SOCP_CODER_DEST_CHAN, 3)
#define SOCP_CODER_DEST_CHAN_4      SOCP_CHAN_DEF(SOCP_CODER_DEST_CHAN, 4)
#define SOCP_CODER_DEST_CHAN_5      SOCP_CHAN_DEF(SOCP_CODER_DEST_CHAN, 5)
#define SOCP_CODER_DEST_CHAN_6      SOCP_CHAN_DEF(SOCP_CODER_DEST_CHAN, 6)

#define SOCP_DECODER_SRC_CHAN_0     SOCP_CHAN_DEF(SOCP_DECODER_SRC_CHAN, 0)
#define SOCP_DECODER_SRC_CHAN_1     SOCP_CHAN_DEF(SOCP_DECODER_SRC_CHAN, 1)
#define SOCP_DECODER_SRC_CHAN_2     SOCP_CHAN_DEF(SOCP_DECODER_SRC_CHAN, 2)
#define SOCP_DECODER_SRC_CHAN_3     SOCP_CHAN_DEF(SOCP_DECODER_SRC_CHAN, 3)

#define SOCP_CODER_SRC_CHAN_BASE            0x00000000
#define SOCP_CODER_DEST_CHAN_BASE           0x00010000
#define SOCP_DECODER_SRC_CHAN_BASE          0x00020000
#define SOCP_DECODER_DEST_CHAN_BASE         0x00030000

/**************************************************************************
  错误码定义
**************************************************************************/
#define BSP_ERR_SOCP_BASE            BSP_DEF_ERR(BSP_MODU_SOCP, 0)
#define BSP_ERR_SOCP_NULL            (BSP_ERR_SOCP_BASE + 0x1)
#define BSP_ERR_SOCP_NOT_INIT        (BSP_ERR_SOCP_BASE + 0x2)
#define BSP_ERR_SOCP_MEM_ALLOC       (BSP_ERR_SOCP_BASE + 0x3)
#define BSP_ERR_SOCP_SEM_CREATE      (BSP_ERR_SOCP_BASE + 0x4)
#define BSP_ERR_SOCP_TSK_CREATE      (BSP_ERR_SOCP_BASE + 0x5)
#define BSP_ERR_SOCP_INVALID_CHAN    (BSP_ERR_SOCP_BASE + 0x6)
#define BSP_ERR_SOCP_INVALID_PARA    (BSP_ERR_SOCP_BASE + 0x7)
#define BSP_ERR_SOCP_NO_CHAN         (BSP_ERR_SOCP_BASE + 0x8)
#define BSP_ERR_SOCP_SET_FAIL        (BSP_ERR_SOCP_BASE + 0x9)
#define BSP_ERR_SOCP_TIMEOUT         (BSP_ERR_SOCP_BASE + 0xa)
#define BSP_ERR_SOCP_NOT_8BYTESALIGN (BSP_ERR_SOCP_BASE + 0xb)
#define BSP_ERR_SOCP_CHAN_RUNNING    (BSP_ERR_SOCP_BASE + 0xc)
#define BSP_ERR_SOCP_CHAN_MODE       (BSP_ERR_SOCP_BASE + 0xd)
#define BSP_ERR_SOCP_DEST_CHAN       (BSP_ERR_SOCP_BASE + 0xe)
#define BSP_ERR_SOCP_DECSRC_SET      (BSP_ERR_SOCP_BASE + 0xf)
/**************************************************************************
  结构定义
**************************************************************************/


typedef enum tagSOCP_EVENT_E
{
    SOCP_EVENT_PKT_HEADER_ERROR         = 0x1,    /* 包头检测错误，"HISI" */
    SOCP_EVENT_OUTBUFFER_OVERFLOW       = 0x2,    /* 目的buffer上溢 */
    SOCP_EVENT_RDBUFFER_OVERFLOW        = 0x4,    /* RDbuffer上溢 */
    SOCP_EVENT_DECODER_UNDERFLOW        = 0x8,    /* 解码源buffer下溢 */
    SOCP_EVENT_PKT_LENGTH_ERROR         = 0x10,   /* 解码包长度检测错误 */
    SOCP_EVENT_CRC_ERROR                = 0x20,   /* 解码CRC校验错误 */
    SOCP_EVENT_DATA_TYPE_ERROR          = 0x40,   /* 解码数据类型错误*/
    SOCP_EVENT_HDLC_HEADER_ERROR        = 0x80,   /* 解码HDLC检测错误 */
    SOCP_EVENT_BUTT
}SOCP_EVENT_E;

/* 编码源的数据结构模式 */
typedef enum tagSOCP_ENCSRC_CHNMODE_E
{
    SOCP_ENCSRC_CHNMODE_CTSPACKET       = 0,    /* 连续数据包环形缓冲区 */
    SOCP_ENCSRC_CHNMODE_FIXPACKET,              /* 固定长度数据包环形缓冲区 */
    SOCP_ENCSRC_CHNMODE_LIST,                   /* 链式环形缓冲区 */
    SOCP_ENCSRC_CHNMODE_BUTT
}SOCP_ENCSRC_CHNMODE_E;

/* 解码源的数据结构模式 */
typedef enum tagSOCP_DECSRC_CHNMODE_E
{
    SOCP_DECSRC_CHNMODE_BYTES        = 0,       /* 连续字节环形缓冲区 */
    SOCP_DECSRC_CHNMODE_LIST,                   /* 链式环形缓冲区 */
    SOCP_DECSRC_CHNMODE_BUTT
}SOCP_DECSRC_CHNMODE_E;

/* 超时选择及使能数据结构体 */
typedef enum tagSOCP_TIMEOUT_EN_E
{
    SOCP_TIMEOUT_BUFOVF_DISABLE        = 0,       /* buffer溢出，不上报中断 */
    SOCP_TIMEOUT_BUFOVF_ENABLE,                   /* buffer溢出，超时计数上报中断 */
    SOCP_TIMEOUT_TRF,                             /* 传输中断超时计数 */
    SOCP_TIMEOUT_BUTT
}SOCP_TIMEOUT_EN_E;

/* 同一类型的通道不同优先级枚举值*/
typedef enum tagSOCP_CHAN_PRIORITY_E
{
    SOCP_CHAN_PRIORITY_0     = 0,               /* 最低优先级*/
    SOCP_CHAN_PRIORITY_1,                       /* 次低优先级*/
    SOCP_CHAN_PRIORITY_2,                       /* 次高优先级*/
    SOCP_CHAN_PRIORITY_3,                       /* 最高优先级*/
    SOCP_CHAN_PRIORITY_BUTT
}SOCP_CHAN_PRIORITY_E;

/* 数据类型枚举值*/
typedef enum tagSOCP_DATA_TYPE_E
{
    SOCP_DATA_TYPE_0            = 0,            /* LTE OAM数据 */
    SOCP_DATA_TYPE_1,                           /* GU OAM数据 */
    SOCP_DATA_TYPE_2,                           /* 保留 */
    SOCP_DATA_TYPE_3,                           /* 保留 */
    SOCP_DATA_TYPE_BUTT
}SOCP_DATA_TYPE_E;


/* BBP 数采模式，数据丢弃或覆盖 */
typedef enum tagSOCP_BBP_DS_MODE_E
{
    SOCP_BBP_DS_MODE_DROP           = 0,        /* 数据丢弃 */
    SOCP_BBP_DS_MODE_OVERRIDE,                  /* 数据覆盖 */
    SOCP_BBP_DS_MODE_BUTT
}SOCP_BBP_DS_MODE_E;

/* 编码源通道data type 使能位 */
typedef enum tagSOCP_DATA_TYPE_EN_E
{
    SOCP_DATA_TYPE_EN           = 0,        /* data type 使能，默认值 */
    SOCP_DATA_TYPE_DIS,                     /* data type 不使能 */
    SOCP_DATA_TYPE_EN_BUTT
}SOCP_DATA_TYPE_EN_E;

/* 编码源通道debug 使能位 */
typedef enum tagSOCP_ENC_DEBUG_EN_E
{
    SOCP_ENC_DEBUG_DIS          = 0,       /* debug 不使能，默认值 */
    SOCP_ENC_DEBUG_EN,                     /* debug 使能 */
    SOCP_ENC_DEBUG_EN_BUTT
}SOCP_ENC_DEBUG_EN_E;

/* 解码通路包长配置结构体 */
typedef struct tagSOCP_DEC_PKTLGTH_S
{
    BSP_U32                 u32PktMax;         /*包长度最大值*/
    BSP_U32                 u32PktMin;         /*包长度最小值*/
}SOCP_DEC_PKTLGTH_S;


/* 通用源通道buffer结构体定义*/
typedef struct tagSOCP_SRC_SETBUF_S
{
    BSP_U32                 u32InputStart;      /* 输入通道起始地址*/
    BSP_U32                 u32InputEnd;        /* 输入通道结束地址*/
    BSP_U32                 u32RDStart;         /* RD buffer起始地址*/
    BSP_U32                 u32RDEnd;           /* RD buffer结束地址*/
    BSP_U32                 u32RDThreshold;     /* RD buffer数据上报阈值*/
}SOCP_SRC_SETBUF_S;

/* 通用目的通道buffer结构体定义*/
typedef struct tagSOCP_DST_SETBUF_S
{
    BSP_U32                 u32OutputStart;     /* 输出通道起始地址*/
    BSP_U32                 u32OutputEnd;       /* 输出通道结束地址*/
    BSP_U32                 u32Threshold;       /* 输出通道门限值 */
}SOCP_DST_SETBUF_S;

/* 编码源通道结构体定义*/
typedef struct tagSOCP_CODER_SRC_CHAN_S
{
    BSP_U32                 u32DestChanID;      /* 目标通道ID*/
    BSP_U32                 u32BypassEn;        /* 通道bypass使能*/
    SOCP_DATA_TYPE_E        eDataType;          /* 数据类型，指明数据封装协议，用于复用多平台*/
#if defined (CHIP_BB_6920CS)
    SOCP_DATA_TYPE_EN_E     eDataTypeEn;        /* 数据类型使能位*/
    SOCP_ENC_DEBUG_EN_E     eDebugEn;           /* 调试位使能*/
#endif
    SOCP_ENCSRC_CHNMODE_E   eMode;              /* 通道数据模式*/
    SOCP_CHAN_PRIORITY_E    ePriority;          /* 通道优先级*/
    SOCP_SRC_SETBUF_S       sCoderSetSrcBuf;
}SOCP_CODER_SRC_CHAN_S;

/* 编码目的配置结构体定义*/
typedef struct tagSOCP_CODER_DEST_CHAN_S
{
#if defined (CHIP_BB_6920CS)
    BSP_U32                 u32EncDstThrh;     /* 编码目的通道阈值门限，仲裁通道时使用*/
#endif
    SOCP_DST_SETBUF_S       sCoderSetDstBuf;
}SOCP_CODER_DEST_CHAN_S;

/* 解码源通道结构体定义*/
typedef struct tagSOCP_DECODER_SRC_CHAN_S
{
#if defined (CHIP_BB_6920CS)
    SOCP_DATA_TYPE_EN_E     eDataTypeEn;        /* 数据类型使能位*/
#endif
    SOCP_DECSRC_CHNMODE_E   eMode;              /* 通道模式*/
    SOCP_SRC_SETBUF_S       sDecoderSetSrcBuf;
}SOCP_DECODER_SRC_CHAN_S;

/* 解码目的通道结构体定义*/
typedef struct tagSOCP_DECODER_DEST_CHAN_S
{
    BSP_U32                 u32SrcChanID;       /* 数据来源通道ID*/
    SOCP_DATA_TYPE_E        eDataType;          /* 数据类型，指明数据封装协议，用于复用多平台*/
    SOCP_DST_SETBUF_S       sDecoderDstSetBuf;
}SOCP_DECODER_DEST_CHAN_S;

/* 通用buffer描述结构体定义*/
typedef struct tagSOCP_BUFFER_RW_S
{
    BSP_CHAR   *pBuffer;                        /* buffer指针*/
    BSP_U32     u32Size;                        /* 可用buffer大小*/
    BSP_CHAR   *pRbBuffer;                      /* 回卷buffer指针*/
    BSP_U32     u32RbSize;                      /* 回卷buffer大小*/
}SOCP_BUFFER_RW_S;

/* 解码源通道错误计数结构体定义*/
typedef struct tagSOCP_DECODER_ERROR_CNT_S
{
    BSP_U32     u32PktlengthCnt;                /* 包长检测错误计数*/
    BSP_U32     u32CrcCnt;                      /* CRC校验错误计数*/
    BSP_U32     u32DataTypeCnt;                 /* 解码数据类型检验错误计数*/
    BSP_U32     u32HdlcHeaderCnt;               /* 0x7E校验错误*/
}SOCP_DECODER_ERROR_CNT_S;

/* 解码通路包长配置结构体 */
typedef struct tagSOCP_ENCSRC_RSVCHN_SCOPE_S
{
    BSP_U32                 u32RsvIDMin;        /*编码源保留通道ID最小值*/
    BSP_U32                 u32RsvIDMax;        /*编码源保留通道ID最大值*/
}SOCP_ENCSRC_RSVCHN_SCOPE_S;

#define SOCP_OM_CHANNEL_CNT    3
/*******************************************************************************
 结构名    : SOCP_LOG_EXC_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : LOG2.0 OM数据复位信息保存
 特殊说明:   在双核共享内存分配了该结构长度的空间
             改动该结构时请确保同步更新excdrv.h中EXCH_A_C_SOCP_LOG_SIZE
*******************************************************************************/
typedef struct
{
    UINT32 logOnFlag;    
    UINT32 dataBaseAddr[SOCP_OM_CHANNEL_CNT];
    UINT32 dataLen[SOCP_OM_CHANNEL_CNT];
    UINT32 dataUnreelAddr[SOCP_OM_CHANNEL_CNT];
    UINT32 dataUnreelLen[SOCP_OM_CHANNEL_CNT]; 
} SOCP_LOG_EXC_INFO_STRU;

typedef struct SOCP_ENC_DST_BUF_LOG_CFG
{
    BSP_U32     guWaterMark;    /* SOCP编码目的通道数据传输水线，GU通道 */
    BSP_U32     lWaterMark;     /* SOCP编码目的通道数据传输水线，L通道 */
    BSP_U32     overTime;       /* SOCP编码目的通道数据传输超时时间 */
    BSP_U32     flushFlag;      /* SOCP编码目的通道数据立即输出标志 */
    BSP_U32     logOnFlag;      /* LOG2.0功能启动标志 */
} SOCP_ENC_DST_BUF_LOG_CFG_STRU;

typedef BSP_S32 (*socp_event_cb)(BSP_U32 u32ChanID, SOCP_EVENT_E u32Event, BSP_U32 u32Param);
typedef BSP_S32 (*socp_read_cb)(BSP_U32 u32ChanID);
typedef BSP_S32 (*socp_rd_cb)(BSP_U32 u32ChanID);

/**************************************************************************
  函数声明
**************************************************************************/
/*****************************************************************************
 函 数 名  : BSP_SOCP_SleepIn
 功能描述  : SOCP进入睡眠
 输入参数  : pu32SrcChanID:编码源通道ID列表
             uSrcChanNum:  编码源通道个数
             pu32DstChanID:编码目的通道ID列表
             uSrcChanNum:  编码目的通道个数
 输出参数  : 无。
 返 回 值  : SOCP_OK:进入睡眠成功。
             其他:   进入睡眠失败
*****************************************************************************/
extern BSP_U32 DRV_SOCP_SLEEPIN(BSP_U32 *pu32SrcChanID, BSP_U32 uSrcChanNum, BSP_U32 *pu32DstChanID, BSP_U32 uDstChanNum);

/*****************************************************************************
 函 数 名  : DRV_SOCP_SLEEPOUT
 功能描述  : SOCP退出睡眠
 输入参数  : pu32SrcChanID:编码源通道ID列表
             uSrcChanNum:  编码源通道个数
             pu32DstChanID:编码目的通道ID列表
             uSrcChanNum:  编码目的通道个数
 输出参数  : 无。
 返 回 值  : SOCP_OK:进入睡眠成功。
             其他:   进入睡眠失败
*****************************************************************************/
extern BSP_U32 DRV_SOCP_SLEEPOUT(BSP_U32 *pu32SrcChanID, BSP_U32 uSrcChanNum, BSP_U32 *pu32DstChanID, BSP_U32 uDstChanNum);

/*****************************************************************************
 函 数 名  : BSP_SOCP_CoderAllocSrcChan
 功能描述  : 此接口完成SOCP编码器源通道的分配，根据编码器源通道参数设置通道属性，连接目标通道，返回函数执行结果。
 输入参数  : pSrcAttr:编码器源通道参数结构体指针。
             pSrcChanID:申请到的源通道ID。
 输出参数  : 无。
 返 回 值  : SOCP_OK:编码源通道分配成功。
             SOCP_ERROR:编码源通道分配失败。
*****************************************************************************/
extern BSP_S32 BSP_SOCP_CoderAllocSrcChan(SOCP_CODER_SRC_CHAN_S *pSrcAttr, BSP_U32 *pSrcChanID);
#define DRV_SOCP_CoderAllocSrcChan(pSrcAttr, pSrcChanID)    BSP_SOCP_CoderAllocSrcChan(pSrcAttr, pSrcChanID)

/*****************************************************************************
 函 数 名  : BSP_SOCP_CoderSetDestChanAttr
 功能描述  : 此接口完成某一编码目标通道的配置，返回函数执行的结果。
 输入参数  : u32DestChanID:SOCP编码器的目标通道ID。
             pDestAttr:SOCP编码器目标通道参数结构体指针。
 输出参数  : 无。
 返 回 值  : SOCP_OK:编码目的通道设置成功。
             SOCP_ERROR:编码目的通道设置失败。
*****************************************************************************/
extern BSP_S32 BSP_SOCP_CoderSetDestChanAttr(BSP_U32 u32DestChanID, SOCP_CODER_DEST_CHAN_S *pDestAttr);
#define DRV_SOCP_CoderSetDestChanAttr(u32DestChanID, pDestAttr)    BSP_SOCP_CoderSetDestChanAttr(u32DestChanID, pDestAttr)

/*****************************************************************************
 函 数 名      : BSP_SOCP_DecoderAllocDestChan
 功能描述  :此接口完成SOCP解码器目标通道的分配，
                根据解码目标通道参数设置通道属性，
                并连接源通道，返回函数执行结果。
 输入参数  : pAttr:解码器目标通道参数结构体指针
                         pDestChanID:申请到的目标通道ID
 输出参数  : 无。
 返 回 值      : SOCP_OK:解码目的通道分配成功。
                             SOCP_ERROR:解码目的通道设置失败。
*****************************************************************************/
extern BSP_S32 BSP_SOCP_DecoderAllocDestChan (SOCP_DECODER_DEST_CHAN_S *pAttr, BSP_U32 *pDestChanID);
#define  DRV_SOCP_DecoderAllocDestChan(pAttr ,pDestChanID)  BSP_SOCP_DecoderAllocDestChan(pAttr ,pDestChanID)

/*****************************************************************************
 函 数 名      : BSP_SOCP_DecoderSetSrcChanAttr
 功能描述  :此接口完成某一解码源通道的配置，返回函数执行的结果。
 输入参数  : u32SrcChanID:解码器源通道ID
                         pInputAttr:解码器源通道参数结构体指针
 输出参数  : 无。
 返 回 值      : SOCP_OK:解码源通道设置成功
                             SOCP_ERROR:解码源通道设置失败
*****************************************************************************/
extern BSP_S32 BSP_SOCP_DecoderSetSrcChanAttr ( BSP_U32 u32SrcChanID,SOCP_DECODER_SRC_CHAN_S *pInputAttr);
#define  DRV_SOCP_DecoderSetSrcChanAttr(u32SrcChanID, pInputAttr)   BSP_SOCP_DecoderSetSrcChanAttr(u32SrcChanID, pInputAttr)

/*****************************************************************************
 函 数 名      : BSP_SOCP_DecoderGetErrCnt
 功能描述  :此接口给出解码通道中四种异常情况的计数值。
 输入参数  : u32ChanID:解码器通道ID
                         pErrCnt:解码器异常计数结构体指针
 输出参数  : 无。
 返 回 值      : SOCP_OK:返回异常计数成功
                             SOCP_ERROR:返回异常计数失败
*****************************************************************************/
extern  BSP_S32 BSP_SOCP_DecoderGetErrCnt (BSP_U32 u32ChanID, SOCP_DECODER_ERROR_CNT_S *pErrCnt);
#define  DRV_SOCP_DecoderGetErrCnt(u32ChanID, pErrCnt)   BSP_SOCP_DecoderGetErrCnt ( u32ChanID, pErrCnt)

/*****************************************************************************
 函 数 名  : BSP_SOCP_FreeChannel
 功能描述  : 此接口根据通道ID释放分配的编解码通道。
 输入参数  : u32ChanID:通道ID。
 输出参数  : 无。
 返 回 值  : SOCP_OK:通道释放成功。
             SOCP_ERROR:通道释放失败。
*****************************************************************************/
extern BSP_S32 BSP_SOCP_FreeChannel(BSP_U32 u32ChanID);
#define  DRV_SOCP_FreeChannel(u32ChanID)  BSP_SOCP_FreeChannel(u32ChanID)

/*****************************************************************************
 函 数 名  : BSP_SOCP_RegisterEventCB
 功能描述  : 此接口为给定通道注册事件回调函数。
 输入参数  : u32ChanID:通道ID。
             EventCB:事件回调函数，参考socp_event_cb函数定义
 输出参数  : 无。
 返 回 值  : SOCP_OK:注册事件回调函数成功。
             SOCP_ERROR:注册事件回调函数失败。
*****************************************************************************/
extern BSP_S32 BSP_SOCP_RegisterEventCB(BSP_U32 u32ChanID, socp_event_cb EventCB);
#define  DRV_SOCP_RegisterEventCB( u32ChanID,  EventCB) BSP_SOCP_RegisterEventCB( u32ChanID,  EventCB)

/*****************************************************************************
 函 数 名  : BSP_SOCP_Start
 功能描述  : 此接口用于源通道，启动编码或者解码。
 输入参数  : u32SrcChanID:源通道ID。
 输出参数  : 无。
 返 回 值  : SOCP_OK:编码或解码启动成功。
             SOCP_ERROR:编码或解码启动失败。
*****************************************************************************/
extern BSP_S32 BSP_SOCP_Start(BSP_U32 u32SrcChanID);
#define  DRV_SOCP_Start( u32SrcChanID)   BSP_SOCP_Start( u32SrcChanID)

/*****************************************************************************
 函 数 名  : BSP_SOCP_Stop
 功能描述  : 此接口用于源通道，停止编码或者解码。
 输入参数  : u32SrcChanID:源通道ID。
 输出参数  : 无。
 返 回 值  : SOCP_OK:编码或解码停止成功。
             SOCP_ERROR:编码或解码停止失败。
*****************************************************************************/
extern BSP_S32 BSP_SOCP_Stop(BSP_U32 u32SrcChanID);
#define DRV_SOCP_Stop( u32SrcChanID)  BSP_SOCP_Stop( u32SrcChanID)


/*****************************************************************************
 函 数 名      : BSP_SOCP_SetTimeout
 功能描述  :此接口设置超时阈值。
 输入参数  : u32Timeout:超时阈值

 输出参数  : 无。
 返 回 值      : SOCP_OK:设置超时时间阈值成功。
                             SOCP_ERROR:设置超时时间阈值失败
*****************************************************************************/
extern BSP_S32 BSP_SOCP_SetTimeout (SOCP_TIMEOUT_EN_E eTmOutEn, BSP_U32 u32Timeout);
#define DRV_SOCP_SetTimeout(eTmOutEn,u32Timeout)   BSP_SOCP_SetTimeout(eTmOutEn,u32Timeout)

/*****************************************************************************
 函 数 名   : BSP_SOCP_SetDecPktLgth
 功能描述  :设置解码包长度极限值
 输入参数  : pPktlgth:解码包长度极值

 输出参数  : 无。
 返 回 值      : SOCP_OK:设置成功。
                    其他值:设置失败
*****************************************************************************/
extern BSP_S32 BSP_SOCP_SetDecPktLgth(SOCP_DEC_PKTLGTH_S *pPktlgth);
#define DRV_SOCP_SetDecPktLgth(pPktlgth)   BSP_SOCP_SetDecPktLgth(SOCP_DEC_PKTLGTH_S *pPktlgth)

/*****************************************************************************
 函 数 名   : BSP_SOCP_SetDebug
 功能描述  :设置解码源通道的debug模式
 输入参数  : u32ChanID:通道ID
                  u32DebugEn: debug标识
 输出参数  : 无。
 返 回 值      : SOCP_OK:设置成功。
                     其他值:设置失败
*****************************************************************************/
BSP_S32 BSP_SOCP_SetDebug(BSP_U32 u32DestChanID, BSP_U32 u32DebugEn);
#define DRV_SOCP_SetDebug(u32ChanID, u32DebugEn)   BSP_SOCP_SetDebug(BSP_U32 u32DestChanID, BSP_U32 u32DebugEn)

/*****************************************************************************
 函 数 名   : BSP_SOCP_ChnSoftReset
 功能描述  : 源通道软复位
 输入参数  : u32ChanID:通道ID
 输出参数  : 无。
 返 回 值      : SOCP_OK:设复位成功。
                     其他值:复位失败
*****************************************************************************/
extern BSP_S32 BSP_SOCP_ChnSoftReset(BSP_U32 u32ChanID);
#define DRV_SOCP_ChnSoftReset(u32ChanID)   BSP_SOCP_ChnSoftReset(BSP_U32 u32ChanID)

/*****************************************************************************
 函 数 名      : BSP_SOCP_GetWriteBuff
 功能描述  :此接口用于获取写数据buffer。
 输入参数  : u32SrcChanID:源通道ID
                  pBuff:           :写数据buffer

 输出参数  : 无。
 返 回 值      : SOCP_OK:获取写数据buffer成功。
                             SOCP_ERROR:获取写数据buffer失败
*****************************************************************************/
extern  BSP_S32 BSP_SOCP_GetWriteBuff( BSP_U32 u32SrcChanID, SOCP_BUFFER_RW_S *pBuff);
#define  DRV_SOCP_GetWriteBuff( u32SrcChanID, pBuff)  BSP_SOCP_GetWriteBuff( u32SrcChanID, pBuff)

/*****************************************************************************
 函 数 名      : BSP_SOCP_WriteDone
 功能描述  :该接口用于数据的写操作，提供写入数据的长度。
 输入参数  : u32SrcChanID:源通道ID
                  u32WrtSize:   已写入数据的长度
 输出参数  : 无。
 返 回 值      : SOCP_OK:写入数据成功。
                             SOCP_ERROR:写入数据失败
*****************************************************************************/
extern   BSP_S32  BSP_SOCP_WriteDone(BSP_U32 u32SrcChanID, BSP_U32 u32WrtSize);
#define  DRV_SOCP_WriteDone(u32SrcChanID, u32WrtSize)  BSP_SOCP_WriteDone(u32SrcChanID, u32WrtSize)

/*****************************************************************************
 函 数 名      : BSP_SOCP_RegisterRdCB
 功能描述  :该接口用于注册从RD缓冲区中读取数据的回调函数。
 输入参数  : u32SrcChanID:源通道ID
                  RdCB:  事件回调函数
 输出参数  : 无。
 返 回 值      : SOCP_OK:注册RD环形缓冲区读数据回调函数成功。
                             SOCP_ERROR:注册RD环形缓冲区读数据回调函数失败
*****************************************************************************/
extern BSP_S32 BSP_SOCP_RegisterRdCB(BSP_U32 u32SrcChanID, socp_rd_cb RdCB);
#define DRV_SOCP_RegisterRdCB( u32SrcChanID,  RdCB) BSP_SOCP_RegisterRdCB( u32SrcChanID,  RdCB)

/*****************************************************************************
 函 数 名      : BSP_SOCP_GetRDBuffer
 功能描述  :该此接口用于获取RD buffer的数据指针。
 输入参数  : u32SrcChanID:源通道ID
                  pBuff:  RD buffer
 输出参数  : 无。
 返 回 值      : SOCP_OK:获取RD环形缓冲区成功
                             SOCP_ERROR:获取RD环形缓冲区失败
*****************************************************************************/
extern  BSP_S32 BSP_SOCP_GetRDBuffer( BSP_U32 u32SrcChanID,SOCP_BUFFER_RW_S *pBuff);
#define DRV_SOCP_GetRDBuffer(  u32SrcChanID,pBuff)  BSP_SOCP_GetRDBuffer(  u32SrcChanID,pBuff)


/*****************************************************************************
 函 数 名      : BSP_SOCP_ReadRDDone
 功能描述  :此接口用于上层通知SOCP驱动，从RD buffer中实际读取的数据。
 输入参数  : u32SrcChanID:源通道ID
                  u32RDSize:  从RD buffer中实际读取的数据长度
 输出参数  : 无。
 返 回 值      : SOCP_OK:读取RDbuffer中的数据成功
                             SOCP_ERROR:读取RDbuffer中的数据失败
*****************************************************************************/
extern BSP_S32 BSP_SOCP_ReadRDDone(BSP_U32 u32SrcChanID, BSP_U32 u32RDSize);
#define DRV_SOCP_ReadRDDone( u32SrcChanID,  u32RDSize)   BSP_SOCP_ReadRDDone( u32SrcChanID,  u32RDSize)

/*****************************************************************************
 函 数 名      : BSP_SOCP_RegisterReadCB
 功能描述  :该接口用于注册读数据的回调函数。
 输入参数  : u32DestChanID:目标通道ID
                  ReadCB: 事件回调函数
 输出参数  : 无。
 返 回 值      : SOCP_OK:注册读数据回调函数成功
                             SOCP_ERROR:注册读数据回调函数失败
*****************************************************************************/
extern BSP_S32 BSP_SOCP_RegisterReadCB( BSP_U32 u32DestChanID, socp_read_cb ReadCB);
#define   DRV_SOCP_RegisterReadCB(  u32DestChanID,  ReadCB)  BSP_SOCP_RegisterReadCB(  u32DestChanID,  ReadCB)

/*****************************************************************************
 函 数 名      : BSP_SOCP_RegisterReadCB
 功能描述  :该此接口用于获取读数据缓冲区指针。
 输入参数  : u32DestChanID:目标通道ID
                  ReadCB: 读数据buffer
 输出参数  : 无。
 返 回 值      : SOCP_OK:获取读数据缓冲区成功。
                             SOCP_ERROR:获取读数据缓冲区成功。
*****************************************************************************/
extern BSP_S32 BSP_SOCP_GetReadBuff( BSP_U32 u32DestChanID,SOCP_BUFFER_RW_S *pBuffer);
#define   DRV_SOCP_GetReadBuff(  u32DestChanID,  pBuffer)  BSP_SOCP_GetReadBuff(  u32DestChanID,  pBuffer)

/*****************************************************************************
 函 数 名      : BSP_SOCP_ReadDataDone
 功能描述  :该接口用于上层告诉SOCP驱动，从目标通道中读走的实际数据。
 输入参数  : u32DestChanID:目标通道ID
                  u32ReadSize: 已读出数据的长度
 输出参数  : 无。
 返 回 值      : SOCP_OK:读数据成功。
                             SOCP_ERROR:读数据失败
*****************************************************************************/
extern BSP_S32 BSP_SOCP_ReadDataDone(BSP_U32 u32DestChanID,BSP_U32 u32ReadSize);
#define  DRV_SOCP_ReadDataDone( u32DestChanID, u32ReadSize)  BSP_SOCP_ReadDataDone( u32DestChanID, u32ReadSize)

/*****************************************************************************
 函 数 名      : BSP_SOCP_SetBbpEnable
 功能描述  :使能或停止BBP通道。
 输入参数  : bEnable:通道ID
 输出参数  : 无。
 返 回 值      : SOCP_OK:设置成功。
                   其他值:设置失败
*****************************************************************************/
extern BSP_S32 BSP_SOCP_SetBbpEnable(BSP_BOOL bEnable);
#define  DRV_SOCP_SetBbpEnable( bEnable)  BSP_SOCP_SetBbpEnable(BSP_BOOL bEnable)

/*****************************************************************************
 函 数 名      : BSP_SOCP_SetBbpDsMode
 功能描述  :设置BBP DS通道数据溢出处理模式。
 输入参数  : eDsMode:DS通道数据溢出时处理模式设置
 输出参数  : 无。
 返 回 值      : SOCP_OK:设置成功。
                   其他值:设置失败
*****************************************************************************/
extern BSP_S32 BSP_SOCP_SetBbpDsMode(SOCP_BBP_DS_MODE_E eDsMode);
#define  DRV_SOCP_SetBbpDsMode( eDsMode)  BSP_SOCP_SetBbpDsMode(SOCP_BBP_DS_MODE_E eDsMode)

/*****************************************************************************
* 函 数 名  : BSP_SOCP_GetRsvChnScope
* 功能描述  : 获得保留通道ID的范围
* 输入参数  : 无
* 输出参数  : pScope,编码源保留通道的ID范围
* 返 回 值  :
*****************************************************************************/
extern BSP_S32 BSP_SOCP_GetRsvChnScope(SOCP_ENCSRC_RSVCHN_SCOPE_S *pScope);
#define  DRV_SOCP_GetRsvChnScope(pScope)  BSP_SOCP_GetRsvChnScope(SOCP_ENCSRC_RSVCHN_SCOPE_S *pScope)

/*****************************************************************************
* 函 数 名  : DRV_SOCP_SET_HIFICHAN
* 功能描述  : 分配固定通道
* 输入参数  : 目的通道ID
* 输出参数  : 无
* 返 回 值  :
*****************************************************************************/
extern BSP_S32 DRV_SOCP_SET_HIFICHAN(BSP_U32 u32DestChanID);

/*****************************************************************************
* 函 数 名  : DRV_SOCP_START_HIFICHAN
* 功能描述  : 启动固定通道
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  :
*****************************************************************************/
extern BSP_VOID DRV_SOCP_START_HIFICHAN(BSP_VOID);

/*****************************************************************************
* 函 数 名  : BSP_SOCP_StartDsp
* 功能描述  :enable DSP channel
* 输入参数  : 
* 输出参数  : 无
* 返 回 值  :
*****************************************************************************/
BSP_VOID BSP_SOCP_StartDsp(BSP_VOID);


/*****************************************************************************
* 函 数 名  : BSP_SOCP_StopDsp
* 功能描述  :disable DSP channel
* 输入参数  : 
* 输出参数  : 无
* 返 回 值  :
*****************************************************************************/
BSP_S32 BSP_SOCP_StopDsp(BSP_VOID);

/*****************************************************************************
* 函 数 名  : BSP_SOCP_StartTdDsp
* 功能描述  :enable Tds DSP channel
* 输入参数  : 
* 输出参数  : 无
* 返 回 值  :
*****************************************************************************/
BSP_VOID BSP_SOCP_StartTdDsp(BSP_VOID);


/*****************************************************************************
* 函 数 名  : BSP_SOCP_StopTdDsp
* 功能描述  :disable Tds DSP channel
* 输入参数  : 
* 输出参数  : 无
* 返 回 值  :
*****************************************************************************/
BSP_S32 BSP_SOCP_StopTdDsp(BSP_VOID);


/*****************************************************************************
* 函 数 名  : BSP_SOCP_GetLogCfg
*
* 功能描述  : 读取LOG2.0配置
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : SOCP_ENC_DST_BUF_LOG_CFG_STRU *
*****************************************************************************/
extern SOCP_ENC_DST_BUF_LOG_CFG_STRU * BSP_SOCP_GetLogCfg();

/*****************************************************************************
* 函 数 名  : BSP_SOCP_EncDstBufFlush
*
* 功能描述  : SOCP编码目的buffer缓存数据立即输出
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : BSP_S32 BSP_OK:成功 BSP_ERROR:失败 
*****************************************************************************/
extern BSP_S32 BSP_SOCP_EncDstBufFlush();
#define DRV_SOCP_EncDstBufFlush() BSP_SOCP_EncDstBufFlush()
/*****************************************************************************
* 函 数 名  : BSP_SOCP_SaveEncDstBufInfo
*
* 功能描述  : 保存SOCP编码目的buffer缓存数据信息
*
* 输入参数  : SOCP_LOG_EXC_INFO_STRU * logExcInfo
*
* 输出参数  : 无
*
* 返 回 值  : BSP_S32 BSP_OK:成功 BSP_ERROR:失败 
*****************************************************************************/
extern BSP_S32 BSP_SOCP_SaveEncDstBufInfo(SOCP_LOG_EXC_INFO_STRU * logExcInfo);


/*************************SOCP END************************************/

/*************************CIPHER START**********************************/

/* 错误码 */
#define CIPHER_SUCCESS                  0

#define CHN_BUSY                    0
#define CHN_FREE                    1

/* CIPHER支持的算法种类,注意枚举值与寄存器设置值一致 */
typedef enum tagCIPHER_ALGORITHM_E
{
    CIPHER_ALG_NULL = 0,        /* 无算法 */
    CIPHER_ALG_SNOW3G  = 1,     /* SNOW3G算法 */
    CIPHER_ALG_AES_128 = 2,     /* AES128算法 */
    CIPHER_ALG_AES_192 = 3,     /* AES192算法 */
    CIPHER_ALG_AES_256 = 4,     /* AES256算法 */
    CIPHER_ALG_BUTTOM
}CIPHER_ALGORITHM_E;

typedef enum tagKDF_SHA_KEY_SOURCE_E
{
    SHA_KEY_SOURCE_DDR,             //sha_key来源于input point指定的地址空间
    SHA_KEY_SOURCE_KEYRAM,          //sha_key来源于内部KeyRam
    SHA_KEY_SOURCE_FOLLOW,          //sha_key来源于上次KDF操作的sha_key
    SHA_KEY_SOURCE_RESULT,          //sha_key来源于上一次KDF操作的结果
    SHA_KEY_SOURCE_MAX
}KDF_SHA_KEY_SOURCE_E;

typedef enum tagKDF_SHA_S_SOURCE_E
{
    SHA_S_SOURCE_DDR,               //sha_s来源于input point指定的地址空间
    SHA_S_SOURCE_KEYRAM,            //sha_s来源于内部KeyRam
    SHA_S_SOURCE_MAX
}KDF_SHA_S_SOURCE_E;

/**************************************************************************
  枚举定义
**************************************************************************/
enum CIPHER_SECURITY_CHANNEL_ENUM
{
    CIPHER_SECURITY_CHANNEL_0            = 0,                /*通道0*/
    CIPHER_SECURITY_CHANNEL_1            = 1,                /*通道1*/
    CIPHER_SECURITY_CHANNEL_2            = 2,                /*通道2*/
    CIPHER_SECURITY_CHANNEL_3            = 3,                /*通道3*/

    CIPHER_SECURITY_CHANNEL_BUTT
};

/* 通道号*/
#define LTE_SECURITY_CHANNEL_NAS    CIPHER_SECURITY_CHANNEL_2
#define LTE_SECURITY_CHANNEL_RRC    CIPHER_SECURITY_CHANNEL_2
#define LTE_SECURITY_CHANNEL_UL_SRB CIPHER_SECURITY_CHANNEL_1
#define LTE_SECURITY_CHANNEL_DL_SRB CIPHER_SECURITY_CHANNEL_1
#define LTE_SECURITY_CHANNEL_UL_DRB CIPHER_SECURITY_CHANNEL_0
#define LTE_SECURITY_CHANNEL_DL_DRB CIPHER_SECURITY_CHANNEL_3

/* CIPHER单独操作分类 */
typedef enum tagCIPHER_SINGLE_OPT_E
{
    /* 单独操作类型,单独枚举值与寄存器设置值一致 */
    CIPHER_OPT_ENCRYPT = 0x0,   /* 加密 */
    CIPHER_OPT_DECRYPT = 0x1,   /* 解密 */
    CIPHER_OPT_PROTECT_SMAC = 0x2,   /* 保护操作，输出MAC长度为2Byte */
    CIPHER_OPT_PROTECT_LMAC = 0x3,   /* 保护操作，输出MAC长度为4Byte */
    CIPHER_OPT_CHECK_PRE_SMAC  = 0x4,   /* 检查操作，MAC长度为2Byte，位置在数据前面 */
    CIPHER_OPT_CHECK_POST_SMAC = 0x5,   /* 检查操作，MAC长度为2Byte，位置在数据后面 */
    CIPHER_OPT_CHECK_PRE_LMAC  = 0x6,   /* 检查操作，MAC长度为4Byte，位置在数据前面 */
    CIPHER_OPT_CHECK_POST_LMAC = 0x7,   /* 检查操作，MAC长度为4Byte，位置在数据后面 */
    CIPHER_SINGLE_OPT_BUTTOM
}CIPHER_SINGLE_OPT_E;


/* CIPHER关联操作分类 */     //PDCP 关联操作随意组合
typedef enum tagCHPHER_RELA_OPT_E
{
    /*  关联操作类型 */
    CIPHER_PDCP_PRTCT_ENCY,     /*PDCP保护加密(上行)*/
    CIPHER_PDCP_DECY_CHCK,      /*PDCP解密检查(下行)*/

    CIPHER_NAS_ENCY_PRTCT,      /* NAS层加密保护(上行) */
    CIPHER_NAS_CHCK_DECY,       /* NAS层检查解密(下行) */

	CIPHER_RELA_OPT_BUTTOM
}CIPHER_RELA_OPT_E;


/* CIPHER密钥长度, 注意枚举值与寄存器设置值一致 */
typedef enum tagCIPHER_KEY_LEN_E
{
    CIPHER_KEY_L128 = 0,        /* 密钥长度128 bit */
    CIPHER_KEY_L192 = 1,        /* 密钥长度192 bit */
    CIPHER_KEY_L256 = 2,        /* 密钥长度256 bit */
    CIPHER_KEY_LEN_BUTTOM
}CIPHER_KEY_LEN_E;


/* CIPHER操作提交类型 */
typedef enum tagCIPHER_SUBMIT_TYPE_E
{
    CIPHER_SUBM_NONE = 0,           /* 不作任何通知 */
    CIPHER_SUBM_BLK_HOLD = 1,       /* 循环查询等待完成 */
    CIPHER_SUBM_CALLBACK = 2,       /* 回调函数通知 */
    CIPHER_SUBM_BUTTOM
}CIPHER_SUBMIT_TYPE_E;

/* CIPHER通知类型 */
typedef enum tagCIPHER_NOTIFY_STAT_E
{
    CIPHER_STAT_OK = 0,           /* 成功完成 */
    CIPHER_STAT_CHECK_ERR = 1,    /* 完整性检查错误 */
    CIPHER_STAT_BUTTOM
}CIPHER_NOTIFY_STAT_E;

typedef enum CIPHER_HDR_BIT_ENUM
{
    CIPHER_HDR_BIT_TYPE_0              = 0,     /*对应的头或附加头字节长度为0，即不启动此功能*/
    CIPHER_HDR_BIT_TYPE_5              = 1,     /*对应的头或附加头字节长度为1，即启动此功能，对应附加头的内容是Count值的低5位，参与保护*/
    CIPHER_HDR_BIT_TYPE_7              = 2,     /*对应的头或附加头字节长度为1，即启动此功能，对应附加头的内容是Count值的低7位，参与保护*/
    CIPHER_HDR_BIT_TYPE_12             = 3,     /*对应的头或附加头字节长度为2，即启动此功能，对应附加头的内容是Count值的低12位，参与保护*/

    CIPHER_APPEND_HDR_BIT_TYPE_5       = 4,     /*对应的头或附加头字节长度为1，即启动此功能，对应附加头的内容是Count值的低5位，不参与保护，只搬移*/
    CIPHER_APPEND_HDR_BIT_TYPE_7       = 5,     /*对应的头或附加头字节长度为1，即启动此功能，对应附加头的内容是Count值的低7位，不参与保护，只搬移*/
    CIPHER_APPEND_HDR_BIT_TYPE_12      = 6,     /*对应的头或附加头字节长度为2，即启动此功能，对应附加头的内容是Count值的低12位，不参与保护，只搬移*/

    CIPHER_HDR_BIT_TYPE_BUTT
}CIPHER_HDR_E;

/* 算法配置信息 */
typedef struct tagCIHPER_ALG_INFO_S
{
    BSP_U32 u32KeyIndexSec;
    BSP_U32 u32KeyIndexInt;
    CIPHER_ALGORITHM_E enAlgSecurity;          /* 安全操作算法选择，AEC算法还是SNOW3G算法 */
    CIPHER_ALGORITHM_E enAlgIntegrity;         /* 完整性操作算法选择，AEC算法还是SNOW3G算法 */
}CIHPER_ALGKEY_INFO_S;


/*组包加速配置信息*/
typedef struct tagACC_SINGLE_CFG_S
{
    BSP_U32 u32BearId;
    BSP_U32 u32AppdHeaderLen;
    BSP_U32 u32HeaderLen;
    BSP_U32 u32Count;
    BSP_BOOL bMemBlock;                 /* 是否是单内存块 */
    BSP_U32 u32BlockLen;                /* 如果是单块内存，需要知道长度*/
    BSP_U32 u32Offset;                  /* 使用数据距离数据包起始地址偏移*/
    BSP_U32 u32OutLen;                  /* 使用到的数据长度*/
    BSP_U32 u32Aph;                      /*附加包头域*/
    CIHPER_ALGKEY_INFO_S stAlgKeyInfo;  /* 单独操作的算法密钥配置信息 */
} ACC_SINGLE_CFG_S;

/* CIPHER配置信息设置 */
typedef struct tagCIHPER_SINGLE_CFG_S
{
    CIPHER_SINGLE_OPT_E enOpt;          /* 单独操作类型(纯DMA操作忽略下面的算法配置) */
    BSP_U8 u8BearId;
    BSP_U8 u8Direction;
    CIPHER_HDR_E enAppdHeaderLen;
    CIPHER_HDR_E enHeaderLen;
    BSP_U32 u32Count;
    BSP_BOOL bMemBlock;                 /* 是否是单内存块 */
	BSP_U32 u32BlockLen;                /* 如果是单块内存，需要知道长度*/
    CIHPER_ALGKEY_INFO_S stAlgKeyInfo;  /* 单独操作的算法密钥配置信息 */
} CIHPER_SINGLE_CFG_S;

typedef struct tagCIHPER_RELA_CFG_S
{
    CIPHER_RELA_OPT_E enOpt;             /* 关联操作类型(纯DMA操作忽略下面的算法配置) */
    BSP_U8 u8BearId;
    BSP_U8 u8Direction;
    CIPHER_HDR_E enAppdHeaderLen;
    CIPHER_HDR_E enHeaderLen;
    BSP_U32 u32Count;
    BSP_BOOL bMemBlock;                 /* 是否是单内存块 */
	BSP_U32 u32BlockLen;                /* 如果是单块内存，需要知道长度*/
    CIHPER_ALGKEY_INFO_S stAlgKeyInfo;  /* 关联操作第一步的算法密钥配置信息 */

} CIHPER_RELA_CFG_S;


typedef struct tagS_CONFIG_INFO_S
{
    KDF_SHA_S_SOURCE_E   enShaSSource;      //sha_s来源
    BSP_U32              u32ShaSIndex;       //sha_s来源于KeyRam时，其在keyRam中的索引
    BSP_U32              u32ShaSLength;     //sha_s的长度
    BSP_VOID             *pSAddr;           //
}S_CONFIG_INFO_S;

typedef enum tagCIPHER_KEY_OUTPUT_E
{
    CIPHER_KEY_NOT_OUTPUT,    //不输出密钥
    CIPHER_KEY_OUTPUT,        //输出密钥
    CIPHER_KEY_OUTPUT_BUTTOM
}CIPHER_KEY_OUTPUT_E;

typedef BSP_U32 CIPHER_KEY_LEN_E_U32;
typedef BSP_U32 CIPHER_KEY_OUTPUT_E_U32;

/* 获取Key长度和Key内容的结构体*/
typedef struct
{
    CIPHER_KEY_LEN_E_U32    enKeyLen;//要读取的Key的长度，由上层传入
    BSP_VOID                *pKeyAddr;//Key值存放地址
    CIPHER_KEY_LEN_E_U32    *penOutKeyLen;//该地址用于存放实际返回的Key的长度
}KEY_GET_S;

/* KeyMake时，获取Key的长度和Key值*/
typedef struct
{
    CIPHER_KEY_OUTPUT_E_U32 enKeyOutput; //指示是否输出Key到stKeyGet中
    KEY_GET_S stKeyGet;
}KEY_MAKE_S;


/* 完成回调函数类型定义 */
/*
u32ChNum   :  通道号;
u32SourAddr:  源地址
u32DestAddr:  目的地址
enStatus   :  通知的状态,比如: 完成 / 数据传输出错 / 数据完成性检查出错
u32Private   :  用户私有数据;
*/
typedef BSP_VOID (*CIPHER_NOTIFY_CB_T)(BSP_U32 u32ChNum, BSP_U32 u32SourAddr,
              BSP_U32 u32DestAddr, CIPHER_NOTIFY_STAT_E enStatus, BSP_U32 u32Private);

typedef BSP_VOID (*CIPHER_FREEMEM_CB_T)(BSP_VOID* pMemAddr);

/**************************************************************************
  KDF相关
**************************************************************************/

typedef struct tagKEY_CONFIG_INFO_S
{
    KDF_SHA_KEY_SOURCE_E enShaKeySource;    //sha_key来源
    BSP_U32              u32ShaKeyIndex;     //sha_key来源于KeyRam时，其在keyRam中的索引
    BSP_VOID             *pKeySourceAddr;   //
}KEY_CONFIG_INFO_S;


/*****************************************************************************
* 函 数 名  : BSP_Accelerator_DMA
*
* 功能描述  : 为指定通道配置描述符，用于单纯的DMA搬移
*
* 输入参数  : u32BdFifoAddr: BDFIFO首地址
*             pInMemMgr    : 输入buffer首地址
*             pOutMemMgr   : 输出buffer首地址
* 输出参数  : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
BSP_S32 BSP_Accelerator_DMA(BSP_U32 u32BdFifoAddr, const BSP_VOID* pInMemMgr, const BSP_VOID* pOutMemMgr,ACC_SINGLE_CFG_S *pstCfg);

/*****************************************************************************
* 函 数 名  : BSP_Accelerator_Cipher
*
* 功能描述  : 为指定通道配置描述符，用于需要Cipher进行加密的操作
*
* 输入参数  : u32BdFifoAddr: BDFIFO首地址
*             pInMemMgr    : 输入buffer首地址
*             pOutMemMgr   : 输出buffer首地址
*             pstCfg       : Cipher配置信息
* 输出参数   : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
BSP_S32 BSP_Accelerator_Cipher(BSP_U32 u32BdFifoAddr, const BSP_VOID* pInMemMgr, const BSP_VOID* pOutMemMgr,
               ACC_SINGLE_CFG_S *pstCfg);


/**************************************************************************
 函数声明
**************************************************************************/

/*****************************************************************************
* 函 数 名  : BSP_CIPHER_RegistNotifyCB
*
* 功能描述  : 注册通知回调函数
*
* 输入参数  : pFunNotifyCb: 通知回调函数
*
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*           : 失败:CIPHER_NOT_INIT
*****************************************************************************/
BSP_S32 BSP_CIPHER_RegistNotifyCB(CIPHER_NOTIFY_CB_T pFunNotifyCb);

/*****************************************************************************
* 函 数 名  : BSP_CIPHER_FreeMemCB
*
* 功能描述  : 注册Memory释放回调函数
*
* 输入参数  : u32Chn: 通道号
*             bSrc:   0:输入Buffer / 1:输出Bufffer
*             pFunFreeMemCb: Memory释放回调函数
*
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*           : 失败:CIPHER_NOT_INIT
*           :      CIPHER_INVALID_CHN
*****************************************************************************/
BSP_S32 BSP_CIPHER_RegistFreeMemCB(BSP_U32 u32Chn, BSP_BOOL bSrc, CIPHER_FREEMEM_CB_T pFunFreeMemCb);

/*****************************************************************************
* 函 数 名  : BSP_CIPHER_Purge
*
* 功能描述  : 清除所有当前指定通道的CIPHER任务(阻塞接口,完成purge后返回)
*
* 输入参数  : u32Chn: 要清除的通道号
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*           : 失败:CIPHER_NOT_INIT
*           :      CIPHER_INVALID_CHN
*           :      CIPHER_TIME_OUT
*****************************************************************************/
BSP_S32 BSP_CIPHER_Purge(BSP_U32 u32Chn);

/*****************************************************************************
* 函 数 名  : BSP_CIPHER_SingleSubmitTask
*
* 功能描述  : 开始CIPHER 指定通道的工作，用于单独操作。
*
* 输入参数  : u32Chn: 通道号
*             pInMemMgr: 输入buffer内存管理结构指针
*             pOutMemMgr:输出buffer内存管理结构指针
*             pstCfg: Cipher操作配置属性
*             pstSubmAttr:提交的属性
*             u32Private:私有数据
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*             失败:CIPHER_NOT_INIT
*             CIPHER_INVALID_CHN
*             CIPHER_NULL_PTR
*             CIPHER_ALIGN_ERROR
*             CIPHER_INVALID_ENUM
*             CIPHER_PURGING
*             CIPHER_FIFO_FULL
*             CIPHER_TIME_OUT
*****************************************************************************/
BSP_S32 BSP_CIPHER_SingleSubmitTask(BSP_U32 u32Chn, const BSP_VOID* pInMemMgr, const BSP_VOID* pOutMemMgr,
                   CIHPER_SINGLE_CFG_S *pstCfg, CIPHER_SUBMIT_TYPE_E enSubmAttr, BSP_U32 u32Private);

/*****************************************************************************
* 函 数 名  : BSP_CIPHER_RelaSubmitTask
*
* 功能描述  : 开始CIPHER 指定通道的工作，用于关联操作。
*
* 输入参数  : u32Chn: 通道号
*             pInMemMgr: 输入buffer内存管理结构指针
*             pOutMemMgr:输出buffer内存管理结构指针
*             pstCfg: Cipher操作配置属性
*             enSubmAttr:提交的属性
*             u32Private:私有数据
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*             失败:CIPHER_NOT_INIT
*             CIPHER_INVALID_CHN
*             CIPHER_NULL_PTR
*             CIPHER_ALIGN_ERROR
*             CIPHER_INVALID_ENUM
*             CIPHER_PURGING
*             CIPHER_FIFO_FULL
*             CIPHER_TIME_OUT
*****************************************************************************/
BSP_S32 BSP_CIPHER_RelaSubmitTask(BSP_U32 u32Chn, const BSP_VOID* pInMemMgr, const BSP_VOID* pOutMemMgr,
                   CIHPER_RELA_CFG_S *pstCfg, CIPHER_SUBMIT_TYPE_E enSubmAttr, BSP_U32 u32Private);

/*****************************************************************************
* 函 数 名  : BSP_CIPHER_SetKeyIndex
*
* 功能描述  : 设置密钥值
*
* 输入参数  : pKeyAddr   : 密钥地址
*             enKeyLen   : 密钥长度
*             u32KeyIndex: 密钥索引
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*           : 失败:CIPHER_NOT_INIT
*           :      CIPHER_NULL_PTR
*           :      CIPHER_INVALID_ENUM
*           :      CIPHER_INVALID_KEY
*****************************************************************************/
BSP_S32 BSP_CIPHER_SetKey(const BSP_VOID* pKeyAddr,CIPHER_KEY_LEN_E enKeyLen,BSP_U32 u32KeyIndex);

/*****************************************************************************
* 函 数 名  : BSP_CIPHER_GetKey
*
* 功能描述  : 根据KeyIndex获取Key的地址
*
* 输入参数  : u32KeyIndex: 密钥索引
* 输出参数  : 无
* 返 回 值  : 成功:根据KeyIndex得到的Key的地址
*           : 失败:获取Key值所在地址失败,返回0
*****************************************************************************/
BSP_S32 BSP_CIPHER_GetKey(BSP_U32 u32KeyIndex, KEY_GET_S *pstKeyGet);

/*****************************************************************************
* 函 数 名  : CIPHER_DataBuffPara
*
* 功能描述  : 获取上层数据信息并设置信息
*
* 输入参数  : u32Chn        : 通道号
*             bSrc          : TRUE:源地址的属性配置 / FALSE:目的地址属性配置
*             u32BufOft     : 数据buffer指针偏移
*             u32LenOft     : Buffer长度偏移
*             NextOft       : 下一节点偏移
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*           : 失败:CIPHER_NOT_INIT
*           :      CIPHER_INVALID_CHN
*****************************************************************************/
BSP_S32 BSP_CIPHER_SetDataBufPara(BSP_U32 u32Chn, BSP_BOOL bSrc, BSP_U32 u32BufOft,
                                 BSP_U32 u32LenOft, BSP_U32 u32NextOft);

/*****************************************************************************
* 函 数 名  : CIPHER_GetCmplSrcBuff
*
* 功能描述  : 获取已经操作完成的源数据地址
*
* 输入参数  : u32ChNum     : 通道号
* 输出参数  : pu32SourAddr : 操作完成的源地址;
*             pu32DestAddr : 操作完成的目的地址
*             penStatus    : 操作完成的状态
*             pu32Private  : 返回的私有数据
* 返 回 值  : 成功:CIPHER_SUCCESS
*           : 失败:CIPHER_NOT_INIT
*           :      CIPHER_INVALID_CHN
*           :      CIPHER_NULL_PTR
*           :      CIPHER_INVALID_RD
*****************************************************************************/
BSP_S32 BSP_CIPHER_GetCmplSrcBuff(BSP_U32 u32Chn, BSP_U32 *pu32SourAddr,
             BSP_U32 *pu32DestAddr, CIPHER_NOTIFY_STAT_E *penStatus, BSP_U32 *pu32Private);

/*****************************************************************************
* 函 数 名  : BSP_Accelerator_GetBdFifoAddr
*
* 功能描述  : 用于获取当前可用的BDFIFO首地址
*
* 输入参数  : 无
*
* 输出参数  : 无
* 返 回 值  : BDFIFO首地址
*****************************************************************************/
BSP_U32 BSP_Accelerator_GetBdFifoAddr(void);

/*****************************************************************************
* 函 数 名  : BSP_Accelerator_GetStatus
*
* 功能描述  : 获取当前通道状态
*
* 输入参数  :  无
* 输出参数   : 无
* 返 回 值  : 通道忙/空闲
*****************************************************************************/
BSP_S32 BSP_Accelerator_GetStatus(void);

/*****************************************************************************
* 函 数 名  : BSP_Accelerator_Enable
*
* 功能描述  : 使能组包加速
*
* 输入参数  : u32BdFifoAddr: BDFIFO首地址
*
* 输出参数  : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
BSP_S32 BSP_Accelerator_Enable(BSP_U32 u32BdFifoAddr);

/*****************************************************************************
* 函 数 名  : BSP_KDF_KeyRamRead()
*
* 功能描述  : 从Key Ram中读取数据
*
* 输入参数  : enKeyIndex: Key索引，从该索引处开始读取数据
*             u32Length : 读取数据长度，以Byte为单位
* 输出参数  : pDestAddr : 目的地址，将从KeyRam中读取的数据存储到该位置
* 返 回 值  : 成功: BSP_OK
*           : 失败: BSP_ERROR
*****************************************************************************/
BSP_S32 BSP_KDF_KeyRamRead(BSP_VOID *pDestAddr, BSP_U32 u32KeyIndex ,BSP_U32 u32ReadLength);

/*****************************************************************************
* 函 数 名  : BSP_KDF_KeyRamWrite
*
* 功能描述  : 向KeyRam中写入数据
*
* 输入参数  : enKeyIndex: Key索引，从该索引处开始写入数据
*             pSourAddr : 源地址，写入到KeyRam中的数据地址
*             u32Length : 数据长度
*
* 输出参数  : 无
* 返 回 值  : 成功: BSP_OK
*           : 失败: BSP_ERROR
*****************************************************************************/
BSP_S32 BSP_KDF_KeyRamWrite(BSP_U32 u32KeyIndex, BSP_VOID *pSourAddr, BSP_U32 u32Length);


/*****************************************************************************
* 函 数 名  : BSP_KDF_KeyMake
*
* 功能描述  : KDF计算，用于产生新的Key
*
* 输入参数  : stKeyCfgInfo: KDF运算时参数sha_key属性配置
            : stKeyCfgInfo: KDF运算时参数sha_s属性配置
            : enDestIndex : KDF运算产生的Key放置在KeyRam中的位置
* 输出参数  : 无
* 返 回 值  : 成功:BSP_OK
*           : 失败:BSP_ERROR
*****************************************************************************/
BSP_S32 BSP_KDF_KeyMake(KEY_CONFIG_INFO_S *pstKeyCfgInfo, S_CONFIG_INFO_S *pstSCfgInfo,
                        BSP_U32 u32DestIndex, KEY_MAKE_S *pstKeyMake);



/**************************************************************************
  宏定义
**************************************************************************/

/* 打印级别 */
#define CIPHER_PRNT_NOT           BSP_LOG_LEVEL_MAX       /* 不打印任何信息 */
#define CIPHER_PRNT_ERR           BSP_LOG_LEVEL_ERROR     /* 只打印错误信息 */
#define CIPHER_PRNT_WAR           BSP_LOG_LEVEL_WARNING   /* 只打印错误, 告警信息*/
#define CIPHER_PRNT_LOG           BSP_LOG_LEVEL_DEBUG     /* 打印调试, 告警, 错误信息 */

/* 注意CIPHER错误码要为负值 */
#define CIPHER_ERROR_BASE               0x80000800

typedef enum tagCIPHER_ERR_CODE_E
{
    CIPHER_ERR_CODE_NULL_PTR = 1,
    CIPHER_ERR_CODE_NO_MEM ,
    CIPHER_ERR_CODE_NOT_INIT ,
    CIPHER_ERR_CODE_FIFO_FULL ,
    CIPHER_ERR_CODE_INVALID_CHN ,
    CIPHER_ERR_CODE_INVALID_OPT ,
    CIPHER_ERR_CODE_ALIGN_ERROR ,
    CIPHER_ERR_CODE_PURGING ,
    CIPHER_ERR_CODE_TIME_OUT,
    CIPHER_ERR_CODE_INVALID_ENUM,
    CIPHER_ERR_CODE_INVALID_RD,
    CIPHER_ERR_CODE_RD_NULL,
    CIPHER_ERR_CODE_INVALID_KEY,
    CIPHER_ERR_CODE_CHECK_ERROR,
    CIPHER_ERR_CODE_BDLEN_ERROR,
    CIPHER_ERR_CODE_INVALID_NUM,
    CIPHER_ERR_CODE_NO_KEY,
    CIPHER_ERR_CODE_KEYLEN_ERROR
}CIPHER_ERR_CODE_E;

#define CIPHER_NULL_PTR      ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_NULL_PTR))
#define CIPHER_NO_MEM        ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_NO_MEM))
#define CIPHER_NOT_INIT      ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_NOT_INIT))
#define CIPHER_FIFO_FULL     ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_FIFO_FULL))
#define CIPHER_INVALID_CHN   ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_INVALID_CHN))
#define CIPHER_INVALID_OPT   ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_INVALID_OPT))
#define CIPHER_ALIGN_ERROR   ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_ALIGN_ERROR))
#define CIPHER_PURGING       ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_PURGING))
#define CIPHER_TIME_OUT      ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_TIME_OUT))
#define CIPHER_INVALID_ENUM  ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_INVALID_ENUM))
#define CIPHER_INVALID_RD    ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_INVALID_RD))
#define CIPHER_RDQ_NULL      ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_RD_NULL))
#define CIPHER_INVALID_KEY   ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_INVALID_KEY))
#define CIPHER_CHECK_ERROR   ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_CHECK_ERROR))
#define CIPHER_BDLEN_ERROR   ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_BDLEN_ERROR))
#define CIPHER_INVALID_NUM   ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_INVALID_NUM))
#define CIPHER_NO_KEY        ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_NO_KEY))
#define CIPHER_KEYLEN_ERROR  ((BSP_S32)(CIPHER_ERROR_BASE|CIPHER_ERR_CODE_KEYLEN_ERROR))
#define CIPHER_UNKNOWN_ERROR ((BSP_S32)(CIPHER_ERROR_BASE|0xff))



/*************************CIPHER END **********************************/

/*************************WIFI START**********************************/

/*****************************************************************************
 函 数 名  : WifiCalDataLen
 功能描述  : WIFI计算需要数据块内存
 输入参数  : usLen - 用户申请数据长度Len
 输出参数  : 无
 返 回 值  : 申请的数据区的总长度
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年12月22日
    修改内容   : 新生成函数

*****************************************************************************/
extern unsigned short BSP_WifiCalDataLen(unsigned short usLen);
#define DRV_WifiCalDataLen(usLen) BSP_WifiCalDataLen(usLen)

/*****************************************************************************
 函 数 名  : WIFI_TEST_CMD
 功能描述  : 测试命令
 输入参数  : cmdStr；命令字符串
 输出参数  : 无
 返回值    ：无
*****************************************************************************/
extern void WIFI_TEST_CMD(char * cmdStr);

/*****************************************************************************
 函 数 名  : WIFI_GET_TCMD_MODE
 功能描述  : 获取测试命令的模式
 输入参数  : 无
 输出参数  : 16：校准测试模式
             17：表示处于发射模式
             18：表示接收模式
 返回值    ：函数执行的状态结果值
*****************************************************************************/
extern int WIFI_GET_TCMD_MODE(void);

/*****************************************************************************
 函 数 名  : WIFI_POWER_START
 功能描述  : WIFI上电
 输入参数  : 无
 输出参数  : 无
 返回值    ： 0: execute ok
              1: execute failed
*****************************************************************************/
extern int WIFI_POWER_START(void);

/*****************************************************************************
 函 数 名  : WIFI_POWER_SHUTDOWN
 功能描述  : WIFI下电
 输入参数  : 无
 输出参数  : 无
 返回值    ： 0: execute ok
              1: execute failed
*****************************************************************************/
extern int WIFI_POWER_SHUTDOWN(void);

/*****************************************************************************
 函 数 名  : WIFI_GET_STATUS
 功能描述  : WIFI状态获取
 输入参数  : 无
 输出参数  : 无
 返回值    ： 0: wifi is off
              1: wifi is in normal mode
              2: wifi is in tcmd mode
*****************************************************************************/
extern int WIFI_GET_STATUS(void);

/*****************************************************************************
 函 数 名  : WIFI_GET_RX_DETAIL_REPORT
 功能描述  : get result of rx report: totalPkt, GoodPkt, ErrorPkt
 输入参数  : 无
 输出参数  : totalPkt、goodPkt、badPkt
 返回值    ：无
*****************************************************************************/
extern void WIFI_GET_RX_DETAIL_REPORT(int* totalPkt,int* goodPkt,int* badPkt);

/*****************************************************************************
 函 数 名  : WIFI_GET_RX_PACKET_REPORT
 功能描述  : get result of rx ucast&mcast packets
 输入参数  : 无
 输出参数  : ucastPkts、mcastPkts
 返回值    ：无
*****************************************************************************/
extern void  WIFI_GET_RX_PACKET_REPORT(unsigned int *ucastPkts, unsigned int *mcastPkts);

/*****************************************************************************
 函 数 名  : WIFI_GET_PA_CUR_MODE
 功能描述  : get the currrent PA mode of the wifi chip
 输入参数  : 无
 输出参数  : 无
 返回值    ：0:  (WIFI_ONLY_PA_MODE) WIFI chip is in PA mode
             1:  (WIFI_ONLY_NOPA_MODE) WIFI chip is in no PA mode
             -1: wifi chip is in abnormal mode
*****************************************************************************/
extern int WIFI_GET_PA_CUR_MODE(void);

/*****************************************************************************
 函 数 名  : WIFI_GET_PA_MODE
 功能描述  : get the support PA mode of wifi chip
 输入参数  : 无
 输出参数  : 无
 返回值    ：0:  (WIFI_ONLY_PA_MODE) WIFI suppport only PA mode
             1:  (WIFI_ONLY_NOPA_MODE) WIFI suppport only no PA mode
             2:  (WIFI_PA_NOPA_MODE) WIFI suppport both PA &  no PA mode
             -1: failed
*****************************************************************************/
extern int WIFI_GET_PA_MODE(void);

/*****************************************************************************
 函 数 名  : WIFI_SET_PA_MODE
 功能描述  : set the PA mode of wifi chip
 输入参数  : 0:  (WIFI_ONLY_PA_MODE) WIFI suppport only PA mode
             1:  (WIFI_ONLY_NOPA_MODE) WIFI suppport only no PA mode
             2:  (WIFI_PA_NOPA_MODE) WIFI suppport both PA &  no PA mode
 输出参数  : 无
 返回值    ：0: success
             -1: failed
*****************************************************************************/
extern int WIFI_SET_PA_MODE(int wifiPaMode);

/*****************************************************************************
 函 数 名  : DRV_WIFI_DATA_RESERVED_TAIL
 功能描述  : WIFI计算需要数据块内存
 输入参数  : usLen - 用户申请数据长度Len
 输出参数  : 无
 返 回 值  : 数据区的尾部预留的长度
*****************************************************************************/
extern unsigned int DRV_WIFI_DATA_RESERVED_TAIL(unsigned int len);

/*h00106354 20120201 合入流控接口 add start */
/*****************************************************************************
 函 数 名  : DRV_WIFI_SET_RX_FCTL
 功能描述  : 设置WIFI接收流控标识
 输入参数  : para1、para2
 输出参数  : 无
 返 回 值  : BSP_OK/BSP_ERROR
*****************************************************************************/
extern unsigned long DRV_WIFI_SET_RX_FCTL(unsigned long para1, unsigned long para2);

/*****************************************************************************
 函 数 名  : DRV_WIFI_CLR_RX_FCTL
 功能描述  : 清除WIFI接收流控标识
 输入参数  : para1、para2
 输出参数  : 无
 返 回 值  : 1 : 有
             0 : 无
*****************************************************************************/
extern unsigned long DRV_WIFI_CLR_RX_FCTL(unsigned long para1, unsigned long para2);

/*****************************************************************************
 函 数 名  : DRV_AT_GET_USER_EXIST_FLAG
 功能描述  : 返回当前是否有USB连接或者WIFI用户连接(C核调用)
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 1 : 有
             0 : 无
*****************************************************************************/
extern unsigned long  DRV_AT_GET_USER_EXIST_FLAG(void);


unsigned long USB_ETH_DrvSetRxFlowCtrl   (unsigned long ulParam1, unsigned long ulParam2);
unsigned long USB_ETH_DrvClearRxFlowCtrl (unsigned long ulParam1, unsigned long ulParam2);
unsigned long USB_ETH_DrvSetHostAssembleParam(unsigned long ulHostOUtTimeout);
unsigned long USB_ETH_DrvSetDeviceAssembleParam(unsigned long ulEthTxMinNum,
											    unsigned long ulEthTxTimeout,
											    unsigned long ulEthRxMinNum,
											    unsigned long ulEthRxTimeout);
/*h00106354 20120201 合入流控接口 add end */

/*************************WIFI END************************************/

/*************************AUDIO START*********************************/

/*AUDIO IOCTL接口命令字*/
#define AUDIO_IOCTL_BASE                    (0x30000000)
#define IOCTL_AUDIO_SAMPLE_RATE_SET         (AUDIO_IOCTL_BASE+1)
#define IOCTL_AUDIO_PCM_MODE_SET            (AUDIO_IOCTL_BASE+7)
#define IOCTL_AUDIO_IN_DEV_SELECT           (AUDIO_IOCTL_BASE+11)
#define IOCTL_AUDIO_OUT_DEV_SELECT          (AUDIO_IOCTL_BASE+12)
#define IOCTL_AUDIO_OUT_DEV_UNSELECT        (AUDIO_IOCTL_BASE+13)
#define IOCTL_AUDIO_IN_DEV_UNSELECT         (AUDIO_IOCTL_BASE+14)
#define IOCTL_AUDIO_VOICE_OPEN              (AUDIO_IOCTL_BASE+23)
#define IOCTL_AUDIO_VOICE_CLOSE             (AUDIO_IOCTL_BASE+24)

typedef enum
{
    NODEV=0x00,
    HEADSET=0x01,
    HANDSET=0x02,
    HANDFREE=0x04
}AUDIO_DEV_TYPE;
typedef enum
{
    SAMPLE_RATE_8K=0,
    SAMPLE_RATE_11K025=1,
    SAMPLE_RATE_12K=2,
    SAMPLE_RATE_16K=3,
    SAMPLE_RATE_22K05=4,
    SAMPLE_RATE_24K=5,
    SAMPLE_RATE_32K=6,
    SAMPLE_RATE_44K1=7,
    SAMPLE_RATE_48K=8,
    SAMPLE_RATE_INVALID=0xFFFF
}AUDIO_SAMPLE_RATE;


/********************************************************************************************************
 函 数 名  : BSP_AUDIO_CodecOpen
 功能描述  : 打开一个audio设备，并对codec进行初始化。
 输入参数  : devname：该参数固定为"/dev/codec0"。
                           flags：该参数表示读写方式，固定为0。
                           mode：该参数表示新建文件方式，固定为0。。
  输出参数  : 无。
 返 回 值  : 非负值:  设备句柄；
             -1：操作失败。
********************************************************************************************************/
extern int BSP_AUDIO_CodecOpen(const char *devname, int flags, int mode);
#define DRV_CODEC_OPEN(devname, flags, mode)    BSP_AUDIO_CodecOpen(devname, flags, mode)

/********************************************************************************************************
 函 数 名  : BSP_AUDIO_Codec_ioctl
 功能描述  : codec设备命令字控制。
 输入参数  : devid：设备句柄。
                           cmd：命令字。
                           arg：命令参数。
  输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
********************************************************************************************************/
extern int BSP_AUDIO_Codec_ioctl(int devid, int cmd, int arg);
#define DRV_CODEC_IOCTL(devid, cmd, arg)   BSP_AUDIO_Codec_ioctl(devid, cmd, arg)

/*************************AUDIO END***********************************/

/*************************USB START***********************************/
/* 设备枚举最大端口个数 */
#define DYNAMIC_PID_MAX_PORT_NUM        17

typedef  enum
{
    FLOW_CON,     /* 流控        */
    PDP_STATUS,   /* PDP激活结果 */
    SIGNAL_LEVEL, /* 信号强度    */
    PKT_STATICS,  /* 流量统计    */
    AT_CMD,       /* AT命令      */
    PS_NOTIFY,    /* Notify命令  */
    RESERVE       /* 保留        */
}NDIS_NOTICE;

typedef unsigned long (*pPCSCFUNC)(unsigned long CmdType, unsigned char *pApdu, unsigned long ApduLen);

/* NV项50091结构，代表设备将要枚举的端口形态 */
typedef struct
{
    unsigned long ulStatus;
    unsigned char aucFirstPortStyle[DYNAMIC_PID_MAX_PORT_NUM];  /* 设备切换前端口形态 */
    unsigned char aucRewindPortStyle[DYNAMIC_PID_MAX_PORT_NUM]; /* 设备切换后端口形态 */
    unsigned char reserved[22];
}DRV_DYNAMIC_PID_TYPE_STRU;

typedef enum
{
    WWAN_WCDMA    = 1,/*WCDMA模式*/
    WWAN_CDMA    = 2  /*CDMA模式*/
}NDIS_WWAN_MODE;


/*管脚信号的定义*/
#define IO_CTRL_FC                      (0x02)
#define IO_CTRL_DSR                     (0x20)
#define IO_CTRL_DTR                     (0x01)
#define IO_CTRL_RFR                     (0x04)
#define IO_CTRL_CTS                     (0x10)
#define IO_CTRL_RI                      (0x40)
#define IO_CTRL_DCD                     (0x80)

typedef int  (*pComRecv)(unsigned char  uPortNo, unsigned char* pData, unsigned short uslength);

typedef int  (*pUartRecv)(unsigned char uPortNo, unsigned char* pData, unsigned short uslength);

typedef long (*pUsbStatus)(unsigned char ucStatus);

typedef long  (*pUsbState)(unsigned char ucStatus);

typedef int  (*pComStatusNotify)(unsigned char uStatus);

/* MODEM 管角信号结构体定义*/
typedef struct tagMODEM_MSC_STRU
{
    BSP_U32 OP_Dtr     :    1;          /*DTR CHANGE FLAG*/
    BSP_U32 OP_Dsr     :    1;          /*DSR CHANGE FLAG*/
    BSP_U32 OP_Cts     :    1;          /*CTSCHANGE FLAG*/
    BSP_U32 OP_Rts     :    1;          /*RTS CHANGE FLAG*/
    BSP_U32 OP_Ri      :    1;          /*RI CHANGE FLAG*/
    BSP_U32 OP_Dcd     :    1;          /*DCD CHANGE FLAG*/
    BSP_U32 OP_Fc      :    1;          /*FC CHANGE FLAG*/
    BSP_U32 OP_Brk     :    1;          /*BRK CHANGE FLAG*/
    BSP_U32 OP_Spare   :    24;         /*reserve*/
    BSP_U8   ucDtr;                     /*DTR  VALUE*/
    BSP_U8   ucDsr;                     /*DSR  VALUE*/
    BSP_U8   ucCts;                     /*DTS VALUE*/
    BSP_U8   ucRts;                     /*RTS  VALUE*/
    BSP_U8   ucRi;                      /*RI VALUE*/
    BSP_U8   ucDcd;                     /*DCD  VALUE*/
    BSP_U8   ucFc;                      /*FC  VALUE*/
    BSP_U8   ucBrk;                     /*BRK  VALUE*/
    BSP_U8   ucBrkLen;                  /*BRKLEN VALUE*/
} MODEM_MSC_STRU, *PMODEM_MSC_STRU,AT_DCE_MSC_STRU;

/*协议栈BULK数据接收回调函数类型定义*/
typedef int (*USB_MODEM_RECV_CALL)
(
    unsigned char ucPortType,          /*端口号*/
    unsigned char ucDlci,                 /*链路号*/
    unsigned char *pData,               /*数据指针*/
    unsigned short usLen                /*数据长度*/
);


/*协议栈消息通道状态变化数据接收回调函数定义*/
typedef int (*USB_MODEM_AT_Sig)
(
    unsigned char ucPortType,          /*端口号*/
    unsigned char ucDlci,                   /*链路号*/
    MODEM_MSC_STRU* data  /*指向MODEM 管角信号结构体的指针*/
);


/*协议栈断链、建链回调函数*/
typedef int (*USB_MODEM_IN_OUT)
(
    unsigned char ucPortType,           /*端口号*/
    unsigned char ucDlci                   /*链路号*/
);

/*协议栈高性能复制函数定义*/
typedef void (*USB_MODEM_COPY)
(
    unsigned char   *pDest,
    unsigned char   *pSrc,
    unsigned long   ulLen
);


typedef struct
{
    unsigned int    GateWayIpAddr;  /* PDP激活获取到的远程网关IP地址      */
    unsigned int    LocalIpAddr;    /* PDP激活获取到的分配到MODEM的IP地址 */
    unsigned int    MaskAddr;       /* 子网掩码 */
    unsigned int    speed;          /* 连接速度，单位bps */
    unsigned char   ActiveSatus;    /* 激活结果，0为成功，其他为失败 */
}NDIS_PDP_STRU,*pNDIS_PDP_STRU;


typedef struct
{
    unsigned int    currentTx;    /* CURRENT_TX */
    unsigned int    currentRx;    /* CURRENT_RX */
    unsigned int    maxTx;        /* MAX_TX */
    unsigned int    maxRx;        /* MAX_RX */
}NDIS_RATE_STRU,*pNDIS_RATE_STRU;


typedef struct
{
    unsigned int     TxOKCount;         /*发送包数*/
    unsigned int     RxOKCount;         /*接收包数*/
    unsigned int     TxErrCount;        /*发送错误*/
    unsigned int     RxErrCount;        /*接收错误*/
    unsigned int     TxOverFlowCount;   /*发送溢出丢包*/
    unsigned int     RxOverFlowCount;   /*接收溢出丢包*/
    unsigned int     MaxTx;         /*发送速率*/
    unsigned int     MaxRx;         /*接收速率*/
} NDIS_PKT_STATISTICS, *pNDIS_PKT_STATISTICS;

typedef struct
{
    unsigned char   *pucAtCmd;          /*指向AT命令的指针*/
    unsigned short   usLen;             /*AT命令的长度*/
}NDIS_AT_CMD_STRU, *pNDIS_AT_CMD_STRU;

/* 信号上报函数使用的结构体*/
typedef struct
{
    NDIS_NOTICE                 type;
    union
    {
        NDIS_PDP_STRU           pdp;
        NDIS_PKT_STATISTICS     pktStatics;
        unsigned char           flowFlag;   /*  1:流控，0：解流控 */
        int                     dbm;
        NDIS_AT_CMD_STRU        atCmd;
    }NDIS_STATUS_DATA;

} NDIS_PRO_STRU, *pNDIS_PRO_STRU;

typedef int (*USB_NDIS_RECV_CALL)
(
    unsigned char *pData, /* 接收缓冲区指针,内容为IP报文 */
    unsigned short usLen  /* 数据长度 */
);

typedef int (*USB_NDIS_BRK)
(
    void
);

typedef int (*USB_NDIS_EST)
(
    unsigned char *pData /* 接收缓冲区指针，内容为连接配置管理参数 */
);

typedef int (*USB_NDIS_PKT_STATISTICS_PERIOD)
(
    unsigned int period /*单位S */
);

typedef int (*USB_NDIS_GET_RATE)
(
    NDIS_RATE_STRU  *pRate
);

typedef int (*USB_NDIS_AT_CMD_RECV)
(
    unsigned char *pBuff,   /*指向AT命令指针*/
    unsigned short usLen    /*AT命令长度*/
);

typedef unsigned int (*USB_NDIS_GET_WWAN_MODE)  /*获取当前网络模式cdma/wcdma*/
(
    void
);

typedef unsigned long (*USB_NET_DEV_SET_MAX_TX_PACKET_NUMBER)
(
    unsigned long ulNumber
);

typedef void (*IPC_RECV_HANDLER)(unsigned char ucCidNo, unsigned char *pstData, unsigned int ulLength );

typedef int (*IPC_COM_RECV_HANDLER)(unsigned char uPortNo, unsigned char* pData, unsigned short uslength);

typedef unsigned long (*MNTN_ERRLOGREGFUN)(char * cFileName,unsigned int ulFileId, unsigned int ulLine,
                unsigned int ulErrNo, void * pBuf, unsigned int ulLen);

/*****************************************************************************
 函 数 名  : BSP_USB_SetPid
 功能描述  :
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern int BSP_USB_SetPid(unsigned char u2diagValue);
#define DRV_SET_PID(u2diagValue)    BSP_USB_SetPid(u2diagValue)

/*****************************************************************************
函数名：   BSP_USB_PortTypeQuery
功能描述:  查询当前的设备枚举的端口形态值
输入参数： stDynamicPidType  端口形态
输出参数： stDynamicPidType  端口形态
返回值：   0:    查询成功
           其他：查询失败
*****************************************************************************/
unsigned int BSP_USB_PortTypeQuery(DRV_DYNAMIC_PID_TYPE_STRU *pstDynamicPidType);
#define DRV_SET_PORT_QUIRY(a)   BSP_USB_PortTypeQuery(a)

/*****************************************************************************
函数名：   BSP_USB_PortTypeValidCheck
功能描述:  提供给上层查询设备端口形态配置合法性接口
           1、端口为已支持类型，2、包含PCUI口，3、无重复端口，4、端点数不超过16，
           5、第一个设备不为MASS类
输入参数： pucPortType  端口形态配置
           ulPortNum    端口形态个数
返回值：   0:    端口形态合法
           其他：端口形态非法
*****************************************************************************/
unsigned int BSP_USB_PortTypeValidCheck(unsigned char *pucPortType, unsigned long ulPortNum);
#define DRV_USB_PORT_TYPE_VALID_CHECK(pucPortType, ulPortNum)  \
                    BSP_USB_PortTypeValidCheck(pucPortType, ulPortNum)

/*****************************************************************************
函数名：   BSP_USB_GetAvailabePortType
功能描述:  提供给上层查询当前设备支持端口形态列表接口
输入参数： ulPortMax    协议栈支持最大端口形态个数
输出参数:  pucPortType  支持的端口形态列表
           pulPortNum   支持的端口形态个数
返回值：   0:    获取端口形态列表成功
           其他：获取端口形态列表失败
*****************************************************************************/
unsigned int BSP_USB_GetAvailabePortType(unsigned char *pucPortType,
                            unsigned long *pulPortNum, unsigned long ulPortMax);
#define DRV_USB_GET_AVAILABLE_PORT_TYPE(pucPortType, pulPortNum, ulPortMax)  \
                BSP_USB_GetAvailabePortType(pucPortType, pulPortNum, ulPortMax)

/*****************************************************************************
 函 数 名  : BSP_USB_ATProcessRewind2Cmd
 功能描述  : rewind2 CMD 处理。
 输入参数  : pData：数据。
 输出参数  : 无
 返 回 值  : false(0):处理失败
             tool(1):处理成功
*****************************************************************************/
extern int BSP_USB_ATProcessRewind2Cmd(unsigned char *pData);
#define DRV_PROCESS_REWIND2_CMD(pData)    BSP_USB_ATProcessRewind2Cmd(pData)

/*****************************************************************************
 函 数 名  : BSP_USB_GetDiagModeValue
 功能描述  : 获得设备类型。
 输入参数  : 无。
 输出参数  : ucDialmode:  0 - 使用Modem拨号; 1 - 使用NDIS拨号; 2 - Modem和NDIS共存
              ucCdcSpec:   0 - Modem/NDIS都符合CDC规范; 1 - Modem符合CDC规范;
                           2 - NDIS符合CDC规范;         3 - Modem/NDIS都符合CDC规范
 返 回 值  : VOS_OK/VOS_ERR
*****************************************************************************/
extern BSP_S32 BSP_USB_GetDiagModeValue(unsigned char *pucDialmode,
                             unsigned char *pucCdcSpec);
#define DRV_GET_DIAG_MODE_VALUE(pucDialmode, pucCdcSpec)    \
                             BSP_USB_GetDiagModeValue(pucDialmode, pucCdcSpec)

/*****************************************************************************
 函 数 名  : BSP_USB_GetPortMode
 功能描述  : 获取端口形态模式，网关对接需求，打桩。
 输入参数  : 。
 输出参数  :
 返 回 值  :
*****************************************************************************/
extern unsigned char BSP_USB_GetPortMode(char*PsBuffer, unsigned long*Length );
#define DRV_GET_PORT_MODE(PsBuffer, Length)    BSP_USB_GetPortMode(PsBuffer,Length)

/*****************************************************************************
 函 数 名  : BSP_USB_GetU2diagDefaultValue
 功能描述  : 获得端口默认u2diag值
 输入参数  : 无。
 输出参数  : 无。
 返回值：   u2diag值

*****************************************************************************/
extern BSP_U32 BSP_USB_GetU2diagDefaultValue(void);
#define DRV_GET_U2DIAG_DEFVALUE()    BSP_USB_GetU2diagDefaultValue()

/*****************************************************************************
 函 数 名  : ErrlogRegFunc
 功能描述  : USB MNTN注册异常日志接口，Porting项目中打桩
 输入参数  : 无。
 输出参数  : 无。
 返回值：   无

*****************************************************************************/
//extern void ErrlogRegFunc(MNTN_ERRLOGREGFUN pRegFunc);
extern void MNTN_ERRLOG_REG_FUNC(MNTN_ERRLOGREGFUN pRegFunc);

/*****************************************************************************
 函 数 名  : getSystemResetInfo
 功能描述  : 获取系统复位信息
 输入参数  : 无

 输出参数  : 无。
 返 回 值  : 无
 注意事项  ：
*****************************************************************************/
extern void * GET_SYSTEM_RESET_INFO(void);

/*****************************************************************************
* 函 数 名  : DRV_DSP_AHB_RESET
* 功能描述  : ZSP AHB总线复位
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 无
* 其它说明  : 无
*****************************************************************************/
extern void DRV_DSP_AHB_RESET(void);

/*****************************************************************************
* 函 数 名  : DRV_DSP_AHB_RESET_CANCEL
* 功能描述  : ZSP AHB总线解复位
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 无
* 其它说明  : 无
*****************************************************************************/
extern void DRV_DSP_AHB_RESET_CANCEL(void);


/*****************************************************************************
 函 数 名  : BSP_USB_UdiagValueCheck
 功能描述  : 本接口用于检查NV项中USB形态值的合法性.
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 0：OK；
            -1：ERROR
*****************************************************************************/
extern int BSP_USB_UdiagValueCheck(unsigned long DiagValue);
#define DRV_UDIAG_VALUE_CHECK(DiagValue)     BSP_USB_UdiagValueCheck(DiagValue)
#define DRV_U2DIAG_VALUE_CHECK(DiagValue)     BSP_USB_UdiagValueCheck(DiagValue)

/*****************************************************************************
 函 数 名  : BSP_USB_GetLinuxSysType
 功能描述  : 本接口用于检查PC侧是否为Linux，以规避Linux后台二次拨号失败的问题.
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 0：Linux；
            -1：非Linux。
*****************************************************************************/
extern int BSP_USB_GetLinuxSysType(void) ;
#define DRV_GET_LINUXSYSTYPE()    BSP_USB_GetLinuxSysType()

/********************************************************
函数说明： 返回当前设备列表中支持(sel=1)或者不支持(sel=0)PCSC的设备形态值
函数功能:
输入参数：sel
          0: 通过参数dev_type返回当前不带PCSC是设备形态值
          1：通过参数dev_type返回当前带PCSC是设备形态值
输出参数：dev_type 写入需要的设备形态值，如果没有则不写入值。
          NV中存储的设备形态值
输出参数：pulDevType 与ulCurDevType对应的设备形态值，如果没有返回值1。
返回值：
          0：查询到相应的设备形态值；
          1：没有查询到响应的设备形态值。
********************************************************/
int BSP_USB_PcscInfoSet(unsigned int  ulSel, unsigned int  ulCurDevType, unsigned int *pulDevType);
#define DRV_USB_PCSC_INFO_SET(ulSel, ulCurDevType, pulDevType)    BSP_USB_PcscInfoSet(ulSel, ulCurDevType, pulDevType)

/*TCP/IP协议栈可维可测捕获的消息标识*/
enum IPS_MNTN_TRACE_IP_MSG_TYPE_ENUM
{
    /* IP 数据包可维可测上报 */
    ID_IPS_TRACE_IP_ADS_UL                  = 0xD030,
    ID_IPS_TRACE_IP_ADS_DL                  = 0xD031,
    ID_IPS_TRACE_IP_USB_UL                  = 0xD032,
    ID_IPS_TRACE_IP_USB_DL                  = 0xD033,

    ID_IPS_TRACE_IP_MSG_TYPE_BUTT
};

/********************************************************
函数说明：协议栈注册USB将SKB关键信息上报到SDT回调函数
函数功能:
输入参数：pFunc: SKB关键信息上报到SDT回调函数指针
输出参数：无
输出参数：无
返回值：  0：成功
          1：失败
********************************************************/
typedef void (*USB_IPS_MNTN_TRACE_CB_T)(struct sk_buff *skb,unsigned short usType);
extern unsigned int BSP_USB_RegIpsTraceCB(USB_IPS_MNTN_TRACE_CB_T pFunc);
#define DRV_USB_REG_IPS_TRACECB(x) BSP_USB_RegIpsTraceCB(x)

/********************************************************
函数说明：协议栈注册USB使能通知回调函输
函数功能:
输入参数：pFunc: USB使能回调函输指针
输出参数：无
输出参数：无
返回值：  0：成功
          1：失败
********************************************************/
typedef void (*USB_UDI_ENABLE_CB_T)(void);
extern unsigned int BSP_USB_RegUdiEnableCB(USB_UDI_ENABLE_CB_T pFunc);
#define DRV_USB_REGUDI_ENABLECB(x) BSP_USB_RegUdiEnableCB(x)

/********************************************************
函数说明：协议栈注册USB去使能通知回调函输
函数功能:
输入参数：pFunc: USB使能回调函输指针
输出参数：无
输出参数：无
返回值：  0：成功
          1：失败
********************************************************/
typedef void (*USB_UDI_DISABLE_CB_T)(void);
extern unsigned int BSP_USB_RegUdiDisableCB(USB_UDI_DISABLE_CB_T pFunc);
#define DRV_USB_REGUDI_DISABLECB(x) BSP_USB_RegUdiDisableCB(x)

/********************************************************
函数说明：协议栈注册HSIC使能通知回调函输
函数功能:
输入参数：pFunc: HSIC使能回调函输指针
输出参数：无
输出参数：无
返回值：  0：成功
          1：失败
********************************************************/
typedef void (*HSIC_UDI_ENABLE_CB_T)(void);
extern unsigned int BSP_HSIC_RegUdiEnableCB(HSIC_UDI_ENABLE_CB_T pFunc);
#define DRV_HSIC_REGUDI_ENABLECB(x) BSP_HSIC_RegUdiEnableCB(x)

/********************************************************
函数说明：协议栈注册HSIC去使能通知回调函输
函数功能:
输入参数：pFunc: HSIC去使能回调函输指针
输出参数：无
输出参数：无
返回值：  0：成功
          1：失败
********************************************************/
typedef void (*HSIC_UDI_DISABLE_CB_T)(void);
extern unsigned int BSP_HSIC_RegUdiDisableCB(HSIC_UDI_DISABLE_CB_T pFunc);
#define DRV_HSIC_REGUDI_DISABLECB(x) BSP_HSIC_RegUdiDisableCB(x)
/********************************************************
函数说明：协议栈查询HSIC枚举状态
函数功能:
输入参数：无
输出参数：无
输出参数：无
返回值：  1：枚举完成
          0：枚举未完成
********************************************************/
extern unsigned int BSP_GetHsicEnumStatus(void);
#define DRV_GET_HSIC_ENUM_STATUS() BSP_GetHsicEnumStatus()

/********************************************************
函数说明：协议栈查询HSIC是否支持NCM
函数功能:
输入参数：无
输出参数：无
输出参数：无
返回值：  1：支持NCM
          0：不支持NCM
********************************************************/
extern int BSP_USB_HSIC_SupportNcm(void);
#define DRV_USB_HSIC_SUPPORT_NCM() BSP_USB_HSIC_SupportNcm()

/********************************************************
函数说明：TTF查询预申请SKB Num
函数功能:
输入参数：无
输出参数：无
输出参数：无
返回值：  SKB Num
********************************************************/
extern BSP_U32 BSP_AcmPremallocSkbNum(void);
#define DRV_GET_PREMALLOC_SKB_NUM() BSP_AcmPremallocSkbNum()

/********************************************************
函数名：   BSP_UDI_FindVcom
功能描述： 查询当前设备形态下指定的UDI虚拟串口端口是否存在
输入参数： UDI_DEVICE_ID枚举值，即待查询的虚拟串口端口ID
输出参数： 无
返回值：
           0：当前设备形态不支持查询的虚拟串口端口；
           1：当前设备形态支持查询的虚拟串口端口。
注意事项： 无
********************************************************/
extern int BSP_UDI_FindVcom(UDI_DEVICE_ID enVCOM);

/*****************************************************************************
* 函 数 名  : DRV_USB_RegEnumDoneForMsp
* 功能描述  : 提供给 MSP 注册 USB 枚举完成后通知函数
* 输入参数  : pFunc: 枚举完成回调函数指针
* 输出参数  : 无
* 返 回 值  : 0: 成功注册,等待枚举完成通知;
*             1: USB 已经枚举完成, 可以直接初始化 USB 部分;
*             -1: 失败, 非USB形态,没有USB驱动
*****************************************************************************/
extern signed int BSP_USB_RegEnumDoneForMsp(void *pFunc);
#define DRV_USB_RegEnumDoneForMsp   BSP_USB_RegEnumDoneForMsp

/*****************************************************************************
* 函 数 名  : BSP_USB_RegEnumDoneForPs
* 功能描述  : 提供给 PS 注册 USB 枚举完成后通知函数
* 输入参数  : pFunc: 枚举完成回调函数指针
* 输出参数  : 无
* 返 回 值  : 0: 成功注册,等待枚举完成通知;
*             1: USB 已经枚举完成, 可以直接初始化 USB 部分;
*             -1: 失败, 非USB形态,没有USB驱动
*****************************************************************************/
extern signed int BSP_USB_RegEnumDoneForPs(void *pFunc);
#define DRV_USB_RegEnumDoneForPs    BSP_USB_RegEnumDoneForPs

/********************************************************
函数说明： 返回当前NV项中的设备形态值，是否支持PCSC
函数功能:
输入参数： dev_type是设备形态值（OAM从NV中读出）

输出参数： 无。
返回值：
          0: dev_type不支持PCSC设备；
          1：dev_type支持PCSC设备
********************************************************/
/*unsigned int pcsc_info_quiry(unsigned int ulDevType);*/
#define DRV_USB_PCSC_INFO_QUIRY(a)            DRV_OK

typedef unsigned long (*pFunAPDUProcess)(unsigned long CmdType, unsigned char *pApdu, unsigned long ApduLen);
typedef unsigned long (*GetCardStatus)(void);

typedef struct
{
    pFunAPDUProcess   pFuncApdu;
    GetCardStatus  pFuncGetCardStatus;
}BSP_CCID_REGFUNC;

/*****************************************************************************
 函 数 名  : pcsc_usim_int
 功能描述  : 注册PCSC命令接收函数
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/
extern void pcsc_usim_int(pFunAPDUProcess pFun1, GetCardStatus pFun2);
#define DRV_PCSC_REG_CALLBACK(pFun1, pFun2) pcsc_usim_int((pFun1), (pFun2))


/*****************************************************************************
 函 数 名  : pcsc_usim_ctrl_cmd
 功能描述  : PCSC命令回复函数
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/
extern unsigned long pcsc_usim_ctrl_cmd(unsigned long CmdTpye, unsigned long Result,
                                        unsigned char *Buf, unsigned long Length);
#define DRV_PCSC_SEND_DATA(CmdType, Result, Buffer, Length) pcsc_usim_ctrl_cmd(CmdType, Result, Buffer, Length)

/* Porting联编，暂时定义为空 */
#define DRV_COM_RCV_CALLBACK_REGI(uPortNo,pCallback)    DRV_OK


/*****************************************************************************
 函 数 名  : pcsc_usim_ctrl_cmd
 功能描述  : PCSC命令回复函数
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/


/*闪电卡还未开发 先暂时API 打桩begin*/
/*****************************************************************************
 函 数 名  : BSP_USB_RndisAppEventDispatch
 功能描述  : 本接口用于通知APP 相应的USB插拔事件
 输出参数  : usb事件。
 返 回 值  :无
*****************************************************************************/
extern void BSP_USB_RndisAppEventDispatch(unsigned ulStatus) ;
#define DRV_RNDIS_APP_ENENT_DISPATCH(ulStatus)    BSP_USB_RndisAppEventDispatch(ulStatus)

/************************************************************************
 * FUNCTION
 *       rndis_app_event_dispatch
 * DESCRIPTION
 *       闪电卡版本控制应用进行拨号或断开拨号连接
 * INPUTS
 *       进行拨号或断开拨号指示
 * OUTPUTS
 *       NONE
 *************************************************************************/
extern VOID rndis_app_event_dispatch(unsigned int ulStatus);
extern VOID DRV_AT_SETAPPDAILMODE(unsigned int ulStatus);


/************************************************************************
 * FUNCTION
 *       PDP_ACT_DRV_CALLBACK
 * DESCRIPTION
 *       PDP激活后调用底软的回调函数，原本在闪电卡上用作加载符号表信息，现在打桩
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
 *************************************************************************/
extern int PDP_ACT_DRV_CALLBACK(void);

typedef void(* USB_NET_DEV_SWITCH_GATEWAY)
(
    void
);

/*****************************************************************************
 函 数 名  : BSP_USB_NASSwitchGatewayRegExtFunc
 功能描述  : 本接口用于NAS注册切换网关通知回调函数
 输入参数  :回调接口。
 输出参数  : 无。
 返 回 值  : 0：成功
                       非零:失败
*****************************************************************************/
extern int BSP_USB_NASSwitchGatewayRegFunc(USB_NET_DEV_SWITCH_GATEWAY switchGwMode) ;
#define DRV_USB_NAS_SWITCH_GATEWAY_REGFUNC(switchGwMode)    BSP_USB_NASSwitchGatewayRegFunc(switchGwMode)
/*闪电卡还未开发 先暂时API 打桩end*/

/*************************USB END*************************************/


/*************************OM START**********************************/

#define PS_L2_UL_TRACE           (0)
#define PS_L2_DL_TRACE           (1)

#define PROCESS_ENTER(pADDRESS)  do { } while (0)
#define PROCESS_EXIT(pADDRESS)   do { } while (0)

typedef enum{
     ADDRTYPE8BIT,
     ADDRTYPE16BIT,
     ADDRTYPE32BIT
}ENADDRTYPE;

typedef enum{
     BOARD_TYPE_LTE_ONLY    = 0,
     BOARD_TYPE_GUL
}BOARD_TYPE_E;

typedef enum{
     BSP_MODEM_CORE         = 0,
     BSP_APP_CORE
}BSP_CORE_TYPE_E;

typedef enum{
     PV500_CHIP             = 0,
     V7R1_CHIP              = 1,
     PV500_PILOT_CHIP,
     V7R1_PILOT_CHIP
}BSP_CHIP_TYPE_E;

/******************************************************************************
* Function     :   BSP_OM_RegRead
*
* Description  :
*
* Input        :
*
* Output       :   无
*
* return       :
******************************************************************************/
BSP_S32 BSP_OM_RegRead(BSP_U32 u32RegAddr, ENADDRTYPE enAddrType, BSP_U32 *pu32Value);


/******************************************************************************
* Function     :   BSP_OM_RegWrite
*
* Description  :
*
* Input        :
*
* Output       :   无
*
* return       :
******************************************************************************/
BSP_S32 BSP_OM_RegWrite(BSP_U32 u32RegAddr, ENADDRTYPE enAddrType, BSP_U32 u32Value);

/******************************************************************************
* Function     :   BSP_OM_SoftReboot
*
* Description  :
*
* Input        :
*
* Output       :   无
*
* return       :
******************************************************************************/
BSP_VOID BSP_OM_SoftReboot(void);

/*****************************************************************************
* 函 数 名  : BSP_OM_GetBoardType
*
* 功能描述  : 获取单板类型
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : BSP单板类型枚举
*
* 其它说明  : 无
*
*****************************************************************************/
BOARD_TYPE_E BSP_OM_GetBoardType(BSP_VOID);

/*****************************************************************************
* 函 数 名  : BSP_GetHostCore
*
* 功能描述  : 查询当前CPU主从核类型
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : CPU主从核类型
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_CORE_TYPE_E BSP_GetHostCore(BSP_VOID);

/*****************************************************************************
* 函 数 名  : BSP_OM_GetChipType
*
* 功能描述  : 获取芯片类型
*
* 输入参数  : BSP_VOID
*
* 输出参数  : 无
*
* 返 回 值  : 芯片类型
*             PV500_CHIP:PV500芯片
*             V7R1_CHIP: V7R1芯片
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_CHIP_TYPE_E BSP_OM_GetChipType(BSP_VOID);

typedef struct tagBSP_OM_NET_S
{
    BSP_U32 u32NetRxStatOverFlow;       /* 接收FIFO溢出统计计数 */
    BSP_U32 u32NetRxStatPktErr;         /* 接收总错包计数 */
    BSP_U32 u32NetRxStatCrcErr;         /* 接收CRC错包计数 */
    BSP_U32 u32NetRxStatLenErr;         /* 接收无效长度包计数 */
    BSP_U32 u32NetRxNoBufInt;           /* 接收没有BUFFER中断计数 */
    BSP_U32 u32NetRxStopInt;            /* 接收停止中断计数 */
    BSP_U32 u32NetRxDescErr;            /* 接收描述符错误 */

    BSP_U32 u32NetTxStatUnderFlow;      /* 发送FIFO下溢统计计数 */
    BSP_U32 u32NetTxUnderFlowInt;       /* 发送FIFO下溢中断计数 */
    BSP_U32 u32NetTxStopInt;            /* 发送停止中断计数 */
    BSP_U32 u32NetTxDescErrPs;          /* 发送描述符错误(Ps) */
    BSP_U32 u32NetTxDescErrOs;          /* 发送描述符错误(Os) */
    BSP_U32 u32NetTxDescErrMsp;         /* 发送描述符错误(Msp) */

    BSP_U32 u32NetFatalBusErrInt;      /* 总线错误*/
}BSP_OM_NET_S;

/******************************************************************************
* Function     :   BSP_OM_NET
* 
* Description  :   
* 
* Input        :  
* 
* Output       :   无
* 
* return       :   
******************************************************************************/
BSP_VOID BSP_OM_NET(BSP_OM_NET_S *pstNetOm);

/*************************OM END  **********************************/

/*************************MNTN START**********************************/
/*VERSIONINFO_I数据结构中版本字符串最大有效字符长度*/
#define VER_MAX_LENGTH                  30

/*memVersionCtrl接口操作类型*/
#define VERIONREADMODE                  0
#define VERIONWRITEMODE                 1

#define OM_SAVE_EXCHFILE_ING            0
#define OM_SAVE_EXCHFILE_END            1


/*组件类型*/
typedef enum
{
    VER_BOOTLOAD = 0,
    VER_BOOTROM =1,
    VER_NV =2 ,
    VER_VXWORKS =3,
    VER_DSP =4 ,
    VER_PRODUCT_ID =5 ,
    VER_WBBP =6 ,
    VER_PS =7,
    VER_OAM =8,
    VER_GBBP =9 ,
    VER_SOC =10,
    VER_HARDWARE =11,
    VER_SOFTWARE =12,
    VER_MEDIA =13,
    VER_APP =14,
    VER_ASIC =15,
    VER_RF =16,
    VER_PMU =17,
    VER_PDM = 18,
    VER_PRODUCT_INNER_ID = 19,
    VER_INFO_NUM =20
}COMP_TYPE_I;

/* 三色灯状态编号*/
enum
{
    LED_LIGHT_SYSTEM_STARTUP,
    LED_LIGHT_POWER_ON,
    LED_LIGHT_G_REGISTERED,
    LED_LIGHT_W_REGISTERED,
    LED_LIGHT_G_CONNECTED,
    LED_LIGHT_W_CONNNECTED,
    LED_LIGHT_H_CONNNECTED,
    LED_LIGHT_OFFLINE,
    LED_LIGHT_FORCE_UPDATING,
    LED_LIGHT_NORMAL_UPDATING,
    LED_LIGHT_UPDATE_FAIL,
    LED_LIGHT_UPDATE_SUCCESS,
    LED_LIGHT_UPDATE_FILEFAIL,
    LED_LIGHT_UPDATE_NVFAIL,
    LED_LIGHT_SIM_ABSENT,
    LED_LIGHT_SHUTDOWN,
    LED_LIGHT_G_PLMN_SEARCH,
    LED_LIGHT_W_PLMN_SEARCH,
    LED_LIGHT_L_REGISTERED,
    LED_LIGHT_STATE_MAX = 32
};/*the state of LED */

enum SECURE_SUPPORT_STATUS_I
{
    SECURE_NOT_SUPPORT = 0,
    SECURE_SUPPORT = 1
};

enum SECURE_ENABLE_STATUS_I
{
    SECURE_DISABLE = 0,
    SECURE_ENABLE = 1
};

typedef enum                /* CACHE_TYPE */
{
    PS_OSAL_INSTRUCTION_CACHE ,
    PS_OSAL_DATA_CACHE
} PS_OSAL_CACHE_TYPE_I;

typedef int  (*OM_SAVE_FUNC)(int funcType, unsigned char *data, unsigned int* ulLength);

typedef struct
{
    unsigned char CompId;              /* 组件号：参见COMP_TYPE */
    unsigned char CompVer[VER_MAX_LENGTH+1];         /* 最大版本长度 30 字符+ \0 */
}VERSIONINFO_I;

typedef struct
{
    unsigned char enHSDSCHSupport;          /*硬件是否支持HSDPA*/
    unsigned char ucHSDSCHPhyCategory;      /*硬件支持的最大HSDPA能力等级*/
    unsigned char enEDCHSupport;            /*硬件是否支持HSUPA*/
    unsigned char ucEDCHPhyCategory;        /*硬件支持的最大HSUPA能力等级*/
    unsigned int  reserved;                 /*预留*/
}SUPPORT_MAX_HSPA_CAPA_STRU;

/*****************************BSP_MspProcReg  begin******************************/
typedef enum tagMSP_PROC_ID_E
{
    OM_REQUEST_PROC = 0,
    OM_ADDSNTIME_PROC = 1,
    OM_PRINTF_WITH_MODULE = 2,
    OM_PRINTF = 3,

    OM_PRINTF_GET_MODULE_IDLEV = 4,
    OM_READ_NV_PROC = 5,
    OM_WRITE_NV_PROC = 6,
    OM_MNTN_ERRLOG = 7, 

    MSP_PROC_REG_ID_MAX
}MSP_PROC_ID_E;

/*RF供电类型*/
typedef enum tagRF_VOLTAGE
{
    RF_VOLTAGE_18V,                                                          
    RF_VOLTAGE_22V,
    RF_VOLTAGE_MAX
}RF_VOLTAGE_E;


typedef void (*BSP_MspProc)(void);
/*****************************************************************************
* 函 数 名  : DRV_MSP_PROC_REG
*
* 功能描述  : DRV提供给OM的注册函数
*
* 输入参数  : MSP_PROC_ID_E eFuncID, BSP_MspProc pFunc
* 输出参数  : NA
*
* 返 回 值  : NA
*
* 其它说明  : 可维可测接口函数
*
*****************************************************************************/
extern void DRV_MSP_PROC_REG(MSP_PROC_ID_E eFuncID, BSP_MspProc pFunc);

/********************************************************************************************************
 函 数 名  : BSP_FS_GetDskspc
 功能描述  : 得到文件系统空间。
 输入参数  : path：文件系统路径。
 输出参数  : DskSpc：总空间。
                           UsdSpc：用过空间。
                           VldSpc：空闲空间。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
********************************************************************************************************/
extern int BSP_FS_GetDskspc(const char *path,unsigned int *DskSpc,unsigned int  *UsdSpc,  unsigned int *VldSpc);
#define DRV_FILE_GET_DISKSPACE(path,DskSpc,UsdSpc,VldSpc)    BSP_FS_GetDskspc(path,DskSpc,UsdSpc,VldSpc)


/*****************************************************************************
 函 数 名  : BSP_MNTN_GetFlashSpec
 功能描述  : Get flash infomation
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern int BSP_MNTN_GetFlashSpec(unsigned char * pFlashInfo, unsigned int ulLength);
#define DRV_GET_FLASH_INFO(pFlashInfo, usLength)    BSP_MNTN_GetFlashSpec (pFlashInfo, usLength )


/************************************************************************
 * FUNCTION
 *       max_freeblock_size_get
 * DESCRIPTION
 *       get memory max free block size
 * INPUTS
 *       无
 * OUTPUTS
 *       max free block size
 *************************************************************************/
extern int DRV_GET_FREE_BLOCK_SIZE(void);

/*****************************************************************************
 函 数 名  : BSP_MNTN_UartRecvCallbackRegister
 功能描述  : 提供上层应用程序注册数据接收回调函数指针的API接口函数。
 输入参数  : uPortNo：串口实例号。
             pCallback：回调函数指针。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int  BSP_MNTN_UartRecvCallbackRegister(unsigned char  uPortNo, pUartRecv pCallback);
#define DRV_UART_RCV_CALLBACK_REGI(uPortNo,pCallback)\
                BSP_MNTN_UartRecvCallbackRegister(uPortNo,pCallback)

/*************************************************
 函 数 名   : BSP_MNTN_ProductTypeGet
 功能描述   : 返回当前产品类型
 输入参数   : 无
 输出参数   : 无
 返 回 值   :0:STICK
             1:MOD
             2:E5
             3:CPE
*************************************************/
extern BSP_U32 BSP_MNTN_ProductTypeGet(void);
#define DRV_PRODUCT_TYPE_GET()   BSP_MNTN_ProductTypeGet()


/*************************************************
 函 数 名   : DRV_HKADC_BAT_VOLT_GET
 功能描述   : 返回当前电池电压值
 输入参数   : pslData : 电池电压值
 输出参数   : pslData : 电池电压值
 返 回 值   :0:获取成功
            -1:获取失败
*************************************************/
extern BSP_S32 DRV_HKADC_BAT_VOLT_GET(BSP_S32 *ps32Data);


/*****************************************************************************
* 函 数 名  : BSP_HKADC_PaValueGet
*
* 功能描述  :
*
* 输入参数  :
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2011-3-29 wuzechun creat
*
*****************************************************************************/
BSP_S32 BSP_HKADC_PaValueGet( BSP_U16 *pusValue );


#define BUILD_DATE_LEN  12
#define BUILD_TIME_LEN  12

typedef struct  tagUE_SW_BUILD_VER_INFO_STRU
{
    BSP_U16 ulVVerNo;        // V部分
    BSP_U16 ulRVerNo;        // R部分
    BSP_U16 ulCVerNo;        // C部分
    BSP_U16 ulBVerNo;        // B部分
    BSP_U16 ulSpcNo;         // SPC部分
    BSP_U16 ulCustomVer;     // 客户需定制部分, 针对硬件接口低8bit PCB号,高8bitHW号
    BSP_U32 ulProductNo;     // such as porting,CPE, ...
    BSP_S8 acBuildDate[BUILD_DATE_LEN];  // build日期,
    BSP_S8 acBuildTime[BUILD_TIME_LEN];  // build时间
} UE_SW_BUILD_VER_INFO_STRU; // 内部版本


typedef struct {
    BSP_U32   bandGU;         	/*支持的GU频段*/
    BSP_U32   bamdLTELow;         /*支持LTE频段，B1~B32*/
    BSP_U32   bamdLTEHigh;        /*支持LTE频段，B33~B64*/
}BAND_INFO_T;

typedef struct {
    BSP_U16  index;         /*硬件版本号数值(大版本号1+大版本号2)，区分不同产品*/
    BSP_U16   hwIdSub;         /*硬件子版本号，区分产品的不同的版本*/
    BSP_CHAR* name;           /*内部产品名*/
    BSP_CHAR* namePlus;       /*内部产品名PLUS*/
    BSP_CHAR* hwVer;          /*硬件版本名称*/
    BSP_CHAR* dloadId;        /*升级中使用的名称*/
    BSP_CHAR* productId;      /*外部产品名*/
    BAND_INFO_T  band;            /*产品支持的频段*/
    BAND_INFO_T  bandDiv;         /*产品支持的分集*/
    BSP_CHAR reserve[4];      /*预留*/
}PRODUCT_INFO_T;

/*****************************************************************************
* 函 数 名  : BSP_HKADC_PaValueGet
*
* 功能描述  :
*
* 输入参数  :
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2011-3-29 wuzechun creat
*
*****************************************************************************/
const UE_SW_BUILD_VER_INFO_STRU* BSP_GetBuildVersion(BSP_VOID);


/*****************************************************************************
* 函 数 名  : BSP_HwGetHwVersion
*
* 功能描述  : 获取硬件版本名称
*
* 输入参数  : BSP_CHAR* pHwVersion,字符串指针，保证不小于32字节
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2011-3-29 wuzechun creat
*
*****************************************************************************/
BSP_S32 BSP_HwGetHwVersion (char* pFullHwVersion, BSP_U32 ulLength);


/*****************************************************************************
* 函 数 名  : BSP_GetSupportBands
*
* 功能描述  : 获取支持的频段
*
* 输入参数  : BAND_INFO_T *pBandInfo,频段信息结构体指针
* 输出参数  :
*
 返 回 值  : 0：正确，非0: 失败
*
* 其它说明  :
*
*****************************************************************************/
BSP_S32 BSP_GetSupportBands( BAND_INFO_T *pBandInfo);

/*****************************************************************************
* 函 数 名  : BSP_HwIsSupportWifi
*
* 功能描述  : 打印硬件版本信息
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_BOOL BSP_HwIsSupportWifi(BSP_VOID);

/*****************************************************************************
 函 数 名  : hkadcBatADCRead
 功能描述  : 获取电池电压采样值
 输入参数  : 无
 输出参数  : pTemp：        指向电池温度的指针。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern BSP_S32 DRV_GET_BATTERY_ADC(BSP_S32 * pslData);


/*****************************************************************************
 函 数 名  : BSP_PMU_LDOOFF
 功能描述  : 本接口用于关闭某路LDO的电压输出。
 输入参数  : ucLDO：指示某路LDO电压。取值范围为1～12。
 输出参数  : 无。
 返 回 值  : 无。
 注意事项　：LDO6由硬件控制，软件不支持对LDO6的操作。如果对LDO1关闭，则系统会关机。
*****************************************************************************/
extern void BSP_PMU_LDOOFF(BSP_U8 u8LDO);
#define DRV_PM_LDO_OFF(u8LDO)                 BSP_PMU_LDOOFF(u8LDO)

/*****************************************************************************
 函 数 名  : BSP_PMU_LDOON
 功能描述  : 本接口用于设置打开某路LDO的电压输出。
 输入参数  : ucLDO：指示某路LDO电压。取值范围为2～12。
 输出参数  : 无。
 返 回 值  : 无。
 注意事项　：不支持对LDO1，LDO6的操作。LDO1是在系统开机时自动打开，一旦关闭则整个系统会下电；LDO6是由硬件管脚控制。
*****************************************************************************/
extern void BSP_PMU_LDOON(BSP_U8 u8LDO);
#define DRV_PM_LDO_ON(u8LDO)                  BSP_PMU_LDOON(u8LDO)

/************************************
函 数 名  : BSP_PMU_AptEnable
功能描述  : 使能PMU的APT功能
输入参数  : 无
输出参数  : 返回设置状态
返 回 值  : BSP_OK，设置成功
		其他值：设置失败
**************************************/
extern BSP_S32 BSP_PMU_AptEnable(BSP_VOID);
#define DRV_PMU_APT_ENABLE()    BSP_PMU_AptEnable()

/************************************
函 数 名  : BSP_PMU_AptDisable
功能描述  : 去使能PMU的APT功能
输入参数  : 无
输出参数  : 返回设置状态
返 回 值  : BSP_OK，设置成功
		其他值：设置失败
**************************************/
extern BSP_S32 BSP_PMU_AptDisable(BSP_VOID);
#define DRV_PMU_APT_DISABLE()    BSP_PMU_AptDisable()

/************************************
函 数 名  : BSP_PMU_AptIsEnable
功能描述  : 查询PMU的APT功能是否使能
输入参数  : 无
输出参数  : 返回设置状态
返 回 值  : BSP_TURE，使能APT
		    BSP_FALSE,未使能APT
**************************************/
extern BSP_BOOL  BSP_PMU_AptIsEnable(BSP_VOID);
#define DRV_PMU_APT_ISENABLE()    BSP_PMU_AptIsEnable()

extern BSP_U32 DRV_ADJUST_RF_VOLTAGE(RF_VOLTAGE_E enRfVol, BSP_U32 u32Voltage);

/*****************************************************************************
 函 数 名  : BSP_ONOFF_DrvPowerOff
 功能描述  : 单板直接下电
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
extern void DRV_POWER_OFF(void);


/*****************************************************************************
 函 数 名  : BSP_MNTN_GetGsmPATemperature
 功能描述  : 获取GSM PA温度
 输入参数  : Temprature
             hkAdcTalble
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_MNTN_GetGsmPATemperature(int *temperature, unsigned short *hkAdcTable);
#define DRV_GET_PA_GTEMP(temperature, hkAdcTable)   \
                BSP_MNTN_GetGsmPATemperature(temperature, hkAdcTable)

/*****************************************************************************
 函 数 名  : getWcdmaPATemperature
 功能描述  : 获取WCDMA PA温度
 输入参数  : Temprature
             hkAdcTalble
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_MNTN_GetWcdmaPATemperature (int *temperature, unsigned short *hkAdcTable);
#define  DRV_GET_PA_WTEMP(temperature, hkAdcTable)  \
              BSP_MNTN_GetWcdmaPATemperature(temperature, hkAdcTable)

/*****************************************************************************
 函 数 名  : BSP_MNTN_GetSIMTemperature
 功能描述  : 获取SIM卡温度
 输入参数  : pusHkAdcTable：电磁电压表
 输出参数  : pTemp：        指向SIM卡温度的指针。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_MNTN_GetSIMTemperature(int *plTemp, unsigned short *pusHkAdcTable);
#define DRV_GET_SIM_TEMP(plTemp, pusHkAdcTable)   \
               BSP_MNTN_GetSIMTemperature(plTemp, pusHkAdcTable)

/*****************************************************************************
 函 数 名  : void BSP_PWRCTRL_DfsCpuUp(void)
 功能描述  : Set CPU freq to 450M
 输入参数  : 
 输出参数  : 
 返 回 值  : 
*****************************************************************************/
extern void BSP_PWRCTRL_DfsCpuUp(void);
#define BSP_PWRCTRL_DFS_CPUUP() BSP_PWRCTRL_DfsCpuUp()

/*****************************************************************************
 函 数 名  : void BSP_PWRCTRL_DfsThrProOpen(void)/
 			 void BSP_PWRCTRL_DfsThrProClose(void)
 功能描述  : open CPU freq to THREE PROFILES MODE
 输入参数  : 
 输出参数  : 
 返 回 值  : 
*****************************************************************************/

extern void BSP_PWRCTRL_DfsThrProOpen(void);
#define BSP_PWRCTRL_DFS_THRPRO_OPEN() BSP_PWRCTRL_DfsThrProOpen()
/*****************************************************************************
 函 数 名  : void BSP_PWRCTRL_DfsThrProClose(void)
 功能描述  : CLOSE CPU freq to THREE PROFILES MODE
 输入参数  : 
 输出参数  : 
 返 回 值  : 
*****************************************************************************/
extern void BSP_PWRCTRL_DfsThrProClose(void);
#define BSP_PWRCTRL_DFS_THRPRO_CLOSE() BSP_PWRCTRL_DfsThrProClose()

/*****************************************************************************
Function:   BSP_PWRCTRL_GetCpuFreq(void)
Description:Get CPU frequency
Input:      None
Output:     None
Return:     None
Others:
*****************************************************************************/
extern int BSP_PWRCTRL_GetCpuFreq();
#define BSP_PWRCTRL_GET_CPUFREQ() BSP_PWRCTRL_GetCpuFreq()


/*****************************************************************************
 函 数 名  : BSP_DFS_GetCurCpuLoad
 功能描述  : 查询当前CPU
 输入参数  : pu32AcpuLoad ACPUload指针
             pu32CcpuLoad CCPUload指针
 输出参数  : pu32AcpuLoad ACPUload指针
             pu32CcpuLoad CCPUload指针
 返 回 值  : 0:  操作成功；
            -1：操作失败。
*****************************************************************************/
extern BSP_U32 BSP_DFS_GetCurCpuLoad(BSP_U32 *pu32AcpuLoad,BSP_U32 *pu32CcpuLoad);
#define DRV_GET_CUR_CPU_LOAD(pu32AcpuLoad,pu32CcpuLoad) BSP_DFS_GetCurCpuLoad(pu32AcpuLoad,pu32CcpuLoad)


/*****************************************************************************
 函 数 名  : BSP_GU_GetVerTime
 功能描述  : 获取版本编译时间
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
extern char *BSP_GU_GetVerTime(void);
#define DRV_GET_VERSION_TIME()    BSP_GU_GetVerTime()

/*****************************************************************************
 函 数 名  : BSP_MNTN_MemVersionCtrl
 功能描述  : 组件版本读写接口。
 输入参数  : pcData：当Mode为读的时候，为调用者待保存返回的组件版本信息的内存地址；
                     当Mode为写的时候，为调用者准备写入的组件版本信息的地址。
             ucLength：当Mode为读的时候，为调用者待保存返回的组件版本信息的内存大小；
                       当Mode为写的时候，为调用者准备写入的组件版本信息的字符数（不包括'\0'）。
             ucType：版本信息ID，
                     0： BOOTLOAD；
                     1： BOOTROM；
                     2： NV；
                     3： VXWORKS；
                     4： DSP；
                     5； CDROMISO；
                     6： PHY；
                     7： PS；
                     8： COM；
                     9： RF；
                     10：SOC；
                     11：HARDWARE；
                     12：SOFTWARE；
                     13：MEDIA；
                     14：APP；
                     15：INFO_NUM。
             ucMode：0：读取指定ID的组件版本信息；1：写入指定ID的组件版本信息。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_MNTN_MemVersionCtrl(signed char *pcData, unsigned char ucLength, unsigned char ucType, unsigned char ucMode);
#define DRV_MEM_VERCTRL(pcData,ucLength,ucType,ucMode)  \
                          BSP_MNTN_MemVersionCtrl(pcData,ucLength,ucType,ucMode)

/********************************************************************************************************
 函 数 名  : getHwVersionIndex
 功能描述  : 获取由HKADC读取的硬件ID经过拼装后的硬件索引值
 输入参数  : 无
 输出参数  : 硬件ID的索引值

 返 回 值  : 非-1:  操作成功；
             -1：操作失败。
********************************************************************************************************/
extern int getHwVersionIndex(void);
#define DRV_GET_HW_VERSION_INDEX()       getHwVersionIndex()

/*****************************************************************************
 函 数 名  : BSP_MNTN_GetFullHwVersion
 功能描述  : 硬件完整版本读接口。
 输入参数  : pFullHwVersion：为调用者待保存返回的硬件完整版本信息的内存首地址；
             ulLength      ：为调用者待保存返回的硬件完整版本信息的内存大小；
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_MNTN_GetFullHwVersion(char * pFullHwVersion,unsigned int ulLength);
#define  DRV_GET_FULL_HW_VER(pFullHwVersion,ulLength)  \
               BSP_MNTN_GetFullHwVersion(pFullHwVersion,ulLength)

/*****************************************************************************
 函 数 名  : BSP_MNTN_GetProductIdInter
 功能描述  : 产品名称完整版本读接口。
 输入参数  : pProductIdInter：为调用者待保存返回的产品名称完整版本的内存首地址；
             ulLength       ：为调用者待保存返回的产品名称完整版本的内存大小；
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_MNTN_GetProductIdInter(char * pProductIdInter, unsigned int ulLength);
#define  DRV_GET_PRODUCTID_INTER_VER(pProductIdInter,ulLength)  \
              BSP_MNTN_GetProductIdInter(pProductIdInter,ulLength)

/*****************************************************************************
 函 数 名  : BSP_MNTN_VersionQueryApi
 功能描述  : 查询所有组件的版本号。
 输入参数  : ppVersionInfo：待保存的版本信息地址。
 输出参数  : ucLength：待返回的数据的字节数。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_MNTN_VersionQueryApi(void ** ppVersionInfo, unsigned int * ulLength);
#define  DRV_VER_QUERY(ppVersionInfo,ulLength)  \
              BSP_MNTN_VersionQueryApi (ppVersionInfo,ulLength)



typedef enum DRV_SHUTDOWN_REASON_tag_s
{
    DRV_SHUTDOWN_LOW_BATTERY,           /* 电池电量低                 */
    DRV_SHUTDOWN_BATTERY_ERROR,         /* 电池异常                   */
    DRV_SHUTDOWN_POWER_KEY,             /* 长按 Power 键关机          */
    DRV_SHUTDOWN_TEMPERATURE_PROTECT,   /* 过温保护关机               */
    DRV_SHUTDOWN_LOW_TEMP_PROTECT,
    DRV_SHUTDOWN_RESET,                 /* 系统软复位                 */
    DRV_SHUTDOWN_CHARGE_REMOVE,         /* 关机充电模式下，拔除充电器 */
    DRV_SHUTDOWN_UPDATE,                /* 关机并进入升级模式         */
    DRV_SHUTDOWN_BUTT
}DRV_SHUTDOWN_REASON_ENUM;

#ifdef FEATURE_HUAWEI_MBB_CHG
/*BATTERY(电池)主动上报的事件*/
typedef enum _GPIO_BAT_EVENT
{
    GPIO_BAT_CHARGING_ON_START = 0,  /* 在开机模式，开始充电 */
    GPIO_BAT_CHARGING_OFF_START,     /* 在关机模式，开始充电 */   
    GPIO_BAT_LOW_POWER,              /* 电池电量低时，上报此事件 */
    GPIO_BAT_CHARGING_UP_STOP,       /* 开机停止充电时，上报此事件 */
    GPIO_BAT_CHARGING_DOWN_STOP,     /* 关机停止充电时，上报此事件 */
    GPIO_BAT_LOW_POWEROFF = 9,           /* 电池电量低导致的关机，上报此事件*/       /* BEGIN: PN:归一化充电新规格 Modified by d00135750, 2012/12/20   */
    GPIO_BAT_EVENT_MAX               /* 如果事件值不小于此值，则为非法事件 */
}GPIO_BATTERY_EVENT;

/*电池高温上报事件*/
typedef enum _TEMP_EVENT
{
   GPIO_TEMP_BATT_LOW,   /* 电池低温事件 */      
   GPIO_TEMP_BATT_HIGH,   /* 电池高温事件 */ 
   GPIO_TEMP_BATT_NORMAL, /* 电池高温解除事件 */
   GPIO_TEMP_BATT_MAX     /* 如果事件值不小于此值，则为非法事件*/
}GPIO_TEMP_EVENT;

/*KEY主动上报的事件*/
typedef enum _GPIO_KEY_EVENT
{
    GPIO_KEY_WAKEUP = 0,           /* 短按任意按键，上报此事件，用于点亮屏幕 */    
    GPIO_KEY_WLAN,                 /* 用于打开/关闭WLAN设备，不同的版本使用的按键不同 */
    GPIO_KEY_WPS,                  /* 长按WPS键（大于5秒）时，上报此事件，用于激活WPS功能 */
    GPIO_KEY_FACTORY_RESTORE,      /* 长按复位键时，上报此事件，用于恢复出厂设置 */
    GPIO_KEY_POWER_OFF,            /* 长按POWER键时，上报此事件，用于关机 */
    GPIO_KEY_UPDATE,               /* 当系统需要升级时，上报此事件，用于升级系统 */
    GPIO_KEY_SSID,                 /* 短按WPS键（小于5秒）时，用于显示SSID，只有部分版本需要 */
    GPIO_KEY_WPS_PROMPT,           /* 短按WPS键（大于1秒）时，用于显示WPS提示语，只有部分版本需要 */ 
    GPIO_KEY_POWER_SHORT,          /* 短按Power按键，500毫秒内要松开按键 */
    GPIO_KEY_WPS_SHORT,            /* 短按WPS按键，500毫秒内要松开按键 */
    GPIO_KEY_RESET_SHORT,          /* 短按Reset按键，500 毫秒内要松开按键 */
    GPIO_KEY_WPS_POWER,            /* WPS和POWER 键一起按下2秒 */
    GPIO_KEY_WPS_POWER_UP,         /* WPS和POWER 键一起按下2秒后在5秒前抬起，或者5秒后未在500毫秒内抬起按下WPS */
    GPIO_KEY_POWER_ONE_SECOND,     /* Power按下1秒 */
    GPIO_KEY_POWER_ONE_SECOND_UP,  /* Power按下1秒后在2秒前抬起 */
    GPIO_KEY_WPS_ONE_SECOND,       /*WPS 短按 1秒事件，主要用于开关 WiFi Offload*/
    GPIO_KEY_WPS_DIAL= 16,         /*WPS 短按松开上报 WPS 拨号事件，主要用于按键拨号*/  
#ifdef FEATURE_FUNC_FAST_BOOT
    /*平台提供5秒快速关机事件*/
    GPIO_KEY_SHORT_POWER_OFF = 17,
    /*平台提供5秒快速开机事件*/
    GPIO_KEY_SHORT_POWER_ON = 18,
 #endif
    GPIO_KEY_EVENT_MAX             /* 如果事件值不小于此值，则为非法事件 */
}GPIO_KEY_EVENT;
#endif /* FEATURE_HUAWEI_MBB_CHG */

/*****************************************************************************
 函 数 名  : DRV_SHUT_DOWN
 功能描述  : 单板关机
 输入参数  : 无
 输出参数  : eReason：        关机原因
 返 回 值  : 无
*****************************************************************************/
void drvShutdown( DRV_SHUTDOWN_REASON_ENUM eReason );
void DRV_SHUT_DOWN( DRV_SHUTDOWN_REASON_ENUM eReason );

typedef enum tagMNTN_FILE_TYPE_E
{
	MNTN_TYPE_ZSP = 0,
	MNTN_TYPE_HIFI,
    MNTN_TYPE_MAX
}MNTN_FILE_TYPE_E;

/*****************************************************************************
 函 数 名  : BSP_MNTN_ABBSelfCheck
 功能描述  : 返回abb自检结果
 输入参数  : None
 输出参数  : None
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_MNTN_ABBSelfCheck(void);
#define DRV_ABB_SELFCHECK()    BSP_MNTN_ABBSelfCheck()

/*****************************************************************************
 函 数 名  : BSP_MNTN_PmuSelfCheck
 功能描述  : PMU自检。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_MNTN_PmuSelfCheck(void);
#define DRV_PMU_SELFCHECK()    BSP_MNTN_PmuSelfCheck()

/*****************************************************************************
 函 数 名  : BSP_MNTN_I2cSelfCheck
 功能描述  : 返回I2c自检结果
 输入参数  : None
 输出参数  : None
 返 回 值  : 0:  操作成功；
             -1：操作失败。
 注意事项　：该接口仅在PS的PC工程中使用，目前没有应用，暂保留。
*****************************************************************************/
extern int BSP_MNTN_I2cSelfCheck(void);
#define DRV_I2C_SELFCHECK()    BSP_MNTN_I2cSelfCheck()

/*****************************************************************************
 函 数 名  : BSP_MNTN_SpiSelfCheck
 功能描述  : 返回spi自检结果
 输入参数  : None
 输出参数  : None
 返 回 值  : 0:  操作成功；
             -1：操作失败。
 注意事项　：该接口仅在PS的PC工程中使用，目前没有应用，暂保留。
*****************************************************************************/
extern int BSP_MNTN_SpiSelfCheck(void);
#define DRV_SPI_SELFCHECK()    BSP_MNTN_SpiSelfCheck()

/*****************************************************************************
 函 数 名  : BSP_MNTN_GetCodecFuncStatus
 功能描述  : 获取CODEC功能状态
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 1:  CODEC存在
             0：CODEC 不存在
*****************************************************************************/
extern  int BSP_MNTN_GetCodecFuncStatus(void);
#define DRV_GET_CODEC_FUNC_STATUS()    BSP_MNTN_GetCodecFuncStatus()

/*****************************************************************************
 函 数 名  : MNTN_RFGLockStateGet
 功能描述  : 读取GSM RF锁定状态。
 输入参数  : 无。
 输出参数  : Status：存放锁定状态的输出值，
                        0：TX或者RX被锁定。
                        1：TX、RX都没有被锁定；

 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_MNTN_RFGLockStateGet(unsigned int *Status);
#define DRV_GET_RF_GLOCKSTATE(Status)    BSP_MNTN_RFGLockStateGet(Status)

/****************************************************************************
 函 数 名  : BSP_MNTN_LedStateFuncReg
 功能描述  : 本接口为提供给协议栈的钩子函数，用于获取协议栈的点灯状态。
 输入参数  : 协议栈获取点灯状态函数的指针。
 输出参数  : 无。
 返 回 值  : 无。
 注意事项  : 无。

******************************************************************************/
extern void BSP_MNTN_LedStateFuncReg (pFUNCPTR p);
#define DRV_LED_STATE_FUNREG(p)    BSP_MNTN_LedStateFuncReg(p)

/*****************************************************************************
 函 数 名  : BSP_MNTN_LedFlush
 功能描述  : 三色灯设置。
 输入参数  : status：三色灯的状态，见LED_STATUS_I
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_MNTN_LedFlush(unsigned long state);
#define DRV_LED_FLUSH(state)    BSP_MNTN_LedFlush(state)

/*****************************************************************************
 函 数 名  : BSP_MNTN_LedControlAddressGet
 功能描述  : 得到定制的点灯数据
 输入参数  : 无
 输出参数  : 无。
 返 回 值  : 点灯控制数据的全局变量地址
*****************************************************************************/
extern unsigned int BSP_MNTN_LedControlAddressGet(void);
#define DRV_LED_GET_ADDRESS()   BSP_MNTN_LedControlAddressGet()

/*****************************************************************************
 函 数 名  : BSP_MNTN_BootForceloadModeCheck
 功能描述  : 强制加载模式查询。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 0：强制加载；
             1：非强制加载。
*****************************************************************************/
extern unsigned int BSP_MNTN_BootForceloadModeCheck(void);
#define DRV_BOOT_FORCELOAD_MODE_CHECK()    BSP_MNTN_BootForceloadModeCheck()

/*****************************************************************************
 函 数 名  : BSP_MNTN_BootFlagConfig
 功能描述  : 配置Bootline数据中的启动标志为快速自启动、倒计时7秒自启动或手动启动。
 输入参数  : ulBootFlag：暂未定义。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern  int BSP_MNTN_BootFlagConfig(unsigned int ulBootFlag);
#define DRV_BOOTFLAG_CFG(ulBootFlag)    BSP_MNTN_BootFlagConfig(ulBootFlag)

/*****************************************************************************
 函 数 名  : BSP_MNTN_CheckArmTCM
 功能描述  : TCM检测
 输入参数  : 无。
 输出参数  : 无。
 返回值：   0xffff0000：检测成功，TCM正常
            地址：检测出错的地址（0－0x5fff）
            0xffffffff:检测失败

*****************************************************************************/
extern unsigned int BSP_MNTN_CheckArmTCM(void);
#define DRV_CHECK_ARM_TCM()    BSP_MNTN_CheckArmTCM()

/*****************************************************************************
 函 数 名  : BSP_MNTN_ExchOMSaveRegister
 功能描述  : 注册给底软的trace保存函数，用来在单板复位时，
             保存当前的信令和日志信息。
 输入参数  : funcType: 功能ID
             pFunc:注册的回调函数
 输出参数  : None
 返 回 值  : 0:  操作成功；
             -1：操作失败。
 注意事项　：相同的funcType只能注册一个回调函数，重复注册会覆盖原来函数
*****************************************************************************/
extern int BSP_MNTN_ExchOMSaveRegister(int funcType, OM_SAVE_FUNC *pFunc);
#define DRV_SAVE_REGISTER(funcType, pFunc)    BSP_MNTN_ExchOMSaveRegister(funcType, pFunc)

/*****************************************************************************
 函 数 名  : BSP_MNTN_ExchOMRegisterTdsPhy
 功能描述  : TDS注册给底软的内存地址，用于死机时记录TDS物理层信息到死机BIN文件。初始化时调用
 输入参数  : ulLength: 内存长度
            tphyaddr: 起始地址
 输出参数  : None
 返 回 值  : 
 注意事项　：
*****************************************************************************/
extern void BSP_MNTN_ExchOMRegisterTdsPhy(BSP_U32 ulLength,char * tphyaddr);


/*****************************************************************************
 函 数 名  : BSP_MNTN_SystemError
 功能描述  : 系统热启动。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_MNTN_SystemError(int modId, int arg1, int arg2, char * arg3, int arg3Length);
#define DRV_SYSTEM_ERROR(modId, arg1, arg2, arg3, arg3Length)\
                   BSP_MNTN_SystemError(modId, arg1, arg2, arg3, arg3Length)

/*****************************************************************************
 函 数 名  : BSP_MNTN_ExchMemMalloc
 功能描述  : 与systemOsaError接口一起用来记录大块的复位信息。
             目前最多支持128k.
 输入参数  : ulSize - 内存大小
 输出参数  : 无。
 返 回 值  : 内存地址
*****************************************************************************/
extern unsigned int BSP_MNTN_ExchMemMalloc(unsigned int ulSize);
#define DRV_EXCH_MEM_MALLOC(ulSize)    BSP_MNTN_ExchMemMalloc(ulSize)

/*****************************************************************************
 函 数 名  : BSP_MNTN_OmExchFileSave
 功能描述  : 用来记录ARM和DSP交互的异常文件
 输入参数  :    address：buffer地址
                length：存储长度
                IsFileEnd：1表示文件的最后一块，0表示非最后一块
 输出参数  : 无。
 返 回 值  : 0 为OK。
*****************************************************************************/
extern int BSP_MNTN_OmExchFileSave(void * address, unsigned long length,unsigned char IsFileEnd,unsigned char type);
#define DRV_EXCH_FILE_SAVE(address, length, IsFileEnd, type) BSP_MNTN_OmExchFileSave(address, length, IsFileEnd, type)

/*****************************************************************************
 函 数 名  : BSP_MNTN_GetHwGpioInfo
 功能描述  : Get flash infomation
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern int BSP_MNTN_GetHwGpioInfo(unsigned char *pGpioInfo, unsigned long usLength );
#define DRV_GET_GPIO_INFO(pGpioInfo, usLength)    BSP_MNTN_GetHwGpioInfo(pGpioInfo, usLength )

/*****************************************************************************
 函 数 名  : BSP_MNTN_Int4ToString
 功能描述  : 将寄存器的值转换为ASCII字符
 输入参数  : 寄存器的值，和转换字符的存储空间,最大空间不超过30byte
 输出参数  : 转换字符
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_MNTN_Int4ToString(unsigned int value, char * string);
#define DRV_MEM_VERCONVERT(value,string)    BSP_MNTN_Int4ToString(value, string)

/*****************************************************************************
 函 数 名  : BSP_MNTN_HeapInfoGet
 功能描述  : 获取单板侧的内存信息。
 输入参数  : allocSize: 已经分配的堆内存大小，单位byte。
             totalSize: 堆内存总尺寸，单位byte。
 输出参数  : None
 返 回 值  : 0: 操作成功；
             -1：  操作失败。
*****************************************************************************/
extern int BSP_MNTN_HeapInfoGet(unsigned int *allocSize, unsigned int *totalSize);
#define DRV_GET_HEAPINFO(allocSize, totalSize)    BSP_MNTN_HeapInfoGet(allocSize, totalSize)

/*****************************************************************************
 函 数 名  : BSP_MNTN_GetSupportBands
 功能描述  : 从底软获得当前支持的Bands.
 输入参数  : pusWBands - WCDMA的频段值
             pusGBands - GSM的频段值

 输出参数  : pusWBands - WCDMA的频段值
             pusGBands - GSM的频段值

 返 回 值  : 0：正确，非0: 失败
             通过Bit位来表示哪些频段支持。
*****************************************************************************/
extern int BSP_MNTN_GetSupportBands(unsigned short *pusWBands, unsigned short *pusGBands);
#define DRV_GET_SUPPORT_BANDS(pusWBands, pusGBands)    BSP_MNTN_GetSupportBands(pusWBands, pusGBands)

/*****************************************************************************
 函 数 名  : BSP_MNTN_GetSupportDivBands
 功能描述  : 从底软获得当前支持的分级频段
 输入参数  : N/A

 输出参数  : pDivBands - 返回当前单板所支持的分级频段，频段按bit
 			      流方式排列(同W 和G的频段查询)

 返 回 值  : 0：正确，非0: 失败
*****************************************************************************/
extern int BSP_MNTN_GetSupportDivBands(unsigned short* pDivBands);
#define DRV_GET_SUPPORT_DIVBANDS(pDivBands)    BSP_MNTN_GetSupportDivBands(pDivBands)

/********************************************************************************************************
 函 数 名  : BSP_MNTN_WriteSysBackupFile
 功能描述  : 向FLASH备份ZSP、NV数据
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 0:  操作成功；
             -1：操作失败。
********************************************************************************************************/
extern int BSP_MNTN_WriteSysBackupFile(void);
#define DRV_BACKUP_SYS_FILE()    BSP_MNTN_WriteSysBackupFile()

/*****************************************************************************
 函 数 名  : DRV_MNTN_GetExtAntenLockState
 功能描述  : 获取有线与无线的连接状态
 输入参数  : None
 输出参数  : 0 - 无线连接
             1 - 有线连接
 返 回 值  : 0 - 成功
             其它为失败

*****************************************************************************/
extern int DRV_MNTN_GetExtAntenLockState(unsigned int *Status);
#define DRV_GET_ANTEN_LOCKSTATE(Status)    DRV_MNTN_GetExtAntenLockState(Status)

/*****************************************************************************
 函 数 名  : BSP_MNTN_ExtAntenIntInstall
 功能描述  : 中断注册函数，用来获得当前的天线状态
 输入参数  : routine   - 中断处理函数
             para      - 保留字段
 输出参数  : None
 返 回 值  : void

*****************************************************************************/
extern void BSP_MNTN_ExtAntenIntInstall(void* routine, int para);
#define DRV_ANTEN_INT_INSTALL(routine, para)    BSP_MNTN_ExtAntenIntInstall(routine, para)

/*****************************************************************************
 函 数 名  : BSP_MNTN_ExchHookFuncAdd
 功能描述  : 任务切换及中断钩子注册
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
extern void BSP_MNTN_ExchHookFuncAdd(void);
#define DRV_EXCH_HOOK_FUNC_ADD()    BSP_MNTN_ExchHookFuncAdd()

/*****************************************************************************
 函 数 名  : BSP_MNTN_ExchHookFuncDelete
 功能描述  : 任务切换及中断钩子注销
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
extern void BSP_MNTN_ExchHookFuncDelete(BSP_CHAR* file, BSP_U32 line);
#define DRV_EXCH_HOOK_FUNC_DELETE()    BSP_MNTN_ExchHookFuncDelete(__FILE__, __LINE__)

/*****************************************************************************
 函 数 名  : BSP_MNTN_ExcStackPeakRecord
 功能描述  : 记录任务栈超标的任务信息.
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_MNTN_ExcStackPeakRecord(void);
#define DRV_STACK_HIGH_RECORD()    BSP_MNTN_ExcStackPeakRecord()

/*****************************************************************************
 函 数 名  : BSP_MNTN_SubIntHook
 功能描述  : 二级中断服务钩子函数注册。可维可测相关
 输入参数  : p_Func：钩子函数指针。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_MNTN_SubIntHook(pFUNCPTR p_Func);
#define DRV_VICINT_HOOK_ADD(p_Func)    BSP_MNTN_SubIntHook(p_Func)

/*****************************************************************************
 函 数 名  : BSP_MNTN_SubIntHookDel
 功能描述  : 二级中断服务钩子函数注销。可维可测相关
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_MNTN_SubIntHookDel(void);
#define DRV_VICINT_HOOK_DEL()    BSP_MNTN_SubIntHookDel()

/*****************************************************************************
 函 数 名  : BSP_MNTN_IntBreakInHook
 功能描述  : 一级中断入口钩子函数注册。可维可测相关
 输入参数  : p_Func :钩子函数。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_MNTN_IntBreakInHook(pFUNCPTR p_Func);
#define DRV_VICINT_IN_HOOK_ADD(p_Func)   BSP_MNTN_IntBreakInHook(p_Func)

/*****************************************************************************
 函 数 名  : BSP_MNTN_IntBreakInHookDel
 功能描述  : 一级中断入口钩子函数注销。可维可测相关
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_MNTN_IntBreakInHookDel(void);
#define DRV_VICINT_IN_HOOK_DEL()    BSP_MNTN_IntBreakInHookDel()

/*****************************************************************************
 函 数 名  : BSP_MNTN_IntBreakOutHook
 功能描述  : 一级中断出口钩子函数注册。可维可测相关，V7先打桩
 输入参数  : p_Func: 钩子函数。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_MNTN_IntBreakOutHook(pFUNCPTR p_Func);
#define DRV_VICINT_OUT_HOOK_ADD(p_Func)    BSP_MNTN_IntBreakOutHook(p_Func)

/*****************************************************************************
 函 数 名  : vicIntBreakOutHookDel
 功能描述  : 一级中断出口钩子函数注销。可维可测相关
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_MNTN_IntBreakOutHookDel(void);
#define DRV_VICINT_OUT_HOOK_DEL()    BSP_MNTN_IntBreakOutHookDel()

/*****************************************************************************
 函 数 名  : BSP_MNTN_IntLvlChgHook
 功能描述  : 一级中断level change钩子函数注册。可维可测相关
 输入参数  : p_Func: 钩子函数。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_MNTN_IntLvlChgHook(pFUNCPTR p_Func);
#define DRV_VICINT_LVLCHG_HOOK_ADD(p_Func)    BSP_MNTN_IntLvlChgHook(p_Func)

/*****************************************************************************
 函 数 名  : BSP_MNTN_IntLvlChgHookDel
 功能描述  : 一级中断level change钩子函数注销。可维可测相关
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_MNTN_IntLvlChgHookDel(void);
#define DRV_VICINT_LVLCHG_HOOK_DEL()    BSP_MNTN_IntLvlChgHookDel()

/*****************************************************************************
 函 数 名  : BSP_MNTN_TFUPIfNeedNvBackup
 功能描述  : 使用TF卡升级前通过该接口判断是否需要备份NV项.
 输入参数  : None
 输出参数  : None
 返 回 值  : 1:需要备份
             0:无需备份
*****************************************************************************/
extern int BSP_MNTN_TFUPIfNeedNvBackup(void);
#define DRV_TF_NVBACKUP_FLAG()    BSP_MNTN_TFUPIfNeedNvBackup()

/*****************************************************************************
 函 数 名  : BSP_MNTN_GetSDDevName
 功能描述  : 获取设备列表中的SD设备名
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : SD_DEV_TYPE_STR结构指针。
*****************************************************************************/
extern SD_DEV_TYPE_STR * BSP_MNTN_GetSDDevName(void);
#define DRV_GET_SD_PATH()     BSP_MNTN_GetSDDevName()

/*****************************************************************************
 函 数 名  : BSP_PROD_GetRecoverNvInfo
 功能描述  : 从底软获得当前需要覆盖的NV信息
 输入参数  : N/A

 输出参数  : ppNvInfo  - 指向NV信息结构体的数组
             pulNvNum  - 数组个数

 返 回 值  : 0：正确，非0: 失败
*****************************************************************************/
extern  int BSP_PROD_GetRecoverNvInfo(void **ppNvInfo, unsigned long *pulNvNum);
#define  DRV_GET_RECOVER_NV_INFO(ppNvInfo, pulNvNum)  \
                BSP_PROD_GetRecoverNvInfo(ppNvInfo, pulNvNum)


/*****************************************************************************
 函 数 名  : BSP_MNTN_GETSDSTATUS
 功能描述  : 返回SD卡在位状态
 输入参数  : 无。
 输出参数  : 无。
 返回值：   1在位
            0不在位

*****************************************************************************/
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
extern int BSP_MNTN_GETSDSTATUS(void);
#define DRV_SDMMC_GET_STATUS()    BSP_MNTN_GETSDSTATUS()
#else
extern int BSP_SDMMC_GetSDStatus(void);
#define DRV_SDMMC_GET_STATUS()    BSP_SDMMC_GetSDStatus()
#endif

/*****************************************************************************
 函 数 名  : SDIO_read_write_blkdata
 功能描述  : SD卡读写接口
 输入参数  :    nCardNo     SD卡号
                dwBlkNo     块数
                nBlkCount   读写大小
                pbDataBuff  读写缓冲区
                nFlags      读/写
 输出参数  : 无。
 返回值：   0:  读写成功
            非0:错误码
*****************************************************************************/
extern int BSP_MNTN_SDRWBlkData(int nCardNo,unsigned int dwBlkNo,
                   unsigned int nBlkCount, unsigned char *pbDataBuff,int nFlags);
#define DRV_SDMMC_BLK_DATA_RW(nCardNo,dwBlkNo,nBlkCount,pbDataBuff,nFlags)   \
             BSP_MNTN_SDRWBlkData(nCardNo,dwBlkNo,nBlkCount,pbDataBuff, nFlags)


/***********************************内存拷贝优化*****************************************/
/*****************************************************************************
 函 数 名  : __rt_memcpy
 功能描述  : 汇编版本的memcpy函数
 输入参数  : Dest :目的地址
             Src :源地址
             Count:拷贝数据的大小
 输出参数  : 无。
 返 回 值  : 目的地址。
*****************************************************************************/
#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
extern void * __rt_memcpy(void * Dest, const void * Src, unsigned long Count);
#define DRV_RT_MEMCPY(Dest,Src,Count)       __rt_memcpy(Dest,Src,Count)
#else
#define DRV_RT_MEMCPY(Dest,Src,Count)       memcpy(Dest,Src,Count)
#endif



/************************************其余宏定义为空的函数******************************************/
/*****************************************************************************
 函 数 名  : UsbStatusCallbackRegister
 功能描述  : 记录pCallBack至全局变量中。SD卡功耗相关接口，V7先打桩
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
 /*extern int  UsbStatusCallbackRegister(pUsbState pCallBack);*/
#define DRV_USB_STATUS_CALLBACK_REGI(pCallBack)              DRV_OK

/*****************************************************************************
 函 数 名  : CicomClkEnable
 功能描述  : This routine enable CICOM IP clock gating.低功耗相关，V7打桩
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
/*extern void CicomClkEnable(void);*/
#define DRV_CICOM_CLK_ENABLE()

/*****************************************************************************
 函 数 名  : CicomClkDisable
 功能描述  : This routine disable CICOM IP clock gating.低功耗相关，V7打桩
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
/*extern void CicomClkDisable(void);*/
#define DRV_CICOM_CLK_DISABLE()

/*****************************************************************************
 函 数 名  : HdlcClkEnable
 功能描述  : This routine enable HDLC IP clock gating.低功耗相关，V7打桩
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
/*extern void HdlcClkEnable(void);*/
#define DRV_HDLC_CLK_ENABLE()

/*****************************************************************************
 函 数 名  : HdlcClkDisable
 功能描述  : This routine disable HDLC IP clock gating.低功耗相关，V7打桩
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
/*extern void HdlcClkDisable(void);*/
#define DRV_HDLC_CLK_DISABLE()

/*****************************************************************************
 函 数 名  : DRV_MEM_READ
 功能描述  : 按32位宽度查询内存，输出4字节数据。可维可测，V7先打桩
 输入参数  : ulAddress：查询地址空间地址，地址需在内存范围内，否则返回-1。
 输出参数  : pulData：指向查询内容的指针。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int DRV_MEM_READ(unsigned int ulAddress, unsigned int* pulData);

/*****************************************************************************
 函 数 名  : DrvLogInstall
 功能描述  : 打印函数注册。可维可测，V7先打桩
 输入参数  : fptr 注册的函数指针
 输出参数  : None
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
/*extern int DrvLogInstall(PVOIDFUNC fptr, PVOIDFUNC fptr1, PVOIDFUNC fptr2, PVOIDFUNC fptr3, PVOIDFUNC fptr4);*/
#define DRV_LOG_INSTALL(fptr, fptr1, fptr2, fptr3, fptr4)     DRV_OK

/*****************************************************************************
* 函 数 名  : BSP_MNTN_GetBattState
*
* 功能描述  : 获取电池在位状态
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 成功：0，1，2
              失败：负数
*
*
*****************************************************************************/
extern BSP_S32 BSP_MNTN_GetBattState(void);
#define DRV_MNTN_GET_BATT_STATE() BSP_MNTN_GetBattState()

/*****************************************************************************
 函 数 名  : pwrctrl_wpa_pwr_up
 功能描述  : RF下电
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
extern void pwrctrl_wpa_pwr_up(void);
#define DRV_WPA_POWERUP()

/*****************************************************************
Function: free_mem_size_get
Description:
    get mem size
Input:
    N/A
Output:
    N/A
Return:
    free mem size
*******************************************************************/
extern unsigned long free_mem_size_get(void);
#define FREE_MEM_SIZE_GET() free_mem_size_get()

/*****************************************************************************
 函 数 名  : BSP_DMR_ATAnalyze
 功能描述  : 供NAS查询动态内存占用信息，通过AT命令查询
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 成功0，失败-1
*****************************************************************************/
extern int BSP_DMR_ATANALYZE(void * pstMem,unsigned int uiSize, unsigned int * puiModIdSum);

/*************************MNTN END************************************/

/*************************DPM START***********************************/

#define PWRCTRL_GET_SUCCESS         (0x00)
#define PWRCTRL_GET_PARAINVALID     (0x01)
#define PWRCTRL_ONORLOCK            (0x10)
#define PWRCTRL_OFFORUNLOCK         (0x20)

typedef unsigned long (*pULULFUNCPTR)( unsigned long ulPara );

typedef struct
{
    pULULFUNCPTR pRestoreFunction;
    pULULFUNCPTR pEnableFunction;
    pULULFUNCPTR pDisableFunction;
    pFUNCPTR2    pDfsSetFunc;
    pULULFUNCPTR pDfsReleaseFunc;
}DPM_SLEEP_CALLBACK_STRU;

/*wangwei dfs*/
typedef  enum
{
    PWRCTRL_DFS_USB = 0,
    PWRCTRL_DFS_SD  = 1,
    PWRCTRL_DFS_PS1 = 2,
    PWRCTRL_DFS_HIFI = 3,  /*hifi*/
    PWRCTRL_DFS_BUTT
} PWC_DFS_ID_E;

typedef enum
{
    DFS_PROFILE_0 = 0,            /*频率级别1*/
    DFS_PROFILE_1 = 1,            /*频率级别2*/
    DFS_PROFILE_2 = 2,            /*频率级别3*/
    DFS_PROFILE_3 = 3,            /*频率级别4*/
    DFS_PROFILE_4 = 4,            /*频率级别5*/
    DFS_PROFILE_5 = 5,
    DFS_PROFILE_6 = 6,
    DFS_PROFILE_7 = 7,
    DFS_PROFILE_8 = 8,
    DFS_PROFILE_BUTT
} PWC_DFS_PROFILE_E;


typedef enum tagPWC_COMM_MODE_E
{
    PWC_COMM_MODE_WCDMA,
    PWC_COMM_MODE_GSM,
    PWC_COMM_MODE_LTE,
    PWC_COMM_MODE_TDS,
    PWC_COMM_MODE_BUTT
}PWC_COMM_MODE_E;

typedef enum tagPWC_COMM_MODULE_E
{
    PWC_COMM_MODULE_PA,
    PWC_COMM_MODULE_RF,
    PWC_COMM_MODULE_BBP_SOC,
    PWC_COMM_MODULE_BBP_PWR,
    PWC_COMM_MODULE_DSP,
    PWC_COMM_MODULE_ABB,
    PWC_COMM_MODULE_HIFI,
    PWC_COMM_MODULE_BUTT
}PWC_COMM_MODULE_E;

/* 睡眠投票ID */
typedef enum tagPWC_CLIENT_ID_E
{
    PWRCTRL_SLEEP_SCI       = 0x100,
    PWRCTRL_SLEEP_PS        = 0x101,
    PWRCTRL_SLEEP_BBPMASTER = 0x102,
    PWRCTRL_SLEEP_CICOM     = 0x103,
    PWRCTRL_SLEEP_NV        = 0x104,
	PWRCTRL_SLEEP_RNIC      = 0x105,
	PWRCTRL_SLEEP_APP		= 0x106,
	PWRCTRL_SLEEP_TDSPS     = 0x107,
    PWRCTRL_SLEEP_DMA       = 0x108,
    PWRCTRL_SLEEP_FLASH     = 0x109,
    PWRCTRL_SLEEP_SD        = 0x10A,
    PWRCTRL_SLEEP_FTM       = 0x10B,
    PWRCTRL_SLEEP_OAM       = 0x10C,
    PWRCTRL_SLEEP_SOCP      = 0x10D,
    PWRCTRL_SLEEP_SPECIALTIMER        = 0x10E,	
    PWRCTRL_SLEEP_GUDSP_EDMA = 0x10E, 

    PWRCTRL_SLEEP_BOOT      = 0x110,
    PWRCTRL_SLEEP_ACPU      = 0x111,
    PWRCTRL_SLEEP_HSIC      = 0x112,

    PWRCTRL_LIGHTSLEEP_WIFI = 0x113, /* HSIC不能掉电 */
    PWRCTRL_SLEEP_WIFI      = 0x114,
    PWRCTRL_SLEEP_USB       = 0x115,
    PWRCTRL_SLEEP_LCD       = 0x116,
    PWRCTRL_SLEEP_KEY     	= 0x117,
    PWRCTRL_SLEEP_ACM     	= 0x118,
    PWRCTRL_SLEEP_ICC     	= 0x119,
    PWRCTRL_SLEEP_NAS     	= 0x11a,
    PWRCTRL_SLEEP_LTEPS     = 0x11b,
    PWRCTRL_SLEEP_IFC		= 0x11c,
	PWRCTRL_SLEEP_CHG		= 0x11d,
    /* Test */
    PWRCTRL_TEST_LIGHTSLEEP = 0x11E,
    PWRCTRL_TEST_DEEPSLEEP  = 0x11F,
    PWRCTRL_CLIENT_BUTT     = 0x120
} PWC_CLIENT_ID_E;

typedef struct
{
    PWRCTRLFUNCPTRVOID  pFuncDsIn;
    PWRCTRLFUNCPTRVOID  pFuncDsOut;
}PWC_DS_SOCP_CB_STRU;

typedef enum tagPWC_TEMP_PROTECT_E
{
    PWC_TEMP_OLED,
    PWC_TEMP_BATTERY,
	PWC_TEMP_L_PA,
    PWC_TEMP_G_PA,
    PWC_TEMP_W_PA,
    PWC_TEMP_SIM_CARD,
    PWC_TEMP_DCXO,
    /* BEGIN PN:N/A,Modified by c00191475, 2013/01/08*/
    PWC_TEMP_SURFACE,/*Surface枚举量定义*/
    /* END   PN:N/A,Modified by c00191475, 2013/01/08*/
    PWC_TEMP_BUTT
}PWC_TEMP_PROTECT_E;

typedef struct
{
    PWRCTRLFUNCPTR drx_slow;
} PWC_SLEEP_CALLBACK_STRU;


typedef enum tagPWRCTRL_MODULE_E
{
    PWRCTRL_MODU_BOOTROM,       /*0x0*/
    PWRCTRL_MODU_EFUSE,
    PWRCTRL_MODU_SSDMA,
    PWRCTRL_MODU_LBBP,
    PWRCTRL_MODU_LDSP,    	
    PWRCTRL_MODU_HSUART,
    PWRCTRL_MODU_EMI,  
    PWRCTRL_MODU_SDCC,
    PWRCTRL_MODU_USBHSIC,
    PWRCTRL_MODU_SOCP,
    PWRCTRL_MODU_GMAC,          /* 0xa */
    PWRCTRL_MODU_SCI0,
    PWRCTRL_MODU_SCI1,
    PWRCTRL_MODU_UICC,
    PWRCTRL_MODU_USBOTG_BC,
    PWRCTRL_MODU_MMC0,              /*0x10*/
    PWRCTRL_MODU_MMC1,
    PWRCTRL_MODU_MMC2,
    PWRCTRL_MODU_USBOTG,
    PWRCTRL_MODU_WDT,
    PWRCTRL_MODU_TCSSI,
    PWRCTRL_MODU_IPF,
    PWRCTRL_MODU_CIPHER,
    PWRCTRL_MODU_BBPCOMM,
    PWRCTRL_MODU_TENSI,             /*0x1a*/
    PWRCTRL_MODU_CORESIGHT,
    PWRCTRL_MODU_ACP,
    PWRCTRL_MODU_I2C,
    PWRCTRL_MODU_SPI1,
    PWRCTRL_MODU_DMAC,
    PWRCTRL_MODU_LSIO,                  /*0x20*/
    PWRCTRL_MODU_HIFI,
    PWRCTRL_MODU_TSENSOR,
    PWRCTRL_MODU_CICOM,
    PWRCTRL_MODU_EDMA,
    PWRCTRL_MODU_HDLC,
    PWRCTRL_MODU_DM,
    PWRCTRL_MODU_DWSSI0,
    PWRCTRL_MODU_WGBBP,
    PWRCTRL_MODU_ZSP,
    PWRCTRL_MODU_NANDC,
    PWRCTRL_MODU_SSI0,
    PWRCTRL_MODU_SSI1,
    PWRCTRL_MODU_SSI2,
    PWRCTRL_MODU_SSI3,
    PWRCTRL_MODU_SPI0,
    PWRCTRL_MODU_MST,
    PWRCTRL_MODU_ALL,   /* 代表所有的模块 */
    PWRCTRL_MODU_MAX    /* 边界值 */
}PWRCTRL_MODULE_E;
/************************************************************************
 * FUNCTION
 *      PWRCTRL_BSP_GuDsp_StatusInfo
 * DESCRIPTION
 *        用于查询GUDSP_ZSPcore复位、解复位时间和次数
 * INPUTS
 *        GuZspAddr :GU_ZSP保存复位、解复位信息的指针地址
 * OUTPUTS
           BSP_ERROR:指针非法。
 *************************************************************************/
 BSP_U32 PWRCTRL_BSP_GuDsp_StatusInfo(BSP_U32 *GuZspAddr);

/************************************************************************
 * FUNCTION
 *      PWRCTRL_BSP_RTT_StatusInfo
 * DESCRIPTION
 *        BBP 状态查询
 * INPUTS
 *       NONE
 * OUTPUTS
          BSP_BBP_ERROR: 不能访问
          BSP_BBP_OK:可以访问
 *************************************************************************/
BSP_U32  PWRCTRL_BSP_RTT_StatusInfo (BSP_VOID);


/*****************************************************************************
* 函 数 名  : BSP_PWRCTRL_TimerClose
*
* 功能描述  : SOC外设时钟关闭接口
*
* 输入参数  : 
* 输出参数  : 
* 返 回 值   :
*
* 修改记录  : 

*****************************************************************************/
BSP_S32 BSP_PWRCTRL_TimerClose(PWRCTRL_MODULE_E enModu);

/*****************************************************************************
* 函 数 名  : BSP_PWRCTRL_TimerOpen
*
* 功能描述  : SOC外设时钟使能接口
*
* 输入参数  : 
* 输出参数  : 
* 返 回 值   :
*
* 修改记录  : 

*****************************************************************************/
BSP_S32 BSP_PWRCTRL_TimerOpen(PWRCTRL_MODULE_E enModu);

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_DEEPSLEEP_FOREVER
 功能描述  : AARM CARM 下电接口
 输入参数  : None
 输出参数  : None
 返 回 值  : None

*****************************************************************************/
extern void DRV_PWRCTRL_DEEPSLEEP_FOREVER(void );

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_SLEEP_CALLBACK_REG
 功能描述  : 注册PWC函数给底软使用
 输入参数  : 待注册的函数
 输出参数  : None
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int DRV_PWRCTRL_SLEEP_CALLBACK_REG(PWC_SLEEP_CALLBACK_STRU pCallback);

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_GET_TEMP
 功能描述  : 获取OLED、电池、GPA、WPA、SIM卡、DCXO的温度。
 输入参数  : enModule:PWC_TEMP_PROTECT_E
             hkAdcTable:电压温度换算表
 输出参数  : pslData:存储的电压转换为的温度值
 返 回 值  : HKADC_OK/HKADC_ERROR 。
*****************************************************************************/
/*V3R2 CS f00164371，这个接口暂时不隔离，因为SFT平台是返回0，实际回片需要实现，
需要使用CS_SFT宏进行区分，但是不同组件的CS_SFT宏不一定统一，所以不能直接屏蔽掉，
采用在接口代码内部进行隔离*/
extern BSP_S32 DRV_PWRCTRL_GET_TEMP(PWC_TEMP_PROTECT_E enModule, BSP_U16 *hkAdcTable,BSP_S32 *pslData);

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_AHB_BAK_ADDR_ALLOC
 功能描述  : arm上下电功能，需要备份ahb数据，该接口申请数据保存地址和长度
 输入参数  : length     保存申请空间大小的指针
 输出参数  : 无
 返 回 值  : 申请空间的地址 。
*****************************************************************************/
/*V3R2 CS f00164371，这个接口暂时不隔离*/
extern void * DRV_PWRCTRL_AHB_BAK_ADDR_ALLOC(unsigned int * length);

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_GetCcpuLoadCB
 功能描述  : arm 提供给TTF的回调函数
 输入参数  : pFunc:TTF函数指针
 输出参数  : 无
 返 回 值  : 申请空间的地址 。
*****************************************************************************/
extern void DRV_PWRCTRL_GetCcpuLoadCB(PWRCTRLFUNCPTR pFunc );

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_GetCpuLoad
 功能描述  : arm cpu占有率查询函数
 输入参数  : 无
 输出参数  : ulAcpuLoad:Acpu占有率地址.
             ulCcpuLoad:Ccpu占有率地址.
 返 回 值  : 0/1 。
*****************************************************************************/
extern unsigned int DRV_PWRCTRL_GetCpuLoad(unsigned int *ulAcpuLoad,unsigned int *ulCcpuLoad);

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_PWRUP
 功能描述  : W模、G模、L模下PA、RF、BBP、DSP、ABB、HIFI的上电控制。
 输入参数  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_RF/PWC_COMM_MODULE_BBP/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
/*V3R2 CS f00164371，这个接口暂时不隔离，因为HIFI加载需要，在函数内部进行了隔离*/
extern BSP_U32 DRV_PWRCTRL_PWRUP (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule);

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_PWRDOWN
 功能描述  : W模、G模、L模下PA、RF、BBP、DSP、ABB、HIFI的下电控制。
 输入参数  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_RF/PWC_COMM_MODULE_BBP/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
/*V3R2 CS f00164371，这个接口暂时不隔离，因为HIFI加载需要，在函数内部进行了隔离*/
extern BSP_U32 DRV_PWRCTRL_PWRDOWN (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule);

/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_PwrStatusGet
 功能描述  : W模、G模、L模下PA、RF、BBP、DSP、ABB的上下电状态查询。
 输入参数  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_RF/PWC_COMM_MODULE_BBP/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 输出参数  : None
 返 回 值  : PWRCTRL_GET_SUCCESS/PWRCTRL_GET_PARAINVALID/PWRCTRL_ONORLOCK/PWRCTRL_OFFORUNLOCK
*****************************************************************************/
extern  BSP_U32 DRV_PWRCTRL_PWRSTATUSGET(PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule);

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_PLL_ENABLE
 功能描述  : W模、G模、L模下PA、DSP、ABB的PLL使能。
 输入参数  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
extern BSP_U32 DRV_PWRCTRL_PLL_ENABLE (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule);

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_PLL_DISABLE
 功能描述  : W模、G模、L模下PA、DSP、ABB的PLL去使能。
 输入参数  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
extern BSP_U32 DRV_PWRCTRL_PLL_DISABLE (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule);

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_DSP_RESET
 功能描述  : L模下DSP复位接口。
 输入参数  : 
 输出参数  : None
 返 回 值  : 
*****************************************************************************/
extern BSP_VOID DRV_PWRCTRL_DSP_RESET();

/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_PllStatusGet
 功能描述  : W模、G模、L模下PA、DSP、ABB的PLL状态查询。
 输入参数  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 输出参数  : None
 返 回 值  : PWRCTRL_GET_SUCCESS/PWRCTRL_GET_PARAINVALID/PWRCTRL_ONORLOCK/PWRCTRL_OFFORUNLOCK
*****************************************************************************/
extern BSP_U32 DRV_PWRCTRL_PllStatusGet (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule);

/************************************************************************
 * FUNCTION
 *       BSP_PWRCTRL_ZspSleepFlagStore
 * DESCRIPTION
 *       ZSP睡眠标志位保存接口
 * INPUTS
 *       
 * OUTPUTS
 *       NONE
 *************************************************************************/
extern BSP_VOID DRV_ZSP_SLEEP_FALG_STORE(BSP_VOID);

/************************************************************************
 * FUNCTION
 *       BSP_PWRCTRL_ZspSleepFlagRead
 * DESCRIPTION
 *       ZSP睡眠标志位读取接口
 * INPUTS
 *       
 * OUTPUTS
 *       NONE
 *************************************************************************/
extern BSP_U32 DRV_ZSP_SLEEP_FALG_READ(BSP_VOID);


/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_SLEEPVOTE_LOCK
 功能描述  : 外设禁止睡眠投票接口。
 输入参数  : enClientId:PWC_CLIENT_ID_E
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
extern BSP_U32 DRV_PWRCTRL_SLEEPVOTE_LOCK(PWC_CLIENT_ID_E enClientId);

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_SLEEPVOTE_UNLOCK
 功能描述  : 外设允许睡眠投票接口。
 输入参数  : enClientId:PWC_CLIENT_ID_E
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
extern BSP_U32 DRV_PWRCTRL_SLEEPVOTE_UNLOCK(PWC_CLIENT_ID_E enClientId);

/*****************************************************************************
Function:   DRV_PWRCTRL_DFS_SET_PRF_CCPU
Description:Set the System Min Profile
Input:      ulClientID: The Vote Module Client
            enProfile:  The Min Profile Value
Output:     None
Return:     The Vote Result
Others:
*****************************************************************************/
extern BSP_U32  DRV_PWRCTRL_DFS_SET_PRF_CCPU(PWC_DFS_ID_E ulClientID, PWC_DFS_PROFILE_E enProfile);

/*****************************************************************************
Function:   DRV_PWRCTRL_DFS_RLS_PRF_CCPU
Description:Release the Vote Result
Input:      ulClientID: The Vote Module Client
Output:     None;
Return:     The Vote Result
Others:
*****************************************************************************/
extern BSP_U32 DRV_PWRCTRL_DFS_RLS_PRF_CCPU(PWC_DFS_ID_E ulClientID);

/*****************************************************************************
Function:   DRV_PWRCTRL_SLEEP_IN_CB
Description:
Input:
Output:     None;
Return:
Others:
*****************************************************************************/
extern int DRV_PWRCTRL_SLEEP_IN_CB(PWC_DS_SOCP_CB_STRU stFunc);

/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_UsbLowPowerEnter
 功能描述  : USB进入低功耗接口
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
extern void DRV_PWRCTRL_USB_LOWPOWER_ENTER(void);

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_USB_LOWPOWER_EXIT
 功能描述  : USB退出低功耗接口
 输入参数  : None
 输出参数  : None
 返 回 值  : None
*****************************************************************************/
extern void DRV_PWRCTRL_USB_LOWPOWER_EXIT(void);


/*****************************************************************************
 函 数 名  : BSP_32K_GetTick
 功能描述  : 32K时钟对应Tick查询接口
 输入参数  : None
 输出参数  : None
 返 回 值  : 32K时钟对应Tick值
*****************************************************************************/
extern BSP_U32 BSP_32K_GetTick( BSP_VOID );


/*****************************************************************************
Function:   BSP_PWC_GetMeansFlag
Description:
Input:
Output:     the means flag value;
Return:
Others:
*****************************************************************************/
extern BSP_S32 BSP_PWC_GetMeansFlag(PWC_COMM_MODE_E enCommMode);
#define DRV_PWC_GET_MEANSFLAG(enCommMode) BSP_PWC_GetMeansFlag(enCommMode)

extern BSP_VOID BSP_PWC_MspVoteRegister(FUNCPTR routine);
extern BSP_VOID BSP_PWC_SocpVoteRegister(FUNCPTR routine);
extern BSP_VOID BSP_PWC_SocpRestoreRegister(FUNCPTR routine);

/*****************************************************************************
 Function   : BSP_PWC_SetTimer4WakeSrc
 Description: 设置timer4作为唤醒源
 Input      : 
 Return     : void
 Other      : 
*****************************************************************************/
extern VOID BSP_PWC_SetTimer4WakeSrc(VOID);

/*****************************************************************************
 Function   : BSP_PWC_DelTimer4WakeSrc
 Description: 设置timer4不作为唤醒源 
 Input      :  
            : 
 Return     : void
 Other      : 
*****************************************************************************/
extern VOID BSP_PWC_DelTimer4WakeSrc(VOID);


/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_StandbyStateCcpu/BSP_PWRCTRL_StandbyStateAcpu
 功能描述  : AT^PSTANDBY
 输入参数  :
 输出参数  :
 返回值：
*****************************************************************************/
extern unsigned int BSP_PWRCTRL_StandbyStateAcpu(unsigned int ulStandbyTime, unsigned int ulSwitchTime);
extern unsigned int BSP_PWRCTRL_StandbyStateCcpu(unsigned int ulStandbyTime, unsigned int ulSwitchTime);
#define DRV_PWRCTRL_STANDBYSTATEACPU(ulStandbyTime, ulSwitchTime)	BSP_PWRCTRL_StandbyStateAcpu(ulStandbyTime, ulSwitchTime)
#define DRV_PWRCTRL_STANDBYSTATECCPU(ulStandbyTime, ulSwitchTime)	BSP_PWRCTRL_StandbyStateCcpu(ulStandbyTime, ulSwitchTime)



/*************************DPM END*************************************/

/*************************ABB START***********************************/

/*****************************************************************************
 函 数 名  : BSP_ABB_RegGet
 功能描述  : 获取ABB寄存器值
 输入参数  : usInstruction: 待读寄存器地址
             uspData: 存放寄存器值
 输出参数  :
 返 回 值  :
*****************************************************************************/
extern int BSP_ABB_RegGet(unsigned char usInstruction, unsigned char *uspData);
#define DRV_ABB_REG_VALUE_GET(usInstruction,uspData)    BSP_ABB_RegGet (usInstruction,uspData)

/*************************ABB END*************************************/

/*************************SYSCTRL START*******************************/
/*****************************************************************************
 函 数 名  : BSP_GUSYS_GuDspCountInfo
 功能描述  : 本接口实现ZSP模块的解复位动作，包括zsp BRG、总线
 输入参数  : *GuDspAddr 用于保存计数信息
 输出参数  : 无。
 返 回 值  : BSP_ERROR:地址非法
                       BSP_OK:地址合法
         徐经翠x00221564  2012.11.20
*****************************************************************************/
extern unsigned int BSP_GUSYS_GuDspCountInfo(unsigned int *GuDspAddr);
#define DRV_DSP_RESET_GET_INFO(GuDspAddr)    BSP_GUSYS_GuDspCountInfo (GuDspAddr)

/*****************************************************************************
 函 数 名  : BSP_GUSYS_DspReset
 功能描述  : 本接口实现ZSP模块的复位动作，包括zsp core、总线和外设。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_GUSYS_DspReset(void);
#define DRV_DSP_ZONE_RESET()    BSP_GUSYS_DspReset()

/*****************************************************************************
 函 数 名  : BSP_GUSYS_DspResetCancel
 功能描述  : 本接口实现ZSP模块的解复位动作，包括zsp core、总线和外设。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_GUSYS_DspResetCancel(void);
#define DRV_DSP_ZONE_RESET_CANCEL()    BSP_GUSYS_DspResetCancel()

/*****************************************************************************
 函 数 名  : BSP_GUSYS_ABBGSMRxCtrl
 功能描述  : 本接口实现ABB GSM接收控制。
 输入参数  : ucStatus：0：关闭；1：打开。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern unsigned int BSP_GUSYS_ABBGSMRxCtrl(unsigned char ucStatus);
#define DRV_ABB_GRX_CTRL(ucStatus)    BSP_GUSYS_ABBGSMRxCtrl(ucStatus)

/*****************************************************************************
 函 数 名  : ABBWCDMARxCtrl
 功能描述  : 本接口实现ABB WCDMA接收控制。
 输入参数  : ucStatus：0：关闭；1：打开。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern unsigned int BSP_GUSYS_ABBWCDMARxCtrl(unsigned char ucStatus);
#define DRV_ABB_WRX_CTRL(ucStatus)    BSP_GUSYS_ABBWCDMARxCtrl(ucStatus)

/*****************************************************************************
 函 数 名  : ABBTxCtrl
 功能描述  : 本接口实现ABB 发送控制。
 输入参数  : ucStatus：0：关闭；1：打开。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
 注意事项  ：ABB发送控制不区分W/G模式。
*****************************************************************************/
extern int BSP_GUSYS_ABBTxCtrl(unsigned char ucStatus);
#define DRV_ABB_TX_CTRL(ucStatus)    BSP_GUSYS_ABBTxCtrl(ucStatus)

/*****************************************************************************
 函 数 名  : BSP_GUSYS_WcdmaBbpPllEnable
 功能描述  : 本接口实现使能WCDMA BBP PLL。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_GUSYS_WcdmaBbpPllEnable(void);
#define DRV_WBBP_PLL_ENABLE()    BSP_GUSYS_WcdmaBbpPllEnable()

/*****************************************************************************
 函 数 名  : BSP_GUSYS_WcdmaBbpPllDisable
 功能描述  : 本接口实现关闭WCDMA BBP PLL。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_GUSYS_WcdmaBbpPllDisable(void);
#define DRV_WBBP_PLL_DSABLE()    BSP_GUSYS_WcdmaBbpPllDisable()

/*****************************************************************************
 函 数 名  : BSP_GUSYS_GsmBbpPllEnable
 功能描述  : 本接口实现使能GSM BBP PLL。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_GUSYS_GsmBbpPllEnable(void);
#define DRV_GBBP_PLL_ENABLE()    BSP_GUSYS_GsmBbpPllEnable()

/*****************************************************************************
 函 数 名  : BSP_GUSYS_GsmBbpPllDisable
 功能描述  : 本接口实现关闭GSM BBP PLL。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_GUSYS_GsmBbpPllDisable(void);
#define DRV_GBBP_PLL_DISABLE()    BSP_GUSYS_GsmBbpPllDisable()

/*****************************************************************************
 函 数 名  : BSP_GUSYS_RFLdoOn
 功能描述  : 本接口实现WCDMA和GSM RF LDO上电。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_GUSYS_RFLdoOn(void);
#define DRV_RF_LDOUP()    BSP_GUSYS_RFLdoOn()

/*****************************************************************************
 函 数 名  : BSP_GUSYS_RFLdoDown
 功能描述  : 本接口实现WCDMA和GSM RF LDO下电。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_GUSYS_RFLdoDown(void);
#define DRV_RF_LDODOWN()    BSP_GUSYS_RFLdoDown()

/*****************************************************************************
 函 数 名  : BSP_GUSYS_WcdmaPllStatusGet
 功能描述  : 读取WCDMA BBP PLL稳定状态。
 输入参数  : 无。
 输出参数  :无。
 返 回 值  :
                     0：稳定
                     1：未稳定
*****************************************************************************/
extern unsigned long BSP_GUSYS_WcdmaPllStatusGet(void);
#define DRV_GET_WCDMA_PLL_STATUS()    BSP_GUSYS_WcdmaPllStatusGet()

/*****************************************************************************
 函 数 名  : BSP_GUSYS_GsmPllStatusGet
 功能描述  : 读取GSM BBP PLL稳定状态。
 输入参数  : 无。
 输出参数  :无。
 返 回 值  :
                     0：稳定
                     1：未稳定
*****************************************************************************/
extern unsigned long BSP_GUSYS_GsmPllStatusGet(void);
#define DRV_GET_GSM_PLL_STATUS()    BSP_GUSYS_GsmPllStatusGet()

/*****************************************************************************
 函 数 名  : BSP_GUSYS_DspPllStatusGet
 功能描述  : 读取DSP PLL稳定状态。
 输入参数  : 无。
 输出参数  :无。
 返 回 值  :
                     0：稳定
                     1：未稳定
*****************************************************************************/
extern unsigned long BSP_GUSYS_DspPllStatusGet(void);
#define DRV_GET_DSP_PLL_STATUS()    BSP_GUSYS_DspPllStatusGet()


/*****************************************************************************
 函 数 名  : BSP_GUSYS_ArmPllStatusGet
 功能描述  : 读取ARM PLL稳定状态。
 输入参数  : 无。
 输出参数  :无。
 返 回 值  :
                     0：稳定
                     1：未稳定
*****************************************************************************/
extern unsigned int BSP_GUSYS_ArmPllStatusGet(void);
#define DRV_GET_ARM_PLL_STATUS()    BSP_GUSYS_ArmPllStatusGet()

/*****************************************************************************
 函 数 名  : BSP_GUSYS_BBPAutoAdjust
 功能描述  : 初始化ABB的自校准使能
 输入参数  : uiSysMode：0：WCDMA模式；1：GSM模式。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_GUSYS_BBPAutoAdjust(unsigned char ucAdjustMode);
#define DRV_BBP_AUTO_ADJUST(ucAdjustMode)    BSP_GUSYS_BBPAutoAdjust(ucAdjustMode)

/*****************************************************************************
 函 数 名  : BSP_GUSYS_DspPllEnable
 功能描述  : 打开DSP PLL
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_GUSYS_DspPllEnable(void);
#define DRV_DSP_PLL_ENABLE()    BSP_GUSYS_DspPllEnable()

/*****************************************************************************
 函 数 名  : BSP_GUSYS_DspPllDisable
 功能描述  : 关闭DSP PLL
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_GUSYS_DspPllDisable(void);
#define DRV_DSP_PLL_DISABLE()    BSP_GUSYS_DspPllDisable()


#define WCDMA_MODE      0
#define GSM_MODE        1
#define LTE_MODE        2
#define TDS_MODE        3

/*****************************************************************************
 函 数 名  : BSP_GUSYS_ModeSwitchs
 功能描述  : 本接口实现WCDMA/GSM系统模式切换。
 输入参数  : uiSysMode：0：切换到WCDMA模式；1：切换到GSM模式。
 输出参数  : 无。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern int BSP_GUSYS_ModeSwitch(unsigned int uiSysMode);
#define DRV_SYSMODE_SWITCH(uiSysMode)    BSP_GUSYS_ModeSwitch(uiSysMode)

/*************************SYSCTRL START*******************************/

/*************************GUDSP加载 START*****************************/

/*****************************************************************************
 函 数 名  : BSP_GUDSP_LayerInfoGet
 功能描述  : 获取物理层加载信息。
 输入参数  : 无。
 输出参数  : pulAddr：存放物理层加载信息的缓存。
 返 回 值  : 无。
*****************************************************************************/
extern void BSP_GUDSP_LayerInfoGet(unsigned long* pulAddr, unsigned long* pulLength);
#define DRV_PHY_SLOAD_INFO_GET(pulAddr,pulLength)    BSP_GUDSP_LayerInfoGet(pulAddr,pulLength)

/*****************************************************************************
 函 数 名  : BSP_GUDSP_CommonInfoGet
 功能描述  : 获取物理层BSS COMMON段信息。
 输入参数  : 无。
 输出参数  : pulAddr：存放物理层加载信息的缓存。
 返 回 值  : 0:成功，-1:失败。
*****************************************************************************/
extern int BSP_GUDSP_CommonInfoGet(unsigned long* pulAddr, unsigned long* pulLength);
#define DRV_DSP_COMMON_INFO_GET(pulAddr,pulLength)    BSP_GUDSP_CommonInfoGet(pulAddr,pulLength)

/*****************************************************************************
 函 数 名  : BSP_GUDSP_Load
 功能描述  : 完成GU ZSP加载功能：将ZSP静态段从DDR加载到ZSP TCM。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 0:成功，-1:失败。
*****************************************************************************/
extern int BSP_GUDSP_Load(void);
#define DRV_GUDSP_LOAD()    BSP_GUDSP_Load()

/*************************GUDSP加载 END*******************************/

/*************************EDMA START**********************************/

#if defined(BSP_CORE_MODEM) || defined(PRODUCT_CFG_CORE_TYPE_MODEM)
typedef void (*channel_isr)(BSP_U32 channel_arg, BSP_U32 int_status);

/*定义外设请求号*/
typedef enum _BALONG_DMA_REQ
{
    EDMA_DWSSI0_RX = 0,
    EDMA_DWSSI0_TX,
    EDMA_DWSSI1_RX,
    EDMA_DWSSI1_TX,
    EDMA_HIFISIO_RX,
    EDMA_HIFISIO_TX,
    EDMA_HSSPI_RX,
    EDMA_HSSPI_TX,
    EDMA_HSUART_RX,
    EDMA_HSUART_TX,
    EDMA_UART0_RX,
    EDMA_UART0_TX,
    EDMA_UART1_RX,
    EDMA_UART1_TX,
    EDMA_UART2_RX,
    EDMA_UART2_TX,
    EDMA_SCI_RX,
    EDMA_SCI_TX,
    EDMA_UART3_RX,
    EDMA_UART3_TX,
    EDMA_MMC0,     /*20*/
    EDMA_MMC1,
    EDMA_MMC2,
    EDMA_MEMORY,    /*memory to memory ,虚拟外设请求线*/
    EDMA_REQ_MAX    /*如果设备请求不小于此值，则为非法请求*/
} BALONG_DMA_REQ;

/* 函数void (*channel_isr)(BSP_U32 channel_arg, BSP_U32 int_status)的参数int_status、
    函数int balong_dma_channel_init (BALONG_DMA_REQ req,  channel_isr pFunc,
                     UINT32 channel_arg, UINT32 int_flag)的参数int_flag
    为以下几种中断类型，可组合       */
#define BALONG_DMA_INT_DONE           1          /*DMA传输完成中断*/
#define BALONG_DMA_INT_LLT_DONE       2          /*链式DMA节点传输完成中断*/
#define BALONG_DMA_INT_CONFIG_ERR     4          /*DMA配置错误导致的中断*/
#define BALONG_DMA_INT_TRANSFER_ERR   8          /*DMA传输错误导致的中断*/
#define BALONG_DMA_INT_READ_ERR       16         /*DMA链表读错误导致的中断*/

/* EDMAC传输方向定义*/
#define BALONG_DMA_P2M      1
#define BALONG_DMA_M2P      2
#define BALONG_DMA_M2M      3

/* EDMAC流控制与传输类型*/
typedef enum tagEDMA_TRANS_TYPE
{
    MEM_MEM_DMA = 0x00,    /* 内存到内存，DMA流控*/
    MEM_PRF_DMA = 0x01,        /* 内存与外设，DMA流控*/
    MEM_PRF_PRF = 0x10        /* 内存与外设，外设流控*/
} EDMA_TRANS_TYPE;

/* 通道状态 */

#define   EDMA_CHN_FREE          1   /* 通道空闲 */
#define   EDMA_CHN_BUSY          0   /* 通道忙 */


/* EDMA传输位宽，源、目的地址约束为一致的值 */
#define   EDMA_TRANS_WIDTH_8       0x0   /* 8bit位宽*/
#define   EDMA_TRANS_WIDTH_16      0x1   /* 16bit位宽*/
#define   EDMA_TRANS_WIDTH_32      0x2   /* 32bit位宽*/
#define   EDMA_TRANS_WIDTH_64      0x3   /* 64bit位宽*/

/*  EDMA burst length, 取值范围0~15，表示的burst长度为1~16*/
#define   EDMA_BUR_LEN_1    0x0    /* burst长度，即一次传输的个数为1个*/
#define   EDMA_BUR_LEN_2    0x1    /* burst长度，即一次传输的个数为2个*/
#define   EDMA_BUR_LEN_3    0x2   /* burst长度，即一次传输的个数为3个*/
#define   EDMA_BUR_LEN_4    0x3   /* burst长度，即一次传输的个数为4个*/
#define   EDMA_BUR_LEN_5    0x4   /* burst长度，即一次传输的个数为5个*/
#define   EDMA_BUR_LEN_6    0x5   /* burst长度，即一次传输的个数为6个*/
#define   EDMA_BUR_LEN_7    0x6   /* burst长度，即一次传输的个数为7个*/
#define   EDMA_BUR_LEN_8    0x7   /* burst长度，即一次传输的个数为8个*/
#define   EDMA_BUR_LEN_9    0x8   /* burst长度，即一次传输的个数为9个*/
#define   EDMA_BUR_LEN_10   0x9   /* burst长度，即一次传输的个数为10个*/
#define   EDMA_BUR_LEN_11   0xa   /* burst长度，即一次传输的个数为11个*/
#define   EDMA_BUR_LEN_12   0xb   /* burst长度，即一次传输的个数为12个*/
#define   EDMA_BUR_LEN_13   0xc   /* burst长度，即一次传输的个数为13个*/
#define   EDMA_BUR_LEN_14   0xd   /* burst长度，即一次传输的个数为14个*/
#define   EDMA_BUR_LEN_15   0xe   /* burst长度，即一次传输的个数为15个*/
#define   EDMA_BUR_LEN_16   0xf   /* burst长度，即一次传输的个数为16个*/


/* EDMA 对应的具体位，供EDMA  寄存器配置宏
       EDMAC_BASIC_CONFIG、BALONG_DMA_SET_LLI、BALONG_DMA_SET_CONFIG 使用*/
#define EDMAC_TRANSFER_CONFIG_SOUR_INC      (0X80000000)
#define EDMAC_TRANSFER_CONFIG_DEST_INC      (0X40000000)

#define EDMAC_TRANSFER_CONFIG_SOUR_BURST_LENGTH( _len )  ((BSP_U32)((_len)<<24))
#define EDMAC_TRANSFER_CONFIG_DEST_BURST_LENGTH( _len )  ((BSP_U32)((_len)<<20))

#define EDMAC_TRANSFER_CONFIG_SOUR_WIDTH( _len )  ((BSP_U32)((_len)<<16))
#define EDMAC_TRANSFER_CONFIG_DEST_WIDTH( _len )  ((BSP_U32)((_len)<<12))

#define EDMAC_TRANSFER_CONFIG_REQUEST( _ulReg )    ( (_ulReg ) << 4)
#define EDMAC_TRANSFER_CONFIG_FLOW_DMAC( _len )    ((BSP_U32)((_len)<<2))

#define EDMAC_TRANSFER_CONFIG_INT_TC_ENABLE            ( 0x2 )
#define EDMAC_TRANSFER_CONFIG_INT_TC_DISABLE           ( 0x0 )

#define EDMAC_TRANSFER_CONFIG_CHANNEL_ENABLE           ( 0x1 )
#define EDMAC_TRANSFER_CONFIG_CHANNEL_DISABLE          ( 0x0 )

#define EDMAC_TRANSFER_CONFIG_EXIT_ADD_MODE_A_SYNC     ( 0UL )
#define EDMAC_TRANSFER_CONFIG_EXIT_ADD_MODE_AB_SYNC    ( 0x00008000 )

#define EDMAC_NEXT_LLI_ENABLE       0x2           /* Bit 1 */

/*链式传输时的节点信息*/
typedef struct _BALONG_DMA_CB
{
    volatile BSP_U32 lli;     /*指向下个LLI*/
    volatile BSP_U32 bindx;
    volatile BSP_U32 cindx;
    volatile BSP_U32 cnt1;
    volatile BSP_U32 cnt0;   /*块传输或者LLI传输的每个节点数据长度 <= 65535字节*/
    volatile BSP_U32 src_addr; /*物理地址*/
    volatile BSP_U32 des_addr; /*物理地址*/
    volatile BSP_U32 config;
} BALONG_DMA_CB;

#define P2M_CONFIG   (EDMAC_TRANSFER_CONFIG_FLOW_DMAC(MEM_PRF_DMA) | EDMAC_TRANSFER_CONFIG_DEST_INC)
#define M2P_CONFIG   (EDMAC_TRANSFER_CONFIG_FLOW_DMAC(MEM_PRF_DMA) | EDMAC_TRANSFER_CONFIG_SOUR_INC)
#define M2M_CONFIG   (EDMAC_TRANSFER_CONFIG_FLOW_DMAC(MEM_MEM_DMA) | EDMAC_TRANSFER_CONFIG_SOUR_INC | EDMAC_TRANSFER_CONFIG_DEST_INC)

#define EDMAC_BASIC_CONFIG(burst_width, burst_len) \
               ( EDMAC_TRANSFER_CONFIG_SOUR_BURST_LENGTH(burst_len) | EDMAC_TRANSFER_CONFIG_DEST_BURST_LENGTH(burst_len) \
               | EDMAC_TRANSFER_CONFIG_SOUR_WIDTH(burst_width) | EDMAC_TRANSFER_CONFIG_DEST_WIDTH(burst_width) )

/*addr:物理地址*/
#define BALONG_DMA_SET_LLI(addr, last)   ((last)?0:(EDMAC_MAKE_LLI_ADDR(addr) | EDMAC_NEXT_LLI_ENABLE))

#define BALONG_DMA_SET_CONFIG(req, direction, burst_width, burst_len) \
                 ( EDMAC_BASIC_CONFIG(burst_width, burst_len) | EDMAC_TRANSFER_CONFIG_REQUEST(req) \
                 | EDMAC_TRANSFER_CONFIG_INT_TC_ENABLE | EDMAC_TRANSFER_CONFIG_CHANNEL_ENABLE \
                 | ((direction == BALONG_DMA_M2M)?M2M_CONFIG:((direction == BALONG_DMA_P2M)?P2M_CONFIG:M2P_CONFIG)))

/**************************************************************************
  宏定义
**************************************************************************/

/*错误码定义*/
#define DMAC_SUCCESS                       BSP_OK
#define DMA_FAIL                          BSP_ERROR

#define DMA_ERROR_BASE                          -100
#define DMA_CHANNEL_INVALID                     (DMA_ERROR_BASE-1)
#define DMA_TRXFERSIZE_INVALID                  (DMA_ERROR_BASE-2)
#define DMA_SOURCE_ADDRESS_INVALID              (DMA_ERROR_BASE-3)
#define DMA_DESTINATION_ADDRESS_INVALID         (DMA_ERROR_BASE-4)
#define DMA_MEMORY_ADDRESS_INVALID              (DMA_ERROR_BASE-5)
#define DMA_PERIPHERAL_ID_INVALID               (DMA_ERROR_BASE-6)
#define DMA_DIRECTION_ERROR                     (DMA_ERROR_BASE-7)
#define DMA_TRXFER_ERROR                        (DMA_ERROR_BASE-8)
#define DMA_LLIHEAD_ERROR                       (DMA_ERROR_BASE-9)
#define DMA_SWIDTH_ERROR                        (DMA_ERROR_BASE-0xa)
#define DMA_LLI_ADDRESS_INVALID                 (DMA_ERROR_BASE-0xb)
#define DMA_TRANS_CONTROL_INVALID               (DMA_ERROR_BASE-0xc)
#define DMA_MEMORY_ALLOCATE_ERROR               (DMA_ERROR_BASE-0xd)
#define DMA_NOT_FINISHED                        (DMA_ERROR_BASE-0xe)
#define DMA_CONFIG_ERROR                        (DMA_ERROR_BASE-0xf)


/*******************************************************************************
  函数名:      BSP_S32 balong_dma_init(void)
  功能描述:    DMA初始化程序，挂接中断
  输入参数:    无
  输出参数:    无
  返回值:      0
*******************************************************************************/
extern BSP_S32 balong_dma_init(void);

/*******************************************************************************
  函数名:       int balong_dma_current_transfer_address(UINT32 channel_id)
  函数描述:     获得某通道当前传输的内存地址
  输入参数:     channel_id : 通道ID，调用balong_dma_channel_init函数的返回值
  输出参数:     无
  返回值:       成功：通道当前传输的内存地址
                失败：负数
*******************************************************************************/
extern int balong_dma_current_transfer_address(BSP_U32 channel_id);
#define DRV_EDMA_CURR_TRANS_ADDR(channel_id)  balong_dma_current_transfer_address(channel_id)

/*******************************************************************************
  函数名:       int balong_dma_channel_stop(UINT32 channel_id)
  函数描述:     停止指定的DMA通道
  输入参数:     channel_id : 通道ID，调用balong_dma_channel_init函数的返回值
  输出参数:     无
  返回值:       成功：通道当前传输的内存地址
                失败：负数
*******************************************************************************/
extern BSP_S32 balong_dma_channel_stop(BSP_U32 channel_id);
#define DRV_EDMA_CHANNEL_STOP(channel_id)  balong_dma_channel_stop(channel_id)


/*******************************************************************************
  函数名:      BALONG_DMA_CB *balong_dma_channel_get_lli_addr(UINT32 channel_id)
  函数描述:    获取指定DMA通道的链表控制块的起始地址
  输入参数:    channel_id：通道ID,调用balong_dma_channel_init函数的返回值
  输出参数:    无
  返回值:      成功：0
               失败：负数
*******************************************************************************/
extern BALONG_DMA_CB *balong_dma_channel_get_lli_addr (BSP_U32 channel_id);
#define DRV_EDMA_CHAN_GET_LLI_ADDR(channel_id)  balong_dma_channel_get_lli_addr(channel_id)


/******************************************************************************
  函数名:      int balong_dma_channel_init (BALONG_DMA_REQ req,
                  channel_isr pFunc, UINT32 channel_arg, UINT32 int_flag)
  函数描述:    根据外设号分配通道，注册通道中断回调函数、初始化传输完成信号量、
               将外设号写入config寄存器
  输入参数:    req : 外设请求号
               pFunc : 上层模块注册的DMA通道中断处理函数，NULL时表明不注册
               channel_arg : pFunc的入参1，
                             pFunc为NULL，不需要设置这个参数
               int_flag : pFunc的入参2, 产生的中断类型，取值范围为
                        BALONG_DMA_INT_DONE、BALONG_DMA_INT_LLT_DONE、
                      ALONG_DMA_INT_CONFIG_ERR、BALONG_DMA_INT_TRANSFER_ERR、
                        BALONG_DMA_INT_READ_ERR之一，或者组合。
                        pFunc为NULL，不需要设置这个参数
  输出参数:    无
  返回值:      成功：通道号
               失败：负数
*******************************************************************************/
extern BSP_S32 balong_dma_channel_init (BALONG_DMA_REQ req, channel_isr pFunc, BSP_U32 channel_arg, BSP_U32 int_flag);
#define DRV_EDMA_CHANNEL_INIT(req, pFunc,channel_arg,int_flag) balong_dma_channel_init(req, pFunc,channel_arg,int_flag)

/*******************************************************************************
  函数名:      int balong_dma_channel_set_config (UINT32 channel_id,
                       UINT32 direction, UINT32 burst_width, UINT32 burst_len)
  函数描述:    非链式DMA传输时，调用本函数配置通道参数
               链式DMA传输时，不需要使用本函数。
  输入参数:    channel_id : 通道ID，调用balong_dma_channel_init函数的返回值
               direction : DMA传输方向, 取值为BALONG_DMA_P2M、BALONG_DMA_M2P、
                           BALONG_DMA_M2M之一
               burst_width：取值为0、1、2、3，表示的burst位宽为8、16、32、64bit
               burst_len：取值范围0~15，表示的burst长度为1~16
  输出参数:    无
  返回值:      成功：0
               失败：负数
*******************************************************************************/
extern BSP_S32 balong_dma_channel_set_config (BSP_U32 channel_id, BSP_U32 direction,BSP_U32 burst_width, BSP_U32 burst_len);
#define DRV_EDMA_CHANNEL_CONFIG(channel_id, direction,burst_width,burst_len)  balong_dma_channel_set_config(channel_id, direction,burst_width,burst_len)

/*******************************************************************************
  函数名:      int balong_dma_channel_start (UINT32 channel_id, UINT32 src_addr,
                       UINT32 des_addr, UINT32 len)
  函数描述:    启动一次同步DMA传输, DMA传输完成后，才返回
               使用本函数时，不需要注册中断处理函数
  输入参数:    channel_id：通道ID,调用balong_dma_channel_init函数的返回值
               src_addr：数据传输源地址，必须是物理地址
               des_addr：数据传输目的地址，必须是物理地址
               len：数据传输长度，单位：字节；一次传输数据的最大长度是65535字节
  输出参数:    无
  返回值:      成功：0
               失败：负数
*******************************************************************************/
extern BSP_S32 balong_dma_channel_start (BSP_U32 channel_id, BSP_U32 src_addr, BSP_U32 des_addr, BSP_U32 len);
#define DRV_EDMA_CHANNEL_START(channel_id,src_addr,des_addr,len)  balong_dma_channel_start(channel_id,src_addr,des_addr,len)


/*******************************************************************************
  函数名:      int balong_dma_channel_async_start (UINT32 channel_id,
                unsigned int src_addr, unsigned int des_addr, unsigned int len)
  函数描述:    启动一次异步DMA传输。启动DMA传输后，就返回。不等待DMA传输完成。
               使用本函数时，注册中断处理函数，中断处理函数中处理DMA
传输完成事件
               或者，不注册中断处理函数，使用balong_dma_channel_is_idle函数查询
               DMA传输是否完成
  输入参数:    channel_id：通道ID,调用balong_dma_channel_init函数的返回值
               src_addr：数据传输源地址，必须是物理地址
               des_addr：数据传输目的地址，必须是物理地址
               len：数据传输长度，单位：字节；一次传输数据的最大长度是65535字节
  输出参数:    无
  返回值:      成功：0
               失败：负数
*******************************************************************************/
extern BSP_S32 balong_dma_channel_async_start (BSP_U32 channel_id, BSP_U32 src_addr, BSP_U32 des_addr, BSP_U32 len);
#define DRV_EDMA_CHANNEL_ASYNC_START(channel_id,src_addr,des_addr,len) balong_dma_channel_async_start(channel_id,src_addr,des_addr,len)


/*******************************************************************************
  函数名:      int balong_dma_channel_lli_start (UINT32 channel_id)
  函数描述:    启动链式DMA传输。在链式DMA的所有节点传输都全部完成后才返回。
               链式DMA的每个节点的数据最大传输长度为65535字节。
               注意：调用此函数前，必须设置好链表控制块。
  输入参数:    channel_id：通道ID,调用balong_dma_channel_init函数的返回值
  输出参数:    无
  返回值:      成功：0
               失败：负数
*******************************************************************************/
extern BSP_S32 balong_dma_channel_lli_start (BSP_U32 channel_id);
#define DRV_EDMA_CHANNEL_lli_START(channel_id)   balong_dma_channel_lli_start(channel_id)

/*******************************************************************************
  函数名:      int balong_dma_channel_lli_start (UINT32 channel_id)
  函数描述:    启动链式DMA传输，然后立即返回，不等待DMA传输完成。
               链式DMA的每个节点的数据最大传输长度为65535字节。
               注意：调用此函数前，必须设置好链表控制块。
  输入参数:    channel_id：通道ID,调用balong_dma_channel_init函数的返回值
  输出参数:    无
  返回值:      成功：0
               失败：负数
*******************************************************************************/
extern BSP_S32 balong_dma_channel_lli_async_start (BSP_U32 channel_id);
#define DRV_EDMA_CHANNEL_lli_ASYNC_START(channel_id)   balong_dma_channel_lli_async_start(channel_id)


/******************************************************************************
*
  函数名:       int balong_dma_channel_is_idle (UINT32 channel_id)
  函数描述:     查询DMA通道是否空闲
  输入参数:     channel_id : 通道ID，调用balong_dma_channel_init函数的返回值
  输出参数:     无
  返回值:       0 : 通道忙碌
                1 : 通道空闲
                负数 : 失败
*******************************************************************************/
extern BSP_S32 balong_dma_channel_is_idle (BSP_U32 channel_id);
#define DRV_EDMA_CHANNEL_IS_IDLE(chanel_id) balong_dma_channel_is_idle(chanel_id)
#endif

/*************************EDMA END************************************/

/*************************SEC START***********************************/

/*************************************************
 函 数 名       : DRV_SECURE_SUPPORT
 功能描述   : 当前版本是否支持安全启动
 输入参数   : unsigned char *pData
 输出参数   : unsigned char *pData
 返 回 值      : OK/ERROR
*************************************************/
extern BSP_S32 DRV_SECURE_SUPPORT(BSP_U8 *pu8Data);

/*************************************************
 函 数 名       : DRV_SECURE_ALREADY_USE
 功能描述   : 查询当前版本是否已经启用安全启动
 输入参数   : unsigned char *pData
 输出参数   : unsigned char *pData
 返 回 值      : OK/ERROR
*************************************************/
extern BSP_S32 DRV_SECURE_ALREADY_USE(BSP_U8 *pu8Data);

/*************************************************
 函 数 名       : DRV_START_SECURE
 功能描述   : 启用安全启动
 输入参数   :
 输出参数   :
 返 回 值      : OK/ERROR
*************************************************/
extern BSP_S32 DRV_START_SECURE(void);


/*************************************************
 函 数 名       : DRV_SEC_CHECK
 功能描述   : 安全镜像判断
 输入参数   :
 输出参数   :
 返 回 值      : 0:不是安全镜像
                           1:是安全镜像
                           其他:错误
 调用函数   :
 被调函数   :

 修改历史   :
    日    期       : 2011年03月07日
   修改内容 : 新生成函数

*************************************************/
extern BSP_U32 DRV_SEC_CHECK(void);

/*************************SEC END*************************************/

/*************************MD5相关 START*******************************/

/*****************************************************************************
 函 数 名  : VerifySIMLock
 功能描述  : 判断当前解锁码是否正确 .
 输入参数  : imei       - 单板IMEI号
             unlockcode - 解锁码
 输出参数  : 无。
 返 回 值  : 1：   解锁成功
             0:    解锁失败
*****************************************************************************/
extern int VerifySIMLock(char* UnlockCode, char* Key);
#define DRV_CARDLOCK_MD5_VERIFY(unlockcode, imei)  VerifySIMLock(unlockcode, imei)

/*******************************************************
  函数名称：GetAuthVer
  调用关系：Produce Tool
  输入：
  返回值：
    描述：SIMLock manager模块版本号
    类型：整型值
    如果为1，表示采用早期的版本进行密码获取。即1.0版本。
                    如果为2，则表示2.0版本，采取本次优化之后的版本获取密码。
                    其他为保留值。
 修改历史      :
  1.日    期   : 2011年4月23日
    修改内容   : SIMLock合入

********************************************************/
extern int GetAuthVer(void);
#define DRV_GET_AUTH_VER()  GetAuthVer()

/*************************MD5相关 END*********************************/

/*************************SYNC START**********************************/

/**************************************************************************
  错误码定义
**************************************************************************/
#define BSP_ERR_SYNC_BASE                (BSP_S32)(0x80000000 | (BSP_DEF_ERR(BSP_MODU_SYNC, 0)))
#define BSP_ERR_SYNC_TIMEOUT             (BSP_ERR_SYNC_BASE + 0x0)

/**************************************************************************
  结构定义
**************************************************************************/
typedef enum tagSYNC_MODULE_E
{
    SYNC_MODULE_MEM     = 0,
    SYNC_MODULE_NANDC,
    SYNC_MODULE_PMU,
    SYNC_MODULE_SOCP,
    SYNC_MODULE_DMA,
    SYNC_MODULE_IPF,
    SYNC_MODULE_NV,
    SYNC_MODULE_YAFFS,
    SYNC_MODULE_MSP,
    SYNC_MODULE_GPIO,
    SYNC_MODULE_CSHELL,
    SYNC_MODULE_MNTN,
	SYNC_MODULE_MSPNV,
	SYNC_MODULE_ONLINE,
	SYNC_MODULE_CHG,
	SYNC_MODULE_TFUPDATE,
    SYNC_MODULE_BUTT
}SYNC_MODULE_E;

/**************************************************************************
  函数声明
**************************************************************************/
BSP_S32 BSP_SYNC_Lock(SYNC_MODULE_E u32Module, BSP_U32 *pState, BSP_U32 u32TimeOut);
BSP_S32 BSP_SYNC_UnLock(SYNC_MODULE_E u32Module, BSP_U32 u32State);
BSP_S32 BSP_SYNC_Wait(SYNC_MODULE_E u32Module, BSP_U32 u32TimeOut);
BSP_S32 BSP_SYNC_Give(SYNC_MODULE_E u32Module);

/*************************SYNC END************************************/

/*************************CHG模块START********************************/

/*****************************************************************************
 函 数 名  : BSP_CHG_GetCbcState
 功能描述  : 返回电池状态和电量
 输入参数  :pusBcs 0:电池在供电 1:与电池连接电池未供电 2:没有与电池连接
                          pucBcl  0:电量不足或没有与电池连接
 输出参数  : pusBcs 0:电池在供电 1:与电池连接电池未供电 2:没有与电池连接
                          pucBcl  0:电量不足或没有与电池连接
 返回值：   0 操作成功
                         -1操作失败

*****************************************************************************/
extern int BSP_CHG_GetCbcState(unsigned char *pusBcs,unsigned char *pucBcl);
extern int DRV_CHG_GET_CBC_STATE(unsigned char *pusBcs,unsigned char *pucBcl);

typedef enum CHARGING_STATE_ENUM_tag
{
        CHARGING_INIT = -1,
        NO_CHARGING_UP = 0,  /*开机未充电*/
        CHARGING_UP ,              /*开机正充电*/
        NO_CHARGING_DOWN ,  /*关机未充电*/
        CHARGING_DOWN         /*关机未充电*/
}CHARGING_STATE_ENUM;

typedef enum BATT_LEVEL_ENUM_tag
{
        BATT_INIT = -2,
        BATT_LOW_POWER =-1,    /*电池低电*/
        BATT_LEVEL_0,                 /*0格电池电量*/
        BATT_LEVEL_1,                  /*1格电池电量*/
        BATT_LEVEL_2,                   /*2格电池电量*/
        BATT_LEVEL_3,                    /*3格电池电量*/
        BATT_LEVEL_4,                    /*4格电池电量*/
        BATT_LEVEL_MAX
}BATT_LEVEL_ENUM;

typedef struct BATT_STATE_tag
{
    CHARGING_STATE_ENUM  charging_state;
    BATT_LEVEL_ENUM      battery_level;
    #ifdef FEATURE_HUAWEI_MBB_CHG
    GPIO_TEMP_EVENT      batt_temp_state;
	#endif
#if(FEATURE_ON == FEATURE_HUAWEI_VDF)
    int                       batt_percent;
#endif
}BATT_STATE_T;

/*AT 命令枚举类型*/
typedef enum
{
    CHG_AT_BATTERY_CHECK,        /* Battery校准*/
    CHG_AT_BATTERY_LEVEL,        /* Battery电量 */
    CHG_AT_BATTERY_ADC,          /* Battery Temperature保护*/
    CHG_AT_BATTERY_INVALID
} ENUM_CHG_ATCM_TYPE;

typedef enum ENUM_POWER_ON_MODE_tag
{
    POWER_ON_INVALID = 0,          	/* 无效开机模式*/
    POWER_ON_NORMAL,       	 	/* 正常开机模式*/
    POWER_ON_CHARGING,       	/* 关机充电模式*/
    POWER_ON_MAX
} ENUM_POWER_ON_MODE;
/*****************************************************************************
 函 数 名  : BSP_CHG_GetBatteryState
 功能描述  :获取底层电池状态信息
 输入参数  :battery_state 电量信息
 输出参数  :battery_state 电量信息
 返回值：   0 操作成功
                         -1操作失败

*****************************************************************************/
extern int BSP_CHG_GetBatteryState(BATT_STATE_T *battery_state);
extern int DRV_CHG_GET_BATTERY_STATE(BATT_STATE_T *battery_state);

/*****************************************************************************
 函 数 名  : BSP_CHG_ChargingStatus
 功能描述  :查询目前是否正在充电中
 输入参数  :无
 输出参数  :无
 返回值：   0 未充电
                          1 充电中
*****************************************************************************/
extern int BSP_CHG_ChargingStatus(void);
extern int DRV_CHG_GET_CHARGING_STATUS(void);

/*****************************************************************************
 函 数 名  : BSP_CHG_StateSet
 功能描述  :使能或者禁止充电
 输入参数  :ulState      0:禁止充电
 						1:使能充电
 输出参数  :无
 返回值：    无
*****************************************************************************/
extern void DRV_CHG_STATE_SET(unsigned long ulState);

/*****************************************************************************
 函 数 名  : BSP_CHG_StateGet
 功能描述  :查询
 输入参数  :
 输出参数  :无
 返回值：    无
*****************************************************************************/
extern BSP_S32 BSP_CHG_StateGet(void);
#define DRV_CHG_STATE_GET()    BSP_CHG_StateGet()

/*****************************************************************************
 函 数 名  : BSP_CHG_Sply
 功能描述  :查询
 输入参数  :
 输出参数  :无
 返回值：    无
*****************************************************************************/
extern BSP_S32 BSP_CHG_Sply(void);
#define DRV_CHG_BATT_SPLY()    BSP_CHG_Sply()

#ifdef FEATURE_HUAWEI_MBB_CHG
/* 	AT读接口 for chg normalizaton*/
extern int BSP_TBAT_Read(ENUM_CHG_ATCM_TYPE atID, void *pItem);
extern int BSP_TBAT_Write(ENUM_CHG_ATCM_TYPE atID, void *pItem);
#else
/* 	AT读接口*/
extern BSP_S32 BSP_TBAT_Read(ENUM_CHG_ATCM_TYPE atID, void *pItem);
extern BSP_S32 BSP_TBAT_Write(ENUM_CHG_ATCM_TYPE atID, void *pItem);
#endif /* FEATURE_HUAWEI_MBB_CHG */

/**********************************************************************
函 数 名      : BSP_TBAT_CHRStGet
功能描述  :  TBAT AT^TCHRENABLE?是否需要补电

输入参数  : 无
输出参数  : 无
返 回 值      : 1:需要补电
			      0:不需要补电
注意事项  : 无
***********************************************************************/
extern BSP_S32 BSP_TBAT_CHRStGet(void);

/**********************************************************************
函 数 名      : BSP_TBAT_CHRSuply
功能描述  :  TBAT AT^TCHRENABLE=4补电实现
            			需要提供补电成功LCD显示图样
输入参数  : 无
输出参数  : 无
返 回 值      :
注意事项  : 无
***********************************************************************/
extern BSP_S32 BSP_TBAT_CHRSply(void);

/**********************************************************************
函 数 名      : BSP_ONOFF_StartupModeGet
功能描述  :  A核判断开关机模式
            			
输入参数  : 无
输出参数  : 无
返 回 值      :
注意事项  : 无
***********************************************************************/
ENUM_POWER_ON_MODE BSP_ONOFF_StartupModeGet( void );
/*****************************************************************************
* 函 数 名  : BSP_PMU_BattCali
*
* 功能描述  : 电池校准前减小单板电流接口，给AT调用 
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
extern void BSP_PMU_BattCali(void);


/*************************CHG模块 END*********************************/

/*************************ICC BEGIN***********************************/
/**************************************************************************
  宏定义
**************************************************************************/
#define ICC_CHAN_NUM_MAX            (32)

#define ICC_CHAN_PRIORITY_HIGH      (0)
#define ICC_CHAN_PRIORITY_LOW       (255)
#define ICC_CHAN_PRIORITY_DEFAULT   ICC_CHAN_PRIORITY_LOW

#define ICC_CHAN_FIFO_MIN           (32)
#define ICC_CHAN_FIFO_MAX           (65536)
#define ICC_CHAN_FIFO_SIZE1         (1024)
#define ICC_CHAN_FIFO_SIZE2         (2048)
#define ICC_CHAN_FIFO_SIZE3         (4096)

/**************************************************************************
  结构定义
**************************************************************************/
typedef enum tagICC_CHAN_STATE_E
{
    ICC_CHAN_STATE_CLOSED   = 0,
    ICC_CHAN_STATE_OPENED,
    ICC_CHAN_STATE_CLOSEING,
    ICC_CHAN_STATE_BUTT
}ICC_CHAN_STATE_E;

typedef enum tagICC_CHAN_MODE_E{
    ICC_CHAN_MODE_STREAM    = 0,
    ICC_CHAN_MODE_PACKET,
    ICC_CHAN_MODE_BUTT
}ICC_CHAN_MODE_E;

typedef enum tagICC_IOCTL_CMD_E{
    ICC_IOCTL_SET_WRITE_CB  = 0,
    ICC_IOCTL_SET_READ_CB,
    ICC_IOCTL_SET_EVENT_CB,
    ICC_IOCTL_GET_STATE,
    ICC_IOCTL_SET_BUTT
}ICC_IOCTL_CMD_E;


typedef enum tagICC_EVENT_E{
    ICC_EVENT_CLOSE         = 0,
    ICC_EVENT_RESET,
    ICC_EVENT_BUTT
}ICC_EVENT_E;


typedef BSP_U32 (*icc_event_cb)(BSP_U32 u32ChanID, BSP_U32 u32Event, BSP_VOID* Param);
typedef BSP_U32 (*icc_write_cb)(BSP_U32 u32ChanID);
typedef BSP_U32 (*icc_read_cb)(BSP_U32 u32ChanID, BSP_S32 u32Size);

typedef struct tagICC_CHAN_ATTR_S{
    BSP_U32             u32FIFOInSize;
    BSP_U32             u32FIFOOutSize;
    BSP_U32             u32Priority;
    ICC_CHAN_MODE_E     enChanMode;
    BSP_U32             u32TimeOut;
    icc_event_cb        event_cb;
    icc_write_cb        write_cb;
    icc_read_cb         read_cb;
}ICC_CHAN_ATTR_S;

/**************************************************************************
  函数声明
**************************************************************************/
BSP_S32 BSP_ICC_Open(BSP_U32 u32ChanId, ICC_CHAN_ATTR_S *pChanAttr);
BSP_S32 BSP_ICC_Close(BSP_U32 u32ChanId);
BSP_S32 BSP_ICC_Write(BSP_U32 u32ChanId, BSP_U8* pData, BSP_S32 s32Size);
BSP_S32 BSP_ICC_Read(BSP_U32 u32ChanId, BSP_U8* pData, BSP_S32 s32Size);
BSP_S32 BSP_ICC_Ioctl(BSP_U32 u32ChanId, BSP_U32 cmd, BSP_VOID *param);
BSP_U32 BSP_ICC_CanSleep(BSP_U32 u32Flag);

/**************************************************************************
  错误码定义
**************************************************************************/
#define BSP_ERR_ICC_BASE                (BSP_S32)(0x80000000 | (BSP_DEF_ERR(BSP_MODU_ICC, 0)))
#define BSP_ERR_ICC_NOT_INIT            (BSP_ERR_ICC_BASE + 0x1)
#define BSP_ERR_ICC_NOT_CONNECT         (BSP_ERR_ICC_BASE + 0x2)
#define BSP_ERR_ICC_NULL                (BSP_ERR_ICC_BASE + 0x3)
#define BSP_ERR_ICC_NO_MEM              (BSP_ERR_ICC_BASE + 0x4)
#define BSP_ERR_ICC_INVALID_CHAN        (BSP_ERR_ICC_BASE + 0x5)
#define BSP_ERR_ICC_INVALID_PARAM       (BSP_ERR_ICC_BASE + 0x6)
#define BSP_ERR_ICC_EXIST               (BSP_ERR_ICC_BASE + 0x7)
#define BSP_ERR_ICC_NOT_EXIST           (BSP_ERR_ICC_BASE + 0x8)
#define BSP_ERR_ICC_OPENED              (BSP_ERR_ICC_BASE + 0x9)
#define BSP_ERR_ICC_NOT_OPEN            (BSP_ERR_ICC_BASE + 0xa)
#define BSP_ERR_ICC_NO_SEM              (BSP_ERR_ICC_BASE + 0xb)
#define BSP_ERR_ICC_NEGOTIATE_FAIL      (BSP_ERR_ICC_BASE + 0xc)
#define BSP_ERR_ICC_BUFFER_FULL         (BSP_ERR_ICC_BASE + 0xd)
#define BSP_ERR_ICC_BUFFER_EMPTY        (BSP_ERR_ICC_BASE + 0xe)
#define BSP_ERR_ICC_INVALID_PACKET      (BSP_ERR_ICC_BASE + 0xf)
#define BSP_ERR_ICC_PACKET_SIZE         (BSP_ERR_ICC_BASE + 0x10)
#define BSP_ERR_ICC_USER_BUFFER_SIZE    (BSP_ERR_ICC_BASE + 0x11)

/*************************ICC END*************************************/



/*************************IFC START*************************************/
/* 模块ID枚举*/
typedef enum tagIFC_MODULE_E
{
    IFC_MODULE_BSP=  0,
    IFC_MODULE_LPS=  1,
    IFC_MODULE_GUPS= 2,
    IFC_MODULE_LMSP = 3,
    IFC_MODULE_GUOM = 4,
    IFC_MODULE_BUTT
} IFC_MODULE_E;


/* IFC回调函数结构*/
/* pMsgBody:该函数ID对应函数的参数,前四字节需保证为moduleId*/
/* u32Len:pMsgBody长度*/
typedef BSP_S32 (*BSP_IFC_REG_FUNC)(BSP_VOID *pMsgBody,BSP_U32 u32Len);

/*****************************************************************************
* 函 数 名  : BSP_IFC_RegFunc
*
* 功能描述  : 注册IFC回调函数接口
*
* 输入参数  : IFC_MODULE_E enModuleId    模块ID
*             BSP_IFC_REG_FUNC pFunc     回调函数指针
* 输出参数  : 无
*
* 返 回 值  : BSP_SUCCESS
*             BSP_ERR_MODULE_NOT_INITED
*             BSP_ERR_INVALID_PARA
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_U32 BSP_IFC_RegFunc(IFC_MODULE_E enModuleId, BSP_IFC_REG_FUNC pFunc);


/*****************************************************************************
* 函 数 名  : BSP_IFC_Send
*
* 功能描述  : IFC发送消息接口
*
* 输入参数  : BSP_VOID * pMspBody           发送消息内容
*             BSP_U32 u32Len               pMspBody的长度
* 输出参数  : 无
*
* 返 回 值  : BSP_SUCCESS
*             BSP_ERR_MODULE_NOT_INITED
*             BSP_ERR_INVALID_PARA
*             BSP_ERR_IFC_SEND_FAIL
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_U32 BSP_IFC_Send(BSP_VOID * pMspBody, BSP_U32 u32Len);

/*************************IFC END*************************************/

/*************************Build优化 START*****************************/
/*************************内存分配 START******************************/

/* DDR中各个内存段的类型 */
typedef enum tagBSP_DDR_SECT_TYPE_E
{
    BSP_DDR_SECT_TYPE_TTF = 0x0,
    BSP_DDR_SECT_TYPE_ARMDSP,
    BSP_DDR_SECT_TYPE_UPA,
    BSP_DDR_SECT_TYPE_CQI,
    BSP_DDR_SECT_TYPE_APT,
    BSP_DDR_SECT_TYPE_ET,
    BSP_DDR_SECT_TYPE_BBPMASTER,
    BSP_DDR_SECT_TYPE_NV,
    BSP_DDR_SECT_TYPE_DICC,
    BSP_DDR_SECT_TYPE_WAN,
    BSP_DDR_SECT_TYPE_SHARE_MEM,
    BSP_DDR_SECT_TYPE_EXCP,
    BSP_DDR_SECT_TYPE_HIFI,
    BSP_DDR_SECT_TYPE_TDS_LH2,
    BSP_DDR_SECT_TYPE_BUTTOM
}BSP_DDR_SECT_TYPE_E;


/* AXI中各个内存段的类型 */
typedef enum tagBSP_AXI_SECT_TYPE_E
{
    BSP_AXI_SECT_TYPE_ACORE_DEEPSLEEP = 0x0,
    BSP_AXI_SECT_TYPE_FLASH_SEM,
    BSP_AXI_SECT_TYPE_IFC,
    BSP_AXI_SECT_TYPE_ICC,
    BSP_AXI_SECT_TYPE_MEMMGR_FLAG,
    BSP_AXI_SECT_TYPE_DYNAMIC,
    BSP_AXI_SECT_TYPE_SOFT_FLAG,
    BSP_AXI_SECT_TYPE_IPF,
    BSP_AXI_SECT_TYPE_TEMPERATURE,
    BSP_AXI_SECT_TYPE_ONOFF,
    BSP_AXI_SECT_TYPE_DICC,
    BSP_AXI_SECT_TYPE_HIFI,
    BSP_AXI_SECT_TYPE_PTABLE,
    BSP_AXI_SECT_TYPE_RESERVE,
    BSP_AXI_SECT_TYPE_DLOAD_AUTOINSTALL,
    BSP_AXI_SECT_TYPE_DLOAD,
    BSP_AXI_SECT_TYPE_GUDSP_LOG,
    BSP_AXI_SECT_TYPE_BUTTOM
}BSP_AXI_SECT_TYPE_E;


/* 内存段属性 */
typedef enum tagBSP_DDR_SECT_ATTR_E
{
    BSP_DDR_SECT_ATTR_CACHEABLE = 0x0,
    BSP_DDR_SECT_ATTR_NONCACHEABLE,
    BSP_DDR_SECT_ATTR_BUTTOM
}BSP_DDR_SECT_ATTR_E;


/* 定义虚实地址是否相同的枚举 */
typedef enum tagBSP_DDR_SECT_PVADDR_E
{
    BSP_DDR_SECT_PVADDR_EQU = 0x0,
    BSP_DDR_SECT_PVADDR_NOT_EQU,
    BSP_DDR_SECT_PVADDR_BUTTOM
}BSP_DDR_SECT_PVADDR_E;


/* DDR内存段的查询结构 */
typedef struct tagBSP_DDR_SECT_QUERY
{
    BSP_DDR_SECT_TYPE_E     enSectType;
    BSP_DDR_SECT_ATTR_E     enSectAttr;
    BSP_DDR_SECT_PVADDR_E   enPVAddr;
    BSP_U32                 ulSectSize;
}BSP_DDR_SECT_QUERY;


/* DDR内存段的详细信息 */
typedef struct tagBSP_DDR_SECT_INFO
{
    BSP_DDR_SECT_TYPE_E    enSectType;
    BSP_DDR_SECT_ATTR_E    enSectAttr;
    BSP_U32                ulSectVirtAddr;
    BSP_U32                ulSectPhysAddr;
    BSP_U32                ulSectSize;
}BSP_DDR_SECT_INFO;


/* AXI内存段的详细信息 */
typedef struct tagBSP_AXI_SECT_INFO
{
    BSP_AXI_SECT_TYPE_E    enSectType;
    BSP_U32                ulSectVirtAddr;
    BSP_U32                ulSectPhysAddr;
    BSP_U32                ulSectSize;
}BSP_AXI_SECT_INFO;

/*****************************************************************************
 函 数 名  : BSP_DDR_GetSectInfo
 功能描述  : DDR内存段查询接口
 输入参数  : pstSectQuery: 需要查询的内存段类型、属性
 输出参数  : pstSectInfo:  查询到的内存段信息
 返回值    ：BSP_OK/BSP_ERROR
*****************************************************************************/
BSP_S32 BSP_DDR_GetSectInfo(BSP_DDR_SECT_QUERY *pstSectQuery, BSP_DDR_SECT_INFO *pstSectInfo);
#define DRV_GET_FIX_DDR_ADDR(pstSectQuery, pstSectInfo) BSP_DDR_GetSectInfo(pstSectQuery, pstSectInfo)

/*****************************************************************************
 函 数 名  : BSP_AXI_GetSectInfo
 功能描述  : AXI内存段查询接口
 输入参数  : enSectType: 需要查询的内存段类型
 输出参数  : pstSectInfo:  查询到的内存段信息
 返回值    ：BSP_OK/BSP_ERROR
*****************************************************************************/
BSP_S32 BSP_AXI_GetSectInfo(BSP_AXI_SECT_TYPE_E enSectType, BSP_AXI_SECT_INFO *pstSectInfo);
#define DRV_GET_FIX_AXI_ADDR(enSectType, pstSectInfo) BSP_AXI_GetSectInfo(enSectType, pstSectInfo)

/*************************内存分配 END********************************/

/*************************IP基地址、中断号查询 START******************/

/* 定义所有需要查询的IP类型 */
typedef enum tagBSP_IP_TYPE_E
{
    BSP_IP_TYPE_SOCP = 0x0,
    BSP_IP_TYPE_CICOM0,
    BSP_IP_TYPE_CICOM1,
    BSP_IP_TYPE_HDLC,
    BSP_IP_TYPE_BBPMASTER,
    BSP_IP_TYPE_ZSP_ITCM,
    BSP_IP_TYPE_ZSP_DTCM,
    BSP_IP_TYPE_AHB,
    BSP_IP_TYPE_WBBP,
    BSP_IP_TYPE_WBBP_DRX,
    BSP_IP_TYPE_GBBP,
    BSP_IP_TYPE_GBBP_DRX,
    BSP_IP_TYPE_ZSPDMA,
    BSP_IP_TYPE_SYSCTRL,
    BSP_IP_TYPE_TDSSYS,
    BSP_IP_TYPE_ZSPDHI,
    BSP_IP_TYPE_BUTTOM
}BSP_IP_TYPE_E;

/*****************************************************************************
 函 数 名  : BSP_GetIPBaseAddr
 功能描述  : IP基地址查询
 输入参数  : enIPType: 需要查询的IP类型
 输出参数  : 无
 返回值    ：查询到的IP基地址
*****************************************************************************/
BSP_U32 BSP_GetIPBaseAddr(BSP_IP_TYPE_E enIPType);
#define DRV_GET_IP_BASE_ADDR(enIPType)  BSP_GetIPBaseAddr(enIPType)


/* 需要查询的中断类型 */
typedef enum tagBSP_INT_TYPE_E
{
    BSP_INT_TYPE_RTC = 0,
    BSP_INT_TYPE_WDT,
    BSP_INT_TYPE_USBOTG,
    BSP_INT_TYPE_UICC,
    BSP_INT_TYPE_IPF_C,
    BSP_INT_TYPE_IPF_A,
    BSP_INT_TYPE_SOCP_A,
    BSP_INT_TYPE_SOCP_C,
    BSP_INT_TYPE_CICOM0,
    BSP_INT_TYPE_CICOM1,
    BSP_INT_TYPE_HDLC,
    BSP_INT_TYPE_BBPMASTER,
    BSP_INT_TYPE_GBBP,          /*G_BBP_INT_LEVEL*/
    BSP_INT_TYPE_GBBP_AWAKE,    /*G_BBP_AWAKE_INT_LEVEL*/
    BSP_INT_TYPE_WBBP_0MS,      /*W_BBP_0MS_INT_LEVEL*/
    BSP_INT_TYPE_WBBP_AWAKE,    /*W_BBP_AWAKE_INT_LEVEL*/
    BSP_INT_TYPE_WBBP_SWITCH,   /*W_BBP_CLOCK_SWITCH_INT_LEVEL*/
    BSP_INT_TYPE_INT_OSRTC,     /*RTC_DUAL_TIMER_INT_LEVEL*/
    BSP_INT_TYPE_INT_SIMI,      /*SOC_SMIM_INT_LEVEL*/
    BSP_INT_TYPE_INT_ZSP_DOG,   /*ZSP_WDG_INT_LEVEL*/
    BSP_INT_TYPE_INT_HIFI_DOG,  /*HIFI_WDG_INT_LEVEL*/
    BSP_INT_TYPE_INT_DRX_TIMER, /**/
    BSP_INT_TYPE_BUTTOM
}BSP_INT_TYPE_E;


/*****************************************************************************
 函 数 名  : BSP_GetIntNO
 功能描述  : 中断号查询
 输入参数  : enIntType: 需要查询的中断类型
 输出参数  : 无
 返回值    ：查询到的中断号
*****************************************************************************/
BSP_S32 BSP_GetIntNO(BSP_INT_TYPE_E enIntType);
#define DRV_GET_INT_NO(enIntType)    BSP_GetIntNO(enIntType)


/*************************IP基地址、中断号查询 END********************/

/*************************查询模块是否支持 START**********************/

/* 当前版本是否支持某个模块 */
typedef enum tagBSP_MODULE_SUPPORT_E
{
    BSP_MODULE_SUPPORT     = 0,
    BSP_MODULE_UNSUPPORT   = 1,
    BSP_MODULE_SUPPORT_BUTTOM
}BSP_MODULE_SUPPORT_E;

/* 查询的模块类型 */
typedef enum tagBSP_MODULE_TYPE_E
{
    BSP_MODULE_TYPE_SD = 0x0,
    BSP_MODULE_TYPE_CHARGE,
    BSP_MODULE_TYPE_WIFI,
    BSP_MODULE_TYPE_OLED,
    BSP_MODULE_TYPE_HIFI,
    BSP_MODULE_TYPE_POWER_ON_OFF,
    BSP_MODULE_TYPE_HSIC,
    BSP_MODULE_TYPE_LOCALFLASH,    
    BSP_MODULE_TYPE_BUTTOM
}BSP_MODULE_TYPE_E;

/*****************************************************************************
 函 数 名  : BSP_CheckModuleSupport
 功能描述  : 查询模块是否支持
 输入参数  : enModuleType: 需要查询的模块类型
 输出参数  : 无
 返回值    ：BSP_MODULE_SUPPORT或BSP_MODULE_UNSUPPORT
*****************************************************************************/
BSP_MODULE_SUPPORT_E BSP_CheckModuleSupport(BSP_MODULE_TYPE_E enModuleType);

#define DRV_GET_BATTERY_SUPPORT()   BSP_CheckModuleSupport(BSP_MODULE_TYPE_CHARGE)

#define DRV_GET_WIFI_SUPPORT()      BSP_CheckModuleSupport(BSP_MODULE_TYPE_WIFI)

#define DRV_GET_SD_SUPPORT()        BSP_CheckModuleSupport(BSP_MODULE_TYPE_SD)

#define DRV_GET_OLED_SUPPORT()      BSP_CheckModuleSupport(BSP_MODULE_TYPE_OLED)

#define DRV_GET_HIFI_SUPPORT()      BSP_CheckModuleSupport(BSP_MODULE_TYPE_HIFI)

#define DRV_GET_HSIC_SUPPORT()      BSP_CheckModuleSupport(BSP_MODULE_TYPE_HSIC)

#define DRV_GET_LOCAL_FLASH_SUPPORT()      BSP_CheckModuleSupport(BSP_MODULE_TYPE_LOCALFLASH)
/*************************查询模块是否支持 END************************/

/*************************虚实地址转换 START**************************/

/*****************************************************************************
 函 数 名  : DRV_DDR_VIRT_TO_PHY
 功能描述  : DDR内存虚地址往实地址转换
 输入参数  : ulVAddr；虚地址
 输出参数  : 无
 返回值    ：实地址
*****************************************************************************/
unsigned int DRV_DDR_VIRT_TO_PHY(unsigned int ulVAddr);

/*****************************************************************************
 函 数 名  : DRV_DDR_PHY_TO_VIRT
 功能描述  : DDR内存虚地址往实地址转换
 输入参数  : ulPAddr；实地址
 输出参数  : 无
 返回值    ：虚地址
*****************************************************************************/
unsigned int DRV_DDR_PHY_TO_VIRT(unsigned int ulPAddr);

/*****************************************************************************
 函 数 名  : TTF_VIRT_TO_PHY
 功能描述  : TTF内存虚地址往实地址转换
 输入参数  : ulVAddr；虚地址
 输出参数  : 无
 返回值    ：实地址
*****************************************************************************/
extern unsigned int TTF_VIRT_TO_PHY(unsigned int ulVAddr);

/*****************************************************************************
 函 数 名  : TTF_PHY_TO_VIRT
 功能描述  : TTF内存虚地址往实地址转换
 输入参数  : ulPAddr；实地址
 输出参数  : 无
 返回值    ：虚地址
*****************************************************************************/
extern unsigned int TTF_PHY_TO_VIRT(unsigned int ulPAddr);

/*****************************************************************************
 函 数 名  : IPF_VIRT_TO_PHY
 功能描述  : IPF寄存器虚地址往实地址转换
 输入参数  : ulVAddr；虚地址
 输出参数  : 无
 返回值    ：实地址
*****************************************************************************/
extern unsigned int IPF_VIRT_TO_PHY(unsigned int ulVAddr);

/*****************************************************************************
 函 数 名  : IPF_PHY_TO_VIRT
 功能描述  : IPF寄存器虚地址往实地址转换
 输入参数  : ulPAddr；实地址
 输出参数  : 无
 返回值    ：虚地址
*****************************************************************************/
extern unsigned int IPF_PHY_TO_VIRT(unsigned int ulPAddr);

/*************************虚实地址转换 END****************************/

/*************************OAM组件新增接口 START***********************/

/*****************************************************************************
 函 数 名  : BSP_GUDSP_ShareAddrGet
 功能描述  : 获取物理层共享地址段的信息。
 输入参数  : 无。
 输出参数  : pulAddr：存放物理层共享地址段信息的缓存。
 返 回 值  : 无。
*****************************************************************************/
extern int BSP_GUDSP_ShareAddrGet(unsigned int * pulAddrInTcm, unsigned int * pulAddrInDdr, unsigned int * pulLength);
#define DRV_DSP_SHARE_ADDR_GET(pulAddrInTcm,pulAddrInDdr,pulLength)    BSP_GUDSP_ShareAddrGet(pulAddrInTcm,pulAddrInDdr,pulLength)

/*****************************************************************************
 函 数 名  : BSP_UpateDSPShareInfo
 功能描述  : 更新物理层BSS COMMON段信息。
 输入参数  : 无。
 输出参数  : 无
 返 回 值  : 0:成功，-1:失败。
*****************************************************************************/
extern int BSP_GUDSP_UpateShareInfo(void);
#define DRV_BSP_UPDATE_DSP_SHAREINFO()  BSP_GUDSP_UpateShareInfo()

/* 下行业务数据DMA搬移完成中断,此中断只对应Modem核*/
typedef BSP_VOID (*BSPBBPIntDlTbFunc)(BSP_VOID);
/* BBP子帧中断处理函数,此中断只对应Modem核*/
typedef BSP_VOID (*BSPBBPIntTimerFunc)(BSP_VOID);

/*****************************************************************************
* 函 数 名  : BSP_BBPIntTimerRegCb
*
* 功能描述  : 被PS调用，用来向底软注册1ms定时中断的回调
*
* 输入参数  : 
* 输出参数  :无
*
* 返 回 值  : VOID
*
* 修改记录  : 2011年3月7日  wangjing  creat
*****************************************************************************/
extern BSP_VOID BSP_BBPIntTimerRegCb(BSPBBPIntTimerFunc pFunc);

/*****************************************************************************
* 函 数 名  : BSP_BBPIntTimerClear
*
* 功能描述  : 被PS调用，用来清除1ms定时中断
*
* 输入参数  : 无
* 输出参数  :无
*
* 返 回 值  : VOID
*
* 修改记录  : 2011年5月31日  wangjing  creat
*****************************************************************************/
extern BSP_VOID BSP_BBPIntTimerClear(void);

/*****************************************************************************
* 函 数 名  : BSP_BBPIntTimerEnable
*
* 功能描述  : 被PS调用，用来打开1ms定时中断
*
* 输入参数  : 无
* 输出参数  :无
*
* 返 回 值  : VOID
*
* 修改记录  : 2011年3月7日  wangjing  creat
*****************************************************************************/
extern BSP_S32 BSP_BBPIntTimerEnable(void);


/*****************************************************************************
* 函 数 名  : BSP_BBPIntTimerDisable
*
* 功能描述  : 被PS调用，用来关闭1ms定时中断
*
* 输入参数  : 无
* 输出参数  :无
*
* 返 回 值  : VOID
*
* 修改记录  : 2011年3月7日  wangjing  creat
*****************************************************************************/
extern BSP_VOID BSP_BBPIntTimerDisable(void);

/*****************************************************************************
* 函 数 名  : BSP_BBPGetCurTime
*
* 功能描述  : 被PS调用，用来获取系统精确时间
*
* 输入参数  : 无
* 输出参数  : BSP_U32 u32CurTime:当前时刻
*
* 返 回 值  : BSP_OK : 获取成功
*                        BSP_ERR_INVALID_PARA  :获取失败
*
* 修改记录  : 2011年3月7日  wangjing  creat
*****************************************************************************/
extern BSP_U32 BSP_BBPGetCurTime(BSP_U64 *pCurTime);

/*****************************************************************************
* 函 数 名  : BSP_BBPIntDlTbRegCb
*
* 功能描述  : 被PS调用，用来向底软注册下行数据DMA搬移完成中断的回调
*
* 输入参数  : 
* 输出参数  :无
*
* 返 回 值  : VOID
*
* 修改记录  : 2011年3月7日  wangjing  creat
*****************************************************************************/
extern BSP_VOID BSP_BBPIntDlTbRegCb(BSPBBPIntDlTbFunc pFunc);

/*****************************************************************************
* 函 数 名  : BSP_GetSysFrame
*
* 功能描述  : get system frame num 
*
* 输入参数  : 无
* 输出参数  :
*
* 返 回 值  : frame num/0xffff
*
* 修改记录  : 2012年4月18日  wangjing  creat
*****************************************************************************/
extern BSP_U32 BSP_GetSysFrame(BSP_VOID);

/*****************************************************************************
* 函 数 名  : BSP_GetSysSubFrame
*
* 功能描述  : get sub system frame num 
*
* 输入参数  : 无
* 输出参数  :
*
* 返 回 值  : frame num/0xffff
*
* 修改记录  : 2012年4月18日  wangjing  creat
*****************************************************************************/
extern BSP_U32 BSP_GetSysSubFrame(BSP_VOID);

/*****************************************************************************
 函 数 名  : BSP_GetSliceValue
 功能描述  : 获取Slice定时器的值。
 输入参数  : 无。
 输出参数  : 无
 返 回 值  : 定时器的值。
*****************************************************************************/
extern unsigned int BSP_GetSliceValue(BSP_VOID);
#define DRV_GET_SLICE()   BSP_GetSliceValue()

/*****************************************************************************
 函 数 名  : BSP_StartHardTimer
 功能描述  : 启动一个定时器的值。
 输入参数  : 无。
 输出参数  : 无
 返 回 值  : 定时器的值。
*****************************************************************************/
extern BSP_VOID BSP_StartHardTimer( BSP_U32 value );
#define DRV_STATR_HARD_TIMER(value)   BSP_StartHardTimer(value)

/*****************************************************************************
 函 数 名  : BSP_StartHardTimer
 功能描述  : 启动一个定时器的值。
 输入参数  : 无。
 输出参数  : 无
 返 回 值  : 定时器的值。
*****************************************************************************/
extern BSP_VOID BSP_StopHardTimer(BSP_VOID);
#define DRV_STOP_HARD_TIMER()   BSP_StopHardTimer()

/*****************************************************************************
 函 数 名  : BSP_GetHardTimerCurTime
 功能描述  : 获取一个定时器的剩余值。
 输入参数  : 无。
 输出参数  : 无
 返 回 值  : 定时器的剩余值。
*****************************************************************************/
extern BSP_U32 BSP_GetHardTimerCurTime(BSP_VOID);
#define DRV_GET_TIMER_CUR_TIME()   BSP_GetHardTimerCurTime()

/*****************************************************************************
 函 数 名  : BSP_ClearTimerINT
 功能描述  : 清除一个定时器的中断
 输入参数  : 无。
 输出参数  : 无
 返 回 值  : 定时器的剩余值。
*****************************************************************************/
extern BSP_VOID BSP_ClearTimerINT(BSP_VOID);
#define DRV_CLEAR_TIMER_INT()   BSP_ClearTimerINT()

/*****************************************************************************
 函 数 名  : BSP_StartHardTimer
 功能描述  : 启动一个定时器的值。
 输入参数  : 无。
 输出参数  : 无
 返 回 值  : 定时器的值。
*****************************************************************************/
extern BSP_VOID BSP_StartDrxTimer( BSP_U32 value );
#define DRV_STATR_DRX_TIMER(value)   BSP_StartDrxTimer(value)

/*****************************************************************************
 函 数 名  : BSP_StartHardTimer
 功能描述  : 启动一个定时器的值。
 输入参数  : 无。
 输出参数  : 无
 返 回 值  : 定时器的值。
*****************************************************************************/
extern BSP_VOID BSP_StopDrxTimer(BSP_VOID);
#define DRV_STOP_DRX_TIMER()   BSP_StopDrxTimer()

/*****************************************************************************
 函 数 名  : BSP_GetHardTimerCurTime
 功能描述  : 获取一个定时器的剩余值。
 输入参数  : 无。
 输出参数  : 无
 返 回 值  : 定时器的剩余值。
*****************************************************************************/
extern BSP_U32 BSP_GetDrxTimerCurTime(BSP_VOID);
#define DRV_GET_DRX_TIMER_CUR_TIME()   BSP_GetDrxTimerCurTime()

/*****************************************************************************
 函 数 名  : BSP_ClearTimerINT
 功能描述  : 清除一个定时器的中断
 输入参数  : 无。
 输出参数  : 无
 返 回 值  : 定时器的剩余值。
*****************************************************************************/
extern BSP_VOID BSP_ClearDrxTimerINT(BSP_VOID);
#define DRV_CLEAR_DRX_TIMER_INT()   BSP_ClearDrxTimerINT()

/*****************************************************************************
* 函 数 名  : BSP_PWC_SetDrxTimerWakeSrc
* 功能描述  : 设置DRX timer 作为唤醒源
* 输入参数  : 
* 输出参数  : 
* 返 回 值   :
* 修改记录  : 
*****************************************************************************/
extern BSP_VOID BSP_PWC_SetDrxTimerWakeSrc(VOID);
#define DRV_SET_DRX_TIMER_WAKE_SRC() BSP_PWC_SetDrxTimerWakeSrc()
/*****************************************************************************
* 函 数 名  : BSP_PWC_DelDrxTimerWakeSrc
* 功能描述  : 设置DRX timer 不作为唤醒源
* 输入参数  : 
* 输出参数  : 
* 返 回 值  :
* 修改记录  : 
*****************************************************************************/
extern BSP_VOID BSP_PWC_DelDrxTimerWakeSrc(VOID);
#define DRV_DEL_DRX_TIMER_WAKE_SRC() BSP_PWC_DelDrxTimerWakeSrc()
/**************DRX对齐 Timer20***************/

/*****************************************************************************
 函 数 名  : BSP_ClearZSPWatchDogInt
 功能描述  : 清除一个ZSP WatchDog的中断
 输入参数  : 无。
 输出参数  : 无
 返 回 值  : 中断是否需要处理
*****************************************************************************/
extern BSP_BOOL BSP_ClearZSPWatchDogInt(BSP_VOID);
#define DRV_CLEAR_ZSPDOG_INT()   BSP_ClearZSPWatchDogInt()

/*****************************************************************************
 函 数 名  : BSP_ClearHIFIWatchDogInt
 功能描述  : 清除一个HIFI WatchDog的中断
 输入参数  : 无。
 输出参数  : 无
 返 回 值  : 中断是否需要处理
*****************************************************************************/
extern BSP_BOOL BSP_ClearHIFIWatchDogInt(BSP_VOID);
#define DRV_CLEAR_HIFIDOG_INT()   BSP_ClearHIFIWatchDogInt()

/*****************************************************************************
 函 数 名  : BSP_ZspWatchDogIntEnalbe
 功能描述  : 使能一个ZSP WatchDog的中断
 输入参数  : 无。
 输出参数  : 无
 返 回 值  : 中断是否需要处理
*****************************************************************************/
extern BSP_VOID BSP_ZSPWatchDogIntEnalbe(int level);
#define DRV_ZSPDOG_INT_ENABLE(level) BSP_ZSPWatchDogIntEnalbe(level)

/*****************************************************************************
 函 数 名  : BSP_ZspWatchDogIntEnalbe
 功能描述  : 使能一个ZSP WatchDog的中断
 输入参数  : 无。
 输出参数  : 无
 返 回 值  : 中断是否需要处理
*****************************************************************************/
extern BSP_VOID BSP_HIFIWatchDogIntEnalbe(int level);
#define DRV_HIFIDOG_INT_ENABLE(level) BSP_HIFIWatchDogIntEnalbe(level)

/*****************************************************************************
 函 数 名  : BSP_SendNMIInterrupt
 功能描述  : 发送 NMI 的中断
 输入参数  : NMI中断的bit位
 输出参数  : 无
 返 回 值  : 中断是否需要处理
*****************************************************************************/
extern BSP_VOID BSP_SendNMIInterrupt(unsigned int SocBitNO, unsigned int ZspBitNO);
#define DRV_SEND_NMI_INT(SocBitNO,ZspBitNO) BSP_SendNMIInterrupt(SocBitNO,ZspBitNO)

/*****************************************************************************
 函 数 名  : BSP_InitPlatformVerInfo
 功能描述  : 初始化芯片的版本号
 输入参数  : 无
 输出参数  : none
 返 回 值  : void
*****************************************************************************/
extern BSP_VOID BSP_InitPlatformVerInfo(BSP_VOID);
#define DRV_INIT_PLATFORM_VER() BSP_InitPlatformVerInfo()

/******************************************************************************
*  Function:  DRV_START_MODEGET
*  Description:
*  History:
********************************************************************************/
extern BSP_S32  DRV_START_MODEGET(void);

/*****************************************************************************
 函 数 名  : BSP_GetPlatformInfo
 功能描述  : 获取芯片的版本号
 输入参数  : 无
 输出参数  : u32PlatformInfo:芯片的版本号
 返 回 值  : void
*****************************************************************************/
extern BSP_VOID BSP_GetPlatformInfo(unsigned long *u32PlatformInfo);
#define DRV_GET_PLATFORM_INFO(u32PlatformInfo) BSP_GetPlatformInfo(u32PlatformInfo)

/*产线版本写SDT信息的文件系统的分区名*/
#define MANUFACTURE_ROOT_PATH "/manufacture"

/*****************************************************************************
 函 数 名  : drvOnLineUpdateResult
 功能描述  : WebUI模块函数注册。
 输入参数  : 无
 输出参数  : 无。
 返 回 值  : 无
 注意事项  ：
*****************************************************************************/
extern void drvOnLineUpdateResult(void);
#define DRV_ONLINE_UPDATE_RESULT()    drvOnLineUpdateResult()

/******************************************************************************
*
  函数名:       BSP_S32 nand_get_bad_block (BSP_U32 *len, BSP_U32 **ppBadBlock)
  函数描述:     查询整个NAND的所有FLASH 坏块
  输入参数:     无
  输出参数:     pNum       : 返回坏块个数
                ppBadBlock ：数组指针，返回所有坏块的index索引
  返回值:       0    : 查询成功
                负数 : 查询失败
*******************************************************************************/
extern BSP_S32 nand_get_bad_block(BSP_U32 *pNum, BSP_U32 **ppBadBlock);
#define NAND_GET_BAD_BLOCK(pNum, ppBadBlock) nand_get_bad_block(pNum, ppBadBlock)

/******************************************************************************
*
  函数名:       BSP_VOID  nand_free_bad_block_mem(BSP_U32* pBadBlock)
  函数描述:     通过nand_get_bad_block接口申请的坏块内存由底软申请，由协议栈调用
                该接口释放。
  输入参数:     pBadBlock
  输出参数:     无
  返回值:       BSP_VOID
*******************************************************************************/
extern BSP_VOID  nand_free_bad_block_mem(BSP_U32* pBadBlock);
#define NAND_FREE_BAD_BLOCK_MEM(pBadBlock)  nand_free_bad_block_mem(pBadBlock)

#define NAND_MFU_NAME_MAX_LEN    16
#define NAND_DEV_SPEC_MAX_LEN    32

typedef struct
{
    BSP_U32           MufId;                                         /* 厂商ID */
    BSP_U8      aucMufName[NAND_MFU_NAME_MAX_LEN];             /* 厂商名称字符串 */
    BSP_U32           DevId;                                         /* 设备ID */
    BSP_U8      aucDevSpec[NAND_DEV_SPEC_MAX_LEN];             /* 设备规格字符串 */
}NAND_DEV_INFO_S;

/******************************************************************************
*
  函数名:       BSP_S32 nand_get_dev_info (NAND_DEV_INFO_S *pNandDevInfo)
  函数描述:     查询NAND设备相关信息：包括厂商ID、厂商名称、设备ID、设备规格
  输入参数:     无
  输出参数:     pNandDevInfo    存储NAND设备相关信息的结构体
  返回值:       0    : 查询成功
                负数 : 查询失败
*******************************************************************************/
extern BSP_S32 nand_get_dev_info(NAND_DEV_INFO_S *pNandDevInfo);
#define NAND_GET_DEV_INFO(pNandDevInfo) nand_get_dev_info(pNandDevInfo)


/******************************************************************************
* Function     :   BSP_NANDF_Read
*
* Description  :   读取Nand Flash中指定地址和长度内容到指定空间中
*
* Input        :   u32FlashAddr  读取数据的源地址
*              :   u32NumByte    读取数据长度，单位为字节
*
* Output       :   pRamAddr      读取的数据存放的地址
*
* return       :   读操作成功与否
******************************************************************************/
BSP_S32 BSP_NANDF_Read(BSP_VOID* pRamAddr, BSP_U32 u32FlashAddr, BSP_U32 u32NumByte);

/******************************************************************************
* Function     :   BSP_NANDF_Write
*
* Description  :   将指定地址和长度内容到写到指定FLASH地址中
*
* Input        :   pRamAddr     写操作源地址
*              :   u32NumByte   数据长度，单位为字节
*
* Output       :   u32FlashAddr 写操作目的地址
*
* return       :   写操作成功与否
******************************************************************************/
BSP_S32 BSP_NANDF_Write(BSP_U32 u32FlashAddr, BSP_VOID *pRamAddr,  BSP_U32 u32NumByte);

/******************************************************************************
* Function     :   BSP_NANDF_Erase
*
* Description  :   擦除指定Flash地址所在块
*
* Input        :   u32address   要擦除块的ID
*
* Output       :   无
*
* return       :   擦除操作成功与否
******************************************************************************/
BSP_S32 BSP_NANDF_Erase(BSP_U32 u32BlockID);

BSP_U32 nand_isbad(BSP_U32 blockID, BSP_U32 *flag);

/********************************************************************************************************
 函 数 名  : USB_otg_switch_signal_set
 功能描述  : 用于BALONG和K3对接时USB通道切换GPIO引脚控制
 输入参数  : 组号、引脚号和值
 输出参数  : 无

 返 回 值  : 0:  操作成功；
             -1：操作失败。
********************************************************************************************************/
extern int USB_otg_switch_signal_set(UINT8 ucGroup, UINT8 ucPin, UINT8 ucValue);
#define DRV_GPIO_SET(group, pin, value) USB_otg_switch_signal_set(group, pin, value)

#define DRV_GPIO_USB_SWITCH     200
#define DRV_GPIO_HIGH           1
#define DRV_GPIO_LOW            0

/********************************************************************************************************
 函 数 名  : USB_otg_switch_set
 功能描述  : 用于BALONG和K3对接时MODEM侧开启或关闭USB PHY
 输入参数  : 开启或关闭
 输出参数  : 无

 返 回 值  : 0:  操作成功；
             -1：操作失败。
********************************************************************************************************/
extern int USB_otg_switch_set(UINT8 ucValue);
#define DRV_USB_PHY_SWITCH_SET(value) USB_otg_switch_set(value)

#define AT_USB_SWITCH_SET_VBUS_VALID            1
#define AT_USB_SWITCH_SET_VBUS_INVALID          2

#define USB_SWITCH_ON       1
#define USB_SWITCH_OFF      0

/********************************************************************************************************
 函 数 名  : USB_otg_switch_get
 功能描述  : 用于BALONG和K3对接时MODEM侧USB PHY状态查询
 输入参数  : 无
 输出参数  : 返回USB PHY开启状态

 返 回 值  : 0:  操作成功；
             -1：操作失败。
********************************************************************************************************/
extern int USB_otg_switch_get(UINT8 *pucValue);
#define DRV_USB_PHY_SWITCH_GET(value) USB_otg_switch_get(value)

/*************************OAM组件新增接口 END*************************/

/************************ Build优化 END*******************************/

enum GPIO_OPRT_ENUM
{
    GPIO_OPRT_SET = 0,
    GPIO_OPRT_GET,
    GPIO_OPRT_BUTT
};


/* IOCTL CMD 定义 */
#define ACM_IOCTL_SET_WRITE_CB      0x7F001000
#define ACM_IOCTL_SET_READ_CB       0x7F001001
#define ACM_IOCTL_SET_EVT_CB        0x7F001002
#define ACM_IOCTL_SET_FREE_CB       0x7F001003

#define ACM_IOCTL_WRITE_ASYNC       0x7F001010
#define ACM_IOCTL_GET_RD_BUFF       0x7F001011
#define ACM_IOCTL_RETURN_BUFF       0x7F001012
#define ACM_IOCTL_RELLOC_READ_BUFF  0x7F001013
#define ACM_IOCTL_SEND_BUFF_CAN_DMA 0x7F001014

#define ACM_IOCTL_IS_IMPORT_DONE    0x7F001020
#define ACM_IOCTL_WRITE_DO_COPY     0x7F001021

/* Modem 控制命令码 */
#define ACM_MODEM_IOCTL_SET_MSC_READ_CB 0x7F001030
#define ACM_MODEM_IOCTL_MSC_WRITE_CMD   0x7F001031
#define ACM_MODEM_IOCTL_SET_REL_IND_CB  0x7F001032
#define ACM_IOCTL_FLOW_CONTROL  		0x7F001035

/* UDI IOCTL 命令ID */
#define UDI_ACM_IOCTL_SET_READ_CB           ACM_IOCTL_SET_READ_CB
#define UDI_ACM_IOCTL_GET_READ_BUFFER_CB    ACM_IOCTL_GET_RD_BUFF
#define UDI_ACM_IOCTL_RETUR_BUFFER_CB       ACM_IOCTL_RETURN_BUFF

#define UART_IOCTL_SET_WRITE_CB      0x7F001000
#define UART_IOCTL_SET_READ_CB       0x7F001001
#define UART_IOCTL_SET_EVT_CB        0x7F001002
#define UART_IOCTL_SET_FREE_CB       0x7F001003
#define UART_IOCTL_WRITE_ASYNC       0x7F001010
#define UART_IOCTL_GET_RD_BUFF       0x7F001011
#define UART_IOCTL_RETURN_BUFF       0x7F001012
#define UART_IOCTL_RELLOC_READ_BUFF  0x7F001013
#define UART_IOCTL_SEND_BUFF_CAN_DMA 0x7F001014
#define UART_IOCTL_SET_WRITE_TIMEOUT 0x7F001015
#define UART_IOCTL_IS_IMPORT_DONE    0x7F001020
#define UDI_UART_IOCTL_SET_READ_CB           UART_IOCTL_SET_READ_CB
#define UDI_UART_IOCTL_GET_READ_BUFFER_CB    UART_IOCTL_GET_RD_BUFF
#define UDI_UART_IOCTL_RETUR_BUFFER_CB       UART_IOCTL_RETURN_BUFF

/* 异步数据收发结构 */
typedef struct tagACM_WR_ASYNC_INFO
{
    char* pBuffer;
    unsigned int u32Size;
    void* pDrvPriv;
}ACM_WR_ASYNC_INFO;

/* ACM设备事件类型 */
typedef enum tagACM_EVT_E
{
    ACM_EVT_DEV_SUSPEND = 0,        /* 设备不可以进行读写(主要用于事件回调函数的状态) */
    ACM_EVT_DEV_READY = 1,          /* 设备可以进行读写(主要用于事件回调函数的状态) */
    ACM_EVT_DEV_BOTTOM
}ACM_EVT_E;

typedef enum tagACM_IOCTL_FLOW_CONTROL_E
{
    ACM_IOCTL_FLOW_CONTROL_DISABLE = 0,      /* resume receiving data from ACM port */
    ACM_IOCTL_FLOW_CONTROL_ENABLE      /* stop receiving data from ACM port */
}ACM_IOCTL_FLOW_CONTROL_E;

/* 读buffer信息 */
typedef struct tagACM_READ_BUFF_INFO
{
    unsigned int u32BuffSize;
    unsigned int u32BuffNum;
}ACM_READ_BUFF_INFO;

/*************************NCM START***********************************/

typedef struct tagNCM_PKT_S
{
    BSP_U8 *pBuffer;       /* buffer指针*/
    BSP_U32  u32BufLen;      /* buffer长度 */
}NCM_PKT_S;

/* NCM设备类型枚举*/
typedef enum tagNCM_DEV_TYPE_E
{
    NCM_DEV_DATA_TYPE,      /* 数据通道类型，PS使用*/
    NCM_DEV_CTRL_TYPE       /* 控制通道，MSP传输AT命令使用*/
}NCM_DEV_TYPE_E;

/* 上行线路收包函数指针 */
typedef BSP_VOID (*USBUpLinkRxFunc)(UDI_HANDLE handle, BSP_VOID * pPktNode);

/* 包封装释放函数指针 */
typedef BSP_VOID (*USBFreePktEncap)(BSP_VOID *PktEncap);

/* USB IOCTL枚举 */
typedef enum tagNCM_IOCTL_CMD_TYPE_E
{
    NCM_IOCTL_NETWORK_CONNECTION_NOTIF,      /* 0x0,NCM网络是否连接上*/
    NCM_IOCTL_CONNECTION_SPEED_CHANGE_NOTIF, /* 0x1,NCM设备协商的网卡速度*/
    NCM_IOCTL_SET_PKT_ENCAP_INFO,            /* 0x2,设置包封装格式*/
    NCM_IOCTL_REG_UPLINK_RX_FUNC,            /* 0x3,注册上行收包回调函数*/
    NCM_IOCTL_REG_FREE_PKT_FUNC,             /* 0x4,注册释放包封装回调函数*/
    NCM_IOCTL_FREE_BUFF,                     /* 0x5,释放底软buffer*/
    NCM_IOCTL_GET_USED_MAX_BUFF_NUM,         /* 0x6,获取上层可以最多占用的ncm buffer个数*/
    NCM_IOCTL_GET_DEFAULT_TX_MIN_NUM,        /* 0x7,获取默认发包个数阈值，超过该阈值会启动一次NCM传输*/
    NCM_IOCTL_GET_DEFAULT_TX_TIMEOUT,        /* 0x8,获取默认发包超时时间，超过该时间会启动一次NCM传输*/
    NCM_IOCTL_GET_DEFAULT_TX_MAX_SIZE,       /* 0x9,获取默认发包字节阈值，超过该阈值会启动一次NCM传输*/
    NCM_IOCTL_SET_TX_MIN_NUM,                /* 0xa,设置发包个数阈值，超过该阈值会启动一次NCM传输*/
    NCM_IOCTL_SET_TX_TIMEOUT,                /* 0xb,设置发包超时时间，超过该时间会启动一次NCM传输*/
    NCM_IOCTL_SET_TX_MAX_SIZE,               /* 0xc,该命令字不再使用。设置发包字节阈值，超过该阈值会启动一次NCM传输*/
    NCM_IOCTL_GET_RX_BUF_SIZE,               /* 0xd,获取收包buffer大小*/
    NCM_IOCTL_FLOW_CTRL_NOTIF,               /* 0xe,流控控制开关*/
    NCM_IOCTL_REG_AT_PROCESS_FUNC,           /* 0xf,注册AT命令处理回调函数*/
    NCM_IOCTL_AT_RESPONSE,                   /* 0x10,AT命令回应*/
    NCM_IOCTL_REG_CONNECT_STUS_CHG_FUNC,     /* 0x11,注册网卡状态改变通知回调函数*/
    NCM_IOCTL_SET_PKT_STATICS,               /* 0x12,配置统计信息*/
       /*【BSP 新需求 USB NCM】PS需要底软提供NCM流控状态查询接口*/
    NCM_IOCTL_GET_FLOWCTRL_STATUS,           /* 0x13 查询NCM流控状态*/
    /* END:   Modified by liumengcun, 2011-4-21 */

       /*PS和MSP新需求，提供查询当前发包门限个数接口和NCM 挂起时回调函数增加NCM 设备ID参数*/
    NCM_IOCTL_GET_CUR_TX_MIN_NUM,              /* 0x14 获取当前发包个数阈值*/
    NCM_IOCTL_GET_CUR_TX_TIMEOUT,               /* 0x15 获取当前发包超时时间*/
    NCM_IOCTL_IPV6_DNS_NOTIF,              /*0x16 IPV6 DNS主动上报*/
    /* END:   Modified by liumengcun, 2011-6-23 */
    /* BEGIN: Modified by liumengcun, 2011-7-20 支持IPV6 DNS配置*/
    NCM_IOCTL_SET_IPV6_DNS,                     /* 0x16 配置IPV6 DNS*/
    /* END:   Modified by liumengcun, 2011-7-20 */
    /* BEGIN: Modified by liumengcun, 2011-8-10 MSP新需求*/
    NCM_IOCTL_CLEAR_IPV6_DNS,                     /* 0x17 清除IPV6 DNS在板端的缓存,param在此命令字没有意义，不填空指针即可*/
    NCM_IOCTL_GET_NCM_STATUS,                     /* 0x18 获取NCM网卡状态 enable:TRUE(1);disable:FALSE(0) */
    /* END:   Modified by liumengcun, 2011-8-10 */

    NCM_IOCTL_SET_ACCUMULATION_TIME,

    /* BEGIN: Modified by baoxianchun, 2012-5-17 GU PS 新需求*/
	NCM_IOCTL_SET_RX_MIN_NUM,		/*配置收包个数阈值*/
	NCM_IOCTL_SET_RX_TIMEOUT,			/*配置收包超时时间*/
    /* END: Modified by baoxianchun, 2012-5-17 GU PS 新需求*/

    NCM_IOCTL_REG_NDIS_RESP_STATUS_FUNC,   /* NDIS通道AT命令状态处理回调函数 */
}NCM_IOCTL_CMD_TYPE_E;

/* NCM连接状态枚举,NCM_IOCTL_NETWORK_CONNECTION_NOTIF命令字对应参数枚举*/
typedef enum tagNCM_IOCTL_CONNECTION_STATUS_E
{
    NCM_IOCTL_CONNECTION_LINKDOWN,      /* NCM网络断开连接*/
    NCM_IOCTL_CONNECTION_LINKUP         /* NCM网络连接*/
}NCM_IOCTL_CONNECTION_STATUS_E;

/* NCM连接速度结构,NCM_IOCTL_CONNECTION_SPEED_CHANGE_NOTIF命令字对应参数结构体*/
typedef struct tagNCM_IOCTL_CONNECTION_SPEED_S
{
    BSP_U32 u32DownBitRate;
    BSP_U32 u32UpBitRate;
}NCM_IOCTL_CONNECTION_SPEED_S;

/* 包封装结构体,NCM_IOCTL_SET_PKT_ENCAP_INFO命令字对应参数结构体*/
typedef struct tagNCM_PKT_ENCAP_INFO_S
{
    BSP_S32 s32BufOft;      /* buf偏移量 */
    BSP_S32 s32LenOft;      /* len偏移量 */
    BSP_S32 s32NextOft;     /* next偏移量 */
}NCM_PKT_ENCAP_INFO_S;

/* AT命令回复数据指针及长度 NCM_IOCTL_AT_RESPONSE*/
typedef struct tagNCM_AT_RSP_S
{
    BSP_U8* pu8AtAnswer;
    BSP_U32 u32Length;
} NCM_AT_RSP_S;
typedef struct tagNCM_IPV6_DNS_NTF_S
{
    BSP_U8* pu8Ipv6DnsNtf;
    BSP_U32 u32Length;
} NCM_AT_IPV6_DNS_NTF_S;

/* AT命令接收函数指针，该函数为同步接口，对应NCM_IOCTL_REG_AT_PROCESS_FUNC命令字*/
typedef BSP_VOID (*USBNdisAtRecvFunc)(BSP_U8 * pu8Buf, BSP_U32 u32Len);

/* NCM流控开关枚举,NCM_IOCTL_NETWORK_CONNECTION_NOTIF命令字对应参数枚举*/
typedef enum tagNCM_IOCTL_FLOW_CTRL_E
{
    NCM_IOCTL_FLOW_CTRL_ENABLE,      /* 打开流控*/
    NCM_IOCTL_FLOW_CTRL_DISABLE      /* 关闭流控*/
}NCM_IOCTL_FLOW_CTRL_E;

/* NCM网卡状态改变通知枚举,NCM_IOCTL_REG_CONNECT_STUS_CHG_FUNC命令字对应参数枚举*/
typedef enum tagNCM_IOCTL_CONNECT_STUS_E
{
    NCM_IOCTL_STUS_CONNECT,      /* 建链*/
    NCM_IOCTL_STUS_BREAK         /* 网卡断开,断链*/
}NCM_IOCTL_CONNECT_STUS_E;

/* 网卡状态切换通知函数，对应NCM_IOCTL_REG_CONNECT_STUS_CHG_FUNC命令字*/
typedef BSP_VOID (*USBNdisStusChgFunc)(NCM_IOCTL_CONNECT_STUS_E enStatus, BSP_VOID * pBuffer);

/* NDIS通道AT命令状态处理回调函数 */
typedef BSP_VOID (*USBNdisRespStatusCB)(BSP_VOID *pBuffer, BSP_U32 status);

/*  下传上传、下载包的各种统计信息，
    不发送给PC，被动等待PC的读取,NCM_IOCTL_SET_PKT_STATICS命令字对应参数结构*/
typedef struct tagNCM_IOCTL_PKT_STATISTICS_S
{
    BSP_U32     u32TxOKCount;         /*发送包数*/
    BSP_U32     u32RxOKCount;         /*接收包数*/
    BSP_U32     u32TxErrCount;        /*发送错误*/
    BSP_U32     u32RxErrCount;        /*接收错误*/
    BSP_U32     u32TxOverFlowCount;   /*发送溢出丢包*/
    BSP_U32     u32RxOverFlowCount;   /*接收溢出丢包*/
    BSP_U32     u32CurrentTx;         /*发送速率*/
    BSP_U32     u32CurrentRx;         /*接收速率*/
} NCM_IOCTL_PKT_STATISTICS_S;

/* BEGIN: Modified by liumengcun, 2011-7-20 IPV6 DNS配置结构,NCM_IOCTL_SET_IPV6_DNS对应参数结构*/
#define BSP_NCM_IPV6_DNS_LEN     32
 typedef struct tagNCM_IPV6DNS_S  /* 0x16 配置IPV6 DNS*/
 {
     BSP_U8 * pu8Ipv6DnsInfo;/* 32字节，低16字节表示primaryDNS；高16字节表示SecondaryDNS。*/
     BSP_U32 u32Length;
 } NCM_IPV6DNS_S;
/* END:   Modified by liumengcun, 2011-7-20 */
typedef struct tagNCM_PKT_INFO_S
{
    BSP_U32 u32PsRcvPktNum;              /* 收包送到PS的包个数*/
    BSP_U32 u32RcvUnusedNum;             /* 收包不符合PS要求丢弃包个数*/
    BSP_U32 u32NcmSendPktNum;            /* 发包个数*/
}NCM_PKT_INFO_S;


/*************************TIMER BEGIN*****************************/

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_Connect
*
* 功能描述  : This routine specifies the interrupt service routine to be called
*             at each clock interrupt.  It does not enable usr clock interrupts.
*
* 输入参数  : FUNCPTR routine   routine to be called at each clock interrupt
              BSP_S32 arg           argument with which to call routine
              BSP_S32 s32UsrClkid      which user clock the interrup routine belongs to
* 输出参数  : 无
* 返 回 值  : OK, or ERROR if the routine cannot be connected to the interrupt.
*
* 修改记录  : 2009年1月20日   liumengcun  creat
*****************************************************************************/
extern BSP_S32  BSP_USRCLK_Connect(FUNCPTR routine, BSP_S32 arg, BSP_S32 s32UsrClkid);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_Disable
*
* 功能描述  : This routine disables user clock interrupts.
*
* 输入参数  : BSP_S32 s32UsrClkid  which user clock the interrup routine belongs to
* 输出参数  : 无
* 返 回 值  : OK, or ERROR
*
* 修改记录  : 2009年1月20日   liumengcun  creat
*****************************************************************************/
extern BSP_S32   BSP_USRCLK_Disable (BSP_S32 s32UsrClkid);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_Enable
*
* 功能描述  : This routine enables user clock interrupts.
*
* 输入参数  : BSP_S32 s32UsrClkid  which user clock the interrup routine belongs to
* 输出参数  : 无
* 返 回 值  : OK, or ERROR
*
* 修改记录  : 2009年1月20日   liumengcun  creat
*****************************************************************************/
extern BSP_S32    BSP_USRCLK_Enable (BSP_S32 s32UsrClkid);

/*****************************************************************************
* 函 数 名  : BSP_USRCLK_RateSet
*
* 功能描述  : This routine sets the interrupt rate of the usr clock.
*
* 输入参数  : BSP_S32 ticksPerSecond   number of clock interrupts per second
              BSP_S32 s32UsrClkid         which user clock the interrup routine belongs to
* 输出参数  : 无
* 返 回 值  : OK, or ERROR if the tick rate is invalid or the timer cannot be set.
*
* 修改记录  : 2009年1月20日   liumengcun  creat
*****************************************************************************/
extern BSP_S32  BSP_USRCLK_RateSet(BSP_S32 ticksPerSecond, BSP_S32 s32UsrClkid);

/*****************************************************************************
 函 数 名  : DRV_AXI_VIRT_TO_PHY
 功能描述  : AXI内虚地址往实地址转换
 输入参数  : ulVAddr；虚地址
 输出参数  : 无
 返回值    ：虚地址
*****************************************************************************/
extern unsigned int DRV_AXI_VIRT_TO_PHY(unsigned int ulVAddr);

/*****************************************************************************
 函 数 名  : DRV_AXI_PHY_TO_VIRT
 功能描述  : AXI内实地址往虚地址转换
 输入参数  : ulVAddr；实地址
 输出参数  : 无
 返回值    ：虚地址
*****************************************************************************/
extern unsigned int DRV_AXI_PHY_TO_VIRT(unsigned int ulPAddr);

/*****************************************************************************
* 函 数 名  : BSP_IPM_FreeBspBuf
*
* 功能描述  : 释放内存接口
*
* 输入参数  : BSP_U8 *pBuf 需要释放的指针
*
* 输出参数  : 无
* 返 回 值  : 无
*
* 修改记录  :2011年1月27日   鲁婷  创建
*****************************************************************************/
BSP_VOID BSP_IPM_FreeBspBuf(BSP_U8 *pBuf);

/*for create_crypto_key,hash algorithm enum*/
typedef enum 
{
    CREATE_CRYPTO_KEY_ALGORITHM_MD5 = 0x0,
    CREATE_CRYPTO_KEY_ALGORITHM_SHA1,
    CREATE_CRYPTO_KEY_ALGORITHM_MAX
}CREATE_CRYPTO_KEY_ALGORITHM;

/*for crypto_hash,hash algorithm enum*/
typedef enum 
{
    CRYPTO_ALGORITHM_MD5 = 0x0,
    CRYPTO_ALGORITHM_SHA1,
    CRYPTO_ALGORITHM_MAX
}CRYPTO_HASH_ALGORITHM;

/*for crypto_encrypt,aes algorithm enum*/
typedef enum 
{
    CRYPTO_ENCRYPT_ALGORITHM_AES_ECB = 0x0,
    CRYPTO_ENCRYPT_ALGORITHM_MAX
}CRYPTO_ENCRYPT_ALGORITHM;



/*****************************************************************************
* 函 数 名  : create_crypto_key
*
* 功能描述  : 使用输入的数据和HUK，生成密钥。
*当前支持MD5、和SHA-1算法。生成密钥的方法：把HUK和输入
*的数据连接起来作为MD5或SHA-1算法的输入，计算其HASH值
*
* 输入参数  : data：输入参数。存放用于生成密钥的数据。
*                           len：输入参数。存放输入数据长度(字节)
*                           algorithm：输入参数。用于产生密钥的算法。
*                           key：输出参数。存放生成的密钥。
*            注意：其长度必须不小于16字节。因为密钥为16字节。
*                           klen：输入输出参数。作为输入参数，存放key的
*            缓冲区的长度。作为输出参数，存放生成的密钥的长度。(字节)
* 输出参数  : 
*
* 返 回 值  :  BSP_OK--加密成功;BSP_ERROR--加密失败
*
* 其它说明  :内存由调用者申请
*
*****************************************************************************/
int create_crypto_key(char *data, int len, CREATE_CRYPTO_KEY_ALGORITHM algorithm, char *key, int *klen);
#define CREATE_CRYPTO_KEY(data,len,algorithm,key,klen)  create_crypto_key(data,len,algorithm,key,klen)

/*****************************************************************************
* 函 数 名  : crypto_hash
*
* 功能描述  : 计算输入的数据的HASH值。
*                           当前支持MD5、和SHA-1算法。输出HASH值长度是16字节
*
* 输入参数  : data：输入参数。存放用于需要计算HASH值的数据。
*                           len：输入参数。存放输入数据长度(字节)
*                           algorithm：输入参数。HASH算法。
*                           hash：输出参数。存放生成的HASH值。
*                           hlen：输入输出参数。作为输入参数，存放HASH值的缓冲区的长度。(字节)
*                           作为输出参数，存放生成的HASH值的长度。
* 输出参数  : 
*
* 返 回 值  : BSP_OK--加密成功;BSP_ERROR--加密失败
*
* 其它说明  : 内存由调用者申请
*
*****************************************************************************/
int crypto_hash(char *data, int len, CRYPTO_HASH_ALGORITHM algorithm, char *hash, int *hlen);
#define CRYPTO_HASH(data,len,algorithm,hash,hlen)  crypto_hash(data,len,algorithm,hash,hlen)

/*****************************************************************************
* 函 数 名  : crypto_encrypt
*
* 功能描述  : 使用指定的密钥和指定的算法对输入的数据加密，输出加密后的数据。
                             当前支持AES-ECB算法。
*
* 输入参数  : data：输入参数。存放需要加密的数据。
*                           len：输入参数。存放输入数据长度(字节)
*                           algorithm：输入参数。HASH算法。
*                           key：输入参数。存放加密密钥。
*                           klen：输入参数。key的长度。(字节)
*                           cipher_data：输出参数。存放加密后的数据。
*                           cipher_len：输入输出参数。作为输入参数，存放密文的缓冲区的长度。(字节)
*               作为输出参数，存放生成的密文的长度。
* 输出参数  : 
*
* 返 回 值  :  BSP_OK--加密成功;BSP_ERROR--加密失败
*
* 其它说明  : 内存由调用者申请
*
*****************************************************************************/
int crypto_encrypt (char *data, int len, CRYPTO_ENCRYPT_ALGORITHM algorithm, char *key, int klen, char *cipher_data, int *cipher_len);
#define CRYPTO_ENCRYPT(data,len,algorithm,key,klen,cipher_data,cipher_len)  \
crypto_encrypt(data,len,algorithm,key,klen,cipher_data,cipher_len)

/*****************************************************************************
* 函 数 名  : crypto_decrypt
*
* 功能描述  : 使用指定的密钥和指定的算法对输入的数据解密，输出解密后的数据。
*             当前支持AES-ECB算法。
*
* 输入参数  : 
*             cipher_data: 待密的数据的存放buffer。
*             cipher_len:  待解密的数据的实际长度。(byte)
*             algorithm:   所用解密算法，暂只提供AES-ECB。
*             key:         密钥buffer。
*             klen:        密钥buffer长度。(byte)
*             len:  解密后的数据的存放buffer的buffer size。(byte)(没有检查)
*
* 输出参数  : 
*             data:        解密后的数据。
*             len:         解密后的数据长度。(byte)
*
* 返 回 值  : BSP_OK:      解密成功。
*             BSP_ERROR:   解密失败。
*
* 其它说明  : len为输入/输出参数，传入的len变量所用内存必须可写回。
*             所以避免直接传入类似sizeof()的函数调用结果。
*
*****************************************************************************/
extern int crypto_decrypt (char *cipher_data,int cipher_len,CRYPTO_ENCRYPT_ALGORITHM algorithm, char *key, int klen, char *data, int *len);
#define CRYPTO_DECRYPT(cipher_data,cipher_len,algorithm, key, klen, data, len)  \
crypto_decrypt(cipher_data,cipher_len,algorithm, key, klen, data, len)


/*****************************************************************************
* 函 数 名  : crypto_rsa_encrypt
*
* 功能描述  : 使用保存在NV中的改制用RSA公钥（读取时需要同样进行签名验证）
*           对输入的数据加密，输出加密后的数据。
* 输入参数  : data：输入参数。存放需要加密的数据。
*                           len：输入参数。存放输入数据长度(字节)
*                           rsa_key:RSA公钥
*                           rsa_len:RSA公钥长度(字节)
*                           cipher_data：输出参数。存放加密后的数据。
*                           cipher_len：输入输出参数。作为输入参数，存放密文的缓冲区的长度。(字节)
*               作为输出参数，存放生成的密文的长度。
* 输出参数  : 
*
* 返 回 值  :  BSP_OK--加密成功;BSP_ERROR--加密失败
*
* 其它说明  : 内存由调用者申请
*
*****************************************************************************/
int crypto_rsa_encrypt (char *data, int len, char *rsa_key, int rsa_klen, char *cipher_data, int *cipher_len);
#define CRYPTO_RSA_ENCRYT(data,len,rsa_key,rsa_klen,cipher_data,cihper_len) \
crypto_rsa_encrypt(data,len,rsa_key,rsa_klen,cipher_data,cihper_len)

/*****************************************************************************
* 函 数 名  : crypto_rsa_decrypt
*
* 功能描述  : 使用保存在NV中的改制用RSA公钥，对输入的数据解密
*               输出解密后的数据。
* 输入参数  : cipher_data：输入参数。存放加密数据。
*                           cipher_len：输入参数。存放密文的缓冲区的长度。(字节)
*                           rsa_key:RSA公钥
*                           rsa_len:RSA公钥长度(字节)
*                           data：输出参数。存放需解密后的数据。
*                           len：输入输出参数。作为输入参数，存放解密后的缓冲区的长度(字节)
*               作为输出参数，存放生成的明文的长度
* 输出参数  : 
*
* 返 回 值  :  BSP_OK--解密成功;BSP_ERROR--解密失败
*
* 其它说明  : 内存由调用者申请
*
*****************************************************************************/
int crypto_rsa_decrypt (char *cipher_data, int cipher_len, char *rsa_key, int rsa_klen, char *data, int *len);
#define CRYPTO_RSA_DECRYPT(cipher_data,cihper_len,rsa_key,rsa_klen,data,len) \
crypto_rsa_decrypt(cipher_data,cihper_len,rsa_key,rsa_klen,data,len)

/*****************************************************************************
* 函 数 名  : crypto_rand
*
* 功能描述  : 随机数生成接口
* 输入参数  : rand_data:随机数存放buffer
*                           len:期望得到的随机数字节数
*
* 输出参数  : 
*
* 返 回 值  :  BSP_OK--获取随机数成功;BSP_ERROR--获取失败
*
* 其它说明  : 内存由调用者申请
*
*****************************************************************************/
int crypto_rand (char *rand_data, int len);
#define CRYPTO_RAND(rand_data,len)  crypto_rand(rand_data,len)

/*************************************************
 函 数 名       : efuseWriteHUK
 功能描述   : HUK写efuse接口
 输入参数   : pBuf:烧写内容;len:烧写长度(字节)
 输出参数   : 
 返 回 值      : OK/ERROR
 调用函数   :
 被调函数   :

 修改历史   :
    日    期       : 2012年3月27日
   修改内容 : 新生成函数

*************************************************/
int efuseWriteHUK(char *pBuf,unsigned int len);
#define EFUSE_WRITE_HUK(pBuf,len) efuseWriteHUK(pBuf,len)

/*************************************************
 函 数 名   : CheckHukIsValid
 功能描述   : 判断HUK是否有效，若为全0则无效，非全0则有效
 输入参数   : 无
 输出参数   :
 返 回 值   : BSP_FALSE:HUK无效；BSP_TRUE:HUK有效
 调用函数   :
 被调函数   :

 修改历史   :
   日    期 : 2012年3月27日
   修改内容 : 新生成函数

*************************************************/
int CheckHukIsValid(void);
/*****************************************************************************
 函 数 名  : DRV_SMS_AWAKE_OLED_ANTISLEEP
 功能描述  : A核收到短信唤醒OLED时反对系统休眠的API接口，供短信模块调用
 输入参数  : None
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
extern BSP_U32 DRV_SMS_AWAKE_OLED_ANTISLEEP(void);
#define DRV_CHECK_HUK_IS_VALID() CheckHukIsValid()

extern long bsp_sys_creat(const char  *pathname, int mode);
#define DRV_SYS_CREAT(pathname,mode) bsp_sys_creat(pathname,mode)

extern long bsp_sys_open(const char  *filename,int flags, int mode);
#define DRV_SYS_OPEN(filename,flags,mode) bsp_sys_open(filename,flags,mode)

extern long bsp_sys_close(unsigned int fd);
#define DRV_SYS_CLOSE(fd) bsp_sys_close(fd)

extern long bsp_sys_lseek(unsigned int fd, unsigned int offset,unsigned int origin);
#define DRV_SYS_LSEEK(fd,offset,origin) bsp_sys_lseek(fd,offset,origin)

extern long bsp_sys_read(unsigned int fd, char  *buf, unsigned int count);
#define DRV_SYS_READ(fd, buf, count) bsp_sys_read(fd, buf, count)

extern long bsp_sys_write(unsigned int fd, const char  *buf,unsigned int count);
#define DRV_SYS_WRITE(fd, buf, count) bsp_sys_write(fd, buf, count)

extern long bsp_sys_mkdir(const char  *pathname, int mode);
#define DRV_SYS_MKDIR(pathname, mode) bsp_sys_mkdir(pathname, mode)

extern long bsp_sys_rmdir(const char  *pathname);
#define DRV_SYS_RMDIR(pathname) bsp_sys_rmdir(pathname)

extern long bsp_sys_unlink(const char  *pathname);
#define DRV_SYS_UNLINK(pathname) bsp_sys_unlink(pathname)

extern long bsp_sys_getdents(unsigned int fd,
          void  *dirent,
          unsigned int count);
#define DRV_SYS_GETDENTS(fd,dirent,count) bsp_sys_getdents(fd,dirent,count)

extern long bsp_sys_sync(void);
#define DRV_SYS_SYNC() bsp_sys_sync()

extern long bsp_sys_sync_file(unsigned int fd);
#define DRV_SYS_SYNC_FILE(fd) bsp_sys_sync_file(fd)



#if (FEATURE_UE_MODE_TDS == FEATURE_ON)

typedef enum tagTDS_IPC_INT_CORE_E
{
    TDS_IPC_CORE_ARM_ZSP1 = 0x1,
    TDS_IPC_CORE_ARM_ZSP2 = 0x2,
    TDS_IPC_CORE_BUTTOM   
}TDS_IPC_INT_CORE_E;


/*****************************************************************************
 * 函 数 名  : BSP_TDS_GetDynTableAddr
 *
 * 功能描述  : TDS 获取动态加载表首地址
 *
 * 输入参数  : 无
 * 输出参数  : 无
 *
 * 返 回 值  : Addr:动态表首地址(DDR) / NULL: Fail
 *
 * 修改记录  :
 *****************************************************************************/
BSP_U32 BSP_TDS_GetDynTableAddr(BSP_VOID);

/*****************************************************************************
 * 函 数 名  : BSP_TDS_TF_IntConnect
 *
 * 功能描述  : TDS 帧中断挂接函数
 *
 * 输入参数  : routine:   挂接函数指针
 *             parameter: 参数
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 修改记录  :
 *****************************************************************************/
BSP_S32 BSP_TDS_TF_IntConnect(VOIDFUNCPTR routine, BSP_U32 parameter);

/*****************************************************************************
 * 函 数 名  : BSP_TDS_TF_IntEnable
 *
 * 功能描述  : TDS 帧中断使能
 *
 * 输入参数  : 无
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 修改记录  :
 *****************************************************************************/
BSP_S32 BSP_TDS_TF_IntEnable(BSP_VOID);

/*****************************************************************************
 * 函 数 名  : BSP_TDS_TF_IntDisable
 *
 * 功能描述  : TDS 帧中断去使能
 *
 * 输入参数  : 无
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 修改记录  :
 *****************************************************************************/
BSP_S32 BSP_TDS_TF_IntDisable(BSP_VOID);

/*****************************************************************************
 * 函 数 名  : BSP_RunTdsDsp
 *
 * 功能描述  : 加载 ZSP (当前读取的路径为: "/yaffs0/tds_zsp.bin")
 *
 * 输入参数  : 无
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 修改记录  :
 *****************************************************************************/
BSP_VOID ZSP_Init(BSP_VOID);


/*****************************************************************************
 * 函 数 名  : BSP_RunTdsDsp
 *
 * 功能描述  : 加载 ZSP (当前读取的路径为: "/yaffs0/tds_zsp.bin")
 *
 * 输入参数  : 无
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 修改记录  :
 *****************************************************************************/
BSP_S32 BSP_RunTdsDsp(BSP_VOID);

/*****************************************************************************
 * 函 数 名  : BSP_TDS_IPC_IntConnect
 *
 * 功能描述  : 注册TDS MailBox中断
 *
 * 输入参数  :
               BSP_U32 ulLvl :     不需要使用(为保持接口形式一致)
               VOIDFUNCPTR routine 中断服务程序
 *             BSP_U32 parameter      中断服务程序参数
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 修改记录  :
 *****************************************************************************/
BSP_S32 BSP_TDS_IPC_IntConnect  (IPC_INT_LEV_E ulLvl,VOIDFUNCPTR routine, BSP_U32 parameter);

/*****************************************************************************
* 函 数 名  : BSP_IPC_IntSend
*
* 功能描述  : 发送中断
*
* 输入参数  :
                IPC_INT_CORE_E enDstore 不需要使用(为保持接口形式一致)
                BSP_U32 u32Msg 要发送的消息
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  :
*****************************************************************************/
BSP_S32 BSP_TDS_IPC_IntSend(TDS_IPC_INT_CORE_E enDstCore, BSP_U32 u32Msg);

/*****************************************************************************
* 函 数 名  : BSP_TDS_IPC_IntEnable
*
* 功能描述  : 使能MailBox中断
*
* 输入参数  :
                BSP_U32 ulLvl 不需要使用(为保持接口形式一致)
* 输出参数  : 无
*
* 返 回 值  : OK&ERROR
*
* 修改记录  :
*****************************************************************************/
BSP_S32 BSP_TDS_IPC_IntEnable(IPC_INT_LEV_E ulLvl);

/*****************************************************************************
 * 函 数 名  : BSP_TDS_IPC_IntDisable
 *
 * 功能描述  : 去使能MailBox中断
 *
 * 输入参数  :
                BSP_U32 ulLvl 不需要使用(为保持接口形式一致)
 * 输出参数  : 无
 *
 * 返 回 值  : OK&ERROR
 *
 * 修改记录  :
 *****************************************************************************/
BSP_S32 BSP_TDS_IPC_IntDisable (IPC_INT_LEV_E ulLvl);




/*************************TDS IPC END************************/
#endif 


/*****************************************************************************
* 函 数 名  : BSP_LDSP_EDMA_MemRestore
*
* 功能描述  : Restore LDSP mem
*
* 输入参数  : 
* 输出参数  : 
* 返 回 值   :
*
* 修改记录  : 

*****************************************************************************/
BSP_S32 BSP_LDSP_EDMA_MemRestore();


/*****************************************************************************
* 函 数 名  : BSP_LDSP_EDMA_MemStore
*
* 功能描述  : Store LDSP mem
*
* 输入参数  : 
* 输出参数  : 
* 返 回 值  :
*
* 修改记录  : 

*****************************************************************************/
BSP_S32 BSP_LDSP_EDMA_MemStore();

/*****************************************************************************
* 函 数 名  : BSP_LDSP_GetEdmaTaskState
*
* 功能描述  : 查询LDSP备份TCM所用的edma通道时否空闲
*
* 输入参数  : 
* 输出参数  : 
* 返 回 值  :
*
* 修改记录  : 

*****************************************************************************/
BSP_VOID BSP_LDSP_GetEdmaTaskState();

BSP_VOID BSP_GetEdmaTaskState();


/*****************************************************************************
* 函 数 名  : DRV_Get_DspPsAddr
* 功能描述  : 返回在AXI中为DSP_PS分配的4字节空间首地址
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : MEMORY_AXI_DSP_PS_ADDR
* 其它说明  : 无
*****************************************************************************/
BSP_U32 DRV_Get_DspPsAddr();


/*****************************************************************************
* 函 数 名  : DRV_Get_DspMspAddr
* 功能描述  : 返回在AXI中为DSP_MSP分配的4字节空间首地址
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : MEMORY_AXI_DSP_MSP_ADDR
* 其它说明  : 无
*****************************************************************************/
BSP_U32 DRV_Get_DspMspAddr();


/*******************************************************************************
  Function:     BSP_Modem_OS_Status_Swtich
  Description:  指示 Modem Ready 或者 非Ready状态

  Input:        int enable
                非0:设置Modem处于Ready状态 
                0:设置Modem处于非Ready状态

  Output:
  Return:       0:    操作成功
                -1:   操作失败
*******************************************************************************/
extern int BSP_Modem_OS_Status_Switch(int enable);

/*****************************************************************************
* 函 数 名  : CHG_CHIP_HARD_READ 
* 功能描述  : 获取单板充电芯片是否OK
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 0：正确，-1：不正确
* 修改记录  : 2013-3-30 wanghaijie
*****************************************************************************/
extern BSP_S32 CHG_CHIP_HARD_READ (void);

#define DRV_OS_STATUS_SWITCH(enable) BSP_Modem_OS_Status_Switch(enable)
#pragma pack(0)


#endif /* end of __DRV_INTERFACE_H__ */

