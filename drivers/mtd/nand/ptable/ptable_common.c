/*******************************************************************************
* Copyright (C), 2008-2011, HUAWEI Tech. Co., Ltd.
*
* Module name: flash partition table
*
* Version: v1.0
*
* Filename:    ptable_common.c
* Description:  Balong plantform flash partition table common functions
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
/*******************************���ⵥ�޸ļ�¼********************************
����            ���ⵥ��            �޸���          �޸�����
******************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

/*#define MODULE_MAIN          NAND*/
#define SUB_MODULE              NANDC_TRACE_PTABLE

#ifdef __FASTBOOT__
typedef unsigned char       BSP_U8;
#else
#include "BSP.h"
#endif

#include "ptable_inc.h"

extern int ptable_size(void);
extern void ptable_mark_default(void);

/**********************************************************************************
 * FUNC NAME:  
 * nand_get_spec() - external API: for get flash table ram address
 *
 * PARAMETER:
 * @none
 *
 * DESCRIPTION:
 * this function get flash table address WITH TABLE HEAD, whitch description flash useage 
 *      information
 *
 * CALL FUNC:
 * 
 *********************************************************************************/
struct ST_PART_TBL * ptable_get_ram_head(void)
{

    return (struct ST_PART_TBL *)PTABLE_RAM_TABLE_ADDR;
}

/**********************************************************************************
 * FUNC NAME:  
 * nand_get_spec() - external API: for get flash table ram address
 *
 * PARAMETER:
 * @none
 *
 * DESCRIPTION:
 * this function get flash table address WITHOUT TABLE HEAD, 
 *    whitch description flash useage information.
 *
 * CALL FUNC:
 * 
 *********************************************************************************/
struct ST_PART_TBL * ptable_get_ram_data(void)
{
    return (struct ST_PART_TBL *)((u32)PTABLE_RAM_TABLE_ADDR + PTABLE_HEAD_SIZE) ;
}

/**********************************************************************************
 * FUNC NAME:  
 * ptable_ensure() - external API: make sure there is a ptable to be use
 *
 * PARAMETER:
 * @none
 *
 * DESCRIPTION:
 *    check the flash ram table, and use symbol table when ram table not exist
 *      
 *
 * CALL FUNC:
 * 
 *********************************************************************************/
u32 ptable_ensure(void)
{
    struct ST_PART_TBL * ram_parts = ptable_get_ram_head();
    u32 copy_addr;

#if (FEATURE_HANDSET_DOWNLOAD == FEATURE_ON)
#else
#if (!defined(PTABLE_AXI_USE_BSPMEMORY_H) && defined (BSP_IMAGE_BOOTROM))
    /*��ʹ�� bsp_memory.h����Ļ���Ҫʹ��.data���ķ��ű��ַ��Ϊram��ַ,*/
    /*bootrom��Ҫ���bootload.s��ȡ��0block������*/
    memcpy(ram_parts , PTABLE_BOOT_LOAD_AXI_ADDR , PTABLE_RAM_TABLE_SIZE);
    
#endif

    if(0 != strcmp(PTABLE_HEAD_STR, ram_parts->name))
#endif
    {
        copy_addr = (u32)ram_parts;
        hiout(("partition tabel: 0x"));
        BSPLOGU32(copy_addr);
        hiout((" not find, use default\r\n"));
        
        memset((void *)copy_addr, 0x00, PTABLE_HEAD_SIZE);

        /*set start string*/
        strcpy((char*)copy_addr, PTABLE_HEAD_STR);

        /*set table property*/
        memcpy((char*)copy_addr + PTABLE_HEAD_PROPERTY_OFFSET, ptable_property, PTABLE_HEAD_PROPERTY_SIZE);

        /*set bootrom version*/
        strcpy((char*)copy_addr + PTABLE_BOOTROM_VER_OFFSET, ptable_bootrom_version);

        /*set table version name*/
        strcpy((char*)copy_addr + PTABLE_NAME_OFFSET, ptable_name);

        /*mark as "Default"*/
        ptable_mark_default();
        
        /*skip head*/
        copy_addr += PTABLE_HEAD_SIZE;

        memcpy((void *)copy_addr, ptable_product, ptable_size());
    }

    return NAND_OK;
}

