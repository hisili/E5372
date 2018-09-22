/*******************************************************************************
* Copyright (C), 2008-2011, HUAWEI Tech. Co., Ltd.
*
* Module name: flash partition table
*
* Version: v1.0
*
* Filename:    part_dload.c
* Description:  Balong plantform partition operation native functions
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
#define SUB_MODULE           NANDC_TRACE_PTABLE

#include <strlib.h>/*lint -e322 */
#include "dload-pri.h"
#include "yaffsDrv.h"
#include "ptable_inc.h"

static u32 ptable_is_changed = 0;
u32 g_ulFlagOfSetIntegrity = 0;

extern u32 ptable_show(struct ST_PART_TBL *part);


/*
  +m00176101 buf size shoule align with USB pkt size(4K),
  nand blocksize(128K or 256K),and page+spare size(2K+64 
  or 4K+128)
*/
/*oob is 16, change this to 0x1020000 @2011104*//*for 2K page nand , 
  4k page nand should be 0x840000
*/    
/* 16M太大写入Flash时间长，导致PC侧工具概率超时，这里改为2M左右 */
#define DLOAD_NAND_SPARE_ALIGN      (0x204000) 

/******************************************************************************************
 * FUNC NAME:  
 * @ptable_replace_protected() - external API:  
 *                           
 * PARAMETER: 
 * @new_tbl   -[input] the flash table that should replace current flash table                
 *
 * RETURN:
 *    operation result
 *
 * DESCRIPTION:
 *    copy old protected partition's data to ram and then write it to new table        
 *
 * CALL FUNC:
 *****************************************************************************************/
static u32 ptable_replace_protected(u32 new_tbl, u32 old_tbl)
{
    struct ST_PART_TBL * ptble_old = NULL;
    struct ST_PART_TBL * ptble_new = NULL;
    struct ST_PART_TBL * ptble_temp = NULL;
    u32 ret_val = NAND_OK;
    u32 data_type;

    ptble_new = (struct ST_PART_TBL *)((u32)new_tbl + PTABLE_HEAD_SIZE);

    /*secend: write protected partition to flash */
    ptble_old = (struct ST_PART_TBL *)((u32)old_tbl + PTABLE_HEAD_SIZE);
    while(0 != strcmp(PTABLE_END_STR, ptble_old->name))
    {
        if(NULL != ptble_old->loadaddr)
        {
            ptble_temp = ptable_find_by_name(ptble_old->name, ptble_new);
            if(NULL == ptble_temp)
            {
                TRACE_ERROR(("ptable_replace_protected: ptable_find_by_name(%s) ERROR!\n",ptble_old->name));
                ret_val = NAND_ERROR;
                ptble_old++;
                continue;               
            }

            TRACE_ERROR(("protect copy: ptble_old(0x%x)->ptble_new(0x%x)!\n", ptble_old->offset,
                          ptble_temp->offset ));
            
            data_type = (ptble_old->property & PTABLE_IMAGE_TYPE_MASK);
            
            /*按数据类型从ram空间写入flash*/
            if(NAND_OK !=  ptable_write_by_type((FSZ) ptble_temp->offset, ptble_old->loadaddr, data_type, ptble_old->capacity, 0))
            {
                TRACE_ERROR(("ptable_replace_protected(%s): nand_write ERROR!\n", ptble_old->name));
                ret_val = NAND_ERROR;
                /*goto EXIT;*/
            }
        }
        
        ptble_old++;
    }

EXIT:
    /*last: release memory malloced */
    ptble_old = (struct ST_PART_TBL *)((u32)old_tbl + PTABLE_HEAD_SIZE);
    while(0 != strcmp(PTABLE_END_STR, ptble_old->name))
    {
        if(NULL != ptble_old->loadaddr)
        {
            hifree((void*)ptble_old->loadaddr);
            ptble_old->loadaddr = 0;
        }

        ptble_old++;
    }
	
    return ret_val;
}

/****************************************************************************************
 * FUNC NAME:  
 * ptable_replace_ram_table() - external API:  
 *                           
 *
 * PARAMETER:
 * @data_tbl -[input] which name of partition to find             
 *
 * DESCRIPTION:
 *     replace ram table by data_tbl, the ram table is working flash table. 
 *      
 * CALL FUNC:
 * 
 ***************************************************************************************/
