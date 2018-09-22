/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_PMU_API.h
*
*   ��    �� :  z67193
*
*   ��    �� :  BSP���Ͷ���ͷ�ļ�
*
*   �޸ļ�¼ :  2012��04��14��  v1.00  z67193  ����
*************************************************************************/

#ifndef	__BSP_PMU_API_H__
#define __BSP_PMU_API_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


/* ������ */
#define BSP_ERR_PMU_PARA_INVALID      BSP_DEF_ERR(BSP_MODU_SPI, 0x50)
#define BSP_ERR_PMU_ERROR_INIT        BSP_DEF_ERR(BSP_MODU_SPI, 0x51)	
#define BSP_ERR_PMU_SPI_NOT_INIT      BSP_DEF_ERR(BSP_MODU_SPI, 0x52)
#define BSP_ERR_PMU_NOT_INIT          BSP_DEF_ERR(BSP_MODU_SPI, 0x53)

#ifdef __VXWORKS__

/* PMU�˼�ͨ�Ź��� */

/*****************************************************************************
* �� �� ��  : BSP_PMU_CtrlCb
*
* ��������  : ��A�˵��õ�PMU���ƺ���
*
* �������  : 
* �������  : 
*
* �� �� ֵ  : 
*
* ����˵��  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_CtrlCb( BSP_VOID *pMsgBody,BSP_U32 u32Len);


/*****************************************************************************
* �� �� ��  : BSP_PMU_BuckOff
*
* ��������  : �ر�PMU��Buck
*
* �������  : BuckID, Buck�ţ���1��ʼ
* �������  : 
*
* �� �� ֵ  : 
*
* ����˵��  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_BuckOff( BSP_U8 BuckID );

/*****************************************************************************
* �� �� ��  : BSP_PMU_BuckOn
*
* ��������  : ��PMU��Buck
*
* �������  : BuckID, Buck�ţ���1��ʼ
* �������  : 
*
* �� �� ֵ  : 
*
* ����˵��  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_BuckOn( BSP_U8 BuckID );

/*****************************************************************************
* �� �� ��  : BSP_PMU_DROff
*
* ��������  : DR�򿪹رտ���
*
* �������  : DR_ID,DR���,��1��ʼ
* �������  : 
*
* �� �� ֵ  : 
*
* ����˵��  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_DROff(BSP_U8 DR_ID);

/*****************************************************************************
* �� �� ��  : BSP_PMU_DROn
*
* ��������  : DR��
*
* �������  : DR_ID,DR���,��1��ʼ
* �������  : 
*
* �� �� ֵ  : 
*
* ����˵��  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_DROn(BSP_U8 DR_ID);

/*****************************************************************************
* �� �� ��  : BSP_PMU_LDOVoltGet
*
* ��������  : ��ȡLDOͨ���ĵ�ѹֵ
*
* �������  : LdoID��LDOͨ��
*             pusVoltValue����ѹֵָ�룬����ʵ�ʵ�ѹ����100
* �������  : 
*
* �� �� ֵ  : 
*
* ����˵��  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_LDOVoltGet(BSP_U8 ucLDO);

/*****************************************************************************
* �� �� ��  : BSP_PMU_LDOVoltSet
*
* ��������  : ����PMU LDO��ѹ���ֵ
*
* �������  : LdoID��LDOͨ��
*             vltg����ѹֵ������ʵ�ʵ�ѹ����100
* �������  : 
*
* �� �� ֵ  : 
*
* ����˵��  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_LDOVoltSet(BSP_U8 LdoID, BSP_U16 vltg);

/*****************************************************************************
* �� �� ��  : BSP_PMU_BuckVoltSet
*
* ��������  : ����PMU BUCK��ѹ���ֵ
*
* �������  : buckID��BUCKͨ��
*             vltg����ѹֵ������ʵ�ʵ�ѹ����100
* �������  : 
*
* �� �� ֵ  : 
*
* ����˵��  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_BuckVoltSet(BSP_U8 buckID, BSP_U16 vltg);
/*****************************************************************************
* �� �� ��  : BSP_PMU_BuckVoltGet
*
* ��������  : ����PMU BUCK��ѹ���ֵ
*
* �������  : buckID��BUCKͨ��
*
* �������  : 
*
* �� �� ֵ  : 
* �� �� ֵ  : ��ѹֵ,����ʵ�ʵ�ѹ����100
*               -1,����
*
* ����˵��  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_BuckVoltGet(BSP_U8 buckID);

/*******************************************************************************
  Function:     BSP_PMU_DrCurrentSet
  Description:  �������ø�·����Դ���������

  Input:        Vs_ID: PMU����Դ��ţ�ȡֵ��Χ��PMU_ISRC���壺
                          DR1~DR3��
                ImA:      Ŀ�����ֵ����λ:mA����ȡֵ��Χ��
                          3mA,6mA,9mA,12mA,15mA,18mA,21mA,24mA
  Output:
  Return:       BSP_OK:    �����ɹ�
                BSP_ERROR: ����ʧ��
  Others:
*******************************************************************************/
int BSP_PMU_DrCurrentSet(int I_ID, BSP_U16 ImA);