/**********************************************************************************
 * FUNC NAME:  
 * ptable_show() - external API: show flash partition table in shell
 *
 * PARAMETER:
 * @part -[input] if "part" is not NULL, show flash partition info in "part",else
 *                get default flash partition info and show.
 *
 * DESCRIPTION:
 *     show flash partition table in shell
 *      
 * CALL FUNC:
 * 
 *********************************************************************************/
u32 ptable_show(struct ST_PART_TBL *part)
{
    int number = 1;

    if(NULL == part)
    {
        part = ptable_get_ram_head();
    }

    BSPLOGSTR("Partition Table list(HEX):");
	/*������İ汾��ƫ��*/
    BSPLOGSTR((u8 *)((char*)part + PTABLE_BOOTROM_VER_OFFSET));    
    BSPLOGSTR((u8 *)((char*)part + PTABLE_NAME_OFFSET));   
    BSPLOGSTR("\r\n");    
    BSPLOGSTR("NO. |offset    |loadsize  |capacity  |loadaddr  |entry     |property   |count    |id         |name     |\r\n");
    BSPLOGSTR("-------------------------------------------------------------------------------------------------------\r\n");
    
    /*skip head*/
    part = (struct ST_PART_TBL *)((u32)part + PTABLE_HEAD_SIZE); 
     
    while(0 != strcmp(PTABLE_END_STR, part->name))
    {
        BSPLOGU8(number);
        BSPLOGSTR(":  ");
        BSPLOGU32(part->offset);
        BSPLOGSTR("  ,");
        BSPLOGU32(part->loadsize);
        BSPLOGSTR("  ,");
        BSPLOGU32(part->capacity);
        BSPLOGSTR("  ,");
        BSPLOGU32(part->loadaddr);
        BSPLOGSTR("  ,");
        BSPLOGU32(part->entry);
        BSPLOGSTR("  ,");
        BSPLOGU32(part->property);
        BSPLOGSTR("  ,");
		BSPLOGU32(part->count);
        BSPLOGSTR("  ,");
        BSPLOGU32(part->image);
        BSPLOGSTR("  ,");
        BSPLOGSTR(part->name);
        BSPLOGSTR("\r\n");
        part++;
        number++;
    }
    BSPLOGSTR("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\r\n");

    return 0;
}

/**********************************************************************************
 * FUNC NAME:  
 * ptable_parse_mtd_partitions() - external API: get mtd partitions from flash table
 *
 * PARAMETER:
 * @mtd_parts -[output] pointer to mtd partitions
 * @nr_parts - [output] number of mtd partitions
 *                
 *
 * DESCRIPTION:
 *     get mtd partitions from flash table
 *      
 * CALL FUNC:
 * 
 *********************************************************************************/
