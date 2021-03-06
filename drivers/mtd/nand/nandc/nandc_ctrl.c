/*******************************************************************************
* Copyright (C), 2008-2011, HUAWEI Tech. Co., Ltd.
*
* Module name: Hisilicon Nand Flash Controller Dirver
*
* Description: The Hisilicon Nand Flash Controller physical and logical driver
*
* Version: v1.0
*
* Filename:    nandc_ctrl.c
* Description: nand controller operations in dependence on  hardware
*
* Function List:
*
* History:
* 1.date:2011-07-27
* question number:
* modify reasion:        create
*******************************************************************************/
/******************************************************************************
*    Copyright (c) 2009-2011 by  Hisilicon Tech. Co., Ltd.
*    
*    All rights reserved.
*
******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
/*lint -e506 */
#include "nandc_inc.h"

#define NFCDBGLVL(LVL)       (NANDC_TRACE_CTRL|NANDC_TRACE_##LVL)

/********************************************************************************************
 * FUNC NAME:  
 * nandc_ctrl_divs_shift() - shift operation for nand flash chip page size to nand controller buffer size.
 *
 * PARAMETER:
 * @pagesize - [input] page size for nand flash chip. 
 *               
 * DESCRIPTION:
 * Nand controller buffer for most Hisi chips is between 2k~3k bytes,but for multifarious nand 
 * flash chips,they are (2048+64)bytes,(4096+128)bytes,(8192+256)bytes ect page size.so when 
 * we read from or write to a 4k page,we need 2 times operation (read/write)between nand controller
 * buffer and nand flash chip,be the same as,when we read from or write to a 8k page,we need 4 times 
 * operation (read/write)between nand controller buffer and nand flash chip.
 * 
 * CALL FUNC:
 * nandc_ctrl_get_param() -
 * 
 ********************************************************************************************/
__inline u32 nandc_ctrl_divs_shift(u32 pagesize)
{
    switch(pagesize)
    {
        case nandc_size_2k:
        return 0;
        
        case nandc_size_4k:
        return 1;  
        
        case nandc_size_8k:
        return 2;
        
        default:
        return 0;
    }
}

/********************************************************************************************
 * FUNC NAME:  
 * nandc_ctrl_get_param() - 
 *
 * PARAMETER:
 * @host - [input]
 * @totalsize - [input]data size for one read or write operation to one page of nand flash chip.
 *              It is oob data size (64,128,256bytes.etc)when read or write oob data only.otherways it is 
 *              nand flash page size and spare area size(2048+64,4096+128,8192+256bytes.etc).
 * @ecctype - [input]nand controller ecc type (eg:1 bit,4 symbol,16 bits etc)
 * @eccselect - [input]nand controller ecc on-off which include raw data ecc on-off and oob data ecc on-off.               
 * @onesize - [input]data size for one read or write operation to nand controller buffer.
 *              It is oob data size (64,128,256bytes.etc)when read or write oob data only.otherways it is 
 *              should no more than the size of nand controller buffer(2k~3k).
 *            
 * DESCRIPTION:
 * This function establish default parameters for latter read and write operation accroding to the information 
 * of nand flash chip which is achieved by nandc_native_get_id().
 * 
 * CALL FUNC:
 * nandc_ctrl_create_snapshot() -
 * 
 ********************************************************************************************/
__inline u32 nandc_ctrl_get_param(struct nandc_host *host, 

                                                u32 *totalsize,
                                                u32 *ecctype,
                                                u32 *eccselect,
                                                u32 *onesize
                                                )
{
    u32  offset_in_page , total_size, total_oob, shifttimes,one_size;
    struct nandc_nand       *nand   =   host->nandchip;
    struct nandc_spec       *spec   =   &nand->spec;
    struct nandc_bit_cmd    *ctrlcmd=   &host->bitcmd;

    total_oob   = host->curpart->ecclayout->oobavail;
    offset_in_page  = ((u32)host->addr_physics[0]) & 0x0000FFFF;
    
    if(nandc_bus_16 == nand->busset)
    {
        offset_in_page <<=  1;
    }

    shifttimes   =  nandc_ctrl_divs_shift(spec->pagesize);

    if(host->options & NANDC_OPTION_ECC_FORBID)
    {
         total_size = host->length;

          /* read size is less than  512 or between page size and whole page size*/ 
         if(!((total_size <= NANDC_RANDOM_ACCESS_THRESHOLD) ||
            ((0 == offset_in_page) && (total_size <= spec->pagesize + spec->sparesize))))
        {
            /*lint -e778*/
            NANDC_TRACE(NFCDBGLVL(ERRO), ("mode0 not support yet! length:0x%x, offset in page: 0x%x",total_size, offset_in_page));
            /*lint +e778*/
			NANDC_DO_ASSERT(0, HICHAR_NULL, 0);
            /*TODO:  read size between 512 and page size  or read address not align to page size*/
            /*else*/
            goto ERRO;

        }
        one_size    =   (spec->pagesize + spec->sparesize) >> shifttimes;
        /*one_size    =   (total_size <= one_size) ? total_size:one_size;*/
        *ecctype    =   0 ;/*nandc_ecc_none*/
        *eccselect  =   ctrlcmd->ecc_none; /* nandc_noecc_all */
    }
    else
    {
        /* ecc not forbided, use ecc defined in partition table*/
        *ecctype    =   nand->ecctype;
         NANDC_REJECT_NULL(!offset_in_page);
         
        if(host->options & NANDC_OPTION_OOB_ONLY)
        {
            total_size  =   total_oob;
            one_size    =   total_size;
            *eccselect  =   ctrlcmd->ecc_oob; /* nandc_ecc_oob */

        }
        else
        {
            total_size  =   spec->pagesize + total_oob;
            one_size    =   total_size >> shifttimes;
            *eccselect  =   ctrlcmd->ecc_all;/* nandc_ecc_all */

        }

    }
    
    *totalsize = total_size;  
    *onesize = one_size;     

    return NANDC_OK;

ERRO:

    return NANDC_ERROR;
}

/********************************************************************************************
 * FUNC NAME:  
 * nandc_ctrl_reg_to_list() - creat a list which stores the registers' address and value
 *
 * PARAMETER:
 * @host - [input]the main structure,for future usage.
 * @regaddr - [input]offset from nand controller registers' base address.  
 * @listreg - [input]list which stores the registers' address and value.  
 *               
 * DESCRIPTION:
 * This function creat a list which stores the registers' address and value.
 * 
 * CALL FUNC:
 * nandc_ctrl_write_snapshot() - Macro
 * 
 ********************************************************************************************/
