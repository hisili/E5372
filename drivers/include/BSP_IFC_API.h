/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_IFC.h
*
*
*   描    述 :  BSP类型定义头文件
*
*************************************************************************/
/**********************问题单修改记录*****************************************************
日    期              修改人                     问题单号                   修改内容
***************************************************************************************/

#ifndef	__BSP_IFC_H__
#define __BSP_IFC_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* 错误码*/
/*专用错误码*/
#define BSP_ERR_IFC_SEND_FAIL         BSP_DEF_ERR(BSP_MODU_IFC,(BSP_ERR_SPECIAL + 1)) /*0x10120001*/
#define BSP_ERR_IFC_SEND_TIMEOUT      BSP_DEF_ERR(BSP_MODU_IFC,(BSP_ERR_SPECIAL + 2)) /*0x10120002*/
#define BSP_ERR_IFC_RCV_FAIL          BSP_DEF_ERR(BSP_MODU_IFC,(BSP_ERR_SPECIAL + 3)) /*0x10120003*/

#define IFC_ICC_CHAN_IN_FIFOSIZE  4096
#define IFC_ICC_CHAN_OUT_FIFOSIZE 4096
/*IFC发送最大长度*/
#define BSP_IFC_SEND_MAX_LEN  IFC_ICC_CHAN_IN_FIFOSIZE

BSP_S32 IFC_Init(void);
BSP_S32 BSP_IFC_CanSleep(void);


/* 模块ID枚举*/
typedef enum tagIFCP_FUNC_E
{
    IFCP_FUNC_NANDFINIT =  0,   /* NANDflash初始化同步*/
    IFCP_FUNC_SD_POWER_CTRL,    /* SD卡上下电管理*/
    IFCP_FUNC_DMACINIT,      /* DMAC初始化同步*/
    IFCP_FUNC_USB_PCSC,
    IFCP_FUNC_USB_DIAGMODE,
    IFCP_FUNC_USB_U2DIAG,
    IFCP_FUNC_SD_VOLT_AJUST,
	IFCP_FUNC_DFS_ACPULOAD = 7,
    IFCP_FUNC_DFS_SETPROFILE = 8,
    IFCP_FUNC_DFS_RELEASEPROFILE = 9,

    IFCP_FUNC_ONOFF_NOTIFY_APP      = 10,     /* notify Application power off event */
    IFCP_FUNC_ONOFF_SHUTDOWN        = 11,     /* A-CPU to shutdown the system */
    IFCP_FUNC_ONOFF_DRV_POWER_OFF   = 12,

    IFCP_FUNC_CHGC_CallUsb=13,/*UT打桩*/
    IFCP_FUNC_CHGC_ReportStaToAPP=14,/*UT打桩*/
    IFCP_FUNC_CHGC_GetUsbReply=15,/*UT打桩*/
    IFCP_FUNC_CHGC_InterfacePro=16,/*UT打桩*/
    IFCP_FUNC_CHGC_GetInterface=17,/*UT打桩*/
    IFCP_FUNC_CHGC_SendOledIoctrl=18,/*UT打桩*/
	IFCP_FUNC_LED_CTRL_SET = 19,		 /*LED_DRV added by j00179452*/
	IFCP_FUNC_SD_LDO_CTRL = 20,   /*SD卡电源开关及电压设置,y00186965*/
	IFCP_FUNC_PMU_ID = 21,
    IFCP_FUNC_PMU_CTRL = 22,
	IFCP_FUNC_RTC_RW_SET = 23,	 /*RTC双核通信*/
	IFCP_FUNC_RTC_READ_GET = 24, /*RTC双核通信*/
	IFCP_FUNC_NVIM_READ_SET = 25,
	IFCP_FUNC_NVIM_READ_GET = 26,
    IFCP_FUNC_CHGC_TBAT_SUPL = 27,
    IFCP_FUNC_SD_HOOK = 28,   /*SD卡钩子函数*/
    IFCP_FUNC_SDUP_INFO_SET = 29, /*SD卡升级版本信息获取*/
    IFCP_FUNC_SDUP_INFO_GET = 30, /*SD卡升级版本信息获取*/
    IFCP_FUNC_CHGC_USB_INOUT=33,/*USB 插入 拔出*/
    IFCP_FUNC_ONLINE_UP_INFO_SET =34,/*在线升级双核通讯*/
    IFCP_FUNC_ONLINE_UP_INFO_GET =35,/*在线升级双核通讯*/
    IFCP_FUNC_ERROR_LOG_SET=36, 
    IFCP_FUNC_BREATH_LED_CTRL = 37, /*呼吸灯双核通信*/
	IFCP_FUNC_SIM_DETECT = 38,/*SIM 插入 拔出*/
	IFCP_FUNC_ANTENNA_DETECT = 39,/*天线插拔*/
	IFCP_FUNC_ONLINE_UP_STUTASINFO_SET=40,/*在线升级状态信息设置双核通讯*/
	IFCP_FUNC_ONLINE_UP_STUTASINFO_GET=41,/*在线升级状态信息获取双核通讯*/
	IFCP_FUNC_MMI_USIM_STATUS_SET=42,/*MMI测试中USIM卡状态信息获取*/
	IFCP_FUNC_MMI_USIM_STATUS_GET=43,/*MMI测试中USIM卡状态信息获取*/
	IFCP_FUNC_RFILE_SEM = 44,
	IFCP_FUNC_RFILE_CP2AP_SEM = 45,
/*Added by l00212112, 20120607用于flashlsee远程文件系统可维可测,starts*/
	IFCP_FUNC_RFILE_LOG_SAVE = 46,
	IFCP_FUNC_RFILE_AP_NOTIFY_CP_INIT = 47,
/*ends*/
    IFCP_FUNC_MMI_ADC_STATUS_SET,  /* 48 MMI测试中前壳温度状态信息获取，迁移基线的时候尤其注意该值是否与海思基线值重叠*/
    IFCP_FUNC_MMI_ADC_STATUS_GET,  /* 49 MMI测试中前壳温度状态信息获取，迁移基线的时候尤其注意该值是否与海思基线值重叠*/
    IFCP_FUNC_CHGC_SendUsbExtChgIoctrl = 50,
    IFCP_FUNC_BUTT
} IFCP_FUNC_E;


/* 底软核间调用封装回调函数*/
/* pMsgBody:该函数ID对应函数的参数*/
/* u32Len:pMsgBody长度*/
typedef BSP_S32 (*BSP_IFCP_REG_FUNC)(BSP_VOID *pMsgBody,BSP_U32 u32Len);

BSP_U32 BSP_IFCP_RegFunc(IFCP_FUNC_E enFuncId, BSP_IFCP_REG_FUNC pFuncCb);
BSP_S32 IFC_Process(BSP_VOID *pMsgBody,BSP_U32 u32Len);
BSP_S32 IFCP_Init(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_IFC_H__ */


