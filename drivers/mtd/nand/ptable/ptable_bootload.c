/*******************************************************************************
* Copyright (C), 2008-2011, HUAWEI Tech. Co., Ltd.
*
* Module name: flash partition table
*
* Version: v1.0
*
* Filename:    ptable_bootload.c
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
/*******************************问题单修改记录********************************
日期            问题单号            修改人          修改内容
******************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

#include <strlib.h>/*lint -e322 */
#include "ptable_def.h"
#include "product_config.h"
#include "BSP_GLOBAL.h"
#include "usrApp.h"
#include "BSP_DRV_WDT.h"
#include "uart.h"
#if (FEATURE_POWER_ON_OFF == FEATURE_ON)
#include "powerOn.h"
#endif
#include "BSP_DLOAD.h"
#include "BSP_MEMORY.h"
#include "nandc_balong.h"
#include "FeatureConfig.h"
#include "inflateLib.h"

/*If bootrom loaded for firmware update, light lcd on.*/
#if (FEATURE_E5 == FEATURE_ON)
#include "bootload_lcd.h"
#endif

#include "BOOT_LED.h"
#include "ptable_inc.h"

#include "ddmExtern.h"

typedef void (*preprocess_func)(void);

extern BSP_BOOL IsBootromStart(BSP_U32 *version);
extern BSP_VOID setSoftLoad (BSP_BOOL bSLoadFlag);
extern BSP_VOID setCurMode(DLOAD_MODE_E eDloadMode);
extern BSP_VOID startUpExcHandler(EXCEPTION_HANDLE_TYPE_E enExcep);
extern BSP_BOOL isWarmStart(BSP_VOID);
extern BSP_VOID setTFUpdateFlag(BSP_BOOL bFlag);
extern BSP_VOID activeAppCore(BSP_VOID);
extern struct ST_PART_TBL * ptable_find_by_type(u32 part_type, struct ST_PART_TBL * ptable);
extern struct ST_PART_TBL * ptable_get_ram_data(void);
#if(FEATURE_HANDSET_SECURITY_BOOT == FEATURE_ON)
extern u32 security_image_check( u32 type_id,  u32 buffe,  u32 size);
extern u32 ptable_sec_check_acore(void);
#endif
extern BSP_VOID secSetUsbLoad (BSP_VOID);

u32 empty_data = 0xffffffff;
/******************************************************************************************
 * FUNC NAME:  
 * ptable_load_image() - external API:  
 *                           
 * PARAMETER: 
 * @name          -[input] indicate witch parititon to quary.            
 * @flash_addr    -[input] flash start address to read data          
 * @ram_addr      -[input] ram start address to write data 
 * @read_size     -[input] size to load          
 *
 * DESCRIPTION:
 *     load image data from flash to ram         
 *
 * CALL FUNC:
 * 
 *****************************************************************************************/
