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

BSP_U32 g_E5_Version_Index = E5352s_INDEX;/*��׮Ϊ��OLED�汾*/

const E5_Product_version_info g_E5_Version_Info[]={
        {OLED,MEM_128M,SD_ENABLE_SIGNAL},
        {LED,MEM_128M,SD_ENABLE_SIGNAL},
        {OLED,MEM_128M,SD_ENABLE_SIGNAL},
        {TFT,MEM_128M,SD_ENABLE_SIGNAL}
};

/*****************************************************************************
* �� �� ��  : DrvUniformPMUInit
*
* ��������  : ���ݰ汾������PMU,LED�Ѿ���bootload.s������,OLEDΪ
                            Ĭ��̬,��ֻ������E588��TFT��ʹ�õ�DR1
*
* �������  : ��
* �������  : ��
*
* �� �� ֵ  :  ��
*
*
*****************************************************************************/
/*BSP_VOID DrvUniformPMUInit(BSP_VOID)
{
    
}*/

/*****************************************************************************
* �� �� ��  : DrvUniformScreenInit
*
* ��������  : ���ݰ汾��������(OLED/TFT)��������(LED)
*
* �������  : ��
* �������  : ��
*
* �� �� ֵ  :  ��
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
        oledPwrOn();                               /*TFT����׮*/
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

