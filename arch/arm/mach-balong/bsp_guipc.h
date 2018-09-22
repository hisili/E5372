/*************************************************************************
*   版权所有(C) 1987-2004, 深圳华为技术有限公司.
*
*   文 件 名 :  GUIpc.h
*
*   作    者 :  c61362
*
*   描    述 :  IPC模块用户接口文件
*
*   修改记录 :  2012年2月4日  v1.00  c61362  创建
*************************************************************************/

#ifndef _BSP_GUIPC_H_
#define _BSP_GUIPC_H_

#ifdef __cplusplus
extern "C" {
#endif

#if ((defined (CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)) && defined (BOARD_FPGA_P500))       /*只在V7R1 FPGA平台上提供*/

#define INTSRC_NUM                          (32)

#define BSP_IPC_BASE_ADDR                   (0x5F054000)
#define BSP_IPC_SIZE                        (0x00001000)

#define BSP_IPC_CPU_RAW_INT(i)              (0x400 + (i * 0x10))
#define BSP_IPC_CPU_INT_MASK(i)             (0x404 + (i * 0x10))
#define BSP_IPC_CPU_INT_STAT(i)             (0x408 + (i * 0x10))
#define BSP_IPC_CPU_INT_CLR(i)              (0x40C + (i * 0x10))

#define BSP_IPC_SEM_RAW_INT(i)              (0x600 + (i * 0x10))
#define BSP_IPC_SEM_INT_MASK(i)             (0x604 + (i * 0x10))
#define BSP_IPC_SEM_INT_STAT(i)             (0x608 + (i * 0x10))
#define BSP_IPC_SEM_INT_CLR(i)              (0x60C + (i * 0x10))

#define BSP_IPC_HS_CTRL(i,j)                (0x800 + (i * 0x100) + (j * 0x8 ))
#define BSP_IPC_HS_STAT(i,j)                (0x804 + (i * 0x100) + (j * 0x8 ))

#define IPC_MASK                            0xFFFFFF0F

#define INT_LEV_IPC_COMBINE                (INT_LVL_MAX + SUBVIC_NUM + 15)


#define IPC_CHECK_PARA(ulLvl) \
    do{\
        if(ulLvl >= INTSRC_NUM)\
        {\
            printk("GU IPC Wrong para , line:%d\n", __LINE__,0,0,0,0,0);\
            return BSP_ERROR;\
        }\
    }while(0)

typedef struct tagIPC_DEBUG_E
{
    BSP_U32 u32RecvIntCore;
    BSP_U32 u32IntHandleTimes[INTSRC_NUM];
    BSP_U32 u32IntSendTimes[INTSRC_NUM];
    BSP_U32 u32SemId;
    BSP_U32 u32SemTakeTimes[INTSRC_NUM];
    BSP_U32 u32SemGiveTimes[INTSRC_NUM];
}IPC_DEBUG_E;
 
typedef struct 
{   
    VOIDFUNCPTR	    routine;
    unsigned int	arg;
}BSP_GUIPC_ENTRY;

/*****************************************************************************
* 函 数 名  : BSP_DRV_IPCIntInit
*
* 功能描述  : IPC模块初始化
*
* 输入参数  : 无  
* 输出参数  : 无
*
* 返 回 值  : 无
*
* 修改记录  : 2012年2月4日 c61362 creat
*****************************************************************************/
BSP_S32 BSP_GUIPC_Init();


/*****************************************************************************
 * 函 数 名  : BSP_GUIPC_IpcIntHandler
 *
 * 功能描述  : 中断处理函数
 *
 * 输入参数  : 无  
 * 输出参数  : 无
 *
 * 返 回 值  : 无
 *
 * 修改记录  : 2012年2月4日 c61362 creat
 *****************************************************************************/
 BSP_VOID BSP_GUIPC_IpcIntHandler();

/*****************************************************************************
 * 函 数 名  : BSP_GUIpc_SemIntHandler
 *
 * 功能描述  : 信号量释放中断处理函数
 *
 * 输入参数  : 无  
 * 输出参数  : 无
 *
 * 返 回 值  : 无
 *
 * 修改记录  : 2012年2月4日 c61362 creat
 *****************************************************************************/
 BSP_VOID BSP_GUIPC_SemIntHandler();

/*****************************************************************************
 * 函 数 名  : BSP_GUIPC_IntCombine
 *
 * 功能描述  : 组合中断处理函数
 *
 * 输入参数  : 无  
 * 输出参数  : 无
 *
 * 返 回 值  : 无
 *
 * 修改记录  : 2012年2月4日 c61362 creat
 *****************************************************************************/
 BSP_VOID BSP_GUIPC_IntCombine();

#endif

#ifdef __cplusplus
}
#endif

#endif /* end #define _BSP_IPC_H_*/

