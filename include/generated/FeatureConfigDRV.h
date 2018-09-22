#ifndef __FEATURECONFIGDRV_H__
#define __FEATURECONFIGDRV_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "FeatureConfig.h"
#define FEATURE_SIM_NOTIFY                          FEATURE_OFF
#if 0
/*Ö§³ÖDEEPSLEEP*/
#define FEATURE_DEEPSLEEP                           FEATURE_ON
#if defined(FEATURE_LCARD_BOOTROM)
#undef FEATURE_LCARD
/* Lighting Card */
#define FEATURE_LCARD                               FEATURE_OFF
#else
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
#endif
#endif /* __FEATURECONFIGDRV_H__ */

