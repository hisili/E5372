#if(FEATURE_UPDATEONLINE == FEATURE_ON)

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/timer.h>
#include <linux/list.h>
#include <linux/skbuff.h>
#include <linux/netlink.h>
#include <linux/moduleparam.h>
#include <linux/mutex.h>
#include <linux/device.h>
#include <linux/wait.h>
#include <asm/uaccess.h>
#include <net/sock.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/netlink.h>
#include <linux/fs.h>
#include <linux/syscalls.h>

#include "os_platform.h"
#include "update_common.h"

void writeM_u32(UINT32 pAddr, UINT32 value)
{
    *(volatile UINT32 *)(pAddr)=value;
}

void  ImageTypeChange(int* pType)
{
    char logTmpBuf[LOG_BUF_LEN] = {0};
    UINT32 oldType = *pType;
        
    if( ZSP_FILE_ADDR == *pType )
    {
        *pType = IMAGE_ZSP;
    }
    else if( ISO_FILE_ADDR == *pType )
    {
        *pType = IMAGE_CDROMISO;
    }                
    else if( *pType >= IMAGE_PART_TOP )
    {
        if( MOBILE_CONNECT_HD_ADDR == *pType )
        {
            *pType = IMAGE_CDROMISOVER;
        }
        else if( MOBILE_CONNECT_ADDR == *pType )
        {
            *pType = IMAGE_CDROMISO;
        }
        else if( WEBUI_HD_ADDR == *pType )
        {
            *pType = IMAGE_WEBUIVER;
        }
        else if( WEBUI_ADDR == *pType )
        {
            *pType = IMAGE_WEBUI;
        }
        else
        {
            printk(KERN_INFO "imageTypeChange: image_type=0x%x.\n", (int)(*pType));
            memset((void*)logTmpBuf,LOG_BUF_LEN,0);
            snprintf(logTmpBuf,LOG_BUF_LEN, "%lu imageTypeChange: image_type=0x%x.\n",jiffies%100,(int)(*pType));
            update_record_file(logTmpBuf);
        }                             
    }
    else
    {
        printk("imageTypeChange: image_type=0x%x .\n", (int)(*pType));
        memset((void*)logTmpBuf,LOG_BUF_LEN,0);
        snprintf(logTmpBuf,LOG_BUF_LEN, "%lu imageTypeChange: image_type=0x%x.\n",jiffies%100,(int)(*pType));
        update_record_file(logTmpBuf);
    }

    printk(KERN_INFO "imageTypeChange: old_type=0x%x new_type=0x%x.\n", oldType,(int)(*pType));
    memset((void*)logTmpBuf,LOG_BUF_LEN,0);
    snprintf(logTmpBuf,LOG_BUF_LEN, "%lu imageTypeChange: old_type=0x%x new_type=0x%x.\n",jiffies%100,oldType,(int)(*pType));
    update_record_file(logTmpBuf);
 }

 /******************************************************************************
*  Function:  hi_TFUP_setTFUpdateFlag
*  Description:
*      set Update Flag for NORMAL update and LED control 
*
*  Calls:
*      
*
*  Called By:
*      TFUP_DloadTask
*      TFUP_CheckFileValidify
*
*  Data Accessed: 
*
*  Data Updated: 
*
*  Input:
*         flag: UPDATE STATE 1:设置标志位 0:清除标志位
*        
*  Output:
*         NONE
*         
*  Return:
*         OSAL_OK: the function successful returned.
*         OSAL_ERROR: the function encounter OSAL_ERROR while running.
*
********************************************************************************/
 int update_nv_backup_flag_set(int flag)
{
    if (NV_BACKUP == flag)  /*1置位*/
    {
        writeM_u32(TCM_FOR_UPDATE_NVBACKUP_ADDR, UPDATE_NVBACKUP_MODE);
        return OSAL_OK;
    }
    else if(NV_NO_BACKUP == flag)
    {
        writeM_u32(TCM_FOR_UPDATE_NVBACKUP_ADDR, UPDATE_NO_NVBACKUP_MODE);
        return OSAL_OK;
    }
    else
    {
        return OSAL_ERROR;
    }
}

 /******************************************************************************
*  Function:  update_record_file
*  Description:
*      Online Update record file
*

*  Data Accessed: 
*
*  Data Updated: 
*
*  Input:
*        traceLog:Log message string
*        
*  Output:
*         NONE
*         
*  Return: void
*      
*
********************************************************************************/
void update_record_file(char* traceLog)
{
    int fd = 0;
    int len = 0;
    mm_segment_t old_fs;
    old_fs = get_fs();
    set_fs(KERNEL_DS);
   
    /*若reset文件不存在，建立*/
    if( 0 > (fd = sys_open(UPDATE_FILE_LOG_PATH, O_WRONLY|O_CREAT|O_APPEND, 0)))
    {
        set_fs(old_fs);
        return ;
    }

    /*获取该文件长度*/
    if( 0 > (len = sys_lseek((uint32_t)fd, 0, SEEK_END)) )
    {
        printk(("update_record_file: sys_lseek file log error.\n"));
        sys_close(fd);
        set_fs(old_fs);

        return ;
    }
    else
    {
        printk(KERN_DEBUG"update_record_file: sys_lseek file is [%d].\n",(int)len);
    }

    if (len >= UPDATE_FILE_LOG_MAX )
    {
        printk(("update_record_file: log file is too big,recreate it.\n"));
        sys_close(fd);

        fd= 0;
        /*重新建立reset文件*/
        if( 0 > (fd = sys_open(UPDATE_FILE_LOG_PATH, O_WRONLY|O_CREAT|O_TRUNC, 0)))
        {
            printk(("update_record_file: sys_open log file error.\n"));
            set_fs(old_fs);
            return;
        }
    }

    if ( strlen(traceLog) != sys_write((unsigned int)fd,(const char *)traceLog, strlen(traceLog)))
    {
        printk(("update_record_file: sys_write Log file error.\n"));
    }
    
    sys_fsync((uint32_t)fd);
    sys_close((uint32_t)fd);
    set_fs(old_fs);
}
#endif

