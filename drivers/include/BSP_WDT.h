/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_WDT.h
*
*   作    者 :  z67193
*
*   描    述 :  BSP类型定义头文件
*
*   修改记录 :  2012年04月14日  v1.00  z67193  创建
*************************************************************************/

#ifndef	__BSP_WDT_H__
#define __BSP_WDT_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


#define BSP_HW_WATCHDOG       0
#define BSP_SW_WATCHDOG       1

#define WDT_COUNT_DEFAULT     (0xf0000)



/*看门狗硬件信息数据结构*/
typedef struct tagWDT_ID_INFO_S
{
    BSP_U32 u32PartNum;
    BSP_U32 u32DesignerID;
    BSP_U32 u32Version;
    BSP_U32 u32Config;
    BSP_U32 u32PrimeCellID;
}WDT_ID_INFO_S;

typedef BSP_VOID(*wdt_timeout_cb)(void);


#if ((defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))&&(defined(BOARD_SFT)||defined(BOARD_ASIC)||defined(BOARD_ASIC_BIGPACK)))
/*****************************************************************************
* 函 数 名  : BSP_WDT_SetTime
*
* 功能描述  : 设置看门狗的超时时间
*
* 输入参数  : BSP_U8 u8WdtId:看门狗ID，芯片提供二个看门狗，但只使用一个，ID为0
*             WDT_TIMEOUT_E enTimeOut:看门狗超时间隔
* 输出参数  : 无
*
* 返 回 值  : OK:    操作成功
*             ERROR: 操作失败
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_WDT_SetTimeOut(BSP_U8 u8WdtId, BSP_U32 u32Value);
#else/*V7R1 p500,V3R2所有都会走下面这个分支*/
/*****************************************************************************
* 函 数 名  : BSP_WDT_SetTime
*
* 功能描述  : 设置看门狗的超时时间
*
* 输入参数  : BSP_U8 u8WdtId:看门狗ID，芯片提供二个看门狗，但只使用一个，ID为0
*             WDT_TIMEOUT_E enTimeOut:看门狗超时间隔
* 输出参数  : 无
*
* 返 回 值  : OK:    操作成功
*             ERROR: 操作失败
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_WDT_SetTimeOut(BSP_U8 u8WdtId, WDT_TIMEOUT_E enTimeOut);
/*****************************************************************************
* 函 数 名  : BSP_WDT_HardwareFeed_Force
* 
* 功能描述  : 重置看门狗计数寄存器（喂狗）
* 
* 输入参数  : wdtId  看门狗ID
* 
* 输出参数  : 无
* 
* 返 回 值  : OK& ERROR           
* 
* 修改记录 :  2009年3月5日  v1.00  wangxuesong  创建
*****************************************************************************/
BSP_S32 BSP_WDT_HardwareFeed_Force(BSP_U8 u8WdtId);
#endif

/*****************************************************************************
* 函 数 名  : BSP_WDT_Switch
* 
* 功能描述  : 切换喂狗方式
* 
* 输入参数  : wdtId    看门狗ID
*           feedMode 喂狗方式(0：驱动喂狗；1：软件喂狗)
*            
* 输出参数  : 无
* 返 回 值  : OK& ERROR  
* 
* 修改记录 :  2009年3月5日  v1.00  wangxuesong  创建
*****************************************************************************/
BSP_S32 BSP_WDT_Switch(BSP_U8 wdtId, BSP_U8 feedMode);

/*****************************************************************************
* 函 数 名  : BSP_WDT_IntInstall
* 
* 功能描述  : 在看门狗中断服务程序中注册超时处理函数
* 
* 输入参数  : wdtId  看门狗ID
*		    p      注册的函数指针
*            
* 输出参数  : 无
* 
* 返 回 值  : OK& ERROR  
* 
* 修改记录 :  2009年3月5日  v1.00  wangxuesong  创建
*****************************************************************************/
BSP_S32 BSP_WDT_IntInstall(BSP_U8 wdtId, BSP_VOID * p);

/*****************************************************************************
* 函 数 名  : BSP_WDT_IntInstall
* 
* 功能描述  : 在看门狗中断服务程序中卸载超时处理函数
* 
* 输入参数  : wdtId  看门狗ID
* 
* 输出参数  : 无
* 
* 返 回 值  : OK& ERROR  
* 
* 修改记录 :  2009年3月5日  v1.00  wangxuesong  创建
*****************************************************************************/
BSP_S32 BSP_WDT_IntUnInstall(BSP_U8 wdtId);

/*****************************************************************************
* 函 数 名  : BSP_WDT_TimeoutGet
* 
* 功能描述  : 获得看门狗剩余超时时间
* 
* 输入参数  : wdtId  看门狗ID
* 
* 输出参数  : 无
* 
* 返 回 值  : OK& ERROR  
* 
* 修改记录 :  2009年3月5日  v1.00  wangxuesong  创建
*****************************************************************************/
BSP_S32 BSP_WDT_TimeoutGet(BSP_U8 u8WdtId, BSP_U32 *u32TimeOut);

/*****************************************************************************
* 函 数 名  : BSP_WDT_reboot
*
* 功能描述  : 看门狗复位系统
*
* 输入参数  : BSP_VOID  
* 输出参数  : NA
*
* 返 回 值  : NA
*
* 其它说明  : 使用看门狗进行系统复位
*
*****************************************************************************/
BSP_VOID BSP_WDT_reboot(BSP_VOID);

/*****************************************************************************
* 函 数 名  : BSP_WDT_reboot_direct
*
* 功能描述  : 看门狗复位系统
*
* 输入参数  : BSP_VOID  
* 输出参数  : NA
*
* 返 回 值  : NA
*
* 其它说明  : 使用看门狗进行系统复位
*
*****************************************************************************/
BSP_VOID BSP_WDT_reboot_direct(BSP_VOID);

BSP_S32 WDT_Init(BSP_VOID);
BSP_S32 BSP_WDT_Feed(BSP_VOID);
BSP_VOID BSP_WDT_Feed_Force(BSP_VOID);

/*****************************************************************************
* 函 数 名  : BSP_WDT_TimerReboot
*
* 功能描述  : 通过设置timer中断，通知mcore重启。
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : OK:    操作成功
*             ERROR: 操作失败
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_WDT_TimerReboot(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_WDT_H__ */


