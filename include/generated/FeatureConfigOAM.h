/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : FeatureConfigOAM.h
  �� �� ��   :
  ��    ��   :
  ��������   :
  ����޸�   :
  ��������   :
  �����б�   :
  �޸���ʷ   :
  1.��    ��   :
    ��    ��   :
    �޸�����   :

******************************************************************************/
#ifndef __FEATURECONFIGOAM_H__
#define __FEATURECONFIGOAM_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "FeatureConfig.h"

/* Phone Book֧�ֱ������ڴ��� */
#define OAM_FEATURE_PB_STOREINMEM                           FEATURE_ON

#define OAM_FEATURE_HPA_MSGTRACK                            FEATURE_OFF

#define OAM_FEATURE_DPM_MSGTRACK                            FEATURE_OFF

#define OAM_FEATURE_SD_LOG                                  FEATURE_OFF

#define OAM_FEATURE_TEMP_PROTECT_QOS                        FEATURE_OFF

#define OAM_FEATURE_HIFI_SUPPORT                            FEATURE_OFF

/* SIM���¶ȱ��� */
#define FEATURE_TEMPPROTECT_SIM                             FEATURE_OFF

/* SIM���͵���¶ȱ��� */
#define FEATURE_TEMPPROTECT_BATTERY_SIM                     FEATURE_ON



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* __FEATURECONFIGOAM_H__ */
