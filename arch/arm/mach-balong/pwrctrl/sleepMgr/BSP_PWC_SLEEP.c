/*******************************************************************************
* Copyright (C), 2010-2011, HUAWEI Tech. Co., Ltd
*
* File name:
*                pwcSlee.c
* Description:
*                sleep managerment
*
* Author:        张世楚
* Date:          2011-09-20
* Version:       1.0
*
*
*
* History:
* Author:		张世楚
* Date:			2011-09-20
* Description:	Initial version
* Author:		刘永富
* Date:			2011-09-28
* Description:	修改文件结构
* Author:		王静
* Date:			2011-12-19
* Description:	增加V7R1接口
*******************************************************************************/
/*V3R2 CS f00164371*/
#include "generated/FeatureConfigDRV.h"

#if (defined(CHIP_BB_6920ES)||defined (CHIP_BB_6920CS))
#include <linux/linkage.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/interrupt.h>

#include <mach/balong_v100r001.h>
#if (defined(CHIP_BB_6920ES)||defined (CHIP_BB_6920CS))
#include <mach/pwrctrl/v7r1/BSP_PWC_SOCREG.h>
#include <mach/pwrctrl/v7r1/BSP_PWC_SLEEPASM.h>
#else
#include <mach/pwrctrl/v3r2/BSP_PWC_SOCREG.h>
#endif
#include <mach/pwrctrl/BSP_PWC_COMMON.h>
#include <mach/pwrctrl/BSP_PWC_SLEEP.h>
#include <mach/pwrctrl/BSP_PWC_SLEEPMNTN.h>
#include <mach/pwrctrl/BSP_PWC_SYSFUNC.h>
#include <mach/pwrctrl/BSP_PWC_SLEEPMGR.h>
#include <linux/timer.h>
#include <linux/ktime.h>
#include <linux/jiffies.h>

/*begin_added for hsic suspend and resume debug*/
#include <mach/gpio.h>
#include <product_config.h>
#define BALONG_GPIO_WIFI_RST 12
#define BALON_WIFI_RST BALONG_GPIO_0(BALONG_GPIO_WIFI_RST)
#define HIGH_LVL (1)
#define LOW_LVL (0)
extern void BCM43239_WIFI_PWRCTRL_RESUME(void);
extern void BCM43239_WIFI_PWRCTRL_SUSPEND(void);
extern BSP_U16 BSP_HwGetVerMain( BSP_VOID );

/*end_added for hsic suspend and resume debug*/



extern ST_PWC_SWITCH_STRU g_stPwrctrlSwitch;
extern void PWRCTRL_AsmWfi(void);
extern int PWRCTRL_CreatIdMap(void);

#if (defined(CHIP_BB_6920ES)||defined (CHIP_BB_6920CS))
extern void PWRCTRL_AsmAcpuCodeBegin(void);
extern void PWRCTRL_AsmAcpuSleep(void);  
extern void PWRCTRL_AsmAcpuMemoryCopy(void);
extern uint32_t dcd_synop_devconnect(void);
#ifndef PRODUCT_CFG_BUILD_SEPARATE
extern uint32_t NVM_Read(uint32_t ulId, void *pData, uint16_t usDataLen);
#endif
#else
extern void PWRCTRL_AsmDeepSleep(void);
extern void PWRCTRL_AsmLightSleep(void);
#endif
extern void DCD_MODIFY_PWRCLMP(void);
#if (FEATURE_E5 == FEATURE_ON)
extern void LcdReInit(void);
#endif
extern BSP_PWC_ENTRY g_stPwcSocpRestore;
PWC_SLEEP_COUNT g_stSleepCount;
#ifdef KERNEL_MANAGE_SUSPEND
int hsic_resume_flag = 1;
int hsic_delay_time = 70;/*ms*/

void change_hsic_flag(int value)
{
    hsic_resume_flag = value;
    printk("hsic_resume_flag = %d\n",hsic_resume_flag);
}

void change_hsic_time(int value)
{
    hsic_delay_time = value;
    printk("hsic_delay_time = %d\n",hsic_delay_time);
}

void hsic_resume_process(void)
{
    printk(KERN_DEBUG "HSIC_RESUME START !! \n");
    BCM43239_WIFI_PWRCTRL_RESUME();
    printk(KERN_DEBUG "HSIC_RESUME ok !! \n");
}

DEFINE_TIMER(hsic_timer,hsic_resume_process,0,0);
#endif

#if (defined (CHIP_BB_6920ES) || defined (CHIP_BB_6920CS))
/*系统控制器读写接口*/
void PWRCTRL_AarmWfiEn(void)
{
    BSP_REG_CLRBIT(0, PWR_SC_CARM_MOD, PWR_ACPU_WFI_EN);
}

void PWRCTRL_AarmSetWarmStartAddr(BSP_U32 u32Addr)
{
    BSP_REG_WRITE(0, PWR_SC_AARN_WARM_STAR, u32Addr);
}

