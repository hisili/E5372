/*******************************************************************************
* Copyright (C), 2008-2011, HUAWEI Tech. Co., Ltd.
*
* Module name: Hisilicon Nand Flash Controller Dirver
*
* Description: the Hisilicon Nand Flash Controller physical and logical driver
*
* Version: v1.0
*
* Filename:    nand_balong.c
* Description: nand controller operations in dependence on  hardware
*              
*
* Function List:
*
* History:
* date:2011-07-27
* question number:
* modify reasion:         create
*******************************************************************************/
/* Description: nand controller operations in dependence on  hardware,
*              this source file is only for some platforms but not for Linux
*              platform.
*
*                   |                           Layer: platform(Vxworks/ADS/bootloater/fastboot)                   
*              nand_balong.c
*                   |
*                   |                           Layer: porting 
*              nandc_nand.c
*                   |
*                   |
*        ------------------------
*          |                  | 
*          |(init)            |(operation)
*       nandc_host.c       nandc_ctrl.c               Layer: controller   
*          |           ----------------------- 
*      nandc_native.c         |
*                      nandc_vxxx.c(nandc_v400.c)       Layer: hardware    
******************************************************************************
*    Copyright (c) 2009-2011 by  Hisilicon Tech. Co., Ltd.
* 
*    All rights reserved.
* 
******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

#include "nandc_balong.h"

struct nand_bus  nand_handle = {0, 0, 0, 0, 0, 0, 0};

void PWRCTRL_NAND_SoftClkDis(void)
{
#if NANDC_COMPILER(NANDC_LOW_POWER)
#if ((defined(PRODUCT_CFG_IMAGE_TYPE_VXWORKS) && defined(__VXWORKS__)) \
        || defined(__KERNEL__))
	BSP_PWRCTRL_SoftClkDis(PWRCTRL_MODU_NANDC);
#else
	BSP_REG_WRITE(INTEGRATOR_SC_BASE, 0x4, 1 << 17);
#endif
#endif
}

void PWRCTRL_NAND_SoftClkEn(void)
{
#if NANDC_COMPILER(NANDC_LOW_POWER)
#if ((defined(PRODUCT_CFG_IMAGE_TYPE_VXWORKS) && defined(__VXWORKS__)) \
        || defined(__KERNEL__))
	BSP_PWRCTRL_SoftClkEn(PWRCTRL_MODU_NANDC);
#else
	BSP_REG_WRITE(INTEGRATOR_SC_BASE, 0x0, 1 << 17);
#endif
#endif
}

/******************************************************************
*  Function:  PWRCTRL_NAND_LowPowerEnter                                 *
*  Description:  进入低功耗                                 * 
*  Calls:                                                         *
*  Called By:                                                     *
*  Table Accessed: NONE                                           *
*  Table Updated: NONE                                            *
*  Input:    none                                     *
*  Output:   OK or ERROR                                          *                                
*  Return:                                                        *
*         BSP_OK    - successful                                 *
*         BSP_ERROR - unsuccessful                               *
*                                                                 *
*******************************************************************/
int PWRCTRL_NAND_LowPowerEnter(void)
{
#if NANDC_COMPILER(NANDC_LOW_POWER)
    int result = NAND_ERROR;
    
    PWRCTRL_NAND_SoftClkDis();
    result = BSP_PWRCTRL_SleepVoteUnLock(PWRCTRL_SLEEP_FLASH);
    return result;
#else
    return NAND_OK;
#endif
}


/******************************************************************
*  Function:  PWRCTRL_NAND_LowPowerExit                                 *
*  Description:  退出低功耗                                 * 
*  Calls:                                                         *
*  Called By:                                                     *
*  Table Accessed: NONE                                           *
*  Table Updated: NONE                                            *
*  Input:    none                                     *
*  Output:   OK or ERROR                                          *                                
*  Return:                                                        *
*         BSP_OK    - successful                                 *
*         BSP_ERROR - unsuccessful                               *
*                                                                 *
*******************************************************************/
int PWRCTRL_NAND_LowPowerExit(void)
{
#if NANDC_COMPILER(NANDC_LOW_POWER)
    int result = NAND_ERROR;

    PWRCTRL_NAND_SoftClkEn();
    result = BSP_PWRCTRL_SleepVoteLock(PWRCTRL_SLEEP_FLASH);

    return result;
#else
    return NAND_OK;
#endif
}

/**********************************************************************
 * FUNC NAME: 
 * nand_mutex_in - nand atomic operation lock in.
 *
 * PARAMETER:
 * @handle:	nand bus ram structure
 *
 * DESCRIPTION:
 * This function gets the device and locks it for exclusive access
 *
 ***********************************************************************/
u32 nand_mutex_in(struct nand_bus* handle)
{
    if(handle->sem_lock)
    {
        /*mutitask lock*/
        /*modified for lint e522 */
        (void)handle->sem_lock(handle->sem);
    }

    if(handle->ipc_wait)
    {
        /*muticore lock*/
        /*modified for lint e522 */
        (void)handle->ipc_wait(NAND_IPC_OVERTIME_MS);
    }
	
    /*退出低功耗*/
    if(NAND_OK != PWRCTRL_NAND_LowPowerExit())
    {
        return NAND_ERROR;
    }
	
    return NAND_OK;
}

