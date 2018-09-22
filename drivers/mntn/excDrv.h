

/******************************************************************
* Copyright (C), 2005-2007, HISILICON Tech. Co., Ltd.             *
*                                                                 *
* File name: excDrv.h                                            *
*                                                                 *
* Description:                                                    *
*      exc driver public header file                             *
*                                                                 *
*                                                                 *
* Version: v0.1                                                   *
*                                                                 *
* Function List:                                                  *
*                                                                 *
* Date:                                                           *
*                                                                 *
* History:                                                        *
*                                                                 *
*   1. Date        :    2009-03-14                                 *
*      Modification:    add setupdateflag to enter bootloader when reboot three times           *
*      Question number: AT2D09366               
*   2. Date:2009-08-31                     *      
*      Modification:modify Exc file saving             *
*      Question number:AT2D14131            *
*   3. Date         :   2010-02-20                               *
*      Modification :  add 2 bit error process   *
*      Question number: AT2D16193                          *
*   4. Date         :   2010-06-29                                *
*      Modification :   内存定义方式                              *
*   5. Date         :   2010-09-09                                *
*      Modification :   systemError记录当前系统剩余内存 *

*******************************************************************/

#ifndef _EXC_DRV_H
#define _EXC_DRV_H

/* V7R1暂不添加ICC可维可测功能，后续在FeatureConfig.h中添加，并且在C核ICC模块中同步添加 */
#if (defined(CHIP_BB_6920ES) || defined (CHIP_BB_6920CS))
#define FEATURE_ON                                  1
#define FEATURE_OFF                                 0
#define FEATURE_ICC_DEBUG                           FEATURE_OFF
#endif

#include <mach/common/bsp_memory.h>

#if defined (BOARD_SFT) && defined (VERSION_V3R2)
#include <mach/balong_v3r2sft_v100r001.h>
#elif defined (BOARD_FPGA) && defined (VERSION_V3R2)
#include <mach/balong_p500bbit_v100r001.h>
#elif (defined (BOARD_FPGA_P500) && defined(CHIP_BB_6920ES))
#include <mach/balong_p500fpga.h>
#elif (defined (BOARD_FPGA_P500) && defined (CHIP_BB_6920CS))
#include <mach/balong_hi6920csp500.h>
#elif defined (BOARD_SFT) && defined (VERSION_V7R1)
#include <mach/balong_v7r1sft.h>
#elif defined (BOARD_ASIC_BIGPACK) && defined (VERSION_V3R2)
    /*++by pandong cs*/
	#if defined(CHIP_BB_6756CS)
		#include <mach/balong_v3r2cs.h>
	#else
		#include <mach/balong_v3r2asic.h>
	#endif
	/*--by pandong cs*/
#elif (defined(BOARD_ASIC) && (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
#include <mach/balong_v7r1asic.h>
#elif defined(CHIP_BB_6920CS) && defined(BOARD_SFT)
#include <mach/balong_hi6920cs_sft.h>
#else
#error "there is no file included!"
#endif

#define EXC_TEST

#define EXC_READ_REG(reg,resu32t)  ((resu32t) = *(volatile unsigned int *)(reg))
#define EXC_WRITE_REG(addr,value) (*((volatile unsigned int  *)(addr)) = (value))

#define LINUX_VERSION   "Linux_V7R1"

#define OM_TIMER_CURRENT       (PBXA9_TIMER1_VA_BASE+0x2c)  /*   0x04+0x14*n */

#define EXCH_BASE_ADDR  PBXA9_DRAM_EXC_SHARE_VIRT
#define EXCH_ALL_SIZE   PBXA9_DRAM_EXC_SHARE_SIZE


/*SC reg*/

#define SYSTEM_CONTROL_BASE  (V3R2_SC_VA_BASE)
#define SYSCTRL_SCSYSCTRL    (SYSTEM_CONTROL_BASE+0x00)
#define SYSCTRL_SCSYSSTAT0    (SYSTEM_CONTROL_BASE+0x10)
    /*++by pandong cs*/
#if defined (BOARD_FPGA) && defined (VERSION_V3R2)
#define SYSCTRL_SC_CTRL2    (SYSTEM_CONTROL_BASE+0x40)
#else
#define SYSCTRL_SC_CTRL2    (SYSTEM_CONTROL_BASE+0x408)
#endif
	/*--by pandong cs*/
