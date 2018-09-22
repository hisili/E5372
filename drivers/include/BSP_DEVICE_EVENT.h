/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_DEVICE_EVENT.h
*
*   ��    �� :  z67193
*
*   ��    �� :  BSP���Ͷ���ͷ�ļ�
*
*   �޸ļ�¼ :  2012��04��14��  v1.00  z67193  ����
*************************************************************************/

#ifndef	__BSP_DEVICE_EVENT_H__
#define __BSP_DEVICE_EVENT_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*****************************************************************************
  2 �궨��
*****************************************************************************/

#ifdef __VXWORKS__
typedef enum
{
    DEVICE_ID_NULL_ID =0, /*��id�����ڳ�ʼ��id*/
    DEVICE_ID_USB = 1,     /*USB�豸id*/
    DEVICE_ID_KEY = 2,     /*KEY�����豸id*/
    DEVICE_ID_BATTERY = 3,/*����¼��ϱ�ʹ�ô�ID*/
    DEVICE_ID_CHARGER = 4, /*������¼��ϱ�ʹ�ô�ID*/
    DEVICE_ID_SD_CARD = 5,
    DEVICE_ID_GPIO_INTER=6, /*�ǰ�����gpio�ж��ϱ���ʹ�ô�ID*/
    DEVICE_ID_SCREEN=7,
    DEVICE_ID_WLAN=8,
    DEVICE_ID_OM = 9, /* OM�ϱ���Ϣ */
	DEVICE_ID_TEMP=10,/*�¶ȱ���ID*/
    DEVICE_ID_MAX_ID   /*�����߽籣��*/
}DEVICE_ID_E;

typedef struct device_event_st
{
    int device_id;   /*�豸ID*/
    int value;         /*��Ϣvalue*/
    char * desc;     /*����*/
}device_event_t;

#define  USB_ENUM_UNFINISH   (0)          /* ��δö�����*/
#define  USB_ENUM_FINISH        (1)          /* �����      */


#define CHARGER_ONLINE                  (1) /*�������λ*/
#define CHARGER_OFFLINE                 (0)/*���������λ*/

#define WPS_PIN_LEN (8)                   /* WIFI WPS PIN CODE LEN */


/*****************************************************************************
  3 ö�ٶ���
*****************************************************************************/

/**********************************************************************
*
*device event indication
*
*********************************************************************/
typedef enum
{
  USB_INIT_EVENT    = 0,   /*��ʼ���¼�*/
  USB_HALT_EVNT     = 1,   /*halt�¼�*/
  USB_RESET_EVENT   = 2,   /*�����¼�*/
  USB_ENABLE_EVENT  = 3,   /*ʹ���¼�*/
  USB_DISABLE_EVENT = 4,   /*ȥʹ���¼�*/
  USB_SUSPEND_EVENT = 5,   /*suspend�¼�*/
  USB_RESUME_EVENT  = 6,   /*resume�¼�*/
  USB_RESERVE_EVENT        /*�����¼�*/
}USB_LCARD_EVENT_ENUM;

typedef enum
{
    USB_UP,        /*����usb*/
    USB_DOWN,    /*����usb*/
    USB_NDIS_UP, /*NDIS up*/
    USB_NDIS_DOWN,/*Ndis down*/
    USB_RESET    /*usb ����*/
} USB_EVENT;

typedef enum _KEY_EVENT
{
    KEY_WAKEUP = 0,
    KEY_WLAN,
    KEY_WPS,
    KEY_FACTORY_RESTORE,
    KEY_POWER_OFF,
    KEY_UPDATE,
    KEY_SSID,
#if (FEATURE_KEYBOARD  == FEATURE_ON)
    KEY_WPS_PROMPT,
#endif
    KEY_EVENT_MAX
} KEY_EVENT;

typedef enum
{
    BAT_CHARGING_ON_START = 0,/*�ڿ���ģʽ����ʼ���**/
    BAT_CHARGING_OFF_START,	/*�ڹػ�ģʽ����ʼ���*/
    BAT_LOW_POWER, 	/*��ص�����ʱ���ϱ����¼�*/
    BAT_CHARGING_UP_STOP, 	/*����ֹͣ���ʱ���ϱ����¼�*/
    BAT_CHARGING_DOWN_STOP,	/*�ػ�ֹͣ���ʱ���ϱ����¼�*/
    BAT_EVENT_MAX  /*����¼�ֵ��С�ڴ�ֵ����Ϊ�Ƿ��¼�*/
}BATTERY_EVENT;

typedef enum
{
    TEMP_BATT_LOW, /*��ص����¼�*/
    TEMP_BATT_HIGH  /*��ظ����¼�*/
}TEMPERATURE_EVENT;

typedef enum
{
    CHARGER_ATTACH,
    CHARGER_REMOVE,
    CHARGER_EVENT_MAX
}CHARGER_EVENT;

/*****************************************************************************
 Function   : drivers_event_dispatch
 Description:
 Calls      :
 Called By  :
 Input      : None
 Return     : VOS_OK or VOS_ERR
 Other      : just dispatch, no process, no global variable return quikly
              Not support NOW, dont USE it! remove it later.
 *****************************************************************************/
int drivers_event_dispatch(device_event_t * device_event);

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_DEVICE_EVENT_H__ */


