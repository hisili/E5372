/*
#ifndef WIN32   
#include "osal.h"
#else
#include "edmacWin32.h"
#endif

#include "edmacPri.h"
#include "edmacDrv.h"
#include "edmaLLIMgr.h"
#include "edmacIP.h"
#include "edmacConfig.h"
*/
#include "mach/edmacWin32.h"
#include "mach/edmacPri.h"
/*
    Config it later.
*/
const UINT32 edmac_priority_sets = 0;

void edmac_product_config( void )
{
    edmac_priority_set( edmac_priority_sets );
}