#if defined (BOARD_FPGA) && defined (VERSION_V3R2)
#define SC_SW_RST_VAL   (0x66667384)
#elif defined (BOARD_SFT) && defined (VERSION_V3R2)
#define SC_SW_RST_VAL   (0x48698284)
   /*++by pandong cs*/
#elif  (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2) && defined(CHIP_BB_6756CS))
#define SC_SW_RST_VAL   (0x48698284)
	/*--by pandong cs*/
#endif

#if ((defined (BOARD_FPGA_P500)) \
  && (defined(CHIP_BB_6920ES)||defined (CHIP_BB_6920CS)))
#define WDT_REG_BASE       (PBXA9_WDT_VA_BASE )
#define WDT_REG_CR		(WDT_REG_BASE + 0x0)
#define WDT_REG_TORR		(WDT_REG_BASE + 0x4)
#define WDT_REG_CRR	   (WDT_REG_BASE + 0xc )
#define INTEGRATOR_SC_WDTCTRL_OFFSET		(0x40)
#elif((defined (BOARD_ASIC)||defined (BOARD_SFT)) \
  && (defined(CHIP_BB_6920ES)||defined (CHIP_BB_6920CS))) || \
  defined(VERSION_V3R2)
/*wdt reg*/
#define WDT_REG_BASE       (PBXA9_WDT_VA_BASE )
#define WDT_REG_LOAD       (PBXA9_WDT_VA_BASE + 0X00)       /* 计数初值*/
#define WDT_REG_VAL   	   (WDT_REG_BASE + 0X04)        	/* 当前值*/
#define WDT_REG_CTR        (WDT_REG_BASE + 0X08)       	    /* 控制寄存器 */
#define WDT_REG_INTCLR     (WDT_REG_BASE + 0X0C)       	    /* 中断清除*/

#define WDT_REG_LOCK       (WDT_REG_BASE + 0X0C00)       	/* 加解锁*/
#define WDT_UNLOCK_VALUE   (0x1ACCE551)
#else
   #error "unknown platform type"
#endif

#define DRV_LINUX_TASK_NUM_MAX  128

/*GIC STATUS RREG*/
#define GIC_ICSPISR         IO_ADDRESS(PBXA9_PERIPHBASE)+0x1D04    
/**********************************************************************************************
*
*                    以下保存异常时寄存器地址-Begin
*
***********************************************************************************************/
/**********************************************************************************************
*
*                    以下保存异常时寄存器地址-Begin
*
***********************************************************************************************/



#define EXCH_C_CORE_SIZE                (0x32000)
#define EXCH_A_CORE_SIZE                (0x2D000)

#define EXCH_AREA_ADDR                  (EXCH_BASE_ADDR + EXCH_C_CORE_SIZE)   //0x32000
#define EXCH_BB_REG_SIZE                (0x1000)


#define EXCH_A_CORE_BASE                (EXCH_AREA_ADDR - EXCH_BB_REG_SIZE)//0x31000
#define EXCH_BB_REG_ADDR                (EXCH_A_CORE_BASE)  //0x31000

/*A核C核共享区*/
#define EXCH_A_C_SHARE_ADDR_DDR             (EXCH_A_CORE_BASE )//0x31000
#define EXCH_A_C_SHARE_ADDR         (MEMORY_AXI_EXC_ADDR +  MEMORY_AXI_EXC_SIZE - 0x100)
#define EXCH_A_C_SHARE_SIZE             (0x100)

#define EXCH_C_CORE_FLAG_ADDR           (EXCH_A_C_SHARE_ADDR+0x00)      /*C核是否记录异常文件标志地址*/
#define EXCH_A_CORE_FLAG_ADDR           (EXCH_A_C_SHARE_ADDR+0x04)      /*A核是否记录异常文件标志地址*/
#define EXCH_C_CORE_INIT_END_FLAG_ADDR  (EXCH_A_C_SHARE_ADDR+0x08)      /*C核初始化完成标志地址*/