/*****************************************************************************
 函 数 名  : pwrctrl_store_ip_reg
 功能描述  : 备份soc下电区ip的寄存器
 输入参数  : 无
 输出参数  : 无
 修改历史      :
  1.日    期   : 
    作    者   : 
    修改内容   : 新生成函数

*****************************************************************************/
BSP_VOID PWRCTRL_StoreIpReg(void)
{
#ifdef PRODUCT_CFG_CORE_TYPE_MODEM 
	BSP_IPF_DrxBakReg();
    (BSP_VOID)BSP_AXIDMAC_DrxBakReg();
    (BSP_VOID)BSP_IPCM_DrxBakReg();
    (BSP_VOID)BSP_CIPHER_DrxBakReg();
	(BSP_VOID)BSP_NANDF_DrxBakReg();
	(BSP_VOID)BSP_SOCP_DrxBakReg() ;
 #else
    //(BSP_VOID)BSP_MMC_DrxBakReg() ;
    //BSP_USB_DrxBakReg();
        
 #endif
}
/*****************************************************************************
 函 数 名  : pwrctrl_restore_ip_reg
 功能描述  : 恢复soc下电区ip的寄存器
 输入参数  : 无
 输出参数  : 无
 修改历史      :
  1.日    期   : 
    作    者   : 
    修改内容   : 新生成函数

*****************************************************************************/
BSP_VOID PWRCTRL_RestoreIpReg(void)
{
#if (FEATURE_E5 == FEATURE_ON)
    LcdReInit();
	if (NULL != g_stPwcSocpRestore.routine)
	{
		g_stPwcSocpRestore.routine();
	}
#endif
}

/*****************************************************************************
 函 数 名  : PWRCTRL_CheckIrqPendingStatus
 功能描述  : 检查中断状态
 输入参数  : 无
 输出参数  : 无
 修改历史      :
  1.日    期   :
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
BSP_S32 PWRCTRL_CheckIrqPendingStatus(void)
{
    BSP_U32 i;
    BSP_U32 gicIcdIsprRegValue = 0;
    unsigned long key = 0;

     local_irq_save(key);
    for (i = 0; i < CARM_GIC_ICDABR_NUM; i++)
    {
        gicIcdIsprRegValue = (unsigned int )PWRCTRL_REG32(CARM_GIC_ICDISPR_OFFSET(i));
        if (0 != gicIcdIsprRegValue)
        {
            break;
        }
    }

    if(CARM_GIC_ICDABR_NUM == i)
    {
		local_irq_restore(key);
        return BSP_OK;

    }
    local_irq_restore(key);
    return BSP_ERROR;

}

/*****************************************************************************
 函 数 名  : PWRCTRL_DisableAllGic
 功能描述  : 
 输入参数  : 无
 输出参数  : 无
 修改历史      :
  1.日    期   :
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
BSP_VOID PWRCTRL_DisableAllGic(void)
{
    BSP_U32 *gic1 = (BSP_U32 *)(STORE_ADDR_GIC_ENABLE);
	BSP_U32 *gic2 = (BSP_U32 *)(CARM_GIC_ICDISER);
	BSP_U32 *gic3 = (BSP_U32 *)(CARM_GIC_ICDICER);
    BSP_U32 i;
	
    for(i = 0; i < CARM_GIC_ICDABR_NUM; i++)
	{
		*gic1 = *gic2;
		*gic3 = 0xffffffff;
		gic1++;
		gic2++;
		gic3++;	
	}

}

/*****************************************************************************
 函 数 名  : PWRCTRL_EnableAllGic
 功能描述  : 
 输入参数  : 无
 输出参数  : 无
 修改历史      :
  1.日    期   :
    作    者   :
    修改内容   : 新生成函数

*****************************************************************************/
BSP_VOID PWRCTRL_EnableAllGic(BSP_VOID)
{
    BSP_U32 *gic1 = (BSP_U32 *)STORE_ADDR_GIC_ENABLE;
	BSP_U32 *gic2 = (BSP_U32 *)CARM_GIC_ICDISER;
    BSP_U32 i;
	
    for(i = 0; i < CARM_GIC_ICDABR_NUM; i++)
	{
		*gic2 = *gic1;
		gic1++;
		gic2++;
	}

}

#endif
unsigned long PWRCTRL_HisiBalongIoRemap(unsigned long phys_addr, size_t size)
{
	return (unsigned long)ioremap(phys_addr, size);
}

void PWRCTRL_HisiBalongIoUnmap(unsigned long virt_addr)
{
	iounmap((void *)virt_addr);
	return;
}


void PWRCTRL_ArmSleep(void)
{
#if ((defined (BOARD_SFT) && defined (VERSION_V3R2)) \
    || ((defined (BOARD_ASIC)||defined (BOARD_ASIC_BIGPACK)) \
    && (defined(CHIP_BB_6920ES)||defined (CHIP_BB_6920CS)))) || ((FEATURE_EPAD == FEATURE_ON) || (FEATURE_PHONE == FEATURE_ON))