/***********************************************************************
 * FUNC NAME: 
 * nand_mutex_out - nand atomic operation lock out.
 *
 * PARAMETER:
 * @handle:	nand bus ram structure
 *
 * DESCRIPTION:
 * This function deselects, releases chip lock and wakes up anyone waiting 
 * on the device.
 *
 **********************************************************************/
u32 nand_mutex_out(struct nand_bus* handle)
{
    /*进入低功耗*/
    if(NAND_OK != PWRCTRL_NAND_LowPowerEnter())
    {
        return NAND_ERROR;
    }
	
    if(handle->ipc_post)
    {
        /*mutitask unlock*/
        handle->ipc_post();
    }   

    if(handle->sem_unlock)
    {
        /*muticore unlock*/
        /*modified for lint e522 */
        (void)handle->sem_unlock(handle->sem);
    }

    return NAND_OK;
}

 /*******************************************************************************
 * FUNC NAME:  
 * nand_init() - nand driver plantform layer initialization
 *
 * PARAMETER: none
 *
 * DESCRIPTION:
 * This function initializes nand platform layer ram structure and then call low 
 * layer nand initialization route
 *
 * CALL FUNC:
 * 
 ********************************************************************************/
u32 nand_init_platform(u32 is_from_mtd)
{
    struct nand_bus* handle = NULL;
    u32 result = NAND_ERROR;
        
    handle = &nand_handle;
    
    if(NAND_INIT_MAGIC == handle->init_flag)
    {
        result = NAND_OK;
        NAND_TRACE(("!!nand module already  inited!!\n"));
        goto EXIT;
    }

    memset((void*)handle, 0x00, sizeof(nand_handle));

    /*set structure of nand opreation function pointer */
    handle->funcs  =  &nandc_nand_funcs;

#if NANDC_COMPILER(NANDC_MULTITASK)    
    /*init mutex for mutitask*/
    handle->sem_lock     = nandc_mutex_lock;
    handle->sem_unlock   = nandc_mutex_unlock;
    nandc_mutex_create(&handle->sem );

#endif /*NANDC_MULTITASK*/

#if NANDC_COMPILER(NANDC_DUALCORE)   
    /*init mutex for muticore*/
    handle->ipc_wait = nandc_ipc_wait;
    handle->ipc_post = nandc_ipc_post;
    nandc_ipc_create();
    
#endif /*NANDC_DUALCORE*/


    /*call lower nand initialization route*/
    nand_mutex_in(handle);

    if(handle->funcs->init)
    {
        if(NANDC_FALSE == is_from_mtd)
        {
            result = handle->funcs->init();
        }
        else
        {
            result = NAND_OK;
        }
    }
    else
    {
        NAND_TRACE(("ERRO!!nand module init is NULL\n"));
        result = NAND_ERROR;
    }
    
    nand_mutex_out(handle);


    if(NAND_OK == result)
    {
        handle->init_flag = NAND_INIT_MAGIC;
    }
    else
    {
        NAND_TRACE(("ERRO!!nand module init failed\n"));
    }

EXIT:
    return result;
    
}

u32 nand_init(void)
{
    return nand_init_platform(NANDC_FALSE);
}

u32 nand_balong_port_mtd(void)
{
    return nand_init_platform(NANDC_TRUE);
}
 /**
 * FUNC NAME:  
 * nand_deinit() - nand driver plantform layer de-initialization
 *
 * PARAMETER: none
 *
 * DESCRIPTION:
 * This function deinitialize nand platform layer ram structure and then call low 
 * layer nand de-initialization route
 *
 * CALL FUNC:
 * 
 ************************************************************************************/
u32 nand_deinit(void)
{
    struct nand_bus* handle = NULL;
    u32 result = NAND_ERROR;  
    
	handle = &nand_handle;

    if(NAND_INIT_MAGIC != handle->init_flag)
    {
        result = NAND_OK;
        NAND_TRACE(("!!nand module already  inited!!\n"));
        goto EXIT;
    }

    nand_mutex_in(handle);

    if(handle->funcs->deinit)
    {
        result = handle->funcs->deinit();
    }
    else
    {
        NAND_TRACE(("ERRO!!nand module deinit is NULL\n"));
        result = NAND_ERROR;
    }
    
    nand_mutex_out(handle);


    if(NAND_OK == result)
    {
        handle->init_flag = 0;
    }

#if NANDC_COMPILER(NANDC_MULTITASK)    
    nandc_mutex_delete(handle->sem );
    handle->sem = NULL;
#endif

#if NANDC_COMPILER(NANDC_DUALCORE)    
    nandc_ipc_delete();
#endif


EXIT:
    return result;
    
}

/*******************************************************************************
 * FUNC NAME:  
 * nand_run() - unibus for all nand flash operations from external calling
 *
 * PARAMETER:
 * @param:	    [input]sturcture which contains nand operation parameters.
 * @func_type:	[input]specify nand operation type
 *
 * DESCRIPTION:
 * This is the route to low layer nand driver: nandc_nand.c, 
 * this function must run after nand_init() has been called.
 * 
 * CALL FUNC:
 *
 *********************************************************************************/