#if(FEATURE_ICC_DEBUG == FEATURE_ON)
#define EXCH_C_ICC_FLAG_ADDR             (EXCH_A_C_SHARE_ADDR+0x0C)    /*C核ICC是否记录异常文件标志地址*/
#define EXCH_A_ICC_FLAG_ADDR             (EXCH_A_C_SHARE_ADDR+0x10) /*C核ICC是否记录异常文件标志地址*/
#endif
#define EXCH_BSP_VERSION_ADDR            (EXCH_A_C_SHARE_ADDR+0x14)    /*BSP版本号，供A核使用*/
#define EXCH_BSP_VERSION_SIZE            (0x20)
#define EXCH_C_GIC_STATUS_ADDR           (EXCH_BSP_VERSION_ADDR + EXCH_BSP_VERSION_SIZE)//0x31034 /*记录C核GIC状态*/
#define EXCH_C_GIC_STATUS_SIZE           (0x20)
#define EXCH_A_GIC_STATUS_ADDR           (EXCH_C_GIC_STATUS_ADDR + EXCH_C_GIC_STATUS_SIZE)//0x31054 /*记录A核GIC状态*/
#define EXCH_A_GIC_STATUS_SIZE           (0x20)

#define EXCH_A_C_SOCP_LOG_ADDR            (EXCH_A_GIC_STATUS_ADDR + EXCH_A_GIC_STATUS_SIZE) /*LOG2.0复位信息保存地址*/
#define EXCH_A_C_SOCP_LOG_SIZE            (0x34) 
#define EXCH_A_C_GUDSP_LOG_ADDR           (EXCH_A_C_SOCP_LOG_ADDR + EXCH_A_C_SOCP_LOG_SIZE) /*GUDSP复位信息保存地址*/
#define EXCH_A_C_GUDSP_LOG_SIZE           (0x4) 


#define EXCH_C_CORE_FLAG                (0x43584543)      /*C核是否记录异常文件标志*/
#define EXCH_A_CORE_FLAG                (0x43584541)     /*A核是否记录异常文件标志*/
#define EXCH_C_CORE_INIT_END_FLAG       (0x12345678)     /*C核初始化完成标志*/


/**********************************************************************************************
*
*                    以上保存异常时寄存器地址-End
*
**********************************************************************************************/


#define EXCH_ROOT_ADDR                 (EXCH_BB_REG_ADDR+EXCH_BB_REG_SIZE)//0x0000(0x32000)
#define EXCH_ROOT_SIZE                 (0x0200)
/*
#define EXCH_OS_MSG_ADDR               (EXCH_ROOT_ADDR + EXCH_ROOT_SIZE)//0x0200
#define EXCH_OS_MSG_SIZE               (0x0200)
*/
#define EXCH_TASK_TCB_ADDR             (EXCH_ROOT_ADDR + EXCH_ROOT_SIZE)//0x0200
#define EXCH_TASK_TCB_SIZE             (0x0400)
#define EXCH_CUSTOM_ADDR               (EXCH_TASK_TCB_ADDR + EXCH_TASK_TCB_SIZE)//0x0600
#define EXCH_CUSTOM_SIZE               (0x0400)
#define EXCH_RESERVED_ADDR             (EXCH_CUSTOM_ADDR + EXCH_CUSTOM_SIZE)//0x0A00
#define EXCH_RESERVED_SIZE             (0x0600)
#define EXCH_INT_ADDR                  (EXCH_RESERVED_ADDR + EXCH_RESERVED_SIZE)//0x1000
#define EXCH_INT_SIZE                  (0x4000)
#define EXCH_TASK_SWITCH_ADDR          (EXCH_INT_ADDR + EXCH_INT_SIZE)//0x5000
#define EXCH_TASK_SWITCH_SIZE          (0x8000)
#define EXCH_INT_STACK_ADDR            (EXCH_TASK_SWITCH_ADDR + EXCH_TASK_SWITCH_SIZE)//0xD000
#define EXCH_INT_STACK_SIZE            (0x1000)
#define EXCH_TASK_STACK_ADDR           (EXCH_INT_STACK_ADDR + EXCH_INT_STACK_SIZE)//0xE000
#define EXCH_TASK_STACK_SIZE           (0x2000)
/*
#define EXCH_TASK_EXCSTACK_ADDR        (EXCH_TASK_STACK_ADDR + EXCH_TASK_STACK_SIZE)//0x11000
#define EXCH_TASK_EXCSTACK_SIZE        (0x1000)
*/
#define EXCH_CPU_VIEW_ADDR             (EXCH_TASK_STACK_ADDR + EXCH_TASK_STACK_SIZE)//0x10000
#define EXCH_CPU_VIEW_SIZE             (0x1000)

