/*************************************************************************
*   版权所有(C) 1987-2010, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_ICC.c
*
*   作    者 :  zhouluojun
*
*   描    述 :  本文件主要完成共享内存通道管理，读写接口封装.
*
*   修改记录 :  2011年1月14日  v1.00  zhouluojun  创建
*************************************************************************/

#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/semaphore.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/kthread.h>
#include <linux/freezer.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <asm/io.h>
#include <mach/hardware.h>
#include "BSP.h"
#include "BSP_OSA.h"


#ifdef __cplusplus
extern "C" {
#endif


/*****************************************************************************
* 函 数 名  : BSP_ICC_Init
*
* 功能描述  : ICC初始化
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 
*****************************************************************************/
BSP_S32 OSA_CreateTask( BSP_CHAR * puchName,
                        BSP_U32 * pu32TaskID,
                        osa_task_entry pfnFunc,
                        BSP_U32 u32Priority,
                        BSP_U32 u32StackSize,
                        BSP_VOID * pParam)
{
    pid_t           pid;
    struct task_struct  *tsk;
    struct sched_param  param;

	tsk = kthread_run(pfnFunc, pParam, puchName);
	if (IS_ERR(tsk))
	{
		printk("create kthread icc_thread failed!\n");
		return -1;
	}

/*
    pid = kernel_thread(pfnFunc, pu32TaskID, CLONE_KERNEL);
    if (pid < 0) 
    {
        return BSP_ERROR;
    }

    tsk = pid_task(find_vpid(pid), PIDTYPE_PID);
    if ( BSP_NULL == tsk )
    {
        return BSP_ERROR;
    }

    if ( tsk->pid != pid )
    {
        printk("find task to set pri fail.\r\n");
        return BSP_ERROR;
    }
*/

    param.sched_priority = u32Priority;
    if (BSP_NULL != sched_setscheduler(tsk, SCHED_FIFO, &param))
    {
        printk("\r\nOSA_CreateTask: Creat Task %s ID %d sched_setscheduler Error", puchName, *pu32TaskID);
        return BSP_ERROR;
    }

    *pu32TaskID = (BSP_U32)tsk;
    
    return BSP_OK;
}




#ifdef __cplusplus
}
#endif



