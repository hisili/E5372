/*************************************************************************
*   版权所有(C) 2008-2012, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_TRACE.h
*
*   作    者 :  z67193
*
*   描    述 :  BSP信息记录定义头文件
*
*   修改记录 :  2012年04月14日  v1.00  z67193  创建
*************************************************************************/

#ifndef	__BSP_TRACE_H__
#define __BSP_TRACE_H__

#ifdef __cplusplus
extern "C" 
{
#endif /* __cplusplus */


void BSP_traceProcessInit(); 

extern unsigned int g_u32taskTraceBaseAddr;
extern unsigned int g_u32intTraceBaseAddr;

#ifdef PRODUCT_CFG_VERSION_DEBUG
#define PROCESS_TRACE
#endif

/* v7r1 is not used, deleted by z67193 20120421 */
#if 0
#ifdef  PROCESS_TRACE
#define PROCESS_ENTER(pADDRESS) \
    do { \
        *((unsigned int *)pADDRESS) = 1; \
    } while (0)

#define PROCESS_EXIT(pADDRESS) \
    do { \
        *((unsigned int *)pADDRESS) = 0; \
    } while (0)

#else
#define PROCESS_ENTER(pADDRESS)  do { } while (0)
#define PROCESS_EXIT(pADDRESS)   do { } while (0)   
#endif
#endif

/* 任务对应记录地址定义*/
#define BSP_IPCOM_SYSL_TRACE   (g_u32taskTraceBaseAddr + 0x0 ) /*ipcom_sysl*/   
#define BSP_IPCOM_TICK_TRACE   (g_u32taskTraceBaseAddr + 0x04) /*ipcom_tick*/  
#define BSP_IPCOM_TELN_TRACE   (g_u32taskTraceBaseAddr + 0x08) /*ipcom_teln*/  
#define BSP_SDIO_MONIT_TRACE   (g_u32taskTraceBaseAddr + 0x0c) /*tSdioMonit*/  
#define BSP_SDIO_INT_TRACE     (g_u32taskTraceBaseAddr + 0x10) /*tSdioIntrP*/  
#define BSP_PMU_INT_TRACE      (g_u32taskTraceBaseAddr + 0x14) /*tPmuIntTas*/  
#define BSP_SCISYSNC_TRACE     (g_u32taskTraceBaseAddr + 0x18) /*tSCISYSNC */ 
#define PS_VOS_TIMER_TRACE     (g_u32taskTraceBaseAddr + 0x1c) /*VOS_TIMER */ 
#define PS_RTC_TIMER_TRACE     (g_u32taskTraceBaseAddr + 0x20) /*RTC_TIMER */ 
//#define PS_L2_UL_TRACE         (g_u32taskTraceBaseAddr + 0x24) /*MACRLCUL_F*/ 
#define PS_HPA_TRACE           (g_u32taskTraceBaseAddr + 0x28) /*HPA_FID   */ 
#define BSP_NET0_TRACE         (g_u32taskTraceBaseAddr + 0x2c) /*tNet0     */ 
#define BSP_USB_CONTROL_TRACE  (g_u32taskTraceBaseAddr + 0x30) /*uw_Control*/ 
#define BSP_USB_CORE_TRACE     (g_u32taskTraceBaseAddr + 0x34) /*uw_Core   */ 
#define MSP_TID005_TRACE       (g_u32taskTraceBaseAddr + 0x38) /*tid005    */ 
//#define PS_L2_DL_TRACE         (g_u32taskTraceBaseAddr + 0x3c) /*MACRLCDL_F*/ 
#define BSP_USB_CLASS_TRACE    (g_u32taskTraceBaseAddr + 0x40) /*uw_Class  */ 
#define MSP_SYM_TRACE          (g_u32taskTraceBaseAddr + 0x44) /*mspSYM    */ 
#define MSP_MUX_TRACE          (g_u32taskTraceBaseAddr + 0x48) /*mspMUX    */ 
#define MSP_TID006_TRACE       (g_u32taskTraceBaseAddr + 0x4c) /*tid006    */ 
#define MSP_DIAG_TRACE         (g_u32taskTraceBaseAddr + 0x50) /*mspDIAG   */ 
#define MSP_SIMM_TRACE         (g_u32taskTraceBaseAddr + 0x54) /*mspSIMM   */ 
#define MSP_TID013_TRACE       (g_u32taskTraceBaseAddr + 0x58) /*tid013    */ 
#define MSP_FTM_TRACE          (g_u32taskTraceBaseAddr + 0x5c) /*mspFTM    */ 
#define MSP_NVIM_TRACE         (g_u32taskTraceBaseAddr + 0x60) /*mspNVIM   */ 
#define MSP_RAI_TRACE          (g_u32taskTraceBaseAddr + 0x64) /*mspRAI    */ 
#define BSP_SCIRX_TRACE        (g_u32taskTraceBaseAddr + 0x68) /*tSCIRxTask*/ 
#define PS_RRC_TRACE           (g_u32taskTraceBaseAddr + 0x6c) /*RRC_FID   */ 
#define MSP_MUXOM_TRACE        (g_u32taskTraceBaseAddr + 0x70) /*PS_FID_COM*/ 
#define PS_NAS_MM_TRACE        (g_u32taskTraceBaseAddr + 0x74) /*MM_FID    */ 
#define PS_NAS_CM_TRACE        (g_u32taskTraceBaseAddr + 0x78) /*CM_FID    */ 
#define MSP_L4A_TRACE          (g_u32taskTraceBaseAddr + 0x7c) /*mspL4A    */ 
#define MSP_AT_TRACE           (g_u32taskTraceBaseAddr + 0x80) /*mspAT     */        
#define MSP_DRA_TRACE          (g_u32taskTraceBaseAddr + 0x84) /*mspDRA    */        
#define MSP_TID012_TRACE       (g_u32taskTraceBaseAddr + 0x88) /*tid012    */        
#define PS_OM_TRACE            (g_u32taskTraceBaseAddr + 0x8c) /*OM_FID    */        
#define BSP_WATCHDOG_TRACE     (g_u32taskTraceBaseAddr + 0x90) /*WatchDog  */        
#define BSP_IPCOM_EGD_TRACE    (g_u32taskTraceBaseAddr + 0x94) /*ipcom_egd */        
#define BSP_ARM_SLEEP0_TRACE   (g_u32taskTraceBaseAddr + 0x98) /*tIdletask*/ 
#define BSP_SDIO_SVC_TRACE     (g_u32taskTraceBaseAddr + 0x9c) /*taskSdioSvc*/ 
/*modify by lishangfeng for TaskTrace Begin */
#define PS_ERROR_TASK_TRACE    (g_u32taskTraceBaseAddr + 0xa0) /* 公共任务，异常处理时的记录 */
/*modify by lishangfeng for TaskTrace End */


/* 中断对应地址定义*/
#define BSP_WDT_TRACE              (g_u32intTraceBaseAddr + 0x0 ) /*WDT中断                      */
#define BSP_SWINT_TRACE            (g_u32intTraceBaseAddr + 0x04) /*SoftWare INT                 */
#define BSP_COMMRX_TRACE           (g_u32intTraceBaseAddr + 0x08) /*COMMRX中断                   */
#define BSP_COMMTX_TRACE           (g_u32intTraceBaseAddr + 0x0c) /*COMMTX中断                   */
#define BSP_TIMER0_TRACE           (g_u32intTraceBaseAddr + 0x10) /*Timer0中断                   */
#define BSP_TIMER123_TRACE         (g_u32intTraceBaseAddr + 0x14) /*Timer123中断                 */
#define BSP_SCI_TRACE              (g_u32intTraceBaseAddr + 0x18) /*SCI 中断                     */
#define BSP_DMAC_TRACE             (g_u32intTraceBaseAddr + 0x1c) /*DMAC                         */
#define BSP_CIPHER_TRACE           (g_u32intTraceBaseAddr + 0x20) /*CIPHER                       */
#define BSP_USBOTG_TRACE           (g_u32intTraceBaseAddr + 0x24) /*USB OTG                      */
#define BSP_SSP012_TRACE           (g_u32intTraceBaseAddr + 0x28) /*SSP0 |SSP1|SSP2              */
#define BSP_UART01_TRACE           (g_u32intTraceBaseAddr + 0x2c) /*UART0| UART1                 */
#define BSP_RTC_TRACE              (g_u32intTraceBaseAddr + 0x30) /*RTC                          */
#define BSP_NANDC_TRACE            (g_u32intTraceBaseAddr + 0x34) /*NANDC                        */
#define BSP_SDMMC_TRACE            (g_u32intTraceBaseAddr + 0x38) /*SD/MMC 组合                  */
#define BSP_I2C_TRACE              (g_u32intTraceBaseAddr + 0x3c) /*I2C                          */
#define BSP_FE_TRACE               (g_u32intTraceBaseAddr + 0x40) /*FE                           */
#define BSP_GPIO3_TRACE            (g_u32intTraceBaseAddr + 0x44) /*GPIO3组合中断                */
#define BSP_GPIO012_TRACE          (g_u32intTraceBaseAddr + 0x48) /*GPIO0/1/2组合中断            */
#define BSP_RSA_DES_SHA_TRACE      (g_u32intTraceBaseAddr + 0x4c) /*rsa_int | des_int | sha_int  */
#define BSP_IPCM2ARM_TRACE         (g_u32intTraceBaseAddr + 0x50) /*IPCM2ARM中断                 */
#define BSP_PLUSEM0_TRACE          (g_u32intTraceBaseAddr + 0x54) /*PIU_SEM_0                    */
#define BSP_PLUSEM1_TRACE          (g_u32intTraceBaseAddr + 0x58) /*PIU_SEM_1                    */
#define BSP_PLUSEM2_TRACE          (g_u32intTraceBaseAddr + 0x5c) /*PIU_SEM_2                    */
#define BSP_PLUCR_TRACE            (g_u32intTraceBaseAddr + 0x60) /*PIU_CR                       */
#define BSP_PMU01_TRACE            (g_u32intTraceBaseAddr + 0x64) /*pmu0_irq_int_n|pmu1_irq_int_n*/
#define BSP_RESERVED_TRACE         (g_u32intTraceBaseAddr + 0x68) /*保留                         */
#define BSP_BBP_TIMESWITCH_TRACE   (g_u32intTraceBaseAddr + 0x6c) /*BBP时钟切换                  */
#define BSP_BBP_WAKEUP_TRACE       (g_u32intTraceBaseAddr + 0x70) /*BBP 唤醒                     */
#define BSP_BBP_TRACE              (g_u32intTraceBaseAddr + 0x74) /*BBP                          */
#define BSP_BBP_SUBFRAME_TRACE     (g_u32intTraceBaseAddr + 0x78) /*BBP定位中断                  */
#define BSP_BBP_BACKUP_TRACE       (g_u32intTraceBaseAddr + 0x7c) /*BBP 备份合并中断             */




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif    /* End #define __BSP_TRACE_H__ */