u32 ptable_replace_ram_table(u32 data_tbl, u32 size)
{
    struct ST_PART_TBL * ptble_old = NULL;
    struct ST_PART_TBL * ptble_new = NULL;
    u32 ptable_old_head = 0;
    u32 ptable_new_head = 0;
    u32 is_diff = NANDC_FALSE;
    u32 validaty_mask;
    u32 is_verback = NANDC_FALSE;
    
    if(NULL == data_tbl)
    {
        return NAND_OK;
    }

    /*分区表头的属性比较*/
    ptable_old_head = (u32)ptable_get_ram_head();
    ptable_new_head = (u32)data_tbl;

    /* "二次升级版本"向"非二次升级版本"回退时，不比较bootrom和ptable版本号 */
    if(((!strncmp((const void *)(ptable_new_head + PTABLE_HEAD_PROPERTY_OFFSET),"HI6920",strlen("HI6920"))) \
            || (!strncmp((const void *)(ptable_new_head + PTABLE_BOOTROM_VER_OFFSET),"HI6920",strlen("HI6920")))) \
            && (!strncmp((const void *)(ptable_old_head + PTABLE_BOOTROM_VER_OFFSET),"BOOTROM",strlen("BOOTROM"))))
    {
        is_verback = NANDC_TRUE;
        TRACE_WARNING(("ptable_replace_ram_table:[WARN]version back!\n"));
    }
    
    if(NANDC_TRUE != is_verback)
    {   
        /*比较bootrom版本号*/
        if((0 != memcmp((const void *)(ptable_old_head + PTABLE_BOOTROM_VER_OFFSET), (const void *)(ptable_new_head + PTABLE_BOOTROM_VER_OFFSET), PTABLE_BOOTROM_VER_SIZE)))
        {
            /*bootrom版本号发生变化，设置标志位*/
            is_diff = NANDC_TRUE;
            goto EXIT;
        }
        
        /*比较ptable版本号*/
        if((0 != memcmp((const void *)(ptable_old_head + PTABLE_NAME_OFFSET), (const void *)(ptable_new_head+ PTABLE_NAME_OFFSET), PTABLE_NAME_SIZE)))
        {
            /*ptable版本号发生变化，设置标志位*/
            is_diff = NANDC_TRUE;
            goto EXIT;
        }
    }

    ptble_old = ptable_get_ram_data();
    ptble_new = (struct ST_PART_TBL *)((u32)data_tbl + PTABLE_HEAD_SIZE);

    validaty_mask = (PTABLE_VALIDITY_MASK << PTABLE_VALIDITY_OFFSET);
    
    /*检查 新老table,除了loadsize 和 count 是否有任何不同*/
    while((0 != strcmp((const void *)PTABLE_END_STR, (const void *)ptble_old->name)) &&
          (0 != strcmp((const void *)PTABLE_END_STR, (const void *)ptble_new->name)))
    {
        if(
                (ptble_old->offset != ptble_new->offset)      ||
                (ptble_old->capacity != ptble_new->capacity)  ||
                (ptble_old->loadaddr != ptble_new->loadaddr)  ||
                (ptble_old->entry != ptble_new->entry)  ||
                (ptble_old->image != ptble_new->image)  ||
                (((ptble_old->property | validaty_mask) != (ptble_new->property | validaty_mask)) && (NANDC_TRUE != is_verback))  || /*属性比较需要排除有效性差别*/
                (0 != strcmp( (const void *)ptble_old->name, (const void *)ptble_new->name))
        )
        {
            /*发现不同，设置标志位*/
            is_diff =  NANDC_TRUE;
            goto EXIT;
        }
        
        ptble_old++;
        ptble_new++;
    }


    /*新老表是否个数有差异*/
    if((0 != strcmp(PTABLE_END_STR, ptble_new->name))||(0 != strcmp(PTABLE_END_STR, ptble_old->name)))
    {
        /*发现个数差异，设置标志位*/
        is_diff = NANDC_TRUE;
        goto EXIT;
    }

EXIT:
    /*除了 loadsize 和 count 有任何不同，替换分区表*/
    if((NULL != data_tbl)&&(NANDC_TRUE == is_diff))
    {
        if(size <= PTABLE_RAM_TABLE_SIZE )
        {
            /*新table的大小不超过原来的大小，直接拷贝 */
            memcpy((char *)ptable_get_ram_head(), (const char*)data_tbl, size);
            TRACE_WARNING(("ptable_replace_ram_table, DONE!! \n"));

        }
        else
        {
            /*新table的大小超过了原来的大小，只拷贝原来分区大小的数据*/
            memcpy((char *)ptable_get_ram_head(), (const char*)data_tbl, PTABLE_RAM_TABLE_SIZE);

            /*在最后一个分区强制加上结束标志*/
            ptble_old = ptable_get_ram_head();
            memcpy(&ptble_old[(PTABLE_RAM_TABLE_SIZE/sizeof(struct ST_PART_TBL)) - 1], PTABLE_END_STR , sizeof(PTABLE_END_STR));
            
            TRACE_WARNING(("WARNING! ptable_replace_ram_table:new table size too large!! \n"));
        }
		
		ptable_set_table_changed(TRUE);
            
    	return nandc_init_mtd_partition(nandc_nand_host);
    }

    return NAND_OK;
}