u32 nand_run(struct nand_param* param, u32 func_type)
{
    struct nand_bus* handle;
    struct nand_interface * nandfuncs;
    u32 result;

    handle = &nand_handle;

    if(NAND_INIT_MAGIC != handle->init_flag)
    {
        NAND_TRACE(("error!! balong nand not inited\n"));
        return NAND_ERROR;
    }

    nandfuncs = handle->funcs;
    
    nand_mutex_in(handle);
       
    switch (func_type)
    {
        case READ_NAND:
        {
            if(nandfuncs->read)
            {
                result = nandfuncs->read(param->addr_flash, 
                                         param->addr_data,
                                         param->size_data,
                                         param->size_oob, 
                                         param->skip_length);
            }
            else
            {
                NAND_TRACE(("error!! func read_random is NULL\n"));
                result = NAND_ERROR;
            }
        }
        break;
        
#if !NANDC_COMPILER(NANDC_READ_ONLY)
        
        case READ_YAFFS:
        {
            if(nandfuncs->read_page_yaffs)
            {  
                result = nandfuncs->read_page_yaffs(param->number, 
                                                    (u8*)param->addr_data,  
                                                    (u8*)param->addr_oob,
                                                    param->size_oob);
            }
            else
            {
                NAND_TRACE(("error!! func read_page_yaffs is NULL\n"));
                result = NAND_ERROR;
            }
        }
        break;
        
        case READ_RAW:
        {
            if(nandfuncs->read_page_raw)
            {  
                result = nandfuncs->read_page_raw(param->number,
                                                    (u32)param->addr_flash, 
                                                    (u32)param->addr_data,
                                                    param->size_data);
            }
            else
            {
                NAND_TRACE(("error!! func read_page_yaffs is NULL\n"));
                result = NAND_ERROR;
            }
        }
        break;

        case QUARY_BAD:
        {
            if(nandfuncs->quary_bad)
            {  
                result = nandfuncs->quary_bad(param->number, (u32*)param->addr_data);
            }
            else
            {
                NAND_TRACE(("error!! func quary_bad is NULL\n"));
                result = NAND_ERROR;
            }
        }
        break;
        
        case ERASE_BLOCK:
        {
            if(nandfuncs->erase_by_id)
            {  
                result = nandfuncs->erase_by_id(param->number);
            }
            else
            {
                NAND_TRACE(("error!! func erase_by_id is NULL\n"));
                result = NAND_ERROR;
            }
        }
        break;
        
        case WRITE_NAND:
        {
            if(nandfuncs->write)
            {
                result = nandfuncs->write(param->addr_flash, 
                                          param->addr_data,
                                          param->size_data,
                                          param->size_oob,
                                          param->skip_length);
            }
            else
            {
                NAND_TRACE(("error!! func write flash is NULL\n"));
                result = NAND_ERROR;
            }
        }
        break;

        case WRITE_YAFFS:
        {
            if(nandfuncs->write_page_yaffs)
            {
                result = nandfuncs->write_page_yaffs(param->number,
                                                    (u8*)param->addr_data, 
                                                    (u8*)param->addr_oob,
                                                    param->size_oob);
            }
            else
            {
                NAND_TRACE(("error!! func write_page_yaffs is NULL\n"));
                result = NAND_ERROR;
            }
        }
        break;
        
        case WRITE_RAW:
        {
            if(nandfuncs->write_page_raw)
            {
                result = nandfuncs->write_page_raw(param->number,
                                                    (u32)param->addr_flash, 
                                                    (u32)param->addr_data,
                                                    param->size_data);
            }
            else
            {
                NAND_TRACE(("error!! func write_page_raw is NULL\n"));
                result = NAND_ERROR;
            }
        }
        break;

        case MARK_BAD:
        {
            if(nandfuncs->mark_bad)
            {  
                result = nandfuncs->mark_bad(param->number );
            }
            else
            {
                NAND_TRACE(("error!! func mark_bad is NULL\n"));
                result = NAND_ERROR;
            }
        }
        break;
#endif        
        default:
            NAND_TRACE(("ERRO!! nand function type:%d not defined\n",func_type));
            result = NAND_ERROR;
        break;    
    }

    nand_mutex_out(handle);

    return result;
}


/*******************************************************************************************
 * FUNC NAME:  
 * nand_read() - external API for nand data read operation
 *
 * PARAMETER:
 * @flash_addr - [input]read start address from nand.
 * @dst_data   - [input]ram address where data will be read to.
 * @read_size  - [input]size of data to be read form flash.
 * @skip_length- [output]a ram address that store bad block skip length during read operation,
 *               this parameter could be NULL if you don't care about skip lengt.
 *
 * DESCRIPTION:
 * This function read flash data like memcpy(), it means you can read flash data form any 
 * address and read any size. 
 * 
 * this route don't read oob data from nand spare area.
 * 
 * CALL FUNC:
 * @nand_run() - 
 * 
 *******************************************************************************************/
