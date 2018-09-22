
/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  omdrv.c
*
*   作    者 :  yudongbin
*
*   描    述 :  本文件命名为"wlan_if.c"
*
*   修改记录 :  2011年11月11日  v1.00  yuanqinshun创建
<问题单号>        <作  者>  <修改时间> <版本> <修改描述>
*************************************************************************/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/skbuff.h>
#include <linux/netlink.h>
#include <linux/moduleparam.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <net/sock.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/timer.h>
#include <linux/netlink.h>
#include <mach/gpio.h>

#include <linux/semaphore.h>
#include "DrvInterface.h"
#include "wlan_if.h"

#include "product_config.h"
#include <linux/fake/typedefs.h>



#ifdef KERNEL_MANAGE_SUSPEND
#define NO_NV_RADIO_PWR_CTRL
#endif



#if (defined (CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))

#define USE_HARD_TIMER_V7
#ifdef USE_HARD_TIMER_V7
#include "../SoftTimer/SoftTimer.h"
HTIMER v7r143239_hard_timer = NULL;
HTIMER v7r143239_hard_timer2 = NULL;
#define MAGIC_FOR_EVER 0xA5A55555
//32k max seconds supported
#define MAX_SECONDS (0x3fffff)

#endif

unsigned int wifi43239_run_mode = WIFI_CMD_MOD_MAX;
#endif


#if (defined (CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))
static irqreturn_t wifi_wakeup_host_irq_v7(int irq, void *dev_id);
#include <linux/irq.h>
 
#define WIFI_WAKEUP_NAME "wifi_wakeup_host"
#if defined (CHIP_BB_6920ES)
#define INT_WIFI_WAKEUP_HOST                      112  
#define BALONG_GPIO_WIFI_WAKEUP_HOST	 26
#define BALONG_GPIO_HOST_WAKEUP_WIFI  4
#define BALONG_WAKEUP_HOST		BALONG_GPIO_0(BALONG_GPIO_WIFI_WAKEUP_HOST)    /*GPIO0_26*/
#define BALONG_WAKEUP_WIFI		BALONG_GPIO_1(BALONG_GPIO_HOST_WAKEUP_WIFI)   /*GPIO1_4*/
#else  //#if defined (CHIP_BB_6920CS)
#define INT_WIFI_WAKEUP_HOST                      114  
#define BALONG_GPIO_WIFI_WAKEUP_HOST	 15
#define BALONG_GPIO_HOST_WAKEUP_WIFI  12
#define BALONG_WAKEUP_HOST		BALONG_GPIO_2(BALONG_GPIO_WIFI_WAKEUP_HOST)    /*GPIO0_26*/
#define BALONG_WAKEUP_WIFI		BALONG_GPIO_2(BALONG_GPIO_HOST_WAKEUP_WIFI)   /*GPIO1_4*/
#endif

//add sem for wifi_get_sta_num
//down sem when trying to get sta num, if power off is called, the sem must be get first
struct semaphore driver_alive_sem;

#else
#endif

#define HW_VER_PRODUCT_E5371_DCM_SUBID  (BSP_U16)0x0003     /*E5371S_7 新增硬件子版本号定义*/
#define SUB_ID_BITS                     16                  /*子硬件版本号位数*/
#define MASK_NUM                        0xFFFF              /*掩码*/

/*函数声明*/
int wifi_get_is_LNA(void);
static int g_WIFI_LNA = 0;/*0--No LNA  1--LNA*/

GET_STA_NUM_FUNC g_get_sta_num_func = NULL;
GET_STA_NUM_FUNC g_get_sta_num_func_bak = NULL;

/* WARNING:
  *  This function should ONLY be invoked by the dhd_module_init and dhd_module_cleanup.
  *  NO Other process is permitted to use this function. 
  */
char *
bcm_ether_ntoa(const struct ether_addr *ea, char *buf)
{
    static const char template[] = "%02x:%02x:%02x:%02x:%02x:%02x";
    //移植博通驱动函数
    snprintf(buf, 18, template,
        ea->octet[0]&0xff, ea->octet[1]&0xff, ea->octet[2]&0xff,
        ea->octet[3]&0xff, ea->octet[4]&0xff, ea->octet[5]&0xff);/*网络驱动标准octet使用*/
    return (buf);
}
EXPORT_SYMBOL(bcm_ether_ntoa);
void set_get_sta_num_func(unsigned int func)
{
    WLAN_TRACE_INFO(("%s: enter\n", __FUNCTION__));
    
	if(func == (unsigned int)NULL)
	{
		WLAN_TRACE_ERROR(("%s: NULL\n", __FUNCTION__));
		g_get_sta_num_func = NULL;
		g_get_sta_num_func_bak = NULL;
	}
	else
	{
		WLAN_TRACE_INFO(("%s: func addr = %x\n", __FUNCTION__, func));
		g_get_sta_num_func = (GET_STA_NUM_FUNC)func;	
		g_get_sta_num_func_bak = g_get_sta_num_func;
        
		WLAN_TRACE_INFO(("%s: g_get_sta_num_func addr = %p\n", __FUNCTION__, g_get_sta_num_func));
	}

}


void print_sta_num_func(void)
{
	if(g_get_sta_num_func == NULL)
	{
        WLAN_TRACE_INFO(("%s: g_get_sta_num_func addr = NULL, while g_get_sta_num_func_bak=%p\n",\
						__FUNCTION__,g_get_sta_num_func_bak));        
	}
	else
	{
        WLAN_TRACE_INFO(("%s: g_get_sta_num_func addr = %p\n", __FUNCTION__, g_get_sta_num_func));
	}
}




/**********************************************************
function name: wifi_get_sta_num

description: 

	in mssid mode, count is the sum of clients connected to main ssid and 2nd ssid, 
valid count is returned if main ssid is operational, even 2nd ssid is not ok

input:
	
output:
	-1: something is wrong
	0: no valid wifi interface or no client is connected
	>= 1: count of connected clients. 


		
**********************************************************/
int wifi_get_sta_num(void)
{ 
	int cnt = 0;
	//don't wait to check wifi
    if(down_trylock(&driver_alive_sem))
    {
        WLAN_TRACE_INFO(("%s: 20: fail to down driver_alive_sem\n",  __FUNCTION__));       
        return 0;
    }
    
	if(NULL == g_get_sta_num_func)
	{
		WLAN_TRACE_ERROR(("%s: g_get_sta_num_func is NULL,while g_get_sta_num_func_bak=%p\n",\
							__FUNCTION__,g_get_sta_num_func_bak));
        up(&driver_alive_sem);
		return 0;
	}
	else
	{

		cnt = g_get_sta_num_func();
        up(&driver_alive_sem);
        
		//WLAN_TRACE_INFO(("%s: sta num : %d\n", __FUNCTION__, cnt));
        //just change -1 to 0 for adapter
        if(cnt == -1)
        {return 0;}

		return cnt;
	}

}

/**********数据区**********/
static struct platform_device *wifi_platform_device = NULL;

static WIFI_MSG_CTRL_STRU  *wifi_user_msg_block = NULL;


static int g_iWifiTcmdTxMode = 0;
static int g_iWifiTcmdRxMode = 0;
static int g_iWifiTcmdMode = WIFI_IS_TCMD_MODE;

static unsigned int g_uiRxTotalPkts = 0;
static unsigned int g_uiRxBadPkts = 0;

static unsigned int g_uiUcastPkts = 0;
static unsigned int g_uiMcastPkts = 0;


static struct timer_list *g_pWiFiTimer = NULL;     // used to record the wifi timer
static struct timer_list wifi_ap_timeout_timer; // wifi ap timer


#ifdef CONFIG_WLAN_IF_BCM43239
#include <mach/pwrctrl/BSP_PWC_COMMON.h>

#define NV_MAC_ADDR_V7 50014
#define NV_SUSPEND_WIFI43239 0xD10B

int suspend_enable_43239 = 0;
static WIFI_MSG_CTRL_STRU  *wifi_poweron_msg_block = NULL;


/* 上电启动WIFI，将WIFI加入网桥，由于网桥延迟导致用户接入上报消息丢失 */
#define POWERON_TIMER_MAX_LOOP    2
#define POWERON_TIMER_INTERVAL    5
static struct timer_list poweron_timer;
atomic_t g_poweron_timer_count = ATOMIC_INIT(0);
int g_the_first_power_on = 1;
int g_power_on_timer_scheduled = 0;

static void schedule_poweron_timer(void);
static void power_off_clear_msgqueue(void);

static inline WIFI_EVENT_NODE_STRU *wifi_event_dequeue(WIFI_MSG_CTRL_STRU *pCtrl);
static inline int wifi_eventq_is_empty(WIFI_MSG_CTRL_STRU *pCtrl);


//#define mac_nv_len 32
extern uint32_t NVM_Read(uint32_t ulId, void *pData, uint16_t usDataLen);


extern signed int DR_NV_Read(unsigned short usID, void *pItem, unsigned int ulLength);

static WIFI_EVENT_NODE_STRU *wifi_event_node_alloc(WIFI_MSG_CTRL_STRU *pCtrl,void * data, int len);
static inline void wifi_event_enqueue(WIFI_MSG_CTRL_STRU *pCtrl,WIFI_EVENT_NODE_STRU *pNode);
static inline void wifi_event_node_free(WIFI_EVENT_NODE_STRU *pNode);

static void wifi_event_send(struct work_struct *work);

/*modified for lint e732 */
int v7_nv_read_mac_test(unsigned short ulId, unsigned int ulItemDataLen)
{
	int ret = 0;
	//char mac_str[128] = {0};
	char *mac_str = NULL;

	WLAN_TRACE_INFO(("%s: 100: enter\n", __FUNCTION__));
	
	mac_str = kmalloc(128, GFP_KERNEL);

	if(mac_str == NULL)
	{		
		WLAN_TRACE_INFO(("%s: 130: fail to alloc\n", __FUNCTION__));
	}
	

	ret = DR_NV_Read(ulId, (void *)mac_str, ulItemDataLen);

	if(ret != 0)
	{		
		WLAN_TRACE_INFO(("%s: 200: ret = %d\n", __FUNCTION__, ret));
		//return ret;
	}

	
	WLAN_TRACE_INFO(("%s: 300: got mac\n", __FUNCTION__));

	
	WLAN_TRACE_INFO(("%s:400: mac str is =%s\n", __FUNCTION__, mac_str));
	
	kfree(mac_str);
	
	return ret;	
	
}


/*******************************************************************************
Function:wifi_fet_mac_addr
Description:
	get mac from nv
Input:
	s1
Output:
	N/A
Return:
	N/A
********************************************************************************/

void wifi_get_nvmac_addr(unsigned char *mac_str)                             
{         

	int ret;
    char zero_array[32] = {0};
                                       
	                                                                
	WLAN_TRACE_INFO(("%s: 100: enter\n", __FUNCTION__));            
                                                                        
    #if 1                                                               
	ret = DR_NV_Read(NV_MAC_ADDR_V7, mac_str, 32);                 
    #else                                                               
    {                                                                   
        char *test_e5_mac= "00:11:22:95:96:01";                         
        memcpy(mac_str, test_e5_mac, strlen(test_e5_mac));
        ret = 0;
    }                                                                   
                                                                        
    #endif                                                              
                                                                        
	if((0 != ret) || (memcmp(zero_array, mac_str, 32) == 0))                                                    
	{    	
        char * default_mac = "1C:1D:67:48:C4:9F";        
        memcpy(mac_str, default_mac, strlen(default_mac));                   
		printk("%s:NV Read Fail! ret =%d, set default nv\n", __FUNCTION__, ret);
        
	}                                                               
	printk("%s mac str is =%s\n", __FUNCTION__,mac_str);                 
	                                                                
	return; 
}                               

