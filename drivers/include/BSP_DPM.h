/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_DPM.h
*
*   ��    �� :
*
*   ��    �� :  DPMģ���û��ӿ��ļ�
*
*   �޸ļ�¼ :  2012��04��16��  v1.00  ����
*************************************************************************/

#ifndef __BSP_DPM_H__
#define __BSP_DPM_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/*****************************************************************************
 �� �� ��  : BSP_PWRCTRL_GetTemp
 ��������  : ��ȡOLED����ء�GPA��WPA��SIM����DCXO���¶ȡ�
 �������  : enModule:PWC_TEMP_PROTECT_E
             hkAdcTable:��ѹ�¶Ȼ����
 �������  : pslData:�洢�ĵ�ѹת��Ϊ���¶�ֵ
 �� �� ֵ  : HKADC_OK/HKADC_ERROR ��
*****************************************************************************/
extern BSP_S32 BSP_PWRCTRL_GetTemp(PWC_TEMP_PROTECT_E enModule, BSP_U16 *hkAdcTable,BSP_S32 *pslData);

/*****************************************************************************
 �� �� ��  : BSP_PWRCTRL_AhbBakAddrAlloc
 ��������  : arm���µ繦�ܣ���Ҫ����ahb���ݣ��ýӿ��������ݱ����ַ�ͳ���
 �������  : length     ��������ռ��С��ָ��
 �������  : ��
 �� �� ֵ  : ����ռ�ĵ�ַ ��
*****************************************************************************/
extern void * BSP_PWRCTRL_AhbBakAddrAlloc(unsigned int * length);

/*****************************************************************************
 �� �� ��  : BSP_PWRCTRL_GetCcpuLoadCB
 ��������  : arm �ṩ��TTF�Ļص�����
 �������  : pFunc:TTF����ָ��
 �������  : ��
 �� �� ֵ  : ����ռ�ĵ�ַ ��
*****************************************************************************/
extern void BSP_PWRCTRL_GetCcpuLoadCB(PWRCTRLFUNCPTR pFunc );

/*****************************************************************************
 �� �� ��  : BSP_PWRCTRL_GetCpuLoad
 ��������  : arm cpuռ���ʲ�ѯ����
 �������  : ��
 �������  : ulAcpuLoad:Acpuռ���ʵ�ַ.
             ulCcpuLoad:Ccpuռ���ʵ�ַ.
 �� �� ֵ  : 0/1 ��
*****************************************************************************/
extern unsigned int BSP_PWRCTRL_GetCpuLoad(unsigned int *ulAcpuLoad,unsigned int *ulCcpuLoad);

/*****************************************************************************
 �� �� ��  : BSP_PWRCTRL_PwrUp
 ��������  : Wģ��Gģ��Lģ��PA��RF��BBP��DSP��ABB���ϵ���ơ�
 �������  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_RF/PWC_COMM_MODULE_BBP/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 �������  : None
 �� �� ֵ  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
extern BSP_U32 BSP_PWRCTRL_PwrUp (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule);

/*****************************************************************************
 �� �� ��  : BSP_PWRCTRL_PwrDown
 ��������  : Wģ��Gģ��Lģ��PA��RF��BBP��DSP��ABB���µ���ơ�
 �������  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_RF/PWC_COMM_MODULE_BBP/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 �������  : None
 �� �� ֵ  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
extern BSP_U32 BSP_PWRCTRL_PwrDown (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule);

/*****************************************************************************
 �� �� ��  : BSP_PWRCTRL_PllEnable
 ��������  : Wģ��Gģ��Lģ��PA��DSP��ABB��PLLʹ�ܡ�
 �������  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 �������  : None
 �� �� ֵ  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
extern BSP_U32 BSP_PWRCTRL_PllEnable (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule);

/*****************************************************************************
 �� �� ��  : BSP_PWRCTRL_PllDisable
 ��������  : Wģ��Gģ��Lģ��PA��DSP��ABB��PLLȥʹ�ܡ�
 �������  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 �������  : None
 �� �� ֵ  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
extern BSP_U32 BSP_PWRCTRL_PllDisable (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule);

/*****************************************************************************
 �� �� ��  : BSP_PWRCTRL_SleepVoteLock
 ��������  : �����ֹ˯��ͶƱ�ӿڡ�
 �������  : enClientId:PWC_CLIENT_ID_E
 �������  : None
 �� �� ֵ  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
extern BSP_U32 BSP_PWRCTRL_SleepVoteLock(PWC_CLIENT_ID_E enClientId);

/*****************************************************************************
 �� �� ��  : BSP_PWRCTRL_SleepVoteUnLock
 ��������  : ��������˯��ͶƱ�ӿڡ�
 �������  : enClientId:PWC_CLIENT_ID_E
 �������  : None
 �� �� ֵ  : PWC_PARA_INVALID/PWC_SUCCESS
*****************************************************************************/
extern BSP_U32 BSP_PWRCTRL_SleepVoteUnLock(PWC_CLIENT_ID_E enClientId);

/*****************************************************************************
 �� �� ��  : DRV_PWRCTRL_DSP_RESET
 ��������  : Lģ��DSP��λ�ӿڡ�
 �������  : 
 �������  : None
 �� �� ֵ  : 
*****************************************************************************/
extern BSP_VOID BSP_PWRCTRL_LDspReset();

#if (FEATURE_UE_MODE_TDS == FEATURE_ON)
/*TDS DRX MOD BEGIN*/
/*****************************************************************************
 �� �� ��  : DRV_PWRCTRL_TDSDSP_RESET
 ��������  : TDSģ��DSP��λ�ӿڡ�
 �������  : 
 �������  : None
 �� �� ֵ  : 
*****************************************************************************/
BSP_VOID DRV_PWRCTRL_TDSDSP_RESET();
/*TDS DRX MOD END*/
#endif

/*****************************************************************************
 �� �� ��  : BSP_PWRCTRL_PllStatusGet
 ��������  : Wģ��Gģ��Lģ��PA��DSP��ABB��PLL״̬��ѯ��
 �������  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 �������  : None
 �� �� ֵ  : PWRCTRL_GET_SUCCESS/PWRCTRL_GET_PARAINVALID/PWRCTRL_ONORLOCK/PWRCTRL_OFFORUNLOCK
*****************************************************************************/
//#define	DRV_PWRCTRL_PllStatusGet(enCommMode, enCommModule) DRV_OK


/*****************************************************************************
 �� �� ��  : BSP_PWRCTRL_PwrStatusGet
 ��������  : Wģ��Gģ��Lģ��PA��RF��BBP��DSP��ABB�����µ�״̬��ѯ��
 �������  : enCommMode: PWC_COMM_MODE_WCDMA/PWC_COMM_MODE_GSM/PWC_COMM_MODE_LTE,
             enCommModule:PWC_COMM_MODULE_PA/PWC_COMM_MODULE_RF/PWC_COMM_MODULE_BBP/PWC_COMM_MODULE_DSP/PWC_COMM_MODULE_ABB,
 �������  : None
 �� �� ֵ  : PWRCTRL_GET_SUCCESS/PWRCTRL_GET_PARAINVALID/PWRCTRL_ONORLOCK/PWRCTRL_OFFORUNLOCK
*****************************************************************************/
extern  BSP_U32 BSP_PWRCTRL_PwrStatusGet(PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule);

/*****************************************************************************
 �� �� ��  : getSystemResetInfo
 ��������  : ��ȡϵͳ��λ��Ϣ
 �������  : ��

 �������  : �ޡ�
 �� �� ֵ  : ��
 ע������  ��
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