u32 nand_read(FSZ flash_addr, u32 dst_data, u32 read_size, u32* skip_length)
{
    struct nand_param param = {0, 0, 0, 0, 0, 0, 0}; 

    param.addr_flash    =   flash_addr;
    param.addr_data     =   dst_data;
    param.size_data     =   read_size;
    param.skip_length   =   skip_length;
        
    return nand_run(&param, READ_NAND);

}

/**********************************************************************************
 * FUNC NAME:  
 * nand_get_spec() - external API for get nand chip specification parmeter
 *
 * PARAMETER:
 * @spec - [output]pointer of structure contains nand chip specification parmeter
 *
 * DESCRIPTION:
 * Tish function get flash total size , page size ,spare size ,block size...etc
 *
 * CALL FUNC:
 * 
 *********************************************************************************/
u32 nand_get_spec(struct nand_spec *spec)
{
    struct nand_bus* handle;
    struct nand_interface * nandfuncs;

    handle = &nand_handle;
    if(NAND_INIT_MAGIC != handle->init_flag)
    {
        NAND_TRACE(("error!! balong nand not inited\n"));
        return NAND_ERROR;
    }

    nandfuncs = handle->funcs;

    if(nandfuncs->get_spec)
    {
        return  nandfuncs->get_spec(spec);
    }

    NAND_TRACE(("error!! get_spec not inited\n"));

    return NAND_ERROR;
}

/***********************************************************************************************
 * FUNC NAME:  
 * nand_read_oob() - external API for nand data read with oob
 *
 * PARAMETER:
 * @flash_addr - [input]read start address from nand.
 * @dst_data   - [output]ram address where data will be read to.
 * @data_size  - [input]size of data to be read form flash, this is the sum of all raw data and
                 oob data, it means read_size shouled be multiple of (page size +oob_per_page)
 * @oob_per_page - [input]oob data size per page. 
 *               
 * DESCRIPTION:
 * This function read raw data and spare data page by page, at the same time, the raw data and 
 * oob data will be write to ram buffer one by one
 * 
 * CALL FUNC:
 * @nand_run() - 
 * 
 **********************************************************************************************/
u32 nand_read_oob(FSZ flash_addr, u32 dst_data, u32 data_size, u32 oob_per_page, u32* skip_length)
{
    struct nand_param param = {0, 0, 0, 0, 0, 0, 0}; 

    param.addr_flash    =   flash_addr;
    param.addr_data     =   dst_data;
    param.size_data     =   data_size;
    param.size_oob      =   oob_per_page;
    param.skip_length   =   skip_length;

    return nand_run(&param, READ_NAND);

}

/********************************************************************************************
 * FUNC NAME:  
 * nand_yaffs_data_size() - external API for yaffs's nand data write 
 *
 * PARAMETER:
 * @size_with_tags - [input]  yaffs文件系统镜像数据大小(不带spare区的tag信息)
 *          
 * DESCRIPTION:
 * 
 * RETURN:  yaffs文件系统镜像数据大小(带spare区的tag信息)
 *
 * CALL FUNC:
 * 
 ********************************************************************************************/
u32 nand_yaffs_write_size(u32 size_without_tags)
{
    struct nand_spec spec;
    u32 total_page_size;
    u32 ulmods = 0;
    u32 uldivs = 0;
 
    if(NAND_OK != nand_get_spec(&spec))
    {
        NAND_TRACE(("nand_yaffs_write_size: nand_get_spec failed\n"));
        return NAND_ERROR;
    }
    
    total_page_size = (spec.pagesize) + (YAFFS_BYTES_PER_SPARE);

    uldivs = bootload_div( size_without_tags, spec.pagesize, &ulmods);
        
    if(0 != ulmods)
    {
        NAND_TRACE(("nand_yaffs_write_size: size_without_tags not (spec.pagesize) aligned\n"));
        return NAND_ERROR;
    }
    
    return uldivs * total_page_size;
}


#if !NANDC_COMPILER(NANDC_READ_ONLY)
/******************************************************************************
 * FUNC NAME:  
 * nand_read_raw() - external API for nand raw data read 
 *
 * PARAMETER:
 * @src_page    - [input]the page number from which data will be read from flash.
 * @offset_data - [input]offset for read operation in this page
 * @dst_data    - [output]the ram address where the raw data will be read to.
 * @len_data    - [input]data length for read operation in this page
 *               
 * DESCRIPTION:
 * This function read raw data without ecc correction in one page, the input parameter 
 * (offset_data + len_data) should not exceed one page size.  
 *
 * CALL FUNC:
 * @nand_run() - 
 * 
 ******************************************************************************/
u32 nand_read_raw(u32 src_page, u32 offset_data, u32 dst_data, u32 len_data, u32* skip_length)
{
    struct nand_param param = {0, 0, 0, 0, 0, 0, 0}; 

    param.number        = src_page;
    param.addr_flash    = offset_data;
    param.addr_data     = dst_data;
    param.size_data     = len_data;
    param.skip_length   = skip_length;

    return nand_run(&param, READ_RAW);

}

