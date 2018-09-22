/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_HKADC.h
*
*
*   描    述 :  BSP类型定义头文件
*
*************************************************************************/

#ifndef	__BSP_HKADC_H__
#define __BSP_HKADC_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/*****************************HKADC接口************************************************/
/*************************************************
*  Function:  hkadcLtePATempRead
*  Description: Read lte pa temperature *
*  Called By:sysctrl
*  Table Accessed:
*  Table Updated:
*  Input:
*          
*  Output:
*         pslData:对应hkadc通道的温度值
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
*         pslData:对应hkadc通道的温度值
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
*         pslData:对应hkadc通道的温度值
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
*         pslData:对应hkadc通道的温度值
*  Return:
*         HKADC_OK/HKADC_ERROR
*************************************************/
int hkadcBatTempRead(int *pslData, unsigned short *hkAdcTable);

/*************************************************
*  Function:  hkadcBatRefTempRead
*  Description: 通过电池自身NTC电阻来读温度，用来判断电池是否在位*
*  Called By:sysctrl
*  Table Accessed:
*  Table Updated:
*  Input:*          
           hkAdcTable:  电压温度换算表；
*  Output:
*         pslData:     存储的电压转换为的温度值；
*  Return:
*         函数执行的状态结果值；
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
*  Description:  * 设置Battery最低和最高电压对应的adc值
*  Called By:sysctrl
*  Table Accessed:
*  Table Updated:
*  Input:
*        minVol:最低电压，minAdc：最低电压对应的adc值，
*	 maxVol:最高电压，maxAdc：最高电压对应的adc值
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
*         pslData : 电池电压值
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
*         pslData:对应hkadc通道adc值
*  Return:
*         HKADC_OK/HKADC_ERROR；
*************************************************/
int hkadcBatADCRead(int* pslData);


/*************************************************
 函 数 名   : BSP_HKADC_BatVoltGet
 功能描述   : 返回当前电池电压值
 输入参数   : pslData : 电池电压值
 输出参数   : pslData : 电池电压值
 返 回 值   :0:获取成功
            -1:获取失败
*************************************************/
extern BSP_S32 BSP_HKADC_BatVoltGet(BSP_S32 *ps32Data);


/*****************************************************************************
 函 数 名  : hkadcBatADCRead
 功能描述  : 获取电池电压采样值
 输入参数  : 无
 输出参数  : pTemp：        指向电池温度的指针。
 返 回 值  : 0:  操作成功；
             -1：操作失败。
*****************************************************************************/
extern BSP_S32 BSP_HKADC_BatADCRead(BSP_S32 * pslData);




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_HKADC_H__ */