u32 ptable_parse_mtd_partitions(struct mtd_partition** mtd_parts, u32* nr_parts)
{
    struct ST_PART_TBL * ptable = NULL;
    struct mtd_partition *parts = NULL;
    u32 npart;
    u32 last_end = 0;    
    u32 last_id = 0;    

	if( 0 == (mtd_parts && nr_parts))
	{
		goto ERRO;
	}

    /*ensure that there's a ram table exist*/
    /*lint -e539*/
    ptable_ensure();
    /*lint +e539*/
    
    ptable = ptable_get_ram_head();

    /*show all ptable info*/
    ptable_show(ptable);

   	ptable = ptable_get_ram_data();

    /*get number of partitions*/
    npart = 0;

    while(0 != strcmp(PTABLE_END_STR, ptable->name))
    {
        if(ptable->capacity)    /* skip empty part */
        {
            npart++;
        }
        
        ptable++;
    }

    /*this memory never free */
    parts = himalloc(sizeof(struct mtd_partition) * npart);
    if(NULL == parts)
    {
        hiout(("ptable_to_mtd_partition, error malloc\r\n"));
        goto ERRO;
    }

    memset(parts, 0x00 , sizeof(struct mtd_partition) * npart);

    /*lint -e613*/
    *mtd_parts = parts; 
    /*lint +e613*/
    *nr_parts  = npart;

    /*get address again*/
    ptable = ptable_get_ram_data();

    /*��0�Ժ����¼���ʵ��ת�Ƶ�MTD�ķ�����*/
    npart = 0;

    /*form flash table to mtd partitions */
    while(0 != strcmp(PTABLE_END_STR, ptable->name))
    {
        /*not first partition (PTABLE_HEAD_STR)*/
        if((last_end != ptable->offset) && (0 != last_id)) 
        {
            hiout(("ptable check error! "));
            hiout((ptable->name));
            hiout(("\r\n"));
            goto ERRO;
        }

        #ifdef PRODUCT_CFG_SUPPORT_MOUNT_MTD
		#if NANDC_COMPILER(NANDC_USE_MTD)
        if((ptable->property & MOUNT_MTD) && (ptable->capacity))
        #else
        if(ptable->capacity)
        #endif
		#else
		if(ptable->capacity)
		#endif
        {
            parts->name   = ptable->name;
            parts->offset = ptable->offset;
            parts->size   = ptable->capacity;
            parts++;
            npart++;
        }
        
        last_end = ptable->offset + ptable->capacity;
        last_id  = ptable->image;
        
        ptable++;
    }

    *nr_parts  = npart;

    return NANDC_OK;
    
ERRO:
    if(NULL != parts)
    {
        hifree(parts);
    }
	
    return NANDC_ERROR;
}

/****************************************************************************************
 * FUNC NAME:  
 * ptable_find_by_type() - external API: find one partition address in flash partition 
 *                           table 
 *
 * PARAMETER:
 * @part_type -[input] which type of partition to find
 * @ptable    -[input] form which partition table to find
 *                
 *
 * DESCRIPTION:
 *     find partition description by type���ҵ����µķ���(��Ч)���ڼ��غͶ�ȡ
 *      
 * CALL FUNC:
 * 
 ***************************************************************************************/
struct ST_PART_TBL * ptable_find_by_type(u32 type_idx, struct ST_PART_TBL * ptable)
{
    struct ST_PART_TBL * the_newer = NULL; /*�������µķ���*/

    if(NULL == ptable)
    {
        goto EXIT;
    }

    while(0 != strcmp(PTABLE_END_STR, ptable->name))
    {
        if(type_idx == ptable->image)
        {
            if(NULL == the_newer)
            {
                if(DATA_VALID == ptable_get_validity(ptable))
                {
                    the_newer = ptable;
                }
            }
            else
            {
                if(DATA_VALID == ptable_get_validity(ptable))
                {
                    /*������µķ���*/
                    the_newer = (the_newer->count < ptable->count) ? ptable : the_newer ;
                }
            }
        }

        ptable++;
    }

EXIT:
    return the_newer;

}

/****************************************************************************************
 * FUNC NAME:  
 * ptable_find_theother_by_type() - external API: find the other partition address in flash partition 
 *                           table 
 *
 * PARAMETER:
 * @part_type -[input] which type of partition to find
 * @ptable    -[input] the partition want to find the other
 *                
 *
 * DESCRIPTION:
 *     find partition description by type���ҵ��÷���(��Ч)����һ���������ڼ��غͶ�ȡ
 *      
 * CALL FUNC:
 * 
 ***************************************************************************************/
struct ST_PART_TBL * ptable_find_theother_by_type(u32 type_idx, struct ST_PART_TBL * part)
{
    struct ST_PART_TBL *ptable = (struct ST_PART_TBL *)ptable_get_ram_data();
    struct ST_PART_TBL *the_other = NULL;

