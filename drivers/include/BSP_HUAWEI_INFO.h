/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_HUAWEI_INFO.h
*
*   作    者 :  z67193
*
*   描    述 :  BSP类型定义头文件
*
*   修改记录 :  2012年04月14日  v1.00  z67193  创建
*************************************************************************/

#ifndef	__BSP_HUAWEI_INFO_H__
#define __BSP_HUAWEI_INFO_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



#if 0
/* 硬件产品信息适配 */
#define HW_VER_INVALID              (BSP_U16)0xFFFF
/* Porting板和P500板兼容 */
#define HW_VER_PRODUCT_PORTING      (BSP_U16)0xFFFE
#define HW_VER_PRODUCT_PV500        (BSP_U16)0xFFFD

#if defined (CHIP_BB_6920ES)
/* 测试板 */
#define HW_VER_PRODUCT_UDP      (BSP_U16)0x0000

/* Stick */
#define HW_VER_PRODUCT_E392S_U      (BSP_U16)0x0200
#define HW_VER_PRODUCT_E3276S_150   (BSP_U16)0x0201

/* E5 */
#define HW_VER_PRODUCT_E5_SBM       (BSP_U16)0x0401

/*E5776*/
#define HW_VER_PRODUCT_E5776_EM     (BSP_U16)0x0400

#elif defined (CHIP_BB_6920CS)
/* 测试板 */
#define HW_VER_PRODUCT_UDP          (BSP_U16)0xFFFC

/* Stick */
#define HW_VER_PRODUCT_E392S_U      (BSP_U16)HW_VER_INVALID
#define HW_VER_PRODUCT_E3276S_150   (BSP_U16)0x0201

/* E5 */
#define HW_VER_PRODUCT_E5_SBM       (BSP_U16)0x0001

/*E5776*/
#define HW_VER_PRODUCT_E5776_EM     (BSP_U16)0x0000

#endif
#endif

/*子版本号定义*/
#define HW_VER_SUB_VA				(BSP_U16)0
#define HW_VER_SUB_VB				(BSP_U16)1
#define HW_VER_SUB_VC				(BSP_U16)2
#define HW_VER_SUB_VD				(BSP_U16)3
#define HW_VER_SUB_VE				(BSP_U16)5
#define HW_VER_SUB_VF				(BSP_U16)6
#define HW_VER_SUB_VG				(BSP_U16)7
#define HW_VER_SUB_VH				(BSP_U16)8

/*
用Bit位表示单板支持的频段， WCDMA和GSM共用一个32位的数，每一位对应一个band。
*/

    /*
    用Bit位表示单板支持的频段，bit1代表频段I,bit2代表频段II,
    依次类推,比特位为1,表示支持该频段.下表是比特位和频段对应关系:
    -------------------------------------------------------------------------------
            bit8       bit7      bit6     bit5    bit4     bit3      bit2     bit1
    -------------------------------------------------------------------------------
    WCDMA   900(VIII)  2600(VII) 800(VI)  850(V)  1700(IV) 1800(III) 1900(II) 2100(I)
            spare      spare     spare    spare   spare    spare     spare   J1700(IX)
    -------------------------------------------------------------------------------
    GSM频段 1900(VIII) 1800(VII) E900(VI) R900(V) P900(IV) 850(III)  480(II)  450(I)
            spare      spare     spare    spare   spare    spare     spare    700(IX)
    -------------------------------------------------------------------------------
    */
#define RF_WCDMA_NULL               (0)
#define RF_WCDMA_2100               (1<<0UL)
#define RF_WCDMA_1900               (1<<1UL)
#define RF_WCDMA_1800               (1<<2UL)
#define RF_WCDMA_1700               (1<<3UL)
#define RF_WCDMA_850                (1<<4UL)
#define RF_WCDMA_800                (1<<5UL)
#define RF_WCDMA_2600               (1<<6UL)
#define RF_WCDMA_900                (1<<7UL)
#define RF_WCDMA_J1700              (1<<8UL)
#define RF_WCDMA_U1700              (1<<9UL)
#define RF_WCDMA_J1500              (1<<10UL)

#define RF_GSM_450                  (1<<16UL)
#define RF_GSM_480                  (1<<17UL)
#define RF_GSM_850                  (1<<18UL)
#define RF_GSM_P900                 (1<<19UL)
#define RF_GSM_R900                 (1<<20UL)
#define RF_GSM_E900                 (1<<21UL)
#define RF_GSM_1800                 (1<<22UL)
#define RF_GSM_1900                 (1<<23UL)
#define RF_GSM_700                  (1<<24UL)
#define RF_GSM_ALL                  (RF_GSM_1900 | RF_GSM_1800 | RF_GSM_E900 | RF_GSM_P900 | RF_GSM_850)