u32 ptable_load_image(struct ST_PART_TBL *part)
{
    FSZ flash_addr = part->offset;
    u32 ram_addr   = part->loadaddr;
    u32 read_size  = part->loadsize;
    u32 type_id    = part->image;

    IMAGE_HEAD_S *pimage_head;
    u32 ram_inflate_addr;

    u32 ret;

    BSPLOGSTR(">>loading: ");
    BSPLOGSTR(part->name);
    BSPLOGSTR(" ... ");

    if(DATA_INVALID == ptable_get_validity(part))
    {
        BSPLOGSTR("Err!.partitin data is invalide\r\n");
        return NAND_ERROR;
    }

    /* BootRom 的镜像需要解压，单独处理 */
    if (IMAGE_BOOTROM == type_id)
    {
        /* 读取Head，获取镜像长度 */
        if (NAND_OK == nand_read((FSZ)flash_addr, (u32)ram_addr, sizeof(IMAGE_HEAD_S), NULL))
        {
            /* 尝试解压 */
            BSPLOGSTR("try inflate.\r\n");

            pimage_head = (IMAGE_HEAD_S *)ram_addr;
            ram_inflate_addr = PRODUCT_CFG_MCORE_RAM_HIGH_ADDR - (BLK_HDR_SIZE + BUF_SIZE) - pimage_head->ulImgLen;

            BSPLOGSTR("image length: ");
            BSPLOGU32(pimage_head->ulImgLen);

            BSPLOGSTR("\r\nram_inflate_addr: ");
            BSPLOGU32(ram_inflate_addr);

            if (NAND_OK == nand_read((FSZ)sizeof(IMAGE_HEAD_S) + flash_addr, (u32)ram_inflate_addr, pimage_head->ulImgLen, NULL))
            {
                BSPLOGSTR("\r\ninflating...");
                ret = (u32)inflate((unsigned char*)(ram_inflate_addr),
                    (unsigned char*)(part->entry), (int)pimage_head->ulImgLen);
                BSPLOGSTR("\r\nreturn value: ");
                BSPLOGU32(ret);

                /* 解压过程未出错则返回，否则自动转入正常读取 */
                if (!ret)
                {
                    BSPLOGSTR("\r\ninflate success! ");
                    /*if flash has no data, return null*/
                    if((IMAGE_MCORE == type_id) && (0 == memcmp((void *)part->loadaddr, &empty_data, sizeof(empty_data))))
                    {
                        BSPLOGSTR("but Kernel is 0xFFFFFFFF, ERROR, Now try bootrom!\r\n");
                        return NAND_ERROR;
                    }
                    return ret;
                }
                else
                {
                    BSPLOGSTR("\r\nFail to inflate, turn to normal read...");
                    /* 这里不需要返回，自动转入正常读取 */
                }
            }
            else
            {
                BSPLOGSTR("FAIL!!.\r\n");
                return NAND_ERROR;
            }
        }
    }


    /* 读取未压缩的镜像 */
    if(NAND_OK == nand_read((FSZ)flash_addr, (u32)ram_addr, (u32)read_size, NULL))
    {
        BSPLOGSTR("OK.\r\n");
		
		/*if flash has no data, return null*/
        if((IMAGE_MCORE == type_id) && (0 == memcmp((void *)part->loadaddr, &empty_data, sizeof(empty_data))))
        {
            BSPLOGSTR("but Kernel is 0xFFFFFFFF, ERROR, Now try bootrom!\r\n");
            return NAND_ERROR;
        }
		
        #if (FEATURE_HANDSET_SECURITY_BOOT == FEATURE_ON)
            return security_image_check( type_id,  ram_addr,  read_size);
        #else
            return NAND_OK;
        #endif
    }
    else
    {
        BSPLOGSTR("FAIL!!.\r\n");
        return NAND_ERROR;
    }
}

/******************************************************************************************
 * FUNC NAME:  
 * ptable_load_bootrom() - external API:  
 *                           
 * PARAMETER: 
 * @boot_flag   -[output] store bootrom entry address.                
 *
 * DESCRIPTION:
 *     load image data from flash to ram , when boot mode is download mode(bootrom only)     
 *
 * CALL FUNC:
 *****************************************************************************************/
u32 ptable_load_bootrom(u32 boot_flag)
{
    int cnt = 0;
    struct ST_PART_TBL *part = NULL;

    part = (struct ST_PART_TBL *)ptable_get_ram_data();

    /*找到最新的bootrom分区*/
    part = ptable_find_by_type((u32)IMAGE_BOOTROM, part);

TRY_PART:
    if(part)
    {
        cnt++;
        
        BSPLOGSTR("IMAGE_BOOTROM load from:0x");
        BSPLOGU32((int)part->offset);

        if(NAND_OK == ptable_load_image(part))
        {
            /*if flash has no data, return null*/
            if(0 == memcmp((void *)part->loadaddr, &empty_data, sizeof(empty_data)))
            {
                BSPLOGSTR("error data is 0xffffffff!!");
                if(cnt>IMAGE_BOOTROM_MAX_NUM)
                {
                    BSPLOGSTR("all bootrom error\r\n");
                    return 0;
                }
                BSPLOGSTR("try the other...\r\n");
                part = ptable_find_theother_by_type((u32)IMAGE_BOOTROM, part);
                goto TRY_PART;
            }
            else
            {
                BSPLOGSTR("data check OK!\r\n");
                return part->entry;
            }
        }
        else
        {}
    }
    else
    {
        BSPLOGSTR("error:IMAGE_BOOTROM not find. \r\n");
    }

    return 0;
}

/******************************************************************************************
 * FUNC NAME:  
 * ptable_load_kernel() - external API:  
 *                           
 * PARAMETER: 
 * @fastboot_entry   -[output] store A core fastboot entry address.                
 *
 * RETURN:
 * 
 *  VxWorks entry address
 *
 * DESCRIPTION:
 *     load image data from flash to ram ,when boot mode is normal boot (C core + A core)        
 *
 * CALL FUNC:
 *****************************************************************************************/