__inline  void nandc_ctrl_reg_to_list(struct nandc_host *host,u32 regaddr, struct nandc_reg_list** listreg)
{    
    HI_NOTUSED(host);

    /* when add the first register member, *listreg is NULL,malloc space for it.*/
    if(0 == *listreg)
    {
        *listreg = (struct nandc_reg_list*)himalloc(sizeof(struct nandc_reg_list));
        NANDC_REJECT_NULL(*listreg);
        (*listreg)->address = regaddr;
        (*listreg)->value   = NANDC_REG_READ32(regaddr);
        (*listreg)->p_next  = NANDC_NULL;
        return ;
    }
    else
    {
        struct nandc_reg_list* list = *listreg;
        struct nandc_reg_list* last;

        do
        {   /* modify and save value in register whose address is regaddr*/
            if(regaddr == list->address)
            {
                list->value = NANDC_REG_READ32(regaddr);
                return;
            }
            
            last = list;
            list = list->p_next;
        }while(NANDC_NULL != list);

        /* add a new member to list.*/
        last->p_next = (struct nandc_reg_list*)himalloc(sizeof(struct nandc_reg_list));
        NANDC_REJECT_NULL(last->p_next);
        last->p_next->address = regaddr;
        last->p_next->value   = NANDC_REG_READ32(regaddr);
        last->p_next->p_next  = NANDC_NULL; 
    }
ERRO:
    return;
}

 

#define  nandc_ctrl_write_snapshot(bitfunc,value, snapshot)  \
{                                                                                   \
    if((bitfunc).bitset)                                                            \
    {                                                                               \
        NANDC_SET_REG_BITS(bitfunc, value);                                         \
        nandc_ctrl_reg_to_list(host,(u32)host->regbase + ((bitfunc).reg_offset), &((*snapshot)->listreg));\
    }                                                                               \
}

/***********************************************************************************************************
 * FUNC NAME:  
 * nandc_ctrl_create_snapshot() - create a list to store nand controller registers' address and value.
 *
 * PARAMETER:
 * @host - [input]the main structure.
 * @snapshot - [input]  
 * @totalsize - [input]data length for one page operation.(2k,4k,8k)  
 * @onesize - [input]data length for one time operation.because Hisi nand controller buffer is 
 *                   2k~3k,when nand flash chip's page is 4k,we need 2 times operation.as the
 *                   same,when nand flash chip's page is 8k,we need 3 times operation.
 * @eccselect - [input]ecc calibration and correction is open or forbidden  
 *                     u16 ecc_oob :  extend data area's ecc calibration and correction is open.
 *                     u16 ecc_all :  both data area's and extend data area's ecc calibration and correction is open.
 *                     u16 ecc_none : both data area's and extend data area's ecc calibration and correction is forbidden. 
 *               
 * DESCRIPTION:
 * This function creates a list to store nand controller registers' address and value.
 * 
 * CALL FUNC:
 * nandc_ctrl_snapshot() -
 * 
 ***********************************************************************************************************/
static __inline u32 nandc_ctrl_create_snapshot(struct nandc_host *host,
                                                       struct nandc_config_snapshot **snapshot,
                                                       u32 *totalsize, 
                                                       u32 *onesize,
                                                       u32 *eccselect)
{
    register struct nandc_bit_cluster*   cluster;
    register struct nandc_bit_cmd*       ctrlcmd;
    struct nandc_nand           *nand = host->nandchip;
    struct nandc_spec           *spec = &nand->spec;
    u32  ecctype;
    
    if(NANDC_OK != nandc_ctrl_get_param(host, totalsize, &ecctype, eccselect, onesize))
    {
        goto ERRO;
    }

    cluster = host->normal; /* initialized "nandc_bit_cluster_normal_template" */
    ctrlcmd = &host->bitcmd;

    *snapshot = (struct nandc_config_snapshot*)himalloc(sizeof(struct nandc_config_snapshot));
    NANDC_REJECT_NULL(*snapshot);
    memset((void*)*snapshot, 0x00 ,sizeof(struct nandc_config_snapshot));

    if(NAND_CMD_READSTART == host->command)
    {
        nandc_ctrl_write_snapshot(cluster->cmd, NAND_CMD_READ0|(NAND_CMD_READSTART<<8)|(NAND_CMD_NOP<<16),snapshot);
    }
    else
    {
        nandc_ctrl_write_snapshot(cluster->cmd, NAND_CMD_SEQIN|(NAND_CMD_PAGEPROG<<8)|(NAND_CMD_STATUS<<16),snapshot);
    }
    nandc_ctrl_write_snapshot(cluster->bus_width,    nand->busset,       snapshot );
    nandc_ctrl_write_snapshot(cluster->page_size,    nand->pageset,      snapshot );
    nandc_ctrl_write_snapshot(cluster->ecc_type,     ecctype,            snapshot );
    nandc_ctrl_write_snapshot(cluster->ecc_select,   *eccselect,         snapshot );

    NANDC_WRITE_REG_BITS     (cluster->addr_cycle,   spec->addrcycle              );

    nandc_ctrl_write_snapshot(cluster->addr_cycle,   spec->addrcycle,    snapshot );
    nandc_ctrl_write_snapshot(cluster->chip_select,  host->chipselect,   snapshot );
    nandc_ctrl_write_snapshot(cluster->int_enable,   ctrlcmd->intdis_all,snapshot );
    nandc_ctrl_write_snapshot(cluster->data_num,     *onesize,           snapshot );

    (*snapshot)->totalsize = *totalsize;
    (*snapshot)->onesize   = *onesize;
    (*snapshot)->eccselect = *eccselect;

    return NANDC_OK;
ERRO:
    return NANDC_ERROR;
}

/********************************************************************************************
 * FUNC NAME:  
 * nandc_ctrl_recur_snapshot() - resume nand controller registers' value through "nandc_reg_list"
 *
 * PARAMETER:
 * @host - [input]the main structure.for future usage.
 * @op_config - [input]the list which stores nand controller registers' address and value.   
 * 
 *               
 * DESCRIPTION:
 * This function resumes nand controller registers' value through "nandc_reg_list".
 * 
 * CALL FUNC:
 * nandc_ctrl_snapshot() -
 * 
 ********************************************************************************************/