#if 0
#define EXCH_ALL_TASK_TCB_ADDR                     (EXCH_CPU_VIEW_ADDR + EXCH_CPU_VIEW_SIZE)//0x11000
#define EXCH_ALL_TASK_TCB_SIZE                      (0x10000)


/*以下地址用于WDT复位时记录异常任务栈*/
#define EXCH_WDT_RESET_TASK_STACK_ADDR     (EXCH_CPU_VIEW_ADDR + EXCH_CPU_VIEW_SIZE)//0x11000
#define EXCH_WDT_RESET_PER_STACK_SIZE       (0x2000)                  /*每个任务栈大小*/
#define EXCH_WDT_RESET_TASK_STACK_SIZE       (0x6000)

#endif
/*以下地址用于记录系统异常打印出异常信息*/
#define EXCH_LINUX_PRINTK_INFO_ADDR     (EXCH_CPU_VIEW_ADDR + EXCH_CPU_VIEW_SIZE)//0x11000
#define EXCH_LINUX_PRINTK_INFO_SIZE     (0xC000)    

#define EXCH_OSA_ADDR                   (EXCH_LINUX_PRINTK_INFO_ADDR + EXCH_LINUX_PRINTK_INFO_SIZE)//0x1D000
#define EXCH_OSA_SIZE                   (0x8000)                                                  

#define EXCH_A_TASK_SWITCH_PID_PPID_ADDR   (EXCH_OSA_ADDR+EXCH_OSA_SIZE)  //0X25000
#define EXCH_A_TASK_SWITCH_PID_PPID_SIZE   (0x8000)                       //0x2D000


#define EXCH_EXC_BIN_SIZE               (EXCH_C_CORE_SIZE+EXCH_A_CORE_SIZE)    


#define EXCH_C_OM_LOG_ADDR              (EXCH_BASE_ADDR + EXCH_EXC_BIN_SIZE)//0x5F000
#define EXCH_C_OM_LOG_SIZE              (0x2000)

#define EXCH_C_OM_MSG_ADDR              (EXCH_C_OM_LOG_ADDR + EXCH_C_OM_LOG_SIZE)//0x61000
#define EXCH_C_OM_MSG_SIZE              (0x8000)

#define EXCH_C_FLASH_ADDR               (EXCH_C_OM_MSG_ADDR + EXCH_C_OM_MSG_SIZE)//0X69000
#define EXCH_C_FLASH_SIZE               (0x1000)

#define EXCH_C_SCI_ADDR                 (EXCH_C_FLASH_ADDR + EXCH_C_FLASH_SIZE)//0X6A000
#define EXCH_C_SCI_SIZE                 (0x10000)

#define EXCH_A_OM_LOG_ADDR              (EXCH_C_SCI_ADDR + EXCH_C_SCI_SIZE) //0x7A000
#define EXCH_A_OM_LOG_SIZE              (0x2000)

#define EXCH_A_OM_MSG_ADDR              (EXCH_A_OM_LOG_ADDR + EXCH_A_OM_LOG_SIZE)//0x7C000 
#define EXCH_A_OM_MSG_SIZE              (0x8000)

#define EXCH_A_FLASH_ADDR               (EXCH_A_OM_MSG_ADDR + EXCH_A_OM_MSG_SIZE) //0x84000
#define EXCH_A_FLASH_SIZE               (0x1000)
 
#define EXCH_A_USB_ADDR                 (EXCH_A_FLASH_ADDR + EXCH_A_FLASH_SIZE)//0x85000
#define EXCH_A_USB_SIZE                 (0x8000)                              


#if(FEATURE_ICC_DEBUG == FEATURE_ON)
#define EXCH_A_ICC_SAVE_ADDR            (EXCH_A_USB_ADDR + EXCH_A_USB_SIZE)   //0x8D000
#define EXCH_A_ICC_SAVE_SIZE            (0x4000)            

#define EXCH_C_ICC_SAVE_ADDR            (EXCH_A_ICC_SAVE_ADDR+EXCH_A_ICC_SAVE_SIZE)//0x91000 
#define EXCH_C_ICC_SAVE_SIZE            (0x4000)  //0X95000