u32 ptable_load_kernel(u32 * fastboot_entry)
{
    u32 run_addr = 0;
    struct ST_PART_TBL *part = NULL;

    part = (struct ST_PART_TBL *)ptable_get_ram_data();

    BSPLOGSTR("Start from: vxWorks Kernel.\r\n");

    /*poll search partition talbe*/
    while(0 != strcmp(PTABLE_END_STR, part->name))
    {
        /*find and load excutable image first*/
        if(IMAGE_MCORE == part->image )
        {   
            if(NAND_OK == ptable_load_image(part))
            {
                run_addr = part->entry;
            }
			else
			{
			    run_addr = 0;
                goto EXIT;
            }
        }
        
        /*load some image must load in bootload*/
        else if(PTABLE_FLAG_AUTOLOAD == (PTABLE_FLAG_AUTOLOAD & part->property))
        {
            ptable_load_image(part);
        }
        /*load A-core fastboot to ram */
        else if(IMAGE_FASTBOOT == part->image )
        {
            if(NAND_OK == ptable_load_image(part))
            {
                *fastboot_entry = part->loadaddr;
            }
            else
            {
                *fastboot_entry = 0;
            }
        }
        else
        {}

        part++;
    }

EXIT:
    
    return run_addr;
        
}

/******************************************************************************************
 * FUNC NAME:  
 * ptable_boot_flag() - external API:  
 *                           
 * PARAMETER: 
 * @is_warn_start   -[input] tell it is warn start or cold start                
 *
 * RETURN:
 *     system start type
 *
 * DESCRIPTION:
 *     get system  start type by board start type and softload flag    
 *
 * CALL FUNC:
 *****************************************************************************************/
u32 ptable_boot_flag(u32 is_warm_start)
{
    DLOAD_VERSION_INFO_S *pstVersionInfo = (DLOAD_VERSION_INFO_S *)MEMORY_AXI_DLOAD_AUTOINSTALL_ADDR;
    u32 soft_load_magic;

    /*warn start or cold start*/
	if(IsBootromStart(&soft_load_magic))
    { 
        return soft_load_magic;
    }

    if(is_warm_start) /* 热启动 */
    {
        /* warm start count +1 */
        pstVersionInfo->u32WdgRstCnt++;

        /* try start, if fails more then STARTUP_TRY_TIMES, swich to bootrom*/
        if(STARTUP_TRY_TIMES <= pstVersionInfo->u32WdgRstCnt)
        {
            BSPLOGSTR("ERROR:too many fails ,VXWORKS region is damaged, switch to BOOTROM.\r\n");
            pstVersionInfo->u32WdgRstCnt = 1;
            setSoftLoad(BSP_TRUE);  /* 保证如果此次起不来，能再次进入BOOTROM */

            startUpExcHandler(EXCEPTION_HANDLE_TYPE_REBOOT);
        }
    }
    
    /*start form vxorks*/
    return SOFTLOAD_NORMAL;
}

/******************************************************************************************
 * FUNC NAME:  
 * ptable_activate_fastboot() - external API:  
 *                           
 * PARAMETER: 
 * @fast_boot_start_addr   -[input] A core fastboot entry address.                
 *
 * RETURN:
 *    none
 *
 * DESCRIPTION:
 *     load image data from flash to ram ,when boot mode is normal boot (C core + A core)        
 *
 * CALL FUNC:
 *****************************************************************************************/
void ptable_activate_fastboot(u32 fast_boot_start_addr)
{
    /* 设置从核启动地址*/
	BSPLOGSTR("activate_fastboot...0x");
    BSPLOGU32((int)fast_boot_start_addr);
	BSPLOGSTR("\r\n");
    
#if (defined (BOARD_FPGA_P500))
    BSP_REG_WRITE(0x80000000, 0xb4, (unsigned int)fast_boot_start_addr);
#elif (defined (BOARD_SFT)||defined (BOARD_ASIC)||defined (BOARD_ASIC_BIGPACK))
    BSP_REG_WRITE(0x90000000, 0x414, (unsigned int)fast_boot_start_addr);
#endif
    
    activeAppCore();
}

/******************************************************************************************
 * FUNC NAME:  
 * ptable_activate_fastboot() - external API:  
 *                           
 * PARAMETER: 
 * @entry   -[input] C-core entry             
 *
 * RETURN:
 *    none
 *
 * DESCRIPTION:
 *     goto vxworks kernel       
 *
 * CALL FUNC:
 *****************************************************************************************/
void ptable_start_vxworks(funcptr entry)
{    
	BSPLOGSTR("Starting from entry: 0x");
    BSPLOGU32((int)entry);
    BSPLOGSTR("\r\n");

    (void)(entry)();
}

/******************************************************************************************
 * FUNC NAME:  
 * ptable_activate_fastboot() - external API:  
 *                           
 * PARAMETER: 
 * @entry   -[input] C-core entry             
 *
 * RETURN:
 *    none
 *
 * DESCRIPTION:
 *     goto vxworks kernel       
 *
 * CALL FUNC:
 *****************************************************************************************/
