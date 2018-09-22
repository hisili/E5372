/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_WDT.c
*
*   作    者 :  wangxuesong
*
*   描    述 :  WatchDog模块
*
*   修改记录 :  2009年4月7日  v1.00  wangxuesong  创建
                2009年5月21日 v1.00  wangxuesong  修改
    BJ9D00643:  没有初始化时调用去使能WDT接口，调用失败
    BJ9D00868:  WDT的去使能操作中没有清中断
*************************************************************************/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include "BSP.h"
#include <mach/balong_v100r001.h>
#include "BSP_DRV_WDT.h"

//#define WDT_REGBASE_ADDR  (CLK_REGBASE_0TO7+0x14)
#define ACPU_WDT_TIMER          7
#define CLK_REGBASE_0TO7		0x90002000
/*看门狗*/
#define WDT_REGBASE_ADDR 		(IO_ADDRESS(CLK_REGBASE_0TO7) + 0x14*ACPU_WDT_TIMER)

#define BSP_SW_WDT_PERIOD           (3000)
static struct task_struct * g_s32WDTLowTaskId;

BSP_BOOL g_bWDTInit = BSP_FALSE;
BSP_BOOL g_bWDTEnable = BSP_FALSE;
BSP_S32 WDT_LowTask(void)  //clean warning
{
    for(;;)
    {
        msleep(BSP_SW_WDT_PERIOD);

	 //printk("kick dog. \n");
        
        /* 如果计时器没有耗尽，喂狗*/
        (BSP_VOID)BSP_WDT_HardwareFeed(0);         
    }  

	return OK; //clean warning
}

BSP_S32 WDT_Init(BSP_VOID)
{    
    if(!g_s32WDTLowTaskId)
    {
        g_s32WDTLowTaskId =  kthread_run(WDT_LowTask, NULL, "tWDTLowTask");
        if(IS_ERR( g_s32WDTLowTaskId))
        {
            return ERROR;
        }
    }
    
    g_bWDTInit = BSP_TRUE;

    return OK;
}

BSP_S32 BSP_WDT_Enable(BSP_U8 u8WdtId)
{
    BSP_U32 readValueTmp;
    BSP_U32 u32Times = 100;
    BSP_U32 i = 0;

    if(g_bWDTEnable)
    {
        return OK;
    }

    /* Set up in periodic mode */
    BSP_REG_WRITE(WDT_REGBASE_ADDR,CLK_REGOFF_CTRL, CLK_DEF_DISABLE);
        
    /* Load Timer Reload value into Timer registers */
    BSP_REG_WRITE (WDT_REGBASE_ADDR,CLK_REGOFF_LOAD, 0x1d4c00);   

    /* 查询计数器使能是否生效 */
    do
    {
        BSP_REG_READ(WDT_REGBASE_ADDR,CLK_REGOFF_CTRL, readValueTmp);
        readValueTmp = readValueTmp&0x10;
        i++;
    }while((!readValueTmp)&&(i<u32Times));

    if(i == u32Times)
    {
        printk("write LoadValue error. \n");
        return ERROR;
    }
   
    BSP_REG_WRITE(WDT_REGBASE_ADDR,CLK_REGOFF_CTRL, CLK_DEF_ENABLE);
	
    g_bWDTEnable = BSP_TRUE;

    return OK;
}


/*****************************************************************************
* 函 数 名  : BSP_WDT_HardwareFeed
*
* 功能描述  : 重置看门狗计数寄存器（喂狗）
*
* 输入参数  : BSP_U8 u8WdtId :看门狗ID
* 输出参数  : 无
*
* 返 回 值  : OK:    操作成功
*             ERROR: 操作失败
* 其它说明  : 
*
*****************************************************************************/
BSP_S32 BSP_WDT_HardwareFeed(BSP_U8 wdtId)
{
	
    if(g_bWDTEnable == BSP_FALSE)
        return ERROR;
    
    //printk("feed dog. \n");
    /* Set up in periodic mode */
    BSP_REG_WRITE(WDT_REGBASE_ADDR,CLK_REGOFF_CTRL, CLK_DEF_DISABLE);
        
    /* Load Timer Reload value into Timer registers */
    BSP_REG_WRITE (WDT_REGBASE_ADDR,CLK_REGOFF_LOAD, 0xFFFFFFFF);

    BSP_REG_WRITE(WDT_REGBASE_ADDR,CLK_REGOFF_CTRL, CLK_DEF_ENABLE);    
    
    return OK;
}

EXPORT_SYMBOL(BSP_WDT_HardwareFeed);

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
BSP_S32 BSP_WDT_TimerReboot(void)
{
    //printk("feed dog. \n");
    /* Set up in periodic mode */
    BSP_REG_WRITE(WDT_REGBASE_ADDR,CLK_REGOFF_CTRL, CLK_DEF_DISABLE);
        
    /* Load Timer Reload value into Timer registers */
    BSP_REG_WRITE (WDT_REGBASE_ADDR,CLK_REGOFF_LOAD, 0x1);

    BSP_REG_WRITE(WDT_REGBASE_ADDR,CLK_REGOFF_CTRL, CLK_DEF_ENABLE);    
    
    return OK;
}
EXPORT_SYMBOL(BSP_WDT_TimerReboot);

