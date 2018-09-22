/*
 *  linux/arch/arm/mach-balong/power_exchange.c
 *
 *  Copyright (C) 1996-2011 huawei - Converted to ARM.
 *  Original Copyright (C) 1995  Linus Torvalds
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <generated/FeatureConfig.h>

#if(FEATURE_POWER_ON_OFF == FEATURE_ON )
#include <mach/powerExchange.h>
#include <mach/common/mem/bsp_mem.h>

 
#define POWER_VAR_ADDR     (MEMORY_AXI_ONOFF_ADDR)
#define POWER_VAR_SIZE     (32)

/******************************************************************************
*  Function:  power_on_reboot_flag_set
*  Description: set the reboot flag in shared memory
*  Input:
*         eRebootFlag : reboot flag
*  Output: 
*         None
*  Return:
*         None
*  Note  : 
*         This function can be called in vxWorks / Bootload / Linux kernel.
*         so, shouldnt access other functions.
*  History:     
*               1. Created by l00131505.(2011-10-17)
********************************************************************************/ 
void power_on_reboot_flag_set( POWER_REBOOT_FLAG_ENUM enFlag )
{
    POWER_VAR_STRU *pstVar = (POWER_VAR_STRU *)(POWER_VAR_ADDR);
    
    pstVar->ulReBootFlag = (unsigned long)(enFlag);
    
    printk( " \r\n #########  power_on_reboot_flag_set = 0x%08X ######## \r\n", enFlag );
}


/******************************************************************************
*  Function:  power_on_reboot_flag_get
*  Description: get the reboot flag in shared memory
*  Input:
*         None
*  Output: 
*         None
*  Return:
*         current reboot flag;
*  Note  : 
*         This function can be called in vxWorks / Bootload / Linux kernel.
*         so, shouldnt access other functions.
*  History:     
*               1. Created by l00131505.(2011-10-17)
********************************************************************************/
POWER_REBOOT_FLAG_ENUM power_on_reboot_flag_get( void )
{
    POWER_VAR_STRU *pstVar = (POWER_VAR_STRU *)(POWER_VAR_ADDR);
    
    printk( " \r\n #########  power_on_reboot_flag_get = 0x%08X ######## \r\n", pstVar->ulReBootFlag );
    return (POWER_REBOOT_FLAG_ENUM)pstVar->ulReBootFlag;
}


/******************************************************************************
*  Function:  power_on_start_reason_set
*  Description: set the boot reason. 
*  Input:
*         Boot reason
*  Output: 
*         None
*  Return:
*         None
*  Note  : 
*         This function can be called in vxWorks / Bootload / Linux kernel.
*         so, shouldnt access other functions.
*  History:     
*               1. Created by l00131505.(2011-10-17)
********************************************************************************/
void power_on_start_reason_set( POWRE_ON_START_REASON_ENUM enReason )
{
    POWER_VAR_STRU *pstVar = (POWER_VAR_STRU *)(POWER_VAR_ADDR);
    
    pstVar->ulStartReason = (unsigned long)(enReason);
    
    printk( " \r\n #########  power_on_start_reason_set = 0x%08X ######## \r\n", enReason );
}

/******************************************************************************
*  Function:  power_on_start_reason_get
*  Description: get the start reason
*  Input:
*         None
*  Output: 
*         None
*  Return:
*         see POWER_ON_START_REASON_ENUM
*  Note  : 
*         This function can be called in vxWorks / Bootload / Linux kernel.
*         so, shouldnt access other functions.
*  History:     
*               1. Created by l00131505.(2011-10-17)
********************************************************************************/
POWRE_ON_START_REASON_ENUM power_on_start_reason_get( void )
{
    POWER_VAR_STRU *pstVar = (POWER_VAR_STRU *)(POWER_VAR_ADDR);
    
    printk( " \r\n #########  power_on_start_reason_get = 0x%08X ######## \r\n", pstVar->ulStartReason );
    return (POWRE_ON_START_REASON_ENUM)(pstVar->ulStartReason);
}

/******************************************************************************
*  Function:  power_on_status_set
*  Description: tet the boot reason. the area should be accessed by Boot/Vxworks.
*  Input:
*         Boot reason
*  Output: 
*         None
*  Return:
*         None
*  Note  : 
*         This function can be called in vxWorks / Bootload / Linux kernel.
*         so, shouldnt access other functions.
*  History:     
*               1. Created by l00131505.(2011-10-17)
********************************************************************************/
void power_on_status_set( POWER_ON_STATUS_ENUM enStatus )
{
    POWER_VAR_STRU *pstVar = (POWER_VAR_STRU *)(POWER_VAR_ADDR);
    
    pstVar->ulPowerOnStatus = (unsigned long)(enStatus);
    
    printk( " \r\n #########  power_on_status_set = 0x%08X ######## \r\n", pstVar->ulPowerOnStatus );
}

