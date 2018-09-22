/*
 *  code for wifi at interface utility
 *
 */

#ifndef _wlan_if_h_
#define _wlan_if_h_

#define SKB_DHD_BLOCKSIZE           512
#define SKB_DHD_SDALIGN             32
#define SKB_DHD_BLOCKSIZE_ROUNDUP   9
#define SKB_DHD_SDALIGN_ROUNDUP     5
#define SKB_DHD_TAIL_RESERVED       64

#define  WIFI_AT_DEVICE 	 	"wifi_at_dev"


#define  WL_START_CMD_STR                 "0"  /*wl_cmd_start*/
#define  WL_RX_CMD_STR                    "1"  /*wl_rx_cmd*/
#define  WL_TX_CMD_STR  		          "2"  /*wl_tx_cmd*/
#define  WL_RX_REPORT_STR 			      "3 total:%u bad:%u" /*wl_rx_report*/
#define  WL_RX_PACKET_STR 			      "4 ucast:%u mcast:%u" /*wl_pkt_report*/

#define  WIFI_AP_SHUTDOWN_TIMEOUT_SET_STR       "9 timeout:%u"
#define  WIFI_SLEEP_VOTE_43239_STR       		"12 mode:%u"
/* added by f00104094, the timer set cmd string */
#define WIFI_TIMER_SET_CMD_STR    "13 timeout:%u"

/*this cmd is used to debug*/
#define  WL_DEBUG_CMD_STR                 'd'
#define  WL_START_CMD                      0
#define  WL_RX_CMD                         1
#define  WL_TX_CMD                         2
#define  WL_RX_REPORT_CMD                  3
#define  WL_RX_PACKET_CMD                  4

#define  WIFI_AP_SHUTDOWN_TIMEOUT_SET_CMD       9
#define  WIFI_AP_SHUTDOWN_43239       10
#define  WIFI_AP_UP_43239       11
#define  WIFI_SLEEP_VOTE_43239       12
/* added by f00104094, to add the pure timer operation cmd 13 */
#define  WIFI_TIMER_SET_CMD 13

#define NV_FACTORY_MODE_I 36
/* 工厂模式nv项,0 : 工厂模式 1: 非工厂模式**/
typedef struct
{
    UINT32 factory_mode;
}FACTORY_MODE_TYPE;


enum wifi_pwr_save_mode_e
{
	WIFI_NORMAL_MODE = 1,
	WIFI_LIGTH_SLEEP = 2,
	WIFI_DEEP_SLEEP = 3
};

#define WIFI_CMD_MOD_NORMAL 0
#define WIFI_CMD_MOD_AT 1
#define WIFI_CMD_MOD_MAX 2


/*WIFI mode*/
#define WIFI_IS_OFF                     0
#define WIFI_IS_NORMAL_MODE             1
#define WIFI_IS_TCMD_MODE               2

#define BRCM_WL_TOOL  "/system/bin/wifi_brcm/exe/wl"


/*below macro and define used to */

typedef struct _wifi_msg_ctrl
{
	struct sock   *dev_sock;  //netlink socket;
	unsigned int  user_pid;   //user task pid
	spinlock_t queue_lock;
	struct delayed_work event_work;		
	struct list_head event_queue;
	unsigned int queue_len;
	
} WIFI_MSG_CTRL_STRU;

#define WIFI_MAX_EVENT_QUEUE_LEN  	(20)

typedef struct _wifi_event_node {
    struct list_head list;
    int len;
    char data[0];
}WIFI_EVENT_NODE_STRU;


#define WIFI_DELAY_FOR_NL_SEND 	msecs_to_jiffies(1)

#define WIFI_EVENT_PATH_IS_READY(pCtrl)  (pCtrl->user_pid != (unsigned int)0xFFFFFFFF)

#define WIFI_MSG_CTRL_INIT(pCtrl)  \
{\
	INIT_LIST_HEAD(&(pCtrl->event_queue));	\
	pCtrl->queue_len = 0; \
	INIT_DELAYED_WORK(&(pCtrl->event_work), wifi_event_send); \
	spin_lock_init(&(pCtrl->queue_lock)); \
	pCtrl->dev_sock = netlink_kernel_create(&init_net,NETLINK_USER_WIFI_EVENT, \
	0,(void*)wifi_event_input,NULL,THIS_MODULE); \
	pCtrl->user_pid = (unsigned int)(0xFFFFFFFF); \
}


#define WIFI_MSG_CTRL_UNINIT(pCtrl)  \
{\
	cancel_delayed_work_sync(&(pCtrl->event_work)); \
	if (pCtrl->dev_sock) \
	netlink_kernel_release(pCtrl->dev_sock); \
	pCtrl->user_pid = (unsigned int)(0xFFFFFFFF); \
}


#define WLAN_TRACE_INFO(args)		do {printk args;} while (0)
#define WLAN_TRACE_ERROR(args)		do {printk args;} while (0)


typedef struct _wifi_user_event
{
    unsigned int eventId;
    unsigned int eventVal;
} WIFI_USER_EVENT;



#define USER_WIFI_TIMEOUT_EVENT       1
#define USER_WIFI_GPIO_EVENT 2
#define USER_WIFI_POWER_ON_EVENT 5

#define USER_WIFI_ENABLE_EVENT 3
#define USER_WIFI_DISABLE_EVENT 4

#define USER_WIFI_NULL_EVENT  99



int wifi_tcmd(char *tcmd_str);
int wifi_get_tcmd_mode(void);
int wifi_get_status(void);
void wifi_get_rx_detail_report(unsigned int *totalPkts, unsigned int *googPkts, unsigned int * badPkts);
void wifi_get_rx_packet_report(unsigned int *ucastPkt, unsigned int *mcastPkts);
int wifi_power_on(void);
int wifi_power_on_full(unsigned int mode);

int wifi_power_off(void);
unsigned int BSP_WifiDataReservedTail(unsigned int len);
unsigned long WiFi_DrvSetRxFlowCtrl    (unsigned long para1, unsigned long para2);
unsigned long WiFi_DrvClearRxFlowCtrl  (unsigned long para1, unsigned long para2);
int wifi_event_report(void * data,int len);

int wifi_event_send_tasklet_mode(void *data, int len, int resume_mode) ;

int wifi_netlink_chunnel_init(void);
void wifi_netlink_chunnel_uninit(void);
int wifi_power_save_enable(void);
int wifi_power_save_disable(void);
int wifi_set_pa_mode(int wifiPaMode);
int wifi_get_pa_cur_mode(void);


typedef int (*GET_STA_NUM_FUNC)(void);
void set_get_sta_num_func(unsigned int func);
int wifi_get_sta_num(void);

#endif
