/**********************************************************************
* Copyright (C), 2006  Hisilicon Tech. Co., Ltd.                      *
* All Rights Reserved                                                 *
*                                                                     *
* File name: uniformDrvDrv.c                                                 *
*                                                                     *
* Description : uniform module.                                           *
*                                                                     *
*                                                                     *
* Version: v0.1                                                       *
*                                                                     *
* Date:  2011-12-28                                                    *
*                                                                     *
* History:      

**********************************************************************/


#include "BSP.h"
#include <linux/BSP_UniformDrv.h>
#include"balong_oled_drv.h"
#include "balong_led_drv.h"

#if (FEATURE_E5 == FEATURE_ON)

BSP_U32 g_E5_Version_Index = E5352s_INDEX;/*打桩为带OLED版本*/

const E5_Product_version_info g_E5_Version_Info[]={
        {OLED,MEM_128M,SD_ENABLE_SIGNAL},
        {LED,MEM_128M,SD_ENABLE_SIGNAL},
        {OLED,MEM_128M,SD_ENABLE_SIGNAL},
        {TFT,MEM_128M,SD_ENABLE_SIGNAL}
};

/*****************************************************************************
* 函 数 名  : DrvUniformPMUInit
*
* 功能描述  : 根据版本号配置PMU,LED已经在bootload.s中配置,OLED为
                            默认态,故只需配置E588的TFT屏使用的DR1
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  :  无
*
*
*****************************************************************************/
/*BSP_VOID DrvUniformPMUInit(BSP_VOID)
{
    
}*/

/*****************************************************************************
* 函 数 名  : DrvUniformScreenInit
*
* 功能描述  : 根据版本号配置屏(OLED/TFT)或者闪灯(LED)
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  :  无
*
*
*****************************************************************************/
BSP_VOID DrvUniformScreenInit (BSP_VOID)
{
    
    if(OLED == g_E5_Version_Info[g_E5_Version_Index].E5_Screen_Type)
    {
        oledPwrOn();
        oledClearWholeScreen();
        oledPwrOnAniInstall();
        printk("oled init success!");
        return;
    }
    else if(TFT == g_E5_Version_Info[g_E5_Version_Index].E5_Screen_Type)
    {
        oledPwrOn();                               /*TFT屏打桩*/
        oledClearWholeScreen();
        oledPwrOnAniInstall();
        printk("TFT init success!");
        return;
    }
    else
    {
        return;
    }
    
}

#endif

