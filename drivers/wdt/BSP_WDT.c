/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_WDT.c
*
*   ��    �� :  wangxuesong
*
*   ��    �� :  WatchDogģ��
*
*   �޸ļ�¼ :  2009��4��7��  v1.00  wangxuesong  ����
                2009��5��21�� v1.00  wangxuesong  �޸�
    BJ9D00643:  û�г�ʼ��ʱ����ȥʹ��WDT�ӿڣ�����ʧ��
    BJ9D00868:  WDT��ȥʹ�ܲ�����û�����ж�
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
/*���Ź�*/
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
        
        /* �����ʱ��û�кľ���ι��*/
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

    /* ��ѯ������ʹ���Ƿ���Ч */
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
* �� �� ��  : BSP_WDT_HardwareFeed
*
* ��������  : ���ÿ��Ź������Ĵ�����ι����
*
* �������  : BSP_U8 u8WdtId :���Ź�ID
* �������  : ��
*
* �� �� ֵ  : OK:    �����ɹ�
*             ERROR: ����ʧ��
* ����˵��  : 
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
* �� �� ��  : BSP_WDT_TimerReboot
*
* ��������  : ͨ������timer�жϣ�֪ͨmcore������
*
* �������  : ��
* �������  : ��
*
* �� �� ֵ  : OK:    �����ɹ�
*             ERROR: ����ʧ��
* ����˵��  : 
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