static __inline void nandc_ctrl_recur_snapshot(struct nandc_host *host, struct nandc_reg_list* op_config)
{
    HI_NOTUSED(host);

    while(NANDC_NULL != op_config)
    {
        NANDC_REG_WRITE32(op_config->address, op_config->value);
        op_config = op_config->p_next;
    }
}
/**************************************************************************************************************************
 * FUNC NAME:  
 * nandc_ctrl_snapshot() - 
 *
 * PARAMETER:
 * @host - [input]the main structure.
 * @snapshot - [input]the list which stores nand controller registers' address and value.   
 * @totalsize - [input]data length for one page operation.(2k,4k,8k)  
 * @onesize - [input]data length for one time operation.because Hisi nand controller buffer is 
 *                   2k~3k,when nand flash chip's page is 4k,we need 2 times operation.as the
 *                   same,when nand flash chip's page is 8k,we need 3 times operation.
 * @eccselect - [input]ecc calibration and correction is open or forbidden  
 *                     u16 ecc_oob :  extend data area's ecc calibration and correction is open.
 *                     u16 ecc_all :  both data area's and extend data area's ecc calibration and correction is open.
 *                     u16 ecc_none : both data area's and extend data area's ecc calibration and correction is forbidden. 
 *
 * DESCRIPTION:
 * This function creates the list which stores nand controller registers' address and value,otherways
 * it resumes nand controller registers' value according to the list.
 * 
 * CALL FUNC:
 * nandc_ctrl_read_normal() -
 * nandc_ctrl_program_normal() -
 * 
 *************************************************************************************************************************/
static __inline void nandc_ctrl_snapshot(struct nandc_host *host, struct nandc_snapshot *snapshot,
                                                u32 *total_size, u32 *one_size, u32* eccselect)
{
    register struct nandc_config_snapshot      *config = NULL;
    struct nandc_bit_cluster*   cluster = host->normal;
    u32 onesize = 0,  totalsize=0; /*pc-lint 644*/

    if(0 == host->options)
    {        
        if(NANDC_NULL != (u32)snapshot->config_ecc)
        {
            config = snapshot->config_ecc;
            nandc_ctrl_recur_snapshot(host, config->listreg);
            totalsize  = config->totalsize;
            onesize    = config->onesize;
            *eccselect = config->eccselect;
        }
        else
        {
            nandc_ctrl_create_snapshot(host, &snapshot->config_ecc, &totalsize, &onesize, eccselect);
        }
    }
    else if(host->options & NANDC_OPTION_ECC_FORBID)
    {
        if(NANDC_NULL != (u32)snapshot->config_raw)
        {
            config = snapshot->config_raw;
            nandc_ctrl_recur_snapshot(host, config->listreg);
            totalsize  = host->length;
            onesize    = config->onesize;
            *eccselect = config->eccselect;
        }
        else
        {
            nandc_ctrl_create_snapshot(host,&snapshot->config_raw, &totalsize, &onesize, eccselect);
        }
        
        onesize    = ((totalsize <= onesize) ? totalsize : onesize);
        
        NANDC_SET_REG_BITS(cluster->data_num,   onesize );
    }
    else if(host->options & NANDC_OPTION_OOB_ONLY)
    {
        /* TODO:*/
    }

    *total_size = totalsize;
    *one_size   = onesize;
    
    return ;
}

/********************************************************************************************
 * FUNC NAME:  
 * nandc_ctrl_set_cmd() - write cmd1+cmd2+cmd3 to nand controller's command register.
 *
 * PARAMETER:
 * @host - [input]through it to get host->normal.
 * @cmdval1 - [input]command 1
 * @cmdval2 - [input]command 2
 * @cmdval3 - [input]command 3  
 *   
 * DESCRIPTION:
 * This function writes cmd1+cmd2+cmd3 to nand controller's command register.
 * 
 * CALL FUNC:
 * nandc_ctrl_reset() -
 * nandc_ctrl_read_id() -
 * nandc_ctrl_earse_block() -
 * 
 ********************************************************************************************/
__inline void nandc_ctrl_set_cmd(struct nandc_host *host, u32 cmdval1, u32 cmdval2, u32 cmdval3)
{
    struct nandc_bit_cluster*  cluster;
    
    cluster = host->normal;   
    NANDC_SET_REG_BITS (cluster->cmd,     cmdval1|(cmdval2<<8)|(cmdval3<<16) );

}

/********************************************************************************************
 * FUNC NAME:  
 * nandc_ctrl_is_fresh_page() - to resolve a bug for v200 nand controller.
 *
 * PARAMETER:
 * @host - [input]the main structure.
 *   
 *
 * DESCRIPTION:
 * For v200 nand controller,there is a bug:when a page is erased,reading operation to this page with
 * ecc calibration and correction open will report ecc error.so it needs to call this function to 
 * insure whether it is a real ecc error.
 * 
 * CALL FUNC:
 * nandc_ctrl_read_result() -
 * 
 ********************************************************************************************/
__inline u32 nandc_ctrl_is_fresh_page(struct nandc_host *host)
{
    u32* pbuf  = (u32*)host->bufbase;
    u32 length = (host->nandchip->spec.pagesize)/sizeof(u32);
    
    while(length > 0)
    {
        if(0xffffffff != *pbuf)
        {
            return NANDC_FALSE;
        }

        pbuf++;
        length--;
    }
    
    NANDC_TRACE(NFCDBGLVL(NORMAL), ("nandc_ctrl_is_fresh_page: OK!\n"));

    return NANDC_TRUE;

}

/********************************************************************************************
 * FUNC NAME:  
 * nandc_ctrl_register_wait() - query for nand flash chip status.
 *
 * PARAMETER:
 * @host - [input]the main structure.
 * @status_bit - [input]the data structure which contains register offset,bit offset etc.
 * @value - [input]the expected value of the queried register.
 * @timeout_ms - [input]the times of repeated reading operation to corresponding register
 *   
 * DESCRIPTION:
 * This function queries specifically register for nand flash chip status.
 * 
 * CALL FUNC:
 * most operation interface.
 * 
 ********************************************************************************************/
__inline u32 nandc_ctrl_register_wait(struct nandc_host *host, struct nandc_bit_reg* status_bit, u32 value, u32 timeout_ms)
{
    u32 count =   NANDC_MS_TO_COUNT(timeout_ms);

#ifdef WIN32
    count = 1;
#endif

    do
    {
        if(value == NANDC_REG_GETBIT32((u32)host->regbase + status_bit->reg_offset, 
                                                            status_bit->bitset->offset ,
                                                            status_bit->bitset->length))
        {
            return NANDC_OK;
        }
        count--;
    }
    while(count > 0);

#ifdef WIN32
    return NANDC_OK;
#else

    NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_ctrl_register_wait,timeout! \n"));/*lint !e778*/
    return NANDC_E_TIMEOUT;
#endif
}

