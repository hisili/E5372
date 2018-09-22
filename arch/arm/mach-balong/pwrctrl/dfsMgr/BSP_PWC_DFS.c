/*******************************************************************************
* Copyright (C), 2010-2011, HUAWEI Tech. Co., Ltd
*
* File name:
*                pwcCommon.c
* Description:
*                sleep managerment
*
* Author:   	汪玮
* Date:          2011-09-28
* Version:       1.0
*
*
*
* History:
* Author:	汪玮
* Date:			2011-09-28
* Description:	Initial version
*
*******************************************************************************/
/*V3R2 CS f00164371*/
#include "generated/FeatureConfigDRV.h"
#include <generated/FeatureConfig.h>

#if (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS))
#include <linux/linkage.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <mach/balong_v100r001.h>
#include <asm/io.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/cpumask.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/irqnr.h>
#include <asm/cputime.h>
#include <linux/stat.h>
#include "BSP.h"
#include <mach/pwrctrl/BSP_PWC_DFS.h>
#include <mach/pwrctrl/v7r1/BSP_PWC_SOCREG.h>
#include <mach/pwrctrl/BSP_PWC_COMMON.h>

DFS_CONFIGINFO_STRU g_stDfsConfigInfo ;
extern unsigned int g_ulAcupIdleTime;

void PWRCTRL_DfsIfcGetAcpuLoad(void *para, unsigned int len )
{
	unsigned int temp =0;
	DFS_IFC_MSG_STRU *pMsgBody = NULL;
	
	pMsgBody = (DFS_IFC_MSG_STRU *)kmalloc( sizeof(DFS_IFC_MSG_STRU),0 );

	pMsgBody->module_id= IFC_MODULE_BSP;
	pMsgBody->func_id = IFCP_FUNC_DFS_ACPULOAD;
	pMsgBody->pstVoteMsg.ulCoreID = DFS_ACPU;
	pMsgBody->result = g_ulAcupIdleTime;

	printk(KERN_DEBUG "A--g_ulAcupIdleTime=%d \n",g_ulAcupIdleTime);
	g_ulAcupIdleTime=0;
	
	temp = (unsigned int)BSP_IFC_Send(pMsgBody, sizeof(DFS_IFC_MSG_STRU));
	
	if(DFS_OK != temp)
	{
		printk("PWRCTRL_DfsIfcGetAcpuLoad: BSP_IFC_Send Failed \n");
	}
	kfree(pMsgBody);
	return ;
}

unsigned int PWRCTRL_DfsSetProfileAcpu(PWC_DFS_ID_E ulClientID, PWC_DFS_PROFILE_E enProfile)
{
	unsigned int ulResult = DFS_SUCCESS;
	unsigned int ulId = 0;
	
	DFS_IFC_MSG_STRU *pMsgBody = NULL;

	ulId = ulClientID;

	if(ulId >= PWRCTRL_DFS_BUTT)
	{
		/*printk("DFS_SetProfile: client id is valid \n");*/
		return DFS_PARA_INVAILD;
	}

	if(g_stDfsConfigInfo.ulProfileNum <= enProfile)
	{
		/*printk("DFS_SetProfiel: The Profile error \n");*/
		return DFS_PARA_INVAILD;
	}

	pMsgBody = (DFS_IFC_MSG_STRU *)kmalloc(sizeof(DFS_IFC_MSG_STRU),0);
	
	if(NULL == pMsgBody)
	{
		/*printk("pwrctrl_dfs_msg_profile: kmalloc Failed \n");*/
		kfree(pMsgBody);
		return DFS_FAILURE;
	}
	
	pMsgBody->module_id = IFC_MODULE_BSP;
	pMsgBody->func_id = IFCP_FUNC_DFS_SETPROFILE;
	pMsgBody->result = 0;
	pMsgBody->pstVoteMsg.ulMsgName = DFS_VOTE_MSG;
	pMsgBody->pstVoteMsg.enVoteProfile = enProfile;
	pMsgBody->pstVoteMsg.enVoteType = DFS_VOTE_SET;
	pMsgBody->pstVoteMsg.ulClient = (unsigned int)(ulClientID&DPM_CLIENT_ID_MASK);
	pMsgBody->pstVoteMsg.ulCoreID = DFS_ACPU;
	
	ulResult = (unsigned int)BSP_IFC_Send(pMsgBody, sizeof(DFS_IFC_MSG_STRU));

	/*
	printk("  IFC Send: ulMsgName =%d, enVoteProfile=%d, enVoteType=%d, ulClient=%d, ulCoreID=%d. \n",
   	     pMsgBody->pstVoteMsg.ulMsgName, pMsgBody->pstVoteMsg.enVoteProfile, pMsgBody->pstVoteMsg.enVoteType, pMsgBody->pstVoteMsg.ulClient, pMsgBody->pstVoteMsg.ulCoreID);   // wangwei test
	*/

	if(DFS_MSG_ERROR == ulResult)
	{
		/*printk("pwrctrl_dfs_mgr_send_msg: DFS Send Msg Failed \n");*/
	}

	kfree(pMsgBody);
	return ulResult;
}


