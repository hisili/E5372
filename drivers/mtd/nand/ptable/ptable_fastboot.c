/*******************************************************************************
* Copyright (C), 2008-2011, HUAWEI Tech. Co., Ltd.
*
* Module name: flash partition table
*
* Version: v1.0
*
* Filename:    part_fastboot.c
* Description:  Balong plantform boot loadr partition image(s) loading function
*
* Function List:
*
* History:
1.date:2011-11-19
 question number:
 modify reasion:         create
*******************************************************************************/
/******************************************************************************
*    Copyright (c) 2009-2011 by  Hisilicon Tech. Co., Ltd.
*    All rights reserved.
* ***
*
******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/*#define MODULE_MAIN          NAND*/
#define SUB_MODULE              NANDC_TRACE_PTABLE

#include "ptable_def.h"
#include "flash.h"

struct ptentry pt_find;
/******************************************************************************************
 * FUNC NAME:  
 * @flash_find_ptn() - external API:  
 *                           
 * PARAMETER: 
 * @name   -[input] which partition to be find for fastboot                
 *
 * RETURN:
 *    partition info of struct ptentry
 *
 * DESCRIPTION:
 *     find fastboot partition informatin from flash partition table in AXI memory       
 *
 * CALL FUNC:
 *****************************************************************************************/
struct ptentry *flash_find_ptn(const char *name)
{
    struct ST_PART_TBL *part = NULL;
    
    part = (struct ST_PART_TBL *)PTABLE_RAM_TABLE_ADDR;

    while(0 != strcmp(PTABLE_END_STR, part->name))
    {
        if((0 == strcmp(name ,  part->name)) && (DATA_VALID == ptable_get_validity(part)))
        {
            pt_find.flags = 0;
            pt_find.length = part->loadsize;
            pt_find.start = part->offset;

            strcpy(pt_find.name, part->name);
            
            return &pt_find;
        }

        part++;
    }

    return 0;
}

#ifdef __cplusplus
}
#endif