/****************************************************************************************
 * FUNC NAME:  
 * ptable_replace_flash_table() - external API:  
 *                           
 * PARAMETER:
 * @none            
 *
 * DESCRIPTION:
 *     write back ram table to flash(block 0) 
 *      
 * CALL FUNC:
 * 
 ***************************************************************************************/
u32 ptable_replace_flash_table(void)
{
#ifndef NANDC_SKIP_DEBUG
    ptable_show(ptable_get_ram_head());
#endif

    return nand_write((FSZ)PART_TABLE_NAND_OFFSET, (u32)ptable_get_ram_head(), PTABLE_RAM_TABLE_SIZE, 0);
}

/******************************************************************************************
 * FUNC NAME:  
 * @ptable_prepare_yaffs() - external API:  
 *                           
 * PARAMETER: 
 * @new_tbl   -[input] the flash table that should replace current flash table                
 *
 * RETURN:
 *    operation result
 *
 * DESCRIPTION:
 *    如发现新老表,yaffs分区信息有改变,擦除新表yaffs分区  
 *
 * CALL FUNC:
 *****************************************************************************************/
static u32 ptable_prepare_yaffs(u32 new_tbl, u32 old_tbl)
{
    struct ST_PART_TBL * ptble_old = NULL;
    struct ST_PART_TBL * ptble_new = NULL;

    ptble_old = (struct ST_PART_TBL *)((u32)old_tbl + PTABLE_HEAD_SIZE);
    ptble_new = (struct ST_PART_TBL *)((u32)new_tbl + PTABLE_HEAD_SIZE);

    while((0 != strcmp(PTABLE_END_STR, ptble_old->name)) &&
          (0 != strcmp(PTABLE_END_STR, ptble_new->name)))
   {
        if(
            (DATA_YAFFS == (ptble_new->property & PTABLE_IMAGE_TYPE_MASK)) &&\
            (
                (ptble_old->offset != ptble_new->offset)  ||
                (ptble_old->capacity != ptble_new->capacity) ||
                (DATA_YAFFS != (ptble_old->property & PTABLE_IMAGE_TYPE_MASK)) ||
                (0 != strcmp( ptble_old->name, ptble_new->name))
            )
        )
        {
            TRACE_WARNING(("ptable_prepare_yaffs: yaffs changed (%s)\n ",ptble_new->name));
            nand_scan_bad(ptble_new->offset, ptble_new->capacity, TRUE);
        }
        
        ptble_old++;
        ptble_new++;
    }

    while(0 != strcmp(PTABLE_END_STR, ptble_new->name))
    {
        /*分区个数有差异*/
        if(DATA_YAFFS == (ptble_new->property & PTABLE_IMAGE_TYPE_MASK))
        {
           TRACE_WARNING(("ptable_prepare_yaffs: yaffs changed (%s)\n ",ptble_new->name));
           nand_scan_bad(ptble_new->offset, ptble_new->capacity, TRUE);
        }
        ptble_new++;
    }

    return TRUE;
}