/*modified for lint e732 */
int wifi_suspend_nv_read(void)
{
    int ret = -1;
    ST_PWC_SWITCH_STRU suspend_nv;
    memset(&suspend_nv, 0, sizeof(ST_PWC_SWITCH_STRU));
#ifndef PRODUCT_CFG_BUILD_SEPARATE
	ret =  NVM_Read(NV_SUSPEND_WIFI43239, (void *)&suspend_nv, sizeof(ST_PWC_SWITCH_STRU));
#endif

    if(ret != 0)
    {
        suspend_enable_43239= 0;
    }
    else
    {
        suspend_enable_43239= suspend_nv.deepsleep;
    }

    return ret;
    
}
#endif

/*****************************************************************
Function: wifi_tmd
Description:
    execute the tcmd 
Input:
    tcmd
Output:
    N/A
Return:
    0: execute succes
    1: execute failed
*******************************************************************/
int wifi_tcmd(char *tcmd_str)
{

    int ret = 1;
    char *path;
    char *envp[3];    	
    char **argv = NULL;
    char *temp_argv_0 = NULL;
    
    path=BRCM_WL_TOOL;

    /*output tcmd_str*/
    if(NULL == tcmd_str)
    {
        return 1;
    }
	
	WLAN_TRACE_INFO(("%s: 100: tcmd_str : %s\n",  __FUNCTION__, tcmd_str));

	
    argv=argv_split(0, tcmd_str, &ret);
    if(ret <= 1)
    {
        WLAN_TRACE_ERROR(("%s: error, ret <= 1, ret =%d \n",__FUNCTION__, ret));
        return 1;
    }
    else
    {
        temp_argv_0 = argv[0];
        argv[0]= path;
    }
    
    envp[0]="HOME=/";
    envp[1]="PATH=/sbin:/bin:/usr/sbin:/usr/bin";
    envp[2]=NULL;
    
	WLAN_TRACE_INFO(("%s: 500: end\n",  __FUNCTION__));

    ret=call_usermodehelper(argv[0],argv,envp,(enum umh_wait)1);
    
    WLAN_TRACE_INFO(("%s: success to call_usermodehelper, ret = %d\n",__FUNCTION__, ret));
    argv[0] = temp_argv_0;
    argv_free(argv);
    return ret;
}

/*****************************************************************
Function: wifi_get_tcmd_mode
Description:
    get tcmd current mode
Input:
    N/A    
Output:
    N/A
Return:
    16: for cal_test mode
    17: for TX mode
    18: for RX mode
*******************************************************************/
int wifi_get_tcmd_mode(void)
{
    
    /*g_iWifiTcmdMode = wifi_check_tcmd_mode();*/        
    if(g_iWifiTcmdMode && g_iWifiTcmdTxMode)
    {
        return 17;        
    }

    if(g_iWifiTcmdMode && g_iWifiTcmdRxMode)
    {
        return 18;        
    }

    if(g_iWifiTcmdMode)
    {
        return 16;
    }

    return 0;
}

/*****************************************************************
Function: wifi_get_status
Description:
    get wifi current mode
Input:
    N/A    
Output:
    N/A
Return:
    0: wifi is off
    1: wifi is in normal mode
    2: wifi is in tcmd mode
*******************************************************************/
int wifi_get_status(void)
{
    return (g_iWifiTcmdMode);
}

/*****************************************************************
Function: wifi_get_rx_detail_report
Description:
    get result of rx report: totalPkt, GoodPkt, ErrorPkt
Input:
    N/A    
Output:
     totalPkt, GoodPkt, ErrorPkt
Return:
    N/A
*******************************************************************/

void  wifi_get_rx_detail_report(unsigned int *totalPkts, unsigned int *googPkts, unsigned int * badPkts)
{
    int txMode,rxMode;

   /*record transmit mode*/
    txMode = g_iWifiTcmdTxMode;
    rxMode = g_iWifiTcmdRxMode;

    wifi_tcmd("wl counters");

    /*restore transmit mode*/
    g_iWifiTcmdTxMode = txMode;
    g_iWifiTcmdRxMode = rxMode;

    *totalPkts = g_uiRxTotalPkts;
    *badPkts = g_uiRxBadPkts;
    *googPkts = g_uiRxTotalPkts - g_uiRxBadPkts;    
}


/*****************************************************************
Function: wifi_get_rx_packet_report
Description:
    get result of rx ucast&mcast packets
Input:
    N/A    
Output:
     ucast,mcast
Return:
    N/A
*******************************************************************/

void  wifi_get_rx_packet_report(unsigned int *ucastPkts, unsigned int *mcastPkts)
{

    int txMode,rxMode;

   /*record transmit mode*/
    txMode = g_iWifiTcmdTxMode;
    rxMode = g_iWifiTcmdRxMode;

    wifi_tcmd("wl counters");

    /*restore transmit mode*/
    g_iWifiTcmdTxMode = txMode;
    g_iWifiTcmdRxMode = rxMode;

    *ucastPkts = g_uiUcastPkts;
    *mcastPkts = g_uiMcastPkts;
}


#if (defined (CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))
int wifi_sleep_vote_mode_k(unsigned int mode);
int wifi_sleep_vote_normal(void);



extern void BCM43239_WIFI_PWRCTRL_RESTORE(void);
extern void BCM43239_WIFI_PWRCTRL_SAVE(void);

#ifndef KERNEL_MANAGE_SUSPEND

extern void BCM43239_WIFI_PWRCTRL_SUSPEND(void);
extern void BCM43239_WIFI_PWRCTRL_RESUME(void);
#endif


/***********************************************************************************
 Function:          wifi_suspend_43239
 Description:      config 43239 suspend mode
 Calls:
 Input:              
 Output:            NA
 Return:            0/-1                  
 ************************************************************************************/
int wifi_suspend_43239(int mode)
{
    int ret = 0;
    /*modified for lint e529 char *envp[3]; */
    char *cmd_argv[2] = {"/system/bin/wifi_brcm/exe/dhdarm_le -i WiFi0 hsicsleep 0",
                        "/system/bin/wifi_brcm/exe/dhdarm_le -i WiFi0 hsicsleep 1"};
    /*modified for lint e614 */
    //char *argv[2] = {cmd_argv[mode],NULL};
    
    WLAN_TRACE_INFO(("%s: 100: argv[0] : %s\n",  __FUNCTION__, cmd_argv[mode]));
    /*modified for lint e614 */    

    ret = wifi_tcmd(cmd_argv[mode]);
    WLAN_TRACE_INFO(("%s: 300: ret : after call_wifi_tcmd %d\n",  __FUNCTION__, ret));
    return 0;       
}

/***********************************************************************************
 Function:          wifi_set_awake_host_gpio
 Description:      configr
 Calls:
 Input:              
 Output:            NA
 Return:            0/-1                  
 ************************************************************************************/
int wifi_set_awake_host_gpio(int mode)
{
    int ret = 0;
    //char *envp[3]; 
    char *cmd_argv[2] = {"wl bus:hostwake 0",
                        "wl bus:hostwake 1"};
    /*modified for lint e614 */
    //char *argv[2] = {cmd_argv[mode],NULL};
    WLAN_TRACE_INFO(("%s: 100: argv[0] : %s\n",  __FUNCTION__, cmd_argv[mode]));
    /*modified for lint e614 */
#if 0
    envp[0]="HOME=/";
    envp[1]="PATH=/sbin:/bin:/usr/sbin:/usr/bin";
    envp[2]=NULL;
    ret = call_usermodehelper(argv[0],argv,envp,1);   
    WLAN_TRACE_INFO(("%s: 200: ret : after call_usermodehelper %d\n",  __FUNCTION__, ret));
    if(ret)
    {
        return ret;
    }
#endif
    ret = wifi_tcmd(cmd_argv[mode]);
    WLAN_TRACE_INFO(("%s: 300: ret : after call_wifi_tcmd %d\n",  __FUNCTION__, ret));
    return 0;       
}
#endif

/*****************************************************************
Function: wifi_power_on_full
Description:
    start wifi
Input:
    N/A    
Output:
    N/A
Return:
    0: execute ok 
    1: execute failed
*******************************************************************/
/*modified for lint e732 */
int wifi_power_on_full(unsigned int mode)
{
    int ret = 1;
#if (defined (CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))
	char *envp[3]; 
	char *cmd_argv_860[2] = {"/system/bin/wifi_brcm/exe/wifi_poweron_43239_860.sh",
					"/system/bin/wifi_brcm/exe/wifi_poweron_43239_at_860.sh"};

    char *cmd_argv_925[4] = {"/system/bin/wifi_brcm/exe/wifi_poweron_43239_sbm.sh",
                        "/system/bin/wifi_brcm/exe/wifi_poweron_43239_at.sh",
                        "/system/bin/wifi_brcm/exe/wifi_poweron_43241_925_nolna.sh",
                        "/system/bin/wifi_brcm/exe/wifi_poweron_43241_925_at_nolna.sh"};
    
    char *cmd_argv_601[4] = {"/system/bin/wifi_brcm/exe/wifi_poweron_43239_sbm.sh",
                        "/system/bin/wifi_brcm/exe/wifi_poweron_43239_at.sh",
                        "/system/bin/wifi_brcm/exe/wifi_poweron_43241_601_nolna.sh",
                        "/system/bin/wifi_brcm/exe/wifi_poweron_43241_601_at_nolna.sh"};
    
#ifndef FEATURE_WIFI_OFFLOAD
	char *cmd_argv[2] = {"/system/bin/wifi_brcm/exe/wifi_poweron_43239_em.sh",
						"/system/bin/wifi_brcm/exe/wifi_poweron_43239_at.sh"};
#else
    char *cmd_argv[4] = {"/system/bin/wifi_brcm/exe/wifi_poweron_43239_sbm.sh",
                        "/system/bin/wifi_brcm/exe/wifi_poweron_43239_at.sh",
                        "/system/bin/wifi_brcm/exe/wifi_poweron_43239_sbm_nolna.sh",
                        "/system/bin/wifi_brcm/exe/wifi_poweron_43239_at_nolna.sh"};
#endif
    //char *argv[2] = {cmd_argv[mode],NULL};
    char *argv[2] = {NULL};
    char **tempargv = NULL;
    BSP_U16 proVersion = 0;

	if(mode >= WIFI_CMD_MOD_MAX)
	{
		return ret;
	}

	if (HW_VER_PRODUCT_E5_CMCC == BSP_HwGetVerMain())
	{
		argv[0] = cmd_argv_860[mode];
	}
	else
	{
		argv[0] = cmd_argv[mode];
	}
    proVersion = BSP_HwGetVerMain();
    switch(proVersion)
    {
        case HW_VER_PRODUCT_E5775S_925:
            tempargv = cmd_argv_925;
            printk("[%s] LNA = %d mode = %d\n",__FUNCTION__,g_WIFI_LNA,mode);
            break;
        case HW_VER_PRODUCT_E5372_601:
            tempargv = cmd_argv_601;
            printk("[%s] LNA = %d mode = %d\n",__FUNCTION__,g_WIFI_LNA,mode);
            break;
        default:
            tempargv = cmd_argv;
            printk("[%s] LNA = %d mode = %d\n",__FUNCTION__,g_WIFI_LNA,mode);
    }
    
    if(1 == g_WIFI_LNA)
    {
        argv[0] = tempargv[mode];
        printk("[%s] LNA = %d mode = %d\n",__FUNCTION__,g_WIFI_LNA,mode);
        printk("[%s] argv =%s\n",__FUNCTION__,argv[0]);
    }
    else
    {
        argv[0] = tempargv[mode + 2];
        printk("[%s] LNA = %d mode = %d\n",__FUNCTION__,g_WIFI_LNA,mode);
        printk("[%s] argv =%s\n",__FUNCTION__,argv[0]);
    }
#ifdef CONFIG_WLAN_IF_BCM43239
	atomic_set(&g_poweron_timer_count, POWERON_TIMER_MAX_LOOP);
#endif
	wifi43239_run_mode = mode;
        
	WLAN_TRACE_INFO(("%s: 100: argv[0] : %s\n",  __FUNCTION__, argv[0]));
	
    envp[0]="HOME=/";
    envp[1]="PATH=/sbin:/bin:/usr/sbin:/usr/bin";
    envp[2]=NULL;

	gpio_set_value(BALONG_WAKEUP_WIFI,1);
    BCM43239_WIFI_PWRCTRL_RESTORE();

	ret = call_usermodehelper(argv[0],argv,envp,(enum umh_wait)1);   
	WLAN_TRACE_INFO(("%s: 200: ret : after call_usermodehelper %d\n",  __FUNCTION__, ret));
#if 0
    {
        char *mv_cmd[2] = 
        {
            "/system/bin/wifi_brcm/exe/backup_powersave.sh",
            "/system/bin/wifi_brcm/exe/restore_powersave.sh"
        };
        
        argv[0] = mv_cmd[suspend_enable_43239];
        
        WLAN_TRACE_INFO(("%s: 300: argv[0] : %s\n",  __FUNCTION__, argv[0]));
        /*modified for lint e64 */
        call_usermodehelper(argv[0],argv,envp,(enum umh_wait)1);   
        //WLAN_TRACE_INFO(("%s: 400: after call_usermodehelper, %s, ret  %d\n",  __FUNCTION__, mv_cmd[suspend_enable_43239],ret));

    }
#endif
#ifdef CONFIG_WLAN_IF_BCM43239
	schedule_poweron_timer();
#endif

    //enable int for sbm at bootup
    #ifdef KERNEL_MANAGE_SUSPEND
        gpio_int_unmask_set(BALONG_WAKEUP_HOST);
    #endif

	if(ret)
	{
		return ret;
	}

	return 0;	

#else

    char *envp[3];    	
    char *argv[2] = {"/system/bin/wifi_brcm/exe/wifi_poweron.sh",NULL};
    
    envp[0]="HOME=/";
    envp[1]="PATH=/sbin:/bin:/usr/sbin:/usr/bin";
    envp[2]=NULL;
    
    ret=call_usermodehelper(argv[0],argv,envp,1);    

    return ret;
#endif
}

