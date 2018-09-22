/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_PMU_API.h
*
*   作    者 :  z67193
*
*   描    述 :  BSP类型定义头文件
*
*   修改记录 :  2012年04月14日  v1.00  z67193  创建
*************************************************************************/

#ifndef	__BSP_PMU_API_H__
#define __BSP_PMU_API_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/* 错误码 */
#define BSP_ERR_PMU_PARA_INVALID      BSP_DEF_ERR(BSP_MODU_SPI, 0x50)
#define BSP_ERR_PMU_ERROR_INIT        BSP_DEF_ERR(BSP_MODU_SPI, 0x51)	
#define BSP_ERR_PMU_SPI_NOT_INIT      BSP_DEF_ERR(BSP_MODU_SPI, 0x52)
#define BSP_ERR_PMU_NOT_INIT          BSP_DEF_ERR(BSP_MODU_SPI, 0x53)

#ifdef __VXWORKS__

/* PMU核间通信功能 */

/*****************************************************************************
* 函 数 名  : BSP_PMU_CtrlCb
*
* 功能描述  : 供A核调用的PMU控制函数
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_CtrlCb( BSP_VOID *pMsgBody,BSP_U32 u32Len);


/*****************************************************************************
* 函 数 名  : BSP_PMU_BuckOff
*
* 功能描述  : 关闭PMU的Buck
*
* 输入参数  : BuckID, Buck号，从1开始
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_BuckOff( BSP_U8 BuckID );

/*****************************************************************************
* 函 数 名  : BSP_PMU_BuckOn
*
* 功能描述  : 打开PMU的Buck
*
* 输入参数  : BuckID, Buck号，从1开始
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_BuckOn( BSP_U8 BuckID );

/*****************************************************************************
* 函 数 名  : BSP_PMU_DROff
*
* 功能描述  : DR打开关闭控制
*
* 输入参数  : DR_ID,DR编号,从1开始
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_DROff(BSP_U8 DR_ID);

/*****************************************************************************
* 函 数 名  : BSP_PMU_DROn
*
* 功能描述  : DR打开
*
* 输入参数  : DR_ID,DR编号,从1开始
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_DROn(BSP_U8 DR_ID);

/*****************************************************************************
* 函 数 名  : BSP_PMU_LDOVoltGet
*
* 功能描述  : 获取LDO通道的电压值
*
* 输入参数  : LdoID，LDO通道
*             pusVoltValue，电压值指针，等于实际电压乘以100
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_LDOVoltGet(BSP_U8 ucLDO);

/*****************************************************************************
* 函 数 名  : BSP_PMU_LDOVoltSet
*
* 功能描述  : 设置PMU LDO电压输出值
*
* 输入参数  : LdoID，LDO通道
*             vltg，电压值，等于实际电压乘以100
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_LDOVoltSet(BSP_U8 LdoID, BSP_U16 vltg);

/*****************************************************************************
* 函 数 名  : BSP_PMU_BuckVoltSet
*
* 功能描述  : 设置PMU BUCK电压输出值
*
* 输入参数  : buckID，BUCK通道
*             vltg，电压值，等于实际电压乘以100
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_BuckVoltSet(BSP_U8 buckID, BSP_U16 vltg);
/*****************************************************************************
* 函 数 名  : BSP_PMU_BuckVoltGet
*
* 功能描述  : 设置PMU BUCK电压输出值
*
* 输入参数  : buckID，BUCK通道
*
* 输出参数  : 
*
* 返 回 值  : 
* 返 回 值  : 电压值,等于实际电压乘以100
*               -1,错误
*
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_BuckVoltGet(BSP_U8 buckID);

/*******************************************************************************
  Function:     BSP_PMU_DrCurrentSet
  Description:  用于设置各路电流源的输出电流

  Input:        Vs_ID: PMU电流源编号，取值范围由PMU_ISRC定义：
                          DR1~DR3；
                ImA:      目标电流值（单位:mA），取值范围：
                          3mA,6mA,9mA,12mA,15mA,18mA,21mA,24mA
  Output:
  Return:       BSP_OK:    操作成功
                BSP_ERROR: 操作失败
  Others:
*******************************************************************************/
int BSP_PMU_DrCurrentSet(int I_ID, BSP_U16 ImA);

