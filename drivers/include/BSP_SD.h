/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_SD.h
*
*   ��    �� :  z67193
*
*   ��    �� :  BSP���Ͷ���ͷ�ļ�
*
*   �޸ļ�¼ :  2012��04��14��  v1.00  z67193  ����
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
* �� �� ��  : SDIO_check_card
*
* ��������  :
*
* �������  : BSP_S32 s32CardNo
* �������  :
*
* �� �� ֵ  :
*
* ����˵��  :
*
*****************************************************************************/
BSP_S32 SDIO_check_card(BSP_S32 s32CardNo);

/*****************************************************************************
* �� �� ��  : read_write_blkdata
*
* ��������  : 

* �������  : 

* �������  : 
* �� �� ֵ  : 
*
* �޸ļ�¼  : 

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
* �� �� ��  : sd_get_status
*
* ��������  : ����λ��ѯ
*
* �������  : void
* �������  : NA
*
* �� �� ֵ  : 0 : ��λ��-1: ����λ
*
* ����˵��  : NA
*
*****************************************************************************/
extern int sd_get_status(void);


/*****************************************************************************
* �� �� ��  : sd_get_capacity
*
* ��������  : ��������ѯ
*
* �������  : void
* �������  : NA
*
* �� �� ֵ  : 0 : ʧ�ܣ�>0: ������
*
* ����˵��  : NA
*
*****************************************************************************/
extern int sd_get_capacity(void);


/*****************************************************************************
* �� �� ��  : sd_transfer
*
* ��������  : ���ݴ���
*
* �������  : struct scatterlist *sg    ���������ݽṹ��ָ��
                            unsigned dev_addr   ��д���SD block ��ַ
                            unsigned blocks    ��д���block����
                            unsigned blksz      ÿ��block�Ĵ�С����λ�ֽ�
                            int wrflags    ��д��־λ��д:WRFlAG ; ��:RDFlAG
* �������  : NA
*
* �� �� ֵ  : 0 : �ɹ�������: ʧ��
*
* ����˵��  : NA
*
*****************************************************************************/
extern int sd_transfer(struct scatterlist *sg, unsigned dev_addr,unsigned blocks, unsigned blksz, int wrflags);
/*****************************************************************************
* �� �� ��  : sd_sg_init_table
*
* ��������  : SD������ݴ���sg list��ʼ��
*
* �������  :  const void *buf        ��������buffer��ַ
               unsigned int buflen    ��������buffer��С,С��32K��Ϊ512B��������;
                                      ����32K��Ϊ32KB�������������128K
                            
* �������  : NA
*
* �� �� ֵ  : 0 : �ɹ�������: ʧ��
*
* ����˵��  : NA
*
*****************************************************************************/
extern int sd_sg_init_table(const void *buf,unsigned int buflen);
/*****************************************************************************
* �� �� ��  : sd_multi_transfer
*
* ��������  : SD������ݴ��䡣
*
* �������  : unsigned dev_addr   ��д���SD block ��ַ
              unsigned blocks     ��д���block����
              unsigned blksz      ÿ��block�Ĵ�С����λ�ֽ�
              int write           ��д��־λ��д:1 ; ��:0
* �������  : NA
*
* �� �� ֵ  : 0 : �ɹ�������: ʧ��
*
* ����˵��  : NA
*
*****************************************************************************/
extern int sd_multi_transfer(unsigned dev_addr, unsigned blocks, unsigned blksz, int write);

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_SD_H__ */