int wifi_power_on(void)
{
#if (defined (CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))
    int ret = 0;
    WLAN_TRACE_INFO(("%s: 100: enter\n",  __FUNCTION__));
    
    ret = wifi_tcmd("/system/bin/wifi_brcm/exe/wl up");
    return ret;
#else

    return wifi_power_on_full(WIFI_CMD_MOD_AT);

#endif
}


/*****************************************************************
Function: wifi_power_off_full
Description:
    shutdown wifi and hsic
Input:
    N/A    
Output:
    N/A
Return:
    0: execute ok 
    1: execute failed
*******************************************************************/
int wifi_power_off_full(void)
{
    int ret = 1;
    
	char *envp[3];    	
    char *argv[2] = {"/system/bin/wifi_brcm/exe/wifi_poweroff_43239.sh",NULL};
    
	WLAN_TRACE_INFO(("%s: 50: argv[0] %s\n",  __FUNCTION__, argv[0]));


    //g_get_sta_num_func will be set in module init; 
    if(down_interruptible(&driver_alive_sem))
    {
        WLAN_TRACE_INFO(("%s: 20: fail to down driver_alive_sem\n",  __FUNCTION__));       
        return -1;
    }
    
    //g_get_sta_num_func_bak = NULL;
    set_get_sta_num_func(NULL);
    
    up(&driver_alive_sem);
    
	atomic_set(&g_poweron_timer_count, 0);

    
    /*modified for lint e746 */
    #ifdef CONFIG_WLAN_IF_BCM43239
	power_off_clear_msgqueue();
    #endif
    
	BCM43239_WIFI_PWRCTRL_SAVE();

    envp[0]="HOME=/";
    envp[1]="PATH=/sbin:/bin:/usr/sbin:/usr/bin";
    envp[2]=NULL;

    /*modified for lint e64 */
	ret = call_usermodehelper(argv[0],argv,envp,(enum umh_wait)1);   
	WLAN_TRACE_INFO(("%s: 100: ret : after call_usermodehelper %d\n",  __FUNCTION__, ret));

	if(ret)
	{
		return ret;
	}

	//BCM43239_WIFI_PWRCTRL_SAVE();
	return 0;

}


/*****************************************************************
Function: wifi_power_off
Description:
    shutdown wifi
Input:
    N/A    
Output:
    N/A
Return:
    0: execute ok 
    1: execute failed
*******************************************************************/
int wifi_power_off(void)
{
    int ret = 1;

#if (defined (CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))

   
    WLAN_TRACE_INFO(("%s: 100: enter\n",  __FUNCTION__));

    if(wifi43239_run_mode == WIFI_CMD_MOD_AT)
    {  
	    WLAN_TRACE_INFO(("%s: 30: in at mode, wl down\n",  __FUNCTION__));
        ret = wifi_tcmd("/system/bin/wifi_brcm/exe/wl down");
        return ret;
    }
    else if(wifi43239_run_mode == WIFI_CMD_MOD_MAX)
    {
        //impossible status. should initial system first
	    WLAN_TRACE_INFO(("%s: 40: in WIFI_CMD_MOD_MAX mode\n",  __FUNCTION__));      

    }

	ret = wifi_power_off_full();
	//set mode to default since wifi is down
	wifi43239_run_mode = WIFI_CMD_MOD_MAX;
	return ret;
	
#else

	
    char *envp[3];    	
    char *argv[2] = {"/system/bin/wifi_brcm/exe/wifi_poweroff.sh",NULL};
    
    envp[0]="HOME=/";
    envp[1]="PATH=/sbin:/bin:/usr/sbin:/usr/bin";
    envp[2]=NULL;
    
    ret=call_usermodehelper(argv[0],argv,envp,1);    

    return ret;
#endif

}


/***********************************************************************************
 Function:          wifi_ap_timeout_func
 Description:      report the time out event to APP
 Calls:
 Input:              data: 
 Output:            NA
 Return:            NA                  
 ************************************************************************************/
 
void wifi_ap_timeout_func(unsigned long data)
{

#ifdef USE_HARD_TIMER_V7
	WIFI_USER_EVENT event;
	WLAN_TRACE_INFO((" %s: 100: enter, USE_HARD_TIMER_V7\n",__FUNCTION__));

	//disable interrupt since the wifi chip will be shutdown soon. for both em and sbm.
	//sbm will enable int in power_on_full
    gpio_int_mask_set(BALONG_WAKEUP_HOST); 


    //vote for normal state    
    wifi_sleep_vote_mode_k(WIFI_NORMAL_MODE);

    if(suspend_enable_43239 == 1)
    {
        #ifndef KERNEL_MANAGE_SUSPEND
            BCM43239_WIFI_PWRCTRL_RESUME();
        #endif
    }
    
    event.eventId = USER_WIFI_TIMEOUT_EVENT;
    event.eventVal = 0;
    wifi_event_send_tasklet_mode((void *)&event,sizeof(WIFI_USER_EVENT), 0);
	
    WLAN_TRACE_INFO((" %s: 200:wifi_event_send_tasklet_mode is success, event.eventId = %d, event.eventVal= %d\n",__FUNCTION__ ,event.eventId, event.eventVal));

#else

    struct timer_list *pTimer;
    WIFI_USER_EVENT event;
    WLAN_TRACE_INFO((" %s: 100: enter\n",__FUNCTION__));

    pTimer = (struct timer_list *)g_pWiFiTimer;
    
    del_timer(pTimer);

	
    event.eventId = USER_WIFI_TIMEOUT_EVENT;
    event.eventVal = 0;
    wifi_event_report((void *)&event,sizeof(WIFI_USER_EVENT));
	
    WLAN_TRACE_INFO((" %s: 200:report event to user is success, event.eventId = %d, event.eventVal= %d\n",__FUNCTION__ ,event.eventId, event.eventVal));

#endif


    return;
}


/***********************************************************************************
 Function:          wifi_create_ap_timer
 Description:      create a timer
 Calls:
 Input:              data: 
 Output:            NA
 Return:            NA                  
 ************************************************************************************/

static int wifi_create_ap_timer(void)
{

#ifdef USE_HARD_TIMER_V7
	
    WLAN_TRACE_INFO(("%s enter, use USE_HARD_TIMER_V7\n",__FUNCTION__));
	return 0;
#else

    struct timer_list *pTimer;
    WLAN_TRACE_INFO(("%s enter\n",__FUNCTION__));
    pTimer = (struct timer_list *)&wifi_ap_timeout_timer;

    /*init the timer*/
    init_timer(pTimer);
    
    pTimer->function = wifi_ap_timeout_func;
    pTimer->data = 0;    

    /*record the timer*/
    g_pWiFiTimer = pTimer;

    return 0;
#endif	
}

/***********************************************************************************
 Function:          wifi_delete_ap_timer
 Description:      delete a timer
 Calls:
 Input:              NA: 
 Output:            NA
 Return:            NA                  
 ************************************************************************************/
 
static void  wifi_delete_ap_timer(void)
{


#ifdef USE_HARD_TIMER_V7
	//WLAN_TRACE_INFO(("%s 100: enter, use USE_HARD_TIMER_V7\n",__FUNCTION__));

    SOFTTIMER_DEL_TIMER(&v7r143239_hard_timer);
	v7r143239_hard_timer = NULL;
	return;
#else

    if(g_pWiFiTimer)
    {
        WLAN_TRACE_INFO(("%s enter\n",__FUNCTION__));
        del_timer(g_pWiFiTimer);      
        g_pWiFiTimer = NULL;
    }
    return;
	
#endif	
}

/***********************************************************************************
 Function:          wifi_ap_start_timer
 Description:      delete a timer
 Calls:
 Input:              seconds: time out value 
 Output:            NA
 Return:            0/-1                  
 ************************************************************************************/
 
