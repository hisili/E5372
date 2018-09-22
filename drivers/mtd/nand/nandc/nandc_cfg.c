/*******************************************************************************
* Copyright (C), 2008-2011, HUAWEI Tech. Co., Ltd.
*
* Module name: Hisilicon Nand Flash Controller Dirver
*
* Description: the Hisilicon Nand Flash Controller physical and logical driver
*
* Version: v1.0
*
* Filename:    nandc_cfg.c
* Description: nand controller operations in dependence on  hardware
*
* Function List:
*
* History:
* 1.date:2011-07-27
* question number:
* modify reasion:         create
*******************************************************************************/


#ifdef __cplusplus
extern "C"
{
#endif

#include "nandc_inc.h"

u32 nandc_trace_mask = (NANDC_TRACE_WARNING | (NANDC_TRACE_PTABLE | NANDC_TRACE_NAND | NANDC_TRACE_NATIVE));

/*****************************************************************************/

/*****************************************************************************/
#if defined(NANDC_USE_V200)

struct nandc_init_info  *nandc_init_seed = &nandc2_init_info;

#elif defined (NANDC_USE_V320)

struct nandc_init_info  *nandc_init_seed = &nandc3_init_info;

#elif defined (NANDC_USE_V400)

struct nandc_init_info  *nandc_init_seed = &nandc4_init_info;

#else
#error no bsp board defined!!
#endif

int *nandc_block_buffer = (int*)RVDS_TRACE_WRITE_BUFF ;//0xc6000000; /*for RVDS trace write*/

void nandc_set_trace(u32 traceMask)
{
    nandc_trace_mask = traceMask;
}

u32 nandc_get_trace(void)
{
    return nandc_trace_mask;
}

#ifdef __cplusplus
}
#endif


