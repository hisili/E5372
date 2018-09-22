/*
 *  code for wifi at interface utility
 *
 */

#ifndef _wlu_wifi_at_
#define _wlu_wifi_at_

#define WIFI_AT_CMDBUF_LEN  (50)
extern int g_iAtDevFd;
extern char  wifi_at_cmdbuf[WIFI_AT_CMDBUF_LEN];


#define WIFI_AT_INTERFACE

#define  WIFI_AT_DEVICE         "/sys/devices/platform/wifi_at_dev/wifi_at_dev"
#define  WIFI_CONSOLE_DEVICE    "/dev/console"
#define  WL_CMD_START           "0"  /*wl_cmd_start*/
#define  WL_RX_CMD              "1"  /*wl_rx_cmd*/
#define  WL_TX_CMD              "2"  /*wl_tx_cmd*/
#define  WL_RX_REPORT           "3 total:%u bad:%u" /*wl_rx_report*/
#define  WL_RX_PACKET           "4 ucast:%u mcast:%u" /*wl_rx_packet*/


#endif /* _wlu_wifi_at_ */

