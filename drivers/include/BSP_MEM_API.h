/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_MEM_API.h
*
*   ��    �� :  z67193
*
*   ��    �� :  BSP���Ͷ���ͷ�ļ�
*
*   �޸ļ�¼ :  2012��04��14��  v1.00  z67193  ����
*************************************************************************/

#ifndef	__BSP_MEM_API_H__
#define __BSP_MEM_API_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/**************************************************************************
  �궨�� 
**************************************************************************/

#ifdef __KERNEL__

#ifdef BOARD_FPGA_P500
 
/*AXI*/
#define AXI_MEM_BASE_ADDR            	0x30000000
#define AXI_MEM_ADDR                    IO_ADDRESS(AXI_MEM_BASE_ADDR)
#define AXI_MEM_SIZE                    0x20000

#define PBXA9_DRAM_SHARE_PHYS           CORESHARE_MEM_ADDR
#define PBXA9_DRAM_SHARE_VIRT           IO_ADDRESS(PBXA9_DRAM_SHARE_PHYS)

#elif defined (BOARD_SFT)

/*AXI*/
#define AXI_MEM_BASE_ADDR               0x2ffc0000
#define AXI_MEM_ADDR                    IO_ADDRESS(AXI_MEM_BASE_ADDR)
#define AXI_MEM_SIZE                    0x40000

#define PBXA9_DRAM_SHARE_PHYS           GLOBAL_MEM_CORE_SHARE_ADDR
#define PBXA9_DRAM_SHARE_VIRT           IO_ADDRESS(PBXA9_DRAM_SHARE_PHYS)

#elif((defined(BOARD_ASIC) || defined (BOARD_ASIC_BIGPACK) || defined (BOARD_SFT)))
#define PBXA9_DRAM_SHARE_PHYS           CORESHARE_MEM_ADDR
#define PBXA9_DRAM_SHARE_VIRT           IO_ADDRESS(PBXA9_DRAM_SHARE_PHYS)

//#define TTFA9_DRAM_SHARE_PHYS        	GLOBAL_MEM_GU_RESERVED_ADDR
//#define TTFA9_DRAM_SHARE_VIRT        	GLOBAL_MEM_GU_RESERVED_ADDR_VIRT
	
#endif

#define IO_TYPE(var) ((ICC_SUB_CHAN_S *)IO_ADDRESS((unsigned int)(var)))
//#define PHY_TO_VIRT(var)  (((var) - GLOBAL_MEM_CORE_SHARE_ADDR) + GLOBAL_MEM_CORE_SHARE_ADDR_VIRT)
#define DDR_VIRT_TO_PHY(var)  (((unsigned int)(var) - PBXA9_DRAM_SHARE_VIRT) + PBXA9_DRAM_SHARE_PHYS)
#define DRV_PHY_TO_VIRT(var)  (((unsigned int)(var) - PBXA9_DRAM_SHARE_PHYS) + PBXA9_DRAM_SHARE_VIRT)

#endif


/**************************************************************************
  �ӿ�����
**************************************************************************/
BSP_S32 BSP_MEM_Init(VOID);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_MEM_API_H__ */


