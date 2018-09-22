/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_CHG_API.h
*
*   ��    �� :  z67193
*
*   ��    �� :  BSP���Ͷ���ͷ�ļ�
*
*   �޸ļ�¼ :  2012��04��14��  v1.00  z67193  ����
*************************************************************************/

#ifndef	__BSP_CHG_API_H__
#define __BSP_CHG_API_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */

typedef enum USB_CHARGE_TYPE_ENUM_tag
{
    USB_CHARGE_TYPE_NON_CHECK = -1,          /* ��δ�жϳ������������� */
    USB_CHARGE_TYPE_HW = 0,                      /* ��Ϊ�����             */
    USB_CHARGE_TYPE_NON_HW =1                  /* �ǻ�Ϊ�����           */
}USB_CHARGE_TYPE_ENUM;

#define TBAT_CHECK_INVALID 0x0FFFF

/* ���У׼�������ݽṹ?**/
typedef struct
{
    BSP_U16 min_value;
    BSP_U16 max_value;
}VBAT_CALIBART_TYPE;

int app_get_battery_state(BATT_STATE_T *battery_state);


/*****************************************************************************
 �� �� ��  : BSP_CHG_StateSet
 ��������  :ʹ�ܻ��߽�ֹ���
 �������  :ulState      0:��ֹ���
 						1:ʹ�ܳ��
 �������  :��
 ����ֵ��    ��
*****************************************************************************/
extern void BSP_CHG_StateSet(unsigned long ulState);

/************************************************************************
 * FUNCTION
 *       int getCbcState(unsigned char *pucBcs ,unsigned char  *pucBcl)
 * DESCRIPTION
 *       pucBcs
            0: ����ڹ���
            1: �������ӵ��δ����
            2: û����������
         pucBcl
            0:���������û����������
             100: ʣ������ٷֱ�
 * INPUTS
 *          
 * OUTPUTS: 
           pucBcs
           pucBcl
 * return:    0 Success, -1 failed     
 *******************************************************************/
int getCbcState(unsigned char  *pucBcs ,unsigned char   *pucBcl);
#define DRV_CBC_STATE_GET(pucbcs, pucbcl)   getCbcState(pucbcs, pucbcl)

/************************************************************************************
 *Function Name :  usb_charge_type   
 *Description   :  ��ȡ����USB�ߵ�����,�ж��ǻ�Ϊ�����,
                    ���Ƿǻ�Ϊ�����(�����ǻ�Ϊ���������ͨPC��)
 *Others        :  No
************************************************************************************/
USB_CHARGE_TYPE_ENUM usb_charge_type( void );

/************************************************************************************
 *Function Name :  usb_enum_completed   
 *Description   :  �ж�USB��ö���Ƿ����,
                        ��ɷ���(1),δ��ɷ���(0)
 *Others        :  No
************************************************************************************/
unsigned int usb_enum_completed( void );

/************************************************************************
 * FUNCTION
 *       usb_get_charger_status
 * DESCRIPTION
 *       ��ȡ���������λ״̬
 * INPUTS
 *       NONE
 * OUTPUTS
 *       ��λ�򷵻�1�����򷵻�0��
 *************************************************************************/
unsigned int usb_get_charger_status(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_CHG_API_H__ */


