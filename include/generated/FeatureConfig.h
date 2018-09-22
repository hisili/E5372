/******************************************************************************

                  ��Ȩ���� (C), 2001-2011, ��Ϊ�������޹�˾

 ******************************************************************************
  �� �� ��   : FeatureConfig.h
  �� �� ��   :
  ��    ��   : ���62584
  ��������   : 2008��03��11��
  ����޸�   :
  ��������   : �����˲�Ʒ����ģ�����Ժ궨��
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2008��03��11��
    ��    ��   : ���62584
    �޸�����   : �����ļ�

******************************************************************************/
#ifndef __FEATURECONFIG_H__
#define __FEATURECONFIG_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* �������Ա��� */
#define FEATURE_ON                                  1
#define FEATURE_OFF                                 0

/* Э��ջ�汾 */
#define PS_PTL_VER_DANAMIC                          20

#define PS_PTL_VER_PRE_R99                          (-2)
#define PS_PTL_VER_R99                              (-1)
#define PS_PTL_VER_R3                               0
#define PS_PTL_VER_R4                               1
#define PS_PTL_VER_R5                               2
#define PS_PTL_VER_R6                               3
#define PS_PTL_VER_R7                               4
#define PS_PTL_VER_R8                               5
#define PS_PTL_VER_R9                               6
#define PS_UE_REL_VER                               PS_PTL_VER_R9

/* GSM ͨ�Ž��뼼��ģʽ */
#define FEATURE_UE_MODE_G                           FEATURE_ON

/* WCDMA ͨ�Ž��뼼��ģʽ */
#define FEATURE_UE_MODE_W                           FEATURE_ON

/* GPRS֧�� */
#define FEATURE_GPRS                                FEATURE_ON

/* ֧��CS����ҵ�� */
#define FEATURE_CS                                  FEATURE_ON

/* EDGE֧�� */
#define FEATURE_EDGE                                FEATURE_ON

/*֧��SAIC DARP�����ϱ�*/
#define FEATURE_SAIC_DARP                           FEATURE_OFF

/* Gģ����֧�� */
#define FEATURE_GFAX                                FEATURE_OFF

/* Wģ����֧�� */
#define FEATURE_WFAX                                FEATURE_OFF

/* GС���㲥 */
#define FEATURE_GCBS                                FEATURE_ON

/* WС���㲥 */
#define FEATURE_WCBS                                FEATURE_ON

/* ETWS���ܿ��� */
#define FEATURE_ETWS                                FEATURE_ON
/* ֧��AP */
#define FEATURE_AP                                  FEATURE_OFF

/* ֧��BALONG_HW_CORE */
#define BALONG_HW_CORE                              FEATURE_OFF

/* ֧��BALONG_SW_CORE */
#define BALONG_SW_CORE                              FEATURE_OFF

/* ֧��BALONG_UDP_V1R1 */
#define BALONG_UDP_V1R1                             FEATURE_OFF

/* ֧��BALONG_UDP_V1R2 */
#define BALONG_UDP_V1R2                             FEATURE_OFF

/* ֧��BALONG_UDP_V2R1 */
#define BALONG_UDP_V2R1                             FEATURE_OFF

/* ֧��BALONG_UDP */
#define BALONG_UDP                                  FEATURE_OFF

/* ֧��BALONG_UDP_V3R1 */
#define BALONG_UDP_V3R1                             FEATURE_OFF

#define FEATURE_STICK                               FEATURE_OFF
#define FEATURE_HILINK                              FEATURE_OFF

/* ֧��E5 */
#define FEATURE_E5                                  FEATURE_ON

/* ֧��E5_UDP */
#define FEATURE_E5_UDP                              FEATURE_OFF


/* ֧��E5_PRODUCT */
#define FEATURE_E5_PRODUCT                          FEATURE_OFF

/* ֧��E5_PRODUCT_GLOBAL */
#define FEATURE_E5_PRODUCT_GLOBAL                   FEATURE_OFF