void ptable_start_flag(u32* boot_flag)
{    
    u32 warm_start;
#if( FEATURE_POWER_ON_OFF == FEATURE_ON ) 
    BOOT_IMAGE_LOAD_TYPE_ENUM  eLoadType = BOOT_IMAGE_LOAD_VXWORKS;
#endif    

    /*find warm start or cold start*/
    warm_start = (u32)isWarmStart();

    /*find whether start from vxworks kernel or bootrom*/
    *boot_flag = ptable_boot_flag(warm_start);

/*If bootrom loaded for firmware update, light lcd on.*/
#if (FEATURE_E5 == FEATURE_ON)
	if (SOFTLOAD_MAGIC == *boot_flag)
	{
	    LcdPowerOn();
	    LedPowerOn();
	}
#endif

#if( FEATURE_POWER_ON_OFF == FEATURE_ON )
    if(SOFTLOAD_NORMAL == *boot_flag)
    {   
        /* check the boot condition before load the vxWorks/Linux. 
           Not check the startup reason when enter bootrom.      */
        eLoadType = boot_power_on_check( );
		if( BOOT_IMAGE_LOAD_BOOTROM == eLoadType )
		{
			#if(FEATURE_SDUPDATE == FEATURE_ON)
			setTFUpdateFlag(BSP_TRUE);
			#endif

		    *boot_flag = SOFTLOAD_MAGIC;
		}
		
        /* check the boot condition before load the vxWorks/Linux. 
           if loadvxWorks/Linux start power_led flash mode.      */
        if( BOOT_IMAGE_LOAD_VXWORKS == eLoadType )
        { 
			#if (FEATURE_E5 == FEATURE_ON)
            POWER_led_flash_init();                   /*Power_led Flash mode init*/
            #endif
        }
    }
#endif
}

/******************************************************************************************
 * FUNC NAME:  
 * ptable_bootload() - external API:  
 *                           
 * PARAMETER: 
 * @entry   -[input] C-core entry             
 *
 * RETURN:
 *    none
 *
 * DESCRIPTION:
 *     goto vxworks kernel       
 *
 * CALL FUNC:
 *****************************************************************************************/
void ptable_bootload(preprocess_func startup_preprocess)
{
    u32 boot_flag = SOFTLOAD_NORMAL;
    u32 c_core_entry = 0;
    u32 a_core_entry = 0;
    u32 ret_val = OK;

    ptable_start_flag(&boot_flag);

    if(SOFTLOAD_NORMAL == boot_flag)
    {   
        /* load vxWorks kernel and loadable image*/
		ddmPhaseScoreLoad("start load_kernel",__LINE__);
        c_core_entry = ptable_load_kernel(&a_core_entry);
		ddmPhaseScoreLoad("end load_kernel",__LINE__);
        if(0 == c_core_entry)
        {
            #if(FEATURE_HANDSET_SECURITY_BOOT == FEATURE_ON)
            resetEnterHsuartBoot();
            #else        
            /*load (bootrom A or B)*/
            c_core_entry = ptable_load_bootrom(boot_flag);
            /* 如果BootRom不存在，则进入usb自举 */
            if(0 == c_core_entry)
            {
                secSetUsbLoad();
                startUpExcHandler(EXCEPTION_HANDLE_TYPE_REBOOT);
            }
            /*set NULL，don't start A core*/
            #endif
            a_core_entry = 0;
        }
    }
    else
    {
        /*load (bootrom A or B)*/
        c_core_entry = ptable_load_bootrom(boot_flag);
        /* 如果BootRom不存在，则进入usb自举 */
        if(0 == c_core_entry)
        {
            secSetUsbLoad();
            startUpExcHandler(EXCEPTION_HANDLE_TYPE_REBOOT);
        }
        /*set NULL，don't start A core*/
        a_core_entry = 0;
    }

    #if (FEATURE_HANDSET_SECURITY_BOOT == FEATURE_ON)
    if(0 != a_core_entry)
    {        
        ret_val = ptable_sec_check_acore();        
    }
    #endif

    if(0 != c_core_entry)
    {
        if(startup_preprocess)
        {
            startup_preprocess();
        }
        
        /* EM定制点灯需求，SBM后期需要想法隔开 */
		#if (FEATURE_E5 == FEATURE_ON)
        BOOT_led_task_end();            /*END power_led flash mode*/
		#endif

    	if(0 != a_core_entry)
    	{
        	/*activate A core's fastboot to load linux kernel */
        	if(OK == ret_val)
        	{
            	ptable_activate_fastboot(a_core_entry);
        	}
        	else
        	{
        	}
    	}
        /*run vxWorks kernel*/
    	ptable_start_vxworks((funcptr)c_core_entry);
    }

    startUpExcHandler(EXCEPTION_HANDLE_TYPE_REBOOT);
}

void BSP_LOG_STR(const char * pstr)
{
    BSPLOGSTR(pstr);
}


#ifdef __cplusplus
}
#endif