#if ((FEATURE_EPAD == FEATURE_ON) || (FEATURE_PHONE == FEATURE_ON))
    PWRCTRL_AsmWfi();
#else
    PWRCTRL_AsmWfi();
	g_stSleepCount.u32WfiInTimes++;
#endif

#endif
       
}

void PWRCTRL_SleepInitial(void)
{
#if (defined (BOARD_SFT) && defined (VERSION_V3R2))    


    /*设置sram1零地址跳转位置 :0x400*/
    PWRCTRL_REG_WRITE32(ISRAM1_VA_BASE ,0xe3a0fb01);

   

    /*使能ACPU唤醒中断源*/
    PWRCTRL_REG_WRITE32(PWR_SC_AARM_WKUP_INT_EN,AARM_WKUP_SOURCE);


    memcpy((void *)ISRAM1_VA_SLEEP, PWRCTRL_AsmCodeBegin, PWC_ACPU_DEEPSLEEP_SZ);
#elif ((defined (CHIP_BB_6920ES) || defined (CHIP_BB_6920CS))\
			 && (defined (BOARD_ASIC)||defined (BOARD_ASIC_BIGPACK)))

	BSP_U32 ulTmp = BSP_ERROR;
    /*设Debug标识为0，区分A/M核*/
#ifndef PRODUCT_CFG_BUILD_SEPARATE
    ulTmp = NVM_Read(0xD10B, &g_stPwrctrlSwitch, sizeof(ST_PWC_SWITCH_STRU));
#endif
    if (BSP_OK != ulTmp)
    {
	    g_stPwrctrlSwitch.deepsleep = 0;
	    g_stPwrctrlSwitch.lightsleep = 1;
	}
	/*下面3个地址用于保存深睡过程中地址重映射参数*/
	BSP_REG_WRITE(0,STORE_REMAP_ADDR1,0);
	BSP_REG_WRITE(0,STORE_REMAP_ADDR2,0);	
	BSP_REG_WRITE(0,STORE_REMAP_ADDR3,0);
	/*将深睡代码copy到AXI MEMORY中*/
	memcpy((void *)ISRAM1_VA_SLEEP, PWRCTRL_AsmAcpuCodeBegin, PWC_ACPU_DEEPSLEEP_SZ);
#endif
}


void PWRCTRL_LightSleep(void)
{ 
#if (defined (BOARD_SFT) && defined (VERSION_V3R2))    

    PWRCTRLVOIDFUNCPTR acpu_lightsleep_ptr = NULL;
    
#ifdef __PWRCTRL_VERSION_CONTROL__
    {
      if(PWRCTRL_SWITCH_OFF == g_stPwrctrlSwitch.lightsleep)
      return;
    }
#endif



    acpu_lightsleep_ptr =(PWRCTRLVOIDFUNCPTR)((PWRCTRL_UINT32)(ISRAM1_VA_SLEEP) \
                          + (PWRCTRL_UINT32)(PWRCTRL_AsmLightSleep)-(PWRCTRL_UINT32)(PWRCTRL_AsmCodeBegin));
    

	local_irq_disable();

	local_fiq_disable();

    
    (*acpu_lightsleep_ptr)();
        

	local_fiq_enable();

	local_irq_enable();
#endif

}


BSP_S32  PWRCTRL_DeepSleep(BSP_U32 u32sleepmode)
{
	/*unsigned int  AcpuDebugFlg = 0; */
#ifdef __PWRCTRL_VERSION_CONTROL__
    {
      if(PWRCTRL_SWITCH_OFF == g_stPwrctrlSwitch.deepsleep)
      return OK;
    }
#endif

  
	
    unsigned long keyLock;
#if ((defined (CHIP_BB_6920ES) || defined (CHIP_BB_6920CS))\
		 && (defined (BOARD_ASIC)||defined (BOARD_ASIC_BIGPACK)))

	int phy_addrs,virt_addrs;
	BSP_U32 deepsleepmode;
#endif	
    local_irq_save(keyLock);

