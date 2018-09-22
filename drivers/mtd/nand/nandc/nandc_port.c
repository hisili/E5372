/*******************************************************************************
* Copyright (C), 2008-2011, HUAWEI Tech. Co., Ltd.
*
* Module name: Hisilicon Nand Flash Controller Dirver
*
* Description: the Hisilicon Nand Flash Controller physical and logical driver
*
* Version: v1.0
*
* Filename:    nandc_port.c
* Description: nand controller operations in dependence on  hardware
*
* Function List:
*
* History:
* 1.date:2011-07-27
* question number:
* modify reasion:         create
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

#include "nandc_inc.h"

/*lint -e767*/
#define NFCDBGLVL(LVL)      (NANDC_TRACE_PORT|NANDC_TRACE_##LVL)
/*lint +e767*/


#ifdef __FASTBOOT__
#if 0 /*fast boot overlap 128K*/
static int block_buffer[(NANDC_MAX_BLOCK_MASK + 1)/4];
#else
static int block_buffer[1];
#endif
#endif

/********************************************************************************************
 * FUNC NAME:  
 * nandc_get_block_buf() - get the ram buffer.
 *
 * PARAMETER:
 * @size - [input]the size of buffer to be requested.
 *
 * DESCRIPTION:
 * This function requests ram buffer through different interface according to differernt platform.
 * the platform as follow:
 * WIN32           Window platform :just for inspection of C source codes syntax 
 * __KERNEL__      Linux platform : target kernel nand driver code for Linux OS.
 * __VXWORKS__     Vxworks platform : target nand driver code for Vxworks OS.
 * __FASTBOOT__    Android platform : target nand driver code for Android fastboot.
 * __BOOTLOADER__  Vxworks platform : target nand driver code for bootloader of Vxworks OS.
 * __RVDS__        Realview and Trace 32 :nand driver code for download tools.
 * 
 * CALL FUNC:
 * nandc_nand_mass_write() -
 * 
 ********************************************************************************************/
void* nandc_get_block_buf(u32 size)
{
#if defined(WIN32) 
    return malloc(size);

#elif defined(__KERNEL__) || defined(__VXWORKS__)

    return (void*)himalloc((size_t)size); /*clean the pclint e516*/

#elif defined(__FASTBOOT__)

    if(size <= sizeof(block_buffer))
    {
        return (void*)&block_buffer[0];
    }

    NANDC_TRACE(NFCDBGLVL(NORMAL), ("nandc_get_block_buf: buffer wanted(0x%x = %d), buffer size (0x%x = %d)!\n", 
                            size, size,  sizeof(block_buffer), sizeof(block_buffer)));
    NANDC_DO_ASSERT(0,"critical error: too small !\n",0);
    return NANDC_NULL;
    
#elif defined(__RVDS__)

     return (void*)nandc_block_buffer;

#elif defined(__BOOTLOADER__)

    NANDC_DO_ASSERT(0,"critical error: bootload should not use block buffer!\n",0);
    return NANDC_NULL;
#else
#error palnt MACRO not defined!
#endif

}

/*********************************************************************************************
 * FUNC NAME:  
 * nandc_give_block_buf() - release the ram buffer.
 *
 * PARAMETER:
 * @p - [input] the ram buffer pointer.
 *
 * DESCRIPTION:
 * This function releases ram buffer through different interface according to differernt platform.
 * the platform as follow:
 * WIN32           Window platform :just for inspection of C source codes syntax 
 * __KERNEL__      Linux platform : target kernel nand driver code for Linux OS.
 * __VXWORKS__     Vxworks platform : target nand driver code for Vxworks OS.
 * __FASTBOOT__    Android platform : target nand driver code for Android fastboot.
 * __BOOTLOADER__  Vxworks platform : target nand driver code for bootloader of Vxworks OS.
 * __RVDS__        Realview and Trace 32 :nand driver code for download tools.
 * 
 * 
 * CALL FUNC:
 * nandc_nand_mass_write() - 
 * 
 ********************************************************************************************/
void nandc_give_block_buf(void * p)
{
#if defined(WIN32) 

    free(p);

#elif defined(__KERNEL__) || defined(__VXWORKS__)

    hifree((void*)p); /*clean the pclint e516*/

#elif defined(__FASTBOOT__)
    /*use static array , do not need free*/    
#elif defined(__RVDS__)

    /*use const ddr addrss , do not need free*/    
#elif defined(__BOOTLOADER__)

    NANDC_DO_ASSERT(0,"critical error: bootload should not use block buffer!\n", 0);
#else

#error palnt MACRO not defined!
#endif   

   HI_NOTUSED(p);   /*lint !e449 */

}