    if(NULL == part)
    {
        goto EXIT;
    }

    while(0 != strcmp(PTABLE_END_STR, ptable->name))
    {
        if(type_idx == ptable->image)
        {
            /* ����һ�� */
            if(part != ptable)
            {
                if(DATA_VALID == ptable_get_validity(ptable))
                {
                    the_other = ptable;
                    break;
                }
            }
        }

        ptable++;
    }

EXIT:
    return the_other;

}

/****************************************************************************************
 * FUNC NAME:  
 * ptable_set_validity() - external API: find if the  partition data is valid or invalid
 *
 * PARAMETER:
 * @part_type -[input] which type of partition to find
 * @ptable    -[input] form which partition table to find
 *                
 *
 * DESCRIPTION:
 *    ���÷��������ݵ���Ч��
 *      
 * CALL FUNC:
 * 
 ***************************************************************************************/
void ptable_set_validity(struct ST_PART_TBL * ptable, u32 set_valid)
{

    if(NANDC_FALSE == set_valid)
    {
        NANDC_REG_SETBIT32(&(ptable->property), PTABLE_VALIDITY_OFFSET, PTABLE_VALIDITY_MASK, DATA_INVALID);
    }
    else
    {
        NANDC_REG_SETBIT32(&(ptable->property), PTABLE_VALIDITY_OFFSET, PTABLE_VALIDITY_MASK, DATA_VALID);
    }
}

/****************************************************************************************
 * FUNC NAME:  
 * ptable_get_validity() - external API:  find if the  partition data's validity
 *                           
 *
 * PARAMETER:
 * @part_type -[input] which type of partition to find
 * @ptable    -[input] form which partition table to find
 *                
 *
 * DESCRIPTION:
 *     ��÷��������ݵ���Ч��
 *      
 * CALL FUNC:
 * 
 ***************************************************************************************/
u32 ptable_get_validity(struct ST_PART_TBL * ptable)
{
    return NANDC_REG_GETBIT32(&(ptable->property), PTABLE_VALIDITY_OFFSET, PTABLE_VALIDITY_MASK);
}

/*****************************************************************************
* �� �� ��  : ptable_check_integrality
*
* ��������  : �������ر�־�Ƿ�Ϊ0,  0��ʾ��Ч�������������أ�1��ʾ�ϴ�����ʧ�ܣ�ǿ�ƽ���bootrom
*
* �������  : BSP_VOID  
* �������  : ��
*
* �� �� ֵ  : BSP_TRUE  :���ع�����������ֹ
*            BSP_FALSE :����˳������
*
* ����˵��  : ��
*
*****************************************************************************/
u32 ptable_check_integrality(BSP_VOID)
{
    u32 ram_parts = (u32)ptable_get_ram_head();
    
    if(DATA_VALID == NANDC_REG_GETBIT32(ram_parts + PTABLE_HEAD_PROPERTY_OFFSET, PTABLE_HEAD_VALIDITY_OFFSET ,1))
    {
        return NANDC_TRUE;
    }
    else
    {
        return NANDC_FALSE;
    }
}

/*****************************************************************************
* �� �� ��  : ptable_mark_default
*
* ��������  : ���÷������Ƿ�ΪĬ��ֵ
*
* �������  : bDefault  
* �������  : ��
*
* �� �� ֵ  : BSP_VOID
*
* ����˵��  : ��
*
*****************************************************************************/
void ptable_mark_default(void)
{
    u32  ram_parts = (u32)ptable_get_ram_head();
    
    NANDC_REG_SETBIT32(ram_parts + PTABLE_HEAD_PROPERTY_OFFSET, PTABLE_HEAD_DEFAULT_OFFSET, 1, 1);
}    