/* PC Voice */
#define FEATURE_PC_VOICE                            FEATURE_OFF

/* Lighting Card */
#define FEATURE_LCARD                               FEATURE_OFF

/* Security Boot */
#define FEATURE_SEC_BOOT                            FEATURE_ON

/* E353 HW */
#define FEATURE_E353                                FEATURE_ON

#define FEATURE_LTE                                 FEATURE_ON
#define FEATURE_UE_MODE_TDS							FEATURE_ON

#define FEATURE_GUTL                                FEATURE_ON

#define FEATURE_CSD                                 FEATURE_OFF

#define FEATURE_NDIS                                FEATURE_OFF

#define FEATURE_IPV6                                FEATURE_ON

#define FEATURE_PPP                                 FEATURE_OFF

#define FEATURE_PPPOE                               FEATURE_OFF

#define FEATURE_GNA                                 FEATURE_ON

/* Update OnLine*/
#define FEATURE_UPDATEONLINE                        FEATURE_ON

/* wifi*/
#define FEATURE_WIFI                                FEATURE_ON

/*hifi*/  
#define FEATURE_HIFI                                FEATURE_OFF

/* sdio*/
#define FEATURE_SDIO                                FEATURE_OFF

/* keyboard*/
#define FEATURE_KEYBOARD                            FEATURE_ON

/*charge*/
#define FEATURE_CHARGE                              FEATURE_ON

/*power_on_off*/
#define FEATURE_POWER_ON_OFF                        FEATURE_ON

/*OLED*/
#define FEATURE_OLED                                FEATURE_OFF

/*TFT*/
#define FEATURE_TFT                                 FEATURE_ON

/*MMI_TEST*/
#define FEATURE_MMI_TEST                            FEATURE_ON
/* DL E CELL FACH */
#define FEATURE_DL_E_CELL_FACH                      FEATURE_ON

#define FEATURE_DC_MIMO                             FEATURE_OFF

/*interpeak*/
#define FEATURE_INTERPEAK                           FEATURE_OFF

/*interpeak_mini �ú궨��������FEATURE_INTERPEAK������FEATURE_INTERPEAKΪONʱ��Ч��*/
#define FEATURE_INTERPEAK_MINI                      FEATURE_OFF

/*ipwebs_feature_e5*/
#define IPWEBS_FEATURE_E5                           FEATURE_OFF

/*ipwebs_feature_wireless_dongle*/
#define IPWEBS_FEATURE_WIRELESS_DONGLE              FEATURE_OFF

/*ipwebs_feature_lightning_card*/
#define IPWEBS_FEATURE_LIGHTNING_CARD               FEATURE_OFF

/*feature_webnas*/
#define FEATURE_WEBNAS                              FEATURE_OFF

/* Disable Autorun */
/*#define FEATURE_AUTORUN_HL_1_1                      FEATURE_OFF*/

/* E355 */
#define FEATURE_WIRELESS_DONGLE                     FEATURE_OFF

/* PMU current overflow and high-temperature protection */
#define FEATURE_PMU_PROTECT                         FEATURE_OFF

/* VxWorks TCPIP protocol stack */
#define FEATURE_VXWORKS_TCPIP                       FEATURE_OFF

/* FEATURE_FS_LOG_RECORD */
#define FEATURE_FS_LOG_RECORD                        FEATURE_OFF

#define FEATURE_M2                                  FEATURE_OFF

#define FEATURE_NOSIG                               FEATURE_OFF

/*memory size */
#define FEATURE_64M                                 FEATURE_OFF

#define FEATURE_256M                                FEATURE_OFF

#define FEATURE_ECM_RNDIS                           FEATURE_ON

#define FEATURE_RNIC                                FEATURE_ON

#define FEATURE_NFEXT                               FEATURE_ON

#define FEATURE_UL_E_CELL_FACH                      FEATURE_OFF

