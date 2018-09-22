/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_HUAWEI_INFO.h
*
*   ��    �� :  z67193
*
*   ��    �� :  BSP���Ͷ���ͷ�ļ�
*
*   �޸ļ�¼ :  2012��04��14��  v1.00  z67193  ����
*************************************************************************/

#ifndef	__BSP_HUAWEI_INFO_H__
#define __BSP_HUAWEI_INFO_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



#if 0
/* Ӳ����Ʒ��Ϣ���� */
#define HW_VER_INVALID              (BSP_U16)0xFFFF
/* Porting���P500����� */
#define HW_VER_PRODUCT_PORTING      (BSP_U16)0xFFFE
#define HW_VER_PRODUCT_PV500        (BSP_U16)0xFFFD

#if defined (CHIP_BB_6920ES)
/* ���԰� */
#define HW_VER_PRODUCT_UDP      (BSP_U16)0x0000

/* Stick */
#define HW_VER_PRODUCT_E392S_U      (BSP_U16)0x0200
#define HW_VER_PRODUCT_E3276S_150   (BSP_U16)0x0201

/* E5 */
#define HW_VER_PRODUCT_E5_SBM       (BSP_U16)0x0401

/*E5776*/
#define HW_VER_PRODUCT_E5776_EM     (BSP_U16)0x0400

#elif defined (CHIP_BB_6920CS)
/* ���԰� */
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

/*�Ӱ汾�Ŷ���*/
#define HW_VER_SUB_VA				(BSP_U16)0
#define HW_VER_SUB_VB				(BSP_U16)1
#define HW_VER_SUB_VC				(BSP_U16)2
#define HW_VER_SUB_VD				(BSP_U16)3
#define HW_VER_SUB_VE				(BSP_U16)5
#define HW_VER_SUB_VF				(BSP_U16)6
#define HW_VER_SUB_VG				(BSP_U16)7
#define HW_VER_SUB_VH				(BSP_U16)8

/*
��Bitλ��ʾ����֧�ֵ�Ƶ�Σ� WCDMA��GSM����һ��32λ������ÿһλ��Ӧһ��band��
*/

    /*
    ��Bitλ��ʾ����֧�ֵ�Ƶ�Σ�bit1����Ƶ��I,bit2����Ƶ��II,
    ��������,����λΪ1,��ʾ֧�ָ�Ƶ��.�±��Ǳ���λ��Ƶ�ζ�Ӧ��ϵ:
    -------------------------------------------------------------------------------
            bit8       bit7      bit6     bit5    bit4     bit3      bit2     bit1
    -------------------------------------------------------------------------------
    WCDMA   900(VIII)  2600(VII) 800(VI)  850(V)  1700(IV) 1800(III) 1900(II) 2100(I)
            spare      spare     spare    spare   spare    spare     spare   J1700(IX)
    -------------------------------------------------------------------------------
    GSMƵ�� 1900(VIII) 1800(VII) E900(VI) R900(V) P900(IV) 850(III)  480(II)  450(I)
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
��Bitλ��ʾ����֧�ֵ�Ƶ�Σ� LTEƵ�ι�64����ʹ������U32λ������ʾ��ÿһλ��Ӧһ��band��
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
 * ���ݽṹ                                                     *
 *--------------------------------------------------------------*/


//BSP_U16 BSP_HwGetVerMain( BSP_VOID );
BSP_U16 BSP_HwGetVerSub( BSP_VOID );


/*****************************************************************************
* �� �� ��  : BSP_GetProductName
*
* ��������  : ��ȡ��Ʒ����
*
* �������  : char* pProductName,�ַ���ָ�룬��֤��С��32�ֽ�
*                             BSP_U32 ulLength,����������
* �������  : ��
*
 �� �� ֵ  : 0����ȷ����0: ʧ��
*
* �޸ļ�¼  : 2011-3-30 wuzechun creat
*
*****************************************************************************/
//BSP_S32 BSP_GetProductName (char * pProductName, unsigned int ulLength);

/*****************************************************************************
* �� �� ��  : BSP_GetProductInnerName
*
* ��������  : ��ȡ�ڲ���Ʒ����
*
* �������  : char* pProductIdInter,�ַ���ָ��
*                             BSP_U32 ulLength,����������
* �������  : ��
*
 �� �� ֵ  : 0����ȷ����0: ʧ��
*
* �޸ļ�¼  : 2011-3-29 wuzechun creat
*
*****************************************************************************/
BSP_S32 BSP_GetProductInnerName (char * pProductIdInter, unsigned int ulLength);

/*****************************************************************************
* �� �� ��  : BSP_GetProductDloadName
*
* ��������  : ��ȡ��Ʒ���ذ汾���ƣ�һ�������ݴ��ж��Ƿ���������
*
* �������  : ��
* �������  : ��
*
* �� �� ֵ  : ��Ʒ���ذ汾�����ַ���ָ��
*
* �޸ļ�¼  : 2011-3-30 wuzechun creat
*
*****************************************************************************/
BSP_S32 BSP_GetProductDloadName(char * pProductDloadName, unsigned int ulLength);


/*****************************************************************************
* �� �� ��  : BSP_GetSupportBandsForGu
*
* ��������  : ��ȡGUģ֧�ֵ�Ƶ��
*
* �������  : pusWBands - WCDMA��Ƶ��ֵ
             pusGBands - GSM��Ƶ��ֵ

 �������  : pusWBands - WCDMA��Ƶ��ֵ
             pusGBands - GSM��Ƶ��ֵ

 �� �� ֵ  : 0����ȷ����0: ʧ��
             ͨ��Bitλ����ʾ��ЩƵ��֧�֡�
*
*****************************************************************************/

int BSP_GetSupportBandsForGu(unsigned short *pusWBands, unsigned short *pusGBands);




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_HUAWEI_INFO_H__ */