/*******************************************************************************
  Function:     BSP_PMU_DrCurrentGet
  Description:  ���ڲ�ѯ��·����Դ���������

  Input:        I_ID: PMU����Դ��ţ�ȡֵ��Χ DR1~DR3 (0~2)
  Output:
  Return:       ��ѯ����Դ���������ֵ����λ:mA��
                Ϊ��ֵ��ERROR����ʾ��ѯʧ��
  Others:
*******************************************************************************/
int BSP_PMU_DrCurrentGet(int I_ID);

/*******************************************************************************
  Function:     BSP_PMU_RfAdjustVoltage
  Description:  Rf ��ѹ�ӿ�

  Input:        enRfVol: Ҫ�����Ĺ���(1.8V/2.2V)
  			u32Voltage:Ҫ���ɵĵ�ѹֵ
  Output:
  Return:      ����0Ϊ��ѹ�ɹ�������ʧ��
  Others:
*******************************************************************************/
BSP_U32 BSP_PMU_RfAdjustVoltage(RF_VOLTAGE_E enRfVol, BSP_U32 u32Voltage);

#endif


/**************************************************************************
  ȫ�ֱ�������
**************************************************************************/


/**************************************************************************
  ö�ٶ���
**************************************************************************/
/*DVFS type*/
typedef enum tagPMU_DVFS_TYPE_E
{
	DVFS_ARMCORE = 0,
	DVFS_CEVACORE,
	DVFS_BUTTOM 
}PMU_DVFS_TYPE_E;

/*DVFS Voltage type*/
typedef enum tagPMU_DVFS_VOLTAGE_TYPE_E
{
	DVFS_VOLTAGE_1000 = 0,
	DVFS_VOLTAGE_1100,
	DVFS_VOLTAGE_1150,
	DVFS_VOLTAGE_1200,
	DVFS_VOLTAGE_1250,
	DVFS_VOLTAGE_1350,
	DVFS_VOLTAGE_1500,
	DVFS_VOLTAGE_1800,
    DVFS_VOLTAGE_BUTTOM 
}PMU_DVFS_VOLTAGE_TYPE_E;
	
 /*DEVICE type*/
typedef enum tagPMU_DEVS_TYPE_E
{
	DEVS_RF = 0,
	DEVS_ABB,
	DEVS_SD,
	DEVS_SIM,
	DEVS_LCD,
    DEVS_BUTTOM 
}PMU_DEVS_TYPE_E;

	
/*DEVICE CTRL type*/
typedef enum tagPMU_DEVS_CTRL_TYPE_E
{
	CTRL_ON = 0,
	CTRL_OFF,
    CTRL_BUTTOM 
}PMU_DEVS_CTRL_TYPE_E;

 /*OCP type*/
typedef enum tagPMU_OCP_TYPE_E
{
	OCP_ONLYINT_NOAUTOOFF = 0,
	OCP_INT_AND_AUTOOFF,
	OCP_AUTOOFF,
    OCP_BUTTOM 
}PMU_OCP_TYPE_E;

 /* VOLTAGE ADJUST REG */
typedef enum tagPMU_VOL_SCI_E
{
	PMU_VOL_UP = 0, /* ��1.8��������3.0��*/
	PMU_VOL_DOWN,   /* ��3.0������1.8��*/
	PMU_VOL_SCI_BUTTOM
}PMU_VOL_SCI_E;

/* LED MODE */
typedef enum tagPMU_LED_STATE_E
{
	PMU_LED_FLASH_POWER_OFF = 0,    /* ȫ������ */
	PMU_LED_FLASH_BLUE_ON,          /* ���Ƴ��� */
	PMU_LED_FLASH_BLUE_SLOW,        /* �������� */
	PMU_LED_FLASH_BLUE_FAST,        /* ���ƿ��� */
	PMU_LED_FLASH_BLUE_ONCE,        /* ���Ƶ��� */
	PMU_LED_FLASH_BLUE_TWICE,       /* ����˫�� */
	PMU_LED_FLASH_BLUE_FOUR,        /* �������� */
	
	PMU_LED_FLASH_RED_ON,           /* ��Ƴ��� */
	PMU_LED_FLASH_RED_SLOW,         /* ������� */
	PMU_LED_FLASH_RED_FAST,         /* ��ƿ��� */
	PMU_LED_FLASH_RED_ONCE,         /* ��Ƶ��� */
	PMU_LED_FLASH_RED_TWICE,        /* ���˫�� */
	PMU_LED_FLASH_RED_FOUR,         /* ������� */
	
	PMU_LED_FLASH_GREEN_ON,         /* �̵Ƴ��� */
	PMU_LED_FLASH_GREEN_SLOW,       /* �̵����� */
	PMU_LED_FLASH_GREEN_FAST,       /* �̵ƿ��� */
	PMU_LED_FLASH_GREEN_ONCE,       /* �̵Ƶ��� */
	PMU_LED_FLASH_GREEN_TWICE,      /* �̵�˫�� */
	PMU_LED_FLASH_GREEN_FOUR,       /* �̵����� */
	
	PMU_LED_FLASH_CYANBLUE_ON,      /* ��Ƴ��� */
	PMU_LED_FLASH_CYANBLUE_SLOW,    /* ������� */
	PMU_LED_FLASH_CYANBLUE_FAST,    /* ��ƿ��� */
	PMU_LED_FLASH_CYANBLUE_ONCE,    /* ��Ƶ��� */
	PMU_LED_FLASH_CYANBLUE_TWICE,   /* ���˫�� */
	PMU_LED_FLASH_CYANBLUE_FOUR,    /* ������� */

	PMU_LED_STATE_BUTT
}PMU_LED_STATE_E;


