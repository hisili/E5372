/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_HKADC.h
*
*
*   ��    �� :  BSP���Ͷ���ͷ�ļ�
*
*************************************************************************/

#ifndef	__BSP_HKADC_H__
#define __BSP_HKADC_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/*****************************HKADC�ӿ�************************************************/
/*************************************************
*  Function:  hkadcLtePATempRead
*  Description: Read lte pa temperature *
*  Called By:sysctrl
*  Table Accessed:
*  Table Updated:
*  Input:
*          
*  Output:
*         pslData:��Ӧhkadcͨ�����¶�ֵ
*  Return:
*         HKADC_OK/HKADC_ERROR
*************************************************/
int hkadcLtePATempRead(int *pslData, unsigned short *hkAdcTable);

/*************************************************
*  Function:  hkadcSIMTempRead
*  Description: Read SIM temperature *
*  Called By:sysctrl
*  Table Accessed:
*  Table Updated:
*  Input:
*          
*  Output:
*         pslData:��Ӧhkadcͨ�����¶�ֵ
*  Return:
*         HKADC_OK/HKADC_ERROR
*************************************************/
int hkadcSIMTempRead(int *pslData, unsigned short *hkAdcTable);

/*************************************************
*  Function:  hkadcOLEDTempRead
*  Description: Read OLED temperature *
*  Called By:sysctrl
*  Table Accessed:
*  Table Updated:
*  Input:
*          
*  Output:
*         pslData:��Ӧhkadcͨ�����¶�ֵ
*  Return:
*         HKADC_OK/HKADC_ERROR
*************************************************/
int hkadcTftTempRead(int *pslData, unsigned short *hkAdcTable);

/*************************************************
*  Function:  hkadcBatTempRead
*  Description: Read battery temperature *
*  Called By:sysctrl
*  Table Accessed:
*  Table Updated:
*  Input:
*          
*  Output:
*         pslData:��Ӧhkadcͨ�����¶�ֵ
*  Return:
*         HKADC_OK/HKADC_ERROR
*************************************************/
int hkadcBatTempRead(int *pslData, unsigned short *hkAdcTable);

/*************************************************
*  Function:  hkadcBatRefTempRead
*  Description: ͨ���������NTC���������¶ȣ������жϵ���Ƿ���λ*
*  Called By:sysctrl
*  Table Accessed:
*  Table Updated:
*  Input:*          
           hkAdcTable:  ��ѹ�¶Ȼ����
*  Output:
*         pslData:     �洢�ĵ�ѹת��Ϊ���¶�ֵ��
*  Return:
*         ����ִ�е�״̬���ֵ��
*************************************************/
int hkadcBatRefTempRead(int *pslData, unsigned short *hkAdcTable);

/*************************************************
*  Function:  hkadcPaPowerRead
*  Description: Read gsm PA power *
*  Called By:sysctrl
*  Table Accessed:
*  Table Updated:
*  Input:
*          
*  Output:
*         N/A
*  Return:
*         N/A
*************************************************/
int hkadcPAPowerRead(int *pslData);


/*************************************************
*  Function:  hkadcTemVoltTrans
*  Description:  *
*  Called By:sysctrl
*  Table Accessed:
*  Table Updated:
*  Input:
*          
*  Output:
*         N/A
*  Return:
*         N/A
*************************************************/
signed short hkadcTemVoltTrans(unsigned char ucChan, unsigned short usVoltage, unsigned short *hkAdcTable);


/*************************************************
*  Function:  hkadcBatVoltRefSet
*  Description:  * ����Battery��ͺ���ߵ�ѹ��Ӧ��adcֵ
*  Called By:sysctrl
*  Table Accessed:
*  Table Updated:
*  Input:
*        minVol:��͵�ѹ��minAdc����͵�ѹ��Ӧ��adcֵ��
*	 maxVol:��ߵ�ѹ��maxAdc����ߵ�ѹ��Ӧ��adcֵ
*  Output:
*         N/A
*  Return:
*         HKADC_OK/HKADC_ERROR
*************************************************/
int hkadcBatVoltRefSet(int minVol, int minAdc, int maxVol, int maxAdc);

/*************************************************
*  Function:  hkadcBatVoltGet
*  Description:  *
*  Called By:sysctrl
*  Table Accessed:
*  Table Updated:
*  Input:
*        N/A  
*  Output:
*         pslData : ��ص�ѹֵ
*  Return:
*         HKADC_OK/HKADC_ERROR
*************************************************/
int hkadcBatVoltGet(int *pslData );



/*************************************************
*  Function:  hkadcBatADCRead
*  Description: Read the battery adc value 
*  Table Accessed:
*  Table Updated:
*  Input:
*         N/A
*  Output:
*         pslData:��Ӧhkadcͨ��adcֵ
*  Return:
*         HKADC_OK/HKADC_ERROR��
*************************************************/
int hkadcBatADCRead(int* pslData);


/*************************************************
 �� �� ��   : BSP_HKADC_BatVoltGet
 ��������   : ���ص�ǰ��ص�ѹֵ
 �������   : pslData : ��ص�ѹֵ
 �������   : pslData : ��ص�ѹֵ
 �� �� ֵ   :0:��ȡ�ɹ�
            -1:��ȡʧ��
*************************************************/
extern BSP_S32 BSP_HKADC_BatVoltGet(BSP_S32 *ps32Data);


/*****************************************************************************
 �� �� ��  : hkadcBatADCRead
 ��������  : ��ȡ��ص�ѹ����ֵ
 �������  : ��
 �������  : pTemp��        ָ�����¶ȵ�ָ�롣
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
*****************************************************************************/
extern BSP_S32 BSP_HKADC_BatADCRead(BSP_S32 * pslData);




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_HKADC_H__ */


