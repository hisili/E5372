/*******************************************************************************
* Copyright (C), 2008-2011, HUAWEI Tech. Co., Ltd.
*
* Module name: Hisilicon Nand Flash Controller Dirver
*
* Description: the Hisilicon Nand Flash Controller physical and logical driver
*
* Version: v1.0
*
* Filename:    nandc_native.c
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
#ifdef __cplusplus
extern "C"
{
#endif

#include "nandc_inc.h"

#ifdef __KERNEL__
#else
u32 g_nand_oob_size  = 16;
u32 g_nand_page_size = 0;
u32 g_nand_ecc_bits  = 1;
#endif

/*lint -e767*/
#define NFCDBGLVL(LVL)   (NANDC_TRACE_NATIVE|NANDC_TRACE_##LVL)
/*lint +e767*/
 /*******************************************************************************
 * FUNC NAME:
 * nandc_bbm_real_addr() -
 *
 * PARAMETER:
 * @addr_virtual - [input]
 * @addr_real - [input]
 *
 * DESCRIPTION:
 *
 *
 *
 * CALL FUNC:
 * () -
 *
 ********************************************************************************/
u32 nandc_bbm_real_addr(FSZ addr_virtual, FSZ *addr_real)
{
    *addr_real = addr_virtual;

    return NANDC_OK;
}

 /*******************************************************************************
 * FUNC NAME:
 * nandc_native_reset_chip() - reset nand flash chip.
 *
 * PARAMETER:
 * @host - [input]the main structure.
 * @chipselect - [input]according it to reset which flash chip if there are two or more chips.
 *
 * DESCRIPTION:
 * This function resets the nand flash chip.when we read id from nand flash chip. we
 * should reset nand flash chip first.
 *
 *
 * CALL FUNC:
 * nandc_native_nand_prob() -
 *
 ********************************************************************************/
u32 nandc_native_reset_chip(struct nandc_host* host , u32 chipselect)
{
    host->command = NAND_CMD_RESET;
    host->chipselect   = chipselect;

    return nandc_ctrl_entry(host);

}

 /*******************************************************************************
 * FUNC NAME:
 * nandc_native_read_id() - reads nand flash chip ID data.
 *
 * PARAMETER:
 * @host - [input]the main structure.
 * @idbuf - [input/output]ram buffer to restore  the  nand flash chip ID data.
 * @length - [input]the length of the  nand flash chip ID data.
 *
 *
 * DESCRIPTION:
 * This function reads nand flash chip ID data through low layer interface
 * nandc_ctrl_read_id.
 *
 * CALL FUNC:
 * nandc_native_get_id() -
 *
 ********************************************************************************/
u32 nandc_native_read_id(struct nandc_host* host , u8* idbuf, u32 length)
{

    if(length > NANDC_READID_SIZE)
    {
        /*lint -e778 */
        NANDC_TRACE( NFCDBGLVL(ERRO),("read id length error: %d",length ));
        /*lint +e778 */
        NANDC_DO_ASSERT(0, "", 0 );

        return NANDC_ERROR;
    }

    host->command = NAND_CMD_READID;
    host->databuf = idbuf;

    return nandc_ctrl_entry(host);

}

 /*******************************************************************************
 * FUNC NAME:
 * nandc_native_get_spec() - gets nand chip's spec (page size,oob size,erase size etc.)
 *
 * PARAMETER:
 * @id_data - [input/output]the original id data read from nand flash chip.
 * @spec - [input/output]record nand chip's spec(page size,oob size,erase size etc.)
 *                       which is calculated from original id data.
 *
 * DESCRIPTION:
 * This function gets nand flash chip's spec (page size,oob size,erase size etc.)by reading
 * ID datas from nand chip.
 *  id[0]: manufacture id
 *  id[1]: device id.
 *  id[3]:the most important byte.bits function as follow:
 *  --------------------------------------------------------------
 *  | item       | description  |  7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
 *  |-------------------------------------------------------------
 *  |            |     1KB      |    |   |   |   |   |   | 0 | 0 |
 *  | Page Size  |     2KB      |    |   |   |   |   |   | 0 | 1 |
 *  |            |     4KB      |    |   |   |   |   |   | 1 | 0 |
 *  |            |     8KB      |    |   |   |   |   |   | 1 | 1 |
 *  |-------------------------------------------------------------
 *  |            |    64KB      |    |   |   |   | 0 | 0 |   |   |
 *  | Block Size |   128KB      |    |   |   |   | 0 | 1 |   |   |
 *  |            |   256KB      |    |   |   |   | 1 | 0 |   |   |
 *  |            |   512KB      |    |   |   |   | 1 | 1 |   |   |
 *  |-------------------------------------------------------------
 *  |            |     8        |    |   | 0 | 0 |   |   |   |   |
 *  | Redundant  |    16        |    |   | 0 | 1 |   |   |   |   |
 *  | Area Size  |   Reserved   |    |   | 1 | 0 |   |   |   |   |
 *  | (Byte/512) |   Reserved   |    |   | 1 | 1 |   |   |   |   |
 *  |-------------------------------------------------------------
 *  |Organization|       8      |    | 0 |   |   |   |   |   |   |
 *  |            |      16      |    | 1 |   |   |   |   |
  |   |
 *  |-------------------------------------------------------------
 *  | Reserved   |              |0or1|   |   |   |   |   |   |   |
 *  |-------------------------------------------------------------
 * CALL FUNC:
 * nandc_native_nand_prob() -
 *
 ********************************************************************************/