static int wifi_ap_start_timer(unsigned int seconds)
{

#ifdef USE_HARD_TIMER_V7

    int ret = 0;
    
        
	//WLAN_TRACE_INFO(("%s 50: timer set secs: %u, USE_HARD_TIMER_V7\n",__FUNCTION__, seconds));

    if(MAGIC_FOR_EVER == seconds)
    {
        WLAN_TRACE_INFO(("%s 55: get MAGIC_FOR_EVER\n",__FUNCTION__));
        
		wifi_delete_ap_timer();
        
        if(suspend_enable_43239 == 1)
        {        
            #ifndef KERNEL_MANAGE_SUSPEND  
            if(down_interruptible(&driver_alive_sem))
            {
                WLAN_TRACE_INFO(("%s: 65: fail to down driver_alive_sem\n",  __FUNCTION__));       
                return -1;
            } 
    
			if (g_get_sta_num_func)
				g_get_sta_num_func_bak = g_get_sta_num_func;
			/* revise the func ptr directly, as we modify the func of set_get_sta_num_func */
			//set_get_sta_num_func((unsigned int)NULL);
			g_get_sta_num_func = NULL;

                  
            WLAN_TRACE_INFO(("%s 68: suspend usb in MAGIC_FOR_EVER\n",__FUNCTION__));
            BCM43239_WIFI_PWRCTRL_SUSPEND();
            
            
            up(&driver_alive_sem);     
            #endif
        }
        
		WLAN_TRACE_INFO(("%s 70: timer delete\n",__FUNCTION__));

        #ifndef KERNEL_MANAGE_SUSPEND
        //enable interrupt	
		gpio_int_unmask_set(BALONG_WAKEUP_HOST); 
        #endif
        
		return ret;

	}
	else if(seconds > 0)
	{
	    if(seconds > MAX_SECONDS)
        {
            seconds = MAX_SECONDS;
        }   
	    /*modified for lint e64 */
		SOFTTIMER_CREATE_TIMER(&v7r143239_hard_timer, seconds*1000, SOFTTIMER_NO_LOOP,(SOFTTIMER_FUNC)wifi_ap_timeout_func,0);
        if(suspend_enable_43239 == 1)
        {        
            #ifndef KERNEL_MANAGE_SUSPEND  
            if(down_interruptible(&driver_alive_sem))
            {
                WLAN_TRACE_INFO(("%s: 90: fail to down driver_alive_sem\n",  __FUNCTION__));       
                return -1;
            } 
    
            if(g_get_sta_num_func)
				g_get_sta_num_func_bak = g_get_sta_num_func;
			/* revise the func ptr directly, as we modify the func of set_get_sta_num_func */
			//set_get_sta_num_func((unsigned int)NULL);
			g_get_sta_num_func = NULL;  
                      
            WLAN_TRACE_INFO(("%s 92: suspend usb\n",__FUNCTION__));
            BCM43239_WIFI_PWRCTRL_SUSPEND();
            

            up(&driver_alive_sem);   
			#endif         
        }
        
		WLAN_TRACE_INFO(("%s 95: timer create\n",__FUNCTION__));

        #ifndef KERNEL_MANAGE_SUSPEND
        //enable interrupt	
		gpio_int_unmask_set(BALONG_WAKEUP_HOST); 
        #endif

		return ret;

	}
	else
	{

		wifi_delete_ap_timer();

        #ifndef KERNEL_MANAGE_SUSPEND     
		gpio_int_mask_set(BALONG_WAKEUP_HOST); 
        #endif


        if(suspend_enable_43239 == 1)
        {
        
            #ifndef KERNEL_MANAGE_SUSPEND
            WLAN_TRACE_INFO(("%s 100: resume usb in timer set\n",__FUNCTION__));
            BCM43239_WIFI_PWRCTRL_RESUME();
 
            if((g_get_sta_num_func_bak != NULL)&&(g_get_sta_num_func == NULL))
            {
			/* revise the func ptr directly, as we modify the func of set_get_sta_num_func */
			//set_get_sta_num_func(g_get_sta_num_func_bak);
			g_get_sta_num_func = g_get_sta_num_func_bak;

            }
        #endif

        }

        
		WLAN_TRACE_INFO(("%s 200: timer delete\n",__FUNCTION__));		
		return ret;
	}



#else
    struct timer_list *pTimer;
    WLAN_TRACE_INFO(("%s enter\n",__FUNCTION__));
    pTimer = g_pWiFiTimer;

    if (NULL == pTimer)
    {
        WLAN_TRACE_ERROR(("%s error: g_pWiFiTimer is null\n",__FUNCTION__));  
        return -1;
    }

    /*if secods != 0: meaning app set a wifi timer*/
    if(seconds)
    {
        mod_timer(pTimer, jiffies + seconds*HZ);
        WLAN_TRACE_INFO(("%s: mod_timer success !\n", __FUNCTION__));

    }
    /*if secods == 0: meaning app clear the wifi timer*/
    else
    {
        del_timer(pTimer);
        WLAN_TRACE_INFO(("%s: del_timer  success !\n", __FUNCTION__));
    }

	return 0;
#endif

}

/**
 *@fn  void wifi_timeout_func(unsigned long)
 *@brief  timeout process func 
 *@note The timer timeout callback func. Function wifi_ap_timeout_func is a little stale and confused in design,\
 * if possible, that function would be removed.
 *@parm [in] data: the timeout callback func param, which is set by the parameter ulParam in the \
 * SOFTTIMER_CREATE_TIMER func. We use the low 4 bits to indicate the timerID, so all the timers \
 * timeout func could reuse this code.
 */
void wifi_timeout_func(unsigned long data)
{

    WIFI_USER_EVENT event;
    WLAN_TRACE_INFO(("%s: 100: enter, USE_HARD_TIMER_V7, ulParm=%ld\n",__FUNCTION__,data));
    
    event.eventId = USER_WIFI_TIMEOUT_EVENT;
    event.eventVal = data&0x0f;
    wifi_event_send_tasklet_mode((void *)&event,sizeof(WIFI_USER_EVENT), 0);
	
    WLAN_TRACE_INFO((" %s: 200:wifi_event_send_tasklet_mode is success, event.eventId = %d, event.eventVal= %d\n",__FUNCTION__ ,event.eventId, event.eventVal));


    return;
}
/**
 *@fn static void wifi_delete_timer(void * ptimer)
 *@brief  delete the timer pointer by ptimer. 
 *@note This function is used to delete the timer. Function wifi_delete_ap_timer is a little stale and confused in design,\
 * if possible, that function would be reconstructed based on the wifi_delete_timer vunc.
 *@parm [in] ptimer: the timer pointer. The value depends on timer type such as hard_timer or soft_timer
 */
static void  wifi_delete_timer(void * ptimer)
{
    if( NULL == ptimer )
    {
		WLAN_TRACE_ERROR(("%s 100:  error when deleting timer, timer doesn't exist\n",__FUNCTION__));
		return ;
    }
#ifdef USE_HARD_TIMER_V7
    SOFTTIMER_DEL_TIMER((HTIMER)ptimer);
    ptimer = NULL;
    return;
#endif	
}
/**
 *@fn static int wifi_start_timer
 *@brief  create a timer and start it.
 *@note see the note @wifi_delete_timer
 *@param [in] seconds:time out value
 *@return 0 if success, -1 reverse.
 */
static int wifi_start_timer(unsigned int seconds)
{
#ifdef USE_HARD_TIMER_V7
    int ret = 0;
    if(seconds > 0)
    {
        if(seconds > MAX_SECONDS)
        {
            seconds = MAX_SECONDS;
        }   
        ret=SOFTTIMER_CREATE_TIMER(&v7r143239_hard_timer2, seconds*1000, SOFTTIMER_NO_LOOP,(SOFTTIMER_FUNC)wifi_timeout_func,1);
        if( SOFTTIMER_OK == ret )
        {
            WLAN_TRACE_INFO(("%s 100: timer create success\n",__FUNCTION__));
        	return 0;
        }
        else
        {
	      WLAN_TRACE_ERROR(("%s 100: timer create failed\n",__FUNCTION__));
        	return -1;
        }
    }
    else
    {
        wifi_delete_timer((void *)&v7r143239_hard_timer2);
        WLAN_TRACE_INFO(("%s 200: timer delete\n",__FUNCTION__));		
	  return 0;
    }

#endif

}


/***********************************************************************************
 :          gpio code for v7 begins here
 ************************************************************************************/




#if (defined (CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))
 

int wifi_power_save_enable(void)
{

	int i = 0;
    char *radio_powersave_cmd[] = { 
		"wl radio_pwrsave_quiet_time 0",
		"wl radio_pwrsave_pps 2",
		"wl radio_pwrsave_on_time 40",
		"wl radio_pwrsave_enable 1",
		0
	};

	while(0 != radio_powersave_cmd[i])
	{
	    if(0 != wifi_tcmd(radio_powersave_cmd[i]))
        {
		    WLAN_TRACE_INFO(("%s: 100: %s: ret : after call_wifi_tcmd\n",  __FUNCTION__, radio_powersave_cmd[i]));
            return -1;
        }
		WLAN_TRACE_INFO(("%s: 200: end call_wifi_tcmd \n",  __FUNCTION__));

		++i;
	}
	return 0;

}


int wifi_power_save_disable(void)
{

	int ret = 0;
    char *cmd_seq = "wl radio_pwrsave_enable 0";
    ret = wifi_tcmd(cmd_seq);

    WLAN_TRACE_INFO(("%s: 100: %s: ret : after call_wifi_tcmd %d\n",  __FUNCTION__, cmd_seq, ret));
 
    return ret;
}



int set_wifi_gpio_rising(void)
{

	WLAN_TRACE_INFO(("%s: 50: enter!", __FUNCTION__));

    gpio_int_state_clear(BALONG_WAKEUP_HOST);
       
	if (request_irq(INT_WIFI_WAKEUP_HOST, wifi_wakeup_host_irq_v7, IRQF_SHARED, WIFI_WAKEUP_NAME, wifi_platform_device)) 
	{
		WLAN_TRACE_INFO(("%s: can't get WIFI wakeup irq.\n", __FUNCTION__));
		return -1;
	}
	
	WLAN_TRACE_INFO(("%s: 500: done!", __FUNCTION__));

    //always enable irq for sbm
    #ifdef KERNEL_MANAGE_SUSPEND
    gpio_int_unmask_set(BALONG_WAKEUP_HOST);
    #endif

    
    return 0;
}


/***********************************************************************************
 Function:          setup_wifi_wake_host_gpio_v7
 Description:      configr
 Calls:
 Input:              
 Output:            NA
 Return:            0/-1                  
 ************************************************************************************/
int config_wifi_wakeup_host_gpio_v7(void)
{
    int ret = 0;
	#if defined (CHIP_BB_6920ES) //cs is done by wangxueshong in other module
   	UINT32 *ios_ctrl_addr = (volatile UINT32 *)((UINT32)(ioremap(0x90000988,4)));
    UINT32 *ios_ctrl_13 = (volatile UINT32 *)((UINT32)(ioremap(0x90000834,4)));

	#endif

	//set gpio 5
	
	WLAN_TRACE_INFO(("%s: 50: enter!", __FUNCTION__));
	

    /* 配置GPIO复用 */ //cs is done by wangxueshong in other module
	#if defined (CHIP_BB_6920ES)
	(*ios_ctrl_addr) &= ~(1<<15);
	#endif
    
	if(gpio_request(BALONG_WAKEUP_HOST, WIFI_WAKEUP_NAME))
	{	
		WLAN_TRACE_INFO(("%s: 100: request gpio is busy!", __FUNCTION__));
		return -1;
	}

	/* 设置输入模式 */
	gpio_direction_input(BALONG_WAKEUP_HOST);

    //disable it first and enable it in last step. for both em and sbm
    gpio_int_mask_set(BALONG_WAKEUP_HOST);

    
    /* 配置GPIO下拉  */ //cs is done by wangxueshong in other module
	#if defined (CHIP_BB_6920ES)
    (*ios_ctrl_13) |= (1<<9);    
	#endif
	gpio_set_function(BALONG_WAKEUP_HOST,GPIO_INTERRUPT);

#if 0
	level = gpio_get_value(BALONG_WAKEUP_HOST);
	if(level)
	{
		ret = gpio_int_trigger_set( BALONG_WAKEUP_HOST, IRQ_TYPE_EDGE_FALLING);
	}
	else
	{
		ret = gpio_int_trigger_set(BALONG_WAKEUP_HOST,IRQ_TYPE_EDGE_RISING);
	}
#endif

    ret = gpio_int_trigger_set(BALONG_WAKEUP_HOST,IRQ_TYPE_EDGE_RISING);
    
	set_wifi_gpio_rising();
	
	WLAN_TRACE_INFO(("%s: 200: done well!", __FUNCTION__));

    /*modified for litn e533 */
    return ret;
}

int config_host_wakeup_wifi_gpio_v7(void)
{
	//set gpio 5
	if(gpio_request(BALONG_WAKEUP_WIFI, WIFI_WAKEUP_NAME))
	{	
		WLAN_TRACE_INFO(("%s: 100: request gpio is busy!", __FUNCTION__));
		return -1;
	}

	gpio_direction_output(BALONG_WAKEUP_WIFI, 1);	  
	gpio_set_function(BALONG_WAKEUP_WIFI,GPIO_NORMAL);
	
	gpio_set_value(BALONG_WAKEUP_WIFI,1);

    /*modified for litn e533 */
    return 0;
}

