/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_DEVICE_EVENT.h
*
*   作    者 :  z67193
*
*   描    述 :  BSP类型定义头文件
*
*   修改记录 :  2012年04月14日  v1.00  z67193  创建
*************************************************************************/

#ifndef	__BSP_DEVICE_EVENT_H__
#define __BSP_DEVICE_EVENT_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*****************************************************************************
  2 宏定义
*****************************************************************************/

#ifdef __VXWORKS__
typedef enum
{
    DEVICE_ID_NULL_ID =0, /*空id，用于初始化id*/
    DEVICE_ID_USB = 1,     /*USB设备id*/
    DEVICE_ID_KEY = 2,     /*KEY按键设备id*/
    DEVICE_ID_BATTERY = 3,/*电池事件上报使用此ID*/
    DEVICE_ID_CHARGER = 4, /*充电器事件上报使用此ID*/
    DEVICE_ID_SD_CARD = 5,
    DEVICE_ID_GPIO_INTER=6, /*非按键的gpio中断上报，使用此ID*/
    DEVICE_ID_SCREEN=7,
    DEVICE_ID_WLAN=8,
    DEVICE_ID_OM = 9, /* OM上报消息 */
	DEVICE_ID_TEMP=10,/*温度保护ID*/
    DEVICE_ID_MAX_ID   /*用做边界保护*/
}DEVICE_ID_E;

typedef struct device_event_st
{
    int device_id;   /*设备ID*/
    int value;         /*消息value*/
    char * desc;     /*描述*/
}device_event_t;

#define  USB_ENUM_UNFINISH   (0)          /* 还未枚举完成*/
#define  USB_ENUM_FINISH        (1)          /* 已完成      */


#define CHARGER_ONLINE                  (1) /*充电器在位*/
#define CHARGER_OFFLINE                 (0)/*充电器不在位*/

#define WPS_PIN_LEN (8)                   /* WIFI WPS PIN CODE LEN */


/*****************************************************************************
  3 枚举定义
*****************************************************************************/

/**********************************************************************
*
*device event indication
*
*********************************************************************/
typedef enum
{
  USB_INIT_EVENT    = 0,   /*初始化事件*/
  USB_HALT_EVNT     = 1,   /*halt事件*/
  USB_RESET_EVENT   = 2,   /*重起事件*/
  USB_ENABLE_EVENT  = 3,   /*使能事件*/
  USB_DISABLE_EVENT = 4,   /*去使能事件*/
  USB_SUSPEND_EVENT = 5,   /*suspend事件*/
  USB_RESUME_EVENT  = 6,   /*resume事件*/
  USB_RESERVE_EVENT        /*保留事件*/
}USB_LCARD_EVENT_ENUM;

typedef enum
{
    USB_UP,        /*插入usb*/
    USB_DOWN,    /*拔下usb*/
    USB_NDIS_UP, /*NDIS up*/
    USB_NDIS_DOWN,/*Ndis down*/
    USB_RESET    /*usb 重启*/
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
    BAT_CHARGING_ON_START = 0,/*在开机模式，开始充电**/
    BAT_CHARGING_OFF_START,	/*在关机模式，开始充电*/
    BAT_LOW_POWER, 	/*电池电量低时，上报此事件*/
    BAT_CHARGING_UP_STOP, 	/*开机停止充电时，上报此事件*/
    BAT_CHARGING_DOWN_STOP,	/*关机停止充电时，上报此事件*/
    BAT_EVENT_MAX  /*如果事件值不小于此值，则为非法事件*/
}BATTERY_EVENT;

typedef enum
{
    TEMP_BATT_LOW, /*电池低温事件*/
    TEMP_BATT_HIGH  /*电池高温事件*/
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