#if NANDC_COMPILER(NANDC_USE_MEMPOOL) 

#ifdef WIN32
#define NANDC_POOL_SIZE   (10400 + NANDC_MAX_BLOCK_MASK + 1024)
#else
#define NANDC_POOL_SIZE   (10400 + 1024)
#endif

static int nandc_pool[NANDC_POOL_SIZE/4] = {0};


static u32   poolcur = NANDC_NULL; 
static u32   poolend = NANDC_NULL; 
/*******************************************************************
 * FUNC NAME:  
 * nandc_minit() - initalize memory poll address.
 *
 * PARAMETER:
 * none
 *
 * DESCRIPTION:
 * This function is called only when NANDC_USE_MEMPOOL is select.
 * 
 * CALL FUNC:
 * nand_init_clear() -
 * 
 ******************************************************************/
void nandc_minit(void)
{
    poolcur = (u32)nandc_pool;
    poolend = (u32)&nandc_pool[0]  +   sizeof(nandc_pool); 
}

/***************************************************************************
 * FUNC NAME:  
 * nandc_malloc() - malloc memory space from static array.
 *
 * PARAMETER:
 * @size - [input] the size of requested memory space.
 *
 * DESCRIPTION:
 * This function is called only when NANDC_USE_MEMPOOL is select.
 * 
 * CALL FUNC:
 * himalloc () - malloc function for __BOOTLOADER__,__RVDS__,__FASTBOOT__.
 * 
 ***************************************************************************/
void* nandc_malloc(u32 size)
{
    void*   retbuff = NANDC_NULL;
    u32     masize, thisend;

	if(NANDC_NULL == poolcur)
	{
		nandc_minit();
	}
	

    masize = nandc_alige_size(size, sizeof(int));
    
    thisend = poolcur + masize;

    if(thisend <= poolend)
    {
        retbuff = (void*)poolcur;
        poolcur = thisend;
        NANDC_TRACE(NFCDBGLVL(NORMAL), ("nandc_malloc: pool wanted(0x%x = %d), used (0x%x = %d), left(0x%x = %d)!\n", 
                            masize, masize, poolcur - (u32)&nandc_pool[0], poolcur - (u32)&nandc_pool[0],
                            poolend - poolcur, poolend - poolcur));
    }
    else
    {
       NANDC_TRACE(NFCDBGLVL(ERRO), ("nandc_malloc: pool(%d),used(%d),wanted(%d) exhaust!\n",
                                    sizeof(nandc_pool),poolcur - (u32)&nandc_pool[0], masize));
       NANDC_DO_ASSERT(0,"critical error: nandc_malloc pool size too small!\n",0 );

    }

    return  retbuff;

}

/*********************************************************************************
 * FUNC NAME:  
 * nandc_free() -  free memory space which is malloced by static array.
 *
 * PARAMETER:
 * @p - [input] the ram buffer pointer.
 *
 * DESCRIPTION:
 * This function is called only when NANDC_USE_MEMPOOL is select.
 * 
 * CALL FUNC:
 * hifree () - memory free function for __BOOTLOADER__,__RVDS__,__FASTBOOT__.
 * 
 ********************************************************************************/
void nandc_free(void* p)
{
    HI_NOTUSED(p);
    NANDC_TRACE(NFCDBGLVL(ERRO),("error: nandc_free chould never be called!\n"));
}

#endif /*NANDC_USE_MEMPOOL*/


#if NANDC_COMPILER(NANDC_MULTITASK)
/******************************************************************************
 * FUNC NAME:  
 * nandc_mutex_create() - create semaphore for platform which has multi task.
 *
 * PARAMETER:
 * @sem - [input] semaphore structure pointer.
 *
 * DESCRIPTION:
 * This function creates semaphore for platform select NANDC_MULTITASK.
 * 
 * CALL FUNC:
 * nand_init () - which is only for Vxworks platform.
 * 
 *****************************************************************************/