/******************************************************************************************
 * FUNC NAME:  
 * @ptable_obtain_protected() - external API:  
 *                           
 * PARAMETER: 
 * @new_tbl   -[input] the flash table that should replace current flash table                
 *
 * RETURN:
 *    operation result
 *
 * DESCRIPTION:
 *    to find if the protected partition has changed
 *
 * CALL FUNC:
 *****************************************************************************************/
static u32 ptable_obtain_protected(u32 new_tbl, u32 old_tbl)
{
    struct ST_PART_TBL * ptble_old = NULL;
    struct ST_PART_TBL * ptble_new = NULL;
    u32 ret_val = NAND_OK;
    u32 data_type;
    
    ptble_old = (struct ST_PART_TBL *)((u32)old_tbl + PTABLE_HEAD_SIZE);
    ptble_new = (struct ST_PART_TBL *)((u32)new_tbl + PTABLE_HEAD_SIZE);

    while((0 != strcmp(PTABLE_END_STR, ptble_old->name)) &&
          (0 != strcmp(PTABLE_END_STR, ptble_new->name)))
    {
        if(
            (PTABLE_PROTECTED == (ptble_new->property & PTABLE_PROTECTED))&&\
            (	/*表被保护的情况*/
                (ptble_old->offset != ptble_new->offset)      ||
                (ptble_old->capacity != ptble_new->capacity)  ||
                (0 != strcmp( ptble_old->name, ptble_new->name))
            )
        )
        {
            /*use table_old.loadaddr as temp variable, because it has no use in this situration  */

            data_type = (ptble_old->property & PTABLE_IMAGE_TYPE_MASK);

            if(DATA_YAFFS == data_type)
            {
                /*yaffs区的读写需要获得data区和tag区的总大小*/
                ptble_old->capacity = nand_yaffs_write_size(ptble_old->capacity);
            }
            
            ptble_old->loadaddr = (u32)himalloc(ptble_old->capacity);
            if(NULL == ptble_old->loadaddr)
            {
                TRACE_ERROR(("ptable_obtain_protected: malloc fail!\n"));
                ret_val = NAND_ERROR;
            }
  
            if(NAND_OK != ptable_read_by_type((FSZ) ptble_old->offset, ptble_old->loadaddr, data_type, ptble_old->capacity))
            {
                TRACE_ERROR(("ptable_obtain_protected: nand_read ERROR!\n"));
                ret_val = NAND_ERROR;
            }
        }
        else
        {
            /*此指针置零，后续不进行拷贝也不会进行free释放*/
            ptble_old->loadaddr = NULL;
        }
        
        ptble_old++;
        ptble_new++;
    }

    while(0 != strcmp(PTABLE_END_STR, ptble_old->name))
    {
        /*分区个数有差异*/
        if(PTABLE_PROTECTED == (ptble_old->property & PTABLE_PROTECTED))
        {
            /*use table_old.loadaddr as temp variable, because it has no use in this situration  */
            ptble_old->loadaddr = (u32)himalloc(ptble_old->capacity);
            if(NULL == ptble_old->loadaddr)
            {
                TRACE_ERROR(("ptable_obtain_protected: malloc fail!\n"));
                ret_val = NAND_ERROR;
            }
            
            if(NAND_OK != nand_read((FSZ)ptble_old->offset, ptble_old->loadaddr, ptble_old->capacity, 0))
            {
                TRACE_ERROR(("ptable_obtain_protected: nand_read ERROR!s\n"));
                ret_val = NAND_ERROR;
            }

        }
        else
        {
            /*此指针置零，后续不进行拷贝也不会进行free释放*/
            ptble_old->loadaddr = NULL;
        }
        
        ptble_old++;
    }

    return ret_val;
}

/******************************************************************************************
 * FUNC NAME:  
 * @ptable_flash_prepare() - external API:  
 *                           
 * PARAMETER: 
 * @start       -[input] the scan start address               
 * @length      -[input] the sean end address             
 * @image_type  -[input] flash data type               
 *
 * RETURN:
 *    operation result
 *
 * DESCRIPTION:
 *    scan flash area to find bad block, erase flash if required.
 *
 * CALL FUNC:
 *****************************************************************************************/
