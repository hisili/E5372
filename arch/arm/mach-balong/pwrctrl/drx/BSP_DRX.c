/******************************************************************
 * Copyright (C), 2005-2011, HUAWEI Tech. Co., Ltd.
 *
 * File name:
 *
 * Description:
 *     power ctrl file.
 *
 * Date:       2012.01.01
 * Version:    v1.0
 *
 *
 * History:
 * Date:       2012.01.01
 * Discribe:   Initial
 *******************************************************************/

#ifdef PRODUCT_CFG_CORE_TYPE_MODEM
#include "product_config.h"
#include "BSP.h"
#include "BSP_PWC_SOCREG.h"
#include "BSP_DRX.h"
#include "BSP_PmuDrv.h"
#include "BSP_ABB.h"

BSP_VOID PWRCTRL_LbbpMtcmosOpen()
{
    BSP_REG_WRITEBITS(0, PWR_SC_PW_MTCMOS_CTRL, PWR_BBP_MTCMOS_MASK);
}

BSP_VOID PWRCTRL_LbbpMtcmosClose()
{
    BSP_REG_CLRBIT(0, PWR_SC_PW_MTCMOS_CTRL, PWR_BBP_MTCMOS_MASK);
}

BSP_VOID PWRCTRL_LbbpMtcmosStats(BSP_U32* tmp)
{
    BSP_REG_READ(0, PWR_SC_PW_STAT0, *tmp);
}

BSP_VOID PWRCTRL_LbbpIsoDisable()
{
    BSP_REG_CLRBIT(0, PWR_SC_PW_ISOEN, PWR_BBP_ISO_MASK);
}

BSP_VOID PWRCTRL_LbbpIsoEnable()
{
    BSP_REG_WRITEBITS(0, PWR_SC_PW_ISOEN, PWR_BBP_ISO_MASK);
}

BSP_VOID PWRCTRL_LbbpRstDisable()
{
    BSP_REG_CLRBIT(0, PWR_SC_RST_REQ2, PWR_BBP_SRST_MASK);
}

BSP_VOID PWRCTRL_LbbpRstEnable()
{
    BSP_REG_WRITEBITS(0, PWR_SC_RST_REQ2, PWR_BBP_SRST_MASK);
}

BSP_VOID PWRCTRL_LbbpClkEnable()
{
    BSP_REG_WRITEBITS(0, PWR_SC_PERIPH_CLKEN3,  PWR_BBP_CLOCK_MASK);
}

BSP_VOID PWRCTRL_LbbpClkDisable()
{    
    BSP_REG_WRITEBITS(0, PWR_SC_PERIPH_CLKDIS3,  PWR_BBP_CLOCK_MASK);
}

BSP_VOID PWRCTRL_LdspPllEnable()
{
    BSP_REG_CLRBIT(0, PWR_SC_DSP_DFS, PWR_DSP_PLL_EN);
}

BSP_VOID PWRCTRL_LdspPllDisable()
{
    BSP_REG_WRITEBITS(0, PWR_SC_DSP_DFS, PWR_DSP_PLL_EN);
}

BSP_VOID PWRCTRL_LbbpPllEnable()
{
    BSP_REG_CLRBIT(0, PWR_SC_BBP1_DFS, PWR_BBP_PLL_EN);
}

BSP_VOID PWRCTRL_LbbpPllDisable()
{
    BSP_REG_WRITEBITS(0, PWR_SC_BBP1_DFS, PWR_BBP_PLL_EN);
}

BSP_VOID PWRCTRL_LdspMtcmosOpen()
{
    BSP_REG_WRITEBITS(0, PWR_SC_PW_MTCMOS_CTRL, PWR_DSP_MTCMOS_MASK);
}

BSP_VOID PWRCTRL_LdspMtcmosClose()
{
    BSP_REG_CLRBIT(0, PWR_SC_PW_MTCMOS_CTRL, PWR_DSP_MTCMOS_MASK);
}

BSP_VOID PWRCTRL_LdspMtcmosStats(BSP_U32 tmp)
{
    BSP_REG_READ(0, PWR_SC_PW_STAT0, tmp);
}

BSP_VOID PWRCTRL_LdspPllStats(BSP_U32 tmp)
{
    BSP_REG_READ(0, PWR_SC_DSP_DFS, tmp);
}

BSP_VOID PWRCTRL_LdspIsoDisable()
{
    BSP_REG_CLRBIT(0, PWR_SC_PW_ISOEN, PWR_DSP_ISO_MASK);
}