/*modified for litn e533 */
void set_host_wakeup_wifi_gpio_v7(int value)
{
	gpio_set_value(BALONG_WAKEUP_WIFI,value);
}


/*******************************************************************************
  Function:      wifi_wakeup_host_irq_v7(int irq, void *dev_id)
  Description:   wifi wakeup interrpt handle
  Input:         irq: irq number
                 devid: device id
  Output:        irq handle result
  Return:        NA
  Others:        NA
*******************************************************************************/
static irqreturn_t wifi_wakeup_host_irq_v7(int irq, void *dev_id)    
{
#if 1
    //int ret = 0;
	//struct platform_device *host = (struct platform_device *)dev_id;    
    unsigned int ucData = 0;  
	//int i;
    WIFI_USER_EVENT event;
	
	WLAN_TRACE_INFO(("%s 50: enter\n",__FUNCTION__));
	
    /*判断是否为wifi wake host 的中断*/
#if 1    
    gpio_int_state_get(BALONG_WAKEUP_HOST, (unsigned*)&ucData);
    if (!ucData)
    {
		WLAN_TRACE_INFO(("%s 70: IRQ_NONE, ucData %d\n",__FUNCTION__, ucData));
        return IRQ_NONE;
    }
#endif

    /*锁中断，清状态*/
	gpio_int_state_clear(BALONG_WAKEUP_HOST);

	//upper layer will use set timer to 0 to resume usb controller
    //BCM43239_WIFI_PWRCTRL_RESUME();
    
    gpio_int_mask_set(BALONG_WAKEUP_HOST); 
    
	//RESUME HSIC
    if(suspend_enable_43239 == 1)
    {
    
        
    #ifndef KERNEL_MANAGE_SUSPEND        
	        WLAN_TRACE_INFO(("%s 100: resume usb in isr\n",__FUNCTION__));
            BCM43239_WIFI_PWRCTRL_RESUME();
    
        if(g_get_sta_num_func_bak != NULL)
        {
			/* revise the func ptr directly, as we modify the func of set_get_sta_num_func */
			//set_get_sta_num_func(g_get_sta_num_func_bak);        
            g_get_sta_num_func = g_get_sta_num_func_bak;
        }
    #endif   
    }
    

    //vote for normal state    
    //wifi_sleep_vote_mode_k(WIFI_NORMAL_MODE);
	wifi_sleep_vote_normal();
	//set wifi chip wakeup gpio
	//gpio_set_value(BALONG_WAKEUP_WIFI,1);




	//send event to user space
	
	WLAN_TRACE_INFO(("%s 150: interrupt processed\n",__FUNCTION__));

    event.eventId = USER_WIFI_GPIO_EVENT;
    event.eventVal = 0;
    wifi_event_send_tasklet_mode((void *)&event,sizeof(WIFI_USER_EVENT), 1);
    WLAN_TRACE_INFO((" %s: 200:report event to user is success, event.eventId = %d, event.eventVal= %d\n",__FUNCTION__ ,event.eventId, event.eventVal));    

#ifdef KERNEL_MANAGE_SUSPEND
    gpio_int_unmask_set(BALONG_WAKEUP_HOST);
#endif
        
    return IRQ_HANDLED;
#else

    int i = 0;
	for(i =0;i<5;i++)
	{
		printk("wifi_wakeup_host_irq_v7 i=%d\n",i);
	}
    
    gpio_int_state_clear(BALONG_WAKEUP_HOST);
    return IRQ_HANDLED;
#endif
}


extern unsigned int BSP_PWRCTRL_SleepVoteLock(PWC_CLIENT_ID_E  enClientId);
extern unsigned int BSP_PWRCTRL_SleepVoteUnLock(PWC_CLIENT_ID_E  enClientId);


int wifi_sleep_vote_normal(void)
{
	BSP_PWRCTRL_SleepVoteLock(PWRCTRL_LIGHTSLEEP_WIFI);
	BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_WIFI);
	return 0;
}


int wifi_sleep_vote_mode_k(unsigned int mode)
{
	int ret = 0;
	
    WLAN_TRACE_INFO(("%s: enter, mode : %d\n",  __FUNCTION__, mode));

	
#if 1
	switch(mode)
	{
	    /*lint -e737 */
		case WIFI_NORMAL_MODE:
			ret += BSP_PWRCTRL_SleepVoteLock(PWRCTRL_LIGHTSLEEP_WIFI);
			ret += BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_WIFI);

			#ifdef KERNEL_MANAGE_SUSPEND  
	        if(down_interruptible(&driver_alive_sem))
	        {
	                WLAN_TRACE_INFO(("%s: 90: fail to down driver_alive_sem\n",  __FUNCTION__));       
	                return -1;
	        } 
			/* check to set the func pointer 
			  * in normal logic, NULL conditon check is enough.
			  * add the value comparison to avoid the func pointer's value revised by other monster */
			if(((NULL==g_get_sta_num_func)||(g_get_sta_num_func!=g_get_sta_num_func_bak))\
				&& (g_get_sta_num_func_bak != NULL))
			{
			/* revise the func ptr directly, as we modify the func of set_get_sta_num_func */
			//set_get_sta_num_func(g_get_sta_num_func_bak);
				g_get_sta_num_func = g_get_sta_num_func_bak;			
			}
				   
			up(&driver_alive_sem);	   
			#endif
				   
			break;

		case WIFI_LIGTH_SLEEP:
			ret += BSP_PWRCTRL_SleepVoteUnLock(PWRCTRL_LIGHTSLEEP_WIFI);
			ret += BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_WIFI);
			break;

		case WIFI_DEEP_SLEEP:
			ret += BSP_PWRCTRL_SleepVoteUnLock(PWRCTRL_LIGHTSLEEP_WIFI);
			ret += BSP_PWRCTRL_SleepVoteUnLock(PWRCTRL_SLEEP_WIFI);
                    /* unset the func pointer to avoid invalid access */
                    //g_get_sta_num_func=NULL;
			#ifdef KERNEL_MANAGE_SUSPEND  
	        if(down_interruptible(&driver_alive_sem))
	        {
	            WLAN_TRACE_INFO(("%s: 90: fail to down driver_alive_sem\n",  __FUNCTION__));       
	            return -1;
	        }
			/* revise the func ptr directly, as we modify the func of set_get_sta_num_func */			
			#if 0
			if(g_get_sta_num_func != NULL)
			{g_get_sta_num_func_bak = g_get_sta_num_func;} 					
			
			set_get_sta_num_func((unsigned int)NULL); 			
			#else
			g_get_sta_num_func = NULL;
			#endif
			up(&driver_alive_sem);	   
			#endif
			break;

		default:
			ret = -1;
			break;
        /*lint +e737 */
	}
    
	return ret;
#endif

    /*modified for lint e527 
	return ret;*/

}
	
#define WIFI_ONLY_PA_MODE 0
	
int wifi_set_pa_mode(int wifiPaMode)
{
	if(WIFI_ONLY_PA_MODE == wifiPaMode)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}	

int wifi_get_pa_cur_mode(void)
{
		return 0;
}	


#endif


#ifdef CONFIG_WLAN_IF_BCM43239

static void power_off_clear_msgqueue()
{
	WIFI_MSG_CTRL_STRU *pCtrl;
	WIFI_EVENT_NODE_STRU *pNode;

	WLAN_TRACE_INFO(("%s:enter\n",__FUNCTION__));
	
	/*restore the contrl block*/		
	pCtrl = wifi_poweron_msg_block;

	while (!wifi_eventq_is_empty(pCtrl))
	{
		pNode = wifi_event_dequeue(pCtrl);
		if (NULL == pNode)
		{
			WLAN_TRACE_INFO(("%s: dequeue a null node\n",__FUNCTION__));
			break;
		}

		WLAN_TRACE_INFO(("%s: dequeue a node\n",__FUNCTION__));


		// Release this event entry
		wifi_event_node_free(pNode);

	}

}


static void schedule_poweron_timer()
{

	WIFI_USER_EVENT event;
	WIFI_MSG_CTRL_STRU *pCtrl;
	WIFI_EVENT_NODE_STRU *pNode;
	int count = 0;

	if (g_power_on_timer_scheduled == 1)
	{
		WLAN_TRACE_INFO(("%s: ====>already scheduled\n", __FUNCTION__));
		return;
	}

	if (poweron_timer.function == NULL)
	{
		WLAN_TRACE_INFO(("%s: fail to start power on timer\n", __FUNCTION__));
		return;
	}

	g_power_on_timer_scheduled = 1;

	WLAN_TRACE_INFO(("%s: start power on timer\n", __FUNCTION__));

    /* We need to send power on event to msg_monitor to tell it to query station message */
    event.eventId = USER_WIFI_POWER_ON_EVENT;
    event.eventVal = 0;

	WLAN_TRACE_INFO(("%s: enter\n",__FUNCTION__));

	/*restore the contrl block*/		
	pCtrl = wifi_poweron_msg_block;

	pNode = wifi_event_node_alloc(pCtrl, &event, sizeof(event));
	if (unlikely(NULL == pNode))    /*lint !e730 */
	{
		WLAN_TRACE_ERROR((KERN_ERR "%s: alloc node failed\n",__FUNCTION__));
        /*modified for lint e110 */
    	return;
	}

	wifi_event_enqueue(pCtrl,pNode);

    WLAN_TRACE_INFO((" %s: wifi_event_send_tasklet_mode is success, event.eventId = %d, event.eventVal= %d\n",__FUNCTION__ ,event.eventId, event.eventVal));

	count = atomic_read(&g_poweron_timer_count);

	WLAN_TRACE_INFO((" %s: count = %d\n",__FUNCTION__ , count));

	if (count > 0)
	{
		if (count == POWERON_TIMER_MAX_LOOP)
		{
			if (g_the_first_power_on)
			{
				/* If it's system startup, we need to wait for more time */
				g_the_first_power_on = 0;
				mod_timer(&poweron_timer, jiffies + 15 * HZ);

				WLAN_TRACE_INFO((" %s: =====> first power on \n",__FUNCTION__ ));
			}
			else 
			{
				/* Here is poweron key to trigger WIFI */
				mod_timer(&poweron_timer, jiffies + 10 * HZ);

				WLAN_TRACE_INFO((" %s: =====> first WIFI power on \n",__FUNCTION__ ));
			}

		}
		else 
		{
			/* timer to trigger message sent */
			mod_timer(&poweron_timer, jiffies + 5 * HZ);

			WLAN_TRACE_INFO((" %s: =====> other power on\n",__FUNCTION__ ));
		}
	}

	atomic_dec(&g_poweron_timer_count);

}


/***********************************************************************************
 Function:          wifi_poweron_event_send
 Description:      send power on event to user space
 Calls:
 Input:             workqueue
 Output:            NA
 Return:            NA
                  
 ************************************************************************************/