u32 ptable_flash_prepare(FSZ start, FSZ length, u32 image_type)
{
    if(DATA_YAFFS == image_type)
    {
       return  nand_scan_bad(start, length, TRUE);
    }
    else
    {
#if 0
		return  nand_scan_bad(start, length, FALSE);
#else
		return	NAND_OK;
#endif
    }
    
} 

/******************************************************************************************
 * FUNC NAME:  
 * @ptable_dload_deal_table() - internal API:  
 *                           
 * PARAMETER: 
 * @ptable      -[input]              
 * @length      -[input]                   
 *
 * RETURN:
 *    operation result
 *
 * DESCRIPTION:
 *    对升级映像包中的ptable分区表映像进行处理的程序
 *
 * CALL FUNC:
 *****************************************************************************************/
static u32 ptable_dload_deal_table(struct ST_PART_TBL * ptable, u32 length)
{
    u32 old_tbl = NULL;

    ptable_show((struct ST_PART_TBL *)ptable);

    /*保持老表的数据*/
    old_tbl = (u32)himalloc(PTABLE_RAM_TABLE_SIZE);
    if(NULL == old_tbl)
    {
        TRACE_ERROR(("ptable_dload_deal_table, error malloc\n"));
        goto EXIT;
    }
    
    memcpy(old_tbl, ptable_get_ram_head(), PTABLE_RAM_TABLE_SIZE);
        
    /* find and store if protect partition changed  */
    ptable_obtain_protected(ptable, old_tbl);
    
    /*copy new table to AXI memory*/
    if(NAND_OK != ptable_replace_ram_table(ptable, length))
    {
        TRACE_ERROR(("ptable_dload_deal_table, error :ptable_replace_ram_table\n"));
        /*goto ERRO;*/
    }

	/* erase yaffs partition when new yaffs offset or size changed  */
    ptable_prepare_yaffs((u32)ptable, old_tbl);

     /* protect partition  */
    if(NAND_OK != ptable_replace_protected(ptable, old_tbl))
    {
        TRACE_ERROR(("ptable_dload_deal_table, error :ptable_replace_protected\n"));
        /*goto ERRO;*/
    } 

EXIT:
    /* 将设置分区表无效性操作挪到此处，对一键式升级/SD卡升级/在线升级同时生效 */
    if(0 == g_ulFlagOfSetIntegrity)
    {
        ptable_set_integrality(DATA_INVALID);/*设置升级开始标识位，表示升级过程开始*/
        ptable_replace_flash_table(); /*更新flash分区表 */

        g_ulFlagOfSetIntegrity = 1;
    }

    if(old_tbl)
    {
        hifree(old_tbl);
    }
}

/******************************************************************************************
 * FUNC NAME:  
 * @ptable_get_table_changed() - external API:  
 *                           
 * PARAMETER: 
 *
 * RETURN:
 *    operation result
 *
 * DESCRIPTION:
 *    获取分区表变化，供其他模块调用。
 *
 * CALL FUNC:
 *****************************************************************************************/
u32 ptable_get_table_changed(void)
{
    return ptable_is_changed;
}

/******************************************************************************************
 * FUNC NAME:  
 * @ptable_set_table_changed() - external API:  
 *                           
 * PARAMETER: 
 *
 * RETURN:
 *    operation result
 *
 * DESCRIPTION:
 *    设置分区表变化标志位。
 *
 * CALL FUNC:
 *****************************************************************************************/
void ptable_set_table_changed(u32 is_changed)
{
     ptable_is_changed = is_changed;
}

/****************************************************************************************
 * FUNC NAME:  
 * ptable_find_old_bootrom() - external API: 
 *
 * PARAMETER:
 * @part_type -[input] which type of partition to find
 * @ptable    -[input] form which partition table to find
 * @inc_count -[input] set true to updata the oldest partition's count to the newest            
 * @set_valid -[input] 设置分区表数据的有效性   TRUE:设置分区数据为有效
 *                                             FALSE:设置分区数据为无效
 *
 * DESCRIPTION:
 *     find  partition , the old ，找到最老的分区用于升级下载，并进行计数和有效性设置。
 *      
 * CALL FUNC:
 * 
 ***************************************************************************************/
