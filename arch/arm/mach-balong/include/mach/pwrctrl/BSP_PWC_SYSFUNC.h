/******************************************************************
 * Copyright (C), 2005-2011, HUAWEI Tech. Co., Ltd.
 *
 * File name: pwcSysFunc.h
 *
 * Description:
 *     power ctrl common head file.
 *
 * Author:     l56193
 * Date:       2011.05.01
 * Version:    v1.0
 *
 *
 * History:
 * Author:     l56193
 * Date:       2011.05.01
 * Discribe:   Initial
 *******************************************************************/
#ifndef PWRCTRL_ACPU_SYS_FUNC_H
#define PWRCTRL_ACPU_SYS_FUNC_H

/*****************************************************************************/
/*include headfile */
/*****************************************************************************/

/*****************************************************************************/
/*macro define*/
/*****************************************************************************/
#ifndef NULL 
#define NULL    ( void *)(0)
#endif
#ifndef OK 
#define OK      (0)
#endif
#ifndef ERROR 
#define ERROR   (-1)
#endif
#ifndef TRUE 
#define TRUE    (1)
#endif
#ifndef FALSE 
#define FALSE   (0)
#endif
#ifndef NO_WAIT 
#define  NO_WAIT        (0)
#define  WAIT_FOREVER   (-1)
#endif
#ifndef WAIT_FOREVER 
#define  WAIT_FOREVER   (-1)
#endif
#ifndef SEM_Q_PRIORITY 
#define SEM_Q_PRIORITY  (0)
#endif

/*****************************************************************************/
/*type define*/
/*****************************************************************************/
typedef void* OSAL_SEM_ID;     // wangwei 09-10
typedef signed int  BOOL;

#ifndef __INCvxWindh
typedef int*        SEM_ID;   
typedef void* MSG_Q_ID;

#endif

typedef enum                /* CACHE_TYPE */
{
    OSAL_INSTRUCTION_CACHE,
    OSAL_DATA_CACHE
} OSAL_CACHE_TYPE;


/*****************************************************************************/
/*variable prototype*/
/*****************************************************************************/



/*****************************************************************************/
/*function prototype*/
/*****************************************************************************/

//extern STATUS (*intConnect) (VOIDFUNCPTR * vector, VOIDFUNCPTR routine, int parameter);
//extern int (*intAck) (int unit);    /* interrupt ack */



//extern OSAL_SEM_ID semBCreate(int options,SEM_B_STATE initialState);   // wangwei 09-10

#endif