BSP_VOID PWRCTRL_LdspIsoEnable()
{
    BSP_REG_WRITEBITS(0, PWR_SC_PW_ISOEN, PWR_DSP_ISO_MASK);
}

BSP_VOID PWRCTRL_LdspRstDisable()
{
    BSP_REG_CLRBIT(0, PWR_SC_RST_REQ2, PWR_DSP_SRST_MASK);
}
BSP_VOID PWRCTRL_LdspRegionRstDisable()
{
    BSP_REG_CLRBIT(0, PWR_SC_RST_REQ2, PWR_DSP_SRST_MASK_REGION);
}
BSP_VOID PWRCTRL_LdspRegionRstEnable()
{
    BSP_REG_WRITEBITS(0, PWR_SC_RST_REQ2, PWR_DSP_SRST_MASK_REGION);
}

BSP_VOID PWRCTRL_LdspRstEnable()
{
    BSP_REG_WRITEBITS(0, PWR_SC_RST_REQ2, PWR_DSP_SRST_MASK);
}

BSP_VOID PWRCTRL_LdspClkEnable()
{
    BSP_REG_WRITEBITS(0, PWR_SC_PERIPH_CLKEN3,  PWR_DSP_CLOCK_MASK);
}

BSP_VOID PWRCTRL_LdspClkDisable()
{    
    BSP_REG_WRITEBITS(0, PWR_SC_PERIPH_CLKDIS3,  PWR_DSP_CLOCK_MASK);
}

/************************************************************************
 * FUNCTION
 *       pwrctrl_bbp_powerup
 * DESCRIPTION
 *       bbp subsystem power up.
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
 *************************************************************************/
BSP_VOID   PWRCTRL_BbpPowerUp()
{
     BSP_U32  u32Stat;

#if 0
     /*具体控制哪个BBP需要与上层讨论*/
     PWRCTRL_LbbpMtcmosOpen();
     do
    {
        PWRCTRL_LbbpMtcmosStats(&u32Stat);
        u32Stat &= PWR_BBP_MTCMOS_MASK;
    } while (PWR_BBP_MTCMOS_MASK != u32Stat);
     
    PWRCTRL_LbbpIsoDisable();
    PWRCTRL_LbbpRstDisable();
#endif    
    PWRCTRL_LbbpClkEnable();
}

/************************************************************************
 * FUNCTION
 *       pwrctrl_bbp_powerdown
 * DESCRIPTION
 *       bbp subsystem power down.
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
 *************************************************************************/
BSP_VOID    PWRCTRL_BbpPowerDown()
{    
#if 0
    PWRCTRL_LbbpIsoEnable();
    PWRCTRL_LbbpRstEnable();
    PWRCTRL_LbbpClkDisable();
    PWRCTRL_LbbpMtcmosClose();
#endif
    PWRCTRL_LbbpClkDisable();

}

/************************************************************************
 * FUNCTION
 *       pwrctrl_rf_pwr_down
 * DESCRIPTION
 *       RF power down
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
 *************************************************************************/
BSP_VOID PWRCTRL_RfPowerDown()
{
    (BSP_VOID)BSP_PMU_DEVS_Ctrl(DEVS_RF,CTRL_OFF);
}

/************************************************************************
 * FUNCTION
 *       pwrctrl_rf_pwr_up
 * DESCRIPTION
 *       RF power up
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
 *************************************************************************/
BSP_VOID PWRCTRL_RfPowerUp()
{
    (BSP_VOID)BSP_PMU_DEVS_Ctrl(DEVS_RF,CTRL_ON);
}

/************************************************************************
 * FUNCTION
 *       pwrctrl_rf_pd_io_set
 * DESCRIPTION
 *       RF power down, set io pull-down
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
 *************************************************************************/
void pwrctrl_rf_pd_io_set(void)
{}

/************************************************************************
 * FUNCTION
 *       pwrctrl_rf_pu_io_set
 * DESCRIPTION
 *       RF power up, clr io pull-down
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
 *************************************************************************/
void pwrctrl_rf_pu_io_set(void)
{}

/************************************************************************
 * FUNCTION
 *       pwrctrl_wpa_pwr_down
 * DESCRIPTION
 *       W PA power down
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
 *************************************************************************/
void pwrctrl_lpa_pwr_down(void)
{
}

/************************************************************************
 * FUNCTION
 *       pwrctrl_wpa_pwr_up
 * DESCRIPTION
 *       W PA power up
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
 *************************************************************************/
