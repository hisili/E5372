/*******************************************************************************
* Copyright (C), 2008-2011, HUAWEI Tech. Co., Ltd.
*
* Module name: Hisilicon Nand Flash Controller Dirver
*
* Description: the Hisilicon Nand Flash Controller physical and logical driver
*
* Version: v1.0
*
* Filename:    nandc_mco.h
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
#ifndef _NANDC_MCO_H_
#define _NANDC_MCO_H_
    
#ifdef __cplusplus
extern "C"
{
#endif

#define NANDC_COMPILER(flag)      (NANDC_COMPILE_FLAG & flag)

#define NANDC_TRUE                         1
#define NANDC_FALSE                        0 
#define NANDC_NULL                         0


#define NANDC_OK                           0
#define NANDC_ERROR                        1
#define NANDC_E_NOMEM                      2
#define NANDC_E_PARAM                      3
#define NANDC_E_NOFIND                     4
#define NANDC_E_STATUS                     5
#define NANDC_E_TIMEOUT                    6
#define NANDC_E_INIT                       7
#define NANDC_E_READID                     8
#define NANDC_E_NULL                       9
#define NANDC_E_ERASE                      10
#define NANDC_E_ADDR_OVERSTEP              11
#define NANDC_E_READ                       12
#define NANDC_E_ECC                        13

/*
 * Tracing flags.
 * The flags masked in NANDC_TRACE_ALL|NANDC_TRACE_NORMAL are always traced.
 */
 
#define NANDC_TRACE_ASSERT		        0x00000001
#define NANDC_TRACE_OS			        0x00000002
#define NANDC_TRACE_CTRL			    0x00000004
#define NANDC_TRACE_CHIP			    0x00000008
#define NANDC_TRACE_HOST		        0x00000010
#define NANDC_TRACE_MTD		            0x00000020
#define NANDC_TRACE_CONF	            0x00000040
#define NANDC_TRACE_INIT		        0x00000080
#define NANDC_TRACE_NATIVE		        0x00000100
#define NANDC_TRACE_NAND		        0x00000200
#define NANDC_TRACE_PORT                0x00000400
#define NANDC_TRACE_PTABLE              0x00000800
#define NANDC_TRACE_ALL		            0x0FFFFFFF
#define NANDC_TRACE_ERRO			    0x00000000
#define NANDC_TRACE_WARNING		        0x10000000
#define NANDC_TRACE_NORMAL		        0x20000000
#define NANDC_TRACE_LEVEL			    0xF0000000

#define HICHAR_NULL                     ""

#ifdef NANDC_NAND_SIZE_WIDTH64
/*typedef u64                           FSZ;*/
/*flash size*/
#define FSZ                             long long unsigned int   
/*flash size wildcard character*/
#define FWC                             "016ll"
#else
/*typedef u32                           FSZ;*/
/*flash size*/
#define FSZ                             long unsigned int
/*flash size wildcard character*/
#define FWC                             "08l"
#endif

 
#if NANDC_COMPILER(NANDC_DEBUG)

#define NANDC_TRACE(mask, p)    \
    /*do*/{                                                                                        \
        if (((mask) & nandc_trace_mask & NANDC_TRACE_ALL) &&                                                       \
        ((s32)((mask) &  (NANDC_TRACE_LEVEL)) <= (s32)((nandc_trace_mask) & (NANDC_TRACE_LEVEL)))) \
        {                                                                                           \
                hiout(p);                                                                           \
        }                                                                                           \
    } /*while(0)*/
#else
#define NANDC_TRACE(mask, p)
#endif



#if NANDC_COMPILER(NANDC_USE_ASSERT)
#define NANDC_DO_ASSERT(a, msg, p)    \
do{                                                                                     \
    if(!(a))                                                                            \
    {                                                                                   \
        if(msg)hiout(("%s",(char*)msg));                                                       \
        hiout((NANDC_NULL_ASSERT_FORMAT ,p, __FILE__, __LINE__));                       \
        hiassert(0);                                                                    \
    }                                                                                   \
}while(0)
#else
#define NANDC_DO_ASSERT(a, msg, p)    
#endif


#define NANDC_REJECT_NULL(p)    \
 /*do*/{                                                                                \
    if(NANDC_NULL == (void*)(p))                                                           \
    {                                                                                   \
        NANDC_DO_ASSERT(0, NANDC_NULL_PARAM_FORMAT, 0);                                 \
        goto ERRO;                                                                      \
    }                                                                                   \
 }/*while(0)*/

#define NANDC_CHECK_PAGESIZE(pagesize)      ((pagesize==nandc_size_hk)||(pagesize==nandc_size_2k)||(pagesize==nandc_size_4k)\
                                                                                    ||(pagesize==nandc_size_8k))?pagesize:NANDC_NULL

#define NANDC_CHECK_ERASESIZE(erasesize)     ((erasesize==nandc_size_hk*nandc_block_64)||(erasesize==nandc_size_2k*nandc_block_64)||\
                                                                      (erasesize==nandc_size_4k*nandc_block_64)||(erasesize==nandc_size_8k*nandc_block_128))?erasesize:NANDC_NULL

#define NANDC_CHECK_BUSWIDE(buswidth)       buswidth

#define NANDC_CHECK_CHIPSIZE(chipsize)      chipsize;

#define NANDC_CHECK_SPARESIZE(sparesize)    sparesize;                  


#define NANDC_SET_REG_BITS(bitfunc, value)     \
  /*do*/{if(bitfunc.bitset)                                                                \
        {                                                                                  \
            if(32 == (bitfunc).bitset->length)                                             \
                NANDC_REG_WRITE32((u32)host->regbase + ((bitfunc).reg_offset), value);    \
            else                                                                           \
                NANDC_REG_SETBIT32((u32)host->regbase + ((bitfunc).reg_offset),            \
                                                            (bitfunc).bitset->offset ,      \
                                                            (bitfunc).bitset->length,       \
                                                            value);                         \
        }}/*while(0)*/

#define NANDC_WRITE_REG_BITS(bitfunc, value)     \
    /*do*/{if(bitfunc.bitset)                                                                 \
                    NANDC_REG_WRITEBIT32((u32)host->regbase + ((bitfunc).reg_offset),        \
                                                            (bitfunc).bitset->offset ,        \
                                                            (bitfunc).bitset->length,         \
                                                            value);                           \
        }/*while(0)*/


#define NANDC_GET_REG_BITS(bitfunc)     \
    ((bitfunc.bitset)?                                                                       \
                    NANDC_REG_GETBIT32((u32)host->regbase + ((bitfunc).reg_offset),          \
                                                            (bitfunc).bitset->offset ,        \
                                                            (bitfunc).bitset->length):0 )      

__inline static void nandc_byte_cpy(char* dst, const char* src,  u32 size)
{
	while(size)
	{
		*dst++ = *src++;
		size--;
	}
}

 #define ADDR_DATA_SWAP32(a,b) \
    do{\
                        *(u32*)(a) = (*(u32*)(a)) ^ (*(u32*)(b)); \
                        *(u32*)(b) = (*(u32*)(a)) ^ (*(u32*)(b)); \
                        *(u32*)(a) = (*(u32*)(a)) ^ (*(u32*)(b)); \
    }while(0)
    
#define HI_NOTUSED(p)   ((void)(p)); ((p)=(p))  

#define PARTITION_NAME_TEMPLATE "partition00"

#define NANDC_MS_TO_COUNT(ms)       ms    

#ifdef __cplusplus
}
#endif

#endif  /*_NANDC_MCO_H_*/