/*****************************************************************************
* �� �� ��  : BSP_PMU_ShutDown
*
* ��������  : ���ӿ����ڹر�PMU
*
* �������  : 
* �������  : ��
*
* �� �� ֵ  :
* �޸ļ�¼  : 
*
*****************************************************************************/
BSP_VOID BSP_PMU_ShutDown(void);
/*****************************************************************************
* �� �� ��  : BSP_PMU_DVFS_VolAdjust
*
* ��������  : DVFS ��ѹ����
*
* �������  : 
* �������  : 
*
* �� �� ֵ  :
* �޸ļ�¼  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_DVFS_VolAdjust(PMU_DVFS_TYPE_E enDvfsType,PMU_DVFS_VOLTAGE_TYPE_E enVoltage);
/*****************************************************************************
* �� �� ��  : BSP_PMU_OCPConfig
*
* ��������  : OCPC����
*
* �������  : 
* �������  : 
*
* �� �� ֵ  :
* �޸ļ�¼  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_OCPConfig(PMU_OCP_TYPE_E enOcpType);
/*****************************************************************************
* �� �� ��  : BSP_PMU_DEVS_Ctrl
*
* ��������  : DEVS������
*
* �������  : 
* �������  : 
*
* �� �� ֵ  :
* �޸ļ�¼  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_DEVS_Ctrl(PMU_DEVS_TYPE_E enDevsType, PMU_DEVS_CTRL_TYPE_E enCtrlType);
/*****************************************************************************
* �� �� ��  : BSP_PMU_VolAdjust_ForSci
*
* ��������  : SCI��ѹ����
*
* �������  : 
* �������  : 
*
* �� �� ֵ  :
* �޸ļ�¼  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_VolAdjust_ForSci(PMU_VOL_SCI_E enSCIVolCfg);
/*****************************************************************************
* �� �� ��  : BSP_PMU_VolAdjust_ForSDIO
*
* ��������  : SDIO��ѹ����
*
* �������  : 
* �������  : 
*
* �� �� ֵ  :
* �޸ļ�¼  : 
*
*****************************************************************************/
BSP_S32 BSP_PMU_VolAdjust_ForSDIO(PMU_VOL_SCI_E enSCIVolCfg);

/*****************************************************************************
* �� �� ��  : BSP_PMU_CutOff_ForDrx
*
* ��������  : �͹��ĳ���PMU�µ�
*
* �������  : ��
* �������  : ��
*
* �� �� ֵ  :
* �޸ļ�¼  : ��ȷ/����
*
*****************************************************************************/
BSP_S32 BSP_PMU_CutOff_ForDrx(void);

/*****************************************************************************
* �� �� ��  : BSP_PMU_SetLED
*
* ��������  : ����LTD�ƵĿ���
*
* �������  : enLEDType         LED0
*                               LED1
*                               LED2
*             enPower           PMU_LED_POWER_ON
*                               PMU_LED_POWER_OFF
* �������  : ��
*
* �� �� ֵ  : OK�����óɹ�
*             ERROR������ʧ��
*****************************************************************************/
BSP_S32 BSP_PMU_SetLedState(PMU_LED_STATE_E eLedState);

/*****************************************************************************
 �� �� ��  : BSP_PMU_GetPMUState
 ��������  : ��ȡPMUģ�鿪����ʽ�����״̬����ص����������λ״̬��
 �������  : �ޡ�
 �������  : Pmu_State :������ʽ�����״̬����ص����������λ״̬��
 �� �� ֵ  : 0:  �����ɹ���
             -1������ʧ�ܡ�
 ע��������ýӿڽ���PS��PC������ʹ�ã�Ŀǰû��Ӧ�ã��ݱ�����
*****************************************************************************/
extern unsigned int BSP_PMU_GetPMUState(void* Pmu_State);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_PMUAPI_H__ */