unsigned int PWRCTRL_DfsReleaseProfileAcpu(PWC_DFS_ID_E ulClientID)
{
	unsigned int ulResult = DFS_SUCCESS;
	DFS_IFC_MSG_STRU *pMsgBody = NULL;
	unsigned int ulId = ulClientID;
	
	if(ulId >= PWRCTRL_DFS_BUTT)
	{
		/*printk("DFS_ReleaseProfile: client id is invalid \n");*/
		return DFS_PARA_INVAILD;
	}

	pMsgBody = (DFS_IFC_MSG_STRU *)kmalloc(sizeof(DFS_IFC_MSG_STRU),0);

	if(NULL==pMsgBody)
	{
		/*printk("DFS_ReleaseProfile: OSAL_Malloc is Failed \n");*/
		kfree(pMsgBody);
		return DFS_FAILURE;
	}

	pMsgBody->module_id = IFC_MODULE_BSP;
	pMsgBody->func_id = IFCP_FUNC_DFS_RELEASEPROFILE;
	pMsgBody->pstVoteMsg.ulMsgName = DFS_VOTE_MSG;
	pMsgBody->pstVoteMsg.enVoteType = DFS_VOTE_CLEAN;
	pMsgBody->pstVoteMsg.ulClient =  (unsigned int)(ulClientID&DPM_CLIENT_ID_MASK);
	pMsgBody->pstVoteMsg.ulCoreID = DFS_ACPU;

	ulResult = (unsigned int)BSP_IFC_Send(pMsgBody, sizeof(DFS_IFC_MSG_STRU));

	if(DFS_MSG_ERROR == ulResult)
	{
		/*printk("pwrctrl_dfs_mgr_msg: DFS Send Msg Failed \n");*/
	}
	
	kfree(pMsgBody);
	
	return ulResult;
}


void PWRCTRL_DfsAcpuInit(void)
{
	if (PWRCTRL_SWITCH_OFF == g_stPwrctrlSwitch.dfs)
	{     
	    return;
	}
	
	g_stDfsConfigInfo.ulDFSFunEnableFlag = DFS_TRUE;
	g_stDfsConfigInfo.ulProfileNum = DFS_PROFILE_NUM;
	(void)BSP_IFCP_RegFunc(IFCP_FUNC_DFS_ACPULOAD,  (BSP_IFC_REG_FUNC)PWRCTRL_DfsIfcGetAcpuLoad);
	(void)BSP_IFCP_RegFunc(IFCP_FUNC_DFS_SETPROFILE, (BSP_IFC_REG_FUNC)PWRCTRL_DfsSetProfileAcpu);
	(void)BSP_IFCP_RegFunc(IFCP_FUNC_DFS_RELEASEPROFILE, (BSP_IFC_REG_FUNC)PWRCTRL_DfsReleaseProfileAcpu);
	
	return ;
}


void change_idle_time( int arg)
{
	g_ulAcupIdleTime=arg;
}