#if (defined (BOARD_SFT) && defined (VERSION_V3R2))

	unsigned int value = 0;

	/*关GIC中断*/
	//PWRCTRL_REG_WRITE32(PBXA9_GIC_CPU_CONTROL_VA,0x0)


	
    /*去使能ACPU中断唤醒屏蔽*/
	//PWRCTRL_REG_WRITE32( PWR_SC_AARM_WKUP_INT_EN, PWR_SC_AARM_WKUP_INT_MASK);


	/*备份ACPU专有外设模块寄存器到SRAM1*/
	PWRCTRL_REG_READ32(PWR_SC_PERIPH_CLKSTAT3, value);
	value  &= (PWR_SC_ACPU_SET_REG_MASK);
	PWRCTRL_REG_WRITE32(DSARM1_ACPU_SET_REG_SAVE ,value);

	/*关闭ACPU专有外设时钟、EDMAC DMA外设AXI时钟、*/
	PWRCTRL_SET_BIT32(PWR_SC_PERIPH_CLKDIS3, PWC_SC_ACPU_ZDMAC_CLCK |PWC_SC_ACPU_ZSP_AHB_CLCK\
				|PWC_SC_ACPU_ADMAC_CLCK|PWC_SC_ACPU_EDMAC_ACLCK1|PWC_SC_ACPU_EDMAC_ACLCK0);

	


	PWRCTRL_AsmDeepSleep();	

	/*开GIC中断*/
	//PWRCTRL_REG_WRITE32(PBXA9_GIC_CPU_CONTROL_VA,0x1)
	DCD_MODIFY_PWRCLMP();
		
        
#elif ((defined (CHIP_BB_6920ES) || defined (CHIP_BB_6920CS))\
		 && (defined (BOARD_ASIC)||defined (BOARD_ASIC_BIGPACK)))
	deepsleepmode = u32sleepmode;

    if ((deepsleepmode < DEEPSLEEP_TYPE_CCPU) || (deepsleepmode > DEEPSLEEP_TYPE_TOGETHER))
    {
        printk("Parameter invalided ,please check it  ... \n");
        return ERROR;
    }
	if (BSP_OK == PWRCTRL_CheckIrqPendingStatus()) /*中断状态查询*/
    {   
    	/*屏蔽所有中断*/
	    PWRCTRL_DisableAllGic();

		/*begin_added for hsic suspend and resume debug*/
#if (FEATURE_E5 == FEATURE_ON)	
#ifdef KERNEL_MANAGE_SUSPEND 
        /*wifi_rst引脚 高电平，wifi未关闭，进行suspend*/
		if (HIGH_LVL == gpio_get_value(BALON_WIFI_RST))
		{

			BCM43239_WIFI_PWRCTRL_SUSPEND();
		}
#endif
#endif
		/*end_added for hsic suspend and resume debug*/
		
    	/*保存SOC外设寄存器*/
		if(g_stPwrctrlSwitch.drxPeriPd && (deepsleepmode == 1))
		{
			BSP_REG_WRITE(0, STORE_PERI_PD_FLAG, deepsleepmode);
			PWRCTRL_StoreIpReg();
		}
        
        /*提前做虚实地址映射，将深睡的AXI的那段代码的虚拟地址映射成跟实际地址相同*/
		phy_addrs = PWRCTRL_CreatIdMap();
		virt_addrs = __phys_to_virt(phy_addrs);
		BSP_REG_WRITE(0,STORE_REMAP_ADDR1,virt_addrs);
    	/*?	App Cpu备份GIC寄存器、ARM协处理器寄存器和各模式下的寄存器到片上SRAM*/		
        PWRCTRL_AsmAcpuSleep(); 
		
	    /*App ARM恢复的App ARM专有外设模块的寄存器*/
		if(g_stPwrctrlSwitch.drxPeriPd && (deepsleepmode == 1))
		{
       		PWRCTRL_RestoreIpReg();
			BSP_REG_WRITE(0, STORE_PERI_PD_FLAG, 0);
		}

		/*begin_added for hsic suspend and resume debug*/
#if (FEATURE_E5 == FEATURE_ON)	
#ifdef KERNEL_MANAGE_SUSPEND			
			/*wifi_rst引脚 高电平，wifi未关闭，进行suspend*/
			if (HIGH_LVL == gpio_get_value(BALON_WIFI_RST))
			{
			    if(1 == hsic_resume_flag)
                {
                    mod_timer(&hsic_timer,jiffies + msecs_to_jiffies(hsic_delay_time));
                }
				else
				{
				    BCM43239_WIFI_PWRCTRL_RESUME();
				}
				
			}
#endif
#endif
		/*end_added for hsic suspend and resume debug*/
		PWRCTRL_EnableAllGic();
	
		g_stSleepCount.u32DsInTimes++;

    }
    else
    {
        g_stSleepCount.u32DsOutTimes++;
    }
#endif
    local_irq_restore(keyLock);
		
}

BSP_VOID BSP_PWRCTRL_SleepCount(BSP_VOID)
{
	printk(" ACPU深睡次数: 0x%x, \n",g_stSleepCount.u32DsInTimes);
	printk(" ACPU浅睡次数: 0x%x, \n",g_stSleepCount.u32LsInTimes);
	printk(" ACPU wfi次数: 0x%x, \n",g_stSleepCount.u32WfiInTimes);
}