void pwrctrl_lpa_pwr_up(void)
{
}
/************************************************************************
 * FUNCTION
 *       pwrctrl_LDsp_Pll_Enable
 * DESCRIPTION
 *       dsp power up
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
 *************************************************************************/
BSP_VOID PWRCTRL_DspPllEnable()
{
    BSP_U32 tmp;
    PWRCTRL_LdspPllEnable();
    //BSP_REG_CLRBIT(PWR_SYSCTRL_BASE, PWR_SC_DSP_DFS, PWR_DSP_PLL_EN);
  /*是否需要等待pll稳定*/
    do
    {
        BSP_REG_READ(0, PWR_SC_DSP_DFS, tmp);
        tmp &= (PWR_DSP_PLL_LOCK);
    } while (PWR_DSP_PLL_LOCK != tmp);

}

/************************************************************************
 * FUNCTION
 *       pwrctrl_dsp_pll_disable
 * DESCRIPTION
 *       dsp power down
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
 *************************************************************************/
BSP_VOID PWRCTRL_DspPllDisable()
{
    PWRCTRL_LdspPllDisable();
    //BSP_REG_WRITEBITS(PWR_SYSCTRL_BASE, PWR_SC_DSP_DFS, PWR_DSP_PLL_EN);
}

/*************************************************
  Function: pwrctrl_lbbp_pll_enable
  Description:  enable lbbp pll for drv.
  Input:   N/A
  Output:  N/A
  Return:  N/A.
           N/A.
  Others:  N/A
*************************************************/
BSP_VOID PWRCTRL_BbpPllEnable()
{
    BSP_U32 tmp;
    
    PWRCTRL_LbbpPllEnable();
    
    do
    {
        BSP_REG_READ(0, PWR_SC_BBP1_DFS, tmp);
        tmp &= (PWR_BBP_PLL_LOCK);
    } while (PWR_BBP_PLL_LOCK != tmp);
    
}

/*************************************************
  Function: pwrctrl_lbbp_pll_disable
  Description:  disable lbbp pll for drv.
  Input:   N/A
  Output:  N/A
  Return:  N/A.
           N/A.
  Others:  N/A
*************************************************/
BSP_VOID PWRCTRL_BbpPllDisable()
{
    PWRCTRL_LbbpPllDisable();
}


/************************************************************************
 * FUNCTION
 *       pwrctrl_ldsp_pwr_up
 * DESCRIPTION
 *       dsp power up
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
 *************************************************************************/
BSP_VOID PWRCTRL_DspPowerUp(void)
{
    BSP_U32  u32Stat;

    /*Com ARM配置系统控制器将处于上电区的DSP TCM退出DeepSleep状态；*/

	
    /*Com ARM配置系统控制器使能DSP PLL输出时钟；*/
    PWRCTRL_LdspPllEnable();

    /*Com ARM配置系统控制器打开DSP下电区的MTCMOS*/
    PWRCTRL_LdspMtcmosOpen();

    /*Com ARM读取系统控制器定时器等待DSP下电区电源电压稳定*/
	do
    {
        PWRCTRL_LdspMtcmosStats(u32Stat);
        u32Stat &= PWR_DSP_MTCMOS_MASK;
    } while (PWR_DSP_MTCMOS_MASK != u32Stat);
    /*Com ARM读取系统控制器定时器等待DSP PLL稳定*/
	do
    {
        PWRCTRL_LdspPllStats(u32Stat);
        u32Stat &= PWR_DSP_PLL_MASK;
    } while (PWR_DSP_PLL_MASK != u32Stat);

    /*Com ARM配置系统控制器退复位DSP下电区所有逻辑*/
    PWRCTRL_LdspRegionRstDisable();
	// BSP_REG_CLRBIT(PWR_SYSCTRL_BASE, PWR_SC_RST_REQ2, PWR_DSP_SRST_MASK_REGION);/*需要再确认是否都要复位*/

    /*Com ARM配置系统控制器去使能DSP下电区接口Isolation Cell*/
    PWRCTRL_LdspIsoDisable();

    /*Com ARM配置系统控制器打开DSP下电区时钟*/
    PWRCTRL_LdspClkEnable();

    /*Com ARM配置系统控制器对DSP解复位；*/
    PWRCTRL_LdspRstDisable();
}

/************************************************************************
 * FUNCTION
 *       pwrctrl_ldsp_pwr_down
 * DESCRIPTION
 *       dsp power down
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
 *************************************************************************/