#endif
/*Added by l00212112, 20120607用于flashlsee远程文件系统可维可测,starts*/
#if(FEATURE_ICC_DEBUG == FEATURE_ON)
#define EXCH_RFS_ADDR (EXCH_C_ICC_SAVE_ADDR + EXCH_C_ICC_SAVE_SIZE)                     //0X89000                      
#define EXCH_RFS_SIZE (0x10000)                                                                        //0X99000
#else
#define EXCH_RFS_ADDR (EXCH_A_USB_ADDR + EXCH_A_USB_SIZE)                     //0X81000                      
#define EXCH_RFS_SIZE (0x10000)                                                                        //0X91000
#endif
/*ends*/

#if 0
 #define EXCH_SD_ADDR (EXCH_OM_MSG_ADDR + EXCH_OM_MSG_SIZE)                                       //0x5D000
 #define EXCH_SD_SIZE (0x10000)


 #define EXCH_SCI_ADDR (EXCH_SD_ADDR + EXCH_SD_SIZE)                                      //0x80000
 #define EXCH_SCI_SIZE (0x10000)
 #define EXCH_USB_ADDR (EXCH_SCI_ADDR + EXCH_SCI_SIZE)                                     //0x90000
 #define EXCH_USB_SIZE (0x10000)

 #define EXCH_FLASH_ADDR (EXCH_USB_ADDR + EXCH_USB_SIZE)                                  //0xa0000
 #define EXCH_FLASH_SIZE (0x100)
 #endif
    

#define SYS_FILE_NUM               8

#define SYS_BACK_FILE_MAGIC_SIZE 0x100

#define EXC_MAX_FILE_NUM    2
#define EXC_FILE_CAPACITY  10
#define EXC_MAX_FIEL_NAME_LENGTH 50
#define EXC_NVID_SYSTIME_ONOFF  22





#define EXCH_RESET_LOG_MAX 0x40000

#define SYSTEM_NORMAL_RESET 0xffffffff


#define USB_PC_DRIVER_RESET 0x10
#define USB_PC_DRIVER_RESET1 0x11
#define USB_PC_DRIVER_RESET2 0x12
#define USB_PC_DRIVER_RESET3 0x13
#define USB_PC_DRIVER_RESET4 0x14

#define UPDATE_IMAGE_RESET 0x20

#define NAND2BITERR_COUNT_MAX 0xff   /*整个flash最多出现2bit错误的次数*/


#define EXCH_STATE_TASK  0x10
#define EXCH_STATE_INT  0x80
#define EXCH_KERNEL_STATE  0x30
#define EXCH_INT_EXIT   0xAAAA


typedef struct
{
    unsigned int   maxNum;
    unsigned int   front;
    unsigned int   rear;
    unsigned int   num;
    unsigned int   data[1];
}Queue;

typedef enum
{
    EXCH_S_NORMAL,
    EXCH_S_ARM,
    EXCH_S_OS,
    EXCH_S_WDT,
    EXCH_S_SF,
    EXCH_S_SD,
    EXCH_S_SCI,
    EXCH_S_USB,
    EXCH_S_ONLINE,
    EXCH_S_SC_ERR = 0x99,
    EXCH_S_PMU = 0xAA,
    EXCH_S_PMU_ERR = 0xDD,
    EXCH_S_UNDEF = 0xFF
} EXCH_SOURCE;




typedef struct
{
    unsigned int   exchSwitch;  /*异常处理开关，1为开，0为关*/
    
    union 
    {
       unsigned int  uintValue;
       struct 
        {
            unsigned int    loop : 1;    /*0 死循环检测开关*/
            unsigned int    ARMexc : 1;  /*1 ARM异常检测开关*/
            unsigned int    OSexc : 1; /*2 OS异常检测开关*/
            unsigned int    stackFlow : 1; /*3 堆栈溢出检测开关*/
            unsigned int    taskSwitchSave : 1; /*4 任务切换记录开关*/
            unsigned int    intSave : 1; /*5 中断记录开关*/
            unsigned int    force : 1; /*6 强制记录开关，暂未使用*/
            unsigned int    sciRecord : 1; /*7 SIM TRACE记录开关*/
            unsigned int    reserved1 : 23; 
            unsigned int    sysErrReboot : 1; /*31 systemError复位开关*/
       }Bits;
    } exchCfg;    

    unsigned int    reserved2; 
    unsigned int   wdtTimerOut;  /*看门狗超时时间，单位ms*/
} EH_NV;