#if (defined(CHIP_BB_6920ES)||defined (CHIP_BB_6920CS))
/*****************************************************************************
* 函 数 名  : BSP_PWRCTRL_SetWakeSrc
*
* 功能描述  : 设置中断唤醒源接口
*
* 输入参数  : 
* 输出参数  : 
* 返 回 值   :
*
* 修改记录  : 

*****************************************************************************/
BSP_VOID BSP_PWRCTRL_SetWakeSrc(PWC_SLEEP_TYPE_E enSleepType)
{
    switch(enSleepType)
    {
        case 0:
         #ifdef PRODUCT_CFG_CORE_TYPE_MODEM
            /*使能唤醒中断源*/
            BSP_REG_WRITE(IO_ADDRESS(INTEGRATOR_SC_BASE),0xC3C, 0xffffffff);
            BSP_REG_WRITE(IO_ADDRESS(INTEGRATOR_SC_BASE),0xc40, 0xffffffff); 
            BSP_REG_WRITE(IO_ADDRESS(INTEGRATOR_SC_BASE),0xc44, 0xffffffff); 
            BSP_REG_WRITE(IO_ADDRESS(INTEGRATOR_SC_BASE),0xc54, 0xffffffff); 
            #else    
            BSP_REG_WRITE(IO_ADDRESS(INTEGRATOR_SC_BASE),0xc48, 0xffffffff);
            BSP_REG_WRITE(IO_ADDRESS(INTEGRATOR_SC_BASE),0xc4c, 0xffffffff); 
            BSP_REG_WRITE(IO_ADDRESS(INTEGRATOR_SC_BASE),0xc50, 0xffffffff); 
            BSP_REG_WRITE(IO_ADDRESS(INTEGRATOR_SC_BASE),0xc58, 0xffffffff); 
            #endif
          break;
         case 1:
         case 2:
		 case 3:
         case 4:
         default:
		 return;
    }
}

/*****************************************************************************
* 函 数 名  : BSP_PWRCTRL_ClkGateEn
*
* 功能描述  : 自动时钟门控使能接口
*
* 输入参数  : 
* 输出参数  : 
* 返 回 值   :
*
* 修改记录  : 

*****************************************************************************/
BSP_VOID BSP_PWRCTRL_ClkGateEn(BSP_VOID)
{
    BSP_U32 u32Tmp;
            
    BSP_REG_READ(IO_ADDRESS(INTEGRATOR_SC_BASE), 0x450, u32Tmp);
    u32Tmp |= PWR_CLK_GATE_MASK1;
    BSP_REG_WRITE(IO_ADDRESS(INTEGRATOR_SC_BASE), 0x450, u32Tmp);
            
    BSP_REG_READ(IO_ADDRESS(INTEGRATOR_SC_BASE), 0x454, u32Tmp);
    u32Tmp |= PWR_CLK_GATE_MASK2;
    BSP_REG_WRITE(IO_ADDRESS(INTEGRATOR_SC_BASE), 0x454, u32Tmp);
    /*打开Cipher自动门控使能*/
    BSP_REG_WRITEBITS(PWC_CIPHER_REGBASE_ADDR, 0x0,1 << 1);
    /*打开IPF自动门控*/
    BSP_REG_WRITE(PWC_IPF_REGBASE_ADR, 0x10,0xffffffff);    
    /*SOCP自动门控使能*/
    BSP_REG_WRITE(PWC_SOCP_REG_BASEADDR, 0x14,0xffffffff); 
}

/*****************************************************************************
* 函 数 名  : BSP_PWRCTRL_ClkGateDis
*
* 功能描述  : 自动时钟门控去使能接口
*
* 输入参数  : 
* 输出参数  : 
* 返 回 值   :
*
* 修改记录  : 

*****************************************************************************/
BSP_VOID BSP_PWRCTRL_ClkGateDis(BSP_VOID)
{
    BSP_U32 u32Tmp;
            
    BSP_REG_READ(INTEGRATOR_SC_BASE, 0x450, u32Tmp);
    u32Tmp &= (~PWR_CLK_GATE_MASK1);
    BSP_REG_WRITE(INTEGRATOR_SC_BASE, 0x450, u32Tmp);
            
    BSP_REG_READ(INTEGRATOR_SC_BASE, 0x454, u32Tmp);
    u32Tmp &= (~PWR_CLK_GATE_MASK2);
    BSP_REG_WRITE(INTEGRATOR_SC_BASE, 0x454, u32Tmp);
    /*Cipher自动门控去使能*/
    BSP_REG_CLRBITS(PWC_CIPHER_REGBASE_ADDR, 0x0, 1, 1);
    /*IPF自动门控去使能*/
    BSP_REG_WRITE(PWC_IPF_REGBASE_ADR, 0x10,0);       
    /*SOCP自动门控去使能*/
    BSP_REG_WRITE(PWC_SOCP_REG_BASEADDR, 0x14,0);
}