/************************************************************************************************
 * FUNC NAME:  
 * nand_read_yaffs() - external API for data read operation to yaffs filesysem.
 *
 * PARAMETER:
 * @flash_page - [input]the page number from which data will be read from flash.
 * @dst_data   - [output] ram address where the yaffs data will be read to.
 * @dst_oob    - [output] ram address where the yaffs oob data will be read to.
 * @len_oob    - [input]  indicate the oob length in one yaffs page
 *               
 * DESCRIPTION:
 * This function read data and oob data of yaffs filesystem with flash hardware ecc correction from one page.
 * this route don't go through nand_run() for high efficiency, so before call this, nand_init()
 * must be called, otherwise unforeseeable fatal error will occur.
 * 
 * CALL FUNC:
 * @nand_mutex_in
 * @nandc_nand_yaffs_read() -
 * @nand_mutex_out
 * 
 ************************************************************************************************/
u32 nand_read_yaffs(u32 flash_page, u32 dst_data, u32 dst_oob, u32 len_oob)
{
    struct nand_bus* handle = &nand_handle;
    u32 result;
    
    nand_mutex_in(handle);

    result = nandc_nand_yaffs_read(flash_page, (u8*)dst_data, (u8*)dst_oob, len_oob);

    nand_mutex_out(handle);

    return result;
}


/**********************************************************************************
 * FUNC NAME:  
 * nand_isbad() - external API :check if nand block is bad
 *
 * PARAMETER:
 * @blockID - [input] the block number to be check.
 * @flag    - [output]ram address contain a flag which indicate whether a block is bad block.
 *               
 * DESCRIPTION:

 * CALL FUNC:
 * @nand_run() - 
 * 
 **********************************************************************************/
u32 nand_isbad(u32 blockID, u32 *flag)
{
    struct nand_param param = {0, 0, 0, 0, 0, 0, 0}; 

    param.number    =   blockID;
    param.addr_data =   (u32)flag;
        
    return nand_run(&param, QUARY_BAD);

}

/*********************************************************
 * FUNC NAME:  
 * nand_bad() - external API :mark a block as a bad block.
 *
 * PARAMETER:
 * @blockID - [input] the block number to be marked.
 *               
 * DESCRIPTION:

 * CALL FUNC:
 * @nand_run() - 
 * 
 ***********************************************************/
u32 nand_bad(u32 blockID)
{
    struct nand_param param = {0, 0, 0, 0, 0, 0, 0}; 

    param.number    =   blockID;

    return nand_run(&param, MARK_BAD);

}
/********************************************************************
 * FUNC NAME:  
 * nand_erase() - external API :erase a block by block number.
 *
 * PARAMETER:
 * @blockID - [input] the block number to be erased.
 *               
 * DESCRIPTION:

 * CALL FUNC:
 * @nand_run() - 
 * 
 ********************************************************************/
u32 nand_erase(u32 blockID)
{
    struct nand_param param = {0, 0, 0, 0, 0, 0, 0}; 

    param.number    =   blockID;

    return nand_run(&param, ERASE_BLOCK);

}

/**********************************************************************************************
 * FUNC NAME:  
 * nand_write() - external API for nand data write
 *
 * PARAMETER:
 * @flash_addr - [input]write start address to nand.
 * @src_data   - [input]ram address where stored the source data.
 * @write_size - [input]size of data that will be writen to from ram to flash
 * @skip_length- [output]a ram address that store bad block skip length during write operation,
 *               this parameter could be NULL if you don't care about skip length;
 *
 * DESCRIPTION:
 * This function write flash data like memcpy(), it means you can write ram data to any flash address and 
 * write any size.
 *
 * this route don't write oob date to nand spare area.
 * 
 * CALL FUNC:
 * @nand_run() - 
 * 
 *********************************************************************************************/
u32 nand_write(FSZ flash_addr, u32 src_data, u32 write_size, u32* skip_length)
{
    struct nand_param param = {0, 0, 0, 0, 0, 0, 0}; 

    param.addr_flash    =   flash_addr;
    param.addr_data     =   src_data;
    param.size_data     =   write_size;
    param.skip_length   =   skip_length;
    
    return nand_run(&param, WRITE_NAND);

}

/**********************************************************************************************
 * FUNC NAME:  
 * nand_write_oob() - external API for nand data write with oob 
 *
 * PARAMETER:
 * @flash_addr - [input]write start address to nand.
 * @src_data   - [input]ram address where stored the source data.
 * @data_size  - [input]size of data that will be write to flash, this is the sum of all raw data and
                 oob data, it means data_size shouled be multiple of (page size +oob_per_page)
 * @oob_per_page-[input]oob data size per page. 
 *               
 * DESCRIPTION:
 * This function write raw data and spare data from ram to nand page by page. 
 
 * CALL FUNC:
 * @nand_run() - 
 * 
 *********************************************************************************************/
 u32 nand_write_oob(FSZ flash_addr, u32 src_data, u32 data_size, u32 oob_per_page, u32* skip_length)
{
    struct nand_param param = {0, 0, 0, 0, 0, 0, 0}; 

    param.addr_flash    =   flash_addr;
    param.addr_data     =   src_data;
    param.size_data     =   data_size;
    param.size_oob      =   oob_per_page;
    param.skip_length   =   skip_length;

    return nand_run(&param, WRITE_NAND);
    
}

