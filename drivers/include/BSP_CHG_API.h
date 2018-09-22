/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_CHG_API.h
*
*   作    者 :  z67193
*
*   描    述 :  BSP类型定义头文件
*
*   修改记录 :  2012年04月14日  v1.00  z67193  创建
*************************************************************************/

#ifndef	__BSP_CHG_API_H__
#define __BSP_CHG_API_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */

typedef enum USB_CHARGE_TYPE_ENUM_tag
{
    USB_CHARGE_TYPE_NON_CHECK = -1,          /* 还未判断出来连接线类型 */
    USB_CHARGE_TYPE_HW = 0,                      /* 华为充电器             */
    USB_CHARGE_TYPE_NON_HW =1                  /* 非华为充电器           */
}USB_CHARGE_TYPE_ENUM;

#define TBAT_CHECK_INVALID 0x0FFFF

/* 电池校准参数数据结构?**/
typedef struct
{
    BSP_U16 min_value;
    BSP_U16 max_value;
}VBAT_CALIBART_TYPE;

int app_get_battery_state(BATT_STATE_T *battery_state);


/*****************************************************************************
 函 数 名  : BSP_CHG_StateSet
 功能描述  :使能或者禁止充电
 输入参数  :ulState      0:禁止充电
 						1:使能充电
 输出参数  :无
 返回值：    无
*****************************************************************************/
extern void BSP_CHG_StateSet(unsigned long ulState);

/************************************************************************
 * FUNCTION
 *       int getCbcState(unsigned char *pucBcs ,unsigned char  *pucBcl)
 * DESCRIPTION
 *       pucBcs
            0: 电池在供电
            1: 与电池连接电池未供电
            2: 没有与电池连接
         pucBcl
            0:电量不足或没有与电池连接
             100: 剩余电量百分比
 * INPUTS
 *          
 * OUTPUTS: 
           pucBcs
           pucBcl
 * return:    0 Success, -1 failed     
 *******************************************************************/
int getCbcState(unsigned char  *pucBcs ,unsigned char   *pucBcl);
#define DRV_CBC_STATE_GET(pucbcs, pucbcl)   getCbcState(pucbcs, pucbcl)

/************************************************************************************
 *Function Name :  usb_charge_type   
 *Description   :  获取插入USB线的类型,判断是华为充电器,
                    还是非华为充电器(包括非华为充电器和普通PC机)
 *Others        :  No
************************************************************************************/
USB_CHARGE_TYPE_ENUM usb_charge_type( void );

/************************************************************************************
 *Function Name :  usb_enum_completed   
 *Description   :  判断USB的枚举是否完成,
                        完成返回(1),未完成返回(0)
 *Others        :  No
************************************************************************************/
unsigned int usb_enum_completed( void );

/************************************************************************
 * FUNCTION
 *       usb_get_charger_status
 * DESCRIPTION
 *       获取充电器的在位状态
 * INPUTS
 *       NONE
 * OUTPUTS
 *       在位则返回1，否则返回0。
 *************************************************************************/
unsigned int usb_get_charger_status(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_CHG_API_H__ */