u32 nandc_native_get_spec(u8* id_data, struct nandc_spec *spec)
{
    struct nand_flash_dev *type = NANDC_NULL;
    u32 dev_id, maf_id;
    u32 writesize, erasesize, busw,maf_idx, oobsize = 0;
    FSZ chipsize;
    u8  cellinfo;

#ifdef WIN32
    id_data[0] = 0xEC; id_data[1] = 0xBC; id_data[2] = 0x00; id_data[3] = 0x55; id_data[4] = 0x44; /*2k 256MiB*/
    //id_data[0] = 0xEC; id_data[1] = 0xBC; id_data[2] = 0x00; id_data[3] = 0x66; id_data[4] = 0x56; /*4k 512MiB*/
    //id_data[0] = 0xEC; id_data[1] = 0xB1; id_data[2] = 0x00; id_data[3] = 0x55; id_data[4] = 0x40; /*2k 256MiB*/
    //id_data[0] = 0xAD; id_data[1] = 0xB1; id_data[2] = 0x00; id_data[3] = 0x55; /*2k 256MiB*/
    //id_data[0] = 0xEC; id_data[1] = 0xD7; id_data[2] = 0x94; id_data[3] = 0x72; id_data[4] = 0x54; id_data[5] = 0x42;/*8k 512MiB*/
    id_data[6] = id_data[0]; id_data[7] = id_data[1];
#endif

    maf_id = id_data[0];
    dev_id = id_data[1];

    /*Copy and modified from linux2.6.35 mtd code
    look up for the achieved ID in flash IDs table.*/
    type = nand_flash_ids;

    for (; type->name != NANDC_NULL; type++)
    {
        if (dev_id == (u32)type->id)
        {
            break;
        }
    }

    if (!type->name)
    {   /*lint -e778*/
        NANDC_TRACE( NFCDBGLVL(ERRO),("find dev by id  error ,maf_idx: 0x%02x, dev_id:0x%02x\n",maf_id, dev_id));
		/*lint +e778*/
		return NANDC_E_READID;
    }

    chipsize = (FSZ)type->chipsize << 20;

    /* Newer devices have all the information in additional id bytes */
    if (!type->pagesize)
    {
        int extid;
        /* The 3rd id byte holds MLC / multichip data */
        cellinfo = id_data[2];
        /* The 4th id byte is the important one */
        extid = id_data[3];

        /*
        * Field definitions are in the following datasheets:
        * Old style (4,5 byte ID): Samsung K9GAG08U0M (p.32)
        * New style   (6 byte ID): Samsung K9GAG08U0D (p.40)
        *
        * Check for wraparound + Samsung ID + nonzero 6th byte
        * to decide what to do.
        */
        if (id_data[0] == id_data[6] && id_data[1] == id_data[7] &&
            id_data[0] == NAND_MFR_SAMSUNG &&
            (cellinfo & NAND_CI_CELLTYPE_MSK) &&
            id_data[5] != 0x00)
        {
            /* Calc pagesize */
            writesize = 2048 << (extid & 0x03);
            extid >>= 2;
            /* Calc oobsize */
            //oobsize = (extid & 0x03) == 0x01 ? 128 : 218;
            if((extid & 0x03) == 0x01)
            {
                oobsize = 128;
            }
            else if((extid & 0x03) == 0x02)
            {
                oobsize = 218;
            }
            else if((extid & 0x03) == 0x03)
            {
                oobsize = 400;
            }
            else if(((extid & 0x03) == 0x00)&&((extid & 0x08) == 0x08))
            {
                oobsize = 436;
            }

            extid >>= 2;
            /* Calc blocksize */
            erasesize = (128 * 1024) <<(((extid >> 1) & 0x04) | (extid & 0x03));/*lint !e504 */
            busw = 0;
        }
		else if((id_data[0] == NAND_MFR_SAMSUNG)
			&& ((id_data[1] == 0xaa) || (id_data[1] == 0xbc)))
		{
            /* Calc pagesize */
            writesize = 1024 << (extid & 0x03);
            extid >>= 2;
            /* Calc oobsize */
		  oobsize = (8 << (extid & 0x03)) *
		  (writesize >> 9);

            extid >>= 2;
            /* Calc blocksize. Blocksize is multiples of 64KiB */
            erasesize = (64 * 1024) << (extid & 0x03);
            extid >>= 2;
            /* Get buswidth information */
            busw = (extid & 0x01) ? NAND_BUSWIDTH_16 : 0;
		}
		/* Hynix NF, 4k page size */
		else if((id_data[0] == NAND_MFR_HYNIX) && (id_data[1] == 0xbc)
			&& (id_data[0] == id_data[6]) && (id_data[1] == id_data[7])
			&& (id_data[5] != 0x00))
		{
			/* Calc pagesize */
			if(id_data[2] == 0x80)
			{
				writesize = 2014 << (extid & 0x03);
			}
			else
			{
				writesize = 2048 << (extid & 0x03);
			}
			extid >>= 2;
			/* Calc oobsize */
			if((extid & 0x13) == 0x00)
			{
				oobsize = 128;
			}
			else if((extid & 0x13) == 0x01)
			{
				if(id_data[2] == 0x80)
				{
					oobsize = 256;
#ifdef __KERNEL__
#else
					g_nand_ecc_bits = 4;
#endif
				}
				else
				{
					oobsize = 224;
				}
			}
			else if((extid & 0x13) == 0x13)
			{
				oobsize = 64;
			}
			extid >>= 2;
            /* Calc blocksize */
            if((extid & 0x08) == 0x00)
            {
            	if((extid & 0x03) == 0x03)
            	{
					erasesize = 768;
				}
				else
				{
					erasesize = (2048 * 64) << (extid & 0x03);
				}
            }
			else if((extid & 0x0b) == 0x08)
			{
				erasesize = 1024 * 1024;
			}
            /* Get buswidth information */
            busw = NAND_BUSWIDTH_16;
		}
        else
        {
            /* Calc pagesize */
            writesize = 1024 << (extid & 0x03);
            extid >>= 2;
            /* Calc oobsize */
            oobsize = (8 << (extid & 0x01)) *
            (writesize >> 9);
            extid >>= 2;
            /* Calc blocksize. Blocksize is multiples of 64KiB */
            erasesize = (64 * 1024) << (extid & 0x03);
            extid >>= 2;
            /* Get buswidth information */
            busw = (extid & 0x01) ? NAND_BUSWIDTH_16 : 0;
        }
    }
    else
    {
        /*
        * Old devices have chip data hardcoded in the device id table
        */
        erasesize = type->erasesize;
        writesize = type->pagesize;
        oobsize = writesize / 32;
        busw = type->options & NAND_BUSWIDTH_16;
    }

    /* Try to identify manufacturer */
    for (maf_idx = 0; nand_manuf_ids[maf_idx].id != 0x0; maf_idx++)
    {
        if (nand_manuf_ids[maf_idx].id == (int)maf_id)
        break;
    }

	/*
	 * Check, if buswidth is correct. Hardware drivers should set
	 * chip correct !
	 */

    NANDC_TRACE( NFCDBGLVL(WARNING),("NAND device: Manufacturer ID:"
        " 0x%02x, Chip ID: 0x%02x (%s %s)\n", maf_id, dev_id,
        nand_manuf_ids[maf_idx].name, type->name));
    /*-copy and modified from linux6.2.35 mtd code*/

    NANDC_TRACE( NFCDBGLVL(NORMAL),("NAND device: chipsize:0x%"FWC"x, pagesize:0x%x, \
 oobsize:0x%x, blocksize:0x%x, buswidth:0x%x \n", chipsize, writesize, oobsize, erasesize, busw));

#if (FEATURE_EPAD == FEATURE_ON)
    /*added by zkf52403*/
    spec->manuf = nand_manuf_ids[maf_idx].name;
    spec->name = type->name;
    memcpy(spec->devid, id_data, NANDC_READID_SIZE);
#endif
    spec->buswidth = busw;
    spec->blocksize = erasesize;
    spec->pagesize = writesize;
    spec->chipsize = chipsize;
#if (FEATURE_EPAD == FEATURE_OFF)
    spec->name = type->name;
#endif
    spec->sparesize = oobsize;

#ifdef __KERNEL__
#else
	g_nand_page_size = writesize;
#endif

    return NANDC_OK;
}

 /*******************************************************************************
 * FUNC NAME:
 * nandc_native_get_id() -  get id for the nand flash chip.
 *
 * PARAMETER:
 * @host - [input]the main structure.
 * @id_data - [input]where to store the id data.
 * @length - [input]the length of the id data.
 *
 *
 * DESCRIPTION:
 * This function gets id data.
 *
 *
 * CALL FUNC:
 * nandc_native_nand_prob() -
 *
 ********************************************************************************/