u32 nandc_mutex_create(void** sem)
{
    *sem = (void*)semMCreate(SEM_Q_PRIORITY | SEM_INVERSION_SAFE);

    NANDC_REJECT_NULL(*sem);

    return NANDC_OK;
    
ERRO:
    return NANDC_ERROR;
}

/*****************************************************************************
 * FUNC NAME:  
 * nandc_mutex_lock() -  lock mutex semaphor 
 *
 * PARAMETER:
 * @sem - [input] semaphore structure pointer.
 *
 * DESCRIPTION:
 * This function locks mutex semaphor for platform select NANDC_MULTITASK.
 * 
 * CALL FUNC:
 * handle->sem_lock () - nand_init() in the file :nandc_balong.c .
 * 
 *******************************************************************************/
u32 nandc_mutex_lock(void* sem)
{
    if(NANDC_OK !=  semTake((SEM_ID)sem, WAIT_FOREVER))
    {
        NANDC_TRACE(NFCDBGLVL(ERRO), ("nand mutex lock return error!\n")); /*lint !e778*/

        return NANDC_ERROR;
    }

    return NANDC_OK;
}

/********************************************************************************
 * FUNC NAME:  
 * nandc_mutex_unlock() - unlock mutex semaphor 
 *
 * PARAMETER:
 * @sem - [input] semaphore structure pointer.
 *
 * DESCRIPTION:
 * This function unlocks mutex semaphor for platform select NANDC_MULTITASK.
 * 
 * CALL FUNC:
 * handle->sem_unlock () - nand_init() in the file :nandc_balong.c .
 * 
 *********************************************************************************/
u32 nandc_mutex_unlock(void* sem)
{
    if(NANDC_OK !=  semGive((SEM_ID)sem))
    {
        NANDC_TRACE(NFCDBGLVL(ERRO), ("nand mutex unlock return error!\n")); /*lint !e778*/
        return NANDC_ERROR;
    }

    return NANDC_OK;
}

/**********************************************************************************
 * FUNC NAME:  
 * nandc_mutex_delete() - delete semaphore for platform which has multi task.
 *
 * PARAMETER:
 * @sem - [input] semaphore structure pointer.
 *
 * DESCRIPTION:
 * This function deletes semaphore for platform select NANDC_MULTITASK.
 * 
 * CALL FUNC:
 * nand_init () - which is only for Vxworks platform.
 * 
 ***********************************************************************************/
u32 nandc_mutex_delete(void* sem)
{
    return semDelete(sem);/*lint !e732*/
}

#endif /*NANDC_COMPILER(NANDC_MULTITASK)*/


#if NANDC_COMPILER(NANDC_DUALCORE)
#ifdef NANDC_USE_IPC
/************************************************************************************
 * FUNC NAME:  
 * nandc_ipc_create() - delete semaphore for platform which has multi task.
 *
 * PARAMETER:
 * none 
 *
 * DESCRIPTION:
 *
 * 
 * CALL FUNC:
 * nand_init () - which is only for Vxworks platform.
 * 
 ***********************************************************************************/
u32 nandc_ipc_create(void)
{
    if(NANDC_OK !=  BSP_IPC_SemCreate(IPC_SEM_NAND))
    {
        NANDC_TRACE(NFCDBGLVL(ERRO), ("nand ipc create return error!\n")); /*lint !e778*/
        return NANDC_ERROR;
    }

    return NANDC_OK;
}

/**************************************
 * FUNC NAME:  
 * nandc_ipc_delete() - 
 *
 * PARAMETER:
 * none
 *
 * DESCRIPTION:
 *
 * 
 * CALL FUNC:
 * () -
 * 
 *************************************/
u32 nandc_ipc_delete(void)
{
    if(NANDC_OK !=  BSP_IPC_SemCreate(IPC_SEM_NAND))
    {
        NANDC_TRACE(NFCDBGLVL(ERRO), ("nand ipc delete return error!\n")); /*lint !e778*/
        return NANDC_ERROR;
    }

    return NANDC_OK;
}

/**************************************
 * FUNC NAME:  
 * nandc_ipc_wait() - 
 *
 * PARAMETER:
 * @overtime- [input] 
 *
 * DESCRIPTION:
 *
 * 
 * CALL FUNC:
 * () -
 * 
 *************************************/