#if !defined(__BOOTLOADER__) 
/*****************************************************************************
* �� �� ��  : ptable_set_integrality
*
* ��������  : ���÷�����ȫ��������
*
* �������  : BSP_VOID  
* �������  : ��
*
* �� �� ֵ  : BSP_VOID
*
* ����˵��  : ��
*
*****************************************************************************/
void ptable_set_integrality(u32 integrality)
{
    u32  ram_parts = (u32)ptable_get_ram_head();

    TRACE_ERROR(("ptable_set_integrality is %d \n", integrality));
    
    NANDC_REG_SETBIT32(ram_parts + PTABLE_HEAD_PROPERTY_OFFSET, PTABLE_HEAD_VALIDITY_OFFSET, 1, integrality);
}    

/****************************************************************************************
 * FUNC NAME:  
 * ptable_find_by_type() - external API: find one partition address in flash partition 
 *                           table 
 *
 * PARAMETER:
 * @name_to_find -[input] which name of partition to find
 * @ptable    -[input] form which partition table to find
 *                
 *
 * DESCRIPTION:
 *     find partition description by name,�ҵ����µķ���,��ʶ�������Ч��
 *      
 * CALL FUNC:
 * 
 ***************************************************************************************/
struct ST_PART_TBL * ptable_find_by_name(char* name_to_find, struct ST_PART_TBL * ptable)
{
    struct ST_PART_TBL * the_newer = NULL; /*�������µķ���*/

    if(NULL == ptable)
    {
        goto EXIT;
    }

    while(0 != strcmp(PTABLE_END_STR, ptable->name))
    {
        if(0 == strcmp(name_to_find, ptable->name))
        {
            if(NULL == the_newer)
            {
                the_newer = ptable;
            }
            else
            {
                /*������µķ���*/
                the_newer = (the_newer->count < ptable->count) ? ptable : the_newer ;
            }
        }
        
        ptable++;
    }

EXIT:
	
    return the_newer;

}

/******************************************************************************************
 * FUNC NAME:  
 * ptable_read_by_type() - external API:  
 *                           
 * PARAMETER: 
 * @flash_addr  -[input] the flash address to read from             
 * @data_buf    -[input] the ram buffer to store flash data          
 * @image_type  -[input] the flash data type           
 * @length      -[input] how much data to be read             
 *
 * DESCRIPTION:
 *     read data from nand ��ʶ�������Ч��
 *      
 * CALL FUNC:
 * 
 *****************************************************************************************/
u32 ptable_read_by_type(FSZ flash_addr, u32 data_buf, u32 image_type, u32 length, u32* skip_length)
{
    switch(image_type)
    {
        /*nand data without oob */
        case DATA_NORMAL:
        {
            if(NAND_OK != nand_read((FSZ)flash_addr, data_buf,  length, skip_length))
            {
                TRACE_ERROR(("ptable_read_by_type, error nand_read\n"));
                goto ERRO;
            }
        }
        break;

        /*nand data with oob*/
        case DATA_YAFFS:
        {
            if(NAND_OK != nand_read_oob((FSZ)flash_addr, data_buf,  length, YAFFS_BYTES_PER_SPARE, skip_length))
            {
                TRACE_ERROR(("ptable_read_by_type, error nand_read_oob\n"));
                goto ERRO;
            }
        }
        break;
        
        default:
           TRACE_ERROR(("ptable_read_by_type, error image_type:%d\n",image_type));
           goto ERRO;
    }

    return NAND_OK;
ERRO:

    return NAND_ERROR;
}

/******************************************************************************************
 * FUNC NAME:  
 * ptable_write_by_type() - external API:  
 *                           
 * PARAMETER: 
 * @flash_addr  -[input] the flash address to be write to             
 * @data_buf    -[input] the ram buffer to store data          
 * @image_type  -[input] the flash data type           
 * @length      -[input] how much data to be write             
 *
 * DESCRIPTION:
 *     write data to nand
 *      
 * CALL FUNC:
 * 
 *****************************************************************************************/
