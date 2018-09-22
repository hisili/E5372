/*************************************************************************
*   ��Ȩ����(C) 2008-2012, ���ڻ�Ϊ�������޹�˾.
*
*   �� �� �� :  BSP_WIFI.h
*
*
*   ��    �� :  BSP���Ͷ���ͷ�ļ�
*
*************************************************************************/

#ifndef	__BSP_WIFI_H__
#define __BSP_WIFI_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */

void DRV_HSIC_Release(void);
void DRV_HSIC_ControllerPhyCutOff(void);
void DRV_HSIC_ControllerPhyRelease(void);
void BCM43239_WIFI_Release(void);
void BCM43239_WIFI_PowerDown(void);
void BCM43239_WIFI_PowerOn(void);

/*************************************************
*  Function:  wifi_athtestcmd
*  Description: WIFI Control API
*  Called By:sysctrl
*  Table Accessed:
*  Table Updated:
*  Input:
*          cmdStr:     ����WIFI����������
*  Output:
*         N/A
*  Return:
*         ����ִ�е�״̬���ֵ��
*************************************************/
int wifi_tcmd(char *tcmd_str);

/*************************************************
*  Function:  wifi_get_tcmd_mode
*  Description: ��ѯWIFI״̬
*  Called By:sysctrl
*  Table Accessed:
*  Table Updated:
*  Input:
*          ��
*  Output:
*         16��У׼����ģʽ
          17����ʾ���ڷ���ģʽ
          18����ʾ����ģʽ

*  Return:
*         ����ִ�е�״̬���ֵ��
*************************************************/
int wifi_get_tcmd_mode(void);


/*****************************************************************
Function: wifi_power_on
Description:
    start wifi
Input:
    N/A    
Output:
    N/A
Return:
    0: execute ok 
    1: execute failed
*******************************************************************/
int wifi_power_on();


/*****************************************************************
Function: wifi_power_off
Description:
    shutdown wifi
Input:
    N/A    
Output:
    N/A
Return:
    0: execute ok 
    1: execute failed
*******************************************************************/
int wifi_power_off(void);

/*****************************************************************
Function: wifi_get_status
Description:
    get wifi current mode
Input:
    N/A    
Output:
    N/A
Return:
    0: wifi is off
    1: wifi is in normal mode
    2: wifi is in tcmd mode
*******************************************************************/

extern int wifi_get_status(void);

/*****************************************************************
Function: wifi_get_rx_detail_report
Description:
    get result of rx report: totalPkt, GoodPkt, ErrorPkt
Input:
    N/A    
Output:
     totalPkt, GoodPkt, ErrorPkt
Return:
    N/A
*******************************************************************/
//extern void wifi_get_rx_detail_report(int* totalPkt,int* goodPkt,int* badPkt);
extern void  wifi_get_rx_detail_report(unsigned int *totalPkts, unsigned int *googPkts, unsigned int * badPkts);


/*****************************************************************
Function: wifi_get_rx_packet_report
Description:
    get result of rx ucast&mcast packets
Input:
    N/A    
Output:
     ucast,mcast
Return:
    N/A
*******************************************************************/
extern void  wifi_get_rx_packet_report(unsigned int *ucastPkts, unsigned int *mcastPkts);

/*****************************************************************
Function: wifi_set_pa_mode
Description:
  
Input:
    N/A    
Output:
     ucast,mcast
Return:
    N/A
*******************************************************************/

extern int wifi_set_pa_mode(int wifiPaMode);
extern int wifi_get_pa_cur_mode();


/*****************************************************************************
 �� �� ��  : WifiDataReservedTail
 ��������  : WIFI������Ҫ���ݿ��ڴ�
 �������  : usLen - �û��������ݳ���Len
 �������  : ��
 �� �� ֵ  : ��������β��Ԥ���ĳ���
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2011��12��22��
    �޸�����   : �����ɺ���

*****************************************************************************/
extern unsigned int BSP_WifiDataReservedTail(unsigned int len);

/*****************************************************************************
*  Function:  drv_get_local_usr_connect_status
*  Description: ���ص�ǰ�Ƿ���USB���ӻ���WIFI�û�����
*  Called By:sysctrl
*  Table Accessed:
*  Table Updated:
*  Input:
*  Output:
*  Return:
*         1 : ��
*         0 : ��
*************************************************/
extern unsigned long  drv_get_local_usr_connect_status(void);

/*h00106354 20120201 �������ؽӿ� add start */
unsigned long WiFi_DrvSetRxFlowCtrl    (unsigned long para1, unsigned long para2);

unsigned long WiFi_DrvClearRxFlowCtrl  (unsigned long para1, unsigned long para2);
/*h00106354 20120201 �������ؽӿ� add end */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_WIFI_H__ */


