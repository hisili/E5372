/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_NANDF.h
*
*   作    者 :  z67193
*
*   描    述 :  BSP类型定义头文件
*
*   修改记录 :  2012年04月14日  v1.00  z67193  创建
*************************************************************************/

#ifndef	__BSP_NANDF_H__
#define __BSP_NANDF_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */

/*************************NANDF BEGIN************************/
/**************************************************************************
  宏定义 
**************************************************************************/
/*错误码*/
#define NANDF_OK              BSP_OK
#define NANDF_ERROR_INIT      BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_MODULE_NOT_INITED)  /*没有初始化*/
#define NANDF_ERROR_ARGS      BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_INVALID_PARA)  /*参数错误*/
#define NANDF_ERROR_MALLOC    BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_BUF_ALLOC_FAILED)  /*内存空间分配失败*/
#define NANDF_ERROR_TIMEOUT   BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_RETRY_TIMEOUT)  /*循环次数达到最大*/
#define NANDF_BAD_BLOCK       BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 1)  /*坏块*/
#define NANDF_GOOD_BLOCK      BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 2)  /*好块*/
#define NANDF_ERROR_READ      BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 3)  /*读操作失败*/
#define NANDF_ERROR_WRITE     BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 4)  /*写操作失败*/
#define NANDF_ERROR_ERASE     BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 5)  /*擦除操作失败*/
#define NANDF_ERROR_MARKBAD   BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 6)  /*标志坏块失败*/
#define NANDF_ERROR_ERASE_MARKBAD   BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 7)  /*擦除失败并且标志坏块失败*/
#define NANDF_ERROR_WRITE_MARKBAD   BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 8)  /*写入失败并且标志坏块失败*/
#define NANDF_ERROR_STATUS_CHECK   BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 9)  /*读取状态寄存器，状态不正确*/
#define NANDF_ERROR_SEMTAKE   BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 10)  /*申请信号量失败*/
#define NANDF_ERROR_SEMGIVE   BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 11)  /*释放信号量失败*/
#define NANDF_ERROR_ADDR_OVERFLOW BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 12)  /* 地址越界 */
#define NANDF_ERROR_MODULE_LOCKED  BSP_DEF_ERR(BSP_MODU_NANDF, BSP_ERR_SPECIAL + 13)

BSP_S32 BSP_NANDF_DrxBakReg();
BSP_VOID BSP_NANDF_DrxRestoreReg();


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_NANDF_H__ */


