/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  balong_led_drv.h
*
*
*   ��    �� :  LED_DRV A��ģ��ͷ�ļ�
*
*************************************************************************/
#ifndef _BALONG_LED_DRV_H_
#define _BALONG_LED_DRV_H_


/**************************************************************************
  ͷ�ļ�����                            
**************************************************************************/
#include <linux/device.h>

#define NV_POWER_SAVE       0xd114
#define LED_ENABLE              1
#define LED_DISABLE             0
#define NV_OK                   0

/**************************************************************************
  ö�ٶ���
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
    LED_ID_NETWORK = 0,   /*��������ָʾ��*/
    LED_ID_WiFi,      /*WiFiָʾ��*/
    LED_ID_BATTERY,   /*���ָʾ��*/
    LED_ID_SM,        /*����ָʾ��*/
    LED_ID_POWER,     /*��Դָʾ��*/
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
    LED_STATE_BLINK,        /*����״̬ ��ɫ��˸*/
    LED_STATE_COLOR_BLINK,  /*˫ɫ������˸*/
    LED_STATE_ALL_BLINK,    /*����״̬ 5����һ����˸*/
    LED_STATE_MAX
}LED_STATE;
/**************************************************************************
  STRUCT����
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
    BSP_U32 ulLedCmd;                /*LED_DRV �������*/
    LED_IOCTL_ARG  stLedIoctlArg;    /* ��Ʋ��������ݽṹ */
}LED_IFC_MSG_STRU;

/**************************************************************************
  ��������
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
    BREATH_LED_ID_POWER = 0,        /*��Դָʾ��*/
    BREATH_LED_ID_MAX
}BREATH_LED_ID;

typedef enum
{
    BREATH_LED_STATE_ON = 0,
    BREATH_LED_STATE_OFF,
    BREATH_LED_STATE_BLINK,        /*��������˸*/
    BREATH_LED_STATE_ENABLE,       /*������ʹ��*/
    BREATH_LED_STATE_DISABLE,      /*�����Ʋ�ʹ��*/
    BREATH_LED_STATE_MAX
}BREATH_LED_STATE;

typedef struct
{	
	BREATH_LED_ID led_id;          /*ָʾ��ID*/	
	BREATH_LED_STATE led_state;    /*ָʾ�Ƶ�״̬*/
}BREATH_LED_IOCTL_ARG;

typedef struct
{
    BSP_U32 u32ModuleId;
    BSP_U32 u32FuncId;
    BSP_U32 ulLedId;       /*���ID*/
    BSP_U32 ulLedState;    /* ��Ʋ���*/
}BREATH_LED_IFC_MSG_STRU;

/*ʡ�繦�����ã���ʶ�������Ƿ�ʹ��NV���webui�Ͽ�����*/
typedef struct
{
    BSP_U32 ulLEDEnable;   /*������LED �Ƿ�ʹ�ܣ�0����ʹ�ܣ�1��ʹ��*/
    BSP_U32 ulReserved1;   /*����Ԥ��  �Ƿ�ʹ�ܣ�0����ʹ�ܣ�1��ʹ��*/
    BSP_U32 ulReserved2;   /*����Ԥ��  �Ƿ�ʹ�ܣ�0����ʹ�ܣ�1��ʹ��*/
}NV_POWER_SAVE_TYPE;
int balong_breath_led_ioctl(struct inode *inode, struct file *file, unsigned int led_id, unsigned int led_state);
void balong_breath_temp_enable(int flag);
int balong_breath_get_status();

/*V3R2 E5 BREATH_LED end*/

#endif  /*end #define _BALONG_LED_DRV_H_*/

