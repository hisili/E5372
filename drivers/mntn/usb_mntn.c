/**********************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  usb_mntn.c
*
*   作    者 :  yudongbin
*
*   描    述 :  本文件命名为"usb_mntn.c"
*
*   修改记录 :  2011年9月19日  v1.00  yudongbin创建
************************************************************************************/

#include <linux/semaphore.h>    /*创建mutex所需的头文件*/
#include <linux/kernel.h>       /*   kmalloc,printk*/
#include <linux/slab.h>
#include "BSP.h"
#include "omDrv.h"
#include "usb_mntn.h"

/*整个SDT的内存*/
BSP_U32                  g_usb_mntn_func_type_on   = 0;
BSP_U32                  g_usb_mntn_init_flag      = 0;
BSP_U32                  g_drv_mntn_dbg_level      = 0x0;
BSP_U8                   *g_drv_mntn_buf           = NULL;
drv_mntn_info_header     *g_drv_mntn_info_header   = NULL;
struct semaphore         g_drv_mntn_mutex;

#if 0
BSP_U32 omTimerGet(void)
{
    return 0x0;
}
#else
extern unsigned int omTimerGet(void);
#endif

/***********************************************************************************
 Function:          get_gUsbMntnFuncTypeOnValue
 Description:       这个函数作为调试函数显示当前的值
 Calls:
 Input:             NA
 Output:            NA
 Return:            NA
 ************************************************************************************/
void get_gUsbMntnFuncTypeOnValue(void)
{
      printk("g_usb_mntn_func_type_on = %d\n",g_usb_mntn_func_type_on) ; 
}
/***********************************************************************************
 Function:          set_gUsbMntnFuncTypeOnValue
 Description:       这个函数作为调试函数设置当前的值
 Calls:
 Input:             NA
 Output:            NA
 Return:            NA
 ************************************************************************************/
void set_gUsbMntnFuncTypeOnValue(BSP_U32 value)
{
    g_usb_mntn_func_type_on = value;
    printk("g_usb_mntn_func_type_on = 0X%x\n",g_usb_mntn_func_type_on) ; 
}
/***********************************************************************************
 Function:          usb_mntn_info_init
 Description:       初始化USB定位信息模块，分配资源
 Calls:
 Input:             NA
 Output:            NA
 Return:            NA
 ************************************************************************************/
 void BSP_MNTN_DrvInfoInit(void)
{
    BSP_U32 offset_mem = 0x0;
	
#ifdef MNTN_DBG
    printk("\nGO INTO INIT DRV MNTN MODULE\n");
#endif

    if (1 == g_usb_mntn_init_flag)
    {
         printk("do not init again!\n");
         return;
    }
	
    /*把所有的要打到SDT上去的内存全部都申请好了*/	
    g_drv_mntn_buf = (BSP_U8*)kmalloc(DRV_MNTN_BUF_SIZE, GFP_KERNEL);
    if (NULL == g_drv_mntn_buf)
    {
        printk("\n kmalloc failed\n");
        goto ERR;
    }

    /*这是USB定位信息的头*/
    offset_mem = (sizeof(DRV_TRACE_IND_STRU) - 4);
    g_drv_mntn_info_header = (drv_mntn_info_header*)(g_drv_mntn_buf + offset_mem);
    
    g_drv_mntn_info_header->header_flag      = DRV_MNTN_HEADER_FLAG;
    g_drv_mntn_info_header->record_index     = 0x0;
    g_drv_mntn_info_header->time_stamp       = 0x0;
    g_drv_mntn_info_header->prim_id          = 0x0;
    g_drv_mntn_info_header->module_id        = 0x0;
    g_drv_mntn_info_header->attribute        = 0x0;
    g_drv_mntn_info_header->length           = 0x0;

    /*初始化互斥信号量*/
    sema_init(&g_drv_mntn_mutex, SEM_FULL);

    g_usb_mntn_func_type_on = DRV_MNTN_USB_AT | DRV_MNTN_USB_CTRL  | DRV_MNTN_USB_SCSI|
		                      DRV_MNTN_USB_NCM| DRV_MNTN_USB_MODEM | DRV_MNTN_USB_INT ;
    g_usb_mntn_init_flag = 1;
    g_drv_mntn_dbg_level = 1;
    return;
    
    ERR:
    BSP_MNTN_DrvInfoUnInit();    
}

