/*******************************************************************************
* Copyright (C), 2008-2011, HUAWEI Tech. Co., Ltd.
*
* Module name: Hisilicon Nand Flash Controller Dirver
*
* Description: the Hisilicon Nand Flash Controller physical and logical driver
*
* Version: v1.0
*
* Filename:    nandc_balong.h
* Description: nand controller operations in dependence on  hardware
*
* Function List:
*
* History:
1.date:2011-07-27
 question number:
 modify reasion:         create
*******************************************************************************/
/******************************************************************************
*    Copyright (c) 2009-2011 by  Hisilicon Tech. Co., Ltd.
*    All rights reserved.
* ***
*
******************************************************************************/
#ifndef _NANDC_BALONG_H_
#define _NANDC_BALONG_H_
    
#ifdef __cplusplus
extern "C"
{
#endif

#include "nandc_inc.h"


/*enum of nand operation type */
enum NAND_FUNC
{
    READ_NAND,              /*nand_read()*/
    READ_YAFFS,             /*nand_read_yaffs()*/
    READ_PAGE,
    READ_RAW,               /*nand_read_raw()*/
    WRITE_NAND,             /*nand_write()*/
    WRITE_YAFFS,            /*nand_write_yaffs()*/
    WRITE_PAGE, 
    WRITE_RAW,              /*nand_write_raw()*/
    ERASE_BLOCK,            /*nand_erase()*/
    QUARY_BAD,              /*nand_isbad()*/
    MARK_BAD                /*nand_bad()*/
};

/*structure of nand operation parameter */
struct nand_param
{
    u32 number;             /*indicate a number ,may be a flash block number or a flash page number*/
    FSZ addr_flash;         /*flash address to be read from or be writen to */
    u32 addr_data;          /*the ram address contain data that should be writen to flash, or the ram address 
                              should store data read from flash */
    u32 addr_oob;           /*the ram address contain oob that should be writen to flash, or the ram address 
                              should store oob read from flash */
    u32 size_data;          /*the size of data to be read or writen*/
    u32 size_oob;           /*the size of oob to be read or writen*/
    u32 *skip_length;       /*flash skip length during nand operation*/
};

/*structure of nand chip specification parameter */
struct nand_spec
{
#if (FEATURE_EPAD == FEATURE_ON)
    char* menuf_name;       /*factory name string*/
    char* dev_name;         /*device id*/
    u32   menuf_id;         /*factory ID */
    u32   dev_id;           
#endif	
    u32 blocksize;          /*one block size of nand flash*/
    u32 pagesize;           /*one page size of nand flash*/
    u32 chipsize;           /*the total size of nand flash*/
    u32 sparesize;           /*the total size of nand flash*/
};


/*structure of nand plantform layer bus */
struct nand_bus
{
    struct nand_interface * funcs;          /*point to nand flash operation structure*/
    u32 (*sem_lock)   (void* sem);          /*lock for multitask access*/    
    u32 (*sem_unlock) (void* sem);          /*unlock for multitask access*/  
    u32 (*ipc_wait)     (u32 overtime);     /*lock for multicore access*/ 
    void (*ipc_post)     (void);            /*unlock for multicore access*/     
    void * sem;
    u32 init_flag;                          /*the flag indicate if nand module is initialized*/
};

struct nand_interface
{
    u32 (*init)(void);
    u32 (*deinit)(void);
    u32 (*read)(FSZ flashaddr, u32 dstaddr, u32 readsize, u32 oobsize, u32 *skipblks);
    u32 (*get_spec)(struct nand_spec *specs);

    u32 (*read_page_yaffs)(u32 pagenum, u8* databuff, u8* oobbuff, u32 oobsize);
    u32 (*read_page_ecc)(FSZ address, u8* databuff, u8* oobbuff, u32 oobsize);
    u32 (*read_page_raw)(u32 pagenum, u32 offset, u32 dst_addr, u32 readsize);
    
    u32 (*quary_bad)(u32 blkid, u32 * isbad );
    
    u32 (*write)(FSZ flashaddr, u32 srcaddr, u32 writesize, u32 oobsize, u32 *skiplength);
    
    u32 (*write_page_yaffs)(u32 pagenum, u8* databuff, u8* oobbuff, u32 oobsize);
    u32 (*write_page_ecc)(FSZ address, u8* databuff, u8* oobbuff, u32 oobsize);
    u32 (*write_page_raw)(u32 pagenum, u32 offset, u32 src_addr, u32 writesize);
    
    u32 (*mark_bad)(u32 blk_id);
    u32 (*erase_by_id)(u32 blk_id);
    /*u32 (*erase_by_addr)(FSZ blk_addr);*/
};


#define NAND_INIT_MAGIC 0x19840815
#define NAND_OK             0
#define NAND_ERROR        1


#if (NANDC_COMPILE_FLAG & NANDC_DEBUG)
#define NAND_TRACE(p)  hiout(p)
#else
#define NAND_TRACE(p)  
#endif

#define NAND_IPC_OVERTIME_MS    0xFFFFFFFF   


extern struct nandc_host*  nandc_nand_host;



 /**
 * FUNC NAME:  
 * nand_init() - nand driver plantform layer initialize
 *
 * PARAMETER: none
 *
 * DESCRIPTION:
 * this functon initialize nand plantform layer ram structure and then call low 
 * layer nand initialization route
 *
 * CALL FUNC:
 * nandc_mutex_create() : create semaphore for multitask mutex
 * nandc_ipc_create():    create IPC for multicore mutex
 * 
 */
u32 nand_init(void);
u32 nand_balong_port_mtd(void);
  /**
 * FUNC NAME:  
 * nand_deinit() - nand driver plantform layer deinitialize
 *
 * PARAMETER: none
 *
 * DESCRIPTION:
 * this functon deinitialize nand plantform layer ram structure and then call low 
 * layer nand deinitialization route
 *
 * CALL FUNC:
 * nandc_mutex_delete() : delete semaphore for multitask mutex
 * nandc_ipc_delete():    delete IPC for multicore mutex
 * 
 */
u32 nand_deinit(void);

  
/**
 * FUNC NAME:  
 * nand_read() - external API for nand data read
 *
 * PARAMETER:
 * @flash_addr - [input]read start address from nand
 * @dst_data   - [input]data buffer address where to store flash data that should be read.
 * @read_size  - [input]how much data to be read form flash
 * @skip_length- [output]a ram address that store bad block skip length during read operation,
 *               this parameter could be NULL if you don't care about skip length;
 *
 * DESCRIPTION:
 * read flash data like memcpy(), it means you can read flash data form any address and 
 * read any size.
 *
 * this route don't read oob date from nand spare area.
 * 
 * CALL FUNC:
 * @nand_run() - 
 * 
 */
u32 nand_read(FSZ flash_addr, u32 dst_data, u32 read_size, u32* skip_length);

/**
 * FUNC NAME:  
 * nand_read_oob() - external API for nand data read with oob
 *
 * PARAMETER:
 * @flash_addr - [input]read start address from nand.
 * @dst_data   - [output]ram data buffer address where to store flash data that should be read.
 * @read_size  - [input]how much data to be read form flash, this is the sum of all data and
                 oob size, it means read_size shouled be multiple of (page size +oob_per_page)
 * @oob_per_page-[input]oob data size per page. 
 *               
 * DESCRIPTION:
 * read flash data and spare data page by page, at the same time, the flash data and oob data will write 
 * to ram buffer one by one
 *
 * CALL FUNC:
 * @nand_run() - 
 * 
 */
u32 nand_read_oob(FSZ flash_addr, u32 dst_data, u32 data_size, u32 oob_per_page, u32* skip_length);

/**
 * FUNC NAME:  
 * nand_read_raw() - external API for nand raw data read 
 *
 * PARAMETER:
 * @src_page    - [input]the page number to be read from nand.
 * @offset_data - [input]data offset in this page
 * @dst_data    - [output]the ram buffer address to store flash data
 * @len_data    - [input]data length to be read in this page
 *               
 * DESCRIPTION:
 * read raw flash data without ecc correction in one page, the input parameter 
 * (offset_data + len_data) should not exceed one page size.  
 *
 * CALL FUNC:
 * @nand_run() - 
 * 
 */
u32 nand_read_raw(u32 src_page, u32 offset_data, u32 dst_data, u32 len_data, u32* skip_length);

/**
 * FUNC NAME:  
 * nand_read_yaffs() - external API for yaffs's nand data read 
 *
 * PARAMETER:
 * @flash_page - [input]the page number to be read from nand.
 * @dst_data   - [output] ram address to store yaffs data
 * @dst_oob    - [output] ram address to store yaffs oob
 * @len_oob    - [input]  indicate the oob length in one yaffs page
 *               
 * DESCRIPTION:
 * read flash data and oob data with ecc correction in one page.
 * this routing don't go through nand_run() for high efficiency, so before call this, nand_init()
 * must run first, otherwise unforeseeable fatal error should occur.
 * 
 * CALL FUNC:
 * @nand_mutex_in
 * @nandc_nand_yaffs_read() -
 * @nand_mutex_out
 * 
 */
u32 nand_read_yaffs(u32 flash_page, u32 dst_data, u32 dst_oob, u32 len_oob);

/**
 * FUNC NAME:  
 * nand_erase() - external API :erase a nand block by block number
 *
 * PARAMETER:
 * @blockID - [input] the block number to be erased.
 *               
 * DESCRIPTION:

 * CALL FUNC:
 * @nand_run() - 
 * 
 */
u32 nand_erase(u32 blockID);

u32 nand_isbad(u32 blockID, u32 *flag);

/**
 * FUNC NAME:  
 * nand_bad() - external API :mark a nand block as bad
 *
 * PARAMETER:
 * @blockID - [input] the block number to be marked.
 *               
 * DESCRIPTION:

 * CALL FUNC:
 * @nand_run() - 
 * 
 */
u32 nand_bad(u32 blockID);

/**
 * FUNC NAME:  
 * nand_write() - external API for nand data write
 *
 * PARAMETER:
 * @flash_addr - [input]write start address to nand
 * @src_data   - [input]data buffer address where to store data that should be writen.
 * @write_size - [input]how much data to be writen to flash
 * @skip_length- [output]a ram address that store bad block skip length during write operation,
 *               this parameter could be NULL if you don't care about skip length;
 *
 * DESCRIPTION:
 * write flash data like memcpy(), it means you can write flash data to any address and 
 * write any size.
 *
 * this route don't write oob date from nand spare area.
 * 
 * CALL FUNC:
 * @nand_run() - 
 * 
 */
u32 nand_write(FSZ flash_addr, u32 src_data, u32 write_size, u32* skip_length);

/**
 * FUNC NAME:  
 * nand_write_oob() - external API for nand data write with oob 
 *
 * PARAMETER:
 * @flash_addr - [input]write start address from nand.
 * @src_data   - [input]ram data buffer address where to store flash data that should be writen.
 * @data_size  - [input]how much data to be write form flash, this is the sum of all data and
                 oob size, it means data_size shouled be multiple of (page size +oob_per_page)
 * @oob_per_page-[input]oob data size per page. 
 *               
 * DESCRIPTION:
 * read flash data and spare data page by page, at the same time, the flash data and oob data will write 
 * to ram buffer one by one
 *
 * CALL FUNC:
 * @nand_run() - 
 * 
 */
u32 nand_write_oob(FSZ flash_addr, u32 src_data, u32 data_size, u32 oob_per_page, u32* skip_length);

/**
 * FUNC NAME:  
 * nand_write_raw() - external API for nand raw data write 
 *
 * PARAMETER:
 * @src_page    - [input]the page number to be writen to nand.
 * @offset_data - [input]data offset in this page
 * @dst_data    - [output]the ram buffer address to store flash data
 * @len_data    - [input]data length to be writen to this page
 *               
 * DESCRIPTION:
 * write raw data without ecc correction to one page, the input parameter 
 * (offset_data + len_data) should not exceed one page size.  
 *
 * CALL FUNC:
 * @nand_run() - 
 * 
 */
u32 nand_write_raw(u32 dst_page, u32 offset_data, u32 src_data, u32 len_data, u32* skip_length);

/**
 * FUNC NAME:  
 * nand_write_yaffs() - external API for yaffs's nand data write 
 *
 * PARAMETER:
 * @flash_page - [input]  the page number to be writen to nand.
 * @src_data   - [output] ram address to store yaffs data
 * @dst_oob    - [output] ram address to store yaffs oob
 * @len_oob    - [input]  indicate the oob length in one yaffs page
 *               
 * DESCRIPTION:
 * write flash data and oob data with ecc correction to one page.
 * this routing don't go through nand_run() for high efficiency, so before call this, nand_init()
 * must run first, otherwise unforeseeable fatal error should occur.
 * 
 * CALL FUNC:
 * @nandc_nand_yaffs_write() - 
 * @nand_mutex_in   -
 * @nand_mutex_out  -
 * 
 */
u32 nand_write_yaffs(u32 flash_page, u32 dst_data, u32 dst_oob, u32 len_oob);

/**
 * FUNC NAME:  
 * nand_get_spec() - external API for get nand chip specification parmeter
 *
 * PARAMETER:
 * @spec -  [output]pointer of structure contains nand chip specification parmeter
 *
 * DESCRIPTION:
 * to get flash total size , page size ,spare size ,block size...etc
 *
 * CALL FUNC:
 * 
 */
u32 nand_get_spec(struct nand_spec *spec);

/**
 * FUNC NAME:  
 * and_isinit() - external API: to quary if nand module has been initialized
 *
 * PARAMETER: none
 *               
 * DESCRIPTION: 
 * 
 * CALL FUNC: none
 */
u32 nand_isinit(void);
void nandc_minit(void);

/**
 * FUNC NAME:  
 * nand_init_clear() - external API: thid function clear .bss section of nandc module
 *
 * PARAMETER: none
 *               
 * DESCRIPTION:
 * in some plantform, the .bss section is not cleared when system power(eg. trace .AXF),so I clear .bss
 * manually here to avoid critical error.
 *
 * CALL FUNC: 
 * @nandc_minit()  -
 */
void nand_init_clear(void);

/**
 * FUNC NAME:  
 * nand_scan_bad() - external API for yaffs's nand block scan and mark bad
 *
 * PARAMETER:
 * @start  - [input] the start address of nand flash to scan.
 * @length - [input] the length of nand flash to scan.
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
u32 nand_scan_bad(FSZ start, FSZ length, u32 if_erase);

extern u32 nand_yaffs_data_size(u32 size_with_spare);

extern u32 nand_yaffs_write_size(u32 size_without_tags);

extern u32 bootload_div( u32 ulDividEnd, u32 ulDivisor, u32 *ulmods);


int PWRCTRL_NAND_LowPowerEnter(void);
int PWRCTRL_NAND_LowPowerExit(void);
extern u32 nand_mutex_in(struct nand_bus* handle);
extern u32 nand_mutex_out(struct nand_bus* handle);

#ifdef __cplusplus
}
#endif

#endif  /*_NANDC_DEF_H_*/



