/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_SCREEN.h
*
*   作    者 :  
*
*   描    述 :  IPF模块用户接口文件
*
*   修改记录 :  2012年04月16日  v1.00  创建
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


/*IOCTRL命令字*/
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
 函 数 名  : oledInit
 功能描述  : oled初始化函数
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/
extern BSP_S32 oledInit(BSP_VOID);

/*****************************************************************************
 函 数 名  : oledPwrOn
 功能描述  : oled上电函数
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/
extern BSP_VOID  oledPwrOn(BSP_VOID);

/*****************************************************************************
 函 数 名  : oledPwrOff
 功能描述  : oled下电函数
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/
extern BSP_VOID oledPwrOff(BSP_VOID);
extern BSP_VOID DRV_OLED_POWER_OFF(BSP_VOID);


/*****************************************************************************
 函 数 名  : oledReset
 功能描述  : oled复位函数
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/
extern BSP_VOID oledReset(BSP_VOID);

/*****************************************************************************
 函 数 名  : oledLight
 功能描述  : oled点亮屏幕函数，与oledSleep函数对应
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/
extern BSP_VOID oledLight(BSP_VOID);

/*****************************************************************************
 函 数 名  : oledSleep
 功能描述  : oled屏幕睡眠函数，与oledLight函数对应
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/
extern BSP_VOID oledSleep(BSP_VOID);

/*****************************************************************************
 函 数 名  : oledDim
 功能描述  : oled对比度设置函数，与setcontrast功能类似，c核有实现，A核暂不解释
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/
extern BSP_VOID oledDim(BSP_VOID);

/*****************************************************************************
 函 数 名  : oledInitImage
 功能描述  : oled初始化静态开机图片函数
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/
extern BSP_VOID oledInitImage(BSP_VOID);

/*****************************************************************************
 函 数 名  : oledClear
 功能描述  : oled在指定位置清屏函数
 输入参数  :
 输出参数  : x\y坐标的起始地址和偏移地址
 返 回 值  : BSP_S32
*****************************************************************************/
extern BSP_S32 oledClear(BSP_U32 ulXStart, BSP_U32 ulYStart, BSP_U32 ulXOffset, BSP_U32 ulYOffset);

/*****************************************************************************
 函 数 名  : oledUpdateDisplay
 功能描述  : SD卡烧片版本升级，OLED显示
 输入参数  : BOOL UpdateStatus
 输出参数  : 无。
 返回值：   无
 其它:  升级成功，NV备份完成后调用该函数打勾
        升级失败后调用该函数打叉
*****************************************************************************/
extern void oledUpdateDisplay(int UpdateStatus);


/*****************************************************************************
 函 数 名  : oledClearWholeScreen
 功能描述  : SD卡烧片版本升级，OLED清屏
 输入参数  : 无 
 输出参数  : 无。
 返回值：   无
*****************************************************************************/
extern void oledClearWholeScreen();


/*****************************************************************************
 函 数 名  : oledRefresh
 功能描述  : oled在指定位置刷屏函数
 输入参数  :
 输出参数  : 
*          UINT32 ulXStart, UINT32 ulYStart－－起始坐标
*          UINT32 ulXOffset, UINT32 ulYOffset－－横向纵向偏移量
*          UINT8 *pucBuffer－－显示数据
 返 回 值  : void
*****************************************************************************/
extern BSP_S32 oledRefresh(BSP_U32 ulXStart, BSP_U32 ulYStart, BSP_U32 ulXOffset, BSP_U32 ulYOffset, UINT8 *pucBuffer);

#ifdef __KERNEL__
/*****************************************************************************
 函 数 名  : balong_oled_ioctl
 功能描述  : oled ioctl
 输入参数  :
*            int cmd --command ID
*            arg--para
 输出参数  : none
 返 回 值  : BSP_S32
*****************************************************************************/
extern BSP_S32 balong_oled_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
#endif

/*****************************************************************************
 函 数 名  : oledCharDisplay
 功能描述  : oled字符显示函数
 输入参数  :
 *           要显示字符的起始坐标
 *           要显示的字符
 输出参数  : none
 返 回 值  : BSP_VOID
*****************************************************************************/
extern BSP_VOID oledCharDisplay(UINT8 ucX, UINT8 ucY, UINT8 *pucChar);

/*****************************************************************************
 函 数 名  : oledCharDisplay
 功能描述  : oled字符串显示函数
 输入参数  :
 *           要显示字符串的起始坐标
 *           要显示的字符串
 输出参数  : none
 返 回 值  : void
*****************************************************************************/
extern BSP_VOID oledStringDisplay(UINT8 ucX, UINT8 ucY, UINT8 *pucStr);