u32 ptable_write_by_type(FSZ flash_addr ,u32 data_buf, u32 image_type,  u32 length, u32 *skip_len)
{
    switch(image_type)
    {
        /*nand data without oob */
        case DATA_NORMAL:
        {
            if(NAND_OK != nand_write((FSZ)flash_addr, data_buf,  length, skip_len))
            {
                TRACE_ERROR(("ptable_write_data, error nand_write\n"));
                goto ERRO;
            }
        }
        break;

        /*nand data with oob */
        case DATA_YAFFS:
        {
            if(NAND_OK != nand_write_oob((FSZ)flash_addr, data_buf,  length, YAFFS_BYTES_PER_SPARE, skip_len))
            {
                TRACE_ERROR(("ptable_write_data, error nand_write_oob\n"));
                goto ERRO;
            }
        }
        break;
        
        default:
           TRACE_ERROR(("ptable_write_data, error image_type:%d",image_type));
           goto ERRO;
    }

    return NAND_OK;
ERRO:

    return NAND_ERROR;
}

/******************************************************************************************
 * FUNC NAME:  
 * ptable_erase_partition() - external API:  
 *                           
 * PARAMETER: 
 * @name_or_id  -[input] indicate witch parititon to be erase, can be name of partitin or   
 *                        type of partition.          
 * @is_force    -[input] set this parameter TRUE to erase bad block.                 
 *
 * DESCRIPTION:
 *     erase one partition, from partition start to partition end
 *      
 *     ע�⣬�������ֻ����Ϊ���Ե����ã��Ͻ������ڲ�����!!!!!
 * CALL FUNC:
 * 
 *****************************************************************************************/
int ptable_erase_partition( char * name_or_id ,u32 is_force)
{
    int i;
    int start_block ;
    int end_block ;
    u32 isBad = NANDC_BAD_BLOCK;
    struct nand_spec spec;
    struct ST_PART_TBL * part = NULL;
    
    part = ptable_find_by_name(name_or_id, ptable_get_ram_data());

    if(NULL == part)
    {
        part = ptable_find_by_type((u32)name_or_id, ptable_get_ram_data());
        if(NULL == part)
        {
            TRACE_ERROR(("ERROR:partiton not find .\n"));
            return NAND_ERROR;
        }
    }
    
    if(nand_get_spec(&spec))
    {
        TRACE_ERROR(("ERROR:nand_get_spec()"));
    }

    /*calculate block number*/
    start_block = (part->offset)/(spec.blocksize);
    end_block = (part->offset + part->capacity)/spec.blocksize - 1; /* Make it smaller*/
    
    for(i = start_block ;i <= end_block; i++)
    {
        nand_isbad(i, &isBad);
        
       if( isBad == NANDC_BAD_BLOCK)
       {    if(NANDC_TRUE == is_force)
            {
                hiout(("Block %d is bad, but force erase!!\n",i));
                nand_erase(i);
            }
            else
            {
                hiout(("Block %d is bad, skip erase!!\n",i));
            }
       }
       else if( isBad == NANDC_GOOD_BLOCK)
       {
            hiout(("Block %d is good,erasing block:\n",i));
            nand_erase(i);
       }

     }
    
    return NAND_OK;
}

/******************************************************************************************
 * FUNC NAME:  
 * ptable_quary_partition() - external API: scan a flash partition and find bad block
 *                           
 * PARAMETER: 
 * @name_or_id  -[input] indicate witch parititon to quary, type of partition.                          
 *
 * DESCRIPTION:
 *        ע�⣬�������ֻ����Ϊ���Ե����ã��Ͻ������ڲ�����!!!!!
 *
 * CALL FUNC:
 * 
 *****************************************************************************************/