u32 nandc_native_get_id(struct nandc_host* host, u8* id_data, u32 length)
{
    u32 dev_id, maf_id;

    if(NANDC_OK != nandc_native_read_id( host,  id_data, length))
    {
        /*lint -e778 */
        NANDC_TRACE( NFCDBGLVL(ERRO),("read id  error1\n"));
        /*lint +e778 */
        NANDC_DO_ASSERT(0, "", 0 );
        return NANDC_E_READID;
    }

    maf_id = id_data[0];
    dev_id = id_data[1];

    /*read ID two times to insure the result for two times is consistent*/
    if(NANDC_OK != nandc_native_read_id( host,  id_data, length))
    {
        /*lint -e778 */
        NANDC_TRACE(NFCDBGLVL(ERRO),("read id length error2\n"));
        /*lint +e778 */
        NANDC_DO_ASSERT(0, HICHAR_NULL, 0 );
        return NANDC_E_READID;
    }

    if (id_data[0] != maf_id || id_data[1] != dev_id)
    {
        /*lint -e778*/
        NANDC_TRACE( NFCDBGLVL(ERRO), ("%s: second ID read did not match \
%02x,%02x against %02x,%02x\n", __func__, maf_id, dev_id, id_data[0], id_data[1]));
		/*lint +e778*/

		return NANDC_E_READID;
    }

    return NANDC_OK;
}

 /*******************************************************************************
 * FUNC NAME:
 * nandc_native_nand_prob() - reads ids for all nand flash chips and get flash information
 *                             from flash table according to flash id
 *
 * PARAMETER:
 * @host - [input]the main struction.
 *
 *
 * DESCRIPTION:
 * This function reads ids for all nand flash chips if host->chipmax is not 1.
 * and get flash information from flash table according to flash id.
 *
 *
 * CALL FUNC:
 * nandc_nand_init() -
 *
 ********************************************************************************/