/*****************************************************************************
* 函 数 名  : BSP_PWRCTRL_TimerClose
*
* 功能描述  : SOC外设时钟关闭接口
*
* 输入参数  : 
* 输出参数  : 
* 返 回 值   :
*
* 修改记录  : 

*****************************************************************************/
BSP_S32 BSP_PWRCTRL_TimerClose(PWRCTRL_MODULE_E enModu)
{
	switch(enModu)
	{
		case PWRCTRL_MODU_BOOTROM:
		    BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS0, 1 << 21);  
		    break;
		case PWRCTRL_MODU_EFUSE:
		    BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS1, 1 << 21);  
		    break;
		case PWRCTRL_MODU_SSDMA:
		    BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS0, 1 << 15);  
		    break;
		case PWRCTRL_MODU_LBBP:
		    BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS1, 0x1f << 2);
		    break;
		case PWRCTRL_MODU_LDSP:
		    BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS0, 1 << 1);
		    break;
		case PWRCTRL_MODU_HSUART:
		    BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS0, 1 << 18);
		    break;
		case PWRCTRL_MODU_EMI:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS0, 1 << 19);
			break;
		case PWRCTRL_MODU_SDCC:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS0, 1 << 14);
			break;
		case PWRCTRL_MODU_USBHSIC:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS0, 1 << 13);
			break;
		case PWRCTRL_MODU_SOCP:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS0, 1 << 11);
			break;
		case PWRCTRL_MODU_GMAC:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS0, 1 << 10);
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS2, 0x7 << 26);
			break;	
		case PWRCTRL_MODU_SCI1:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS1, 1 << 30);
			break;
		case PWRCTRL_MODU_SCI0:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS1, 1 << 29);
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS1, 1 << 17);
			break;
		case PWRCTRL_MODU_UICC:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS1, 1 << 17);
			break;
		case PWRCTRL_MODU_USBOTG_BC:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS1, 1 << 27);
			break;
		case PWRCTRL_MODU_MMC0:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS1, 0x1 << 14);
			break;
		case PWRCTRL_MODU_MMC1:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS1, 0x1 << 15);
			break;
		case PWRCTRL_MODU_MMC2:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS1, 0x1 << 16);
			break;
		case PWRCTRL_MODU_USBOTG:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS1, 1 << 13);
			break;
		case PWRCTRL_MODU_WDT:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS1, 1 << 31);
			break;
		case PWRCTRL_MODU_TCSSI:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS1, 0x3 << 24);
			break;
		case PWRCTRL_MODU_IPF:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS0, 1 << 8);
			break;
		case PWRCTRL_MODU_CIPHER:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS0, 1 << 7);
			break;
		case PWRCTRL_MODU_BBPCOMM:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS3, 1 << 16);
			break;
		case PWRCTRL_MODU_TENSI:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS3, 1 << 0);
			break;
		case PWRCTRL_MODU_CORESIGHT:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS1, 1 << 18);
			break;
		case PWRCTRL_MODU_ACP:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS1, 1 << 9);
			break;
		case PWRCTRL_MODU_I2C:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS0, 1 << 27);
			break;
		case PWRCTRL_MODU_SPI1:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS0, 1 << 26);
			break;
		case PWRCTRL_MODU_DMAC:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS0, 1 << 9);
			break;
		case PWRCTRL_MODU_LSIO:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS3, 1 << 13);
			break;
		case PWRCTRL_MODU_HIFI:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS3, 1 << 12);
			break;
		case PWRCTRL_MODU_TSENSOR:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS1, 1 << 26);
			break;
		case PWRCTRL_MODU_CICOM:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS0, 0X3 << 22);
			break;
		case PWRCTRL_MODU_EDMA:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS0, 1 << 20);
			break;
		case PWRCTRL_MODU_HDLC:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS0, 1 << 16);
			break;
		case PWRCTRL_MODU_DM:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS0, 1 << 12);
			break;
		case PWRCTRL_MODU_DWSSI0:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS1, 1 << 28);
			break;
		case PWRCTRL_MODU_WGBBP:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS4, 0XFFFFFF);
			break;
		case PWRCTRL_MODU_ZSP:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS3, 0X3FF << 20);
			break;
		default:
			return BSP_ERROR;
	}
	return BSP_OK;
}
EXPORT_SYMBOL(BSP_PWRCTRL_TimerClose);