/********************************************************************************************
 * FUNC NAME:  
 * nandc_ctrl_read_result() - query result for reading operation.
 *
 * PARAMETER:
 * @host - [input]the main structure.
 *
 * DESCRIPTION:
 * This function queries result for reading operation.
 * 
 * CALL FUNC:
 * nandc_ctrl_read_normal() -
 *
 ********************************************************************************************/
__inline u32 nandc_ctrl_read_result(struct nandc_host *host)
{
    struct nandc_bit_cluster*  cluster = NANDC_NULL;
    u32 errorcode = NANDC_ERROR; 
    cluster = host->normal;

#ifdef NANDC_CTRL_DEBUG
    nandc_ctrl_dump_reg(host, "nandc read result");
#endif

    errorcode = nandc_ctrl_register_wait(host, &cluster->op_stauts, nandc_status_op_done, NANDC_RD_RESULT_TIMEOUT);
    if(NANDC_OK != errorcode)
    {
        NANDC_TRACE(NFCDBGLVL(ERRO), ("wait nandc_ctrl_read_result!ERROR \n"));
        goto ERRO;
    }

#ifdef NANDC_READ_RESULT_DELAY
    {
        volatile u32 loopcnt = 500;
        while(loopcnt--);  /*wait 500 before read controler status (0x20 bit1)*/
    }
#endif

    if(host->bitcmd.ecc_err_invalid == NANDC_GET_REG_BITS(cluster->ecc_result))
    {
#ifdef ECC_CHECK_FRESH_PAGE
        if(NANDC_FALSE ==  nandc_ctrl_is_fresh_page(host))
#endif
        {
            NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_ctrl_read_result: ecc error invalid!\n"));/*lint !e778*/
            errorcode = NANDC_E_ECC;
            goto ERRO;
        }
    }

    return NANDC_OK;

ERRO:
	/*lint -e778*/
    NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_ctrl_read_result:error address[0]:0x%x, address[1]:0x%x!\n",
                                        host->addr_physics[0],host->addr_physics[1]));
    /*lint +e778*/
    return errorcode;    
}

/********************************************************************************************
 * FUNC NAME:  
 * nandc_ctrl_int_clear() - clear interrupt status registers of nand flash chip.
 *
 * PARAMETER:
 * @host - [input]the main structure. 
 * @value - [input]the value to be written to interrupt status registers of nand flash chip 
 *
 * DESCRIPTION:
 * This function clears interrupt status registers of nand flash chip
 * 
 * CALL FUNC:
 * nandc_ctrl_read_id() etc. -
 * 
 ********************************************************************************************/
__inline u32 nandc_ctrl_int_clear(struct nandc_host *host,  u32 value)
{
    struct nandc_bit_cluster*  cluster; 
    cluster = (struct nandc_bit_cluster*)host->normal;

    NANDC_SET_REG_BITS (cluster->int_clr,     value);

    return NANDC_OK;
}

/********************************************************************************************
 * FUNC NAME:  
 * nandc_ctrl_int_set() - set interrupt setting registers of nand flash chip.
 *
 * PARAMETER:
 * @host - [input]the main structure.
 * @value - [input]the value to be written to interrupt setting registers of nand flash chip
 *
 * DESCRIPTION:
 * This function sets interrupt setting registers of nand flash chip.
 * 
 * CALL FUNC:
 * nandc_ctrl_read_id() -
 * 
 ********************************************************************************************/
__inline u32 nandc_ctrl_int_set(struct nandc_host *host, u32 value)
{
    struct nandc_bit_cluster*  cluster; 
    cluster = (struct nandc_bit_cluster*)host->normal;

    NANDC_SET_REG_BITS (cluster->int_enable,     value);

    return NANDC_OK;
}

/********************************************************************************************
 * FUNC NAME:  
 * nandc_ctrl_reset() - reset nand flash chip.
 *
 * PARAMETER:
 * @host - [input]the main structure.
 *
 * DESCRIPTION:
 * This function resets nand flash chip.
 * 
 * CALL FUNC:
 * nandc_ctrl_entry() - NAND_CMD_RESET
 * 
 ********************************************************************************************/
u32 nandc_ctrl_reset(struct nandc_host *host)
{
    register struct nandc_bit_cluster*  cluster;
    register struct nandc_bit_cmd*     ctrlcmd;
    volatile int reset_timeout = 10000;
    u32 errorcode = NANDC_ERROR;
        
    cluster = (struct nandc_bit_cluster*)host->normal;
    ctrlcmd = (struct nandc_bit_cmd*)&host->bitcmd;
    
    nandc_ctrl_set_cmd(host, NAND_CMD_RESET, NAND_CMD_NOP, NAND_CMD_NOP );
    nandc_ctrl_int_clear(host,                     ctrlcmd->intmask_all );

    NANDC_SET_REG_BITS (cluster->op_mode,          nandc_op_normal      );
    NANDC_SET_REG_BITS (cluster->ecc_select,       ctrlcmd->ecc_none    );

    NANDC_WRITE_REG_BITS(cluster->addr_cycle,      nandc_addrcycle_0    );
    NANDC_SET_REG_BITS (cluster->chip_select,      host->chipselect     );
    NANDC_SET_REG_BITS (cluster->operate,          ctrlcmd->op_reset    );

    /*  Send reset Command, wait for the device ready(sft-debugging) */ 
	/*begin lint-Info 732: (Info -- Loss of sign (arg. no. 4) (int to unsigned int))*/
    errorcode = nandc_ctrl_register_wait(host, &cluster->int_stauts, nandc_status_int_done, (u32)reset_timeout);
	/*end*/
    if(NANDC_OK != errorcode)
    {
        return NANDC_ERROR;
    }

    errorcode = nandc_ctrl_register_wait(host, &cluster->op_stauts, nandc_status_op_done, NANDC_RESULT_TIME_OUT);
    if(NANDC_OK != errorcode)
    {
        return NANDC_ERROR;
    }

    return NANDC_OK;
    
}

/********************************************************************************************
 * FUNC NAME:  
 * nandc_ctrl_read_id() - reads id data of nand flash chip.
 *
 * PARAMETER:
 * @host - [input]the main structure.
 *
 * DESCRIPTION:
 * This function reads id data of nand flash chip.
 * 
 * CALL FUNC:
 * nandc_ctrl_entry() - NAND_CMD_READID
 * 
 ********************************************************************************************/
