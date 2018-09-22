/*******************************************************************************
* Copyright (C), 2008-2011, HUAWEI Tech. Co., Ltd.
*
* Module name: Hisilicon Nand Flash Controller Dirver
*
* Description: the Hisilicon Nand Flash Controller physical and logical driver
*
* Version: v1.0
*
* Filename:    nandc_inc.h
* Description: nand controller operations in dependence on  hardware
*
* Function List:
*
* History:
1.date:2011-07-27
 question number:
 modify reasion:        create
*******************************************************************************/
/******************************************************************************
*    Copyright (c) 2009-2011 by  Hisilicon Tech. Co., Ltd.
*    All rights reserved.
* ***
*
******************************************************************************/
#ifndef _NANDC_INC_H_
#define _NANDC_INC_H_
    
#ifdef __cplusplus
extern "C"
{
#endif


#include "nandc_port.h"
#include "nandc_cfg.h"
#include "nandc_mco.h"
#include "nandc_def.h"
#ifndef __KERNEL__ 
#include "nandc_linux.h"
#endif
#include "nandc_ctrl.h"


#include "ptable_api.h"

extern u32 nandc_trace_mask;


u32     nandc_data_transmit_raw(struct nandc_host *host, u32 datasize, u32 bufoffset, u32 access);
u32     nandc_data_transmit_page(struct nandc_host *host, struct nandc_pagemap *pagemap, u32 times, u32 access);
u32     nandc_native_nand_prob(struct nandc_host* host);
void    nandc_native_host_delete(struct nandc_host* host);
u32     nandc_native_location_by_pagenum(struct nandc_host* host, u32 pagenum);
u32     nandc_native_location_by_address(struct nandc_host* host, FSZ address);
u32     nandc_native_location_by_blocknum(struct nandc_host* host, u32 blknum);
u32     nandc_ctrl_entry(struct nandc_host *host);
u32     nandc_io_bitwidth_set(u32 bitwidth);
u32     nandc_nand_set_address(struct nandc_host *host);
u32     nandc_bbm_real_addr(FSZ addr_virtual, FSZ *addr_real);
u32     nandc_host_set_define(struct nandc_host * host, struct mtd_partition* ptable, u32 nr_parts);
u32     nandc_init_mtd_partition(struct nandc_host* host);
u32     nandc_host_add_nand_chip(struct nandc_host * host,  u32 chipselect, struct nandc_spec *spec);
u32     nandc_host_init_cluster(struct nandc_host *host, struct nandc_reg_desc* reg_tbl, u8 *bitstbl);
u32     nandc_host_set_chipparam(struct nandc_host * host);
u32     nandc_host_init_regfield(struct nandc_host *host, u32 regsize);
u32     nandc_nand_create_host(struct nandc_host  **host);
u32     nandc_nand_yaffs_read(u32 pagenum, u8* databuff, u8* oobbuff, u32 oobsize);
u32     nandc_nand_yaffs_write(u32 pagenum, u8* databuff, u8* oobbuff, u32 oobsize);
void    hiwin_regdebug_init_buf(u32 address, s32 length);
struct nandc_host*  nandc_native_host_create(struct nandc_init_info* init_info);
struct reg_debuger* hiwin_regdebug_create(u32 regsize, u32 chipbase ,char* name);
struct nandc_host*  nandc_native_host_create(struct nandc_init_info* init_info);


void*   nandc_get_block_buf(u32 size);
void    nandc_give_block_buf(void * p);
void*   nandc_malloc(u32 size);
void    nandc_free(void* p);
u32     nandc_mutex_create(void** sem);
u32     nandc_mutex_lock(void* hsem);
u32     nandc_mutex_unlock(void* sem);
u32     nandc_mutex_delete(void* sem);
u32     nandc_ipc_create(void);
u32     nandc_ipc_delete(void);
u32     nandc_ipc_wait(u32 overtime);
void    nandc_ipc_post(void);


 
extern struct nandc_init_info   nandc2_init_info;
extern struct nandc_init_info   nandc3_init_info;
extern struct nandc_init_info   nandc4_init_info;
extern struct nand_interface    nandc_nand_funcs;
extern struct nandc_init_info  *nandc_init_seed;
extern int *nandc_block_buffer;

u32 nandc_bbm_real_addr(FSZ addr_virtual, FSZ *addr_real);

#ifdef __cplusplus
}
#endif

#endif  /*_NANDC_INC_H_*/


