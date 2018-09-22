
/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_SEC.h
*
*   作    者 :  
*
*   描    述 :  SEC模块用户接口文件
*
*   修改记录 :  2012年04月16日  v1.00  创建
*************************************************************************/

#ifndef __BSP_SEC_H__
#define __BSP_SEC_H__

#ifdef __cplusplus
extern "C" 
{ 
#endif /* __cplusplus */



/*----------安全模块 ----------------------- */
#define MAX_N_LENGTH    32

#define BSP_ERR_SEC_BASE            BSP_DEF_ERR(BSP_MODU_SECURITY, 0)
#define BSP_ERR_SEC_OEMCA             (BSP_ERR_SEC_BASE + 1)     /*OEM CA 校验ERROR*/
#define BSP_ERR_SEC_IMG             (BSP_ERR_SEC_BASE + 2)     /*映像校验ERROR*/

#define BSP_ERR_EFUSE_WRT_REFUSE    (BSP_ERR_SEC_BASE + 3)     /*Efsue不允许烧写*/
#define BSP_ERR_EFUSE_OP_TIMEOUT    (BSP_ERR_SEC_BASE + 4)     /*Efsue不允许烧写*/
/* 公钥格式 */
typedef struct
{
    BSP_U16  eLen;    /* 单位为bits,不能大于1024 */
    BSP_U16  nLen;    /* 单位为bits,不能大于1024 */
    BSP_U32  e[MAX_N_LENGTH];       /* 幂e(公钥) */
    BSP_U32  n[MAX_N_LENGTH];       /* 模数 */
}KEY_STRUCT;


/*****************************************************************************
* 函 数 名  : efuseRead
*
* 功能描述  : 按组读取EFUSE中的数据
*
* 输入参数  : group  起始group
*                   num  数组长度(word数,不超过512/4=128)
* 输出参数  : pBuf ：EFUSE中的数据
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
int efuseRead(BSP_U32 group, BSP_U32* pBuf, BSP_U32 num);

/*****************************************************************************
* 函 数 名  : secInit
*
* 功能描述  : 安全驱动初始化
*
* 输入参数  :
* 输出参数  :
*
* 返 回 值  :
*
* 其它说明  :
*
*****************************************************************************/
BSP_S32 secInit(BSP_VOID);
/*****************************************************************************
* 函 数 名  : secDataRsa
*  Description:
*      RSA.
*
*  Calls:
*      SsdmaRsaInit, SetSsdmaRsaMode
*
*  Input:
*         pstCa: 密钥
*         pucInBuf: 输入数据的指针(输入数据的长度不超过128Byte)
*         ulInLen:输入数据的长度(不超过128Byte)
*
*
*  Output:
*         pOutdata: 输出数据的指针(输出数据的长度不超过128Byte)
*         pulOutLen: 输出数据的长度(128Byte)
*
*
*  Return:
*         BSP_OK: the function successful returned.
*         BSP_ERROR: the function encounter SSDMA _ERROR while running.
*  Other: 待进行RSA运算的数据的长度和RSA运算后输出的数据的长度
*****************************************************************************/
BSP_S32 secDataRsa(const KEY_STRUCT *pstCa, const BSP_U8*  pucInBuf, BSP_U32 ulInLen, BSP_U8* pucOutBuf, BSP_U32* pulOutLen);

/******************************************************************************
 *  Function:  secDataSha
 *  Description:
 *      SHA1.
 *  Calls:
 *      SsdmaSha1Init, SetSsdmaSha1Iv, RunSsdmaSha1, byteCopy
 *  Called By:
 *      identify
 *  Input:
 *         pucInBuf:   指向输入数据地址的指针
 *         pucData:   输入数据的长度(字节数)
 *         pucOutBuf: 指向输出数据地址的指针
 *  Output:
 *         hash: 指向输入数据进行SHA1计算后的哈希值的指针(哈希值的长度为20Byte)
 *  Return:
 *         BSP_OK: the function successful returned.
 *         BSP_ERROR: the function encounter BSP_ERROR while running.
 *
 ********************************************************************************/
BSP_S32 secDataSha(const BSP_U8*  pucInBuf, BSP_U32 ulInLen, BSP_U8* pucOutBuf);

/*****************************************************************************
* 函 数 名  : secIdioChk
*
* 功能描述  :   通过校验输入数据的hash值是 否
*                    与其签名用RSA解密后的值一样
*                    来校验输入数据的合法性 *
*
* 输入参数  :   pucData--输入数据的地址
*                   dataLen--输入数据的长度
*                   pubKey--公钥
*                   pIdio--输入数据签名的指针
* 输出参数  :
*
* 返 回 值  : SSDMA_OK/ SSDMA_ERROR
*
* 其它说明  :
*
*****************************************************************************/
BSP_S32 secIdioChk(const BSP_U8* pucData, BSP_U32 dataLen, const KEY_STRUCT *pubKey, const BSP_U32* pIdio);

/*****************************************************************************
* 函 数 名  : secCheck
*
* 功能描述  :   通过校验输入数据的hash值是 否
*                    与其签名用RSA解密后的值一样
*                    来校验输入数据的合法性 *
*
* 输入参数  : KEY_STRUCT *pubKey根公钥
*                   ulImgAddr--映像地址
*                  ulImgLen--镜像长度
* 输出参数  :
*
* 返 回 值  : BSP_OK           安全校验通过
*                BSP_ERR_SEC_OEMCA        OEM CA校验错误
*                BSP_ERR_SEC_IMG        镜像签名校验错误
*
* 其它说明  :
*
*****************************************************************************/
BSP_S32 secCheck(const KEY_STRUCT *pubKey, BSP_U32 ulImgAddr, BSP_U32 ulImgLen);

/*****************************************************************************
* 函 数 名  : efuseWriteAt
*
* 功能描述  : 供AT命令调用直接烧写Efsue
*
* 输入参数  : 无
* 输出参数  :
*
* 返 回 值  : BSP_OK,烧写成功
*             BSP_ERROR,烧写失败
*
* 其它说明  :
*
*****************************************************************************/
BSP_S32 efuseWriteAt(BSP_VOID);

/*****************************************************************************
* 函 数 名  : secIsSecVer
*
* 功能描述  : 判断当前运行版本是否为安全版本
*
* 输入参数  : 
* 输出参数  : 
*
* 返 回 值  : 
*
* 其它说明  : 
*
*****************************************************************************/
BSP_BOOL secIsSecVer( BSP_VOID );


#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif /* __cplusplus */

#endif