u32 nandc_ctrl_read_id(struct nandc_host *host)
{
    register struct nandc_bit_cluster*  cluster;
    register struct nandc_bit_cmd*     ctrlcmd;

    u32 errorcode;
    
    cluster = (struct nandc_bit_cluster*)host->normal;
    ctrlcmd = (struct nandc_bit_cmd*)&host->bitcmd;
    
    nandc_ctrl_int_set(host, ctrlcmd->intdis_all);

    /*nandc_ctrl_reset(host);*/

    nandc_ctrl_set_cmd(host, NAND_CMD_READID, NAND_CMD_NOP, NAND_CMD_NOP);
	NANDC_SET_REG_BITS(cluster->addr_high,         0                   );
	NANDC_SET_REG_BITS(cluster->addr_low,          0                   );
	NANDC_SET_REG_BITS(cluster->ecc_type,          0                   );
	NANDC_SET_REG_BITS(cluster->ecc_select,        ctrlcmd->ecc_none   );

    nandc_ctrl_int_clear(host,ctrlcmd->intmask_all);

    NANDC_SET_REG_BITS(cluster->op_mode,           nandc_op_normal     );
    NANDC_SET_REG_BITS(cluster->bus_width,         host->probwidth     );

    NANDC_WRITE_REG_BITS(cluster->addr_cycle,      nandc_addrcycle_1   );
    NANDC_SET_REG_BITS(cluster->chip_select,       host->chipselect    );
    NANDC_SET_REG_BITS(cluster->operate,           ctrlcmd->op_read_id );

    errorcode = nandc_ctrl_register_wait(host, &cluster->int_stauts, nandc_status_int_done, NANDC_RESULT_TIME_OUT);
    if(NANDC_OK != errorcode)
    {
        goto ERRO;
    }
    
    errorcode = nandc_ctrl_register_wait(host, &cluster->op_stauts, nandc_status_op_done, NANDC_RESULT_TIME_OUT);
    if(NANDC_OK != errorcode)
    {
        goto ERRO;
    }
    
    if(NANDC_OK == errorcode)
    {
         nandc_data_transmit_raw(host, NANDC_READID_SIZE, 0, NAND_CMD_READSTART);
    }

    return errorcode;

ERRO:
    
    return NANDC_ERROR;
    
}

/********************************************************************************************
 * FUNC NAME:  
 * nandc_ctrl_read_dma() - a reading operation interface through DMA
 *
 * PARAMETER:
 * @host - [input]the main structure.
 *
 * DESCRIPTION:
 * This is a reading operation interface through DMA for nand flash controller with DMA.
 * 
 * CALL FUNC:
 * no use for temp.
 * 
 ********************************************************************************************/
u32 nandc_ctrl_read_dma(struct nandc_host *host)
{
    /* TODO  dma read mode */
    HI_NOTUSED(host);
    return NANDC_OK;
}

/********************************************************************************************
 * FUNC NAME:  
 * nandc_ctrl_read_normal() - the main reading operation interface.
 *
 * PARAMETER:
 * @host - [input]the main structure. 
 *
 * DESCRIPTION:
 * The main reading operation interface.
 * 
 * CALL FUNC:
 * nandc_ctrl_entry() - NAND_CMD_READSTART
 * 
 ********************************************************************************************/
u32 nandc_ctrl_read_normal(struct nandc_host *host)
{
    register struct nandc_bit_cluster*   cluster;
    register struct nandc_bit_cmd*       ctrlcmd;
    struct nandc_pagemap        *pagemap;
    struct nandc_nand           *nand = host->nandchip;
    struct nandc_spec           *spec = &nand->spec;
    u32 copy_offset = 0, read_times = 0, errorcode, sizeleft;
    static u32 totalsize, onesize, eccselect;

    cluster = host->normal;
    ctrlcmd = &host->bitcmd;

    if(nandc_read_latch_done != host->latched)
    {
        nandc_ctrl_snapshot(host, &host->read_snapshot, &totalsize, &onesize, &eccselect  );
        
        if(nandc_read_latch_start == host->latched)
        {
            NANDC_TRACE(NFCDBGLVL(NORMAL), ("nandc_ctrl_read_normal, nandc_read_latch_start! \n"));
            host->latched = nandc_read_latch_done;
        }
    }
    else
    {
        NANDC_TRACE(NFCDBGLVL(NORMAL), ("nandc_ctrl_read_normal, nandc_read_latch_done, fast skip!\n"));
    }
    
    NANDC_SET_REG_BITS(cluster->addr_low,       host->addr_physics[0]);
    NANDC_SET_REG_BITS(cluster->addr_high,      host->addr_physics[1]);
    NANDC_SET_REG_BITS(cluster->int_clr,        ctrlcmd->intmask_all );

    if(nandc_size_2k == spec->pagesize)
    {
       NANDC_SET_REG_BITS(cluster->operate,    ctrlcmd->op_read_start | ctrlcmd->op_read_end);
    }
    else if(totalsize == onesize)
    {
          NANDC_SET_REG_BITS(cluster->segment, ctrlcmd->op_segment_start |ctrlcmd->op_segment_end);
          NANDC_SET_REG_BITS(cluster->operate, ctrlcmd->op_read_start |ctrlcmd->op_read_end);
    }
    else
    {
       NANDC_SET_REG_BITS(cluster->segment,     ctrlcmd->op_segment_start);
       NANDC_SET_REG_BITS(cluster->operate,     ctrlcmd->op_read_start);
    }
    
    errorcode = nandc_ctrl_read_result(host);
    if(NANDC_OK == errorcode)
    {
        if((ctrlcmd->ecc_none == eccselect )||(ctrlcmd->ecc_oob == eccselect))
        {
            copy_offset = 0;
            nandc_data_transmit_raw(host, onesize, copy_offset, NAND_CMD_READSTART);
        }
        else 
        {  /*whole page read with ECC mode*/
            pagemap =   host->nandchip->datamap; /* nandc_host_get_pagemap */
            read_times = 0;    

#ifdef BAD_FLAG_SWAP_2K /*for onchip rom, TASKLINK nand chip programme*/
        ADDR_DATA_SWAP32((u32)host->bufbase + 0x7d0 , (u32)host->bufbase + 0x800); 
#endif     

            nandc_data_transmit_page(host, pagemap ,read_times, NAND_CMD_READSTART);
        }
    }
    else
    {
        goto EXIT;
    }

    sizeleft    =   totalsize - onesize;
    while(sizeleft > 0)
    {
        NANDC_SET_REG_BITS (cluster->int_clr,       ctrlcmd->intmask_all);
        sizeleft -= onesize;
        if(0 == sizeleft)
        {
            NANDC_SET_REG_BITS (cluster->segment,   ctrlcmd->op_segment_end);
            NANDC_SET_REG_BITS (cluster ->operate,  ctrlcmd->op_read_end);
        }
        else
        {
            NANDC_SET_REG_BITS (cluster ->operate,  ctrlcmd->op_read_continue);
        }
        
        errorcode = nandc_ctrl_read_result(host);
        if(NANDC_OK == errorcode)
        {
            if(ctrlcmd->ecc_none== eccselect )
            {
                copy_offset +=  onesize;
                nandc_data_transmit_raw(host, onesize, copy_offset, NAND_CMD_READSTART);
            }
            else 
            {
                read_times  +=  1;
                pagemap =   host->nandchip->datamap;   /* nandc_host_get_pagemap */
                nandc_data_transmit_page(host, pagemap ,read_times, NAND_CMD_READSTART);
            }
        }
        else
        {
        /*BEGIN:y00206456 2012-04-26 Modified for pclint e527*/
            goto EXIT;
        /*END:y00206456 2012-04-26 Modified for pclint e527*/
        }
        
    }
/*
    if((defines->feature)&&(NANDC_NULL != defines->feature->cb))
    {
        defines->feature->cb(host, defines->feature);
    }
*/    
    return NANDC_OK;

EXIT:
    return errorcode;

ERRO:
    return NANDC_ERROR;
}

