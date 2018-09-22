/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : FeatureConfigGAS.h
  版 本 号   :
  作    者   :
  生成日期   :
  最近修改   :
  功能描述   :
  函数列表   :
  修改历史   :
  1.日    期   : 2010年03月07日
    修改内容   : 版本构建优化，删除BALONG_GAS_GUL_MODE

******************************************************************************/
#ifndef __FEATURECONFIGGAS_H__
#define __FEATURECONFIGGAS_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "FeatureConfig.h"

/* GAS协议版本定义 */
#define GAS_PTL_VER_PRE_R99             (PS_PTL_VER_PRE_R99)
#define GAS_PTL_VER_R99                 (PS_PTL_VER_R99)
#define GAS_PTL_VER_R3                  (PS_PTL_VER_R3)
#define GAS_PTL_VER_R4                  (PS_PTL_VER_R4)
#define GAS_PTL_VER_R5                  (PS_PTL_VER_R5)
#define GAS_PTL_VER_R6                  (PS_PTL_VER_R6)
#define GAS_PTL_VER_R7                  (PS_PTL_VER_R7)
#define GAS_PTL_VER_R8                  (PS_PTL_VER_R8)
#define GAS_PTL_VER_R9                  (PS_PTL_VER_R9)

/* GAS当前版本 */
#if (FEATURE_ON == FEATURE_LTE)
#define GAS_UE_REL_VER                  (GAS_PTL_VER_R9)
#else
#define GAS_UE_REL_VER                  (GAS_PTL_VER_R6)
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* __FEATURECONFIGGAS_H__ */
