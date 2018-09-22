#ifndef __BALONG_CORE_V3R2CS_H__
#define __BALONG_CORE_V3R2CS_H__

#include <mach/balong_v100r001.h>

extern void balong_timer_init(unsigned int timer_irq);

#define CLK_REGOFF_LOAD	     0x000	/* Load (R/W) */
#define CLK_REGOFF_VALUE     0x004	/* Value (R/O) */
#define CLK_REGOFF_CTRL      0x008	/* Control (R/W) */
#define CLK_REGOFF_CLEAR     0x00C	/* Clear (W/O) */
#define CLK_REGOFF_INTSTATUS 0x010	/* INT STATUS (R/O) */


#define CLK_DEFULT_TICKS_PERSECOND 100

/* �û�ʱ��֧�ֵ�������ֵ*/
/*(0xffffffff * 1000)/48M*/
#define CLK_DEF_MAX_MILLSECOND  89478
#define CLK_DEF_MIN_MILLSECOND  1

#define CLK_DEF_MAX_COUNTER  0xFFFFFFFF  //���ֵΪ2TIMER_WIDTH_N - 1,TIMER_WIDTH_NΪ32λ

/* Ŀ���ʱ��Ƶ��Ϊ48M */
#define USR_TIMER_CLK	                  PBXA9_TIMERS_CLK

#define AMBA_RELOAD_TICKS	0
#define USR_CLK_RATE_MIN   1
#define USR_CLK_RATE_MAX   8000


#define SYS_TIMER_NUM   0
#define AUX_TIMER_NUM   1
#define STAMP_TIMER_NUM 2
#define USR_CLK_NUM     3

//#define CLK_DEF_USRCLK_REGBASE   AMBA_TIMER4_BASE 

#ifdef BSP_CORE_MODEM
/* MODEM CORE����timer 0123*/
#define TIMER_BASE(ClkId) (CLK_REGBASE_0123 + ClkId*0x14)
#else
/* APP CORE����timer 4567*/
#define TIMER_BASE(ClkId) (CLK_REGBASE_4567 + (ClkId)*0x14)
#endif

#define CLK_REGOFF_LOAD	     0x000	/* Load (R/W) */
#define CLK_REGOFF_VALUE     0x004	/* Value (R/O) */
#define CLK_REGOFF_CTRL      0x008	/* Control (R/W) */
#define CLK_REGOFF_CLEAR     0x00C	/* Clear (W/O) */
#define CLK_REGOFF_INTSTATUS 0x010	/* INT STATUS (R/O) */


/* ��ʱ��1 ���������ƼĴ���,����ɶ�ʱ�������������󣬸üĴ����Զ�����*/
#define CLK_DEF_TC_UNLOCK	0       /* 0��������*/
#define CLK_DEF_TC_LOCK		(1<<3)  /* 1������ʱ���ĵ�ǰֵ������TIMER1_CURRENTVALUE��*/

/* TimerXRIS�ж�����λ*/
#define CLK_DEF_TC_INTENABLE	0       /* 0�������θ��ж�*/
#define CLK_DEF_TC_INTDISABLE	(1<<2)  /* 1�����θ��ж� */

/* ��ʱ���ļ���ģʽλ*/
#define CLK_DEF_TC_PERIODIC	(1<<1)          /* 1��user-defined count mode*/
#define CLK_DEF_TC_FREERUN	0               /* 0��free-running mode*/

/* ��ʱ��ʹ��λ*/
#define CLK_DEF_TC_ENABLE	1     /* 1��Timerʹ��*/
#define	CLK_DEF_TC_DISABLE	0     /* 0��Timer��ֹ*/
	  

/* �û�ʱ��ʹ��*/
#define CLK_DEF_ENABLE   (CLK_DEF_TC_UNLOCK | CLK_DEF_TC_PERIODIC | \
	  CLK_DEF_TC_INTENABLE|	CLK_DEF_TC_ENABLE)
/* �û�ʱ��ȥʹ��*/
#define CLK_DEF_DISABLE  (CLK_DEF_TC_UNLOCK | CLK_DEF_TC_PERIODIC | \
	  CLK_DEF_TC_INTDISABLE | CLK_DEF_TC_DISABLE)

/*ʱ���ж�ʹ��*/
#ifndef TIMER_INT_ENABLE
#define TIMER_INT_ENABLE(level) intEnable (level)
#endif
/*ʱ���ж�ȥʹ��*/
#ifndef TIMER_INT_DISABLE
#define TIMER_INT_DISABLE(level) intDisable (level)
#endif
/*****************************************************
 * ʱ���� 
 *****************************************************/
 /* signals generated from various clock generators */
#define PBXA9_TIMERS_CLK        (19200000)           /* Real time clock */
#define PBXA9_UART_CLK          (19200000)           /* UART clock */
/* frequency of counter/timers */
#define SYS_TIMER_CLK           (PBXA9_TIMERS_CLK)
//#define SYS_CLK_RATE_MIN        (10)
//#define SYS_CLK_RATE_MAX        (8000)
#define AUX_TIMER_CLK           (PBXA9_TIMERS_CLK)
#define STAMP_TIMER_CLK         (PBXA9_TIMERS_CLK)

#define STAMP_CLK_RATE_MIN      10
#define STAMP_CLK_RATE_MAX      8000


struct mtd_partition;
struct mtd_info;

/*
 * map_name:	the map probe function name
 * name:	flash device name (eg, as used with mtdparts=)
 * width:	width of mapped device
 * init:	method called at driver/device initialisation
 * exit:	method called at driver/device removal
 * set_vpp:	method called to enable or disable VPP
 * mmcontrol:	method called to enable or disable Sync. Burst Read in OneNAND
 * parts:	optional array of mtd_partitions for static partitioning
 * nr_parts:	number of mtd_partitions for static partitoning
 */
struct flash_platform_data {
	const char	*map_name;
	const char	*name;
	unsigned int	width;
	int		(*init)(void);
	void		(*exit)(void);
	void		(*set_vpp)(int on);
	void		(*mmcontrol)(struct mtd_info *mtd, int sync_read);
	struct mtd_partition *parts;
	unsigned int	nr_parts;
};

#endif //__BALONG_CORE_H__