/*******************************************************************************************
 * FUNC NAME:  
 * nand_write_raw() - external API for nand raw data write 
 *
 * PARAMETER:
 * @dst_page    - [input]the page number of flash for write operation. 
 * @offset_data - [input]offset for write operation in this page
 * @src_data    - [output]the ram buffer address where stored source data
 * @len_data    - [input]data length to be writen to this page
 *               
 * DESCRIPTION:
 * write raw data without ecc correction to one page, the input parameter 
 * (offset_data + len_data) should not exceed one page size.  
 *
 * CALL FUNC:
 * @nand_run() - 
 * 
 ******************************************************************************************/
u32 nand_write_raw(u32 dst_page, u32 offset_data, u32 src_data, u32 len_data ,u32* skip_length)
{
    struct nand_param param = {0, 0, 0, 0, 0, 0, 0}; 

    param.number        = dst_page;
    param.addr_flash    = offset_data;
    param.addr_data     = src_data;
    param.size_data     = len_data;
    param.skip_length   =   skip_length;

    return  nand_run(&param, WRITE_RAW);
}

/********************************************************************************************
 * FUNC NAME:  
 * nand_write_yaffs() - external API for yaffs's nand data write 
 *
 * PARAMETER:
 * @flash_page - [input]  the page number of flash for write operation.
 * @src_data   - [output] ram address where stored yaffs data
 * @src_oob    - [output] ram address where stored yaffs oob
 * @len_oob    - [input]  indicate the oob length in one yaffs page(page size + oob size) 
 *               
 * DESCRIPTION:
 * This function write raw data and oob data (which are in ram)with ecc correction to one flash page.
 * this routing don't go through nand_run() for high efficiency, so before call this, nand_init()
 * must run first, otherwise unforeseeable fatal error should occur.
 * 
 * CALL FUNC:
 * @nandc_nand_yaffs_write() - 
 * @nand_mutex_in   -
 * @nand_mutex_out  -
 * 
 ********************************************************************************************/
u32 nand_write_yaffs(u32 flash_page, u32 src_data, u32 src_oob, u32 len_oob)
{
    u32 result;
    struct nand_bus* handle = &nand_handle;

    nand_mutex_in(handle);

    result = nandc_nand_yaffs_write(flash_page, (u8*)src_data, (u8*)src_oob, len_oob);

    nand_mutex_out(handle);

    return result;
}

/**
 * FUNC NAME:  
 * nand_scan_bad() - external API for yaffs's nand block scan and mark bad
 *
 * PARAMETER:
 * @start  - [input] the start address of nand flash to scan.
 * @length - [input] the length of nand flash to scan.
 * @if_erase - [input] do erase if block is not bad block
 *               
 * DESCRIPTION:
 * this function read data block by block, if failure occured ,it mark current block as bad.  
 * and then go on ,till the total length has been read.
 *
 * CALL FUNC:
 * @nand_read()       - 
 * @nand_get_spec()   -   
 * 
 */
u32 nand_scan_bad(FSZ start, FSZ length, u32 if_erase)
{
    u32 addr_block_align;  
    u32 length_block_align;
    u32 blk_id;
    u32 is_bad;
    u32 times;
    struct nand_spec spec;
    u32 block_buf ;

    if(NAND_OK != nand_get_spec(&spec))
    {
        NAND_TRACE(("nand_scan_bad: nand_get_spec failed\n"));
        return NAND_ERROR;
    }

    addr_block_align = start - start%(spec.blocksize);

    length_block_align = nandc_alige_size(start + length, spec.blocksize) - addr_block_align;

    block_buf = (u32)himalloc(spec.blocksize);
    if(NULL == (void*)block_buf)//pclint 58
    {
        NAND_TRACE(("nand_scan_bad: himalloc failed\n"));
        goto ERRO;
    }
    
    while(length_block_align > 0)
    {
        blk_id = addr_block_align/spec.blocksize;
        if(NAND_OK != nand_isbad(blk_id, &is_bad))
        {
            NAND_TRACE(("nand_scan_bad: nand_isbad failed\n"));
            goto ERRO;
        }
        if(NANDC_GOOD_BLOCK == is_bad)
        {
            times = 1;
            while((NANDC_E_READ == nand_read((FSZ)addr_block_align, block_buf, spec.blocksize, 0)) && (times))
            {
                times--;
            };
            if(0 == times)
            {
                NAND_TRACE(("nand_scan_bad: find and read error, address:0x%x\n",addr_block_align));

                nand_bad(blk_id);
            }
            else if(NANDC_TRUE == if_erase)
            {
                nand_erase(blk_id);
            }
            else
            {
            }
        }
        else
        {
            NAND_TRACE(("nand_scan_bad:find bad block: 0x%x\n",addr_block_align));
        }
        length_block_align -= spec.blocksize;
        addr_block_align   += spec.blocksize;
    }
    
    hifree((void*)block_buf);

    return NAND_OK;
ERRO:
    if(NULL != (void*)block_buf)//pclint 58
    {
        hifree((void*)block_buf);
    }
    return NAND_ERROR;
}

