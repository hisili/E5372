/*************************************************************************
*   ��Ȩ����(C) 1987-2004, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  GUIpc.h
*
*   ��    �� :  c61362
*
*   ��    �� :  IPCģ���û��ӿ��ļ�
*
*   �޸ļ�¼ :  2012��2��4��  v1.00  c61362  ����
*************************************************************************/

#ifndef _BSP_GUIPC_H_
#define _BSP_GUIPC_H_

#ifdef __cplusplus
extern "C" {
#endif

#if ((defined (CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)) && defined (BOARD_FPGA_P500))       /*ֻ��V7R1 FPGAƽ̨���ṩ*/

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
* �� �� ��  : BSP_DRV_IPCIntInit
*
* ��������  : IPCģ���ʼ��
*
* �������  : ��  
* �������  : ��
*
* �� �� ֵ  : ��
*
* �޸ļ�¼  : 2012��2��4�� c61362 creat
*****************************************************************************/
BSP_S32 BSP_GUIPC_Init();


/*****************************************************************************
 * �� �� ��  : BSP_GUIPC_IpcIntHandler
 *
 * ��������  : �жϴ�����
 *
 * �������  : ��  
 * �������  : ��
 *
 * �� �� ֵ  : ��
 *
 * �޸ļ�¼  : 2012��2��4�� c61362 creat
 *****************************************************************************/
 BSP_VOID BSP_GUIPC_IpcIntHandler();

/*****************************************************************************
 * �� �� ��  : BSP_GUIpc_SemIntHandler
 *
 * ��������  : �ź����ͷ��жϴ�����
 *
 * �������  : ��  
 * �������  : ��
 *
 * �� �� ֵ  : ��
 *
 * �޸ļ�¼  : 2012��2��4�� c61362 creat
 *****************************************************************************/
 BSP_VOID BSP_GUIPC_SemIntHandler();

/*****************************************************************************
 * �� �� ��  : BSP_GUIPC_IntCombine
 *
 * ��������  : ����жϴ�����
 *
 * �������  : ��  
 * �������  : ��
 *
 * �� �� ֵ  : ��
 *
 * �޸ļ�¼  : 2012��2��4�� c61362 creat
 *****************************************************************************/
 BSP_VOID BSP_GUIPC_IntCombine();

#endif

#ifdef __cplusplus
}
#endif

#endif /* end #define _BSP_IPC_H_*/