static void wifi_poweron_event_send(void)
{
	WIFI_MSG_CTRL_STRU *pCtrl;
	WIFI_EVENT_NODE_STRU *pNode;
	struct sk_buff *skb;
	struct nlmsghdr *nlh;	
	int msgSize, ret = -1;
	int count = 0;

	WLAN_TRACE_INFO(("%s:enter\n",__FUNCTION__));
	
	/*restore the contrl block*/		
	pCtrl = wifi_poweron_msg_block;

	while (!wifi_eventq_is_empty(pCtrl))
	{
		pNode = wifi_event_dequeue(pCtrl);
		if (NULL == pNode)
		{
			WLAN_TRACE_INFO(("%s: dequeue a null node\n",__FUNCTION__));
			break;
		}

		WLAN_TRACE_INFO(("%s: dequeue a node\n",__FUNCTION__));

		/*
		 * node->len: size of (node->data), is event message length
		 * size: size of (skb) 
		 * size = sizeof(*nlh) + align pad + node->len + aligh pad
		 * 
		 * alloc skb here
		 * But, NOT release skb here
		 * 
		 */
		msgSize = NLMSG_SPACE(pNode->len);
		skb = alloc_skb((unsigned int)msgSize, GFP_ATOMIC);
		if (NULL == skb)
		{
			WLAN_TRACE_ERROR((KERN_ALERT "%s: alloc skb failed\n",__FUNCTION__));
			return;
		}

		// Use "size - sizeof(*nlh)" here (incluing align pads)
		nlh = nlmsg_put(skb, 0, 0, 0, (unsigned int)msgSize - sizeof(*nlh), 0);

		NETLINK_CB(skb).pid = 0;        /*lint !e545 */
		NETLINK_CB(skb).dst_group = 0;  /*lint !e545 */
		memcpy (NLMSG_DATA(nlh), pNode->data, (unsigned int)pNode->len);

		// Release this event entry
		wifi_event_node_free(pNode);

		// Send message to user space
		if (pCtrl->dev_sock != NULL)
		{
			ret = netlink_unicast(pCtrl->dev_sock, skb, pCtrl->user_pid, 0);
			if (ret < 0) 
			{
				WLAN_TRACE_ERROR((KERN_ERR "%s: 800:msg send failed, ret = %d\n",__FUNCTION__, ret));

				WLAN_TRACE_ERROR((KERN_ERR "%s: 850:dev_sock = %p, user_pid = %u\n",__FUNCTION__, pCtrl->dev_sock, pCtrl->user_pid));			
				//kfree_skb(skb);
				WLAN_TRACE_INFO((KERN_ERR "%s: 900: end to kfree_skb\n",__FUNCTION__));
				return;
			}

			WLAN_TRACE_INFO(( "%s: 1000: msg send over\n",__FUNCTION__));
		}
	}

	count = atomic_read(&g_poweron_timer_count);

	if (0 < count)
	{
		schedule_poweron_timer();
	}

	return;
}


static void poweron_report_timer(void)
{
	int count = 0;

	g_power_on_timer_scheduled = 0;

	WLAN_TRACE_ERROR(("%s: enter",__FUNCTION__));

	count = atomic_read(&g_poweron_timer_count);
	if (count == 0) 
	{
		return;
	}

	wifi_poweron_event_send();

}


static void init_poweron_timer(void)
{
	if (poweron_timer.function != NULL)
		return;		/* already started */

	init_timer(&poweron_timer);
	poweron_timer.expires = jiffies + 600 * HZ;
	poweron_timer.data = 0;
    /*modified for lint e546 */
	poweron_timer.function = (void *)&poweron_report_timer;
    /*modified for lint e546 */    
	add_timer(&poweron_timer);
}


/***********************************************************************************
 Function:          wifi_netlink_poweron_init
 Description:      we need a queue to hold poweron event, but we don't want another netlink sock,
                   so both user event and poweron event use the same netlink sock and user pid.
                   this function must be called after netlink sock and user pid is initialized!
 Calls:
 Input:             NA
 Output:            NA
 Return:            OK/ERROR
                  
 ************************************************************************************/
int wifi_netlink_poweron_init(void)
{

	WIFI_MSG_CTRL_STRU *pCtrl;

	WLAN_TRACE_INFO(("%s:enter\n",__FUNCTION__));

	if (NULL == (pCtrl =(WIFI_MSG_CTRL_STRU*) kmalloc(sizeof(WIFI_MSG_CTRL_STRU), GFP_ATOMIC)))
	{
		WLAN_TRACE_ERROR(("%s: kmalloc failed!\n",__FUNCTION__));
		return -1;
	}

	/*init it*/
	INIT_LIST_HEAD(&(pCtrl->event_queue));
	pCtrl->queue_len = 0;
	spin_lock_init(&(pCtrl->queue_lock));

	/* we just inherit them from user event */
	pCtrl->dev_sock = wifi_user_msg_block->dev_sock;
	pCtrl->user_pid = wifi_user_msg_block->user_pid;

	if (NULL == pCtrl->dev_sock)
	{
		/* just set it to default */
		pCtrl->dev_sock = NULL;
		pCtrl->user_pid = 0xFFFFFFFF;
		kfree(pCtrl);
		WLAN_TRACE_ERROR(("%s: WIFI_POWERON_MSG_CTRL_INIT failed!\n",__FUNCTION__));
		return -1;
	}

	/*record the contrl block*/	
	wifi_poweron_msg_block = pCtrl;

	return 0;
}


/***********************************************************************************
 Function:         wifi_netlink_poweron_uninit
 Description:      uninit power on message queue control block
 Calls:
 Input:             NA
 Output:            NA
 Return:            NA
                  
 ************************************************************************************/

void wifi_netlink_poweron_uninit(void)
{
	WIFI_MSG_CTRL_STRU *pCtrl;
	
	WLAN_TRACE_INFO(("%s:enter\n",__FUNCTION__));

	/*restore the contrl block*/		
	pCtrl = wifi_poweron_msg_block;

	if (pCtrl)
	{
		/* just set it to default */
		pCtrl->dev_sock = NULL;
		pCtrl->user_pid = 0xFFFFFFFF;

		kfree(pCtrl);
		wifi_poweron_msg_block = NULL;		
	}

	return;
}


#endif


//for test
int test_wifi_ap_start_timer (unsigned int seconds)
{
	return wifi_ap_start_timer(seconds);
}


#define WIFI_AT_DEBUG

static ssize_t wifi_at_device_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
#if defined (WIFI_AT_DEBUG)    
    char *pCmdStr = NULL;    
    unsigned int totalPkts,goodPkts,badPkts,ap_seconds;
#endif

	unsigned int pwr_save_mode = 0;


    int cmd;

    if ((NULL == dev) || (NULL == attr) || (NULL == buf))
    {
        WLAN_TRACE_ERROR(("%s: wifi_at_dev: invalid para! \n", __FUNCTION__));
        return count;
    }
    WLAN_TRACE_INFO(("%s: receive msg from user is %s\n",  __FUNCTION__, buf));
#if defined (WIFI_AT_DEBUG)
    //WLAN_TRACE_INFO(("%s: wifi_at_dev: input string is %s \n", __FUNCTION__, buf));    
    
    if( WL_DEBUG_CMD_STR == *buf)
    {
        /*example: d0 wl ver*/
        pCmdStr = (char *)(buf + 3);

        switch(*(buf + 1))
        {
            case '0': 
                if (('w'==*pCmdStr) && ('l'==*(pCmdStr+1)))
                {
                    wifi_tcmd(pCmdStr);
                }
                else
                {
                    WLAN_TRACE_ERROR(("%s: wifi_at_dev: dbg cmd is error!\n", __FUNCTION__));
                }                
                break;
                
            case '1' :
                WLAN_TRACE_INFO(("%s: wifi_get_tcmd_mode() = %d \n",  __FUNCTION__, wifi_get_tcmd_mode()));                
                break;
            
            case '2' :
                WLAN_TRACE_INFO(("%s: wifi_get_status() = %d \n", __FUNCTION__, wifi_get_status()));                
                break;
                
            case '3' :
                (void)wifi_get_rx_detail_report(&totalPkts,&goodPkts,&badPkts);
                WLAN_TRACE_INFO(("%s: wifi_get_rx_detail_report: totalPkts = %u, goodPkts = %u, badPkts = %u \n", __FUNCTION__, totalPkts, goodPkts, badPkts));                                
                break;
                
            case '4' :
                wifi_power_on();
                break;

            case '5' :
                wifi_power_off();
                break;
            
            default:
                WLAN_TRACE_ERROR(("%s: wifi_at_dev: dbg cmd is error!\n", __FUNCTION__));
                break;

        }
        
        return count;    
    }
#endif

    if(sscanf(buf,"%d",&cmd) != 1)
    {
        WLAN_TRACE_ERROR(("%s, wifi_at_dev: %s is error!\n", __FUNCTION__,buf));
        return count;
    }
    //WLAN_TRACE_INFO(("%s: cmd = %d\n", __FUNCTION__, cmd));
    switch(cmd)
    {
        case WL_START_CMD: 
            /*clear tx & rx mode*/
            g_iWifiTcmdRxMode = 0;
            g_iWifiTcmdTxMode = 0;
            break;

        case  WL_RX_CMD:
            g_iWifiTcmdRxMode =1;
            break;

        case  WL_TX_CMD:
            g_iWifiTcmdTxMode =1;
            break;
            

        case  WL_RX_REPORT_CMD:
            sscanf(buf,WL_RX_REPORT_STR,&g_uiRxTotalPkts,&g_uiRxBadPkts) ;
            break;

        case  WL_RX_PACKET_CMD:
            sscanf(buf,WL_RX_PACKET_STR,&g_uiUcastPkts,&g_uiMcastPkts) ;
            break;
            
        case WIFI_TIMER_SET_CMD:
            sscanf(buf,WIFI_TIMER_SET_CMD_STR, &ap_seconds);
            if(0 != wifi_start_timer(ap_seconds))
            {
                WLAN_TRACE_ERROR(("%s:290: failed to execute wifi_start_timer!\n",__FUNCTION__));
            }
            break;
        case  WIFI_AP_SHUTDOWN_TIMEOUT_SET_CMD:    	
            sscanf(buf,WIFI_AP_SHUTDOWN_TIMEOUT_SET_STR,&ap_seconds) ;
            if(0 != wifi_ap_start_timer(ap_seconds))
            {
                WLAN_TRACE_ERROR(("%s: 300:wifi_at_dev: failed to execute program wifi_ap_start_timer! \n", __FUNCTION__)); 
            }
			
            //WLAN_TRACE_INFO(("%s: 400:end to call wifi_ap_start_timer\n", __FUNCTION__));
            break; 

		case  WIFI_AP_SHUTDOWN_43239:    	

            if(0 != wifi_power_off())
            {
                WLAN_TRACE_ERROR(("%s, 500: failed to execute wifi_power_off! \n", __FUNCTION__)); 
            }
            WLAN_TRACE_INFO(("%s: 600: end to execute wifi_power_off\n", __FUNCTION__));
            break; 

		case  WIFI_AP_UP_43239:    	

            if(0 != wifi_power_on_full(WIFI_CMD_MOD_NORMAL))
            {
                WLAN_TRACE_ERROR(("%s, 700: failed to execute wifi_power_on_full! \n", __FUNCTION__)); 
            }
            WLAN_TRACE_INFO(("%s: 800: end to execute wifi_power_on_full\n", __FUNCTION__));
            break; 

        case  WIFI_SLEEP_VOTE_43239:
            sscanf(buf,WIFI_SLEEP_VOTE_43239_STR,&pwr_save_mode) ;
            if(0 != wifi_sleep_vote_mode_k(pwr_save_mode))
            {
                WLAN_TRACE_ERROR(("%s, 900:wifi_at_dev: failed to execute program wifi_sleep_vote_mode_k! \n", __FUNCTION__)); 
            }
			
            WLAN_TRACE_INFO(("%s: 1000:end to call wifi_sleep_vote_mode_k\n", __FUNCTION__));
            break; 
			
        default:
            WLAN_TRACE_ERROR(("%s: 1200:wifi_at_dev: %s is not support!\n", __FUNCTION__, buf));
            break;
    }

    return count;
}
DEVICE_ATTR(wifi_at_dev, 0666, NULL, wifi_at_device_store);

static struct attribute *wifi_at_dev_attributes[] = {
    &dev_attr_wifi_at_dev.attr,
    NULL
};

static const struct attribute_group wifi_at_dev_group = {
    .attrs = wifi_at_dev_attributes,
};

