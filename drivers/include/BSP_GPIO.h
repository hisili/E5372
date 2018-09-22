/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_GPIO.h
*
*   ��    �� :  z67193
*
*   ��    �� :  BSP���Ͷ���ͷ�ļ�
*
*   �޸ļ�¼ :  2012��04��14��  v1.00  z67193  ����
*************************************************************************/

#ifndef	__BSP_GPIO_H__
#define __BSP_GPIO_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */

/* �жϴ�������ö��*/
typedef enum tagGPIO_INT_TRIG_E
{
    GPIO_HIGH_LEVEL          	=  1,/* �ߵ�ƽ����*/
    GPIO_LOW_LEVEL           	=  2,/* �͵�ƽ����*/
    GPIO_BOTH_EDGE           	=  3,/* ˫�ش���*/
    GPIO_RISING_EDGE        	=  4,/* �����ش���*/
    GPIO_DECENDING_EDGE    	    =  5 /* �½��ش���*/
} GPIO_INT_TRIG_E;

/* PV500����2��GPIO */
typedef enum tagGPIO_GPIO_NUM_E
{
    GPIO_DEF_GPIO0          	=  0,
    GPIO_DEF_GPIO1           	=  1,
#if ((defined (CHIP_BB_6920ES)||defined (CHIP_BB_6920CS)) \
    && (defined (BOARD_SFT)||defined(BOARD_ASIC)||defined(BOARD_ASIC_BIGPACK)))
	GPIO_DEF_GPIO2				,
	GPIO_DEF_GPIO3				,
	GPIO_DEF_GPIO4				,
	GPIO_DEF_GPIO5				,
    #elif defined (VERSION_V3R2)
	GPIO_DEF_GPIO2,
    GPIO_DEF_GPIO3,
    GPIO_DEF_GPIO4,
    GPIO_DEF_GPIO5,
    GPIO_DEF_GPIO6,
    GPIO_DEF_GPIO7,
    GPIO_DEF_GPIO8,
    GPIO_DEF_GPIO9,
    GPIO_DEF_GPIO10,
    GPIO_DEF_GPIO11,
	#endif
    GPIO_DEF_MAX  
} GPIO_GPIO_NUM_E;

/* ��ѯģʽ */
typedef enum tagGPIO_GPIO_GETMODE_NUM_E
{
    LINE_STATE          	=  0,
    FORCE_LOAD_CTRL       =  1
} GPIO_GETMODE_NUM_E;


#ifdef BSP_CPE
typedef enum tagCPE_SIGNAL_INTENSITY_E
{ 
	SIGNALNULL,
    SIGNALLOW,
    SIGNALMIDDLE,
    SIGNALHIGH,
    SIGNALMAX
}CPE_SIGNAL_INTENSITY_E;

typedef enum tagCPE_SIGNAL_STATUS_E
{
     SIGNAL_STATUS_ABNORMAL,    //�ź��쳣
     SIGNAL_STATUS_NORMAL,      //�ź�����
     SIGNAL_STATUS_MAX
}CPE_SIGNAL_STATUS_E;

typedef enum tagGPIO_CTRL_E
{
    GPIO_ON,
    GPIO_OFF,
    GPIO_MAX
}GPIO_CTRL_E;

#endif

/*****************************************************************************
* �� �� ��  : BSP_GPIO_Init
*
* ��������  : GPIO��ʼ���ӿڣ��ҽ��жϷ������
*
* �������  : BSP_VOID  
* �������  : ��
*
* �� �� ֵ  : ��
*
* �޸ļ�¼  : 2009��3��5��   liumengcun  creat
*****************************************************************************/
extern  BSP_VOID  BSP_GPIO_Init (BSP_VOID);

/*****************************************************************************
* �� �� ��  : BSP_GPIO_DrcSet
*
* ��������  : ����GPIO���ŵķ���
*
* �������  : BSP_U32 u32Mask        ָ�������õ�GPIOλ
*             BSP_U32 u32Directions  �����õķ���ֵ,0�����룻1�����
* �������  : ��
*
* �� �� ֵ  : ��
*
* �޸ļ�¼  : 2009��3��5��   liumengcun  creat
*****************************************************************************/
extern  BSP_S32  BSP_GPIO_DrcSet(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 u32Mask, BSP_U32 u32Directions);

