/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  balong_led_drv.h
*
*
*   描    述 :  LED_DRV A核模块头文件
*
*************************************************************************/
#ifndef _BALONG_LED_DRV_H_
#define _BALONG_LED_DRV_H_


/**************************************************************************
  头文件包含                            
**************************************************************************/
#include <linux/device.h>

#define NV_POWER_SAVE       0xd114
#define LED_ENABLE              1
#define LED_DISABLE             0
#define NV_OK                   0

/**************************************************************************
  枚举定义
**************************************************************************/
typedef enum _LED_COLOR
{ 
    LED_COLOR_RED = 0,
    LED_COLOR_GREEN,  
    LED_COLOR_YELLOW,
    LED_COLOR_MAX
}LED_COLOR;

typedef enum _LED_ID
{
    LED_ID_NETWORK = 0,   /*网络连接指示灯*/
    LED_ID_WiFi,      /*WiFi指示灯*/
    LED_ID_BATTERY,   /*电池指示灯*/
    LED_ID_SM,        /*短信指示灯*/
    LED_ID_POWER,     /*电源指示灯*/
    LED_ID_MAX
} LED_ID;

typedef enum _LED_IOCTL_CMD
{
    LED_IOCTL_SET = 0,
    LED_IOCTL_GET,
    LED_IOCTL_CMD_MAX
}LED_IOCTL_CMD;

typedef enum _LED_STATE
{
    LED_STATE_ON = 0,
    LED_STATE_OFF,
    LED_STATE_BLINK,        /*正常状态 单色闪烁*/
    LED_STATE_COLOR_BLINK,  /*双色交替闪烁*/
    LED_STATE_ALL_BLINK,    /*故障状态 5个灯一起闪烁*/
    LED_STATE_MAX
}LED_STATE;
/**************************************************************************
  STRUCT定义
**************************************************************************/
typedef struct _LED_IOCTL_ARG
{
    LED_STATE led_state; 
    LED_ID led_id; 
    LED_COLOR led_color;
    LED_COLOR second_led_color;
    unsigned int duration;
    unsigned int second_duration;
}LED_IOCTL_ARG;


typedef struct
{
    BSP_U32 u32ModuleId;
    BSP_U32 u32FuncId;
    BSP_U32 ulLedCmd;                /*LED_DRV 点灯命令*/
    LED_IOCTL_ARG  stLedIoctlArg;    /* 点灯参数的数据结构 */
}LED_IFC_MSG_STRU;

/**************************************************************************
  函数声明
**************************************************************************/
int balong_led_SendArg(unsigned int cmd,LED_IOCTL_ARG *pstLedArg);
int balong_led_open(struct inode *inode, struct file *file);
int balong_led_ioctl(struct inode *inode, struct file *file, unsigned int cmd,
    unsigned long arg);
int balong_led_release(struct inode *inode, struct file *file);
static int __init balong_led_init(void);

/*V3R2 E5 BREATH_LED begin*/
typedef enum
{
    BREATH_LED_ID_POWER = 0,        /*电源指示灯*/
    BREATH_LED_ID_MAX
}BREATH_LED_ID;

typedef enum
{
    BREATH_LED_STATE_ON = 0,
    BREATH_LED_STATE_OFF,
    BREATH_LED_STATE_BLINK,        /*呼吸灯闪烁*/
    BREATH_LED_STATE_ENABLE,       /*呼吸灯使能*/
    BREATH_LED_STATE_DISABLE,      /*呼吸灯不使能*/
    BREATH_LED_STATE_MAX
}BREATH_LED_STATE;

typedef struct
{	
	BREATH_LED_ID led_id;          /*指示灯ID*/	
	BREATH_LED_STATE led_state;    /*指示灯的状态*/
}BREATH_LED_IOCTL_ARG;

typedef struct
{
    BSP_U32 u32ModuleId;
    BSP_U32 u32FuncId;
    BSP_U32 ulLedId;       /*点灯ID*/
    BSP_U32 ulLedState;    /* 点灯参数*/
}BREATH_LED_IFC_MSG_STRU;

/*省电功能配置，标识各外设是否使能NV项，在webui上可配置*/
typedef struct
{
    BSP_U32 ulLEDEnable;   /*呼吸灯LED 是否使能：0：不使能；1：使能*/
    BSP_U32 ulReserved1;   /*此项预留  是否使能：0：不使能；1：使能*/
    BSP_U32 ulReserved2;   /*此项预留  是否使能：0：不使能；1：使能*/
}NV_POWER_SAVE_TYPE;
int balong_breath_led_ioctl(struct inode *inode, struct file *file, unsigned int led_id, unsigned int led_state);
void balong_breath_temp_enable(int flag);
int balong_breath_get_status();

/*V3R2 E5 BREATH_LED end*/

#endif  /*end #define _BALONG_LED_DRV_H_*/

