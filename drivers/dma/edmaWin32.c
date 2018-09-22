#if 0
#include "stdio.h"
#include "stdlib.h"

#include "edmacWin32.h"
#include "edmacPri.h"
#include "edmacDrv.h"
#include "edmacConfig.h"

void *OSAL_CacheDmaMalloc( UINT32 ulSize )
{
    return malloc( ulSize );
}

void OSAL_IntEnable( UINT32 ulID )
{
}

void OSAL_IntConnect( UINT32 ulID, void *isr, int iParam )
{
}
#endif 