BSP_VOID PWRCTRL_DspPowerDown()
{
    /*Com ARM配置系统控制器关闭DSP内核时钟*/
    PWRCTRL_LdspClkDisable();
	//BSP_REG_WRITEBITS(PWR_SYSCTRL_BASE, PWR_SC_PERIPH_CLKDIS3,  PWR_DSP_CLOCK_MASK);

    /*Com ARM配置系统控制器使能DSP下电区接口Isolation Cell*/
    PWRCTRL_LdspIsoEnable();
    //BSP_REG_WRITEBITS(PWR_SYSCTRL_BASE, PWR_SC_PW_ISOEN, PWR_DSP_ISO_MASK);
   
	/*Com ARM配置系统控制器退复位DSP下电区所有逻辑*/
    PWRCTRL_LdspRegionRstEnable();
	// BSP_REG_WRITEBITS(PWR_SYSCTRL_BASE, PWR_SC_RST_REQ2, PWR_DSP_SRST_MASK_REGION);
	/*Com ARM配置系统控制器对DSP复位；*/	
    PWRCTRL_LdspRstEnable();
    //BSP_REG_WRITEBITS(PWR_SYSCTRL_BASE, PWR_SC_RST_REQ2, PWR_DSP_SRST_MASK);

    /*Com ARM配置系统控制器使能DSP PLL输出时钟；*/
    PWRCTRL_LdspPllDisable();
    //BSP_REG_WRITEBITS(PWR_SYSCTRL_BASE, PWR_SC_DSP_DFS, PWR_DSP_PLL_EN);

    /*Com ARM配置系统控制器关闭DSP下电区的MTCMOS*/
    PWRCTRL_LdspMtcmosClose();
    //BSP_REG_CLRBIT(PWR_SYSCTRL_BASE, PWR_SC_PW_MTCMOS_CTRL, PWR_DSP_MTCMOS_MASK);

    /*Com ARM配置系统控制器将处于上电区的DSP TCM进入DeepSleep状态；*/

}

/************************************************************************
 * FUNCTION
 *       pwrctrl_abb_pwr_ctrl
 * DESCRIPTION
 *       ABB power control
 * INPUTS
 *       mode: ABB_PWR_UP/ABB_PWR_DOWN
 * OUTPUTS
 *       NONE
 *************************************************************************/
BSP_VOID PWRCTRL_AbbPllEnable()
{
    BSP_U32 u32Data;
    ABB_ReadData_Ssi(0x95,(BSP_U8 *)u32Data);
    u32Data |= (1<< 2);
    ABB_Write_Ssi(0x95,u32Data);
    ABB_Write_Ssi(0xA2,0x3);
}
BSP_VOID PWRCTRL_AbbPllDisable()
{
    ABB_Write_Ssi(0x95,0x0);
    ABB_Write_Ssi(0xA2,0x7f);
}

BSP_U32 BSP_PWRCTRL_PwrUp (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule)
{
    if((enCommMode < PWC_COMM_MODE_WCDMA) ||(enCommMode >= PWC_COMM_MODE_BUTT))
    {
        logMsg("BSP_PWRCTRL_PwrUp enCommMode param false. \n", 0, 0, 0, 0, 0, 0);
        return BSP_ERROR;

    }
    if((enCommModule < PWC_COMM_MODULE_PA)||( enCommModule >= PWC_COMM_MODULE_BUTT))
    {
        logMsg("BSP_PWRCTRL_PwrUp enCommModule param false. \n", 0, 0, 0, 0, 0, 0);
        return BSP_ERROR;

    }

    if(PWC_COMM_MODE_LTE == enCommMode)
    {
        switch(enCommModule)
        {
            case PWC_COMM_MODULE_PA:
                 break;
            case PWC_COMM_MODULE_RF:
                 PWRCTRL_RfPowerUp();
                 break;
            case PWC_COMM_MODULE_BBP_PWR:
                 PWRCTRL_BbpPowerUp();
                 break;                
            case PWC_COMM_MODULE_DSP:
                 PWRCTRL_DspPowerUp();
                 break;
            case PWC_COMM_MODULE_ABB:
                 break;
            default:
                 break;

        }
    }

    return BSP_OK;

}

/************************************************************************
 * FUNCTION
 *       BSP_PWRCTRL_PwrDown
 * DESCRIPTION
 *       peri power down control
 * INPUTS
 *       enCommMode,enCommModule
 * OUTPUTS
 *       NONE
 * RETURN 
 *       OK/ERROR     
 *************************************************************************/