/***********************************************************************************
 Function:          usb_mntn_info_uninit
 Description:       初始化USB定位信息模块，释放资源
 Calls:
 Input:             NA
 Output:            NA
 Return:            NA
 ************************************************************************************/
void BSP_MNTN_DrvInfoUnInit(void)
{
#ifdef MNTN_DBG
    printk("\n UNINIT DRV MNTN MODULE\n");
#endif
    if (NULL != g_drv_mntn_buf)
    {
        kfree(g_drv_mntn_buf);
    }
	
    g_usb_mntn_func_type_on = 0;
    g_usb_mntn_init_flag    = 0;
    g_drv_mntn_dbg_level    = 0;

    return;    
}

/***********************************************************************************
 Function:           BSP_MNTN_DrvInfoSave
 Description:        将搜集的定位信息拷贝至指定的
                     内存调用OM的回调函数输出
 Calls:              BSP_MNTN_OmDrvTraceSend 
 Input:              module_id
                     prim_id
                     buffer
                     reserved
 Output:             NA
 Return:             NA
 ************************************************************************************/
 void BSP_MNTN_DrvInfoSave( BSP_U16 module_id,
                            BSP_U16 prim_id,
                            void    *buffer,
                            void    *reserved)
{
    if(NULL == g_drv_mntn_buf)
    {
        printk("#####not init#######\n");
        return;
	}
    BSP_U32        record_valid = 0x0;
    static BSP_U16 record_index = 0x0;
#ifdef MNTN_DBG
    printk("######GO BSP_MNTN_DrvInfoSave ######\n");
#endif
    if (!g_usb_mntn_init_flag)
    {
        printk("########drv usb mntn not init####### \n");
        return;
    }

    if (NULL == buffer)
    {
        printk("#######buffer is null#######\n");
	    return;
    }

    down(&g_drv_mntn_mutex);
    switch (module_id)
    {
    case DRV_MODULE_ID_USB:
	{
#ifdef MNTN_DBG
        printk("#######now is in DRV_MODULE_ID_USB case#######\n");
#endif
        memset((void *)g_drv_mntn_info_header, 0, sizeof(drv_mntn_info_header));
		
		if (((PRIMID)prim_id >= DRV_PRIM_USB_REG) && ((PRIMID)prim_id <= DRV_PRIM_USB_RNDIS))
		{
            g_drv_mntn_info_header->module_id       =   DRV_MODULE_ID_USB;
		    g_drv_mntn_info_header->header_flag     =   DRV_MNTN_HEADER_FLAG;
		    g_drv_mntn_info_header->time_stamp      =   (unsigned short)omTimerGet();//pclint 734
		    g_drv_mntn_info_header->prim_id         =   prim_id;
		    g_drv_mntn_info_header->record_index    =   record_index++;
		    g_drv_mntn_info_header->length =((usb_mntn_info_t *)buffer)->length;
			 
		    if (g_drv_mntn_info_header->length > DRV_MNTN_USB_INFO_MAXSIZE)
		    {
                g_drv_mntn_info_header->length = DRV_MNTN_USB_INFO_MAXSIZE;
		    }

		    g_drv_mntn_info_header->attribute  = (BSP_U16)(((usb_mntn_info_t *)buffer)->attribute);

		    memcpy ((void *)(g_drv_mntn_info_header->data), 
				    (void *)(((usb_mntn_info_t *)buffer)->data), 
		   	                    g_drv_mntn_info_header->length);

		    record_valid = 1;
            
		}
		else
		{
            printk("invalid prim_id\n");
		}

		    break;
	 }
        default :
		    printk("now is not support!\n");
		    break;
    }

    if (record_valid)
    {
        BSP_MNTN_OmDrvTraceSend(prim_id, module_id, (BSP_U8 *)g_drv_mntn_info_header,
			                                            sizeof(drv_mntn_info_header));
    }
    else
    {
         printk("error invalid record \n ");
    }
	
    up(&g_drv_mntn_mutex);
#ifdef MNTN_DBG
    printk("######GO out BSP_MNTN_DrvInfoSave#######\n");
#endif
}