u32 nandc_native_nand_prob(struct nandc_host* host)
{
    struct nandc_spec spec;
    u8 id_data[NANDC_READID_SIZE];
    u32 addnum = 0;

    /*host->chipmax = NANDC_MAX_CHIPS*/
    for(host->chipselect = 0 ; host->chipselect < host->chipmax ; host->chipselect++)
    {
        nandc_native_reset_chip(host, host->chipselect);
        if(NANDC_OK ==  nandc_native_get_id(host, id_data, sizeof(id_data)))
        {
            if(NANDC_OK == nandc_native_get_spec(id_data, &spec))
            {
                addnum++;
            }
            else
            {
                /*lint -e778*/
                NANDC_TRACE( NFCDBGLVL(ERRO),("nandc_native_get_spec error:chip_select = %d id[0]:0x%x, id[1]:0x%x\n",
                                        host->chipselect, id_data[0], id_data[1]));
				/*lint +e778*/
				return NANDC_E_INIT;
            }
        }
    }

    if(addnum > 0)
    {
        /* 配置NAND高8bit管脚复用,只在BootLoader中配置一次 */
#ifdef COMPILE_BOOTLOADER
        if(NANDC_OK != nandc_io_bitwidth_set(spec.buswidth))
        {
            return NANDC_ERROR;
        }
#endif
        return  nandc_host_add_nand_chip(host, addnum, &spec);
    }
    else
    {
        return NANDC_ERROR;
    }
}

 /*******************************************************************************
 * FUNC NAME:
 * nandc_native_host_delete() -
 *
 * PARAMETER:
 * @host - [input]
 *
 *
 * DESCRIPTION:
 *
 *
 *
 * CALL FUNC:
 * () -
 *
 ********************************************************************************/
