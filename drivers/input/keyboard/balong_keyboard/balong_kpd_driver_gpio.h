#ifndef KPD_DRIVER_GPIO_H
#define KPD_DRIVER_GPIO_H 

#include <linux/timer.h>
#include <linux/mutex.h>
#include <linux/workqueue.h>
#include <linux/spinlock.h>
#include "BSP.h"

#include "../../../SoftTimer/SoftTimer.h"
 
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
    || (defined (BOARD_ASIC) && (defined(CHIP_BB_6920ES)||(defined(CHIP_BB_6920CS)))) /*y00186965*/ /*sunjian:与芯片确认CS*/
   /*++by pandong cs*/
	#if defined(CHIP_BB_6756CS)
	#define INT_GPIO_10  117
	#define GPIOS_PER_GROUP 8
	#define  KPD_GPIO_MAX (3)
	#elif defined(CHIP_BB_6920CS)
	#define INT_GPIO_10  114
	#define GPIOS_PER_GROUP 32
	#define  KPD_GPIO_MAX (3)	
	#else
	#define INT_GPIO_10  112
	//#define INT_PMU         127
	#define GPIOS_PER_GROUP 32
	#define  KPD_GPIO_MAX (3)
	//#define  PMU_BASE_ADDR   0x00
	//#define  PMU_STATUS_OFFSET  0x01
	//#define  PMU_MASK_OFFSET       0x09
	//#define PMU_REG_READ_V(reg,result)  ((result) = *(volatile unsigned char  *)(reg))
	//#define PMU_REG_WRITE_V(reg,data)  (*((volatile unsigned char *)(reg)) = (data))
	#endif
	/*--by pandong cs*/
#else
#define INT_GPIO_10  153
#define GPIOS_PER_GROUP 8
#define  KPD_GPIO_MAX (4)  
#endif

//void *    pmu_virtual_addr ;
/*Log Message*/
#undef HIKPD_TRACE_LEVEL
#define HIKPD_TRACE_LEVEL  (1)
extern int hikpdlevel;
#undef HIKPD_TRACE_FUNCTION
/*comment this macro*/
#define HIKPD_TRACE_FUNCTION
/*
   0 - all message
   1 - dump all register read/write
   2 - flow trace
   3 - timeouut err and protocol err
   */
#undef hikpd_trace
#undef hikpd_error
#define BALONG_KPD_DEBUG_TAG	"hikpd"
#define hikpd_trace(level, format, args...) do { \
	int hiret = 0 ;\
	if((level) >= hikpdlevel) { \
		hiret = printk(KERN_DEBUG""format" :"BALONG_KPD_DEBUG_TAG" :%s:%d: \n", ##args, __FUNCTION__,  __LINE__); \
		if(hiret)\
		{\
		}\
	} \
}while(0)


#define hikpd_error(format, args...) do{ \
	int hiret =0;\
	hiret = printk(KERN_ERR""format" :"BALONG_KPD_DEBUG_TAG" error:%s:%d: \n", ##args, __FUNCTION__, __LINE__); \
		if(hiret)\
		{\
		}\
}while(0)

#ifdef HIKPD_TRACE_FUNCTION
#define hikpd_enter() do{ \
	int hiret = 0;\
	hiret = printk(KERN_DEBUG""BALONG_KPD_DEBUG_TAG": %s, line:%d, enter +++ \n",  __FUNCTION__, __LINE__); \
	if(hiret)\
	{\
	}\
}while(0)

#define hikpd_leave() do{ \
	int hiret =0;\
	hiret = printk(KERN_DEBUG""BALONG_KPD_DEBUG_TAG": %s, line:%d, leave --- \n",  __FUNCTION__, __LINE__); \
	if(hiret)\
	{\
	}\
}while(0)
#else
#define hikpd_enter() do{ }while(0)
#define hikpd_leave() do{ }while(0)
#endif

#define KPD_ASSERT( exp)  do{\
			int hiret =0;\
	        if(exp == 0){\
	        hiret = printk("########## could not enter this function#######\n");\
	    	if(hiret)\
			{\
			}\
	        for(;;){};\
        }\
}while(0)