/*
用Bit位表示单板支持的频段， LTE频段共64个，使用两个U32位的数表示，每一位对应一个band。
*/
#define RF_LTE_B1               (1<<0UL)
#define RF_LTE_B2               (1<<1UL)
#define RF_LTE_B3               (1<<2UL)
#define RF_LTE_B4               (1<<3UL)
#define RF_LTE_B5               (1<<4UL)
#define RF_LTE_B6               (1<<5UL)
#define RF_LTE_B7               (1<<6UL)
#define RF_LTE_B8               (1<<7UL)
#define RF_LTE_B9               (1<<8UL)
#define RF_LTE_B10               (1<<9UL)
#define RF_LTE_B11               (1<<10UL)
#define RF_LTE_B12               (1<<11UL)
#define RF_LTE_B13               (1<<12UL)
#define RF_LTE_B14               (1<<13UL)
#define RF_LTE_B15               (1<<14UL)
#define RF_LTE_B16               (1<<15UL)
#define RF_LTE_B17               (1<<16UL)
#define RF_LTE_B18               (1<<17UL)
#define RF_LTE_B19               (1<<18UL)
#define RF_LTE_B20               (1<<19UL)
#define RF_LTE_B21               (1<<20UL)
#define RF_LTE_B22               (1<<21UL)
#define RF_LTE_B23               (1<<22UL)
#define RF_LTE_B24               (1<<23UL)
#define RF_LTE_B25               (1<<24UL)
#define RF_LTE_B26               (1<<25UL)
#define RF_LTE_B27               (1<<26UL)
#define RF_LTE_B28               (1<<27UL)
#define RF_LTE_B29               (1<<28UL)
#define RF_LTE_B30               (1<<29UL)
#define RF_LTE_B31               (1<<30UL)
#define RF_LTE_B32               (1<<31UL)

#define RF_LTE_B33               (1<<0UL)
#define RF_LTE_B34              (1<<1UL)
#define RF_LTE_B35              (1<<2UL)
#define RF_LTE_B36              (1<<3UL)
#define RF_LTE_B37              (1<<4UL)
#define RF_LTE_B38              (1<<5UL)
#define RF_LTE_B39              (1<<6UL)
#define RF_LTE_B40              (1<<7UL)
#define RF_LTE_B41              (1<<8UL)
#define RF_LTE_B42               (1<<9UL)
#define RF_LTE_B43               (1<<10UL)
#define RF_LTE_B44               (1<<11UL)
#define RF_LTE_B45               (1<<12UL)
#define RF_LTE_B46               (1<<13UL)
#define RF_LTE_B47               (1<<14UL)
#define RF_LTE_B48               (1<<15UL)
#define RF_LTE_B49               (1<<16UL)
#define RF_LTE_B50               (1<<17UL)
#define RF_LTE_B51               (1<<18UL)
#define RF_LTE_B52               (1<<19UL)
#define RF_LTE_B53               (1<<20UL)
#define RF_LTE_B54               (1<<21UL)
#define RF_LTE_B55               (1<<22UL)
#define RF_LTE_B56               (1<<23UL)
#define RF_LTE_B57               (1<<24UL)
#define RF_LTE_B58               (1<<25UL)
#define RF_LTE_B59               (1<<26UL)
#define RF_LTE_B60               (1<<27UL)
#define RF_LTE_B61               (1<<28UL)
#define RF_LTE_B62               (1<<29UL)
#define RF_LTE_B63               (1<<30UL)
#define RF_LTE_B64               (1<<31UL)


#define RF_BAND_ALL             (0xFFFFFFFFUL)

/*--------------------------------------------------------------*
 * 数据结构                                                     *
 *--------------------------------------------------------------*/


//BSP_U16 BSP_HwGetVerMain( BSP_VOID );
BSP_U16 BSP_HwGetVerSub( BSP_VOID );


/*****************************************************************************
* 函 数 名  : BSP_GetProductName
*
* 功能描述  : 获取产品名称
*
* 输入参数  : char* pProductName,字符串指针，保证不小于32字节
*                             BSP_U32 ulLength,缓冲区长度
* 输出参数  : 无
*
 返 回 值  : 0：正确，非0: 失败
*
* 修改记录  : 2011-3-30 wuzechun creat
*
*****************************************************************************/
//BSP_S32 BSP_GetProductName (char * pProductName, unsigned int ulLength);

/*****************************************************************************
* 函 数 名  : BSP_GetProductInnerName
*
* 功能描述  : 获取内部产品名称
*
* 输入参数  : char* pProductIdInter,字符串指针
*                             BSP_U32 ulLength,缓冲区长度
* 输出参数  : 无
*
 返 回 值  : 0：正确，非0: 失败
*
* 修改记录  : 2011-3-29 wuzechun creat
*
*****************************************************************************/
BSP_S32 BSP_GetProductInnerName (char * pProductIdInter, unsigned int ulLength);

/*****************************************************************************
* 函 数 名  : BSP_GetProductDloadName
*
* 功能描述  : 获取产品下载版本名称，一键升级据此判断是否允许升级
*
* 输入参数  : 无
* 输出参数  : 无
*
* 返 回 值  : 产品下载版本名称字符串指针
*
* 修改记录  : 2011-3-30 wuzechun creat
*
*****************************************************************************/
BSP_S32 BSP_GetProductDloadName(char * pProductDloadName, unsigned int ulLength);


/*****************************************************************************
* 函 数 名  : BSP_GetSupportBandsForGu
*
* 功能描述  : 获取GU模支持的频段
*
* 输入参数  : pusWBands - WCDMA的频段值
             pusGBands - GSM的频段值

 输出参数  : pusWBands - WCDMA的频段值
             pusGBands - GSM的频段值

 返 回 值  : 0：正确，非0: 失败
             通过Bit位来表示哪些频段支持。
*
*****************************************************************************/

int BSP_GetSupportBandsForGu(unsigned short *pusWBands, unsigned short *pusGBands);




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_HUAWEI_INFO_H__ */


