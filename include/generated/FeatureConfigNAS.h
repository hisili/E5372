/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : FeatureConfigNAS.h
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
  2.��    ��   : 2008-09-10
    �޸�����   : ���ⵥ��:AT2D05643
  3.��    ��   : 2008��10��10��
    �޸�����   : ���ⵥ�ţ�AT2D06152��AT2D06151��, USIMM�Ż������SMSP�ļ���ض�д����ʧ��
  4.��    ��   : 2008-11-17
    �޸�����   : ���ⵥ��:AT2D06780, ����״̬����Ķ���ɾ�����ϱ��ӿ����ն˹������󲻷�����
  5.��    ��   : 2008-12-02
    �޸�����   : ���ⵥ�ţ�AT2D07148������GCF������׮CMSTUB����
  6.��    ��   : 2009��4��7��
    �޸�����   : ���ⵥ�� AT2D06392, ��֧�ֶ��ź�״̬�����NV�洢��ش���ͨ���꿪�عر�
  7.��    ��   : 2009-04-20
    �޸�����   : ���ⵥ�ţ�AT2D11131/AT2D11170,H3G����,���ڲ���USIMָ�����ȼ�PLMN��Χ�ڵ��������磬W�ĸ�����PLMN�ŵ���ǰ��
  8.��    ��   : 2009��5��10��
    �޸�����   : AT2D12319, NAS R6������
  9.��    ��   : 2009��6��18��
    �޸�����   : ���ⵥ�ţ�AT2D12361,�����������ٻ�������Ҫ���5���Ӳ��������ѵ���
 10.��    ��   : 2010��1��26��
    �޸�����   : ���ⵥ��AT2D16564
                 PS �������������δ���ã�����GCF������������Ҫ����AT ������
                 ��PS������������ܣ�������ԣ�
 11.��    ��   : 2010��6��10��
******************************************************************************/
#ifndef __FEATURECONFIGNAS_H__
#define __FEATURECONFIGNAS_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#include "FeatureConfig.h"


/* SIM��Call Controlҵ�� */
#define NAS_FEATURE_SIM_CALL_CTRL                           FEATURE_OFF

/* ��(U)SIM���ж�ȡCSMP���� */
#define NAS_FEATURE_GET_CSMP_PARA_FROM_USIM                 FEATURE_OFF

/* SIM��MO SMS Controlҵ�� */
#define NAS_FEATURE_SIM_MO_SMS_CTRL                         FEATURE_OFF

/*�����Ƿ�֧�ֶ��ŵ�PP-DOWNLOAD����*/
#define NAS_FEATURE_SMS_PP_DOWNLOAD                         FEATURE_ON

/*����NVIM���Ƿ��ܱ������*/
#define NAS_FEATURE_SMS_NVIM_SMSEXIST                       FEATURE_ON
#define NAS_FEATURE_SMS_FLASH_SMSEXIST                      FEATURE_ON

/*����NVIM���Ƿ��ܱ������״̬����*/
#define NAS_FEATURE_SMS_NVIM_SMSREXIST                      FEATURE_OFF

/*����NVIM���Ƿ��ܱ���㲥��Ϣ*/
#define NAS_FEATURE_SMS_NVIM_CBMEXIST                       FEATURE_OFF

/*�����Ƿ�֧�ֶ��ŵ�CB-DOWNLOAD����*/
#define NAS_FEATURE_SMS_CB_DOWNLOAD                         FEATURE_OFF

/* ֧�ֶ���״̬����浽EFSMS�ļ�*/
#define NAS_FEATURE_SMS_STATUS_REPORT_IN_EFSMS              FEATURE_ON

/* CCBS(��æ�������)ҵ�� */
#define NAS_FEATURE_CCBS                                    FEATURE_ON


/* ֧�ֺ���ƫתҵ�� */
#define NAS_FEATURE_CALL_DEFLECTION                         FEATURE_ON

/*֧����·�л�ҵ��*/
#define NAS_FEATURE_ALS                                     FEATURE_ON

/* ECT(��ʽ����ת��)ҵ�� */
#define NAS_FEATURE_ECT                                     FEATURE_ON


/* AoC(�Ʒ�֪ͨ)ҵ�� */
#define NAS_FEATURE_AOC                                     FEATURE_OFF


