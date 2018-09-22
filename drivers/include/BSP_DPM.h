/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_DPM.h
*
*   作    者 :
*
*   描    述 :  DPM模块用户接口文件
*
*   修改记录 :  2012年04月16日  v1.00  创建
*************************************************************************/

#ifndef __BSP_DPM_H__
#define __BSP_DPM_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_GetTemp
 功能描述  : 获取OLED、电池、GPA、WPA、SIM卡、DCXO的温度。
 输入参数  : enModule:PWC_TEMP_PROTECT_E
             hkAdcTable:电压温度换算表
 输出参数  : pslData:存储的电压转换为的温度值
 返 回 值  : HKADC_OK/HKADC_ERROR 。
*****************************************************************************/
extern BSP_S32 BSP_PWRCTRL_GetTemp(PWC_TEMP_PROTECT_E enModule, BSP_U16 *hkAdcTable,BSP_S32 *pslData);

/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_AhbBakAddrAlloc
 功能描述  : arm上下电功能，需要备份ahb数据，该接口申请数据保存地址和长度
 输入参数  : length     保存申请空间大小的指针
 输出参数  : 无
 返 回 值  : 申请空间的地址 。
*****************************************************************************/
extern void * BSP_PWRCTRL_AhbBakAddrAlloc(unsigned int * length);

/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_GetCcpuLoadCB
 功能描述  : arm 提供给TTF的回调函数
 输入参数  : pFunc:TTF函数指针
 输出参数  : 无
 返 回 值  : 申请空间的地址 。
*****************************************************************************/
extern void BSP_PWRCTRL_GetCcpuLoadCB(PWRCTRLFUNCPTR pFunc );

/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_GetCpuLoad
 功能描述  : arm cpu占有率查询函数
 输入参数  : 无
 输出参数  : ulAcpuLoad:Acpu占有率地址.
             ulCcpuLoad:Ccpu占有率地址.
 返 回 值  : 0/1 。
*****************************************************************************/
extern unsigned int BSP_PWRCTRL_GetCpuLoad(unsigned int *ulAcpuLoad,unsigned int *ulCcpuLoad);

/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_PwrUp
 功能描述  : W模、G模、L模下PA、RF、BBP、DSP、ABB的上电控制。
 输入参数  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_RF/PWC_COMM_MODULE_BBP/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
extern BSP_U32 BSP_PWRCTRL_PwrUp (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule);

/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_PwrDown
 功能描述  : W模、G模、L模下PA、RF、BBP、DSP、ABB的下电控制。
 输入参数  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_RF/PWC_COMM_MODULE_BBP/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
extern BSP_U32 BSP_PWRCTRL_PwrDown (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule);

/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_PllEnable
 功能描述  : W模、G模、L模下PA、DSP、ABB的PLL使能。
 输入参数  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
extern BSP_U32 BSP_PWRCTRL_PllEnable (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule);

/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_PllDisable
 功能描述  : W模、G模、L模下PA、DSP、ABB的PLL去使能。
 输入参数  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
extern BSP_U32 BSP_PWRCTRL_PllDisable (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule);

/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_SleepVoteLock
 功能描述  : 外设禁止睡眠投票接口。
 输入参数  : enClientId:PWC_CLIENT_ID_E
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
extern BSP_U32 BSP_PWRCTRL_SleepVoteLock(PWC_CLIENT_ID_E enClientId);

/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_SleepVoteUnLock
 功能描述  : 外设允许睡眠投票接口。
 输入参数  : enClientId:PWC_CLIENT_ID_E
 输出参数  : None
 返 回 值  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
extern BSP_U32 BSP_PWRCTRL_SleepVoteUnLock(PWC_CLIENT_ID_E enClientId);

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_DSP_RESET
 功能描述  : L模下DSP复位接口。
 输入参数  : 
 输出参数  : None
 返 回 值  : 
*****************************************************************************/
extern BSP_VOID BSP_PWRCTRL_LDspReset();

#if (FEATURE_UE_MODE_TDS == FEATURE_ON)
/*TDS DRX MOD BEGIN*/
/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_TDSDSP_RESET
 功能描述  : TDS模下DSP复位接口。
 输入参数  : 
 输出参数  : None
 返 回 值  : 
*****************************************************************************/
BSP_VOID DRV_PWRCTRL_TDSDSP_RESET();
/*TDS DRX MOD END*/
#endif

/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_PllStatusGet
 功能描述  : W模、G模、L模下PA、DSP、ABB的PLL状态查询。
 输入参数  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 输出参数  : None
 返 回 值  : PWRCTRL_GET_SUCCESS/PWRCTRL_GET_PARAINVALID/PWRCTRL_ONORLOCK/PWRCTRL_OFFORUNLOCK
*****************************************************************************/
//#define	DRV_PWRCTRL_PllStatusGet(enCommMode, enCommModule) DRV_OK


/*****************************************************************************
 函 数 名  : BSP_PWRCTRL_PwrStatusGet
 功能描述  : W模、G模、L模下PA、RF、BBP、DSP、ABB的上下电状态查询。
 输入参数  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_RF/PWC_COMM_MODULE_BBP/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 输出参数  : None
 返 回 值  : PWRCTRL_GET_SUCCESS/PWRCTRL_GET_PARAINVALID/PWRCTRL_ONORLOCK/PWRCTRL_OFFORUNLOCK
*****************************************************************************/
extern  BSP_U32 BSP_PWRCTRL_PwrStatusGet(PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule);

/*****************************************************************************
 函 数 名  : getSystemResetInfo
 功能描述  : 获取系统复位信息
 输入参数  : 无

 输出参数  : 无。
 返 回 值  : 无
 注意事项  ：
*****************************************************************************/
#if defined (VERSION_V3R2)
//extern void * getSystemResetInfo(void);
//#define GET_SYSTEM_RESET_INFO() getSystemResetInfo()
#else
//#define GET_SYSTEM_RESET_INFO() DRV_OK
#endif


/*****************************************************************************
Function:   BSP_PWRCTRL_DfsSetProfileCcpu
Description:Set the System Min Profile
Input:      ulClientID: The Vote Module Client
            enProfile:  The Min Profile Value
Output:     None
Return:     The Vote Result
Others:
*****************************************************************************/
extern BSP_U32  BSP_PWRCTRL_DfsSetProfileCcpu(PWC_DFS_ID_E ulClientID, PWC_DFS_PROFILE_E enProfile);

/*****************************************************************************
Function:   BSP_PWRCTRL_DfsReleaseProfileCcpu
Description:Release the Vote Result
Input:      ulClientID: The Vote Module Client
Output:     None;
Return:     The Vote Result
Others:
*****************************************************************************/
extern BSP_U32 BSP_PWRCTRL_DfsReleaseProfileCcpu(PWC_DFS_ID_E ulClientID);



#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif /* __cplusplus */

#endif

