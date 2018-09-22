/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_GMAC.h
*
*   作    者 :  z67193
*
*   描    述 :  BSP类型定义头文件
*
*   修改记录 :  2012年04月14日  v1.00  z67193  创建
*************************************************************************/

#ifndef	__BSP_GMAC_H__
#define __BSP_GMAC_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */



/*************************GMAC BEGIN************************/


/******************************************************************************
* Function     :   BSP_GMAC_PowerDown
* 
* Description  :   
* 
* Input        :  
* 
* Output       :   无
* 
* return       :   
******************************************************************************/
BSP_S32 BSP_GMAC_PowerDown(BSP_VOID);
/******************************************************************************
* Function     :   BSP_GMAC_PowerUp
* 
* Description  :   
* 
* Input        :  
* 
* Output       :   无
* 
* return       :   
******************************************************************************/
BSP_S32 BSP_GMAC_PowerUp(BSP_VOID);
/******************************************************************************
* Function     :   BSP_GMAC_RegisterUpLinkRxCb
* 
* Description  :   注册上行链路接收回调函数 
* 
* Input        :  GMAC_OWNER_E eOwner,     调用者 
                  UpLinkRxFunc pfunc       上行链路接收回调函数 
* 
* Output       :   无
* 
* return       :   
******************************************************************************/
BSP_S32 BSP_GMAC_RegisterUpLinkRxCb(GMAC_OWNER_E eOwner,   UpLinkRxFunc pfunc );
/******************************************************************************
* Function     :   BSP_GMAC_RegisterFreePktEncapCb
* 
* Description  :    注册包封装释放回调函数  
* 
* Input        :  GMAC_OWNER_E eOwner,     调用者 
                  FreePktEncap pfunc      包封装释放回调函数 
* 
* Output       :   无
* 
* return       :   
******************************************************************************/
BSP_S32 BSP_GMAC_RegisterFreePktEncapCb( GMAC_OWNER_E eOwner, FreePktEncap pfunc );
/******************************************************************************
* Function     :   BSP_GMAC_SetPktEncapInfo
* 
* Description  :    设置包封装结构信息
* 
* Input        :  GMAC_OWNER_E eOwner,    调用者
                  BSP_S32 s32BufOft,      buf偏移量
                  BSP_S32 s32LenOft,      len偏移量
                  BSP_S32 s32NextOft      next偏移量
* 
* Output       :   无
* 
* return       :   
******************************************************************************/
BSP_S32 BSP_GMAC_SetPktEncapInfo(GMAC_OWNER_E eOwner, BSP_S32 s32BufOft, BSP_S32 s32LenOft,BSP_S32 s32NextOft);
/******************************************************************************
* Function     :   BSP_GMAC_DownLinkTx
* 
* Description  :    下行数据发送函数
* 
* Input        :  GMAC_OWNER_E eOwner,    调用模块
                  BSP_VOID *pPktEncap     包封装首地址
* 
* Output       :   无
* 
* return       :   
******************************************************************************/
BSP_S32 BSP_GMAC_DownLinkTx(GMAC_OWNER_E eOwner, BSP_VOID *pPktEncap );
/*************************GMAC END************************/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_GMAC_H__ */