/* Call Controlҵ�� */
#define NAS_FEATURE_CALL_CONTROL                            FEATURE_ON


/* ֧�ֺ�����ҵ���޸� */
#define NAS_FEATURE_IN_CALL_MODIFY                          FEATURE_ON


/* ֧���������潻�� */
#define NAS_FEATURE_ALTER_V_F                               FEATURE_OFF


/* �������������豸��Э��ջ���� */
#define NAS_FEATURE_LOCAL_VOCODER                           FEATURE_ON


/* ֧��SMS(����Ϣҵ��) */
#define NAS_FEATURE_SMS                                     FEATURE_ON

/* �������� */
#define NAS_FEATURE_SUPPORT_SIM_LOCK                        FEATURE_OFF

/* ����Ӫ�̹��� */
#define NAS_FEATURE_SUPPORT_OPERATOR_LOCK                   FEATURE_ON

/* SATλ�ñ��湦�� */
#define NAS_FEATURE_SUPPORT_SAT_LOCATION_STATUS_EVENT       FEATURE_OFF

/* H3G������ */
#define NAS_FEATURE_SUPPORT_H3G_REQ                         FEATURE_ON

/* RAU�Ż����Կ��أ�ĿǰNASֻ��V1R2��֧ά������V1R2����Ҫ��������ԣ�����øú���������V1R2��֧����ά�����ú����ȥ��,2009-05-25 */
#define NAS_FEATURE_RAU_FOR_V2R1                            FEATURE_ON

/* Lu��Attach��Rau Reject #12���Ƿ����� Available Timer ��ʱ������ʱ����ʱ�������� */
#define NAS_FEATURE_REGISTER_LA_NOT_ALLOW_INIT_PLMN_SEARCH  FEATURE_ON

/* ʵ�ֿ���������Ĭ�Ϸ�����CGCLASS�����֧������ΪA/Bģʽ������û��ʹ��NV��¼ */
#define NAS_FEATURE_AT_COMMAND_CGCLASS                      FEATURE_OFF

/* VCģ��VOICE CONTROL���� */
#define NAS_FEATURE_CS_VC                                   FEATURE_OFF

/* CS+PS�����������Ƿ񴥷�PDP MODIFY���̽������أ���ֹCPU���ظ�λ */
#define NAS_FEATURE_CS_PS_FLOW_CONTROL_BY_PDP_MODIFY        FEATURE_OFF

/* ����������ΪPS ONLYʱ���Ƿ�֧��CS����źͺ���ҵ��(�������г���) */
#define NAS_FEATURE_PS_ONLY_CS_SERVICE_SUPPORT              FEATURE_OFF

#define FEATURE_AT_PORTING                                  FEATURE_OFF

/*���modem�˿��������Ժ�Ŀ���*/
#define NAS_FEATURE_MODEM                                   FEATURE_ON
/*��Ժڰ����������������Ժ����*/
#define NAS_FEATURE_BLACK_WHITE_PLMN                        FEATURE_OFF

/*���DOCK͸�����������������Ժ����*/
#define NAS_FEATURE_DOCK                                    FEATURE_OFF

/*���SD���Զ����������������Ժ���� */ 
#define NAS_FEATURE_SD_UPDATE                               FEATURE_ON

/*����ձ�BVT���������޸ģ��������Ժ����*/
#define NAS_FEATURE_BVT_TBAR                                FEATURE_OFF

/*���E5�ļ�¼��ʷ���������������Ժ����*/
#define NAS_FEATURE_HISTORY_FLUX                            FEATURE_OFF

/*����ձ�HPLMNע�ᱻ�������������Ժ����*/
#define NAS_FEATURE_HPLMN_REG_REJ                           FEATURE_OFF

/*���AT�����뱣�������������Ժ����*/
#define NAS_FEATURE_AT_PWD                                  FEATURE_ON

/*���SD�豸���뱣�������������Ժ����*/
#define NAS_FEATURE_SD_PWD                                  FEATURE_ON

/*��԰��貦�����������������Ժ����*/
#define NAS_FEATURE_DEMAND_DIAL_MODE                        FEATURE_OFF

/*���翨��������������*/
#define NAS_FEATURE_GATEWAY                                 FEATURE_OFF

#define FEATURE_BALONGV7R_ADAPT_BALONGV3R1                  FEATURE_OFF


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* __FEATURECONFIGNAS_H__ */
