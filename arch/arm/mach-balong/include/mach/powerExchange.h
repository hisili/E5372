/*
 *  include/mach/powerExchange.h
 *
 *  Copyright (C) 1996-2011 Huawei.
 *  Original Copyright (C) 1995  Linus Torvalds
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef POWER_EXCHANGE_H
#define POWER_EXCHANGE_H

typedef enum POWER_REBOOT_FALG_tag
{
    POWER_REBOOT_FLAG_INVALID  = 0,
    POWER_REBOOT_FLAG_BATTERY_ERROR = 0x50464245,
    POWER_REBOOT_FLAG_TEMP_PROTECT  = 0x50465450,
    POWER_REBOOT_FLAG_NORMAL        = 0x50464E4C,
    POWER_REBOOT_FLAG_UPDATE        = 0x50465550,
    POWER_REBOOT_FLAG_BY_CHARGE     = 0x5046424C,
    POWER_REBOOT_FLAG_BUTT          = 0xFFFFFFFF
}POWER_REBOOT_FLAG_ENUM;

typedef enum POWER_ON_START_REASON_tag
{
    POWER_ON_START_REASON_INVALID = 0x00000000,
    POWER_ON_START_REASON_CHARGE    = 0x504f4348,
    POWER_ON_START_REASON_POWER_KEY = 0x504F504B,
    POWER_ON_START_REASON_WARM_RESET= 0x504F5752,
    POWER_ON_START_REASON_BUTT      = 0xFFFFFFFF
}POWRE_ON_START_REASON_ENUM;

typedef enum POWER_ON_STATUS_tag
{
    POWER_ON_STATUS_INVALID         = 0x00000000,
    POWER_ON_STATUS_BOOTING         = 0x00000000,
    POWER_ON_STATUS_CHARGING        = 0x504F5343,
    POWER_ON_STATUS_CAPACITY_LOW    = 0x504F434C,
    POWER_ON_STATUS_BATTERY_ERROR   = 0x504F4245,
    POWER_ON_STATUS_FINISH_NORMAL   = 0x504F464E,
    POWER_ON_STATUS_FINISH_CHARGE   = 0x504F4643,
    POWER_ON_STATUS_BUTT            = 0xFFFFFFFF    
}POWER_ON_STATUS_ENUM;

typedef enum POWER_OFF_LINUX_STATUS_tag
{
    POWER_OFF_LINUX_STATUS_WORKING  = 0x00000000,
    POWER_OFF_LINUX_STATUS_SHUTDOWN = 0x50534944,
    POWER_OFF_LINUX_STATUS_BUTT     = 0xFFFFFFFF
}POWER_OFF_LINUX_STATUS_ENUM;

typedef enum POWER_ON_WARM_RESET_FLAG_tag
{
    POWER_ON_WARM_RESET_FLAG_INVALID    = 0x00000000,
    POWER_ON_WARM_RESET_FLAG            = 0xCA0F37E1,
    POWER_ON_WARM_RESET_FLAG_BUTT       = 0xFFFFFFFF
}POWER_ON_WARM_RESET_FLAG_ENUM;

typedef struct
{
    volatile unsigned long ulReBootFlag;
    volatile unsigned long ulStartReason;
    volatile unsigned long ulPowerOnStatus;
    volatile unsigned long ulPowerOffLinuxStatus;
    volatile unsigned long ulWarmResetFlag;
}POWER_VAR_STRU;
 

void power_on_reboot_flag_set( POWER_REBOOT_FLAG_ENUM enFlag );
POWER_REBOOT_FLAG_ENUM power_on_reboot_flag_get( void );

void power_on_start_reason_set( POWRE_ON_START_REASON_ENUM enReason );
POWRE_ON_START_REASON_ENUM power_on_start_reason_get( void );

void power_on_status_set( POWER_ON_STATUS_ENUM enStatus );
POWER_ON_STATUS_ENUM power_on_status_get( void );

void power_off_linux_status_set( POWER_OFF_LINUX_STATUS_ENUM enStatus );
POWER_OFF_LINUX_STATUS_ENUM power_off_linux_status_get( void );

void print_exchange_addr( void );


#endif

