/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_WIFI.h
*
*
*   描    述 :  BSP类型定义头文件
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
*          cmdStr:     控制WIFI的命令码流
*  Output:
*         N/A
*  Return:
*         函数执行的状态结果值；
*************************************************/
int wifi_tcmd(char *tcmd_str);

/*************************************************
*  Function:  wifi_get_tcmd_mode
*  Description: 查询WIFI状态
*  Called By:sysctrl
*  Table Accessed:
*  Table Updated:
*  Input:
*          无
*  Output:
*         16：校准测试模式
          17：表示处于发射模式
          18：表示接收模式

*  Return:
*         函数执行的状态结果值；
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
 函 数 名  : WifiDataReservedTail
 功能描述  : WIFI计算需要数据块内存
 输入参数  : usLen - 用户申请数据长度Len
 输出参数  : 无
 返 回 值  : 数据区的尾部预留的长度
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2011年12月22日
    修改内容   : 新生成函数

*****************************************************************************/
extern unsigned int BSP_WifiDataReservedTail(unsigned int len);

/*****************************************************************************
*  Function:  drv_get_local_usr_connect_status
*  Description: 返回当前是否有USB连接或者WIFI用户连接
*  Called By:sysctrl
*  Table Accessed:
*  Table Updated:
*  Input:
*  Output:
*  Return:
*         1 : 有
*         0 : 无
*************************************************/
extern unsigned long  drv_get_local_usr_connect_status(void);

/*h00106354 20120201 合入流控接口 add start */
unsigned long WiFi_DrvSetRxFlowCtrl    (unsigned long para1, unsigned long para2);

unsigned long WiFi_DrvClearRxFlowCtrl  (unsigned long para1, unsigned long para2);
/*h00106354 20120201 合入流控接口 add end */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_WIFI_H__ */