#define FEATURE_DC_UPA                              FEATURE_OFF
#define FEATURE_OTG                                 FEATURE_ON
#define FEATURE_HSIC_ON

/* ��ɫ��ʹ�ܿ��� */
#define FEATURE_LEDSTATUS                           FEATURE_OFF

/*E5 LED*/
#define FEATURE_E5_LED                              FEATURE_OFF

/*���� LED*/
#define FEATURE_BREATH_LIGHT                       	FEATURE_ON

/*STICK LED*/
#define FEATURE_TIP_LED                             FEATURE_OFF

/* BBPС����: �����Կ�����, WTTF�ὫBBP�����е����ݿ�����TTF MEM, �Ӷ���С
BBP������, �ﵽ�ü��ڴ��Ŀ�� */
#define FEATURE_SMALL_MAILBOX                       FEATURE_OFF
/* �͹��ķ������µ���ʱ���� */
#define FEATURE_TEMP_DSP_CORE_POWER_DOWN           FEATURE_OFF

/* ����Shell���� */
#define FEATURE_VSHELL                              FEATURE_ON

/* �ڴ�ع���Ԫ */
#define FEATURE_MMU_BIG                             FEATURE_ON

#define FEATURE_MMU_MEDIUM                          FEATURE_OFF

#define FEATURE_MMU_SMALL                           FEATURE_OFF

#define FEATURE_TTFMEM_CACHE                        FEATURE_ON

#define FEATURE_SDUPDATE                      FEATURE_ON 

#define FEATURE_SMS                        			FEATURE_OFF

/*begin add by wanghaijie for MMI test*/
#define FEATURE_E5376_MMI                           FEATURE_OFF   
/*end  add by wanghaijie for MMI test*/
/*��ȷ��5371_MMI��5371_FACTORY������5371���з�����860����*/
#define FEATURE_E5371_MMI                           FEATURE_OFF  
#define FEATURE_E5371_FACTORY                           FEATURE_OFF   

#define FEATURE_CS_CALL                        		FEATURE_OFF

#define FEATURE_PBM                        			FEATURE_ON

#define FEATURE_USSD                        		FEATURE_ON

#define FEATURE_STK                        			FEATURE_ON
/*�����������EPADʹ�� begin*/ 
#define FEATURE_EPAD                              	FEATURE_OFF

#define FEATURE_HSIC_SLAVE                          FEATURE_OFF

#define FEATURE_APTOMODEM_UPDATE			   		FEATURE_OFF
/*end*/

/*���Ժ����������2���� begin*/
#define FEATURE_SDMMC                               FEATURE_ON

#define FEATURE_USB_SWITCH				            FEATURE_OFF
/*end*/


#define FEATURE_SECURITY_SHELL                      FEATURE_ON

#define FEATURE_TEMP_MULTI_MODE_LP                  FEATURE_ON

#define FEATURE_MANUFACTURE_LOG                     FEATURE_OFF

#define HUAWEI_PRODUCT_E5372S_32                    FEATURE_ON

#define   FEATURE_HUAWEI_MBB_DEVICE_AT_119          FEATURE_ON
#if(FEATURE_SEC_BOOT == FEATURE_ON)
#define FEATURE_HANDSET_SECURITY_BOOT 	FEATURE_OFF/*PAD��ȫ���������￪��*/
#else
#define FEATURE_HANDSET_SECURITY_BOOT 	FEATURE_OFF
#endif

#define FEATRUE_XML_PARSER                          FEATURE_OFF

#define FEATURE_AGPS                                FEATURE_OFF

#if 0
#define FEATURE_HUAWEI_LU                           FEATURE_ON
#endif

#define FEATURE_AT_NWTIME                           FEATURE_ON
#define FEATURE_HUAWEI_WIFI_IPV6_CTF FEATURE_ON

#define FEATURE_HUAWEI_VDF                         FEATURE_ON


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* __FEATURECONFIG_H__ */
