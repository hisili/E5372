/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_SCI.h
*
*   ��    �� :  z67193
*
*   ��    �� :  BSP���Ͷ���ͷ�ļ�
*
*   �޸ļ�¼ :  2012��04��14��  v1.00  z67193  ����
*************************************************************************/

#ifndef	__BSP_SCI_H__
#define __BSP_SCI_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


typedef void (*USIMMSCIInfo)(void);
typedef BSP_U32 (*Sci_Event_Func)(BSP_U32 u32Event, BSP_VOID* para);

/*������*/
#define BSP_ERR_SCI_NOTINIT              951
#define BSP_ERR_SCI_INVALIDFREQ          952
#define BSP_ERR_SCI_USEDEFAULT           953
#define BSP_ERR_SCI_INVALIDPARA          954
#define BSP_ERR_SCI_NOCARD               955
#define BSP_ERR_SCI_NODATA               956
#define BSP_ERR_SCI_NOTREADY             957
#define BSP_ERR_SCI_DISABLED             958
#define BSP_ERR_SCI_UNSUPPORTED          959
#define BSP_ERR_SCI_INSERROR             960 
#define BSP_ERR_SCI_GETAPDU_ERROR        961
#define BSP_ERR_SCI_VLTG_HIGHEST         962
#define BSP_ERR_SCI_CURRENT_STATE_ERR    963
#define BSP_ERR_SCI_CURRENT_VLTG_ERR     964
#define BSP_ERR_SCI_TIMESTOP_TIMEOUT     965


/*�ص�����ע���¼�����*/
#define SCI_EVENT_CARD_IN                0x1
#define SCI_EVENT_CARD_OUT               0x2
#define SCI_EVENT_CARD_DEACTIVE_SUCCESS  0x4
#define SCI_EVENT_CARD_RESET_SUCCESS     0x8
#define SCI_EVENT_CARD_READ_DATA         0x10
#define SCI_EVENT_CARD_TX_ERR            0x20

#define SCI_DEF_CARD_OK     0x0 /* ������������ʹ��*/
#define SCI_DEF_CARD_ERROR  0x1   /* �޿����߿��쳣*/


typedef enum tagSCI_CLK_STOP_TYPE_E
{
    SCI_CLK_STATE_LOW = 0,        /* Э���ָʾͣ��*/
    SCI_CLK_STATE_HIGH = 1,       /*Э���ָʾͣ��*/
    SCI_CLK_STATE_UNSUPPORTED = 2,/*Э���ָʾ��֧��*/
    SCI_CLK_STATE_BUTT
}SCI_CLK_STOP_TYPE_E;