struct ST_PART_TBL * ptable_find_older_part(u32 type_idx, struct ST_PART_TBL * ptable, u32  inc_count, u32 set_valid)
{
	struct ST_PART_TBL * the_older = NULL; /*保存最老的分区*/
	struct ST_PART_TBL * the_newer = NULL; /*保存最新的分区*/
    u32 last_validity;

    /*lint -e539*/
	if(NULL == ptable)
    {
		goto EXIT;
	}
    /*lint +e539*/

    while(0 != strcmp(PTABLE_END_STR, ptable->name))
    {
        if(type_idx == ptable->image)
        {
            /*lint -e525*/
        	if(NULL == the_older)
            {
            	the_older = ptable;

				/*用于计数，区分新老*/ 			
				the_newer = ptable;
			}
            /*lint +e525*/
			else
		    {
		    	/*保存较老的分区*/
				the_older = (the_older->count < ptable->count) ? the_older : ptable; 
				/*保存较新的分区*/
				the_newer = (the_newer->count < ptable->count) ? ptable : the_newer ;
			}
        }

        ptable++;
    }

    if(NULL != the_older)
    {
        /*老区计数在最新的基础上加1, 使之成为最新的分区*/
        if((NANDC_TRUE == inc_count)&&(NULL != the_newer))
        {
            the_older->count = the_newer->count + 1;
        }	

        /*获得修改之前的数据有效性*/
        last_validity = ptable_get_validity(the_older);
            
        ptable_set_validity(the_older,  set_valid);

        /*如果分区数据有效性被修改，分区表信息写回flash */
        if(last_validity != ptable_get_validity(the_older))
        {
            ptable_replace_flash_table();
        }
   }

EXIT:	
    return the_older;

}

/******************************************************************************************
 * FUNC NAME:  
 * @ptable_dload_write() - external API:  
 *                           
 * PARAMETER: 
 * @data_buf     -[input] store data to write                
 * @name_idx     -[input] the type name of partition (IMAGE_TYPE_E)          
 * @this_offset  -[input] the offset form this partition start address in flash(image_idx)               
 * @length       -[input] the data size of "data_buf" to write              
 *
 * RETURN:
 *    operation result
 *
 * DESCRIPTION:
 *    wirte ram data to flash ,according to image type and partitin offset
 *
 * CALL FUNC:
 *****************************************************************************************/
u32 ptable_dload_write(u32 data_buf, u32 image_idx, u32 this_offset , u32 length)
{
    struct ST_PART_TBL * part = NULL;
    u32 write_addr;
    u32 data_type;
    u32 cur_skip_len = 0;   /*本次映像写入的坏块跳过长度*/
    static u32 total_skip_len = 0; /*一个映像分次写入过程中的坏块跳过总长度*/
    static u32 last_type = IMAGE_PART_BOTTOM;

    TRACE_WARNING(("ptable_dload_write:data_buf=0x%x,image_idx=0x%x,this_offset=0x%x,length=0x%x\r\n",
        data_buf,image_idx,this_offset,length));
    if(NULL == data_buf)
    {
        TRACE_ERROR(("ptable_dload_write:[ERROR]NULL == data_buf!\n"));
        goto ERRO;
    }

    if(TYPE_TABLE == image_idx)
    {
        /*对分区表的处理*/
        ptable_dload_deal_table(data_buf, length);
    }
    else
    {
		/*升级选择最老的分区进行下载，暂不更新计数，等整个映像下载完更新,并且设置此分区数据为无效.*/
		part = ptable_find_older_part(image_idx, ptable_get_ram_data(), FALSE, FALSE);
        if(NULL == part)
        {
            TRACE_ERROR(("ptable_dload_write:[ERROR]image_idx(0x%x) not find\n",image_idx));
            goto ERRO;
        }

        data_type = (part->property & PTABLE_IMAGE_TYPE_MASK);

        /*+解决大映像分段写入同一分区跳坏块问题*/
        if((last_type != image_idx) || (0 == this_offset)) 
        {            
            last_type = image_idx;
            total_skip_len = 0;
            /*scan and mark bad block*/
            if(NAND_OK != ptable_flash_prepare((FSZ)part->offset, (FSZ)part->capacity, data_type))
            {
                TRACE_ERROR(("ptable_dload_write:[ERROR]ptable_flash_prepare error\n"));
                goto ERRO;
            }
            part->loadsize =  0;
        }
		else
		{
            this_offset = part->loadsize;
		}
       /*-解决大映像写入错误*/     

        write_addr = part->offset + this_offset + total_skip_len;
        cur_skip_len = 0;
        
        /*write flash with bad block skip and partition boundary protect in nand driver*/
        if(NAND_OK != ptable_write_by_type(write_addr, data_buf, data_type,  length, &cur_skip_len))
        {
        	TRACE_ERROR(("ptable_dload_write:ptable_write_by_type 0x%x error!\n", image_idx));
            goto ERRO;
        }

        /*nand驱动内部已经扫描并跳坏块,这里不需要，去掉*/
        total_skip_len += cur_skip_len;        
        
        /*yaffs文件系统印象大小应转换为,data区实际大小。*/
        if((DATA_YAFFS == data_type) && (0 != length))
        {
            length = nand_yaffs_data_size(length);
        }
 
        /*保存flash数据区已写入大小(不包含坏块长度)*/
        part->loadsize  += length;
    }
    
    return NAND_OK;

ERRO:
    return NAND_ERROR;
}