u32 nandc_ipc_wait(u32 overtime)
{
    //NANDC_TRACE(NFCDBGLVL(NORMAL), ("nandc_ipc_wait in!(%s)\n",taskName(taskIdSelf())));

    if(NANDC_OK !=  BSP_IPC_SemTake(IPC_SEM_NAND, (s32)overtime))
    {
        NANDC_TRACE(NFCDBGLVL(ERRO), ("nand ipc waite return error!\n")); /*lint !e778*/
        return NANDC_ERROR;
    }
    //NANDC_TRACE(NFCDBGLVL(NORMAL), ("nandc_ipc_wait out!(%s)\n",taskName(taskIdSelf())));

    return NANDC_OK;
}

/**************************************
 * FUNC NAME:  
 * nandc_ipc_post() - 
 *
 * PARAMETER:
 * none 
 *
 * DESCRIPTION:
 *
 * 
 * CALL FUNC:
 * () -
 * 
 *************************************/
void nandc_ipc_post(void)
{
    //NANDC_TRACE(NFCDBGLVL(NORMAL), ("nandc_ipc_post in!(%s)\n",taskName(taskIdSelf())));

    BSP_IPC_SemGive(IPC_SEM_NAND);
    
    //NANDC_TRACE(NFCDBGLVL(NORMAL), ("nandc_ipc_post out!(%s)\n",taskName(taskIdSelf())));

    return ;
}
#elif defined(NANDC_USE_SPINLOCK)
extern BSP_U32 spinLockAmpTakeOneTime(BSP_U32 *pSpinLock);
extern BSP_U32 spinLockAmpTake(BSP_U32 *pSpinLock);
extern BSP_VOID spinLockAmpGive(BSP_U32 *pSpinLock);

/*****************************************************************************
* 函 数 名  : NANDF_SpinLock
*
* 功能描述  : NANDFLASH 双核互斥自旋锁
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  : 1:take 超时
*             0:take 成功
*
* 其它说明  : 
*
*****************************************************************************/
u32 NANDF_SpinLock()
{
    u32 i = 0;
    u32 j = 0;
    u32 u32Status = 0;
    u32 u32SpinLockAddr = MEMORY_AXI_FLASH_SEM_ADDR;

    while(i < 6000)
    {
        j = 0;
        while(j < 2000)
        { 
            u32Status  =  spinLockAmpTakeOneTime((u32 *)u32SpinLockAddr);        
            if(0 == u32Status)
            {
                return u32Status;
            }
            j++;
        } 
        i++;
        taskDelay(1);
    }
   
    return u32Status;
}

/*****************************************************************************
* 函 数 名  : NANDF_SpinUnLock
*
* 功能描述  : NANDFLASH 双核互斥解锁
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_VOID NANDF_SpinUnLock()
{        
    u32 u32SpinLockAddr = MEMORY_AXI_FLASH_SEM_ADDR;
    spinLockAmpGive((u32 *)u32SpinLockAddr);
}

u32 nandc_ipc_create(void)
{
    /*
    *(volatile int*)0x30000200 = 0;  //A-core do it before C-core
    */
    
    return NANDC_OK;
}

u32 nandc_ipc_delete(void)
{
    return NANDC_OK;
}


u32 nandc_ipc_wait(u32 overtime)
{
    //NANDC_TRACE(NFCDBGLVL(NORMAL), ("NANDF_SpinLock in!(%s)\n",taskName(taskIdSelf())));

    if(NANDC_OK !=  NANDF_SpinLock())
    {
        NANDC_TRACE(NFCDBGLVL(ERRO), ("NANDF_SpinLock return error!\n"));
        NANDF_SpinUnLock();
        return NANDC_ERROR;
    }
    //NANDC_TRACE(NFCDBGLVL(NORMAL), ("NANDF_SpinLock out!(%s)\n",taskName(taskIdSelf())));

    return NANDC_OK;
}


void nandc_ipc_post(void)
{
    //NANDC_TRACE(NFCDBGLVL(NORMAL), ("NANDF_SpinUnLock in!(%s)\n",taskName(taskIdSelf())));

    NANDF_SpinUnLock();
    
    //NANDC_TRACE(NFCDBGLVL(NORMAL), ("NANDF_SpinUnLock out!(%s)\n",taskName(taskIdSelf())));

    return ;
}

#endif /*NANDC_USE_IPC*/

#endif /*NANDC_COMPILER(NANDC_DUALCORE)*/

#ifdef __cplusplus
}
#endif