/*****************************************************************************
* �� �� ��  : BSP_SCI_SendData
*
* ��������  : ���ӿ����ڷ���һ�����ݵ�USIM��
*
* �������  : BSP_U32 u32DataLength �������ݵ���Ч���ȡ�ȡֵ��Χ1��256����λΪ�ֽ�  
*             BSP_U8 *pu8DataBuffer �������������ڴ���׵�ַ�����Ƕ�̬���䣬���ýӿ�
*                                   ���������ͷţ�������Ӳ����������
* �������  : ��
*
* �� �� ֵ  : OK
*           BSP_ERR_SCI_NOTINIT
*           BSP_ERR_SCI_INVALIDPARA
*           BSP_ERR_SCI_DISABLED
*           BSP_ERR_SCI_NOCARD
*           BSP_ERR_SCI_NODATA
*
* �޸ļ�¼  : 2009��6��29��   liumengcun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_SendData(BSP_U32 u32DataLength, BSP_U8 *pu8DataBuffer);

/*****************************************************************************
* �� �� ��  : BSP_SCI_ReceiveSync
*
* ��������  : ���ӿ�����USIM Manager��ȡ����SCI Driver�Ŀ���������
*             �ýӿ�Ϊ�����ӿڣ�ֻ��SCI���յ��㹻����������Ż᷵�أ�
*             �ýӿڵĳ�ʱ����Ϊ1s
*
* �������  : BSP_U32 u32DataLength USIM Manager����SCI Driver��ȡ�����ݳ��ȡ�
* �������  : BSP_U8 *pu8DataBuffer USIM Managerָ����Buffer��SCI Driver�����ݿ�������Buffer��
* �� �� ֵ  : OK
*             BSP_ERR_SCI_NOTINIT
*             BSP_ERR_SCI_INVALIDPARA
*
* �޸ļ�¼  : 2009��6��29��   liumengcun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_ReceiveSync(BSP_U32 u32Length,BSP_U8 *pu8Data);

/*****************************************************************************
* �� �� ��  : BSP_SCI_ReceiveAll
*
* ��������  : ���ӿ�����USIM Manager�ڶ�����ʱ��ʱ�򣬵��ñ���������ȡ�������ݻ����е���������
*
* �������  : ��  
* �������  : BSP_U32 *u32DataLength Driver��ȡ�����ݳ��ȣ����ظ�USIM Manager��ȡֵ��Χ1��256����λ���ֽ�
*           BSP_U8 * pu8DataBuffer USIM Managerָ����Buffer��SCI Driver�����ݿ�������Buffer
*
* �� �� ֵ  : OK
*          BSP_ERR_SCI_NOTINIT
*          BSP_ERR_SCI_INVALIDPARA
*
* �޸ļ�¼  : 2009��6��29��   liumengcun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_ReceiveAll(BSP_U32 *u32DataLength,BSP_U8 * pu8DataBuffer);

/*****************************************************************************
* �� �� ��  : BSP_SCI_GetATR
*
* ��������  : ���ӿ����ڽ�Driver�㻺���ATR���ݺ����ݸ������ظ�USIM Manager��
*
* �������  : BSP_VOID  
* �������  : BSP_U8 *u8DataLength  Driver��ȡ��ATR���ݳ��ȣ����ظ�USIM Manager��
*                                   ȡֵ��Χ0��32����λ���ֽ�
*           BSP_U8 *pu8ATR          USIM Managerָ����Buffer��SCI Driver��ATR
*                                   ���ݿ�������Buffer��һ��Ϊ����ϵͳ������̬����
*                                   ���߾�̬����ĵ�ַ
* 
*
* �� �� ֵ  : OK
*          BSP_ERR_SCI_NOTINIT
*          BSP_ERR_SCI_INVALIDPARA
*
* �޸ļ�¼  : 2009��6��29��   liumengcun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_GetATR (BSP_U8 *u8DataLength, BSP_U8 *pu8ATR);

/*****************************************************************************
* �� �� ��  : BSP_SCI_ClassSwitch
*
* ��������  : ���ӿ�����֧��PS�Կ��ĵ�ѹ���ͽ����л�����1.8V�л���3V
*
* �������  : BSP_VOID  
* �������  : ��
*
* �� �� ֵ  :  OK    ��ǰ��ѹ������ߵģ����е�ѹ�л�����
*           BSP_ERR_SCI_CURRENT_STATE_ERR �л�ʧ�� current SCI driver state is ready/rx/tx 
*           BSP_ERR_SCI_VLTG_HIGHEST   ��ǰ��ѹ�Ѿ�����ߵ�ѹ��û�н��е�ѹ�л�
*           BSP_ERR_SCI_NOTINIT
*           BSP_ERR_SCI_CURRENT_VLTG_ERR ��ǰ��ѹֵ�쳣����class B����C��
*
* �޸ļ�¼  : 2009��6��29��   liumengcun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_ClassSwitch(BSP_VOID);

/*****************************************************************************
* �� �� ��  : BSP_SCI_TimeStop
*
* ��������  : ���ӿ�����֧��PS�ر�SIM��ʱ��
*
* �������  : 
*           SCI_CLK_STOP_TYPE_E enTimeStopCfg ʱ��ֹͣģʽ
*   
* �������  : ��
*
* �� �� ֵ  : OK - successful completion
*               ERROR - failed
*               BSP_ERR_SCI_NOTINIT
*               BSP_ERR_SCI_INVALIDPARA - invalid mode specified
*               BSP_ERR_SCI_UNSUPPORTED - not support such a operation
* �޸ļ�¼  : 2009��6��29��   liumengcun  creat
*
*****************************************************************************/
BSP_S32 BSP_SCI_TimeStop(SCI_CLK_STOP_TYPE_E enTimeStopCfg);


/*****************************************************************************
* �� �� ��  : BSP_SCI_RegEvent
*
* ��������  : ���ӿ�����ע��ص�����
*
* �������  : BSP_U32 u32Event      �¼����ͣ����������֣�
*                                    0x1���忨��
*                                    0x2���ο���
*                                    0x4�����µ磻
*                                    0x8�����ϵ磻
*                                    0x10�������ݣ�
*                                    0x20���������ݴ���
*              Sci_Event_Func * pFunc   �ص�����
* �������  : ��
*
* �� �� ֵ  : OK    �����ɹ�
*          BSP_ERR_SCI_INVALIDPARA
* �޸ļ�¼  : 2009��6��29��   liumengcun  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_RegEvent(BSP_U32 u32Event, Sci_Event_Func pFunc);


/*****************************************************************************
* �� �� ��  : BSP_SCI_RecordDataSave
*
* ��������  : ���ӿ����ڱ����ά�ɲ���Ϣ��yaffs��
*
* �������  : ��
*
* �������  : ��
*
* �� �� ֵ  : OK    �����ɹ�
*             ����   ����ʧ��
*
* �޸ļ�¼  : 2011��5��21��   yangzhi  creat
*
*****************************************************************************/
BSP_U32 BSP_SCI_RecordDataSave(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_SCI_H__ */