/*****************************************************************************
* 函 数 名  : BSP_PWRCTRL_TimerOpen
*
* 功能描述  : SOC外设时钟关闭接口
*
* 输入参数  : 
* 输出参数  : 
* 返 回 值   :
*
* 修改记录  : 

*****************************************************************************/
BSP_S32 BSP_PWRCTRL_TimerOpen(PWRCTRL_MODULE_E enModu)
{
	switch(enModu)
	{
		case PWRCTRL_MODU_BOOTROM:
		    BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN0, 1 << 21);  
		    break;
		case PWRCTRL_MODU_EFUSE:
		    BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN1, 1 << 21);  
		    break;
		case PWRCTRL_MODU_SSDMA:
		    BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN0, 1 << 15);  
		    break;
		case PWRCTRL_MODU_LBBP:
		    BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN1, 0x1f << 2);
		    break;
		case PWRCTRL_MODU_LDSP:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN0, 1 << 1);
			break;
		case PWRCTRL_MODU_HSUART:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN0, 1 << 18);
			break;
		case PWRCTRL_MODU_EMI:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN0, 1 << 19);
			break;
		case PWRCTRL_MODU_SDCC:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN0, 1 << 14);
			break;
		case PWRCTRL_MODU_USBHSIC:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN0, 1 << 13);
			break;
		case PWRCTRL_MODU_SOCP:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN0, 1 << 11);
			break;
		case PWRCTRL_MODU_GMAC:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN0, 1 << 10);
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN2, 0x7 << 26);
			break;	
		case PWRCTRL_MODU_SCI1:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN1, 1 << 30);
			break;
		case PWRCTRL_MODU_SCI0:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN1, 1 << 29);
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN1, 1 << 17);
			break;
		case PWRCTRL_MODU_UICC:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN1, 1 << 17);
			break;
		case PWRCTRL_MODU_USBOTG_BC:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN1, 1 << 27);
			break;
		case PWRCTRL_MODU_MMC0:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN1, 0x1 << 14);
			break;
		case PWRCTRL_MODU_MMC1:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN1, 0x1 << 15);
			break;
		case PWRCTRL_MODU_MMC2:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN1, 0x1 << 16);
			break;
		case PWRCTRL_MODU_USBOTG:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN1, 1 << 13);
			break;
		case PWRCTRL_MODU_WDT:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN1, 1 << 31);
			break;
		case PWRCTRL_MODU_TCSSI:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN1, 0xf << 22);
			break;
		case PWRCTRL_MODU_IPF:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN0, 1 << 8);
			break;
		case PWRCTRL_MODU_CIPHER:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN0, 1 << 7);
			break;
		case PWRCTRL_MODU_BBPCOMM:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN3, 1 << 16);
			break;
		case PWRCTRL_MODU_TENSI:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN3, 1 << 0);
			break;
		case PWRCTRL_MODU_CORESIGHT:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN1, 1 << 18);
			break;
		case PWRCTRL_MODU_ACP:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN1, 1 << 9);
			break;
		case PWRCTRL_MODU_I2C:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN0, 1 << 27);
			break;
		case PWRCTRL_MODU_SPI1:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN0, 1 << 26);
			break;
		case PWRCTRL_MODU_DMAC:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN0, 1 << 9);
			break;
		case PWRCTRL_MODU_LSIO:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN3, 1 << 13);
			break;
		case PWRCTRL_MODU_HIFI:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN3, 1 << 12);
			break;
		case PWRCTRL_MODU_TSENSOR:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN1, 1 << 26);
			break;
		case PWRCTRL_MODU_CICOM:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN0, 0X3 << 22);
			break;
		case PWRCTRL_MODU_EDMA:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN0, 1 << 20);
			break;
		case PWRCTRL_MODU_HDLC:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN0, 1 << 16);
			break;
		case PWRCTRL_MODU_DM:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN0, 1 << 12);
			break;
		case PWRCTRL_MODU_DWSSI0:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN1, 1 << 28);
			break;
		case PWRCTRL_MODU_WGBBP:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN4, 0XFFFFFF);
			break;
		case PWRCTRL_MODU_ZSP:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN3, 0X3FF << 20);
			break;
		default:
			return BSP_ERROR;
	}
	return BSP_OK;
}
EXPORT_SYMBOL(BSP_PWRCTRL_TimerOpen);
#endif
#endif

BSP_VOID BSP_PWRCTRL_SoftClkEn(PWRCTRL_MODULE_E enModu)
{
	switch(enModu)
	{
		case PWRCTRL_MODU_NANDC:
			/* 写0没影响，写1使能，所以可以 直接写 */
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN0, 1 << 17);  
			break;
		case PWRCTRL_MODU_EMI:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN0, 1 << 19);
			break;
		case PWRCTRL_MODU_I2C:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN0, 1 << 27);
			break;
		case PWRCTRL_MODU_MMC1:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN1, 1 << 15);
			break;
		case PWRCTRL_MODU_SSI0:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN1, 1 << 22);
			break;
		case PWRCTRL_MODU_SSI1:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN1, 1 << 23);
			break;
		case PWRCTRL_MODU_SSI2:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN1, 1 << 24);
			break;
		case PWRCTRL_MODU_SSI3:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN1, 1 << 25);
			break;		
		case PWRCTRL_MODU_SPI0:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN1, 1 << 28);
			break;
		case PWRCTRL_MODU_SCI0:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN1, 1 << 29);
			break;
		case PWRCTRL_MODU_SCI1:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN1, 1 << 30);
			break;	
		case PWRCTRL_MODU_WDT:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKEN1, 1 << 31);
			break;			
		default:
			printk("BSP_PWRCTRL_SoftClkEn modu error\n");
			return;
	}
}
    EXPORT_SYMBOL(BSP_PWRCTRL_SoftClkEn);