void nandc_native_host_delete(struct nandc_host* host)
{

#ifdef WIN32
    if(host->debuger)
        hiwin_regdebug_delete(host->debuger);
#endif

    HI_NOTUSED(host);

}

 /*******************************************************************************
 * FUNC NAME:
 * nandc_native_host_create() - initialize the main struct nandc_host.
 *
 * PARAMETER:
 * @init_info - [input]configuration for corresponding nand flash controller.
 *              v200 : nandc2_init_info
 *              v320 : nandc3_init_info
 *              v400 : nandc4_init_info
 *
 * DESCRIPTION:
 * This function mallocs space for the main struct nandc_host and initializes it.
 *
 * CALL FUNC:
 * nandc_nand_create_host() -
 *
 ********************************************************************************/
struct nandc_host* nandc_native_host_create(struct nandc_init_info* init_info)
{
    struct nandc_host *host = NANDC_NULL ;
    struct nandc_ctrl_desc*  ctrlspec = NANDC_NULL;


    host = (struct nandc_host *)himalloc(sizeof(struct nandc_host));

    NANDC_REJECT_NULL((u32)host & (u32)init_info);
    NANDC_REJECT_NULL((u32)init_info->ctrlspec
                      &(u32)init_info->ctrlfunc
                      &(u32)init_info->eccinfo
                      &(u32)init_info->bitstbl
                      &(u32)init_info->dmaplookup
                     );
    memset((void*)host,0x00,sizeof(struct nandc_host)); /*clean the pclint e516*/

    hiout(("Hisilicon %s initialize...\r\n",(char*)init_info->ctrlspec->ctrlname));


    ctrlspec = init_info->ctrlspec;

    host->bufbase   =   (void __iomem *)ctrlspec->buffbase;
    host->regbase   =   (void __iomem *)ctrlspec->regbase;
    host->chipmax   =   ctrlspec->maxchip;
    host->ctrlfunc  =   init_info->ctrlfunc;
    host->eccinfo   =   init_info->eccinfo;
    /*host->ctrlspec  =   init_info->ctrlspec;*/
    host->dmaplookup=   init_info->dmaplookup;
    host->regsize   =   ctrlspec->regsize;
    host->bufsize   =   ctrlspec->buffsize;
    host->latched   =   nandc_latch_none;


    host->buffers = (struct nand_buffers*)himalloc(sizeof(struct nand_buffers));
    NANDC_REJECT_NULL(host->buffers );
    memset((void*)(host->buffers), 0xff, sizeof(struct nand_buffers)); /*clean the pclint e516*/

#ifdef WIN32
    host->debuger   =  hiwin_regdebug_create(ctrlspec->regsize, ctrlspec->regbase, ctrlspec->ctrlname);
    NANDC_REJECT_NULL(host->debuger );
    host->bufbase   = malloc(nandc_alige_size(ctrlspec->buffsize, 0x100000));
    NANDC_REJECT_NULL(host->bufbase );
    host->bufbase   = (void __iomem *)nandc_alige_size((u32)host->bufbase, 0x100000);
    hiwin_regdebug_init_buf((u32)host->bufbase,  ctrlspec->buffsize);
	memset((void*)host->bufbase, 0xff, 2);
	memset((void*)((u32)host->bufbase + 2048), 0xff, 2);
#endif

    if(NANDC_OK != nandc_host_init_cluster(host,  ctrlspec->regdesc, init_info->bitstbl))
    {
        goto ERRO;
    }

    NANDC_TRACE( NFCDBGLVL(NORMAL), ("nandc_native_host_create ok\n"));

     return host;

ERRO:

    /*modified for lint e613 e429 */
    if ((host != NANDC_NULL) && (host->buffers != NANDC_NULL))
    {
        hifree((void*)host->buffers);
    }

    if (host != NANDC_NULL)
    {
        hifree((void*)host);
    }
    /*modified for lint e613 */

    NANDC_TRACE( NFCDBGLVL(ERRO), ("nandc_native_host_create error\n"));/*lint !e778*/

    return NANDC_NULL;

}

 /*******************************************************************************
 * FUNC NAME:
 * nandc_native_find_partition() - find partition according to address in total address space.
 *
 * PARAMETER:
 * @host - [input]the main struction.
 * @globaddr - [input]address in total address space for all nand flash chips if
 *                   there are two or more nand flash chip.
 *
 *
 * DESCRIPTION:
 * This function finds partition according to address in total address space.
 *
 *
 * CALL FUNC:
 * nandc_native_location_by_pagenum() -
 * nandc_native_find_partition() -
 *
 ********************************************************************************/