int wifi_test_register(void)
{
    int ret=0;
    wifi_platform_device = platform_device_register_simple(WIFI_AT_DEVICE,-1, NULL, 0);

    if(wifi_platform_device==NULL)
    {
        WLAN_TRACE_ERROR(("%s: create wifi_platform_device failed.\n",  __FUNCTION__));
        return -1;
    }
    
    ret = sysfs_create_group(&wifi_platform_device->dev.kobj, &wifi_at_dev_group);
    if (0 != ret)
    {
        WLAN_TRACE_ERROR(("%s: create sdio sys filesystem node failed.\n", __FUNCTION__));
        return -1;
    }
    
    return 0;
}




//extern int wifi_test_register(void);
static int __init wifi_at_init(void)
{
    int ret=0;
    ret = wifi_test_register();

    ret |= wifi_netlink_chunnel_init(); // create netlink for wifi
    ret |= wifi_create_ap_timer();   // create timer for wifi
    g_WIFI_LNA = wifi_get_is_LNA();
#ifdef CONFIG_WLAN_IF_BCM43239
	init_poweron_timer();
	ret |= wifi_netlink_poweron_init();
#endif

#if (defined (CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))		
    /*博通B4版本芯片，需要在WIFI启动时上拉GPIO-1(GPIO-2-12)*/
    ret |= config_host_wakeup_wifi_gpio_v7();//set wifi alive fiest

#ifdef NO_NV_RADIO_PWR_CTRL
    suspend_enable_43239 = 1;
#else
    wifi_suspend_nv_read();
#endif



    init_MUTEX(&driver_alive_sem);
    ret |= config_wifi_wakeup_host_gpio_v7();
    
#endif

    return ret;
	
}

static void __exit wifi_at_exit(void)
{
    sysfs_remove_group(&wifi_platform_device->dev.kobj, &wifi_at_dev_group);
    wifi_platform_device = NULL;
    platform_device_unregister(wifi_platform_device);

#ifdef CONFIG_WLAN_IF_BCM43239
		wifi_netlink_poweron_uninit();
		if (poweron_timer.function != NULL)
		{
			del_timer(&poweron_timer);
		}
#endif

    wifi_netlink_chunnel_uninit( );
    wifi_delete_ap_timer( );
    
    return;
}


/***********************************************************************************
 Function:          WiFi_DrvSetRxFlowCtrl
 Description:       set wifi rx flow control flag, if this flag is set, wifi driver 
                    will drop packets to PS
 Calls:
 Input:             para1 : reserved
                    para2 : reserved
 Output:            NA
 Return:            0: OK
                  
 ************************************************************************************/
int g_wifiRxFlowCtrlFlag = 0;

unsigned long WiFi_DrvSetRxFlowCtrl    (unsigned long para1, unsigned long para2)
{
    g_wifiRxFlowCtrlFlag = 1;
    return 0;
}


/***********************************************************************************
 Function:          WiFi_DrvClearRxFlowCtrl
 Description:       set wifi rx flow control flag, if this flag is clear, wifi driver 
                    will send packets to PS
 Calls:
 Input:             para1 : reserved
                    para2 : reserved
 Output:            NA
 Return:            0: OK
                  
 ************************************************************************************/
unsigned long WiFi_DrvClearRxFlowCtrl  (unsigned long para1, unsigned long para2)
{
    g_wifiRxFlowCtrlFlag = 0;
    return 0;
}


 /***********************************************************************************
 Function:          wifi_event_node_alloc
 Description:      Construct a buffer node
 Calls:
 Input:              pCtrl: control block
 			   Data: date buffer
 			   len: data len
 Output:            NA
 Return:            NULL or node
                  
 ************************************************************************************/
static WIFI_EVENT_NODE_STRU *wifi_event_node_alloc(WIFI_MSG_CTRL_STRU *pCtrl,void * data, int len)
{
	WIFI_EVENT_NODE_STRU *pNode;

	WLAN_TRACE_INFO(("%s:enter\n",__FUNCTION__));

	if (pCtrl->queue_len > WIFI_MAX_EVENT_QUEUE_LEN)
	{
		WLAN_TRACE_ERROR(("%s: max queue len is reached! \n",__FUNCTION__));
		return NULL;
	}
	
    
	pNode = (WIFI_EVENT_NODE_STRU *)kmalloc(((unsigned int)len + sizeof(WIFI_EVENT_NODE_STRU)), GFP_ATOMIC);
	if (NULL == pNode) 
	{
		WLAN_TRACE_ERROR(("%s: kmalloc failed! \n",__FUNCTION__));
		return NULL;
	}

	pNode->len = len;
	memcpy(pNode->data, data, (unsigned int)len);
	
	return pNode;
}



/***********************************************************************************
Function:		   wifi_event_node_free
Description:	   free the node
Calls:
Input:	         pNote
Output: 		   NA
Return: 		   NA
				 
************************************************************************************/

static inline void wifi_event_node_free(WIFI_EVENT_NODE_STRU *pNode)
{
	WLAN_TRACE_INFO(("%s:enter\n",__FUNCTION__));
	kfree(pNode);
	return;
}

/***********************************************************************************
Function:		   wifi_event_enqueue
Description:	   inset the note into queue
Calls:
Input:	         pCtrl: control block
			 pNote: wifi event note
Output: 		   NA
Return: 		   NA
				 
************************************************************************************/

static inline void wifi_event_enqueue(WIFI_MSG_CTRL_STRU *pCtrl,WIFI_EVENT_NODE_STRU *pNode)
{
	unsigned long flags;

	WLAN_TRACE_INFO(("%s:enter\n",__FUNCTION__));

	spin_lock_irqsave(&(pCtrl->queue_lock), flags);
	list_add_tail(&pNode->list, &(pCtrl->event_queue));
	pCtrl->queue_len++;
	spin_unlock_irqrestore(&(pCtrl->queue_lock), flags);
}

/***********************************************************************************
Function:		   wifi_event_dequeue
Description:	  dequeue a note for queue
Calls:
Input:	         pCtrl: control block
Output: 		   NA
Return: 		   event node or NULL
				 
************************************************************************************/

static inline WIFI_EVENT_NODE_STRU *wifi_event_dequeue(WIFI_MSG_CTRL_STRU *pCtrl)
{
	WIFI_EVENT_NODE_STRU *pNode = NULL;
	unsigned long flags;

	WLAN_TRACE_INFO(("%s:enter\n",__FUNCTION__));

	spin_lock_irqsave(&(pCtrl->queue_lock), flags);/*lint !e26 */
	pNode = list_first_entry(&(pCtrl->event_queue), WIFI_EVENT_NODE_STRU, list);
	if (NULL == pNode)
	{
		spin_unlock_irqrestore(&(pCtrl->queue_lock), flags);/*lint !e530 */
		return NULL;
	}
	
	list_del(&pNode->list);	
	pCtrl->queue_len--;	
	spin_unlock_irqrestore(&(pCtrl->queue_lock), flags);
	
	return pNode;
}


/***********************************************************************************
 Function:          wifi_eventq_is_empty
 Description:      check the event queue weather is empty
 Calls:
 Input:              control block
 Output:            NA
 Return:            TRUE : empty
 			   FALSE: not empty
                  
 ************************************************************************************/
static inline int wifi_eventq_is_empty(WIFI_MSG_CTRL_STRU *pCtrl)
{
	unsigned long flags;
	int ret;
	
	WLAN_TRACE_INFO(("%s:enter\n",__FUNCTION__));

	spin_lock_irqsave(&(pCtrl->queue_lock), flags);	/*lint !e26 */
	ret = list_empty(&(pCtrl->event_queue));
	spin_unlock_irqrestore(&(pCtrl->queue_lock),flags);/*lint !e530 */

	return ret;
}


/***********************************************************************************
 Function:          wifi_event_input
 Description:      use to proces msg from APP
 Calls:
 Input:              pCtrl: control block
 			   _skb: date
 Output:            NA
 Return:            NA                  
 ************************************************************************************/
static void wifi_event_input(struct sk_buff *__skb)
{
	struct sk_buff *skb;
	struct nlmsghdr *nlh;
	WIFI_MSG_CTRL_STRU *pCtrl;

	WLAN_TRACE_INFO(("%s:enter\n",__FUNCTION__));

	if (unlikely (NULL == (pCtrl = wifi_user_msg_block)))/*lint !e730 */
	{
		WLAN_TRACE_ERROR(("%s:pCtrl is NULL!\n",__FUNCTION__));
		return;
	}

	skb = skb_get(__skb);
	if (skb->len < NLMSG_SPACE(0))
	{
		WLAN_TRACE_ERROR(("%s: skb->len isn't enough!\n",__FUNCTION__));	
		return;
	}

	nlh = nlmsg_hdr(skb);

	if (!WIFI_EVENT_PATH_IS_READY(pCtrl))
	{
		WLAN_TRACE_ERROR(("%s record user_pid = 0x%u!\n",__FUNCTION__,nlh->nlmsg_pid));
		// Store the user space daemon pid
		pCtrl->user_pid = nlh->nlmsg_pid;

#ifdef CONFIG_WLAN_IF_BCM43239
		if (wifi_poweron_msg_block != NULL)
		{
			wifi_poweron_msg_block->user_pid = nlh->nlmsg_pid;
		}
#endif
		
		// Send device events, reported before daemon start
		schedule_delayed_work(&(pCtrl->event_work),WIFI_DELAY_FOR_NL_SEND);	
	}

	/*
	you could add message process here
	pMsg = NLMSG_DATA(nlh);
	*/
	return;
}


/***********************************************************************************
 Function:          wifi_event_send
 Description:      send event to user space
 Calls:
 Input:             workqueue
 Output:            NA
 Return:            NA
                  
 ************************************************************************************/
static void wifi_event_send(struct work_struct *work)
{
	WIFI_MSG_CTRL_STRU *pCtrl;
	WIFI_EVENT_NODE_STRU *pNode;
	struct sk_buff *skb;
	struct nlmsghdr *nlh;	
	int msgSize, ret;

	WLAN_TRACE_INFO(("%s:enter\n",__FUNCTION__));
	
	/*restore the contrl block*/		
	pCtrl = wifi_user_msg_block;

	while (!wifi_eventq_is_empty(pCtrl))
	{
		pNode = wifi_event_dequeue(pCtrl);
		if (NULL == pNode)
		{
			WLAN_TRACE_INFO(("%s: dequeue a null node\n",__FUNCTION__));
			break;
		}

		WLAN_TRACE_INFO(("%s: dequeue a node\n",__FUNCTION__));

		/*
		 * node->len: size of (node->data), is event message length
		 * size: size of (skb) 
		 * size = sizeof(*nlh) + align pad + node->len + aligh pad
		 * 
		 * alloc skb here
		 * But, NOT release skb here
		 * 
		 */
		msgSize = NLMSG_SPACE(pNode->len);
		skb = alloc_skb((unsigned int)msgSize, GFP_ATOMIC);
		if (NULL == skb)
		{
			WLAN_TRACE_ERROR((KERN_ALERT "%s: alloc skb failed\n",__FUNCTION__));
			return;
		}

		// Use "size - sizeof(*nlh)" here (incluing align pads)
		nlh = nlmsg_put(skb, 0, 0, 0, (unsigned int)msgSize - sizeof(*nlh), 0);

		NETLINK_CB(skb).pid = 0;        /*lint !e545 */
		NETLINK_CB(skb).dst_group = 0;  /*lint !e545 */
		memcpy (NLMSG_DATA(nlh), pNode->data, (unsigned int)pNode->len);

		// Release this event entry
		wifi_event_node_free(pNode);

		// Send message to user space
		ret = netlink_unicast(pCtrl->dev_sock, skb, pCtrl->user_pid, 0);
		if (ret < 0) 
		{
			WLAN_TRACE_ERROR((KERN_ERR "%s: 800:msg send failed, ret = %d\n",__FUNCTION__, ret));

			WLAN_TRACE_ERROR((KERN_ERR "%s: 850:dev_sock = %p, user_pid = %u\n",__FUNCTION__, pCtrl->dev_sock, pCtrl->user_pid));			
			//kfree_skb(skb);
			WLAN_TRACE_INFO((KERN_ERR "%s: 900: end to kfree_skb\n",__FUNCTION__));
			return;
		}
		
		WLAN_TRACE_INFO(( "%s: 1000: msg send over\n",__FUNCTION__));	
	}

	return;
}

