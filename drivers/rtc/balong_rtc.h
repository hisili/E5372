/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  balong_rtc.h
*
*
*   描    述 :  
*
*************************************************************************/

#ifndef _BALONG_NVIM_H_
#define _BALONG_NVIM_H_

#ifdef __cplusplus
extern "C" 
{
#endif


/*
 * Register definitions
 */
#define RTC_REG_BASE    0x90005000   /* SOC RTC base register */
#define RTC_REG_OFFSET  0x18  /*offset max*/

#define	RTC_DR		0x00	/* Data read register */
#define	RTC_MR		0x04	/* Match register */
#define	RTC_LR		0x08	/* Data load register */
#define	RTC_CR		0x0c	/* Interrupt mask set and Control register */
#define	RTC_MIS	    0x10	/* Masked interrupt status register */
#define	RTC_RIS		0x14	/* Raw interrupt status register */
#define	RTC_ICR		0x18	/* Interrupt clear register */

#define RTC_BIT_AI	(1 << 0) /* Alarm interrupt bit */
#define RTC_BIT_IM	(1 << 1) /* Interrupt mask and set bit */

#define PMURTCDR_FLAG   1
#define RTC_RET         1

#define RTC_INT_ID      36  /*SOC RTC interrupt ID*/

#define RTC_BASE_YEAR   1900 /*RTC时间显示用的基准年份*/
/**************************************************************************
   ENUM定义                           
**************************************************************************/
typedef enum
{
    RTCDR_READ = 0, /*RTC Data Register read*/
    RTCLR_WRITE,    /*RTC load register write*/
    RTCLR_READ,     /*RTC load register read*/
    RTCMRA_WRITE,   /*RTC match registerA write*/
    RTCMRA_READ,    /*RTC match registerA read*/
    RTCMRB_WRITE,   /*RTC match registerB write*/
    RTCMRB_READ,    /*RTC match registerB read*/
    RTCMRC_WRITE,   /*RTC match registerC write*/
    RTCMRC_READ,    /*RTC match registerC read*/
    RTCMODE_MAX
}RTC_MODE;
/**************************************************************************
  STRUCT定义
**************************************************************************/
typedef struct
{
    BSP_U32 u32ModuleId;
    BSP_U32 u32FuncId;
    BSP_U32 u32RtcMode;
    BSP_U32 u32RtcData;
}RTC_IFC_MSG_SET;

typedef struct
{
	BSP_U32 ulRtcRet;
	BSP_U32 ulRtcData;
}RTC_IFC_MSG_GET;

/**************************************************************************
  函数声明
**************************************************************************/
int balong_rtc_IFCInit(void);
int balong_rtc_IFCReceive(BSP_VOID *pMsgBody,BSP_U32 u32Len);
int balong_rtc_IFCSend(unsigned int usMode,unsigned int usRtcData);
int balong_rtc_IFCReadWrite(RTC_MODE usMode,unsigned int *pItem);

typedef BSP_VOID (*ALARM_FUNC)(void);

int balong_rtc_alarm_set(unsigned int days,unsigned int hours,unsigned int mins,unsigned int secs,ALARM_FUNC alarm_callback);
int balong_rtc_alarm_clear(void);
unsigned long get_current_working_time(void);

#ifdef __cplusplus
}
#endif

#endif  /*end #define _BALONG_NVIM_H_*/


