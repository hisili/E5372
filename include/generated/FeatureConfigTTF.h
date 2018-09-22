/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : FeatureConfigTTF.h
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
#ifndef __FEATURECONFIGTTF_H__
#define __FEATURECONFIGTTF_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "FeatureConfig.h"

/* TODO:SFT && ASIC平台没有BBP Master */
#define FEATURE_BBP_MASTER                          FEATURE_OFF

/* 压缩开关 */
#define TTF_CF_FEATURE                              FEATURE_OFF

/* TTF内存调试开关 */
/* 目前打开开关以主要检查内存泄漏, 稳定后可以关闭 */
#define FEATURE_TTF_MEM_DEBUG                       FEATURE_ON

/* 是否支持 Piggybacked 功能 */
#define FEATURE_TTF_RLC_PIGGY_BACKED                FEATURE_OFF

/* 是否支持使用R99译码双邮箱 */
#define FEATURE_W_R99_DL_DUAL_MAILBOX               FEATURE_ON

/* 是否支持汇编memcpy */
#define FEATURE_ASSEMBLY_MEM_CPY                    FEATURE_ON

/* HSPA性能优化 */
#define FEATURE_HSPA_PERF_IMP                       FEATURE_ON

/* RACH No-Ack ABB debug */
#define FEATURE_RACH_NO_ACK_DEBUG                   FEATURE_OFF

/* RLC重组优化 */
#define FEATURE_RLC_REASSEMBLE                      FEATURE_ON

/* TCP-ACK前移 */
#define FEATURE_TCP_ACK_IN_FRONT                    FEATURE_ON

#if (BALONG_CHIP_VER == BALONG_CHIP_V500)

/* V5 CICOM优化 */
#define FEATURE_CICOM_V300_SWITCH                   FEATURE_ON

/* 是否支持Cipher芯片的Master模式*/
#define FEATURE_CIPHER_MASTER                       FEATURE_ON

/* SOC HDLC */
#define FEATURE_HDLC_ENHANCE                        FEATURE_ON

/* V7R1平台目前不使用EDMAC */
#ifndef VERSION_V3R2_C00
#define FEATURE_EDMAC                               FEATURE_OFF
#else
/* 目前只在V3R2 BBIT平台上开放EDMAC功能 */
#ifdef BOARD_FPGA
#define FEATURE_EDMAC                               FEATURE_ON
#else
#define FEATURE_EDMAC                               FEATURE_OFF
#endif
#endif

#elif (BALONG_CHIP_VER == BALONG_CHIP_V300)

/* V5 CICOM优化 */
#define FEATURE_CICOM_V300_SWITCH                   FEATURE_OFF

/* 是否支持Cipher芯片的Master模式*/
#define FEATURE_CIPHER_MASTER                       FEATURE_ON

/* SOC HDLC */
#define FEATURE_HDLC_ENHANCE                        FEATURE_ON

#define FEATURE_EDMAC                               FEATURE_OFF

#elif (BALONG_CHIP_VER == BALONG_CHIP_V200)

/* V5 CICOM优化 */
#define FEATURE_CICOM_V300_SWITCH                   FEATURE_OFF

/* 是否支持Cipher芯片的Master模式*/
#define FEATURE_CIPHER_MASTER                       FEATURE_OFF

/* SOC HDLC */
#define FEATURE_HDLC_ENHANCE                        FEATURE_OFF

#define FEATURE_EDMAC                               FEATURE_OFF

#endif
#define FEATURE_CST_ASYN_OR_NOT_TRANS               FEATURE_OFF


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* __FEATURECONFIGTTF_H__ */