int ptable_quary_partition(char * name_or_id)
{
    int i;
    u32 isBad = NANDC_BAD_BLOCK;
    int start_block ;
    int end_block ; 
    struct nand_spec spec;
    struct ST_PART_TBL * part = NULL;
    
    part = ptable_find_by_type((u32)name_or_id, ptable_get_ram_data());

    if(NULL == part)
    {   /*if find by type failed , try to find by name. */
        part = ptable_find_by_name(name_or_id, ptable_get_ram_data());
        if(NULL == part)
        {
            TRACE_ERROR(("ERROR:partiton not find .\n"));
            return NAND_ERROR;
        }
    }
    
    if(nand_get_spec(&spec))
    {
        TRACE_ERROR(("ERROR:nand_get_spec()"));
    }

    /*calculate block number*/
    start_block = (part->offset)/(spec.blocksize);
    end_block = (part->offset + part->capacity)/spec.blocksize - 1; // Make it smaller
      
    for(i = start_block ; i <= end_block ; i++)
    {
       nand_isbad(i, &isBad);

       if( isBad == NANDC_BAD_BLOCK)
       {
            hiout(("Block %d is bad\n",i));
       }
       else if( isBad == NANDC_GOOD_BLOCK)
       {
            hiout(("Block %d is good\n",i));
       }
       else
       {
            hiout(("Block %d is unknow\n",i));
       }
    }
    
    return NAND_OK;
}

/******************************************************************************************
 * FUNC NAME:  
 * ptable_get_range() - external API: get one start partition's start and end address 
 *                           
 * PARAMETER: 
 * @part_type  -[input] indicate witch parititon to quary.            
 * @start      -[output] store start address of quary flash partition.           
 * @end        -[output] store end address of quary flash partition .            
 *
 * DESCRIPTION:
 *      
 * CALL FUNC:
 * 
 *****************************************************************************************/
u32 ptable_get_range(u32 part_type, u32* start , u32* end)
{
    struct ST_PART_TBL * part = NULL;

    if(0 == (start || end))
    {
        return NAND_ERROR;
    }
    
    part = ptable_find_by_type(part_type, ptable_get_ram_data());

    if(NULL != part)
    {
        if(start)
        {
            *start =  part->offset;
        }
        if(end)
        {
            *end   =  part->offset + part->capacity;
        }
        return NAND_OK;
    }
    else
    {
        return NAND_ERROR;
    }
}

#endif

#ifdef __KERNEL__
/*��ȡCDROM MTD����*/
u32 ptable_get_cdromiso_mtdname(char * ptable_name, int len)
{
#define MTDBLK_BASE_NAME "/dev/block/mtdblock"
    struct ST_PART_TBL *ptable = NULL;
    u32 cnt = 0;
    char str[64] = {0};
    
    if(NULL == ptable_name)
    {
        return NAND_ERROR;
    }
    
    ptable = ptable_get_ram_data();
    while(0 != strcmp(PTABLE_END_STR, ptable->name))
    {    
        #ifdef PRODUCT_CFG_SUPPORT_MOUNT_MTD
        #if NANDC_COMPILER(NANDC_USE_MTD)
        if((ptable->property & MOUNT_MTD) && (ptable->capacity))
        #else
        if(ptable->capacity)
        #endif
        #else
        if(ptable->capacity)
        #endif
        {
            if(0 == strcmp("cdromiso", ptable->name))
            {
                sprintf(str,"%s%d",MTDBLK_BASE_NAME,cnt);
                strncpy(ptable_name, str, strlen(str));
                ptable_name[strlen(str)]=0;
                return NAND_OK;
            }

            cnt++;
        }

        ptable++;
    }

    return NAND_ERROR;
}
EXPORT_SYMBOL(ptable_get_cdromiso_mtdname);

u32 ptable_cdromiso_test(void)
{
    s32 ret;
    char str[32];
    
    ret = ptable_get_cdromiso_mtdname(str,32);
    if(ret)
    {
        printk("ptable_cdromiso_test:failed\n");
    }

    printk("ptable_cdromiso_test:str=%s\n",str);

    return ret;   
}

EXPORT_SYMBOL(ptable_cdromiso_test);
#endif