/********************************************************************************************
 * FUNC NAME:  
 * nandc_ctrl_read_status() - always return nand flash chip good status.
 *
 * PARAMETER:
 * @host - [input]the main structure.
 *
 * DESCRIPTION:
 * This function always returns nand flash chip good status.
 * 
 * CALL FUNC:
 * nandc_ctrl_entry() - NAND_CMD_STATUS
 * 
 ********************************************************************************************/
static u32 nandc_ctrl_read_status(struct nandc_host *host)
{
    struct nandc_bit_cluster*  cluster = host->normal;

    if(host->bitcmd.ecc_err_invalid == NANDC_GET_REG_BITS(cluster->ecc_result))
    {
        *(volatile int*)host->bufbase = NAND_STATUS_FAIL;  /*0000 0001*/ 
    }
    else
    {
    
        *(volatile int*)host->bufbase = NAND_STATUS_READY | NAND_STATUS_WP;  /*1100 0000*/ 
    }
    
    return NANDC_OK;
}

/********************************************************************************************
 * FUNC NAME:  
 * nandc_ctrl_program_result() - query result for writing operation.
 *
 * PARAMETER:
 * @host - [input]the main structure.
 *
 * DESCRIPTION:
 * This function query result for writing operation.
 * 
 * CALL FUNC:
 * nandc_ctrl_program_normal() -
 * 
 ********************************************************************************************/
/* 解决由于写操作时寄存器状态未更新导致FLASH产生坏块问题 */
static __inline u32 nandc_ctrl_program_result(struct nandc_host *host, u32 size_left)
{
    struct nandc_bit_cluster*  cluster = NANDC_NULL;
    u32 errorcode = NANDC_ERROR; 
    u32 nand_status_timeout = NANDC_PRG_RESULT_TIMEOUT;
     
    cluster = host->normal;

#ifdef NANDC_CTRL_DEBUG
    nandc_ctrl_dump_reg(host, "nandc program result");
#endif

    errorcode = nandc_ctrl_register_wait(host, &cluster->op_stauts, nandc_status_op_done, NANDC_PRG_RESULT_TIMEOUT);
    if(NANDC_OK != errorcode)
    {
        NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_ctrl_program_result,wait return ERROR! \n"));/*lint !e778*/
        return errorcode;
    }
    if (0 == size_left)
    {
        while ((NAND_STATUS_FAIL ==  (NAND_STATUS_FAIL & NANDC_GET_REG_BITS(cluster->nf_status))) && (nand_status_timeout))
        {
            nand_status_timeout--;
        }
       
        if (0 ==  nand_status_timeout)
        {
            NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_ctrl_program_result,status return FAIL! \n"));/*lint !e778*/
            return NANDC_E_TIMEOUT;
        }
    }
    return NANDC_OK;    
}

/********************************************************************************************
 * FUNC NAME:  
 * nandc_ctrl_erase_result() - query result for eraseing operation.
 *
 * PARAMETER:
 * @host - [input]the main structure.
 *
 * DESCRIPTION:
 * This function query result for eraseing operation.
 * 
 * CALL FUNC:
 * nandc_ctrl_earse_block() - 
 * 
 ********************************************************************************************/
static __inline u32 nandc_ctrl_erase_result(struct nandc_host *host)
{
    struct nandc_bit_cluster*  cluster = NANDC_NULL;
    u32 errorcode = NANDC_ERROR;
    u32 nand_status;
        
    cluster = host->normal;
    
#ifdef NANDC_CTRL_DEBUG
    nandc_ctrl_dump_reg(host, "nandc erase result");
#endif

    errorcode = nandc_ctrl_register_wait(host, &cluster->op_stauts, nandc_status_op_done, NANDC_ERASE_RESULT_TIMEOUT);
    if(NANDC_OK != errorcode)
    {
          NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_ctrl_erase_result,wait return error! \n"));/*lint !e778*/
          goto ERRO;
    }

    if(nandc_status_int_done != NANDC_GET_REG_BITS(cluster->int_stauts))
    {
        errorcode = NANDC_E_ERASE;
#ifndef WIN32
        NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_ctrl_erase_result,int return error! \n"));/*lint !e778*/
#endif
        goto ERRO;
    }

    nand_status = NANDC_GET_REG_BITS(cluster->nf_status);
    if(NAND_STATUS_FAIL & nand_status)
    {
        NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_ctrl_erase_result,status return FAIL!:0x%x \n",nand_status));/*lint !e778*/
        errorcode = NANDC_E_ERASE;
        goto ERRO;
    }

    return NANDC_OK;

ERRO:
    
#ifdef WIN32
    errorcode = NANDC_OK;
#endif
    
    return errorcode;
}

