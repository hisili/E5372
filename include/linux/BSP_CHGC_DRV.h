/*************************************************************************
*   ��Ȩ����(C) 1987-2011, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_CHGC_DRV.h
*
*
*   ��    �� :  CHGCģ���û�ͷ�ļ�
*
*
*************************************************************************/
/**********************���ⵥ�޸ļ�¼************************************************
��    ��              �޸���                     ���ⵥ��                   �޸�����
********************************************************************************************/
#ifndef _BSP_CHGC_DRV_H                                                                  
#define _BSP_CHGC_DRV_H

#ifdef __cplusplus
extern "C"
{
#endif



/**************************************************************************
  ͷ�ļ�����                            
**************************************************************************/
#include <mach/common/bsp_version.h>
#include "BSP.h"


/**************************************************************************
  �궨�� 
**************************************************************************/
#define BSP_SYNC_TIMEOUT        2000
#define BSP_CHGC_TIMEOUT 100
#define USB_ENUM_COM 0x1
#define USB_CHG_TYP 0x2
#define USB_STSTE_REPLY 0x3
#define GET_CHG_STA 0x4
#define GET_BAT_STA 0x5
#define SET_CHG_STA 0x6
#define GET_CBC_STA 0x7
#define ChargeEvent 0x8
//����ӿںͲ�ѯ�Ƿ���Ҫ����ӿ���ֲ��A��
#define GET_CHG_STA_AT 0x9
#define SET_CHG_SPLY 0xA
#define SET_CHG_POLL_TIMER_STA 0xB

#define GET_EXT_CHG_STA 0xC
#define GET_EXT_CHG_VOLT_TEMP_STA 0xD

/*��ǳ��ģ���õ��ĺ˼�ͨ�ŵ��������������˼�ͨ��ʱ��Ҫ�����˱���*/
#define GET_CHG_DATA_MAX (GET_EXT_CHG_VOLT_TEMP_STA + 1)

#if (FEATURE_OTG == FEATURE_ON) 
/*MMI��λ�����Զ���������*/
/*���AF10��λ�Ĺ����ڴ�ֵ*/
#define AF10_STATE_UNPLUG    (0x10AF)
#define AF10_STATE_PLUGIN       (0xAF10)
#define AF10_DEVICE_ATTACH     (0x4DEA)
#define AF10_DEVICE_REMOVE     (0xEA4D)

/*������״̬ö��*/
typedef enum
{
    CHG_EXTCHAGRE_STATE_STOP = 0,
    CHG_EXTCHAGRE_STATE_START,
    CHG_EXTCHAGRE_STATE_TEMP, 
    CHG_EXTCHAGRE_STATE_MAX
}EXTCHAGRE_STATE_ENUM_LIST;

/*AF10��λ״̬ö�ٽṹ*/
typedef enum
{
    AF10_STATE_IN = 0,
    AF10_STATE_OUT,
    AF10_DEVICE_IN,
    AF10_DEVICE_OUT
}AF10_STATE_ENUM_LIST;

/*�����翪��״̬ö�ٽṹ*/
typedef enum
{
    CTRL_DISCHARGE_OFF = 0,
    CTRL_DISCHARGE_ON
}CTRL_DISCHARGE_ENUM_LIST;

/*��ض�����״̬��ѯ�ṹ*/
typedef struct BATT_EXTSTATE_tag        
{   
    AF10_STATE_ENUM_LIST  extAF10_state;
    EXTCHAGRE_STATE_ENUM_LIST   extcharging_state;    
}EXTCHAGRE_STATE_T;
#endif
/* BEIGN ,Modified by l00212897,2012/7/3 */
typedef  enum
{
    CHG_POLL_BATT_LEVEL = 0,     /*��ѯ��ظ���*/
    CHG_POLL_USB_STATUS = 3,     /*��ѯUSB״̬*/
    CHG_POLL_STATE_MAX
} CHG_POLL_STATE;

typedef  enum
{
    CHG_USB_NON = 0,     /*USB����λ*/
    CHG_USB_NON_HW,     /*�Ǳ�����*/
    CHG_USB_HW,     /*��׼�����*/
    CHG_POLL_USB_STATE_MAX     
} CHG_USB_STATE;

/* END ,Modified by l00212897,2012/7/3 */

/*���ģ��˫�˹����ڴ�ṹ�壬��32���ֽڣ�Ŀǰ��28���ֽ�δʹ��*/
typedef struct _st_batt_info_share_mem
{
	BSP_U32 batt_current_volt; /*��ǰ��ѹֵ*/
	BSP_U32 batt_unused[7];   /*��δʹ��*/
}st_batt_info_share_mem;

#define PMU2USB_C_FLAG 		0x55430000

#define CHARGER_ONLINE      (1) /*�������λ*/
#define CHARGER_OFFLINE     (0)/*���������λ*/

#define CHG_TYPE_CHARGER 	0x0	/*��׼��������*/
#define CHG_TYPE_USB 		0x1	/*USB���*/
#define CHG_FAST_POLL_TIMER               0x0    /*�춨ʱ��*/
#define CHG_SHORT_ONOFF_POLL_TIMER        0x1    /*�ٹػ�״̬������ʱ��*/
/**************************************************************************
  ��������
**************************************************************************/
BSP_S32 BSP_CHG_Init(void);
BSP_S32 BSP_CHGC_Wait(BSP_U32 MsgType);
BSP_S32 BSP_CHGC_UsbReply(BSP_U32 Msg_Type,BSP_U32 usb_msg,BSP_U32 Error_msg);
BSP_S32 BSP_CHGC_ReportStaToAPP(BSP_VOID *pMsgBody,BSP_U32 u32Len);
BSP_S32 BSP_CHGC_CallUsb(BSP_VOID *pMsgBody,BSP_U32 u32Len);
BSP_S32 BSP_CHGC_AskInterface(BSP_U32 Msg_Type,BSP_U32 u32_msg);
BSP_U32 BSP_CHGC_GetInterface(BSP_VOID *pMsgBody,BSP_U32 u32Len);
BSP_S32 BSP_CHG_GetBatteryState(BATT_STATE_T *battery_state);
void BSP_CHG_StateSet(unsigned long ulState);
BSP_S32 BSP_CHG_GetCbcState(unsigned char *pusBcs,unsigned char *pucBcl);
void BSP_CHG_ChargeEvent(int charger_status);
BSP_S32 BSP_CHGC_ReportToOled(BSP_VOID *pMsgBody,BSP_U32 u32Len);
BSP_S32 BSP_CHG_Appgetbatterystate(BATT_STATE_T *battery_state);
BSP_S32 BSP_CHG_Appgetusbstate(CHG_USB_STATE *usb_status);
BSP_S32 BSP_CHGC_BattSply(BSP_VOID*pMsgBody,BSP_U32 u32Len);
BSP_S32 chg_dok_detect_handler(void); 
BSP_S32 chg_dok_detect_init(void);   
BSP_S32 chg_get_dok_status(void);  
BSP_U32 BSP_CHG_VoltGet(void);
void BSP_CHG_PollTimerSet(BSP_U32 ulState);

#if (FEATURE_OTG == FEATURE_ON) 
void BSP_CHGC_ReportToUsb(BSP_VOID *pMsgBody,BSP_U32 u32Len);
BSP_S32 BSP_GetExtChgSta(void);
#endif

/**************************************************************************
  �����붨��
**************************************************************************/


#ifdef __cplusplus
}
#endif

#endif   