#if (defined(__VXWORKS__) && !defined(__BOOTLOADER__))
#define PTBL_CDROMISO_RESERVE_LEN   (0x80000)
#define PTBL_CDROMISO_VER_LEN       (128)
#define PTBL_CDROMISO_MAGIC_LEN     (4)
#define PTBL_CDROMISO_MAGIC         (0x55aa1235)
#endif

s32 ptable_get_cdromiso_version(char *pBuf, u32 ulLength)
{
#if (defined(__VXWORKS__) && !defined(__BOOTLOADER__))
    struct ST_PART_TBL * part = NULL;
    FSZ flash_addr;
    u32 cdromiso_magic = 0;
    u32 str_len = 0;
    s32 ret = NAND_ERROR;
    
    if(!pBuf || !ulLength)
    {
        return NAND_ERROR;
    }
    
    part = (struct ST_PART_TBL *)ptable_get_ram_data();
    
    part = ptable_find_by_type(IMAGE_CDROMISO, part);
    if(NULL == part)
    {
        return NAND_ERROR;
    }

    flash_addr = part->offset + part->capacity - PTBL_CDROMISO_RESERVE_LEN;

    /* read iso magic */
    ret = ptable_read_by_type(flash_addr + PTBL_CDROMISO_VER_LEN, (u32)&cdromiso_magic, DATA_NORMAL, sizeof(cdromiso_magic), NULL);
    if(NAND_OK != ret)
    {
        return NAND_ERROR;
    }

    if(PTBL_CDROMISO_MAGIC != cdromiso_magic)
    {
        return NAND_ERROR;
    }

    /* read iso version */
    ret = ptable_read_by_type(flash_addr, pBuf, DATA_NORMAL, ulLength, NULL);
    if(NAND_OK != ret)
    {
        memset(pBuf,0,ulLength);
        return NAND_ERROR;
    }

    str_len = strlen(pBuf);
    
    if(str_len >= ulLength)
    {
        memset(pBuf,0,ulLength);
        return NAND_ERROR;
    }    

    memset(pBuf+str_len,0,ulLength-str_len);
#endif

    return NAND_OK;
}

s32 ptable_set_cdromiso_version(char *pBuf, u32 ulLength)
{
#if (defined(__VXWORKS__) && !defined(__BOOTLOADER__))
    struct ST_PART_TBL * part = NULL;
    s32 ret = NAND_ERROR;
    FSZ flash_addr;
    u32 str_len = 0;
    u32 cdromiso_magic = PTBL_CDROMISO_MAGIC;
    u8 buf[PTBL_CDROMISO_VER_LEN + PTBL_CDROMISO_MAGIC_LEN + 1] = {0};
        
    if(!pBuf || !ulLength)
    {
        return NAND_ERROR;
    }
    
    part = (struct ST_PART_TBL *)ptable_get_ram_data();
    
    part = ptable_find_by_type(IMAGE_CDROMISO, part);
    if(NULL == part)
    {
        return NAND_ERROR;
    }

    str_len = strlen(pBuf);
    if((str_len >= ulLength) || (str_len >= PTBL_CDROMISO_VER_LEN))
    {
        return NAND_ERROR;
    }

    flash_addr = part->offset + part->capacity - PTBL_CDROMISO_RESERVE_LEN;

    memset(buf,0,PTBL_CDROMISO_VER_LEN + PTBL_CDROMISO_MAGIC_LEN + 1);
    memcpy(buf,pBuf,str_len);
    memcpy(buf+PTBL_CDROMISO_VER_LEN,&cdromiso_magic,PTBL_CDROMISO_MAGIC_LEN);

    /* set iso version */
    ret = ptable_write_by_type(flash_addr, buf, DATA_NORMAL, PTBL_CDROMISO_VER_LEN + PTBL_CDROMISO_MAGIC_LEN, 0);
    if(NAND_OK != ret)
    {
        return NAND_ERROR;
    }
        
#endif

    return NAND_OK;
}


#ifdef __cplusplus
}
#endif