BSP_U32 BSP_PWRCTRL_PwrDown (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E 
enCommModule)
{
    if((enCommMode < PWC_COMM_MODE_WCDMA) ||(enCommMode >= PWC_COMM_MODE_BUTT))
    {
        logMsg("BSP_PWRCTRL_PwrDown enCommMode param false. \n", 0, 0, 0, 0, 0, 0);
        return BSP_ERROR;

    }
    if((enCommModule < PWC_COMM_MODULE_PA)||( enCommModule >= PWC_COMM_MODULE_BUTT))
    {
        logMsg("BSP_PWRCTRL_PwrDown enCommModule param false. \n", 0, 0, 0, 0, 0, 0);
        return BSP_ERROR;

    }

    if(PWC_COMM_MODE_LTE == enCommMode)    
    {
        switch(enCommModule)
        {
            case PWC_COMM_MODULE_PA:
                 break;
            case PWC_COMM_MODULE_RF:
                 PWRCTRL_RfPowerDown();
                 break;
            case PWC_COMM_MODULE_BBP_PWR:
                 PWRCTRL_BbpPowerDown();
                 break;               
            case PWC_COMM_MODULE_DSP:
                 PWRCTRL_DspPowerDown();
                 break;
            case PWC_COMM_MODULE_ABB:
                 break;
            default:
                 break;

        }
    }

    return BSP_OK;

}


/************************************************************************
 * FUNCTION
 *       BSP_PWRCTRL_PllEnable
 * DESCRIPTION
 *       peri pll enable control
 * INPUTS
 *       enCommMode,enCommModule
 * OUTPUTS
 *       NONE
 * RETURN 
 *       OK/ERROR     
 *************************************************************************/

BSP_U32 BSP_PWRCTRL_PllEnable (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule)
{
    if((enCommMode < PWC_COMM_MODE_WCDMA) ||(enCommMode >= PWC_COMM_MODE_BUTT))
    {
        logMsg("BSP_PWRCTRL_PllEnable enCommMode param false. \n", 0, 0, 0, 0, 0, 0);
        return BSP_ERROR;

    }
    if((enCommModule < PWC_COMM_MODULE_PA)||( enCommModule >= PWC_COMM_MODULE_BUTT))
    {
        logMsg("BSP_PWRCTRL_PllEnable enCommModule param false. \n", 0, 0, 0, 0, 0, 0);
        return BSP_ERROR;

    }

    if(PWC_COMM_MODE_LTE == enCommMode)    
    {
        switch(enCommModule)
        {
            case PWC_COMM_MODULE_PA:
                 break;
            case PWC_COMM_MODULE_DSP:
                 PWRCTRL_DspPllEnable();
                 break;
            case PWC_COMM_MODULE_BBP_PWR:
                PWRCTRL_BbpPllEnable();
                break;
            case PWC_COMM_MODULE_ABB:
                PWRCTRL_AbbPllEnable();
                 break;
            default:
                 break;

        }
    }

    return BSP_OK;

}

/************************************************************************
 * FUNCTION
 *       BSP_PWRCTRL_PllDisable
 * DESCRIPTION
 *       peri pll dis able control
 * INPUTS
 *       enCommMode,enCommModule
 * OUTPUTS
 *       NONE
 * RETURN 
 *       OK/ERROR     
 *************************************************************************/

BSP_U32 BSP_PWRCTRL_PllDisable (PWC_COMM_MODE_E enCommMode, PWC_COMM_MODULE_E enCommModule)
{
    if((enCommMode < PWC_COMM_MODE_WCDMA) ||(enCommMode >= PWC_COMM_MODE_BUTT))
    {
        logMsg("BSP_PWRCTRL_PllDisable enCommMode param false. \n", 0, 0, 0, 0, 0, 0);
        return PWC_PARA_INVALID;

    }
    if((enCommModule < PWC_COMM_MODULE_PA)||( enCommModule >= PWC_COMM_MODULE_BUTT))
    {
        logMsg("BSP_PWRCTRL_PllDisable enCommModule param false. \n", 0, 0, 0, 0, 0, 0);
        return PWC_PARA_INVALID;

    }

    if(PWC_COMM_MODE_LTE == enCommMode)    
    {
        switch(enCommModule)
        {
            case PWC_COMM_MODULE_PA:
                 break;
            case PWC_COMM_MODULE_DSP:
                 PWRCTRL_DspPllDisable();
                 break;
            case PWC_COMM_MODULE_ABB:
                PWRCTRL_AbbPllDisable();
                 break;
            default:
                 break;

        }
    }

    return PWC_SUCCESS;

}
#endif

