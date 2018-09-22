/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : FeatureConfigOAM.h
  版 本 号   :
  作    者   :
  生成日期   :
  最近修改   :
  功能描述   :
  函数列表   :
  修改历史   :
  1.日    期   :
    作    者   :
    修改内容   :

******************************************************************************/
#ifndef __FEATURECONFIGOAM_H__
#define __FEATURECONFIGOAM_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "FeatureConfig.h"

/* Phone Book支持保存在内存中 */
#define OAM_FEATURE_PB_STOREINMEM                           FEATURE_ON

#define OAM_FEATURE_HPA_MSGTRACK                            FEATURE_OFF

#define OAM_FEATURE_DPM_MSGTRACK                            FEATURE_OFF

#define OAM_FEATURE_SD_LOG                                  FEATURE_OFF

#define OAM_FEATURE_TEMP_PROTECT_QOS                        FEATURE_OFF

#define OAM_FEATURE_HIFI_SUPPORT                            FEATURE_OFF

/* SIM卡温度保护 */
#define FEATURE_TEMPPROTECT_SIM                             FEATURE_OFF

/* SIM卡和电池温度保护 */
#define FEATURE_TEMPPROTECT_BATTERY_SIM                     FEATURE_ON



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* __FEATURECONFIGOAM_H__ */