/******************************************************************************
*  Function:  power_on_status_get
*  Description: get the power on status
*  Input:
*         None
*  Output: 
*         None
*  Return:
*         see POWER_ON_STATUS_ENUM
*  Note  : 
*         This function can be called in vxWorks / Bootload / Linux kernel.
*         so, shouldnt access other functions.
*  History:     
*               1. Created by l00131505.(2011-10-17)
********************************************************************************/
POWER_ON_STATUS_ENUM power_on_status_get( void )
{
    POWER_VAR_STRU *pstVar = (POWER_VAR_STRU *)(POWER_VAR_ADDR);
    
    printk( " \r\n #########  power_on_status_get = 0x%08X ######## \r\n", pstVar->ulPowerOnStatus );
    return (POWER_ON_STATUS_ENUM)(pstVar->ulPowerOnStatus);
}


/******************************************************************************
*  Function:  power_off_linux_status_set
*  Description: set the linux power off status
*  Input:
*         Linux power off status
*  Output: 
*         None
*  Return:
*         None
*  Note  : 
*         This function can be called in vxWorks / Bootload / Linux kernel.
*         so, shouldnt access other functions.
*  History:     
*               1. Created by l00131505.(2011-10-17)
********************************************************************************/
void power_off_linux_status_set( POWER_OFF_LINUX_STATUS_ENUM enStatus )
{
    POWER_VAR_STRU *pstVar = (POWER_VAR_STRU *)(POWER_VAR_ADDR);
    
    pstVar->ulPowerOffLinuxStatus = (unsigned long)(enStatus);    
    
    printk( " \r\n #########  power_off_linux_status_set = 0x%08X ######## \r\n", pstVar->ulPowerOffLinuxStatus );
}

/******************************************************************************
*  Function:  power_off_linux_status_get
*  Description: get the linux power off status.
*  Input:
*         None
*  Output: 
*         None
*  Return:
*         see POWER_OFF_LINUX_STATUS_ENUM
*  Note  : 
*         This function can be called in vxWorks / Bootload / Linux kernel.
*         so, shouldnt access other functions.
*  History:     
*               1. Created by l00131505.(2011-10-17)
********************************************************************************/
POWER_OFF_LINUX_STATUS_ENUM power_off_linux_status_get( void )
{
    POWER_VAR_STRU *pstVar = (POWER_VAR_STRU *)(POWER_VAR_ADDR);
    
    printk( " \r\n #########  power_off_linux_status_get = 0x%08X ######## \r\n", pstVar->ulPowerOffLinuxStatus );
    return (POWER_OFF_LINUX_STATUS_ENUM)(pstVar->ulPowerOffLinuxStatus);
}


/******************************************************************************
*  Function:  power_on_set_warm_reset_flag
*  Description: set the warm reset flag
*  Input:
*         None
*  Output: 
*         None
*  Return:
*         see POWER_OFF_LINUX_STATUS_ENUM
*  Note  : 
*         This function can be called in vxWorks / Bootload / Linux kernel.
*         so, shouldnt access other functions.
*  History:     
*               1. Created by l00131505.(2011-10-17)
********************************************************************************/
void power_on_set_warm_reset_flag( POWER_ON_WARM_RESET_FLAG_ENUM enFlag )
{
    POWER_VAR_STRU * pstVar = (POWER_VAR_STRU *)(POWER_VAR_ADDR);

    pstVar->ulWarmResetFlag = (unsigned long)(enFlag);
}

/******************************************************************************
*  Function:  power_on_get_warm_reset_flag
*  Description: get the warm reset flag
*  Input:
*         None
*  Output: 
*         None
*  Return:
*         see POWER_ON_WARM_RESET_FLAG_ENUM
*  Note  : 
*         This function can be called in vxWorks / Bootload / Linux kernel.
*         so, shouldnt access other functions.
*  History:     
*               1. Created by l00131505.(2011-10-17)
********************************************************************************/
POWER_ON_WARM_RESET_FLAG_ENUM power_on_get_warm_reset_flag( void )
{
    POWER_VAR_STRU * pstVar = (POWER_VAR_STRU *)(POWER_VAR_ADDR);

    return (POWER_ON_WARM_RESET_FLAG_ENUM)(pstVar->ulWarmResetFlag);
}

/******************************************************************************
*  Function:  print_exchange_addr
*  Description: print address
*  Input:
*         None
*  Output: 
*         None
*  Return:
*         None
*  Note  : 
*         This function can be called in vxWorks / Bootload / Linux kernel.
*         so, shouldnt access other functions.
*  History:     
*               1. Created by l00131505.(2011-10-17)
********************************************************************************/
void print_exchange_addr( void )
{
    unsigned long * ulPtr = (unsigned long *)(POWER_VAR_ADDR);
    
    printk( "\r\n ############ PowerExchange(Linux) Addr = 0x%08X \r\n    0x00 = 0x%08X, 0x04 = 0x%08X, 0x08 = 0x%08X, 0x0C = 0x%08X, 0x10 = 0x%08X\r\n", 
         POWER_VAR_ADDR, *(ulPtr+0), *(ulPtr+1), *(ulPtr+2), *(ulPtr+3), *(ulPtr+4) );
         
}
 
 
#endif 
 
 
 