/********************************************************************************************
 * FUNC NAME:  
 * nandc_ctrl_program_dma() - a writing operation interface through DMA
 *
 * PARAMETER:
 * @host - [input]the main structure.
 *
 * DESCRIPTION:
 * This is a writing operation interface through DMA for nand flash controller with DMA.
 * 
 * CALL FUNC:
 * no use for temp.
 * 
 ********************************************************************************************/
static u32 nandc_ctrl_program_dma(struct nandc_host *host)
{
    /* TODO dma write mode */
    HI_NOTUSED(host);
    return NANDC_OK;
}

/********************************************************************************************
 * FUNC NAME:  
 * nandc_ctrl_program_normal() -  the main writing operation interface.
 *
 * PARAMETER:
 * @host - [input]the main structure.
 *
 * DESCRIPTION:
 * The main writing operation interface.
 * 
 * CALL FUNC:
 * nandc_ctrl_entry() - NAND_CMD_PAGEPROG
 * 
 ********************************************************************************************/
    static u32 nandc_ctrl_program_normal(struct nandc_host *host)
    {
        register struct nandc_bit_cluster*   cluster;
        register struct nandc_bit_cmd*       ctrlcmd;
        struct nandc_pagemap        *pagemap;
        struct nandc_nand           *nand = host->nandchip;
        struct nandc_spec           *spec = &nand->spec;  
        u32 copy_offset = 0, write_times = 0, errorcode, sizeleft;
        static u32 totalsize, onesize, eccselect;
    
        cluster = host->normal;
        ctrlcmd = &host->bitcmd;
        if(nandc_write_latch_done != host->latched)
        {
            nandc_ctrl_snapshot(host, &host->write_snapshot, &totalsize, &onesize, &eccselect  );
                   
            if(nandc_write_latch_start == host->latched)
            {
                NANDC_TRACE(NFCDBGLVL(NORMAL), ("nandc_ctrl_program_normal, nandc_write_latch_start! \n"));
                host->latched = nandc_write_latch_done;
            }
        }
        else
        {
            NANDC_TRACE(NFCDBGLVL(NORMAL), ("nandc_ctrl_program_normal, nandc_write_latch_done, fast skip!\n"));
        }
        
        NANDC_SET_REG_BITS (cluster->addr_low,      host->addr_physics[0]   );
        NANDC_SET_REG_BITS (cluster->addr_high,     host->addr_physics[1]   );
        NANDC_SET_REG_BITS (cluster->int_clr,       ctrlcmd->intmask_all);
    
        if((ctrlcmd->ecc_none== eccselect )||(ctrlcmd->ecc_oob== eccselect))
        {
            copy_offset =   0;
            nandc_data_transmit_raw(host, onesize, copy_offset, NAND_CMD_PAGEPROG);
        }
        else 
        {   /* whole page write with ECC mode */
            pagemap     =   host->nandchip->datamap;  /* nandc_host_get_pagemap */
            write_times =   0;    
            nandc_data_transmit_page(host, pagemap ,write_times, NAND_CMD_PAGEPROG);
            
#ifdef BAD_FLAG_SWAP_2K /*for onchip rom, TASKLINK nand chip programme*/
            ADDR_DATA_SWAP32((u32)host->bufbase + 0x7d0 , (u32)host->bufbase + 0x800);  
#endif     
        }
    
        if(nandc_size_2k == spec->pagesize)
        {
           NANDC_SET_REG_BITS(cluster ->operate,    ctrlcmd->op_write_start | ctrlcmd->op_write_end);
        }
		else if(totalsize == onesize)
		{
			NANDC_SET_REG_BITS(cluster->segment,     ctrlcmd->op_segment_start | ctrlcmd->op_segment_end);
			NANDC_SET_REG_BITS(cluster ->operate,    ctrlcmd->op_write_start | ctrlcmd->op_write_end);
		}

        else
        {
           NANDC_SET_REG_BITS(cluster->segment,     ctrlcmd->op_segment_start);
           NANDC_SET_REG_BITS(cluster->operate,     ctrlcmd->op_write_start);
        }
        
        sizeleft    =   totalsize - onesize;
        /* 解决由于写操作时寄存器状态未更新导致FLASH产生坏块问题*/
        errorcode   = nandc_ctrl_program_result(host,sizeleft);
        if(NANDC_OK != errorcode)
        {
            goto EXIT;
        }
    
        while(sizeleft > 0)
        {
            {
                if(ctrlcmd->ecc_none== eccselect )
                {
                    copy_offset += onesize;
                    nandc_data_transmit_raw(host, onesize, copy_offset, NAND_CMD_PAGEPROG);
                }
                else 
                {
                    write_times +=  1;
                    pagemap =   host->nandchip->datamap;  /* nandc_host_get_pagemap */
                    nandc_data_transmit_page(host, pagemap ,write_times, NAND_CMD_PAGEPROG);
                }
            }
    
            NANDC_SET_REG_BITS (cluster->int_clr,       ctrlcmd->intmask_all);
            sizeleft    -=  onesize;    
            if(0 == sizeleft)
            {
                NANDC_SET_REG_BITS (cluster->segment,   ctrlcmd->op_segment_end);
                NANDC_SET_REG_BITS (cluster ->operate,  ctrlcmd->op_write_end);
            }
            else
            {
            	/* 将read 改为write */
            	NANDC_SET_REG_BITS (cluster ->operate,  ctrlcmd->op_write_continue);
            }
        	/*解决由于写操作时寄存器状态未更新导致FLASH产生坏块问题*/
        	errorcode = nandc_ctrl_program_result(host,sizeleft);
            if(NANDC_OK != errorcode)
            {
            /*BEGIN:y00206456 2012-04-26 Modified for pclint e527*/
                goto EXIT;
            /*END:y00206456 2012-04-26 Modified for pclint e527*/
            }
        }
        
        return NANDC_OK;
    
    EXIT:
        return errorcode;
    
    ERRO:
    
        return NANDC_ERROR;
        
    }

/********************************************************************************************
 * FUNC NAME:  
 * nandc_ctrl_earse_block() - erase one block according to block number.
 *
 * PARAMETER:
 * @host - [input]the main structure. 
 *
 * DESCRIPTION:
 * This function erases one block according to block number which is stored in host->addr_physics[0].
 * 
 * CALL FUNC:
 * nandc_ctrl_entry() - NAND_CMD_ERASE2
 * 
 ********************************************************************************************/
