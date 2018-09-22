/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : FeatureConfigTTF.h
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
#ifndef __FEATURECONFIGTTF_H__
#define __FEATURECONFIGTTF_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "FeatureConfig.h"

/* TODO:SFT && ASICƽ̨û��BBP Master */
#define FEATURE_BBP_MASTER                          FEATURE_OFF

/* ѹ������ */
#define TTF_CF_FEATURE                              FEATURE_OFF

/* TTF�ڴ���Կ��� */
/* Ŀǰ�򿪿�������Ҫ����ڴ�й©, �ȶ�����Թر� */
#define FEATURE_TTF_MEM_DEBUG                       FEATURE_ON

/* �Ƿ�֧�� Piggybacked ���� */
#define FEATURE_TTF_RLC_PIGGY_BACKED                FEATURE_OFF

/* �Ƿ�֧��ʹ��R99����˫���� */
#define FEATURE_W_R99_DL_DUAL_MAILBOX               FEATURE_ON

/* �Ƿ�֧�ֻ��memcpy */
#define FEATURE_ASSEMBLY_MEM_CPY                    FEATURE_ON

/* HSPA�����Ż� */
#define FEATURE_HSPA_PERF_IMP                       FEATURE_ON

/* RACH No-Ack ABB debug */
#define FEATURE_RACH_NO_ACK_DEBUG                   FEATURE_OFF

/* RLC�����Ż� */
#define FEATURE_RLC_REASSEMBLE                      FEATURE_ON

/* TCP-ACKǰ�� */
#define FEATURE_TCP_ACK_IN_FRONT                    FEATURE_ON

#if (BALONG_CHIP_VER == BALONG_CHIP_V500)

/* V5 CICOM�Ż� */
#define FEATURE_CICOM_V300_SWITCH                   FEATURE_ON

/* �Ƿ�֧��CipherоƬ��Masterģʽ*/
#define FEATURE_CIPHER_MASTER                       FEATURE_ON

/* SOC HDLC */
#define FEATURE_HDLC_ENHANCE                        FEATURE_ON

/* V7R1ƽ̨Ŀǰ��ʹ��EDMAC */
#ifndef VERSION_V3R2_C00
#define FEATURE_EDMAC                               FEATURE_OFF
#else
/* Ŀǰֻ��V3R2 BBITƽ̨�Ͽ���EDMAC���� */
#ifdef BOARD_FPGA
#define FEATURE_EDMAC                               FEATURE_ON
#else
#define FEATURE_EDMAC                               FEATURE_OFF
#endif
#endif

#elif (BALONG_CHIP_VER == BALONG_CHIP_V300)

/* V5 CICOM�Ż� */
#define FEATURE_CICOM_V300_SWITCH                   FEATURE_OFF

/* �Ƿ�֧��CipherоƬ��Masterģʽ*/
#define FEATURE_CIPHER_MASTER                       FEATURE_ON

/* SOC HDLC */
#define FEATURE_HDLC_ENHANCE                        FEATURE_ON

#define FEATURE_EDMAC                               FEATURE_OFF

#elif (BALONG_CHIP_VER == BALONG_CHIP_V200)

/* V5 CICOM�Ż� */
#define FEATURE_CICOM_V300_SWITCH                   FEATURE_OFF

/* �Ƿ�֧��CipherоƬ��Masterģʽ*/
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