#if 0
/*************************************************************
* Function:	unsigned int pwrctrl_dfs_get_acpuload(void)
*Description:	acpu cpu 占有率统计
*Calls:		
*Called By:		DFS任务调用
*Table Accessed:	no
*Table Updated:		no
*Input:	no
*Output:		no
*Return:		cpuload
*Others:
*************************************************************/
 UINT32 pwrctrl_dfs_get_acpuload(void)
{
	UINT32 tog = 0;  /* toggle swith for cleaner code */
	UINT32 cpuload = 0;
	jiff cpu_use[2]= {0}, cpu_nic[2]={0}, cpu_sys[2]={0},cpu_idl[2]={0}, cpu_iow[2]={0}, cpu_xxx[2]={0}, cpu_yyy[2]={0}, cpu_zzz[2]={0};
	jiff duse=0, dsys=0, didl=0, diow=0, dstl=0, Div=1, divo2=0;
	int debt = 0;		//handle idle ticks running backwards
	UINT32 result = 0;

    get_cpuload(cpu_use, cpu_nic, cpu_sys, cpu_idl, cpu_iow, cpu_xxx, cpu_yyy, cpu_zzz);
	#if 0
	result = pwrctrl_dfs_acpu_get_state(cpu_use, cpu_nic, cpu_sys, cpu_idl, cpu_iow, cpu_xxx, cpu_yyy, cpu_zzz);
	if(result != DFS_OK)
		{
			printk(" !!!!! get cpu load failed \n\n");
			return DFS_ERROR;
		}
	#endif
	duse = *cpu_use + *cpu_nic;
	dsys = *cpu_sys + *cpu_xxx + *cpu_yyy;
	didl = *cpu_idl;
	diow = *cpu_iow;
	dstl = *cpu_zzz;
	Div = duse + dsys + didl + diow + dstl;
   
	msleep(sleep_time);

	tog =!tog;

   //pwrctrl_dfs_acpu_get_state(cpu_use +tog, cpu_nic + tog, cpu_sys + tog, cpu_idl + tog, cpu_iow + tog, 
	//								cpu_xxx + tog, cpu_yyy + tog, cpu_zzz + tog);
	 get_cpuload(cpu_use +tog, cpu_nic + tog, cpu_sys + tog, cpu_idl + tog, cpu_iow + tog, 
									cpu_xxx + tog, cpu_yyy + tog, cpu_zzz + tog);
	 #if 0
	result = pwrctrl_dfs_acpu_get_state(cpu_use +tog, cpu_nic + tog, cpu_sys + tog, cpu_idl + tog, cpu_iow + tog, 
       								cpu_xxx + tog, cpu_yyy + tog, cpu_zzz + tog);
	if(result != DFS_OK)
		{
			printk(" !!!!!!  get cpu load failed \n\n");
			return DFS_ERROR;
		}
	#endif

	duse = cpu_use[tog] - cpu_use[!tog] + cpu_nic[tog] - cpu_nic[!tog];
	dsys = cpu_sys[tog] - cpu_sys[!tog] + cpu_xxx[tog] -cpu_xxx[!tog] + cpu_yyy[tog] - cpu_yyy[!tog];
	didl = cpu_idl[tog] - cpu_idl[!tog];
	diow = cpu_iow[tog] - cpu_iow[!tog];
	dstl = cpu_zzz[tog] - cpu_zzz[!tog];


	 printk("  cpu_use[tog]=%d, cpu_use[!tog]=%d.   ",(int)cpu_use[tog], (int)cpu_use[!tog]);
	 printk("  cpu_nic[tog]=%d, cpu_nic[!tog]=%d.   ",(int)cpu_nic[tog], (int)cpu_nic[!tog]);
	 printk("\n");
	 printk("  cpu_sys[tog]=%d, cpu_sys[!tog]=%d.   ",(int)cpu_sys[tog], (int)cpu_sys[!tog]);
	 printk("  cpu_idl[tog]=%d, cpu_idl[!tog]=%d.   ",(int)cpu_idl[tog], (int)cpu_idl[!tog]);
	 printk("\n");
	 printk("  cpu_iow[tog]=%d, cpu_iow[!tog]=%d.   ",(int)cpu_iow[tog], (int)cpu_iow[!tog]);
	 printk("  cpu_xxx[tog]=%d, cpu_xxx[!tog]=%d.   ",(int)cpu_xxx[tog], (int)cpu_xxx[!tog]);
	 printk("\n");
	 printk("  cpu_yyy[tog]=%d, cpu_yyy[!tog]=%d.   ",(int)cpu_yyy[tog], (int)cpu_yyy[!tog]);
	 printk("  cpu_zzz[tog]=%d, cpu_zzz[!tog]=%d.   ",(int)cpu_zzz[tog], (int)cpu_zzz[!tog]);
	 printk("\n");
    printk("  after sleep: duse = %d, dsys = %d, didl = %d, diow = %d, dstl = %d  \n", (int)duse, (int)dsys, (int)didl, (int)diow, (int)dstl);

	if(debt)
	{
		didl = (int)didl + debt;
		debt = 0;
	}
	if((int)didl < 0)
	{
		debt = (int)didl;
		didl = 0;
	}

	Div = duse + dsys + didl + diow + dstl;
		
	divo2 = Div;   // divo2 = Div/2UL;
    printk("  Div = %d, duse = %d, dsys = %d, didl = %d, diow = %d, dstl = %d, divo2 = %d \n", (int)Div, (int)duse, (int)dsys, (int)didl, (int)diow, (int)dstl, (int)divo2);

	cpuload = (UINT32)(100*(duse + dsys + diow + dstl));  /* exclude the didl  ((100*(duse + dsys + diow + dstl) + divo2*4)/Div) */
	cpuload = cpuload/((UINT32)divo2);
	printk("  cpuload = %d \n",cpuload );
	return  cpuload;

}