/*******************************************************************************
  Function:     BSP_PMU_DrCurrentGet
  Description:  用于查询各路电流源的输出电流

  Input:        I_ID: PMU电流源编号，取值范围 DR1~DR3 (0~2)
  Output:
  Return:       查询电流源的输出电流值（单位:mA）
                为负值（ERROR）表示查询失败
  Others:
*******************************************************************************/
int BSP_PMU_DrCurrentGet(int I_ID);

/*******************************************************************************
  Function:     BSP_PMU_RfAdjustVoltage
  Description:  Rf 调压接口

  Input:        enRfVol: 要调整的供电(1.8V/2.2V)
  			u32Voltage:要调成的电压值
  Output:
  Return:      返回0为调压成功，否则失败
  Others:
*******************************************************************************/
BSP_U32 BSP_PMU_RfAdjustVoltage(RF_VOLTAGE_E enRfVol, BSP_U32 u32Voltage);

#endif


/**************************************************************************
  全局变量声明
**************************************************************************/


/**************************************************************************
  枚举定义
**************************************************************************/
/*DVFS type*/
typedef enum tagPMU_DVFS_TYPE_E
{
	DVFS_ARMCORE = 0,
	DVFS_CEVACORE,
	DVFS_BUTTOM 
}PMU_DVFS_TYPE_E;

/*DVFS Voltage type*/
typedef enum tagPMU_DVFS_VOLTAGE_TYPE_E
{
	DVFS_VOLTAGE_1000 = 0,
	DVFS_VOLTAGE_1100,
	DVFS_VOLTAGE_1150,
	DVFS_VOLTAGE_1200,
	DVFS_VOLTAGE_1250,
	DVFS_VOLTAGE_1350,
	DVFS_VOLTAGE_1500,
	DVFS_VOLTAGE_1800,
    DVFS_VOLTAGE_BUTTOM 
}PMU_DVFS_VOLTAGE_TYPE_E;
	
 /*DEVICE type*/
typedef enum tagPMU_DEVS_TYPE_E
{
	DEVS_RF = 0,
	DEVS_ABB,
	DEVS_SD,
	DEVS_SIM,
	DEVS_LCD,
    DEVS_BUTTOM 
}PMU_DEVS_TYPE_E;

	
/*DEVICE CTRL type*/
typedef enum tagPMU_DEVS_CTRL_TYPE_E
{
	CTRL_ON = 0,
	CTRL_OFF,
    CTRL_BUTTOM 
}PMU_DEVS_CTRL_TYPE_E;

 /*OCP type*/
typedef enum tagPMU_OCP_TYPE_E
{
	OCP_ONLYINT_NOAUTOOFF = 0,
	OCP_INT_AND_AUTOOFF,
	OCP_AUTOOFF,
    OCP_BUTTOM 
}PMU_OCP_TYPE_E;

 /* VOLTAGE ADJUST REG */
typedef enum tagPMU_VOL_SCI_E
{
	PMU_VOL_UP = 0, /* 由1.8伏升级到3.0伏*/
	PMU_VOL_DOWN,   /* 由3.0伏降到1.8伏*/
	PMU_VOL_SCI_BUTTOM
}PMU_VOL_SCI_E;

/* LED MODE */
typedef enum tagPMU_LED_STATE_E
{
	PMU_LED_FLASH_POWER_OFF = 0,    /* 全部不闪 */
	PMU_LED_FLASH_BLUE_ON,          /* 蓝灯长亮 */
	PMU_LED_FLASH_BLUE_SLOW,        /* 蓝灯慢闪 */
	PMU_LED_FLASH_BLUE_FAST,        /* 蓝灯快闪 */
	PMU_LED_FLASH_BLUE_ONCE,        /* 蓝灯单闪 */
	PMU_LED_FLASH_BLUE_TWICE,       /* 蓝灯双闪 */
	PMU_LED_FLASH_BLUE_FOUR,        /* 蓝灯四闪 */
	
	PMU_LED_FLASH_RED_ON,           /* 红灯长亮 */
	PMU_LED_FLASH_RED_SLOW,         /* 红灯慢闪 */
	PMU_LED_FLASH_RED_FAST,         /* 红灯快闪 */
	PMU_LED_FLASH_RED_ONCE,         /* 红灯单闪 */
	PMU_LED_FLASH_RED_TWICE,        /* 红灯双闪 */
	PMU_LED_FLASH_RED_FOUR,         /* 红灯四闪 */
	
	PMU_LED_FLASH_GREEN_ON,         /* 绿灯长亮 */
	PMU_LED_FLASH_GREEN_SLOW,       /* 绿灯慢闪 */
	PMU_LED_FLASH_GREEN_FAST,       /* 绿灯快闪 */
	PMU_LED_FLASH_GREEN_ONCE,       /* 绿灯单闪 */
	PMU_LED_FLASH_GREEN_TWICE,      /* 绿灯双闪 */
	PMU_LED_FLASH_GREEN_FOUR,       /* 绿灯四闪 */
	
	PMU_LED_FLASH_CYANBLUE_ON,      /* 青灯长亮 */
	PMU_LED_FLASH_CYANBLUE_SLOW,    /* 青灯慢闪 */
	PMU_LED_FLASH_CYANBLUE_FAST,    /* 青灯快闪 */
	PMU_LED_FLASH_CYANBLUE_ONCE,    /* 青灯单闪 */
	PMU_LED_FLASH_CYANBLUE_TWICE,   /* 青灯双闪 */
	PMU_LED_FLASH_CYANBLUE_FOUR,    /* 青灯四闪 */

	PMU_LED_STATE_BUTT
}PMU_LED_STATE_E;