#define KPD_CALLBACK_MAX_NUM    3

/*****************************************************************************
   宏定义 
*****************************************************************************/
/*
    GPIO keyboard statemation (normal) define: 
    Idle --(press)--> deshaking --(timeout)--> pressed --(release)--> Idle.
     ^                   |
     |-----------<-------+

    If any error or invalid event, goto idle
*/
typedef enum KPD_GPIO_STATE_tag
{
    KPD_GPIO_STATE_IDLE       = 0,              /* The key is released and wait for press                       */
    KPD_GPIO_STATE_DESHAKING,                   /* deshaking check                                              */
    KPD_GPIO_STATE_PRESSED,                     /* After deshaking, the key being pressed, customer handle it.  */
    KPD_GPIO_STATE_BUTT
}KPD_GPIO_STATE_ENUM;
typedef enum
{
	KPD_DRIVER_EVENT_INVALID= 0,
	KPD_DRIVER_EVENT_PRESS,
	KPD_DRIVER_EVENT_RELEASE,
	KPD_DRIVER_EVENT_TIMEOUT,
	KPD_DRIVER_EVENT_BUTT
}KPD_DRIVER_EVENT_ENUM;

typedef enum
{
	KPD_DRIVER_MSG_INVALID =0,
	KPD_DRIVER_MSG_POWER,
	KPD_DRIVER_MSG_WPS,
	KPD_DRIVER_MSG_RESET,
	KPD_DRIVER_MSG_BUTT

}KPD_DRIVER_MSG_ENUM;


/*组合按键*/
typedef enum
{
    KPD_MUX_NULL = 0,
    KPD_MUX_WPS_PWR,
    KPD_MUX_WPS_RST ,
    KPD_MUX_PWR_RST ,
    KPD_MUX_PWR_WPS_RST,
    KPD_MUX_BUTT
}KPD_MUX_ENUM;

typedef void (*KPD_PARSE_HANDLER)(KPD_DRIVER_MSG_ENUM enmsg,KPD_DRIVER_EVENT_ENUM enevent );
/*
    Keyboard information define
*/
typedef struct
{
	KPD_DRIVER_MSG_ENUM enkey;
	KPD_PARSE_HANDLER pparsecallback[KPD_CALLBACK_MAX_NUM];
	int gpio;
	int     type;
	char * desc ;
	int active_low;
	int index;
	int wakeup;
	unsigned char uckeypresslevelishigh;
	
}KPD_GPIO_INFO_STRU;

typedef struct
{    
    KPD_GPIO_INFO_STRU * pstgpioinfo ; 

    HTIMER              ptimer_softtimer;
    KPD_GPIO_STATE_ENUM         enstate;
   // struct input_dev   *input ;
    //struct mutex disable_lock;
    struct spinlock disable_lock;
    struct work_struct work;
    KPD_DRIVER_EVENT_ENUM enevent;
}KPD_GPIO_STM_STRU;

/*****************************************************************************
   全局变量声明
*****************************************************************************/
/*
    All gpio key list.
*/
KPD_DRIVER_EVENT_ENUM kpd_driver_status_get( KPD_DRIVER_MSG_ENUM enMsg );
BSP_U32 ifc_pmu_init(BSP_VOID);
void kpd_driver_gpio_deshaking_timer_isr(unsigned long index );
int kpd_driver_gpio_one_node_init(int index);

typedef void (*KPD_TIMER_CALLBACK)(unsigned long);

unsigned int kpd_timer_create( struct timer_list* phtimer, unsigned int  ulmsecond, KPD_TIMER_CALLBACK pcallback, int iparam);
void kpd_driver_gpio_status_set(KPD_DRIVER_MSG_ENUM key,bool isMask);

extern KPD_GPIO_STM_STRU s_astkpdlist[ KPD_GPIO_MAX];
extern  KPD_GPIO_INFO_STRU  pstgpioinfoglb[ KPD_GPIO_MAX +1];
#endif