/*************************************************************
* Function:	void pwrctl_dfs_acpu_get_state(jiff *restrict cuse, jiff *restrict cice, jiff *restrict csys, jiff *restrict cide,
*													jiff *restrict ciow, jiff *restrict cxxx, jiff *restrict cyyy, jiff*restrict czzz)
*Description:	获得acpu的状态
*Calls:		
*Called By:		pwrctrl_dfs_get_acpuload
*Table Accessed:	no
*Table Updated:		no
*Input:	jiff *restrict cuse, jiff *restrict cice, jiff *restrict csys, jiff *restrict cide,
*													jiff *restrict ciow, jiff *restrict cxxx, jiff *restrict cyyy, jiff*restrict czzz
*Output:		no
*Return:		no
*Others:
*************************************************************/
UINT32 pwrctrl_dfs_acpu_get_state(jiff *restrict cuse, jiff *restrict cice, jiff *restrict csys, jiff *restrict cide,
													jiff *restrict ciow, jiff *restrict cxxx, jiff *restrict cyyy, jiff *restrict czzz)
{	
	struct file * fd;
	const char *b;
	buff[BUFFSIZE - 1] = 0;
	loff_t pos = 0;

	//if(fd){
	//	filp_open(fd, 0UL, SEEK_SET);  // old is  lseek
	//	}else{
	fd = filp_open("/proc/stat", O_RDONLY, 0);   // old is open  O_RDONLY
	if(fd == -1) 
		{
		printk("/proc/stat \n");//printk("/proc/stat \n");   // wangwei old is  crash("/proc/stat")
		return DFS_ERROR;
		}
	//	}
	vfs_read(fd, buff, BUFFSIZE - 1,&pos);    // old is read

	filp_close(fd, NULL);
	
	b = strstr(buff, "cpu ");
	
	if(b)
	{
		sscanf(b, "cpu %Lu %Lu %Lu %Lu %Lu %Lu %Lu %Lu", cuse, cice, csys, cide, ciow, cxxx,cyyy, czzz);
		printk("cuse=0x%x, cice=0x%x, csys=0x%x, cide=0x%x, ciow=0x%x, cxxx=0x%x, cyyy=0x%x, czzz=0x%x \n", (int)cuse, (int)cice, (int)csys, (int)cide, (int)ciow, (int)cxxx,(int)cyyy, (int)czzz);		
	}
	return DFS_OK;
	
}	



#ifndef arch_irq_stat_cpu
#define arch_irq_stat_cpu(cpu) 0
#endif
#ifndef arch_irq_stat
#define arch_irq_stat() 0
#endif
#ifndef arch_idle_time
#define arch_idle_time(cpu) 0
#endif
void get_cpuload(jiff *restrict cuse, jiff *restrict cice, jiff *restrict csys, jiff *restrict cide,
													jiff *restrict ciow, jiff *restrict cxxx, jiff *restrict cyyy, jiff *restrict czzz)
{
	int i, j;
	unsigned long jif;
	cputime64_t user, nice, system, idle, iowait, irq, softirq, steal;
	cputime64_t guest, guest_nice;
	u64 sum = 0;
	u64 sum_softirq = 0;
	unsigned int per_softirq_sums[NR_SOFTIRQS] = {0};
	struct timespec boottime;
	unsigned int per_irq_sum;

	user = nice = system = idle = iowait =
		irq = softirq = steal = cputime64_zero;
	guest = guest_nice = cputime64_zero;
	getboottime(&boottime);
	jif = boottime.tv_sec;

	for_each_possible_cpu(i) {
		user = cputime64_add(user, kstat_cpu(i).cpustat.user);
		nice = cputime64_add(nice, kstat_cpu(i).cpustat.nice);
		system = cputime64_add(system, kstat_cpu(i).cpustat.system);
		idle = cputime64_add(idle, kstat_cpu(i).cpustat.idle);
		idle = cputime64_add(idle, arch_idle_time(i));
		iowait = cputime64_add(iowait, kstat_cpu(i).cpustat.iowait);
		irq = cputime64_add(irq, kstat_cpu(i).cpustat.irq);
		softirq = cputime64_add(softirq, kstat_cpu(i).cpustat.softirq);
		steal = cputime64_add(steal, kstat_cpu(i).cpustat.steal);
		guest = cputime64_add(guest, kstat_cpu(i).cpustat.guest);
		guest_nice = cputime64_add(guest_nice,
			kstat_cpu(i).cpustat.guest_nice);
		for_each_irq_nr(j) {
			sum += kstat_irqs_cpu(j, i);
		}
		sum += arch_irq_stat_cpu(i);
	}
	*cuse = (unsigned long long)cputime64_to_clock_t(user);
	*cice = (unsigned long long)cputime64_to_clock_t(nice);
	*csys = (unsigned long long)cputime64_to_clock_t(system);
	*cide = (unsigned long long)cputime64_to_clock_t(idle);
	*ciow = (unsigned long long)cputime64_to_clock_t(iowait);
	*cxxx = (unsigned long long)cputime64_to_clock_t(irq);
	*cyyy = (unsigned long long)cputime64_to_clock_t(softirq);
	*czzz = (unsigned long long)cputime64_to_clock_t(steal);
	return DFS_OK;
	
}

#endif
#endif
