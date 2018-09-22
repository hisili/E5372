/*******************************************************************************
* Copyright (C), 2010-2011, HUAWEI Tech. Co., Ltd
*
* File name:
*                pwcCommon.c
* Description:
*                sleep managerment
*
* Author:   	濫艪
* Date:          2011-09-28
* Version:       1.0
*
*
*
* History:
* Author:	濫艪
* Date:			2011-09-28
* Description:	Initial version
*
*******************************************************************************/
#include <linux/kernel.h>
#include <linux/semaphore.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/kthread.h>

//#define MEMORY_TEST_SIZE  (0x100)
unsigned int MEMORY_TEST_SIZE = 0x200; /*512/1024*/

unsigned int dfs_memory_test1 = 100;
unsigned int dfs_memory_test2 = 100;

//extern  unsigned int pwrctrl_dfs_get_acpuload(void);

void pwrctrl_change_memory_test_arg1(unsigned int arg)
{
	dfs_memory_test1 = arg;
	printk("  dfs_memory_test1 = %d \n",dfs_memory_test1);
	return;
}
void pwrctrl_change_memory_test_arg2(unsigned int arg)
{
	dfs_memory_test2 = arg;
	printk("  dfs_memory_test2 = %d \n",dfs_memory_test2);
	return;
}
void pwrctrl_change_memory_test_arg3(unsigned int arg)
{
	MEMORY_TEST_SIZE = arg;
	printk("  dfs_memory_test2 = %d \n",MEMORY_TEST_SIZE);
	return;
}


void pwrctrl_acpu_memory_test()
{
	unsigned int xValue = 0;
	unsigned int *tempAddr = NULL;
	unsigned int *ulAddr = NULL;

	printk("    start pwrctrl_acpu_memory_test \n");
	ulAddr = (unsigned int *)kmalloc(MEMORY_TEST_SIZE*sizeof(unsigned int),0);
	tempAddr = ulAddr;
	if(ulAddr==NULL)
	{
		printk("apply Malloc for ulAddr failed \n");
		return;
	}

	while(1)
	{
		
		if((ulAddr - tempAddr)<=MEMORY_TEST_SIZE)
		{
			*ulAddr = xValue;
			xValue++;
			ulAddr++;
			//printk(" xValue = %d, ulAddr = %d, tempAddr = %d  \n", xValue,ulAddr,tempAddr);
		}
		else
		{
			msleep(dfs_memory_test1);
            msleep(dfs_memory_test2);
			ulAddr = tempAddr;
			xValue = 0;
			continue;
		}
	//result = pwrctrl_dfs_get_acpuload();
	//printk(" curr cpu load = %d, dfs_memory_test = %d  \n", result,dfs_memory_test);
	}
	kfree(ulAddr);
	printk("    start pwrctrl_acpu_memory_test \n");
	return;
}
void pwrctrl_acpu_memory_test_task(void)
{
	 //task_struct * pwrctrl_dfs_acpu_task = NULL;
    /*
    pthread_attr_t attr;
    pthread_t  ntd;
　　struct sched_param param;

　　pthread_attr_init(&attr);

　　pthread_attr_setschedpolicy(&attr, SCHED_RR);

　　param.sched_priority = 10;

　　pthread_attr_setschedparam(&attr, &param);

　　pthread_create(ntd , &attr , pwrctrl_acpu_memory_test , NULL);
    */   


	 kthread_run(pwrctrl_acpu_memory_test, NULL, "pwrctrl_dfs_acpu_task");
	 //if(pwrctrl_dfs_acpu_task==NULL)
	 	//printk("creat pwrctrl_dfs_acpu_task fail \n");
	 //kernel_thread(pwrctrl_acpu_memory_test, 0, CLONE_VM);
	 return;
}