typedef struct
{
    unsigned char     product;   /*0x00  */
    unsigned char     rsv1;
    unsigned char     rsv2;
    unsigned char     rsv3;

    unsigned char     version[32];  /*0x04  */
    
    unsigned int   whyReboot;  /*0x24  */
    unsigned char     excSave;  /*0x28  */
    unsigned char     omLogSave; 
    unsigned char     omMsgSave; 
    unsigned char     cpuViewSave;
    unsigned char     wdtSave; /*0x2C  */
    unsigned char     sdExcSave; 
    unsigned char     sciExcSave;
    unsigned char     usbExcSave;
    unsigned int    rebootTask; /*0x30  */
    unsigned int    rebootTicks; /*0x34  */

    unsigned int   modId; /*0x38  */
    int          arg1; 
    int          arg2; /*0x40  */
    unsigned int   omTimer; 
    
    unsigned int   vec;  /*0x48  */
    unsigned int   cpsr; /*0x4C  */
    unsigned int   regSet[18]; /*0x50  */
        
    unsigned char     taskName[16];  /*0x98  */
    
    unsigned int   taskId;      /*0xA8 */
    unsigned int   taskTicks; 
    
    unsigned int   intId;        /*0xB0  */
    unsigned int   intTicks;
    unsigned int   subIntId;  /*0xB8  */
    unsigned int   subIntTicks;

    unsigned int   excSize; /*0xC0  */
    unsigned int   omLogSize; /*0xC4  */
    unsigned int   omMsgSize; 

    unsigned int  osMsgAddr;  /*0xCC  */
    unsigned int  taskTcbAddr; /*0xD0*/
    unsigned int  customAddr;
    unsigned int  intAddr;
    unsigned int  subintAddr; /*0xDC  */
    unsigned int  taskSwitchAddr;/*0xE0*/
    unsigned int  intStackAddr;
    unsigned int  taskStackAddr;
    unsigned int  taskExcStackAddr;/*0xEC  */
    unsigned int  cpuViewAddr;       /*0xF0*/
    unsigned int  taskAllTcbAddr;	
    
    unsigned int  omLogAddr;
    unsigned int  omMsgAddr;/*0xFC  */
    unsigned int  sdExcAddr;    /*0X100*/
    unsigned int  sciExcAddr;
    unsigned int  usbExcAddr;
    unsigned int  reserved; /*0x10C*/
    unsigned int  freeHeapSize; /*0x110*/
    unsigned int  maxFreeBlockSize; /*0x114*/
    unsigned int  reserved1; /*0x118*/
    
    unsigned int  wdtRebootFlag; /*0x11c*/
   
} EH_ROOT;


typedef struct
{
    unsigned long tid;
    char taskname[12];
} TASK_NAME_STRU;

#define MAX_TASK_STACK_RATE  9/10    /*此处宏定义不能加括号，否则影响计算结果*/
#define MAX_TASK_NUM  128

extern void  usrPlatformReset(void);
extern void wdtReboot(void);

#if((FEATURE_ON == FEATURE_LTE) && (FEATURE_ON == FEATURE_EPAD || FEATURE_ON == FEATURE_PHONE))
/*****************************************************************************
  Function    : exchSetSocpLogOnFlag
  Description : 保存LOG2.0启用状态
  Calls       : 
  Called By   : 
  Input       : NULL
  Output      : NULL
  Return      : NULL
*****************************************************************************/
void exchSetSocpLogOnFlag();

/*****************************************************************************
  Function    : exchSetSocpLogInfo
  Description : 保存LOG2.0复位信息，SOCP编码目的buffer数据信息
  Calls       : 
  Called By   : 
  Input       : SOCP_LOG_EXC_INFO_STRU * logInfo
  Output      : NULL
  Return      : int     BSP_OK:成功,BSP_ERROR:失败
*****************************************************************************/
int exchSetSocpLogInfo();
#endif

#endif    /* end of _EXC_DRV_H. */