BSP_VOID BSP_PWRCTRL_SoftClkDis(PWRCTRL_MODULE_E enModu)
{
	switch(enModu)
	{
		case PWRCTRL_MODU_NANDC:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS0, 1 << 17);  
			break;
		case PWRCTRL_MODU_EMI:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS0, 1 << 19);
			break;
		case PWRCTRL_MODU_I2C:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS0, 1 << 27);
			break;
		case PWRCTRL_MODU_MMC1:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS1, 1 << 15);
			break;
		case PWRCTRL_MODU_SSI0:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS1, 1 << 22);
			break;
		case PWRCTRL_MODU_SSI1:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS1, 1 << 23);
			break;
		case PWRCTRL_MODU_SSI2:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS1, 1 << 24);
			break;
		case PWRCTRL_MODU_SSI3:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS1, 1 << 25);
			break;		
		case PWRCTRL_MODU_SPI0:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS1, 1 << 28);
			break;
		case PWRCTRL_MODU_SCI0:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS1, 1 << 29);
			break;
		case PWRCTRL_MODU_SCI1:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS1, 1 << 30);
			break;
		case PWRCTRL_MODU_WDT:
			BSP_REG_WRITE(0, PWR_SC_PERIPH_CLKDIS1, 1 << 31);
			break;		
		default:
			printk("BSP_PWRCTRL_SoftClkDis modu error\n");
			return;
	}
}
    EXPORT_SYMBOL(BSP_PWRCTRL_SoftClkDis);

BSP_U32 BSP_PWRCTRL_SoftClkStatus(PWRCTRL_MODULE_E enModu)
{
	BSP_U32 u32Ret = 0;
	switch(enModu)
	{
		case PWRCTRL_MODU_MST:
			BSP_REG_READ(0, PWR_SC_SCPERSTAT0, u32Ret);
			u32Ret = (u32Ret>>0)&0x1; 
			break;
		case PWRCTRL_MODU_NANDC:
			BSP_REG_READ(0, PWR_SC_SCPERSTAT0, u32Ret);
			u32Ret = (u32Ret>>17)&0x1; 
			break;
		case PWRCTRL_MODU_EMI:
			BSP_REG_READ(0, PWR_SC_SCPERSTAT0, u32Ret);
			u32Ret = (u32Ret>>19)&0x1; 
			break;
		case PWRCTRL_MODU_I2C:
			BSP_REG_READ(0, PWR_SC_SCPERSTAT0, u32Ret);
			u32Ret = (u32Ret>>27)&0x1; 
			break;
		case PWRCTRL_MODU_MMC1:
			BSP_REG_READ(0, PWR_SC_SCPERSTAT1, u32Ret);
			u32Ret = (u32Ret>>15)&0x1; 
			break;
		case PWRCTRL_MODU_SSI0:
			BSP_REG_READ(0, PWR_SC_SCPERSTAT1, u32Ret);
			u32Ret = (u32Ret>>22)&0x1; 
			break;
		case PWRCTRL_MODU_SSI1:
			BSP_REG_READ(0, PWR_SC_SCPERSTAT1, u32Ret);
			u32Ret = (u32Ret>>23)&0x1; 
			break;
		case PWRCTRL_MODU_SSI2:
			BSP_REG_READ(0, PWR_SC_SCPERSTAT1, u32Ret);
			u32Ret = (u32Ret>>24)&0x1; 
			break;
		case PWRCTRL_MODU_SSI3:
			BSP_REG_READ(0, PWR_SC_SCPERSTAT1, u32Ret);
			u32Ret = (u32Ret>>25)&0x1; 
			break;		
		case PWRCTRL_MODU_SPI0:
			BSP_REG_READ(0, PWR_SC_SCPERSTAT1, u32Ret);
			u32Ret = (u32Ret>>28)&0x1; 
			break;
		case PWRCTRL_MODU_SCI0:
			BSP_REG_READ(0, PWR_SC_SCPERSTAT1, u32Ret);
			u32Ret = (u32Ret>>29)&0x1; 
			break;
		case PWRCTRL_MODU_SCI1:
			BSP_REG_READ(0, PWR_SC_SCPERSTAT1, u32Ret);
			u32Ret = (u32Ret>>30)&0x1; 
			break;
		case PWRCTRL_MODU_WDT:
			BSP_REG_READ(0, PWR_SC_SCPERSTAT1, u32Ret);
			u32Ret = (u32Ret>>31)&0x1; 
			break;
		default:
			printk("BSP_PWRCTRL_SoftClkStatus modu error\n");
			return u32Ret;
		return u32Ret;
	}
}
EXPORT_SYMBOL(BSP_PWRCTRL_SoftClkStatus);
