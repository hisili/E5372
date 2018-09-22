/*************************************************************************
*   ��Ȩ����(C) 1987-2004, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  huawei_info.h
*
*   ��    �� :  ����
*
*   ��    �� :  huawei_info.c ��ͷ�ļ�
*
*   �޸ļ�¼ :  2011��11��12��  v1.00  ����  ����
*
*************************************************************************/

#ifndef __HUAWEI_INFO_H__
#define __HUAWEI_INFO_H__

/*--------------------------------------------------------------*
 * �궨��                                                       *
 *--------------------------------------------------------------*/
 #define HW_VER_SUB_GPIO     GPIO_DEF_GPIO0
 
#define HW_VER_SUB_GPIO_0    4
#define HW_VER_SUB_GPIO_1    5
#define HW_VER_SUB_MASK     ((1<<HW_VER_SUB_GPIO_0) | (1<<HW_VER_SUB_GPIO_1))

#define HW_VER_SUB_A  0x0
#define HW_VER_SUB_B  0x1
#define HW_VER_SUB_C  0x2
#define HW_VER_SUB_D  0x3

#define HW_VER_ID_MAIN0_OFFSET     8
#define HW_VER_ID_MAIN0_MASK       (7<<HW_VER_ID_MAIN0_OFFSET)
#define HW_VER_ID_MAIN1_OFFSET     0
#define HW_VER_ID_MAIN1_MASK       (7<<HW_VER_ID_MAIN1_OFFSET)

#define HW_VER_MAKE_VER_MAINID(main0, main1)   \
    (((main0)<<HW_VER_ID_MAIN0_OFFSET) | ((main1)<<HW_VER_ID_MAIN1_OFFSET))
    
#define HW_VER_GET_MAIN0(hwid)  ((hwid & HW_VER_ID_MAIN0_MASK)>>HW_VER_ID_MAIN0_OFFSET)
#define HW_VER_GET_MAIN1(hwid)  ((hwid & HW_VER_ID_MAIN1_MASK)>>HW_VER_ID_MAIN1_OFFSET)

/*--------------------------------------------------------------*
 * ���ݽṹ                                                     *
 *--------------------------------------------------------------*/

/*--------------------------------------------------------------*
 * ����ԭ������                                                 *
 *--------------------------------------------------------------*/

BSP_S32 BSP_PrintHwVersion(BSP_VOID);
BSP_S32 BSP_ProductInfoInit( BSP_VOID );

#endif /* huawei_info.h */

