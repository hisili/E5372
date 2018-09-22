/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_SCREEN.h
*
*   ��    �� :  
*
*   ��    �� :  IPFģ���û��ӿ��ļ�
*
*   �޸ļ�¼ :  2012��04��16��  v1.00  ����
*************************************************************************/

#ifndef __BSP_SCREEN_H__
#define __BSP_SCREEN_H__

#ifdef __cplusplus
extern "C" 
{ 
#endif /* __cplusplus */

typedef enum
{
  PROC_COMM_HUAWEI_IS_POWEROFF_CHARGING,
  PROC_COMM_HUAWEI_IS_LOW_POWER,
  PROC_COMM_HUAWEI_WPS_LED_ONOFF,
  PROC_COMM_HUAWEI_OLED_SLEEP,
  PROC_COMM_HUAWEI_POWER_OFF = 4, 
  PROC_COMM_HUAWEI_NUM_CMDS
}PROC_COMM_HUAWEI_CMD_TYPE;


/*IOCTRL������*/
typedef enum 
{
    OLED_LIGHT = 3,
    OLED_SLEEP,
    OLED_DIM,
    OLED_RESET,
    OLED_UPDATE,
    OLED_CONTRAST,
    OLED_BRIGHTNESS,
    OLED_POWER_ON,
    OLED_POWER_OFF,
    OLED_DEL_TIMER,/*delete the power on animation timer*/
    OLED_SYS_POWER_DOWN,
    OLED_INIT_IMAGE,
   
    OLED_POWER_OFF_CHARGING_START,/*restart power off charging animation*/
    OLED_POWER_OFF_CHARGING_STOP,/*stop power off charging animation*/
    OLED_GET_FRAME_BUFFER,
    OLED_WRITE_IMAGE,

    OLED_ALL_WHITE,
    OLED_ALL_BLACK,
	OLED_BATS_PASS,
	OLED_BATS_FAIL,
    OLED_CMD_MAX
}OLED_IOCTL_CMD;

/*****************************************************************************
 �� �� ��  : oledInit
 ��������  : oled��ʼ������
 �������  :
 �������  : None
 �� �� ֵ  : void
*****************************************************************************/
extern BSP_S32 oledInit(BSP_VOID);

/*****************************************************************************
 �� �� ��  : oledPwrOn
 ��������  : oled�ϵ纯��
 �������  :
 �������  : None
 �� �� ֵ  : void
*****************************************************************************/
extern BSP_VOID  oledPwrOn(BSP_VOID);

/*****************************************************************************
 �� �� ��  : oledPwrOff
 ��������  : oled�µ纯��
 �������  :
 �������  : None
 �� �� ֵ  : void
*****************************************************************************/
extern BSP_VOID oledPwrOff(BSP_VOID);
extern BSP_VOID DRV_OLED_POWER_OFF(BSP_VOID);


/*****************************************************************************
 �� �� ��  : oledReset
 ��������  : oled��λ����
 �������  :
 �������  : None
 �� �� ֵ  : void
*****************************************************************************/
extern BSP_VOID oledReset(BSP_VOID);

/*****************************************************************************
 �� �� ��  : oledLight
 ��������  : oled������Ļ��������oledSleep������Ӧ
 �������  :
 �������  : None
 �� �� ֵ  : void
*****************************************************************************/
extern BSP_VOID oledLight(BSP_VOID);

/*****************************************************************************
 �� �� ��  : oledSleep
 ��������  : oled��Ļ˯�ߺ�������oledLight������Ӧ
 �������  :
 �������  : None
 �� �� ֵ  : void
*****************************************************************************/
extern BSP_VOID oledSleep(BSP_VOID);

/*****************************************************************************
 �� �� ��  : oledDim
 ��������  : oled�Աȶ����ú�������setcontrast�������ƣ�c����ʵ�֣�A���ݲ�����
 �������  :
 �������  : None
 �� �� ֵ  : void
*****************************************************************************/
extern BSP_VOID oledDim(BSP_VOID);

/*****************************************************************************
 �� �� ��  : oledInitImage
 ��������  : oled��ʼ����̬����ͼƬ����
 �������  :
 �������  : None
 �� �� ֵ  : void
*****************************************************************************/
extern BSP_VOID oledInitImage(BSP_VOID);

/*****************************************************************************
 �� �� ��  : oledClear
 ��������  : oled��ָ��λ����������
 �������  :
 �������  : x\y�������ʼ��ַ��ƫ�Ƶ�ַ
 �� �� ֵ  : BSP_S32
*****************************************************************************/
extern BSP_S32 oledClear(BSP_U32 ulXStart, BSP_U32 ulYStart, BSP_U32 ulXOffset, BSP_U32 ulYOffset);

/*****************************************************************************
 �� �� ��  : oledUpdateDisplay
 ��������  : SD����Ƭ�汾������OLED��ʾ
 �������  : BOOL UpdateStatus
 �������  : �ޡ�
 ����ֵ��   ��
 ����:  �����ɹ���NV������ɺ���øú�����
        ����ʧ�ܺ���øú������
*****************************************************************************/
extern void oledUpdateDisplay(int UpdateStatus);


/*****************************************************************************
 �� �� ��  : oledClearWholeScreen
 ��������  : SD����Ƭ�汾������OLED����
 �������  : �� 
 �������  : �ޡ�
 ����ֵ��   ��
*****************************************************************************/
extern void oledClearWholeScreen();


/*****************************************************************************
 �� �� ��  : oledRefresh
 ��������  : oled��ָ��λ��ˢ������
 �������  :
 �������  : 
*          UINT32 ulXStart, UINT32 ulYStart������ʼ����
*          UINT32 ulXOffset, UINT32 ulYOffset������������ƫ����
*          UINT8 *pucBuffer������ʾ����
 �� �� ֵ  : void
*****************************************************************************/
extern BSP_S32 oledRefresh(BSP_U32 ulXStart, BSP_U32 ulYStart, BSP_U32 ulXOffset, BSP_U32 ulYOffset, UINT8 *pucBuffer);

#ifdef __KERNEL__
/*****************************************************************************
 �� �� ��  : balong_oled_ioctl
 ��������  : oled ioctl
 �������  :
*            int cmd --command ID
*            arg--para
 �������  : none
 �� �� ֵ  : BSP_S32
*****************************************************************************/
extern BSP_S32 balong_oled_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
#endif

/*****************************************************************************
 �� �� ��  : oledCharDisplay
 ��������  : oled�ַ���ʾ����
 �������  :
 *           Ҫ��ʾ�ַ�����ʼ����
 *           Ҫ��ʾ���ַ�
 �������  : none
 �� �� ֵ  : BSP_VOID
*****************************************************************************/
extern BSP_VOID oledCharDisplay(UINT8 ucX, UINT8 ucY, UINT8 *pucChar);

/*****************************************************************************
 �� �� ��  : oledCharDisplay
 ��������  : oled�ַ�����ʾ����
 �������  :
 *           Ҫ��ʾ�ַ�������ʼ����
 *           Ҫ��ʾ���ַ���
 �������  : none
 �� �� ֵ  : void
*****************************************************************************/
extern BSP_VOID oledStringDisplay(UINT8 ucX, UINT8 ucY, UINT8 *pucStr);

/*****************************************************************************
 �� �� ��  : oledPwrOnAniInstall
 ��������  : ��������ע��
 �������  :
 �������  : none
 �� �� ֵ  : void
*****************************************************************************/
extern BSP_VOID oledPwrOnAniInstall(BSP_VOID);

/*****************************************************************************
 �� �� ��  : oledPwrOffChgAniInstall
 ��������  : �ػ���綯��ע��
 �������  :
 �������  : none
 �� �� ֵ  : BSP_S32
*****************************************************************************/
extern BSP_S32 oledPwrOffChgAniInstall(BSP_VOID);


/*****************************************************************************
 �� �� ��  : tftPwrOn
 ��������  : tft�ϵ纯��
 �������  :
 �������  : None
 �� �� ֵ  : void
*****************************************************************************/
extern BSP_VOID  tftPwrOn(BSP_VOID);

/*****************************************************************************
 �� �� ��  : tftPwrOff
 ��������  : tft�µ纯��
 �������  :
 �������  : None
 �� �� ֵ  : void
*****************************************************************************/
extern BSP_VOID tftPwrOff(BSP_VOID);

/*****************************************************************************
 �� �� ��  : tftReset
 ��������  : tft��λ����
 �������  :
 �������  : None
 �� �� ֵ  : void
*****************************************************************************/
extern BSP_VOID tftReset(BSP_VOID);

/*****************************************************************************
 �� �� ��  : tftLight
 ��������  : tft������Ļ��������tftSleep������Ӧ
 �������  :
 �������  : None
 �� �� ֵ  : void
*****************************************************************************/
extern BSP_VOID tftLight(BSP_VOID);

/*****************************************************************************
 �� �� ��  : tftSleep
 ��������  : tft��Ļ˯�ߺ�������tftLight������Ӧ
 �������  :
 �������  : None
 �� �� ֵ  : void
*****************************************************************************/
extern BSP_VOID tftSleep(BSP_VOID);

/*****************************************************************************
 �� �� ��  : tftDim
 ��������  : tft�Աȶ����ú�������setcontrast�������ƣ�c����ʵ�֣�A���ݲ�����
 �������  :
 �������  : None
 �� �� ֵ  : void
*****************************************************************************/
//extern BSP_VOID tftDim(BSP_VOID);

/*****************************************************************************
 �� �� ��  : tftInitImage
 ��������  : tft��ʼ����̬����ͼƬ����
 �������  :
 �������  : None
 �� �� ֵ  : void
*****************************************************************************/
extern BSP_VOID tftInitImage(BSP_VOID);

/*****************************************************************************
 �� �� ��  : tftClear
 ��������  : tft��ָ��λ����������
 �������  :
 �������  : x\y�������ʼ��ַ��ƫ�Ƶ�ַ
 �� �� ֵ  : BSP_S32
*****************************************************************************/
extern BSP_S32 tftClear(BSP_U32 ulXStart, BSP_U32 ulYStart, BSP_U32 ulXOffset, BSP_U32 ulYOffset);

/*****************************************************************************
*  Function:  tftClearWholeScreen
*  Description: tft clear *
*  Called By:AP
*  Table Accessed:
*  Table Updated:
*  Input:
*         N/A
*  Output:
*         N/A
*  Return:
*         N/A
*****************************************************************************/
extern BSP_VOID tftClearWholeScreen(BSP_VOID);
#define DRV_TFT_CLEAR_WHOLE_SCREEN() tftClearWholeScreen()

/*****************************************************************************
 �� �� ��  : tftRefresh
 ��������  : tft��ָ��λ��ˢ������
 �������  :
 �������  : 
*          UINT32 ulXStart, UINT32 ulYStart������ʼ����
*          UINT32 ulXOffset, UINT32 ulYOffset������������ƫ����
*          UINT8 *pucBuffer������ʾ����
 �� �� ֵ  : void
*****************************************************************************/
extern BSP_S32 tftRefresh(BSP_U32 ulXStart, BSP_U32 ulYStart, BSP_U32 ulXOffset, BSP_U32 ulYOffset, UINT8 *pucBuffer);

#ifdef __KERNEL__
/*****************************************************************************
 �� �� ��  : balong_tft_ioctl
 ��������  : tft ioctl
 �������  :
*            int cmd --command ID
*            arg--para
 �������  : none
 �� �� ֵ  : BSP_S32
*****************************************************************************/
extern BSP_S32 balong_tft_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
#endif

/*****************************************************************************
 �� �� ��  : tftCharDisplay
 ��������  : tft�ַ���ʾ����
 �������  :
 *           Ҫ��ʾ�ַ�����ʼ����
 *           Ҫ��ʾ���ַ�
 �������  : none
 �� �� ֵ  : BSP_VOID
*****************************************************************************/
extern BSP_VOID tftCharDisplay(UINT8 ucX, UINT8 ucY, UINT8 *pucChar);

/*****************************************************************************
 �� �� ��  : tftCharDisplay
 ��������  : tft�ַ�����ʾ����
 �������  :
 *           Ҫ��ʾ�ַ�������ʼ����
 *           Ҫ��ʾ���ַ���
 �������  : none
 �� �� ֵ  : void
*****************************************************************************/
extern BSP_VOID tftStringDisplay(UINT8 ucX, UINT8 ucY, UINT8 *pucStr);
#define DRV_TFT_STRING_DISPLAY(ucX, ucY,pucStr)	tftStringDisplay(ucX,ucY,pucStr)

/*****************************************************************************
 �� �� ��  : tftPwrOffChgAniInstall
 ��������  : �ػ���綯��ע��
 �������  :
 �������  : none
 �� �� ֵ  : BSP_S32
*****************************************************************************/
extern BSP_S32 tftPwrOffChgAniInstall(BSP_VOID);
#if(FEATURE_ON == FEATURE_HUAWEI_VDF)
extern BSP_S32 tftPwrOnAniInstall(BSP_VOID);
#endif

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif /* __cplusplus */

#endif