/*****************************************************************************
* �� �� ��  : BSP_GPIO_DrcGet
*
* ��������  : ��ѯGPIO ���ŵķ���
*
* �������  : ��
* �������  : BSP_U32 * pu32Directions  ���淵�صķ���ֵ
*
* �� �� ֵ  : OK&ERROR
*
* �޸ļ�¼  : 2009��3��5��   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_DrcGet(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 * pu32Directions);

/*****************************************************************************
* �� �� ��  : BSP_GPIO_ValSet
*
* ��������  : ����GPIO ���ŵĵ�ƽֵ
*
* �������  : BSP_U32 u32Mask  ָ�������õ�GPIOλ
*             BSP_U32 u32Data  �����õĵ�ƽֵ
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* �޸ļ�¼  : 2009��3��5��   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_ValSet(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 u32Mask, BSP_U32 u32Data);

/*****************************************************************************
* �� �� ��  : BSP_GPIO_ValGet
*
* ��������  : ��ѯGPIO���ŵĵ�ƽֵ
*
* �������  : ��
* �������  : BSP_U32 * pu32Data  ���淵�صĵ�ƽֵ
*
* �� �� ֵ  : OK&ERROR
*
* �޸ļ�¼  : 2009��3��5��   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_ValGet(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 * pu32Data);

/*****************************************************************************
* �� �� ��  : BSP_GPIO_IntMaskSet
*
* ��������  : �����ж�λ
*
* �������  : BSP_U32 u32Mask  ָ�������жϵ�GPIOλ
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* �޸ļ�¼  : 2009��3��5��   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_IntMaskSet(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 u32Mask);  

/*****************************************************************************
* �� �� ��  : BSP_GPIO_IntUnMaskSet
*
* ��������  : ʹ���ж�λ
*
* �������  : BSP_U32 u32Mask  ָ��ʹ���жϵ�GPIOλ
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* �޸ļ�¼  : 2009��3��5��   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_IntUnMaskSet(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 u32Mask);

/*****************************************************************************
* �� �� ��  : BSP_GPIO_IntStateGet
*
* ��������  : ��ѯ�ж�״̬λ
*
* �������  : BSP_U32 *pu32Stat  
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* �޸ļ�¼  : 2009��3��5��   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_IntStateGet(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 *pu32Stat);

/*****************************************************************************
* �� �� ��  : BSP_GPIO_RawIntStateGet
*
* ��������  : ��ѯԭʼ�ж�״̬λ
*
* �������  : ��
* �������  : BSP_U32 *pu32RawStat  ���淵�ص�ԭʼ�ж�״̬
*
* �� �� ֵ  : OK&ERROR
*
* �޸ļ�¼  : 2009��3��5��   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_RawIntStateGet(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 *pu32RawStat);

/*****************************************************************************
* �� �� ��  : BSP_GPIO_IntStateClear
*
* ��������  : ����ж�״̬λ����1��ʾ��Ӧ�ж�λ����ж�
*
* �������  : BSP_U32 u32Mask  ָ���������GPIOλ
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* �޸ļ�¼  : 2009��3��5��   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_IntStateClear(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 u32Mask);

/*****************************************************************************
* �� �� ��  : BSP_GPIO_IntTriggerSet
*
* ��������  : �����жϴ�����ʽ�������ء��½��ء�˫���ء��ߵ�ƽ���͵�ƽ
*
* �������  : BSP_U32 u32Mask             ָ�������õ�GPIOλ
*             GPIO_INT_TRIG enTrigMode  �жϴ���ģʽ
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* �޸ļ�¼  : 2009��3��5��   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_IntTriggerSet(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 u32Mask, GPIO_INT_TRIG_E enTrigMode);

/*****************************************************************************
* �� �� ��  : BSP_GPIO_IntInstall
*
* ��������  : ��������ģ��ע��GPIO�жϴ������Ľӿ�
*
* �������  : BSP_U32 u32Mask  ָ��GPIOλID
*             BSP_VOID* routine  ��ע��Ļص�����ָ��
*             BSP_S32 para       �ص���������
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* �޸ļ�¼  : 2009��3��5��   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_IntInstall(GPIO_GPIO_NUM_E enGPIONum, BSP_U32 u32Mask, BSP_VOID* routine,BSP_S32 para);

/*****************************************************************************
* �� �� ��  : BSP_GPIO_IntUnInstall
*
* ��������  : ��������ģ��ж��GPIO�жϴ������Ľӿ�
*
* �������  : BSP_U32 u32Mask  ָ��GPIOλID
* �������  : ��
*
* �� �� ֵ  : OK&ERROR
*
* �޸ļ�¼  : 2009��3��5��   liumengcun  creat
*****************************************************************************/
extern  BSP_S32 BSP_GPIO_IntUnInstall (GPIO_GPIO_NUM_E enGPIONum, BSP_U32 u32Mask);




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_GPIO_H__ */