u32 nandc_native_find_partition(struct nandc_host* host, FSZ globaddr)
{
    struct mtd_partition* partition;
    u32 curpart;
    u32 nparts;

    partition = host->usrdefine->partition;
    nparts  = host->usrdefine->nbparts;

    for(curpart = 0 ; curpart < nparts ; curpart++)
    {
        if((globaddr >= partition->offset)&&(globaddr < partition->offset + partition->size))
        {
            host->curpart = partition;
            break;
        }
        partition++;
    }

    if(curpart == nparts)
    {
        /*lint -e778 */
        NANDC_TRACE( NFCDBGLVL(ERRO), ("partition not find, address:0x%x\n",globaddr));
        /*lint +e778 */
        return NANDC_E_NOFIND;
    }

    return NANDC_OK;

}

 /*******************************************************************************
 * FUNC NAME:
 * nandc_native_location_by_blocknum() - fix on opeation location by block number.
 *
 * PARAMETER:
 * @host - [input]the main structure.
 * @blknum - [input]block number in total address space for all nand flash chips if
 *                   there are two or more nand flash chip.
 *
 * DESCRIPTION:
 * This function fixs on opeation location by block number(there may be more than one nand
 * flash chip).
 *
 * CALL FUNC:
 * nandc_nand_flag_address() -
 * nandc_nand_erase_block() - erase function for erasing operation.
 *
 ********************************************************************************/
u32 nandc_native_location_by_blocknum(struct nandc_host* host, u32 blknum)
{
    struct nandc_nand* nand = host->nandchip;
    struct nandc_spec* spec = &nand->spec;

    host->addr_real= (blknum & spec->blockmask) << spec->blockshift;

    host->chipselect = blknum >> (spec->chipshift - spec->blockshift);

    return NANDC_OK;/*nandc_bbm_real_addr(host->addr_logic, &host->addr_real);*/

}
#if !NANDC_COMPILER(NANDC_READ_ONLY)
 /*******************************************************************************
 * FUNC NAME:
 * nandc_native_location_by_pagenum() - fix on opeation location by page number.
 *
 * PARAMETER:
 * @host - [input]the main structure.
 * @pagenum - [input]page number in total address space for all nand flash chips if
 *                   there are two or more nand flash chip.
 *
 * DESCRIPTION:
 * This function  fixs on opeation location by page number.
 *
 * CALL FUNC:
 * none
 *
 ********************************************************************************/
u32 nandc_native_location_by_pagenum(struct nandc_host* host, u32 pagenum)
{
    struct nandc_nand* nand = host->nandchip;
    struct nandc_spec* spec = &nand->spec;
    FSZ  globaddr ;


    host->addr_logic = (pagenum & spec->pagemask) << spec->pageshift;

    host->chipselect = pagenum >> (spec->chipshift - spec->pageshift);

    globaddr = pagenum << spec->pageshift;

    if(NANDC_OK != nandc_native_find_partition( host, globaddr))
    {
        goto ERRO;
    }

    return nandc_bbm_real_addr(host->addr_logic, &host->addr_real);

ERRO:

    return NANDC_ERROR;
}
#endif
 /*******************************************************************************
 * FUNC NAME:
 * nandc_native_location_by_address() - fix on opeation location by adderss.
 *
 * PARAMETER:
 * @host - [input]the main structure.
 * @globaddr - [input]address in total address space for all nand flash chips if
 *                   there are two or more nand flash chip.
 *
 *
 * DESCRIPTION:
 * This function  fixs on opeation location by adderss
 *
 * CALL FUNC:
 * nandc_nand_mass_read() - read function for mass data reading operation.
 * nandc_nand_mass_write() - write function for mass data writing operation.
 *
 ********************************************************************************/
u32 nandc_native_location_by_address(struct nandc_host* host, FSZ globaddr )
{
    struct nandc_nand* nand = host->nandchip;
    struct nandc_spec* spec = &nand->spec;

    host->addr_logic = globaddr & (spec->chipsize - 1);

    host->chipselect = globaddr >> spec->chipshift;

    if(NANDC_OK != nandc_native_find_partition( host, globaddr))
    {
        goto ERRO;
    }

    return nandc_bbm_real_addr(host->addr_logic, &host->addr_real);

ERRO:

    return NANDC_E_NOFIND;
}


#ifdef __cplusplus
}
#endif
