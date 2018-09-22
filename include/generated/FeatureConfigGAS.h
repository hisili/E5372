/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : FeatureConfigGAS.h
  �� �� ��   :
  ��    ��   :
  ��������   :
  ����޸�   :
  ��������   :
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2010��03��07��
    �޸�����   : �汾�����Ż���ɾ��BALONG_GAS_GUL_MODE

******************************************************************************/
#ifndef __FEATURECONFIGGAS_H__
#define __FEATURECONFIGGAS_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "FeatureConfig.h"

/* GASЭ��汾���� */
#define GAS_PTL_VER_PRE_R99             (PS_PTL_VER_PRE_R99)
#define GAS_PTL_VER_R99                 (PS_PTL_VER_R99)
#define GAS_PTL_VER_R3                  (PS_PTL_VER_R3)
#define GAS_PTL_VER_R4                  (PS_PTL_VER_R4)
#define GAS_PTL_VER_R5                  (PS_PTL_VER_R5)
#define GAS_PTL_VER_R6                  (PS_PTL_VER_R6)
#define GAS_PTL_VER_R7                  (PS_PTL_VER_R7)
#define GAS_PTL_VER_R8                  (PS_PTL_VER_R8)
#define GAS_PTL_VER_R9                  (PS_PTL_VER_R9)

/* GAS��ǰ�汾 */
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