/*****************************************************************************
* 函 数 名  : BSP_PMU_ShutDown
*
* 功能描述  : 本接口用于关闭PMU
*
* 输入参数  : 
* 输出参数  : 无
*
* 返 回 值  :
* 修改记录  : 
*
*****************************************************************************/
BSP_VOID BSP_PMU_ShutDown(void);
/*****************************************************************************
* 函 数 名  : BSP_PMU_DVFS_VolAdjust
*
* 功能描述  : DVFS 电压调整
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  :
* 修改记录  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_DVFS_VolAdjust(PMU_DVFS_TYPE_E enDvfsType,PMU_DVFS_VOLTAGE_TYPE_E enVoltage);
/*****************************************************************************
* 函 数 名  : BSP_PMU_OCPConfig
*
* 功能描述  : OCPC配置
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  :
* 修改记录  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_OCPConfig(PMU_OCP_TYPE_E enOcpType);
/*****************************************************************************
* 函 数 名  : BSP_PMU_DEVS_Ctrl
*
* 功能描述  : DEVS　控制
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  :
* 修改记录  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_DEVS_Ctrl(PMU_DEVS_TYPE_E enDevsType, PMU_DEVS_CTRL_TYPE_E enCtrlType);
/*****************************************************************************
* 函 数 名  : BSP_PMU_VolAdjust_ForSci
*
* 功能描述  : SCI电压控制
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  :
* 修改记录  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_VolAdjust_ForSci(PMU_VOL_SCI_E enSCIVolCfg);
/*****************************************************************************
* 函 数 名  : BSP_PMU_VolAdjust_ForSDIO
*
* 功能描述  : SDIO电压控制
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  :
* 修改记录  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_VolAdjust_ForSDIO(PMU_VOL_SCI_E enSCIVolCfg);

/*****************************************************************************
* 函 数 名  : BSP_PMU_CutOff_ForDrx
*
* 功能描述  : 低功耗场景PMU下电
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  :
* 修改记录  : 正确/错误
*
*****************************************************************************/
BSP_S32 BSP_PMU_CutOff_ForDrx(void);

/*****************************************************************************
* 函 数 名  : BSP_PMU_SetLED
*
* 功能描述  : 设置LTD灯的开关
*
* 输入参数  : enLEDType         LED0
*                               LED1
*                               LED2
*             enPower           PMU_LED_POWER_ON
*                               PMU_LED_POWER_OFF
* 输出参数  : 无
*
* 返 回 值  : OK：配置成功
*             ERROR：配置失败
*****************************************************************************/
BSP_S32 BSP_PMU_SetLedState(PMU_LED_STATE_E eLedState);

/*****************************************************************************
 函 数 名  : BSP_PMU_GetPMUState
 功能描述  : 获取PMU模块开机方式、充电状态、电池电量、电池在位状态。
 输入参数  : 无。
 输出参数  : Pmu_State :开机方式、充电状态、电池电量、电池在位状态。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
 注意事项　：该接口仅在PS的PC工程中使用，目前没有应用，暂保留。
*****************************************************************************/
extern unsigned int BSP_PMU_GetPMUState(void* Pmu_State);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_PMUAPI_H__ */

