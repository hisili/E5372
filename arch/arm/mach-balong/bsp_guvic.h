/*************************************************************************
*   ��Ȩ����(C) 1987-2004, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  GUIpc.h
*
*   ��    �� :  c61362
*
*   ��    �� :  GU�����ж�ģ���û��ӿ��ļ�
*
*   �޸ļ�¼ :  2012��2��4��  v1.00  c61362  ����
*************************************************************************/

#ifndef _BSP_GUVIC_H_
#define _BSP_GUVIC_H_

#ifdef __cplusplus
extern "C" {
#endif

#if ((defined (CHIP_BB_6920ES) || defined (CHIP_BB_6920CS)) && defined (BOARD_FPGA_P500))       /*ֻ��V7R1 FPGAƽ̨���ṩ*/

#ifndef BSP_VIC_REG_READ
#define BSP_VIC_REG_READ(reg,result) \
	((result) = *(volatile BSP_U32 *)(reg))
#endif /*BSP_VIC_REG_READ*/

#ifndef BSP_VIC_REG_WRITE
#define BSP_VIC_REG_WRITE(reg,data) \
	(*((volatile BSP_U32 *)(reg)) = (data))
#endif /*BSP_VIC_REG_WRITE*/

#define VIC_INT_LOCK(flags) \
    local_irq_save(flags)
#define VIC_INT_UNLOCK(flags) \
    local_irq_restore(flags)

#define GUFPGA_VIC_NUM                (32)
#define INT_LVL_GUFPGAVIC             (INT_LVL_MAX + 16)

#define GUFPGA_VIC_BASE               (0x5F060000)
#define GUFPGA_VIC_SIZE               (0x00001000)
#define GUFPGA_VIC_STATUS             (g_pGUVicBase + 0x000)
#define GUFPGA_VIC_ENABLE             (g_pGUVicBase + 0x010)
#define GUFPGA_VIC_DISABLE            (g_pGUVicBase + 0x014)
 
typedef struct 
{   
    VOIDFUNCPTR	    routine;
    unsigned int	arg;
}BSP_GUVIC_ENTRY;

BSP_VOID BSP_GUFPGA_IntInit();
BSP_S32 BSP_GUFPGA_IntEnable ( BSP_S32 s32Lvl);
BSP_S32 BSP_GUFPGA_IntDisable ( BSP_S32 s32Lvl);
BSP_S32 BSP_GUFPGA_IntConnect  (VOIDFUNCPTR * vector,VOIDFUNCPTR routine, BSP_S32 parameter);
BSP_S32 BSP_GUFPGA_IntDisConnect  (VOIDFUNCPTR * vector,VOIDFUNCPTR routine, BSP_S32 parameter);


#endif

#ifdef __cplusplus
}
#endif

#endif /* end #define _BSP_IPC_H_*/