/********************************************************************************************
 * FUNC NAME:  
 * nand_yaffs_data_size() - external API for yaffs's nand data write 
 *
 * PARAMETER:
 * @size_with_tags - [input]  yaffs文件系统镜像数据大小(带spare区的tag信息)
 *          
 * DESCRIPTION:
 * 
 * RETURN:  yaffs文件系统镜像数据大小(不带spare区的tag信息)
 *
 * CALL FUNC:
 * 
 ********************************************************************************************/
u32 nand_yaffs_data_size(u32 size_with_tags)
{
    struct nand_spec spec;
    u32 total_page_size;
    
    if(NAND_OK != nand_get_spec(&spec))
    {
        NAND_TRACE(("nand_yaffs_data_size: nand_get_spec failed\n"));
        return NAND_ERROR;
    }
    
    total_page_size = (spec.pagesize) + (YAFFS_BYTES_PER_SPARE);
    if(0 != size_with_tags % total_page_size)
    {
        NAND_TRACE(("nand_yaffs_data_size: size_with_tags not total_page_size aligned\n"));
        return NAND_ERROR;
    }

    return (size_with_tags/total_page_size) * (spec.pagesize);
}

u32 nand_data_to_total_size(u32 data_size)
{
    struct nand_spec spec;
    u32 total_page_size;
    
    if(NAND_OK != nand_get_spec(&spec))
    {
        NAND_TRACE(("nand_data_to_total_size: nand_get_spec failed\n"));
        return NAND_ERROR;
    }

    total_page_size = (spec.pagesize) + (YAFFS_BYTES_PER_SPARE);
    if(0 != data_size % spec.pagesize)
    {
        NAND_TRACE(("nand_data_to_total_size: data_size not page_size aligned\n"));
        return NAND_ERROR;
    }
    
    return (data_size/spec.pagesize) * (total_page_size);
}
#endif

/*****************************************************************************
 * FUNC NAME:  
 * and_isinit() - external API: quary if nand module has been initialized
 *
 * PARAMETER: none
 *               
 * DESCRIPTION: 
 * 
 * CALL FUNC: none
 *******************************************************************************/
u32 nand_isinit(void)
{
    return (nand_handle.init_flag == NAND_INIT_MAGIC);
}

/**************************************************************************************************
 * FUNC NAME:  
 * nand_init_clear() - external API: this function clear .bss section of nandc module
 *
 * PARAMETER: none
 *               
 * DESCRIPTION:
 * This function is for some plantform, the .bss section is not cleared when system power(eg. trace .AXF),so I clear .bss
 * manually here to avoid critical error.
 * 
 * CALL FUNC: 
 * @nandc_minit()  -
 *
 *****************************************************************************************************/
void nand_init_clear(void)
{
    nandc_nand_host = NULL;

    memset((void*)&nand_handle, 0x00 ,sizeof(nand_handle));
#if NANDC_COMPILER(NANDC_USE_MEMPOOL)
    nandc_minit();
#endif

}

