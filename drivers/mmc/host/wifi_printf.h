#ifndef _OM_PRINTF_H_
#define _OM_PRINTF_H_
#ifdef __cplusplus
extern "C"{
#endif

#include "../../mntn/include/omDrv.h"
#include "BSP.h"

/*extern global variable */
extern int        balong_sdio_trace_level;
extern MPS_PROC_S g_stMspRegFunc;
extern int        wifi_print_flag;

/*function declare */
unsigned long wifi_printfWithModule(unsigned long ulModuleId,unsigned long ulLevel,char * pcformat,...);
int wifi_printf(void);


#define OM_PrintWithModuleIsReady      (g_stMspRegFunc.OM_PrintfWithModule)
#define OM_PrintGetIdLevIsReady        (g_stMspRegFunc.OM_PrintfGetModuleIdLev)
#define OM_PrintIsReady                (g_stMspRegFunc.OM_Printf)
#define OM_PRINT_WITH_MODULE           (*(g_stMspRegFunc.OM_PrintfWithModule))
#define OM_PRINT                       (*(g_stMspRegFunc.OM_Printf))
#define OM_PRINT_GET_MODULE_IDLEV      (*(g_stMspRegFunc.OM_PrintfGetModuleIdLev))

/*module ID*/
#define	SDIO_ID_DEBUG	   0x8125  /*33061*/

/*trace level*/
typedef enum
{
	WIFI_DEBUG_ERROR = 1,
	WIFI_DEBUG_DEBUG,
	WIFI_DEBUG_WARNING,
	WIFI_DEBUG_INFO
}wifi_debug_level;

#define DEBUG_FUNC(fmt,args...)                                          \
                     ((unlikely(in_atomic()))? \
                      (unlikely(wifi_print_flag)?printk("[WiFi]"fmt, ## args):wifi_printf()): \
                      (int)OM_PRINT(moduleId,level,fmt, ## args))
                    
#define DEBUG_FUNC_WIFI_MODULE(moduleId,level,fmt,args...)  \
                 ((unlikely(in_atomic()))? \
                    (unlikely(wifi_print_flag)?printk("[WiFi]"fmt, ## args):wifi_printf()): \
                    (int)OM_PRINT_WITH_MODULE(moduleId,level,fmt, ## args))



#ifdef __cplusplus
}
#endif

#endif