/******************************************************************************************
 * FUNC NAME:  
 * @ptable_dload_read() - external API:  
 *                           
 * PARAMETER: 
 * @data_buf     -[input] store data that read from flash                
 * @name_idx     -[input] the type name of partition (IMAGE_TYPE_E)          
 * @this_offset  -[input] the offset form this partition start address in flash(image_idx)               
 * @length       -[input] the data size of "data_buf" to write              
 *
 * RETURN:
 *    operation result
 *
 * DESCRIPTION:
 *    read data from flash to ram, according to image type and partitin offset.
 *
 * CALL FUNC:
 *****************************************************************************************/
u32 ptable_dload_read(u32 data_buf, u32 name_idx, u32 this_offset , u32 length)
{
    struct ST_PART_TBL * part = NULL;
    u32 flash_addr;
    u32 image_type;

    TRACE_NORMAL(("ptable_dload_read: data_buf=0x%x, name_idx=%d, \
this_offset=0x%x, length=0x%x \r\n",data_buf, name_idx, this_offset, length));

    if(NULL == data_buf)
    {
        TRACE_ERROR(("ptable_dload_read, error :NULL == data_buf\n"));
        goto ERRO;
    }
	
    part = ptable_find_by_type(name_idx, ptable_get_ram_data());

    if(NULL == part)
    {
        TRACE_ERROR(("ptable_dload_read,name_idx(0x%x) not find\n",name_idx));
        goto ERRO;
    }
    
    flash_addr = part->offset + this_offset;
    image_type = (part->property & PTABLE_IMAGE_TYPE_MASK);


    if(NAND_OK != ptable_read_by_type(flash_addr, data_buf, image_type,  length, NULL))
    {
        TRACE_ERROR(("ptable_dload_read,ptable_write_by_type return error!\n",name_idx));
        goto ERRO;
    }
        
    return NAND_OK;

ERRO:    

    return NAND_ERROR;
    
}

/******************************************************************************************
 * FUNC NAME:  
 * @ptable_yaffs_mount() - external API:  
 *                           
 * PARAMETER: 
 * @none            
 *
 * RETURN:
 *    operation result
 *
 * DESCRIPTION:
 *    mount yaffs file system according to flash partition table
 *
 * CALL FUNC:
 *****************************************************************************************/