#if (defined(__VXWORKS__) && !defined(__BOOTLOADER__) && !defined(BSP_IMAGE_BOOTROM))
int nand_ecc_test_function(u32 flashAddr, u32 testLoop,u32  errNo)
{
    u32 address = 0;//= g_stNfc3Tst.testAddress;
    u32 loopTotal = 0 ;//= g_stNfc3Tst.testLoop;
    u32 loop;
    u32 * bufferR, * bufferW;
    u32 i;
    u32 ret;
    u32 passFlag = 1;
    u32 errByteTotal = 4;
    
    if(0 != flashAddr)
    {
        address = flashAddr;
    }
    if(0 != testLoop)
    {
        loopTotal = testLoop;
    }
    if(0 != errNo)
    {
        errByteTotal = errNo;
    }
    

    bufferR = (UINT32 *)himalloc(0x840*2);
    if(NULL == bufferR)
    {
        OSAL_LogMsg("[nand_ecc_test_function]himalloc error!\r\n",0,0,0,0,0,0);/*lint !e718 */
        return NAND_ERROR;
    }
    bufferW = bufferR + 0x840/sizeof(UINT32);

    /* init all write data */
    for(i = 0; i < 0x840/sizeof(UINT32); i ++)
    {
        *(bufferW + i) = 0xffffffff;
    }


    for(loop = 0;loop < loopTotal; loop++)
    {
        OSAL_LogMsg("[nand_ecc_test_function]Test Begin,Round: %d!\r\n",loop,0,0,0,0,0);/*lint !e713*/


        ret =  nand_erase(flashAddr/0x20000);
        if(NAND_OK != ret)
        {
            OSAL_LogMsg("[nand_ecc_test_function]Erase Block Error, Flash address: 0x%x!\r\n",address,0,0,0,0,0);/*lint !e713*/
            passFlag = 0;
            break;
        }

        /* first generate correct ecc data to the destination */
        ret = nand_write_yaffs(flashAddr/0x800, (u32)bufferW, 0, 0);
        if(NAND_OK != ret)
        {
            OSAL_LogMsg("[nand_ecc_test_function]page Prog with ECC Error, loop:0x%x!\r\n",loop,0,0,0,0,0);/*lint !e713*/
            passFlag = 0;
            break;        
        }

        /* read data without Ecc */
        ret = nand_read_raw(flashAddr/0x800, 0,  (u32)bufferR, 2048, 0);
        if(NAND_OK != ret)
        {
            OSAL_LogMsg("[nand_ecc_test_function]page Read without Ecc Error, loop:0x%x!\r\n",loop,0,0,0,0,0);/*lint !e713*/
            passFlag = 0;
            break;                    
        }

        /* plant some error to data..., based on input parameter:errNo */

        if( errByteTotal>=1)
        {
            *(bufferR + (0x10+0x10)/sizeof(UINT32)) = 0xffffff00;
            *(bufferR + (0x10+0x210)/sizeof(UINT32)) = 0xffffff00;
            *(bufferR + (0x10+0x420)/sizeof(UINT32)) = 0xffffff00;
            *(bufferR + (0x10+0x630)/sizeof(UINT32)) = 0xffffff00;
        }        
        if( errByteTotal>=2 )
        {
            *(bufferR + (0x20+0x10)/sizeof(UINT32)) = 0xffffff00;
            *(bufferR + (0x20+0x210)/sizeof(UINT32)) = 0xffffff00;
            *(bufferR + (0x20+0x420)/sizeof(UINT32)) = 0xffffff00;
            *(bufferR + (0x20+0x630)/sizeof(UINT32)) = 0xffffff00;
        }
        if( errByteTotal>=3 )
        {
            *(bufferR + (0x30+0x10)/sizeof(UINT32)) = 0xffffff00;
            *(bufferR + (0x30+0x210)/sizeof(UINT32)) = 0xffffff00;
            *(bufferR + (0x30+0x420)/sizeof(UINT32)) = 0xffffff00;
            *(bufferR + (0x30+0x630)/sizeof(UINT32)) = 0xffffff00;

        }
        if( errByteTotal>=4 )
        {
            *(bufferR + (0x40+0x10)/sizeof(UINT32)) = 0xffffff00;
            *(bufferR + (0x40+0x210)/sizeof(UINT32)) = 0xffffff00;
            *(bufferR + (0x40+0x420)/sizeof(UINT32)) = 0xffffff00;
            *(bufferR + (0x40+0x630)/sizeof(UINT32)) = 0xffffff00;
        }
        if( errByteTotal>=5 )
        {
            *(bufferR + (0x50+0x10)/sizeof(UINT32)) = 0xffffff00;
            *(bufferR + (0x50+0x210)/sizeof(UINT32)) = 0xffffff00;
            *(bufferR + (0x50+0x420)/sizeof(UINT32)) = 0xffffff00;
            *(bufferR + (0x50+0x630)/sizeof(UINT32)) = 0xffffff00;
        }

        /* write the error to the destination */
        ret = nand_write_raw(flashAddr/0x800, 0,  (u32)bufferR, 2048, NULL);
        if(NAND_OK != ret)
        {
            OSAL_LogMsg("[nand_ecc_test_function]page Prog withoutECC Error, loop:0x%x!\r\n",loop,0,0,0,0,0);/*lint !e713*/
            passFlag = 0;
            break;        
        }

        /* Test Ecc func now... */
        ret = nand_read_yaffs(flashAddr/0x800, (u32)bufferR, 0, 0);

        if(NAND_OK != ret)
        {
            OSAL_LogMsg("[nand_ecc_test_function]page Read With Ecc Error, loop:0x%x!\r\n",loop,0,0,0,0,0);/*lint !e713*/
	     	OSAL_LogMsg("[nand_ecc_test_function] Invalid ecc error occured!!\r\n",0,0,0,0,0,0);	
            passFlag = 0;
            break;                    
        }

        /* compare all data */

        for(i = 0; i < 0x800/sizeof(UINT32);i ++)
        {
            if(*(bufferR + i) != *(bufferW + i))
            {
                OSAL_LogMsg("[nand_ecc_test_function]Data Conflict! loop:0x%x, offset: 0x%x!\r\n",loop,i,0,0,0,0);/*lint !e713*/
                OSAL_LogMsg("[nand_ecc_test_function]Write Data:0x%x, Read Data:0x%x\r\n",*(bufferW + i),*(bufferR + i),0,0,0,0);/*lint !e713*/
                passFlag = 0;
                break;
            }
        }
        
    }

   ret = nand_erase(flashAddr/0x20000);
    if(NAND_OK != ret)
    {
        OSAL_LogMsg("[nand_ecc_test_function]Erase Block Error, Flash address: 0x%x!\r\n",address,0,0,0,0,0);/*lint !e713*/
        passFlag = 0;
    }
    
    if(passFlag)
    {
        OSAL_LogMsg("[nand_ecc_test_function]Readme:ErrNo:0x%x, <=4 is ok, when >4, invalid error should occured!\r\n",errByteTotal,0,0,0,0,0);/*lint !e713*/
        OSAL_LogMsg("[nand_ecc_test_function]Page nand_ecc_test_function Test Complete! Ecc test is OK!Total loop:0x%x!\r\n",loopTotal,0,0,0,0,0);    /*lint !e713*/
    }
    hifree(bufferR);
    return NAND_OK;
  
}
#endif

#ifdef __cplusplus
}
#endif