/*****************************************************************************
 函 数 名  : oledPwrOnAniInstall
 功能描述  : 开机动画注册
 输入参数  :
 输出参数  : none
 返 回 值  : void
*****************************************************************************/
extern BSP_VOID oledPwrOnAniInstall(BSP_VOID);

/*****************************************************************************
 函 数 名  : oledPwrOffChgAniInstall
 功能描述  : 关机充电动画注册
 输入参数  :
 输出参数  : none
 返 回 值  : BSP_S32
*****************************************************************************/
extern BSP_S32 oledPwrOffChgAniInstall(BSP_VOID);


/*****************************************************************************
 函 数 名  : tftPwrOn
 功能描述  : tft上电函数
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/
extern BSP_VOID  tftPwrOn(BSP_VOID);

/*****************************************************************************
 函 数 名  : tftPwrOff
 功能描述  : tft下电函数
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/
extern BSP_VOID tftPwrOff(BSP_VOID);

/*****************************************************************************
 函 数 名  : tftReset
 功能描述  : tft复位函数
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/
extern BSP_VOID tftReset(BSP_VOID);

/*****************************************************************************
 函 数 名  : tftLight
 功能描述  : tft点亮屏幕函数，与tftSleep函数对应
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/
extern BSP_VOID tftLight(BSP_VOID);

/*****************************************************************************
 函 数 名  : tftSleep
 功能描述  : tft屏幕睡眠函数，与tftLight函数对应
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/
extern BSP_VOID tftSleep(BSP_VOID);

/*****************************************************************************
 函 数 名  : tftDim
 功能描述  : tft对比度设置函数，与setcontrast功能类似，c核有实现，A核暂不解释
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/
//extern BSP_VOID tftDim(BSP_VOID);

/*****************************************************************************
 函 数 名  : tftInitImage
 功能描述  : tft初始化静态开机图片函数
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/
extern BSP_VOID tftInitImage(BSP_VOID);

/*****************************************************************************
 函 数 名  : tftClear
 功能描述  : tft在指定位置清屏函数
 输入参数  :
 输出参数  : x\y坐标的起始地址和偏移地址
 返 回 值  : BSP_S32
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
 函 数 名  : tftRefresh
 功能描述  : tft在指定位置刷屏函数
 输入参数  :
 输出参数  : 
*          UINT32 ulXStart, UINT32 ulYStart－－起始坐标
*          UINT32 ulXOffset, UINT32 ulYOffset－－横向纵向偏移量
*          UINT8 *pucBuffer－－显示数据
 返 回 值  : void
*****************************************************************************/
extern BSP_S32 tftRefresh(BSP_U32 ulXStart, BSP_U32 ulYStart, BSP_U32 ulXOffset, BSP_U32 ulYOffset, UINT8 *pucBuffer);

#ifdef __KERNEL__
/*****************************************************************************
 函 数 名  : balong_tft_ioctl
 功能描述  : tft ioctl
 输入参数  :
*            int cmd --command ID
*            arg--para
 输出参数  : none
 返 回 值  : BSP_S32
*****************************************************************************/
extern BSP_S32 balong_tft_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
#endif

/*****************************************************************************
 函 数 名  : tftCharDisplay
 功能描述  : tft字符显示函数
 输入参数  :
 *           要显示字符的起始坐标
 *           要显示的字符
 输出参数  : none
 返 回 值  : BSP_VOID
*****************************************************************************/
extern BSP_VOID tftCharDisplay(UINT8 ucX, UINT8 ucY, UINT8 *pucChar);

/*****************************************************************************
 函 数 名  : tftCharDisplay
 功能描述  : tft字符串显示函数
 输入参数  :
 *           要显示字符串的起始坐标
 *           要显示的字符串
 输出参数  : none
 返 回 值  : void
*****************************************************************************/
extern BSP_VOID tftStringDisplay(UINT8 ucX, UINT8 ucY, UINT8 *pucStr);
#define DRV_TFT_STRING_DISPLAY(ucX, ucY,pucStr)	tftStringDisplay(ucX,ucY,pucStr)

/*****************************************************************************
 函 数 名  : tftPwrOffChgAniInstall
 功能描述  : 关机充电动画注册
 输入参数  :
 输出参数  : none
 返 回 值  : BSP_S32
*****************************************************************************/
extern BSP_S32 tftPwrOffChgAniInstall(BSP_VOID);
#if(FEATURE_ON == FEATURE_HUAWEI_VDF)
extern BSP_S32 tftPwrOnAniInstall(BSP_VOID);
#endif

#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif /* __cplusplus */

#endif