static u32 nandc_ctrl_earse_block(struct nandc_host *host)
{
    register struct nandc_bit_cluster    *cluster;
    register struct nandc_bit_cmd        *ctrlcmd;
    struct nandc_nand           *nand = host->nandchip;
    struct nandc_spec           *spec = &nand->spec;  
    u32 addrcycle;
    u32 count = 3;
    u32 errocode = NANDC_ERROR;    
    
    ctrlcmd = (struct nandc_bit_cmd*)&host->bitcmd;   
    cluster = host->normal;
    
    addrcycle = spec->addrcycle - 2;

    if(0 == host->addr_physics[0])
    {
        NANDC_TRACE(NFCDBGLVL(WARNING), ("nandc_ctrl_earse_block: erase blknum 0.\n"));
    }
    
    nandc_ctrl_set_cmd  (host,      NAND_CMD_ERASE1, NAND_CMD_ERASE2, NAND_CMD_STATUS);
    nandc_ctrl_int_clear(host,      ctrlcmd->intmask_all                );

    NANDC_SET_REG_BITS(cluster->addr_low,       host->addr_physics[0]   );
    NANDC_SET_REG_BITS(cluster->ecc_select,     ctrlcmd->ecc_none       );
    NANDC_SET_REG_BITS(cluster->ecc_type,       0                       );

    NANDC_WRITE_REG_BITS(cluster->addr_cycle,   addrcycle			    );
    NANDC_SET_REG_BITS(cluster->chip_select,    host->chipselect        );

    while((NANDC_OK != errocode)&&(count > 0))
    {
        NANDC_SET_REG_BITS(cluster->operate,        ctrlcmd->op_erase   );
        errocode =  nandc_ctrl_erase_result(host);
        count--;
    }

    if(NANDC_OK != errocode)
    {
       NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_ctrl_earse_block, failed 3 times!\n"));/*lint !e778*/
    }
    
    return errocode;
}

/********************************************************************************************
 * FUNC NAME:  
 * nandc_ctrl_entry() - hand out the operation commands.
 *
 * PARAMETER:
 * @host - [input]the main structure.
 *
 * DESCRIPTION:
 * This function transfers commands to corresponding interfaces through which operation commands and  
 * parameters are sent to corresponding registers of nand flash chip by calling "NANDC_SET_REG_BITS".
 * 
 * CALL FUNC:
 * nandc_mtd_cmd_ctrl() -
 * nandc_nand_access() - 
 *
 ********************************************************************************************/
u32 nandc_ctrl_entry(struct nandc_host *host)
{
    u32 errorcode = NANDC_ERROR ;
    u32 try_left = NANDC_RD_RETRY_CNT; /*try times if read flash error*/
    volatile u32 delay_loop;
    
    NANDC_TRACE(NFCDBGLVL(NORMAL), ("(0x%x)nandc_ctrl_entrycs(%d),real(0x%"FWC"x), low addr:0x%08x, high addr:0x%08x\n",
                                               host->command,  host->chipselect,host->addr_real, host->addr_physics[0],host->addr_physics[1]));
TRY_AGAIN1:

    /* 改为AXI Memory标记，双核都能看到 */
    if(1 == *(u32*)MEMORY_AXI_PERI_DOWN_FLAG_ADDR)
    {
        nandc_ctrl_reset(host);
        *(u32*)MEMORY_AXI_PERI_DOWN_FLAG_ADDR = 0;  // 恢复初值
    }
    
    switch (host->command)
    {
        case NAND_CMD_READSTART:
            if(host->options & NANDC_OPTION_DMA_ENABLE)
            {
                errorcode = nandc_ctrl_read_dma(host);
            }
            else
            {
                errorcode = nandc_ctrl_read_normal(host);
            }            
        break;

        case NAND_CMD_READID:
            errorcode = nandc_ctrl_read_id(host);
        break;

        case NAND_CMD_STATUS:            
            errorcode = nandc_ctrl_read_status(host);
        break;

/* For __BOOTLOADER__, it opens NANDC_READ_ONLY,so !NANDC_COMPILER(NANDC_READ_ONLY) is 0,
   then NAND_CMD_PAGEPROG is disabled,for other platform it closes NANDC_READ_ONLY, so 
   !NANDC_COMPILER(NANDC_READ_ONLY) is 1,and NAND_CMD_PAGEPROG is enabled */
#if !NANDC_COMPILER(NANDC_READ_ONLY)
        case NAND_CMD_PAGEPROG:

            if(host->options & NANDC_OPTION_DMA_ENABLE)
            {
               errorcode =  nandc_ctrl_program_dma(host);
            }
            else
            {
               errorcode =  nandc_ctrl_program_normal(host);
            }
        break;

        case NAND_CMD_ERASE2:

            errorcode = nandc_ctrl_earse_block(host);

        break;
#endif

        case NAND_CMD_SEQIN:
        case NAND_CMD_ERASE1:
        case NAND_CMD_READ0:
        break;

        case NAND_CMD_RESET:
			errorcode = nandc_ctrl_reset(host);
        break;

        default :
            errorcode = NANDC_ERROR;
            NANDC_DO_ASSERT(0, "unknown nand command!", host->command);
        break;
        }

    switch (host->command)
    {
        case NAND_CMD_READSTART: 
        {
            if((NANDC_OK != errorcode)&&(try_left))
            {
                try_left--;
                
                /*delay some time*/
                delay_loop = 100000;
                while(delay_loop--);
                
                goto TRY_AGAIN1;
            }
        }
        break;
        default:
        break;
    }

    return errorcode;

    
}

/*lint +e506 */

#ifdef COMPILE_BOOTLOADER
u32 nandc_io_bitwidth_set(u32 bitwidth)
{
    u32 ioshare_val;

    /* 非16bit即8bit */
    if(NAND_BUSWIDTH_16 == bitwidth)
    {
        ioshare_val = 1;
    }
    else    /* 8bit NAND,配置上下拉 */
    {
        ioshare_val = 0;
        /* 2_24, 2_25, 2_27, 2_28 下拉*/
	    NANDC_REG_WRITE32(INTEGRATOR_SC_BASE + 0x08B0, 0x02000200);
	    NANDC_REG_WRITE32(INTEGRATOR_SC_BASE + 0x08B4, 0x02000000);
	    NANDC_REG_WRITE32(INTEGRATOR_SC_BASE + 0x08B8, 0x00000200);
    }
    
    NANDC_REG_SETBIT32(INTEGRATOR_SC_BASE + INTEGRATOR_SC_IOS_CTRL98, 2, 1, ioshare_val);
    return NANDC_OK;
}
#endif

#ifdef __cplusplus
}
#endif

