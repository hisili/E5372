/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_SD.h
*
*   作    者 :  z67193
*
*   描    述 :  BSP类型定义头文件
*
*   修改记录 :  2012年04月14日  v1.00  z67193  创建
*************************************************************************/

#ifndef	__BSP_SD_H__
#define __BSP_SD_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */

#ifdef __KERNEL__
#include <linux/scatterlist.h>
#endif


/*****************************************************************************
* 函 数 名  : SDIO_check_card
*
* 功能描述  :
*
* 输入参数  : BSP_S32 s32CardNo
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
BSP_S32 SDIO_check_card(BSP_S32 s32CardNo);

/*****************************************************************************
* 函 数 名  : read_write_blkdata
*
* 功能描述  : 

* 输入参数  : 

* 输出参数  : 
* 返 回 值  : 
*
* 修改记录  : 

*****************************************************************************/
BSP_S32  read_write_blkdata(BSP_S32 s32CardNo, BSP_U32 u32Addr,
                            BSP_U32 u32DataSize,
                            BSP_U8  *pu8DataBuff,
                            BSP_U8  *pu8CmdRespBuff,
                            BSP_U8  *pu8ErrRespBuff,
                            BSP_VOID (*callback)(BSP_U32 u32ErrData),
                            BSP_S32 s32NoOfRetries,
                            BSP_S32 s32Flags);


#ifdef __KERNEL__

/*****************************************************************************
* 函 数 名  : sd_get_status
*
* 功能描述  : 卡在位查询
*
* 输入参数  : void
* 输出参数  : NA
*
* 返 回 值  : 0 : 在位；-1: 不在位
*
* 其它说明  : NA
*
*****************************************************************************/
extern int sd_get_status(void);


/*****************************************************************************
* 函 数 名  : sd_get_capacity
*
* 功能描述  : 卡容量查询
*
* 输入参数  : void
* 输出参数  : NA
*
* 返 回 值  : 0 : 失败；>0: 卡容量
*
* 其它说明  : NA
*
*****************************************************************************/
extern int sd_get_capacity(void);


/*****************************************************************************
* 函 数 名  : sd_transfer
*
* 功能描述  : 数据传输
*
* 输入参数  : struct scatterlist *sg    待传输数据结构体指针
                            unsigned dev_addr   待写入的SD block 地址
                            unsigned blocks    待写入的block个数
                            unsigned blksz      每个block的大小，单位字节
                            int wrflags    读写标志位，写:WRFlAG ; 读:RDFlAG
* 输出参数  : NA
*
* 返 回 值  : 0 : 成功；其它: 失败
*
* 其它说明  : NA
*
*****************************************************************************/
extern int sd_transfer(struct scatterlist *sg, unsigned dev_addr,unsigned blocks, unsigned blksz, int wrflags);
/*****************************************************************************
* 函 数 名  : sd_sg_init_table
*
* 功能描述  : SD多块数据传输sg list初始化
*
* 输入参数  :  const void *buf        待操作的buffer地址
               unsigned int buflen    待操作的buffer大小,小于32K，为512B的整数倍;
                                      大于32K，为32KB的整数倍，最大128K
                            
* 输出参数  : NA
*
* 返 回 值  : 0 : 成功；其它: 失败
*
* 其它说明  : NA
*
*****************************************************************************/
extern int sd_sg_init_table(const void *buf,unsigned int buflen);
/*****************************************************************************
* 函 数 名  : sd_multi_transfer
*
* 功能描述  : SD多块数据传输。
*
* 输入参数  : unsigned dev_addr   待写入的SD block 地址
              unsigned blocks     待写入的block个数
              unsigned blksz      每个block的大小，单位字节
              int write           读写标志位，写:1 ; 读:0
* 输出参数  : NA
*
* 返 回 值  : 0 : 成功；其它: 失败
*
* 其它说明  : NA
*
*****************************************************************************/
extern int sd_multi_transfer(unsigned dev_addr, unsigned blocks, unsigned blksz, int write);

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_SD_H__ */