void ptable_yaffs_mount(void)
{
    struct ST_PART_TBL* part = NULL;
    int number = 0;
    char * devname = NULL;

    part = ptable_get_ram_data();
    
    while(0 != strcmp(PTABLE_END_STR, part->name))
    {
        if(
#ifdef __VXWORKS__           
            (part->property & MOUNT_VXWORKS) 
#endif
#ifdef BSP_IMAGE_BOOTROM
         ||(part->property & MOUNT_BOOTROM)
#endif
#if (FEATURE_ON == FEATURE_MANUFACTURE_LOG)
         ||(part->property & MOUNT_FACTORY)
#endif
        )
        {
        #if (FEATURE_ON == FEATURE_MANUFACTURE_LOG)
            if (part->property & MOUNT_FACTORY)
            {
                devname = MANUFACTURE_ROOT_PATH;
            }
            else
        #endif
            {
                devname = part->name;
            }

            TRACE_WARNING(("ptable_yaffs_mount: %s ...",devname ));
			if(part->capacity)
			{
            	if(YAFFSOK == yaffsDevCreate(number, part->offset, (part->offset + part->capacity), devname))
            	{
                	TRACE_WARNING(("OK.\n\n",devname ));
            	}
            	else
            	{
                	TRACE_WARNING(("FAIL!*************\n\n",devname));
            	}
            	number++;
			}
			else
			{
				TRACE_WARNING(("CANCEL!*************\n\n",devname));
			}
        }
        part++;
    }
}

/******************************************************************************************
 * FUNC NAME:  
 * @ptable_yaffs_mount_byname() - external API:  
 *                           
 * PARAMETER: 
 * @none            
 *
 * RETURN:
 *    operation result
 *
 * DESCRIPTION:
 *    mount yaffs file system according to flash partition table
 *
 * CALL FUNC:
 *****************************************************************************************/
void ptable_yaffs_mount_byname(char *name)
{
    struct ST_PART_TBL* part = NULL;
    int number = 0;
    int ret = 0;

    part = ptable_get_ram_data();
    
    while(0 != strcmp(PTABLE_END_STR, part->name))
    {
        if(
#ifdef __VXWORKS__           
            (part->property & MOUNT_VXWORKS) 
#endif
#ifdef BSP_IMAGE_BOOTROM
         ||(part->property & MOUNT_BOOTROM)
#endif
#ifdef BSP_IMAGE_FACTORY
         ||(part->property & MOUNT_FACTORY)
#endif
        )
        {                        
            if(part->capacity)
            {
                if(!strcmp(name, part->name))
                {
                    TRACE_WARNING(("ptable_yaffs_mount_byname: %s ...",part->name));
                    
                    ret = yaffs_mount(number,name);
                    if(YAFFSOK == ret)
                    {
                        TRACE_WARNING(("OK.\n\n",part->name));
                    }
                    else
                    {
                        TRACE_WARNING(("FAIL!*************\n\n",part->name));
                    }
                }                
				number++;
            }
        }
        part++;
    }
}

/******************************************************************************************
 * FUNC NAME:  
 * @ptable_yaffs_unmount_byname() - external API:  
 *                           
 * PARAMETER: 
 * @none            
 *
 * RETURN:
 *    operation result
 *
 * DESCRIPTION:
 *    mount yaffs file system according to flash partition table
 *
 * CALL FUNC:
 *****************************************************************************************/
void ptable_yaffs_unmount_byname(char *name)
{
    struct ST_PART_TBL* part = NULL;
    int number = 0;
    int ret = 0;

    part = ptable_get_ram_data();
    
    while(0 != strcmp(PTABLE_END_STR, part->name))
    {
        if(
#ifdef __VXWORKS__           
            (part->property & MOUNT_VXWORKS) 
#endif
#ifdef BSP_IMAGE_BOOTROM
         ||(part->property & MOUNT_BOOTROM)
#endif
#ifdef BSP_IMAGE_FACTORY
         ||(part->property & MOUNT_FACTORY)
#endif
        )
        {                        
            if(part->capacity)
            {
                if(!strcmp(name, part->name))
                {
                    TRACE_WARNING(("ptable_yaffs_mount_byname: %s ...",part->name));
                    
                    ret = yaffs_unmount(number,name);
                    if(YAFFSOK == ret)
                    {
                        TRACE_WARNING(("OK.\n\n",part->name));
                    }
                    else
                    {
                        TRACE_WARNING(("FAIL!*************\n\n",part->name));
                    }
                }                
				number++;
            }
        }
        part++;
    }
}

#ifdef __cplusplus
}
#endif