/***********************************************************************************
 Function:          wifi_event_send
 Description:      report a message  to user space
 Calls:
 Input:             data: data buffer
 			  len: date len
 Output:            NA
 Return:            OK or ERROR
                  
 ************************************************************************************/
int wifi_event_report(void *data, int len) 
{
	WIFI_MSG_CTRL_STRU *pCtrl;
	WIFI_EVENT_NODE_STRU *pNode;

	WLAN_TRACE_INFO(("%s:enter\n",__FUNCTION__));

	/*restore the contrl block*/		
	pCtrl = wifi_user_msg_block;

	/*check paramate*/
	if (unlikely(NULL == pCtrl) || (!WIFI_EVENT_PATH_IS_READY(pCtrl))) /*lint !e730 */
	{
		WLAN_TRACE_ERROR(("%s msg path isn't ready",__FUNCTION__));
		return -1;
	}

	pNode = wifi_event_node_alloc(pCtrl,data, len);
	if (unlikely(NULL == pNode)) /*lint !e730 */
	{
		WLAN_TRACE_ERROR((KERN_ERR "%s: alloc node failed\n",__FUNCTION__));
    	return  -1;
	}
	
	wifi_event_enqueue(pCtrl,pNode);

	/*
	 * Only put event into buffer queue.
	 */
	schedule_delayed_work(&(pCtrl->event_work), WIFI_DELAY_FOR_NL_SEND);	
	
	return 0;		
}



void wifi_send_event_tasklet(unsigned long unused)
{
	WLAN_TRACE_ERROR(("%s:100: enter",__FUNCTION__));
	wifi_event_send(NULL);
	
}

DECLARE_TASKLET(wifi_send_event_name, wifi_send_event_tasklet,0);


void wifi_event_resume_tasklet(unsigned long unused)
{
	WLAN_TRACE_ERROR(("%s:100: enter",__FUNCTION__));

    //set wifi resume
    //set parameter as 0 to resume
    //wifi_suspend_43239(0);
    //wifi_set_awake_host_gpio(0);
	wifi_event_send(NULL);
}

DECLARE_TASKLET(wifi_event_resume_tasklet_name, wifi_event_resume_tasklet,0);



/***********************************************************************************
 Function:          wifi_event_send_tasklet_mode
 Description:      report a message  to user space
 Calls:
 Input:             data: data buffer
 			  len: date len
 			  suspend_mode, whether to resume wifi chip
 Output:            NA
 Return:            OK or ERROR
                  
 ************************************************************************************/
int wifi_event_send_tasklet_mode(void *data, int len, int resume_mode) 
{
	WIFI_MSG_CTRL_STRU *pCtrl;
	WIFI_EVENT_NODE_STRU *pNode;

	WLAN_TRACE_INFO(("%s:100:enter\n",__FUNCTION__));

	/*restore the contrl block*/		
	pCtrl = wifi_user_msg_block;

	/*check paramate*/
	if (unlikely(NULL == pCtrl) || (!WIFI_EVENT_PATH_IS_READY(pCtrl))) /*lint !e730 */
	{
		WLAN_TRACE_ERROR(("%s:200: msg path isn't ready",__FUNCTION__));
		return -1;
	}

	pNode = wifi_event_node_alloc(pCtrl,data, len);
	if (unlikely(NULL == pNode)) /*lint !e730 */
	{
		WLAN_TRACE_ERROR((KERN_ERR "%s: 300: alloc node failed\n",__FUNCTION__));
    	return  -1;
	}
	
	wifi_event_enqueue(pCtrl,pNode);

	/*
	 * Only put event into buffer queue.
	 */
	if(resume_mode == 0)
    {        
        tasklet_schedule(&wifi_send_event_name);    
    }   
    else if(resume_mode == 1)
    {
        tasklet_schedule(&wifi_event_resume_tasklet_name);    
    }
	
	return 0;		
}





/***********************************************************************************
 Function:          wifi_netlink_chunnel_init
 Description:      initialization a netlink for wifi pass event to user APP
 Calls:
 Input:             NA
 Output:            NA
 Return:            OK/ERROR
                  
 ************************************************************************************/
int wifi_netlink_chunnel_init(void)
{

	WIFI_MSG_CTRL_STRU *pCtrl;

	WLAN_TRACE_INFO(("%s:enter\n",__FUNCTION__));

	if (NULL == (pCtrl =(WIFI_MSG_CTRL_STRU*) kmalloc(sizeof(WIFI_MSG_CTRL_STRU), GFP_ATOMIC)))
	{
		WLAN_TRACE_ERROR(("%s: kmalloc failed!\n",__FUNCTION__));
		return -1;
	}

	/*init netlink and other resource*/
	WIFI_MSG_CTRL_INIT(pCtrl);

	if (NULL == pCtrl->dev_sock)
	{
		/*if failed, release the resource*/
		WIFI_MSG_CTRL_UNINIT(pCtrl);
		kfree(pCtrl);		
		WLAN_TRACE_ERROR(("%s: WIFI_MSG_CTRL_INIT failed!\n",__FUNCTION__));	
		return -1;
	}

	/*record the contrl block*/		
	wifi_user_msg_block = pCtrl;

	return 0;
}


/***********************************************************************************
 Function:          wifi_netlink_chunnel_uninit
 Description:      uninit the netlink resource
 Calls:
 Input:             NA
 Output:            NA
 Return:            NA
                  
 ************************************************************************************/

void wifi_netlink_chunnel_uninit(void)
{
	WIFI_MSG_CTRL_STRU *pCtrl;
	
	WLAN_TRACE_INFO(("%s:enter\n",__FUNCTION__));

	/*restore the contrl block*/		
	pCtrl = wifi_user_msg_block;

	if (pCtrl)
	{
		WIFI_MSG_CTRL_UNINIT(pCtrl);
		kfree(pCtrl);
		wifi_user_msg_block = NULL;		
	}

	return;
}


//#define WIFI_NETLINK_DEBUG
#ifdef WIFI_NETLINK_DEBUG

void wifi_netlink_dump(char *str)
{
    WLAN_TRACE_INFO(("%s: dump %s \n", __FUNCTION__, str));
	
	wifi_event_report((void *)str,strlen(str) + 1);

	return;
}

void wifi_netlink_example()
{
	wifi_netlink_dump("netlink example\n");
	return;
}

#endif
/*****************************************************************************
? ? ?  : WifiDataReservedTail
 ????  : WIFI?????????
 ????  : usLen - ????????Len
 ????  : ?
 ? ? ?  : ???????????
 ????  :
 ????  :

 ????      :
  1.?    ?   : 2011?12?22?
    ????   : ?????

*****************************************************************************/
unsigned int BSP_WifiDataReservedTail(unsigned int len)
{

#ifndef FEATURE_WIFI
   return 0;
#else
   unsigned int ulLen;
   if(len > SKB_DHD_BLOCKSIZE)
   {
        if(len&((1<<SKB_DHD_BLOCKSIZE_ROUNDUP)-1))
        {
           ulLen = ((len>>SKB_DHD_BLOCKSIZE_ROUNDUP)+ 1)<<SKB_DHD_BLOCKSIZE_ROUNDUP;
           return (ulLen - len) + SKB_DHD_TAIL_RESERVED;
        }
        else
        {
           return SKB_DHD_TAIL_RESERVED;

        }
   }
   else
   {
        if(len&((1<<SKB_DHD_SDALIGN_ROUNDUP)-1))
        {
          ulLen = ((len>>SKB_DHD_SDALIGN_ROUNDUP)+ 1)<<SKB_DHD_SDALIGN_ROUNDUP;
          return (ulLen - len) + SKB_DHD_TAIL_RESERVED;
        }
        else
        {
          return SKB_DHD_TAIL_RESERVED;
        }
   }
#endif
}

/******************************************************
  Function:  bsp_get_factory_mode
  Description: identify factory nv 
  Input:  none
  output: 1: 正常模式
             0: 产线模式
  author: g175336
  date: 2012.01.29
*****************************************************/
int bsp_get_factory_mode(void)
{
#define FACTORY_MODE_MAGIC_STR  "+=+=+==factory_mode+=+=+==\n"
    int ret = -1;
    
    FACTORY_MODE_TYPE  real_factory_mode;;

    /* 读错误，认为是正常模式 */
    ret = DR_NV_Read(NV_FACTORY_MODE_I, &real_factory_mode, sizeof(FACTORY_MODE_TYPE));
    if(0 != ret)        
    {
        printk("DR_NV_Read error, default to normal mode\n");
        return 1;
    }

    if(1 == real_factory_mode.factory_mode)
    {
        printk("normal mode\n");
        return 1;
    }
    else
    {
        printk(FACTORY_MODE_MAGIC_STR);
        return 0;
    }

}
/******************************************************************************
Function:       wifi_get_is_LNA
  Description:  Get LNA status via hardware sub version.
  Input:        NULL
  Output:       NULL
  Return:       0--NO LNA,1--LNA.
  Others:       该函数移植至GL04P II期
******************************************************************************/
int wifi_get_is_LNA(void)
{
    BSP_U32 u32HwId;
    BSP_U16 u16IdMain;
    BSP_U16 u16IdSub;
    u32HwId = *(BSP_U32*)MEMORY_AXI_HW_ID_ADDR;
    u16IdMain = (u32HwId >> SUB_ID_BITS) & MASK_NUM; /*获取主硬件版本号*/
    u16IdSub  = u32HwId & MASK_NUM;                  /*获取子硬件版本号*/
    printk("wifi_get_is_LNA :%d \n",u16IdMain);

    if(HW_VER_PRODUCT_E5371_DCM == u16IdMain)
    {
        if(HW_VER_PRODUCT_E5371_DCM_SUBID == u16IdSub)
        {
            printk("LNA \n");
            return 1;  
        }
    }
    printk("NO LNA \n");
    return 0;
}
EXPORT_SYMBOL(bsp_get_factory_mode);


EXPORT_SYMBOL(wifi_event_report);
EXPORT_SYMBOL(g_wifiRxFlowCtrlFlag);
EXPORT_SYMBOL(WiFi_DrvSetRxFlowCtrl);
EXPORT_SYMBOL(WiFi_DrvClearRxFlowCtrl);
EXPORT_SYMBOL(BSP_WifiDataReservedTail);
EXPORT_SYMBOL(wifi_tcmd);
EXPORT_SYMBOL(wifi_get_tcmd_mode);
EXPORT_SYMBOL(wifi_power_on);
EXPORT_SYMBOL(wifi_power_off);
EXPORT_SYMBOL(wifi_get_status);
EXPORT_SYMBOL(wifi_get_rx_detail_report);
EXPORT_SYMBOL(wifi_get_rx_packet_report);

#ifdef  CONFIG_WLAN_IF_BCM43239
EXPORT_SYMBOL(wifi_get_nvmac_addr);
#endif

#if (defined (CHIP_BB_6920ES) || defined(CHIP_BB_6920CS))
EXPORT_SYMBOL(wifi_sleep_vote_mode_k);
EXPORT_SYMBOL(wifi_set_pa_mode);
EXPORT_SYMBOL(wifi_get_pa_cur_mode);

#endif

EXPORT_SYMBOL(set_get_sta_num_func);
EXPORT_SYMBOL(wifi_get_sta_num);

module_init(wifi_at_init);
module_exit(wifi_at_exit);

MODULE_AUTHOR("Hisilicon Drive Group");
MODULE_DESCRIPTION(WIFI_AT_DEVICE);
MODULE_LICENSE("GPL");









