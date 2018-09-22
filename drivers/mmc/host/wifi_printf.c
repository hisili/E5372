/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  wifiPrintf.c
*
*   作    者 :  yuanqinshun
*
*   描    述 :  本文件命名为"wifiPrintf.c"
*
*************************************************************************/

#include<linux/kernel.h>
#include "wifi_printf.h"
#include<linux/module.h>


int wifi_print_flag = 0;

/***********************************************************************************
 Function:          wifi_print_set
 Description:       set flag value
 Calls:
 Input:             new_val 
 Output:            NA
 Return:            NA
 ************************************************************************************/
 
void wifi_print_set(int new_val)
{
    printk("old status of wifi_print_set is %d \n",wifi_print_flag);
    wifi_print_flag = new_val;
    printk("new status of wifi_print_set is %d \n",wifi_print_flag);    

    return;
}

/***********************************************************************************
 Function:          wifi_printfWithModule
 Description:       wifi trace function
 Calls:
 Input:             pcformat -- 格式化字符串
 Output:            NA
 Return:            return value of OM_PRINT_WITH_MODE
 ************************************************************************************/
 
unsigned long wifi_printfWithModule(unsigned long ulModuleId,unsigned long ulLevel,char * pcformat,...)
{	
    unsigned int ret = 0;
    
    if( ulLevel <= (unsigned int)balong_sdio_trace_level)
    {   
        printk("[WIFI_PWM][ID=0x%x][lev=%d]\n",ulModuleId,ulLevel);
        /*
        printk(pcformat);
        */
    }                

    return ret;
}

/***********************************************************************************
 Function:          wifi_printf
 Description:       wifi trace function
 Calls:
 Input:             pcformat -- 格式化字符串
 Output:            NA
 Return:            return value of OM_PRINT
 ************************************************************************************/
int wifi_printf()
{
    printk("%s\n",__FUNCTION__);
    return 0;
}

EXPORT_SYMBOL(wifi_print_flag);

EXPORT_SYMBOL(wifi_print_set);

EXPORT_SYMBOL(wifi_printfWithModule);

EXPORT_SYMBOL(wifi_printf);
